// Author: GPT-4-turbo

#include "../toybox.h"
#include <stdlib.h> // For rand() and srand()
#include <time.h>   // For time()

#define MAX_LETTERS 50 // Maximum number of letters on the screen

typedef struct {
    int x, y;       // Current position of the letter
    int dx, dy;     // Movement direction of the letter
    char ch;        // Character to be displayed
} Letter;

Letter letters[MAX_LETTERS]; // Array to store all letters

void init_letters(int w, int h) {
    int i;

    srand(time(NULL)); // Initialize random number generator
    for (i = 0; i < MAX_LETTERS; i++) {
        letters[i].x = rand() % w;
        letters[i].y = rand() % h;
        letters[i].dx = (rand() % 3) - 1; // -1, 0, or 1
        letters[i].dy = (rand() % 3) - 1; // -1, 0, or 1
        letters[i].ch = 'A' + (rand() % 26); // Random letter
    }
}

void update_letters(int w, int h) {
    int i;

    for (i = 0; i < MAX_LETTERS; i++) {
        letters[i].x += letters[i].dx;
        letters[i].y += letters[i].dy;

        // Check boundaries and reverse direction if needed
        if (letters[i].x < 0 || letters[i].x >= w) {
            letters[i].dx = -letters[i].dx;
            letters[i].x += letters[i].dx * 2; // Prevent getting stuck at the boundary
        }
        if (letters[i].y < 0 || letters[i].y >= h) {
            letters[i].dy = -letters[i].dy;
            letters[i].y += letters[i].dy * 2; // Prevent getting stuck at the boundary
        }
    }
}

void update(int w, int h, draw_function draw) {
    static int initialized = 0;
    int i;

    if (!initialized) {
        init_letters(w, h);
        initialized = 1;
    }

    update_letters(w, h);

    for (i = 0; i < MAX_LETTERS; i++) {
        draw(letters[i].x, letters[i].y, letters[i].ch);
    }
}

void keypress(int key) {
    // Key press handling (if needed)
}

int main(void) {
    toybox_run(20, update, keypress);
}
