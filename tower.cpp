#include "tower.h"
#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

// ---------- 全局变量定义 ----------
int northClear = 0;
int westClear = 0;
int southClear = 0;
int eastUnlock = 0;
int inTower = 0;
int curRoomIdx = 0;

// ---------- 工具函数（原封不动） ----------
void clearScreen() { system("cls"); }
void setColor(int color) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color); }
void hideCursor() { CONSOLE_CURSOR_INFO cursor_info = { 1, 0 }; SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info); }
void gotoxy(int x, int y) { COORD coord; coord.X = x; coord.Y = y; SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord); }

void drawBox(int x, int y) {
    setColor(12);
    gotoxy(x, y); cout << "┌──────────┐";
    gotoxy(x, y + 1); cout << "│          │";
    gotoxy(x, y + 2); cout << "│          │";
    gotoxy(x, y + 3); cout << "└──────────┘";
    setColor(7);
}

void drawVLine(int x, int y1, int y2) {
    setColor(12);
    for (int i = y1; i <= y2; ++i) { gotoxy(x, i); cout << "│"; }
    setColor(7);
}

void drawHLine(int x1, int x2, int y) {
    setColor(12);
    for (int i = x1; i <= x2; ++i) { gotoxy(i, y); cout << "─"; }
    setColor(7);
}

// ================== 主城地图渲染 ==================
void renderMainMap(int playerX, int playerY) {
    clearScreen();
    hideCursor();
    setColor(14); gotoxy(0, 0); cout << "【主城地图】";

    drawBox(30, 3);  // 北塔
    drawBox(5, 12);  // 西塔
    drawBox(30, 12); // 城镇
    drawBox(55, 12); // 东塔
    drawBox(30, 21); // 南塔

    drawVLine(35, 7, 11);
    drawVLine(35, 16, 20);
    drawHLine(17, 29, 14);
    drawHLine(41, 54, 14);

    auto drawText = [&](int nodeX, int nodeY, int x, int y, const string& name) {
        gotoxy(x, y);
        if (playerX == nodeX && playerY == nodeY) { setColor(12); cout << "★" << name << "★"; }
        else { setColor(10); cout << " " << name << " "; }
        };

    drawText(2, 0, 32, 4, "北塔");
    drawText(0, 2, 7, 13, "西塔");
    drawText(2, 2, 32, 13, "城镇");
    drawText(4, 2, 57, 13, "东塔");
    drawText(2, 4, 32, 22, "南塔");

    setColor(13);
    gotoxy(0, 28);
    cout << "W↑ S↓ A← D→ 移动 | [1]进入 | [Q]退出" << endl;
    setColor(7);
}

// ================== 塔内地图渲染 ==================
void renderTowerMap(int playerX, int playerY) {
    clearScreen();
    hideCursor();

    setColor(14); gotoxy(0, 0); cout << "【塔内地图】";

    drawBox(30, 3);  // 奇遇房
    drawBox(30, 9);  // 小怪房
    drawBox(5, 9);   // 分支小怪房
    drawBox(30, 15); // 奇遇房
    drawBox(30, 21); // BOSS房

    drawVLine(35, 7, 8);
    drawHLine(17, 29, 11);
    drawVLine(35, 13, 14);
    drawVLine(35, 19, 20);

    auto drawText = [&](int nodeX, int nodeY, int x, int y, const string& name) {
        gotoxy(x, y);
        if (playerX == nodeX && playerY == nodeY) { setColor(12); cout << "★" << name << "★"; }
        else { setColor(10); cout << " " << name << " "; }
        };

    drawText(1, 0, 32, 4, "奇遇房");
    drawText(1, 1, 32, 10, "小怪房");
    drawText(0, 1, 7, 10, "小怪房");
    drawText(1, 2, 32, 16, "奇遇房");
    drawText(1, 3, 32, 22, "BOSS房");

    setColor(13);
    gotoxy(0, 28);
    cout << "W↑ S↓ A← D→ 移动 | [Q]撤离" << endl;
    setColor(7);
}

// ================== 塔内房间战斗逻辑 ==================
bool canEnterTower(int towerId) {
    if (towerId == 1) return true;             // 北塔默认解锁
    if (towerId == 2) return northClear == 1;  // 西塔需通关北塔
    if (towerId == 3) return westClear == 1;   // 南塔需通关西塔
    if (towerId == 4) return eastUnlock == 1;  // 东塔需特殊解锁
    return false;
}

// 辅助创建敌人宠物
Pet createEnemy(int towerId, bool isBoss) {
    if (isBoss) {
        return Pet("塔主", "火", 10 + towerId * 2); // Boss更高等级
    }
    else {
        return Pet("小怪", "草", 1 + towerId);      // 小怪等级随塔变高
    }
}

void runTower(int towerId, Player& player) {
    if (!canEnterTower(towerId)) return;

    inTower = towerId;
    curRoomIdx = 0;
    vector<string> roomList = { "battle", "gold", "heal", "battle", "boss" }; // 生成本塔房间序列

    while (curRoomIdx < roomList.size()) {
        clearScreen();
        // 打印塔内当前房间状态（沿用之前硬编码地图的展示，但只显示当前所在房间）
        setColor(14); gotoxy(0, 0); cout << "【塔内地牢】当前房间: " << curRoomIdx + 1 << "/" << roomList.size();

        // 展示地图框架
        renderTowerMap(curRoomIdx, 0); // 传个临时坐标，仅用于显示

        string roomType = roomList[curRoomIdx];
        cout << "\n\n当前事件: " << roomType << endl;

        if (roomType == ROOM_BATTLE || roomType == ROOM_BOSS) {
            // 构造敌人
            Pet enemy = createEnemy(towerId, roomType == ROOM_BOSS);
            Pet& our = player.team[0]; // 取第一只精灵出战

            // 【核心调用】调用 battle.h 中的战斗函数
            BattleResult result = battle(our, enemy, player);

            if (result == BATTLE_LOSE) {
                cout << "\n全队阵亡，被迫撤离回城镇！" << endl;
                inTower = 0;
                system("pause");
                return;
            }
            else if (result == BATTLE_ESCAPE) {
                cout << "\n你撤退了，返回城镇。" << endl;
                inTower = 0;
                system("pause");
                return;
            }
            else { // BATTLE_WIN
                if (roomType == ROOM_BOSS) {
                    cout << "恭喜通关此塔！" << endl;
                    if (towerId == 1) northClear = 1;
                    else if (towerId == 2) westClear = 1;
                    else if (towerId == 3) southClear = 1;
                    else if (towerId == 4) eastUnlock = 0; // 东塔通关等特殊设定
                }
            }
        }
        else if (roomType == ROOM_HEAL) {
            cout << "回血房间！你的宠物恢复了！" << endl;
            for (auto& p : player.team) p.heal(p.getMaxHp());
        }
        else if (roomType == ROOM_GOLD) {
            cout << "金币房间！获得 100 金币！" << endl;
            player.gold += 100;
        }

        curRoomIdx++;
        system("pause");
    }

    cout << "塔内全部探索完毕！" << endl;
    inTower = 0;
    system("pause");
}