#include <ctime>
#include <cstdlib>
#include <utility>
#include <algorithm>
#pragma once
class Player{
    int myArea[10][10],  // the position of my ships
        shootArea[10][10], // -1 - miss, 1 - hit  // my shooting enemy ships
        woundArea[10][10]; // -1 - miss, 1 - hit to ship // shooting the enemy on my ships
    int X, Y, prevX, prevY;
    bool lastMove;
    int ships;
    bool Freedom(const int, const int, int [10][10]);
public:
    bool checkCell(int, int);
    int getShootCell(int, int);
    int getWoundCell(int, int);
    std::pair<int, int> shoot();
    int checkHit(int, int);
    void writeShoot(int);
    void genNew();
    bool gameOver();
    // bool state1(int, int);
    Player();
    ~Player();
};
