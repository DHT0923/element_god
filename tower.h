#pragma once

#include "data.h"

// 坐标式控制台 UI：布局与参考 tower.cpp/tower.h 保持一致。
void setColor(int color);
void hideCursor();
void gotoxy(int x, int y);
void drawBox(int x, int y);
void drawVLine(int x, int y1, int y2);
void drawHLine(int x1, int x2, int y);
void renderMainMap(int playerX, int playerY, const Player& player);

// 进入并探索指定高塔；返回 true 表示击败首领并完成该塔。
bool exploreTower(Player& player, TowerDirection direction);
