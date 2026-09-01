#include "save_load.h"

#include <fstream>
#include <iomanip>

namespace {
// 防止损坏存档构造过大的队伍并耗尽内存。
constexpr std::size_t kMaximumTeamSize = 20;
}

bool saveGame(const Player& player, const std::string& fileName) {
    std::ofstream out(fileName);
    if (!out) return false;
    out << player.gold << ' ' << player.healthPotion << ' ' << player.powerPotion << '\n';
    for (bool clear : player.cleared) out << clear << ' ';
    out << '\n' << player.team.size() << '\n';
    for (const Creature& c : player.team) {
        out << std::quoted(c.name) << ' ' << static_cast<int>(c.element) << ' ' << c.level << ' '
            << c.maxHp << ' ' << c.hp << ' ' << c.baseDamage << ' ' << c.skillLevel << ' '
            << c.battlePoints << ' ' << c.damageBuffTurns << ' ' << c.dodgeBuffTurns << ' ' << c.isBoss << '\n';
    }
    return true;
}

bool loadGame(Player& player, const std::string& fileName) {
    std::ifstream in(fileName);
    if (!in) return false;
    Player loaded;
    if (!(in >> loaded.gold >> loaded.healthPotion >> loaded.powerPotion)) return false;
    for (bool& clear : loaded.cleared) if (!(in >> clear)) return false;
    std::size_t count = 0;
    if (!(in >> count) || count == 0 || count > kMaximumTeamSize) return false;
    for (std::size_t i = 0; i < count; ++i) {
        Creature c;
        int element = 0;
        if (!(in >> std::quoted(c.name) >> element >> c.level >> c.maxHp >> c.hp >> c.baseDamage
              >> c.skillLevel >> c.battlePoints >> c.damageBuffTurns >> c.dodgeBuffTurns >> c.isBoss)) return false;
        if (element < 0 || element > 2) return false;
        c.element = static_cast<Element>(element);
        loaded.team.push_back(c);
    }
    player = loaded;
    return true;
}
