# Toybox: The First Game & Animation Engine for C/C++ Beginners

Please refer to the comments in toybox.h and the example in hello.cpp.

Both C and C++ programs can start from the following template. You only need to implement the "TODO" section for screen updates and key press handling (handling key presses via keypress is optional).

```c
#include "toybox.h"

// toybox_run(fps, update, keypress)
// - Enters the game/animation main loop
// - Calls update(w, h, draw) fps times per second
// - Calls keypress(key) whenever a key is pressed

void update(int w, int h, draw_function draw) {
    // The current screen size is w x h (initially empty)
    // Use draw(x, y, ch) to draw character 'ch' at column x, row y

    // TODO
}

void keypress(int key) {
    // Detect a key press, for example, W, A, S, D

    // TODO
}

int main() {
    toybox_run(20, update, keypress);
}
```

```c++
#include "toybox.h"

int main() {
    toybox_run(1, [](int w, int h, auto draw) {
        static int t = 0;
        t++;
        for (int x = 0; x < w; x++) {
            for (int y = 0; y < h; y++) {
                draw(x, y, '0' + t % 10);
            }
        }
    }, nullptr);
}
```

