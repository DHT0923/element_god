#pragma once

#include "data.h"
#include <string>

// 将玩家数据保存到文本存档；默认文件为 savegame.txt。
bool saveGame(const Player& player, const std::string& fileName = "savegame.txt");
// 从文本存档读取玩家数据；读取失败时不修改传入的玩家对象。
bool loadGame(Player& player, const std::string& fileName = "savegame.txt");
