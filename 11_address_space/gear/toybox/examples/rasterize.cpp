// Author: GPT-4-turbo

#include "../toybox.h"
#include <cmath> // For std::abs and std::round

struct Point {
    int x, y;
    Point(int x, int y): x(x), y(y) {}
};

Point p0(0, 0), p1(0, 0), p2(0, 0);

void drawLine(Point p0, Point p1, draw_function draw) {
    int dx = std::abs(p1.x - p0.x), sx = p0.x < p1.x ? 1 : -1;
    int dy = -std::abs(p1.y - p0.y), sy = p0.y < p1.y ? 1 : -1; 
    int err = dx + dy, e2; /* error value e_xy */
    
    while (true) {
        draw(p0.x, p0.y, '*'); // 使用 '*' 绘制线段
        if (p0.x == p1.x && p0.y == p1.y) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; p0.x += sx; }
        if (e2 <= dx) { err += dx; p0.y += sy; }
    }
}

void update(int w, int h, draw_function draw) {
    drawLine(p0, p1, draw);
    drawLine(p1, p2, draw);
    drawLine(p0, p2, draw);
}

void keypress(int key) {
    switch (key) {
        case 'w': p0.y -= 1; break;
        case 's': p0.y += 1; break;
        case 'a': p0.x -= 1; break;
        case 'd': p0.x += 1; break;

        case 'W': p1.y -= 1; break;
        case 'S': p1.y += 1; break;
        case 'A': p1.x -= 1; break;
        case 'D': p1.x += 1; break;
    }
}

int main() {
    toybox_run(20, update, keypress);
}
