#pragma once
#include "data.h"

//战斗结果枚举，移除捕获相关
enum BattleResult
{
    BATTLE_WIN,
    BATTLE_LOSE,
    BATTLE_ESCAPE
};

//战斗动作，删掉ACT_CAPTURE
enum BattleAction
{
    ACT_ATTACK,
    ACT_BUFF,
    ACT_FINAL_SKILL,
    ACT_USE_ITEM,
    ACT_SWITCH_PET,
    ACT_ESCAPE
};

//【C对外唯一入口】完整回合战斗，内部cin+cout输出UI
BattleResult battle(Pet& our, Pet& enemy, Player& player);

//绘制战斗面板、血条、Boss残血提示
void drawBattleUI(const Pet& our, const Pet& enemy, const Player& player);

//战斗子函数
void battleAttack(Pet& attacker, Pet& defender, Player& player);
void battleBuff(Pet& self);
bool battleFinalSkill(Pet& our, Pet& enemy, Player& player);

bool battleUseItem(Pet& our, Pet& enemy, Player& player, int itemId);
bool battleSwitchPet(Player& player, Pet*& outPet);
bool battleTryEscape(const Pet& our, const Pet& enemy);

bool checkDodge(const Pet& defender);
void triggerEnemyPassive(Pet& our, Pet& enemy, Player& player);

void petDeathHandle(Pet& pet);
//胜利：分发奖励；若是Boss自动捕获
void battleGiveReward(Player& player, Pet& defeatedEnemy);
void battleClearAllBuff(Pet& a, Pet& b);