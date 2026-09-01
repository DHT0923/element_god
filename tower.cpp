#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <conio.h>
#include <windows.h>

using namespace std;

// ---------- 工具函数 ----------
void clearScreen() { system("cls"); }
void setColor(int color) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color); }
void hideCursor() { CONSOLE_CURSOR_INFO cursor_info = { 1, 0 }; SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info); }
void gotoxy(int x, int y) { COORD coord; coord.X = x; coord.Y = y; SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord); }

// 画固定方框
void drawBox(int x, int y) {
    setColor(12);
    gotoxy(x, y); cout << "┌──────────┐";
    gotoxy(x, y + 1); cout << "│          │";
    gotoxy(x, y + 2); cout << "│          │";
    gotoxy(x, y + 3); cout << "└──────────┘";
    setColor(7);
}

// 画固定连线
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

// ================== 主城地图 (全硬编码，绝对不乱) ==================
void renderMainMap(int playerX, int playerY) {
    clearScreen();
    hideCursor();

    // 颜色
    setColor(14); gotoxy(0, 0); cout << "【主城地图】";

    // 1. 北塔 (上)
    drawBox(30, 3);
    // 2. 西塔 (左)
    drawBox(5, 12);
    // 3. 城镇 (中)
    drawBox(30, 12);
    // 4. 东塔 (右)
    drawBox(55, 12);
    // 5. 南塔 (下)
    drawBox(30, 21);

    // 连线
    drawVLine(35, 7, 11);  // 北塔连中
    drawVLine(35, 16, 20); // 中连南塔
    drawHLine(17, 29, 14); // 西塔连中
    drawHLine(41, 54, 14); // 中连东塔

    // 文字
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

// ================== 塔内地图 (路线：奇遇 -> 小怪 -> 分支小怪 -> 奇遇 -> BOSS) ==================
void renderTowerMap(int playerX, int playerY) {
    clearScreen();
    hideCursor();

    setColor(14); gotoxy(0, 0); cout << "【塔内地图】";

    // 1. 出生地·奇遇房 (最上)
    drawBox(30, 3);
    // 2. 主线·小怪房
    drawBox(30, 9);
    // 3. 左侧分支·小怪房
    drawBox(5, 9);
    // 4. 主线·奇遇房
    drawBox(30, 15);
    // 5. BOSS房 (最下)
    drawBox(30, 21);

    // 连线
    drawVLine(35, 7, 8);  // 奇遇下连小怪
    drawHLine(17, 29, 11); // 小怪左连分支小怪
    drawVLine(35, 13, 14); // 小怪下连奇遇
    drawVLine(35, 19, 20); // 奇遇下连BOSS

    // 文字
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

// ================== 主程序 ==================
int main() {
    SetConsoleTitle(L"精灵世界地图");

    int mainX = 2, mainY = 2;
    int towerX = 1, towerY = 0; // 出生在奇遇房

    bool inTower = false;
    bool running = true;

    while (running) {
        if (!inTower) {
            renderMainMap(mainX, mainY);
            char key = tolower(_getch());

            if (key == 'w' && mainY > 0) mainY -= 2;
            else if (key == 's' && mainY < 4) mainY += 2;
            else if (key == 'a' && mainX > 0) mainX -= 2;
            else if (key == 'd' && mainX < 4) mainX += 2;

            else if (key == '1') {
                if ((mainX == 2 && mainY == 0) || (mainX == 0 && mainY == 2) ||
                    (mainX == 2 && mainY == 4) || (mainX == 4 && mainY == 2)) {
                    inTower = true;
                    towerX = 1;
                    towerY = 0; // 进塔出生在奇遇
                }
            }
            else if (key == 'q') {
                running = false;
            }
        }
        else {
            renderTowerMap(towerX, towerY);
            char key = tolower(_getch());

            // 死锁路线控制
            if (key == 'w') {
                if (towerY == 1 && towerX == 1) towerY = 0; // 小怪回奇遇
                else if (towerY == 1 && towerX == 0) towerX = 1; // 左侧回主线小怪
                else if (towerY == 2) towerY = 1; // 奇遇回小怪
                else if (towerY == 3) towerY = 2; // BOSS回奇遇
            }
            else if (key == 's') {
                if (towerY == 0) towerY = 1; // 奇遇到小怪
                else if (towerY == 1 && towerX == 1) towerY = 2; // 主线小怪到奇遇
                else if (towerY == 1 && towerX == 0) { /* 左侧分支不能向下 */ }
                else if (towerY == 2) towerY = 3; // 奇遇到BOSS
            }
            else if (key == 'a') {
                if (towerX == 1 && towerY == 1) towerX = 0; // 主线小怪去左侧分支
            }
            else if (key == 'd') {
                if (towerX == 0 && towerY == 1) towerX = 1; // 左侧小怪回主线
            }

            else if (key == 'q') {
                inTower = false;
                mainX = 2; mainY = 2;
            }
        }
    }

    clearScreen();
    cout << "感谢游玩，再见!" << endl;
    return 0;
}