#include <ctime>
#include <cstdlib>
#include <utility>
#include <algorithm>
#pragma once
class Player{
    int myArea[10][10],  // ������������ ���� ��������
        shootArea[10][10], // -1 - miss, 1 - hit  // ��� �������� �� �������� ����������
        woundArea[10][10]; // -1 - miss, 1 - hit to ship // �������� ���������� �� ���� ��������
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
