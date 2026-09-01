#ifndef BATTLE_H
#define BATTLE_H
#include "data.h"

enum BattleResult
{
    BATTLE_WIN,
    BATTLE_LOSE,
    BATTLE_ESCAPE
};

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
 * @param our 初始出战宠物引用
 * @param enemy 敌方宠物引用
 * @param player 玩家对象
 * @return BattleResult
 */
BattleResult battle(Pet& our, Pet& enemy, Player& player);

/**
 * @brief 绘制战斗界面UI
 */
void drawBattleUI(const Pet& our, const Pet& enemy, const Player& player);

/**
 * @brief 执行玩家行动
 * @param action 行动类型
 * @param our 当前出战宠物
 * @param enemy 敌方宠物
 * @param player 玩家
 * @param outActivePet [输出]切换宠物时写入新出战宠物指针
 * @return true行动执行完成；false执行失败
 */
bool doPlayerAction(BattleAction action, Pet& our, Pet& enemy, Player& player, Pet*& outActivePet);

/**
 * @brief 敌方AI回合
 */
void enemyAiTurn(Pet& enemy, Pet& our, Player& player);

/**
 * @brief 使用背包道具
 * @param itemIdx 背包下标
 * @param target 作用目标（出战宠物）
 * @param enemy 敌方
 * @param player 玩家
 * @return true使用成功
 */
bool battleUseItem(int itemIdx, Pet& target, Pet& enemy, Player& player);

/**
 * @brief 切换出战宠物
 * @param teamIdx 队伍下标
 * @param player 玩家
 * @param outActivePet [输出]新出战宠物指针
 * @return true切换成功
 */
bool battleSwitchPet(int teamIdx, Player& player, Pet*& outActivePet);

/**
 * @brief 尝试逃跑
 */
bool battleTryEscape();

/**
 * @brief 战斗结算：buff重置、经验奖励
 */
void battleSettle(BattleResult res, Pet& our, Pet& enemy, Player& player);

#endif // BATTLE_H