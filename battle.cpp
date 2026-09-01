#include "battle.h"
#include <algorithm>
#include <iostream>
#include <limits>

namespace {
// 本文件的辅助函数仅服务战斗流程，因此不暴露给其他模块。
int readChoice(int low, int high) {
    int choice;
    while (!(std::cin >> choice) || choice < low || choice > high) {
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "请输入 " << low << " 到 " << high << " 的数字：";
    }
    return choice;
}
void waitEnter(const char* prompt = "按 Enter 继续") {
    std::cout << '\n' << prompt;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cin.get();
}
void hpBar(int hp, int maxHp) {
    constexpr int width = 24;
    const int filled = maxHp ? std::max(0, hp * width / maxHp) : 0;
    for (int i = 0; i < width; ++i) std::cout << (i < filled ? "█" : "░");
    std::cout << ' ' << hp << '/' << maxHp;
}
void renderBattle(const Creature& hero, const Creature& enemy) {
    clearScreen();
    std::cout << "╔══════════════════ 元素对战 ══════════════════╗\n"
              << "  我方  " << hero.name << "  [" << elementName(hero.element) << "]\n  ";
    hpBar(hero.hp, hero.maxHp);
    std::cout << "\n\n  敌方  " << enemy.name << "  [" << elementName(enemy.element) << "]\n  ";
    hpBar(enemy.hp, enemy.maxHp);
    std::cout << "\n╚═══════════════════════════════════════════════╝\n"
              << "怒气：" << hero.battlePoints << "/3\n\n";
}
int rawDamage(const Creature& c) {
    int damage = c.baseDamage + c.skillLevel * 4;
    return c.damageBuffTurns > 0 ? damage * 3 / 2 : damage;
}
void attack(Creature& a, Creature& d, int base, const char* skill) {
    if (d.dodgeBuffTurns > 0 && randomInt(1, 100) <= 35) { std::cout << d.name << " 闪避了 " << skill << "！\n"; return; }
    const double multiplier = damageMultiplier(a.element, d.element);
    const int damage = std::max(1, static_cast<int>(base * multiplier));
    d.hp = std::max(0, d.hp - damage);
    std::cout << a.name << " 使用 " << skill << "，造成 " << damage << " 点伤害";
    if (multiplier > 1.0) std::cout << "（属性克制）";
    if (multiplier < 1.0) std::cout << "（属性被克制）";
    std::cout << "！\n";
}
void applyBuff(Creature& c) {
    if (c.element == Element::Grass) {
        const int add = 12 + c.skillLevel * 5; c.maxHp += add; healCreature(c, add + 15 + c.skillLevel * 4);
        std::cout << c.name << " 获得生长祝福，生命上限提升并恢复生命。\n";
    } else if (c.element == Element::Fire) {
        c.damageBuffTurns = 3; std::cout << c.name << " 燃起斗志，3 回合内伤害提高。\n";
    } else {
        c.dodgeBuffTurns = 3; std::cout << c.name << " 化作水影，3 回合内有概率闪避。\n";
    }
}
void consumeBuffs(Creature& c) {
    if (c.damageBuffTurns > 0) --c.damageBuffTurns;
    if (c.dodgeBuffTurns > 0) --c.dodgeBuffTurns;
}
int selectAlive(const Player& player) {
    std::cout << "选择出战精灵：\n";
    for (std::size_t i = 0; i < player.team.size(); ++i) {
        const Creature& c = player.team[i];
        std::cout << i + 1 << ". " << c.name << "  "; hpBar(c.hp, c.maxHp);
        if (c.hp <= 0) std::cout << " 已倒下";
        std::cout << '\n';
    }
    while (true) {
        const int selected = readChoice(1, static_cast<int>(player.team.size())) - 1;
        if (player.team[selected].hp > 0) return selected;
        std::cout << "这只精灵已经倒下，请选择其他精灵。\n";
    }
}
}

bool startBattle(Player& player, Creature enemy) {
    clearScreen();
    std::cout << "【战斗开始】敌方 " << enemy.name << " 出现！\n";
    const int initial = selectAlive(player);
    int active = initial;
    bool skipPlayerTurn = false, rage = false;
    while (hasLivingCreature(player) && enemy.hp > 0) {
        Creature& hero = player.team[active];
        renderBattle(hero, enemy);
        if (skipPlayerTurn) {
            std::cout << "草属性首领的藤蔓缠住了你，本回合被跳过！\n"; skipPlayerTurn = false;
        } else {
            std::cout << "1 普通攻击   2 属性增益   3 终结技(消耗3点怒气)\n"
                      << "4 生命药水 " << player.healthPotion << "   5 力量药水 " << player.powerPotion << "\n选择：";
            const int action = readChoice(1, 5);
            if (action == 1) { attack(hero, enemy, rawDamage(hero), "普通攻击"); hero.battlePoints = std::min(3, hero.battlePoints + 1); }
            else if (action == 2) { applyBuff(hero); hero.battlePoints = std::min(3, hero.battlePoints + 1); }
            else if (action == 3 && hero.battlePoints >= 3) { attack(hero, enemy, rawDamage(hero) * 2 + 10, "终结技"); hero.battlePoints = 0; }
            else if (action == 4 && player.healthPotion > 0) { --player.healthPotion; healCreature(hero, 55); std::cout << "使用生命药水，恢复 55 HP。\n"; }
            else if (action == 5 && player.powerPotion > 0) { --player.powerPotion; hero.damageBuffTurns = 5; std::cout << "使用力量药水，伤害提高。\n"; }
            else { std::cout << "条件不足，改为普通攻击。\n"; attack(hero, enemy, rawDamage(hero), "普通攻击"); hero.battlePoints = std::min(3, hero.battlePoints + 1); }
            consumeBuffs(hero);
        }
        if (enemy.hp <= 0) break;
        if (enemy.isBoss && enemy.element == Element::Fire && !rage && enemy.hp * 100 <= enemy.maxHp * 40) { enemy.baseDamage = enemy.baseDamage * 3 / 2; rage = true; std::cout << "敌方进入灼热暴走，伤害提高！\n"; }
        if (enemy.isBoss && enemy.element == Element::Water && enemy.hp * 100 <= enemy.maxHp * 45) { healCreature(enemy, 18 + enemy.skillLevel * 5); std::cout << "敌方借潮汐回复生命！\n"; }
        if (randomInt(1, 100) <= 30) applyBuff(enemy);
        else {
            attack(enemy, hero, rawDamage(enemy), "敌方攻击");
            if (enemy.isBoss && enemy.element == Element::Grass && randomInt(1, 100) <= 35 && hero.hp > 0) { skipPlayerTurn = true; std::cout << "藤蔓将使你下回合无法行动！\n"; }
        }
        consumeBuffs(enemy);
        if (hero.hp <= 0 && hasLivingCreature(player)) { std::cout << hero.name << " 倒下了！\n"; waitEnter("按 Enter 选择下一只精灵"); active = selectAlive(player); }
        else if (enemy.hp > 0) waitEnter();
    }
    clearScreen();
    if (enemy.hp <= 0) {
        std::cout << "╔════════ 战斗胜利 ════════╗\n你击败了 " << enemy.name << "！\n╚══════════════════════════╝\n";
        waitEnter("按 Enter 返回塔内地图"); return true;
    }
    std::cout << "╔════════ 战斗失败 ════════╗\n全队精灵倒下了……元素力将送你回城镇。\n╚══════════════════════════╝\n";
    waitEnter("按 Enter 接受元素力庇护"); return false;
}
