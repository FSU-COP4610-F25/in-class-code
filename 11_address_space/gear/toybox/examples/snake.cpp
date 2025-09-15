// Author: GPT-4-turbo

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>
#include "../toybox.h"
using namespace std;

// Define snake movement directions
enum Direction { UP, DOWN, LEFT, RIGHT };

// Initial snake direction
Direction dir = RIGHT;

// Snake body, represented as a series of x, y coordinates
std::vector<std::pair<int, int>> snake = {{5, 5}, {5, 4}, {5, 3}};

// Food position
std::pair<int, int> food = {7, 7};

// Game over flag
bool gameOver = false;

// Generate food at a random position
void generateFood(int w, int h) {
    srand(time(0));
    food.first = rand() % w;
    food.second = rand() % h;
}

void update();

// Render the game state
void render(int w, int h, void(*draw)(int, int, char)) {
    update();
    if (gameOver) {
        return;
    }

    // Clear the screen
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            draw(x, y, ' ');
        }
    }

    // Draw the snake
    for (auto &part : snake) {
        draw(part.first, part.second, '*');
    }

    // Draw the food
    draw(food.first, food.second, '#');
}

// Handle key presses
void keypress(int key) {
    switch (key) {
        case 'w': dir = UP; break;
        case 's': dir = DOWN; break;
        case 'a': dir = LEFT; break;
        case 'd': dir = RIGHT; break;
    }
}

// Update the game state
void update() {
    if (gameOver) {
        return;
    }

    // Compute new position for the snake's head
    std::pair<int, int> head = snake.front();
    switch (dir) {
        case UP:    head.second--; break;
        case DOWN:  head.second++; break;
        case LEFT:  head.first--; break;
        case RIGHT: head.first++; break;
    }

    // Check if the snake hits the wall or itself
    if (head.first < 0 || head.second < 0 || head.first >= 80 || head.second >= 25 || std::find(snake.begin(), snake.end(), head) != snake.end()) {
        gameOver = true;
        return;
    }

    // Add new head to the snake
    snake.insert(snake.begin(), head);

    // Check if the snake eats the food
    if (head == food) {
        generateFood(80, 25); // Assuming screen size is 80x25
    } else {
        // Remove the tail segment
        snake.pop_back();
    }
}

// Main function
int main() {
    toybox_run(20, render, keypress); // Assume toybox_run takes a function to update the game state
}
