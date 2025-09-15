/*
sudo apt update
sudo apt install g++
g++ -o hello hello.cpp./

*/


#include "toybox.h"

int k = '?', t = 0;

void update(int w, int h, draw_function draw) {
    for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++)
            draw(x, y, k);
    draw(0, 0, "-\\|/"[(t++) / 5 % 4]);
}

void keypress(int ch) {
    k = ch;
}

int main() {
    toybox_run(30, update, keypress);
}
