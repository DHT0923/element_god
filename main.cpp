#include "data.h"
#include "save_load.h"
#include "tower.h"

#include <fstream>
#include <iostream>
#include <limits>

namespace {
// 主程序私有的城镇菜单与世界地图流程。
int readChoice(int low, int high) {
    int choice;
    while (!(std::cin >> choice) || choice < low || choice > high) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "请输入 " << low << " 到 " << high << " 的数字：";
    }
    return choice;
}

void backpack(Player& player) {
    clearScreen();
    std::cout << "╔════════════ 背包 ════════════╗\n"
              << "生命药水  " << player.healthPotion << "  恢复 55 HP\n"
              << "力量药水  " << player.powerPotion << "  战斗中提升伤害\n"
              << "╚══════════════════════════════╝\n";
    printTeam(player);
    std::cout << "选择精灵编号，输入 0 返回：";
    int selected = readChoice(0, static_cast<int>(player.team.size()));
    if (!selected) return;
    int id = selected - 1;
    std::cout << "1 使用生命药水\n2 返回\n选择：";
    int choice = readChoice(1, 2);
    if (choice == 2) return;
    if (choice == 1) {
        if (!player.healthPotion) { std::cout << "生命药水不足。\n"; return; }
        --player.healthPotion; healCreature(player.team[id], 55); std::cout << "已使用生命药水。\n";
    }
}

void shop(Player& player) {
    std::cout << "\n=== 商店（金币：" << player.gold << "）===\n1. 生命药水：20 金币\n2. 力量药水：30 金币\n3. 返回\n选择：";
    int choice = readChoice(1, 3);
    if (choice == 1 && player.gold >= 20) { player.gold -= 20; ++player.healthPotion; std::cout << "购买成功。\n"; }
    else if (choice == 2 && player.gold >= 30) { player.gold -= 30; ++player.powerPotion; std::cout << "购买成功。\n"; }
    else if (choice != 3) std::cout << "金币不足。\n";
}

void trainer(Player& player) {
    const int cost = 45;
    std::cout << "\n=== 精灵培养师（升级技能需 " << cost << " 金币）===\n";
    printTeam(player);
    std::cout << "输入精灵编号升级，输入 0 返回：";
    int id = readChoice(0, static_cast<int>(player.team.size()));
    if (!id) return;
    if (player.gold < cost) { std::cout << "金币不足。\n"; return; }
    Creature& c = player.team[id - 1];
    if (c.skillLevel >= 5) { std::cout << "技能已经满级。\n"; return; }
    player.gold -= cost; ++c.skillLevel; c.baseDamage += 3;
    std::cout << c.name << " 的技能升至 Lv." << c.skillLevel << "！\n";
}

void townMenu(Player& player) {
    while (true) {
        clearScreen();
        int completed = 0; for (bool cleared : player.cleared) if (cleared) ++completed;
        std::cout << "\n╔════════ 城镇广场 ════════╗\n"
                  << "金币 " << player.gold << " | 高塔 " << completed << "/4\n"
                  << "1. 查看队伍  2. 商店  3. 培养师\n"
                  << "4. 背包  5. 保存  0. 返回地图\n"
                  << "╚══════════════════════════╝\n选择：";
        int choice = readChoice(0, 5);
        if (choice == 0) return;
        if (choice == 1) printTeam(player);
        else if (choice == 2) shop(player);
        else if (choice == 3) trainer(player);
        else if (choice == 4) backpack(player);
        else std::cout << (saveGame(player) ? "保存成功。\n" : "保存失败。\n");
    }
}

void worldMap(Player& player) {
    // 节点坐标与参考代码一致：北(2,0) 西(0,2) 城镇(2,2) 东(4,2) 南(2,4)。
    int x = 2, y = 2;
    while (true) {
        renderMainMap(x, y, player);
        char key;
        if (!(std::cin >> key)) return;
        if (key >= 'A' && key <= 'Z') key = static_cast<char>(key - 'A' + 'a');
        if (key == 'q') { saveGame(player); std::cout << "游戏已自动保存，再见！\n"; return; }
        if (key == '1') {
            if (x == 2 && y == 2) townMenu(player);
            else {
                TowerDirection target = x == 2 && y == 0 ? TowerDirection::North :
                                        x == 0 && y == 2 ? TowerDirection::West :
                                        x == 4 && y == 2 ? TowerDirection::East : TowerDirection::South;
                exploreTower(player, target);
                if (!hasLivingCreature(player)) {
                    clearScreen();
                    std::cout << "元素力的庇护将你送回了城镇。\n所有精灵的生命已经完全恢复。\n按 Enter 继续。";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cin.get();
                    fullyRestoreTeam(player); x = 2; y = 2;
                }
            }
            continue;
        }
        if (x == 2 && y == 2 && key == 'w') y = 0;
        else if (x == 2 && y == 2 && key == 's') y = 4;
        else if (x == 2 && y == 2 && key == 'a') x = 0;
        else if (x == 2 && y == 2 && key == 'd') x = 4;
        else if (x == 2 && y == 0 && key == 's') y = 2;
        else if (x == 2 && y == 4 && key == 'w') y = 2;
        else if (x == 0 && y == 2 && key == 'd') x = 2;
        else if (x == 4 && y == 2 && key == 'a') x = 2;
    }
}
}

int main() {
    clearScreen();
    // std::ifstream titleFile("元神.md");
    // std::string titleLine;
    // while (std::getline(titleFile, titleLine)) std::cout << titleLine << '\n';
    std::cout << "                                      #               #";
    std::cout << "      ###################                #            #       ";
    std::cout << "                                  ############        #";
    std::cout << "                                           #   ###############";
    std::cout << " ###############################         #     #      #      # ";
    std::cout << "          #          #                 #       ###############  ";
    std::cout << "          #          #               # # #     #      #      # ";
    std::cout << "          #          #             #   #  #    ###############";
    std::cout << "         #           #                 #   #          #    ";
    std::cout << "        #            #                 #              #     ";
    std::cout << "       #                ##     #       #              #   ";
    std::cout << "     #                   ######        #              #  ";
    std::cout << "\n               你能成为元素精灵之神吗？\n\n按 Enter 开始。";
    std::cin.get();
    Player player;
    std::cout << "1. 新游戏\n2. 读取存档\n选择：";
    if (readChoice(1, 2) == 2 && loadGame(player)) std::cout << "读档成功，欢迎回来！\n";
    else {
        player = newGame();
        clearScreen();
        std::cout << "【序章·元素长老的嘱托】\n\n"
                  << "长老：渊主腐化了四座高塔，元素循环正在崩坏。\n"
                  << "长老：水克火、火克草、草克水。先前往北方的熔岩高塔吧。\n"
                  << "长老将伙伴“初始·泡泡鲸”托付给了你。\n\n按 Enter 出发。";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cin.get();
    }

    std::cout << "\n从城镇广场出发，开始探索吧。\n";
    worldMap(player);
    return 0;
}
