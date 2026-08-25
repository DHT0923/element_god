#pragma once
#include "data.h" // 依赖data.h数据

// 存档文件名固定
const std::string kSaveFileName = "save.txt";

/**
 * @brief 保存游戏进度
 * @param player 玩家对象的引用
 * 
 * 将玩家数据及全局游戏状态序列化到磁盘文件。
 * 格式为文本键值对，便于调试和查看。
 */
void saveGame(Player& player);

/**
 * @brief 读取游戏进度
 * @param player 玩家对象的引用
 * @return bool 读取成功返回 true，失败（文件不存在或损坏）返回 false
 * 
 * 采用原子性读取：先读入临时变量，校验无误后再覆盖 player 数据。
 * 如果读取失败，player 的当前内存数据不会被破坏。
 */
bool loadGame(Player& player);
