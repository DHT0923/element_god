#ifndef TOWER_H
#define TOWER_H

#include "data.h"    // 引入 Player 和 Pet
#include "battle.h"  // 引入 BattleResult 和 battle() 函数

#include <string>
#include <vector>
#include <windows.h>
#include <conio.h>

using namespace std;

// ---------- 全局高塔状态变量（供其他模块只读） ----------
extern int northClear; //0未通关 1通关
extern int westClear;
extern int southClear;
extern int eastUnlock;
extern int inTower;    //0不在高塔；1北 2西 3南 4东
extern int curRoomIdx; //高塔当前房间下标

// ---------- 房间常量 ----------
const string ROOM_BATTLE = "battle";
const string ROOM_HEAL = "heal";
const string ROOM_GOLD = "gold";
const string ROOM_BOSS = "boss";

// ---------- 对外接口 ----------
bool canEnterTower(int towerId);
void runTower(int towerId, Player& player);

// ---------- 地图渲染工具（你在之前要求保留的函数） ----------
void clearScreen();
void setColor(int color);
void hideCursor();
void gotoxy(int x, int y);
void drawBox(int x, int y);
void drawVLine(int x, int y1, int y2);
void drawHLine(int x1, int x2, int y);
void renderMainMap(int playerX, int playerY);
void renderTowerMap(int playerX, int playerY);

#endif // TOWER_H