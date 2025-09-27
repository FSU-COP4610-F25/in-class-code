#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdint>
#include <sys/time.h>
#include <chrono>
#include <thread>

// Get the current time in milliseconds
uint64_t get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
}

// Scan memory for possible timer addresses
uintptr_t find_timer_address(int pid, uint64_t expected_value) {
    std::string memfile = "/proc/" + std::to_string(pid) + "/mem";
    std::string mapsfile = "/proc/" + std::to_string(pid) + "/maps";

    std::ifstream maps(mapsfile);
    if (!maps.is_open()) {
        std::cerr << "Cannot open " << mapsfile << std::endl;
        return 0;
    }

    int fd = open(memfile.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 0;
    }

    std::string line;
    while (std::getline(maps, line)) {
        uintptr_t start, end;
        char perms[5];

        if (sscanf(line.c_str(), "%lx-%lx %4s", &start, &end, perms) != 3)
            continue;

        // Scan only readable and writable memory regions
        if (perms[0] != 'r') continue;

        std::cout << "[INFO] Scanning: " << std::hex << start << "-" << end << std::dec << "..." << std::endl;

        size_t size = end - start;
        std::vector<uint8_t> buffer(size);
        lseek(fd, start, SEEK_SET);
        read(fd, buffer.data(), size);

        // Scan for timestamp (allow a wider range)
        for (size_t i = 0; i < size - sizeof(uint64_t); i++) {
            uint64_t *val = reinterpret_cast<uint64_t *>(&buffer[i]);
            if (*val >= expected_value - 5000 && *val <= expected_value + 5000) {
                uintptr_t addr = start + i;
                std::cout << "[MATCH] Possible timer at address: 0x" << std::hex << addr 
                          << std::dec << " (Value: " << *val << ")\n";
                close(fd);
                return addr; // Return first match
            }
        }
    }

    close(fd);
    return 0;
}

// Modify the timer value dynamically
void modify_timer(int pid, uintptr_t address, double speed_factor) {
    std::string memfile = "/proc/" + std::to_string(pid) + "/mem";

    int fd = open(memfile.c_str(), O_RDWR);
    if (fd < 0) {
        perror("open");
        return;
    }

    uint64_t start_time, new_time;
    pread(fd, &start_time, sizeof(start_time), address); // Read original time
    
    std::cout << "[INFO] Speed hack started (Factor: " << speed_factor << "x)\n";

    while (true) {
        uint64_t real_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::system_clock::now().time_since_epoch()
                             ).count();
        new_time = start_time + ((real_time - start_time) * speed_factor);

        // Write the modified time back
        pwrite(fd, &new_time, sizeof(new_time), address);

        // Sleep briefly to avoid excessive CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <pid> <speed_factor>\n";
        return 1;
    }

    int pid = std::stoi(argv[1]);
    double speed_factor = std::stod(argv[2]);
    uint64_t current_time = get_time_ms();

    std::cout << "[INFO] Scanning for timer near: " << current_time << "...\n";
    uintptr_t timer_address = find_timer_address(pid, current_time);

    if (timer_address == 0) {
        std::cerr << "[ERROR] Timer address not found!\n";
        return 1;
    }

    modify_timer(pid, timer_address, speed_factor);

    return 0;
}

