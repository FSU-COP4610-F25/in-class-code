/*
 *  _____         _           
 * |_   _|___ _ _| |_ ___ _ _ 
 *   | | | . | | | . | . |_'_|
 *   |_| |___|_  |___|___|_,_|
 *           |___|            
 *
 * The first game & animation engine for C/C++ beginners
 *
 * MIT License
 * 
 * Copyright (c) 2024 by Yanyan Jiang and Zesen Liu
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * toybox provides only one function: void toybox_run(fps, update, keypress)
 * toybox_run takes three arguments and then enters an infinite loop:
 *
 * - 1. Integer fps:
 *       The number of refreshes per second (calls update function fps times per second)
 *
 * - 2. Function update:
 *       void update(int w, int h, draw_function draw);
 *       update is called whenever the time interval is reached. Within update, 
 *       draw(x, y, ch) can be used to render a character ch at the coordinates (x, y).
 *       The coordinate system:
 *
 *            (0,0) ---- x ---->
 *            |          |
 *            |          |
 *            |          |
 *            y ------ (x,y) = ch   //  draw(x, y, ch)
 *            |
 *            v
 *
 * - 3. Function keypress:
 *       void keypress(int key);
 *       keypress is called whenever a key is pressed. The key parameter represents
 *       the ASCII code of the pressed key.
 */


/* -= Toybox API =------------------------------------- */
typedef void (*draw_function)(int x, int y, char ch);

static void
toybox_run(int fps,
    void (*update)(int, int, draw_function draw),
    void (*keypress)(int));
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_W_ 128
#define MAX_H_ 64

#define append_(buf, str) \
    do { \
        strcpy(buf, str); \
        buf += strlen(str); \
    } while (0)

static uint64_t start_time_;
static int w_, h_;
static char canvas_[MAX_W_ * MAX_H_];
static int waitkey_(void);
static void get_window_size_(int *w, int *h);

#ifdef _WIN32
#include <windows.h>
#include <conio.h>

static int waitkey_(void) {
    int startTime = GetTickCount();
    while (GetTickCount() - startTime < 10) {
        if (_kbhit()) {
            return _getch();
        }
    }
    return -1;
}

static void get_window_size_(int *w, int *h) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        *w = csbi.srWindow.Right - csbi.srWindow.Left;
        *h = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        *w = 80;
        *h = 25;
    }
}

// copied from https://github.com/confluentinc/librdkafka
static int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}

static void clear_screen_() {
    COORD topLeft  = { 0, 0 };
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(console, &screen);
    FillConsoleOutputCharacterA(
            console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    FillConsoleOutputAttribute(
            console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
            screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    SetConsoleCursorPosition(console, topLeft);
}

#else
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>

static int waitkey_(void) {
    struct timeval timeout;
    fd_set readfds;
    int retval;

    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    retval = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);
    if (retval == -1) {
        exit(1);
    } if (retval) {
        char ch;
        read(STDIN_FILENO, &ch, 1);
        return ch;
    } else {
        return -1;
    }
}

struct termios old_;

static void __attribute__((constructor))
termios_init_(void) {
    struct winsize win;
    struct termios cur;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &win) == -1) {
        printf("Not a terminal window.\n");
        exit(1);
    }

    tcgetattr(STDIN_FILENO, &old_);

    cur = old_;
    cur.c_lflag &= ~(ICANON | ECHO);
    cur.c_cc[VMIN] = 0;
    cur.c_cc[VTIME] = 1;

    tcsetattr(STDIN_FILENO, TCSANOW, &cur);
}

static void __attribute__((destructor))
termios_restore_(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_);
}

static void get_window_size_(int *w, int *h) {
    struct winsize win;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

    *w = win.ws_col < MAX_W_ ? win.ws_col : MAX_W_;
    *h = win.ws_row < MAX_H_ ? win.ws_row : MAX_H_;
}

static void clear_screen_() {
    printf("\033[H");
}

#endif

uint64_t timer_ms_(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
}

static void __attribute__((constructor))
init_timer_(void) {
    start_time_ = timer_ms_();
}

void draw_(int x, int y, char ch) {
    if (0 <= x && x < w_ && 0 <= y && y < h_) {
        canvas_[y * w_ + x] = ch;
    }
}

static void
toybox_run(int fps,
    void (*update)(int, int, draw_function draw),
    void (*keypress)(int)) {
    uint64_t last_time = 0;
    int i, last_size = -1;
    char buffer[MAX_W_ * MAX_H_ + MAX_H_ * 2 + 4096], *head;

    while (1) {
        int key = waitkey_();
        if (key > 0) {
            if (keypress) {
                keypress(key);
            }
            continue;
        } else {
            uint64_t t = timer_ms_() - start_time_;
            if (t - last_time <= 1000 / fps) {
                continue;
            }
            last_time = t;
        }

        get_window_size_(&w_, &h_);
        memset(canvas_, ' ', sizeof(canvas_));
        update(w_, h_, draw_);

        head = buffer;
        clear_screen_();

        if ((w_ << 16) + h_ != last_size) {
            last_size = (w_ << 16) + h_;
            append_(head, "\033[2J");
        }

        for (i = 0; i < h_; i++) {
            if (i != 0) {
                append_(head, "\r\n");
            }
            strncpy(head, &canvas_[i * w_], w_);
            head += w_;
        }

        fwrite(buffer, head - buffer, 1, stdout);
        fflush(stdout);
    }
}
