#pragma once

#include <array>
#include <string>
#include <vector>

// 三种基础元素；克制关系为水克火、火克草、草克水。
enum class Element { Fire = 0, Water = 1, Grass = 2 };

enum class TowerDirection { East = 0, West = 1, South = 2, North = 3 };

// 单只精灵的战斗数据与临时战斗状态。
struct Creature {
    std::string name;
    Element element = Element::Water;
    int level = 1;
    int maxHp = 100;
    int hp = 100;
    int baseDamage = 18;
    int skillLevel = 1;
    int battlePoints = 0;
    int damageBuffTurns = 0;
    int dodgeBuffTurns = 0;
    bool isBoss = false;
};

// 玩家持有的资源、队伍与四座高塔进度。
struct Player {
    int gold = 80;
    int healthPotion = 2;
    int powerPotion = 1;
    std::vector<Creature> team;
    std::array<bool, 4> cleared{};
};

// 返回元素、塔的中文显示名称。
const char* elementName(Element element);
const char* towerName(TowerDirection direction);
// 返回克制指定元素的元素，并计算最终伤害倍率。
Element counterElement(Element element);
double damageMultiplier(Element attacker, Element defender);
// 创建精灵与新游戏初始数据。
Creature makeCreature(Element element, int level, bool boss, const std::string& customName = "");
Player newGame();
// 队伍与生命值工具函数。
bool hasLivingCreature(const Player& player);
void healCreature(Creature& creature, int amount);
void printCreature(const Creature& creature);
void printTeam(const Player& player);
// 清空当前控制台，并将全队恢复为满生命、无临时增益状态。
void clearScreen();
void fullyRestoreTeam(Player& player);
// 返回闭区间内的随机整数，供战斗和高塔事件共用。
int randomInt(int minInclusive, int maxInclusive);
