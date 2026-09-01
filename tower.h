#ifndef TOWER_H
#define TOWER_H

#include <string>
#include <windows.h>
#include <conio.h>

using namespace std;

// ---------- 工具函数 ----------
void clearScreen();
void setColor(int color);
void hideCursor();
void gotoxy(int x, int y);
void drawBox(int x, int y);
void drawVLine(int x, int y1, int y2);
void drawHLine(int x1, int x2, int y);

// ---------- 渲染函数 ----------
void renderMainMap(int playerX, int playerY);
void renderTowerMap(int playerX, int playerY);

#endif // TOWER_H