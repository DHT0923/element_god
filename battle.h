#ifndef BATTLE_H
#define BATTLE_H

#include "data.h"

//战斗结果
enum BattleResult
{
    BATTLE_WIN,
    BATTLE_LOSE,
    BATTLE_ESCAPE
};

//战斗行动
enum BattleAction
{
    ACT_SKILL1,         // 技能1
    ACT_SKILL2,         // 技能2
    ACT_SKILL3,         // 技能3
    ACT_USE_ITEM,       // 使用道具
    ACT_SWITCH_PET,     // 切换宠物
    ACT_ESCAPE          // 逃跑
};

/**
 * @brief 主战斗循环
 * @param our 我方出战宠物引用
 * @param enemy 敌方宠物引用
 * @param player 玩家对象，背包、队伍、奖励
 * @return BattleResult
 */
BattleResult battle(Pet& our, Pet& enemy, Player& player);

/**
 * @brief 绘制战斗界面UI
 */
void drawBattleUI(const Pet& our, const Pet& enemy, const Player& player);

/**
 * @brief 执行玩家选择的行动
 * @return true行动执行成功；false失败(能量不足、下标非法等)
 */
bool doPlayerAction(BattleAction action, Pet& our, Pet& enemy, Player& player);

/**
 * @brief 敌方AI回合逻辑
 */
void enemyAiTurn(Pet& enemy, Pet& our, Player& player);

/**
 * @brief 使用背包物品
 * @param itemIdx 背包bag数组下标
 */
bool battleUseItem(int itemIdx, Pet& our, Pet& enemy, Player& player);

/**
 * @brief 切换出战宠物
 * @param teamIdx player.team队伍下标
 * @param outActivePet 输出：切换后出战宠物指针
 * @return 切换成功返回true
 */
bool battleSwitchPet(int teamIdx, Player& player, Pet*& outActivePet);

/**
 * @brief 尝试逃跑
 */
bool battleTryEscape();

/**
 * @brief 战斗结束统一处理：重置buff、结算奖励
 */
void battleSettle(BattleResult res, Pet& our, Pet& enemy, Player& player);

#endif // BATTLE_H