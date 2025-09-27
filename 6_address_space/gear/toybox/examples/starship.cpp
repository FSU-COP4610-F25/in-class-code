// Author: Claude-3-Opus

#include "../toybox.h"
#include <vector>
using namespace std;

const int W = 60;
const int H = 20;

int shipX, shipY;  // Ship coordinates
vector<pair<int,int>> bullets; // Bullet coordinates
vector<pair<int,int>> targets; // Target coordinates

void update(int w, int h, draw_function draw) {
    // Clear screen
    for(int i = 0; i < W; i++) 
        for(int j = 0; j < H; j++)
            draw(i, j, ' ');
    
    // Draw the ship
    draw(shipX, shipY, 'A'); 
    
    // Draw bullets and update bullet positions
    for(auto &b : bullets) {
        draw(b.first, b.second, '#');
        b.first++;
    }
    
    // Draw targets
    for(auto &t : targets) {
        draw(t.first, t.second, 'X');
    }
    
    // Collision detection: remove targets hit by bullets
    for(auto it = targets.begin(); it != targets.end();) {
        auto t = *it;
        bool hit = false;
        for(auto &b : bullets) {
            if(t == b) {
                hit = true;
                break;
            }
        }
        if(hit)
            it = targets.erase(it);
        else
            ++it;
    }
    
    // Remove bullets that go off-screen
    for(auto it = bullets.begin(); it != bullets.end();) {
        if(it->first >= W) 
            it = bullets.erase(it);
        else
            ++it;
    }
}

void keypress(int key) {
    switch(key) {
        case 'a':
            if(shipX > 0) shipX--;
            break;
        case 'd':
            if(shipX < W-1) shipX++;
            break;
        case 'w':
            if(shipY > 0) shipY--;
            break;
        case 's':
            if(shipY < H-1) shipY++;
            break;
        case ' ': 
            bullets.push_back({shipX+1, shipY}); // Fire bullet
            break;
    }
}

int main() {
    shipX = shipY = 0;
    for(int i = 0; i < 5; i++)
        targets.push_back({W-1, rand()%H});
    toybox_run(20, update, keypress);
}
