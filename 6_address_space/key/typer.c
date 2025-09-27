#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/time.h>

#define CLEAR_SCREEN() printf("\033[2J\033[H")

/*
 *  This program displays a terminal-based "Space Clicker" interface.
 *  It counts how many times the spacebar is pressed per second and
 *  the total count of presses. Press 'q' to exit.
 *
 *  Compile:
 *      gcc -o space_clicker space_clicker.c
 *
 *  Run:
 *      ./space_clicker
 */

int main(void) {
    // Variables for original and modified terminal settings
    struct termios oldt, newt;
    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Disable canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    // Apply new terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Set stdin to non-blocking mode
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    // Counters
    unsigned long totalCount = 0;    // Total number of presses
    unsigned long secondCount = 0;   // Number of presses in the last second

    // Time variables for measuring intervals
    struct timeval tv_last, tv_current;
    gettimeofday(&tv_last, NULL);

    while (1) {
        // Get current time
        gettimeofday(&tv_current, NULL);

        // Calculate time elapsed since last update, in seconds
        double elapsed = (double)(tv_current.tv_sec - tv_last.tv_sec)
                       + (double)(tv_current.tv_usec - tv_last.tv_usec) / 1000000.0;

        // Every 1 second, refresh the display
        if (elapsed >= 1.0) {
            // Clear the screen
            CLEAR_SCREEN();

            // Print a simple ASCII banner
            printf("********************************************\n");
            printf("*   ████████╗██╗   ██╗██████╗ ███████╗██████╗ *\n");
            printf("*   ╚══██╔══╝██║   ██║██╔══██╗██╔════╝██╔══██╗*\n");
            printf("*      ██║   ██║   ██║██████╔╝█████╗  ██████╔╝*\n");
            printf("*      ██║   ██║   ██║██╔═══╝ ██╔══╝  ██╔══██╗*\n");
            printf("*      ██║   ╚██████╔╝██║     ███████╗██║  ██║*\n");
            printf("*      ╚═╝    ╚═════╝ ╚═╝     ╚══════╝╚═╝  ╚═╝*\n");
            printf("********************************************\n\n");
            

            printf("  Last second presses: %lu\n", secondCount);
            printf("  Total presses:       %lu\n\n", totalCount);
            printf("  Press 'q' to quit.\n");

            // Reset secondCount and time checkpoint
            secondCount = 0;
            gettimeofday(&tv_last, NULL);
        }

        // Read user input (non-blocking)
        int ch = getchar();
        if (ch != EOF) {
            // If 'q' is pressed, exit the loop
            if (ch == 'q' || ch == 'Q') {
                break;
            } else if (ch == ' ') {
                // If spacebar is pressed, increment counters
                secondCount++;
                totalCount++;
            }
        }

        // Sleep briefly to avoid high CPU usage
        usleep(5000); // 5 ms
    }

    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    printf("\nExiting program...\n");

    return 0;
}
