#include "data.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <random>

const char* elementName(Element element) {
    switch (element) {
        case Element::Fire: return "火";
        case Element::Water: return "水";
        case Element::Grass: return "草";
    }
    return "未知";
}

const char* towerName(TowerDirection direction) {
    switch (direction) {
        case TowerDirection::East: return "东方·混沌高塔";
        case TowerDirection::West: return "西方·森语高塔";
        case TowerDirection::South: return "南方·潮汐高塔";
        case TowerDirection::North: return "北方·熔岩高塔";
    }
    return "未知高塔";
}

Element counterElement(Element element) {
    // 水克火、火克草、草克水，形成完整的三属性循环。
    if (element == Element::Water) return Element::Fire;
    if (element == Element::Fire) return Element::Grass;
    return Element::Water;
}

double damageMultiplier(Element attacker, Element defender) {
    if (counterElement(attacker) == defender) return 1.5;
    if (counterElement(defender) == attacker) return 0.7;
    return 1.0;
}

Creature makeCreature(Element element, int level, bool boss, const std::string& customName) {
    static constexpr const char* names[] = {"炎尾狐", "泡泡鲸", "叶角鹿"};
    Creature c;
    c.element = element;
    c.level = level;
    c.isBoss = boss;
    c.name = customName.empty() ? names[static_cast<int>(element)] : customName;
    c.maxHp = 82 + level * 18 + (boss ? 38 : 0);
    c.hp = c.maxHp;
    c.baseDamage = 12 + level * 6 + (boss ? 7 : 0);
    return c;
}

Player newGame() {
    Player player;
    player.team.push_back(makeCreature(Element::Water, 1, false, "初始·泡泡鲸"));
    return player;
}

bool hasLivingCreature(const Player& player) {
    for (const Creature& creature : player.team)
        if (creature.hp > 0) return true;
    return false;
}

void healCreature(Creature& creature, int amount) {
    creature.hp = std::min(creature.maxHp, creature.hp + amount);
}

void printCreature(const Creature& creature) {
    std::cout << creature.name << " [" << elementName(creature.element)
              << "] Lv." << creature.level << " HP " << creature.hp << "/" << creature.maxHp
              << " 伤害 " << creature.baseDamage << " 技能Lv." << creature.skillLevel << '\n';
}

void printTeam(const Player& player) {
    std::cout << "\n=== 你的精灵队伍 ===\n";
    for (std::size_t i = 0; i < player.team.size(); ++i) {
        std::cout << i + 1 << ". ";
        printCreature(player.team[i]);
    }
}

void fullyRestoreTeam(Player& player) {
    for (Creature& creature : player.team) {
        creature.hp = creature.maxHp;
        creature.battlePoints = 0;
        creature.damageBuffTurns = 0;
        creature.dodgeBuffTurns = 0;
    }
}

void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\033[2J\033[H";
#endif
}

int randomInt(int minInclusive, int maxInclusive) {
    static std::mt19937 engine(std::random_device{}());
    std::uniform_int_distribution<int> distribution(minInclusive, maxInclusive);
    return distribution(engine);
}
