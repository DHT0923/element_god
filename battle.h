#pragma once

#include "data.h"

// 开始一场回合制战斗；返回 true 表示胜利，false 表示全队战败。
bool startBattle(Player& player, Creature enemy);
