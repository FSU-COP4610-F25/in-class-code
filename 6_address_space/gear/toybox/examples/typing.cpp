// Author: GPT-4-turbo

#include "../toybox.h"
#include <stdlib.h>
#include <time.h>

#define MAX_LETTERS 26

typedef struct {
    char ch; // Falling letter
    int x, y; // Letter position
    bool active; // Whether the letter is active
} Letter;

Letter letters[MAX_LETTERS]; // Store letters on the screen

void init_letters() {
    for (int i = 0; i < MAX_LETTERS; ++i) {
        letters[i].active = false;
    }
}

void spawn_letter(int w) {
    for (int i = 0; i < MAX_LETTERS; ++i) {
        if (!letters[i].active) {
            letters[i].ch = 'a' + rand() % 26; // Randomly select a letter
            letters[i].x = rand() % w; // Randomly select a horizontal position
            letters[i].y = 0; // Start falling from the top
            letters[i].active = true;
            break;
        }
    }
}

void update(int w, int h, draw_function draw) {
    static int tick = 0;
    if (tick++ % 10 == 0) { // Generate a new letter at intervals
        spawn_letter(w);
    }

    for (int i = 0; i < MAX_LETTERS; ++i) {
        if (letters[i].active) {
            draw(letters[i].x, letters[i].y, letters[i].ch); // Draw the letter
            letters[i].y += 1; // Move the letter down
            if (letters[i].y >= h) { // If the letter reaches the bottom, reset it
                letters[i].active = false;
            }
        }
    }
}

void keypress(int key) {
    for (int i = 0; i < MAX_LETTERS; ++i) {
        if (letters[i].active && letters[i].ch == key) {
            letters[i].active = false; // Remove the letter if the key matches
            break;
        }
    }
}

int main() {
    srand(time(NULL)); // Initialize random number generator
    init_letters(); // Initialize letter array
    toybox_run(10, update, keypress); // Run the game
}