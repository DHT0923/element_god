#include "tower.h"

#include "battle.h"
#include <cstdlib>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

void setColor(int color) {
#ifdef _WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(color));
#else
    const char* ansi = color == 14 ? "\033[93m" : color == 12 ? "\033[91m" :
        color == 10 ? "\033[92m" : color == 13 ? "\033[95m" : "\033[0m";
    std::cout << ansi;
#endif
}

void hideCursor() {
#ifdef _WIN32
    CONSOLE_CURSOR_INFO cursorInfo = { 1, FALSE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
#else
    std::cout << "\033[?25l";
#endif
}

void gotoxy(int x, int y) {
#ifdef _WIN32
    COORD coord = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    std::cout << "\033[" << y + 1 << ";" << x + 1 << "H";
#endif
}

void drawBox(int x, int y) {
    setColor(12);
    gotoxy(x, y);     std::cout << "┌──────────┐";
    gotoxy(x, y + 1); std::cout << "│          │";
    gotoxy(x, y + 2); std::cout << "│          │";
    gotoxy(x, y + 3); std::cout << "└──────────┘";
    setColor(7);
}

void drawVLine(int x, int y1, int y2) {
    setColor(12);
    for (int y = y1; y <= y2; ++y) { gotoxy(x, y); std::cout << "│"; }
    setColor(7);
}

void drawHLine(int x1, int x2, int y) {
    setColor(12);
    for (int x = x1; x <= x2; ++x) { gotoxy(x, y); std::cout << "─"; }
    setColor(7);
}

namespace {
    void drawNode(int nodeX, int nodeY, int playerX, int playerY, int textX, int textY,
        const std::string& name, bool finished = false) {
        gotoxy(textX, textY);
        if (playerX == nodeX && playerY == nodeY) {
            setColor(14); std::cout << "★" << name << "★";
        }
        else if (finished) {
            setColor(13); std::cout << " " << name << " ";
        }
        else {
            setColor(10); std::cout << " " << name << " ";
        }
        setColor(7);
    }

    Element towerElement(TowerDirection direction) {
        if (direction == TowerDirection::North) return Element::Fire;
        if (direction == TowerDirection::West) return Element::Grass;
        if (direction == TowerDirection::South) return Element::Water;
        return static_cast<Element>(std::rand() % 3);
    }

    void renderTowerMap(int playerX, int playerY, const bool cleared[5], bool bossOpen) {
        clearScreen();
        hideCursor();
        setColor(14); gotoxy(0, 0); std::cout << "【塔内地图】";

        // 房间、线条与参考文件采用相同坐标和十字布局。
        drawBox(30, 3);   // 顶部奇遇
        drawBox(30, 9);   // 中央小怪
        drawBox(5, 9);    // 左侧小怪
        drawBox(30, 15);  // 下方奇遇
        drawBox(30, 21);  // 首领
        drawVLine(35, 7, 8);
        drawHLine(17, 29, 11);
        drawVLine(35, 13, 14);
        drawVLine(35, 19, 20);

        drawNode(1, 0, playerX, playerY, 32, 4, cleared[0] ? "已探索" : "奇遇房", cleared[0]);
        drawNode(1, 1, playerX, playerY, 32, 10, cleared[1] ? "已探索" : "小怪房", cleared[1]);
        drawNode(0, 1, playerX, playerY, 7, 10, cleared[2] ? "已探索" : "小怪房", cleared[2]);
        drawNode(1, 2, playerX, playerY, 32, 16, cleared[3] ? "已探索" : "奇遇房", cleared[3]);
        drawNode(1, 3, playerX, playerY, 32, 22, bossOpen ? "首领房" : "封印房", false);

        setColor(13);
        gotoxy(0, 28);
        std::cout << "W↑ S↓ A← D→ 移动  |  Q 撤离  |  首领需先击败两只小怪";
        setColor(7);
        gotoxy(0, 30);
        std::cout << "行动：";
    }

    char readMove() {
        char key = 'q';
        std::cin >> key;
        if (key >= 'A' && key <= 'Z') key = static_cast<char>(key - 'A' + 'a');
        return key;
    }

    int moveTowerNode(int current, char key) {
        // 0: 顶部奇遇，1: 中央小怪，2: 左侧小怪，3: 下方奇遇，4: 首领。
        if (current == 0 && key == 's') return 1;
        if (current == 1 && key == 'w') return 0;
        if (current == 1 && key == 'a') return 2;
        if (current == 1 && key == 's') return 3;
        if (current == 2 && key == 'd') return 1;
        if (current == 3 && key == 'w') return 1;
        if (current == 3 && key == 's') return 4;
        if (current == 4 && key == 'w') return 3;
        return current;
    }

    void towerNodePosition(int node, int& x, int& y) {
        const int positions[5][2] = { {1,0}, {1,1}, {0,1}, {1,2}, {1,3} };
        x = positions[node][0]; y = positions[node][1];
    }

    void resolveEvent(Player& player) {
        const int event = std::rand() % 3;
        clearScreen();
        std::cout << "【神秘奇遇】\n\n";
        if (event == 0) { player.gold += 40; std::cout << "迷路商人留下 40 金币后消失了。\n"; }
        else if (event == 1) { ++player.powerPotion; std::cout << "古老祭坛赠予你 1 瓶力量药水。\n"; }
        else { for (Creature& c : player.team) healCreature(c, 30); std::cout << "精灵之风为全队恢复 30 HP。\n"; }
        std::cout << "\n按 Enter 返回地图。";
        std::cin.ignore(10000, '\n'); std::cin.get();
    }
}

// ===========================================================================
// 以下是修改后的 renderMainMap 函数（增加了地点描述）
// ===========================================================================
void renderMainMap(int playerX, int playerY, const Player& player) {
    clearScreen();
    hideCursor();
    setColor(14); gotoxy(0, 0); std::cout << "【主城地图】";
    drawBox(30, 3);
    drawBox(5, 12);
    drawBox(30, 12);
    drawBox(55, 12);
    drawBox(30, 21);
    drawVLine(35, 7, 11);
    drawVLine(35, 16, 20);
    drawHLine(17, 29, 14);
    drawHLine(41, 54, 14);
    drawNode(2, 0, playerX, playerY, 32, 4, "北塔", player.cleared[3]);
    drawNode(0, 2, playerX, playerY, 7, 13, "西塔", player.cleared[1]);
    drawNode(2, 2, playerX, playerY, 32, 13, "城镇");
    drawNode(4, 2, playerX, playerY, 57, 13, "东塔", player.cleared[0]);
    drawNode(2, 4, playerX, playerY, 32, 22, "南塔", player.cleared[2]);
    setColor(13);
    gotoxy(0, 28);
    std::cout << "W↑ S↓ A← D→ 移动  |  1 进入地点  |  Q 保存并退出";
    setColor(7);
    gotoxy(0, 30);
    std::cout << "行动：";

    // ----- 新增：显示当前位置描述 -----
    gotoxy(0, 26);                     // 放在地图和操作提示之间
    setColor(7);                       // 白色文字
    if (playerX == 2 && playerY == 0) {
        std::cout << "北塔：高耸的北塔，闪耀着赤红色的光芒";
    }
    else if (playerX == 0 && playerY == 2) {
        std::cout << "西塔：静谧的西塔，流转着幽蓝色的水光";
    }
    else if (playerX == 2 && playerY == 2) {
        std::cout << "城镇：繁华的精灵城镇，人来人往";
    }
    else if (playerX == 4 && playerY == 2) {
        std::cout << "东塔：死寂的东塔，弥散着墨黑色的阴霾";
    }
    else if (playerX == 2 && playerY == 4) {
        std::cout << "南塔：古老的南塔，缠绕着翠绿的藤蔓";
    }
    else {
        // 如果玩家不在任何节点（初始或异常），清空该行
        std::cout << "                                        ";
    }
    // ----- 新增结束 -----
}

bool exploreTower(Player& player, TowerDirection direction) {
    const int towerIndex = static_cast<int>(direction);
    if (player.cleared[towerIndex]) return false;

    const Element element = towerElement(direction);
    bool cleared[5] = { false, false, false, false, false };
    int current = 1;
    int monsterWins = 0;

    while (true) {
        int x, y; towerNodePosition(current, x, y);
        renderTowerMap(x, y, cleared, monsterWins >= 2);
        const char key = readMove();
        if (key == 'q') return false;
        const int next = moveTowerNode(current, key);
        if (next == current) continue;
        current = next;

        if (cleared[current]) continue;
        if (current == 4) {
            if (monsterWins < 2) continue;
            clearScreen();
            std::cout << "【首领房】" << elementName(element) << "属性塔主降临！\n";
            if (!startBattle(player, makeCreature(element, 3, true, std::string(elementName(element)) + "属性塔主"))) return false;
            player.cleared[towerIndex] = true;
            player.team.push_back(makeCreature(element, 3, false, std::string("捕获·") + elementName(element) + "属性塔主"));
            player.gold += 50;
            return true;
        }
        if (current == 1 || current == 2) {
            clearScreen();
            std::cout << "【小怪房】高塔守卫出现！\n";
            if (!startBattle(player, makeCreature(element, 1 + std::rand() % 2, false, "高塔守卫"))) return false;
            cleared[current] = true;
            ++monsterWins;
            player.gold += 18;
            continue;
        }
        cleared[current] = true;
        resolveEvent(player);
    }
}