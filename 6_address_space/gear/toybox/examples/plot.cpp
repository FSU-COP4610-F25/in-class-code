// Author: GPT-4-turbo
// Prompt: Fill in the template (README.md) to implement a function plotter and navigation.
//         Plot f(x) = x, f(x) = sin(x), and f(x) = sqrt(x) within [-1,1] x [-1,1].
//         Use w/a/s/d keys to move the view; z/x to zoom in/out.
//         Different functions should use different marks; draw horizontal and vertical axes.

#include "../toybox.h"
#include <math.h>
#include <stdio.h>

// View state
double centerX = 0.0;  // View center X
double centerY = 0.0;  // View center Y
double scale = 10.0;   // Zoom level, each unit length represents pixel points

// Function prototypes
void drawAxes(int w, int h, void (*draw)(int, int, char));
void drawFunction(int w, int h, void (*draw)(int, int, char), double (*func)(double), char mark);
double identityFunction(double x);
double sinFunction(double x);
double squareRootFunction(double x);

void update(int w, int h, void (*draw)(int, int, char)) {
    // Clear screen
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            draw(x, y, ' ');
        }
    }

    // Draw axes
    drawAxes(w, h, draw);

    // Draw functions
    drawFunction(w, h, draw, identityFunction, '*');
    drawFunction(w, h, draw, sinFunction, '.');
    drawFunction(w, h, draw, squareRootFunction, 'o');
}

void keypress(int key) {
    switch (key) {
        case 's': centerY -= 1.0 / scale; break;
        case 'w': centerY += 1.0 / scale; break;
        case 'a': centerX -= 1.0 / scale; break;
        case 'd': centerX += 1.0 / scale; break;
        case 'z': scale *= 1.1; break;  // Zoom in
        case 'x': scale *= 0.9; break;  // Zoom out
    }
}

void drawAxes(int w, int h, void (*draw)(int, int, char)) {
    int originX = (int)(w / 2 + centerX * scale);
    int originY = (int)(h / 2 - centerY * scale);

    // Draw X-axis
    for (int x = 0; x < w; x++) {
        draw(x, originY, '-');
    }

    // Draw Y-axis
    for (int y = 0; y < h; y++) {
        draw(originX, y, '|');
    }

    // Draw origin
    draw(originX, originY, '+');
}

void drawFunction(int w, int h, void (*draw)(int, int, char), double (*func)(double), char mark) {
    for (int x = 0; x < w; x++) {
        double worldX = (x - w / 2) / scale - centerX;
        double worldY = func(worldX);
        int screenY = h / 2 - (int)(worldY * scale + centerY * scale);

        if (screenY >= 0 && screenY < h) {
            draw(x, screenY, mark);
        }
    }
}


double identityFunction(double x) {
    return x;
}

double sinFunction(double x) {
    return sin(x);
}

double squareRootFunction(double x) {
    return sqrt(x);
}

int main() {
    toybox_run(20, update, keypress);
    return 0;
}
