#ifndef TOWER_H
#define TOWER_H

#include <vector>
#include <string>

#include "data.h"    // 依赖 Player, Pet 类定义
#include "battle.h"  // 依赖 battle() 函数，用于战斗房间的调用

// ==========================================
// 全局高塔状态变量（声明为 extern，定义在 tower.cpp 中）
// ==========================================
extern int northClear; // 0未通关 1通关
extern int westClear;
extern int southClear;
extern int eastUnlock; // 东塔解锁标记
extern int inTower;    // 0不在高塔；1北 2西 3南 4东
extern int curRoomIdx; // 高塔当前房间下标

// ==========================================
// 房间类型字符串常量（统一使用，防止写错别字）
// ==========================================
const std::string ROOM_BATTLE = "battle";
const std::string ROOM_HEAL   = "heal";
const std::string ROOM_GOLD   = "gold";
const std::string ROOM_BOSS   = "boss";

// ==========================================
// 对外接口（E城镇模块调用）
// ==========================================

/**
 * @brief 判断是否可以进入某高塔
 * @param towerId 1北塔，2西塔，3南塔，4东塔
 * @return true允许进入；false未解锁
 */
bool canEnterTower(int towerId);

/**
 * @brief 运行一整座高塔流程
 * @param towerId 1北/2西/3南/4东
 * @param player 玩家对象
 *
 * 功能实现流程（在 tower.cpp 中实现）：
 * 1. 生成本塔房间序列 vector<string> roomList
 * 2. 循环遍历每一个房间：战斗房、回血房、金币房、Boss房
 * 3. 战斗房内部调用 battle() (来自 battle.h)
 * 4. Boss胜利后，设置对应高塔通关标记；东塔通关代表游戏通关
 * 5. 玩家可中途撤退，直接返回，不改变通关标记
 */
void runTower(int towerId, Player& player);

#endif // TOWER_H