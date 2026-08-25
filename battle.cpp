#include "battle.h"

#include <iostream>
#include <random>
#include <chrono>
#include <thread>

//跨平台毫秒延时
static void msleep(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

//跨平台清屏
#if defined(_WIN32) || defined(_WIN64)
#define BATTLE_CLS system("cls")
#else
#define BATTLE_CLS system("clear")
#endif

void drawBattleUI(const Pet& our, const Pet& enemy, const Player& /*player*/)
{
    BATTLE_CLS;
    std::cout << "==========宠物对战==========\n";

    std::cout << "【敌方】" << enemy.getName()
              << "  属性：" << enemy.getType() << "\n";
    std::cout << "HP：" << enemy.getHp() << "/" << enemy.getMaxHp()
              << "    能量：" << enemy.getEnergyPoints() << "\n";

    std::cout << "----------------------------\n";

    std::cout << "【我方】" << our.getName()
              << "  属性：" << our.getType() << "\n";
    std::cout << "HP：" << our.getHp() << "/" << our.getMaxHp()
              << "    能量：" << our.getEnergyPoints() << "\n";

    std::cout << "\n1-技能1   2-技能2   3-技能3\n";
    std::cout << "4-使用道具  5-切换宠物  6-逃跑\n";
    std::cout << "请输入操作数字：";
}

bool doPlayerAction(BattleAction action, Pet& our, Pet& enemy, Player& player)
{
    switch (action)
    {
    case ACT_SKILL1:
        our.useSkill1(enemy);
        return true;
    case ACT_SKILL2:
        our.useSkill2(enemy);
        return true;
    case ACT_SKILL3:
        our.useSkill3(enemy);
        return true;
    case ACT_USE_ITEM:
    {
        int idx;
        std::cout << "\n输入背包物品下标：";
        std::cin >> idx;
        return battleUseItem(idx, our, enemy, player);
    }
    case ACT_SWITCH_PET:
    {
        int idx;
        std::cout << "\n输入队伍宠物下标：";
        std::cin >> idx;
        Pet* temp = nullptr;
        return battleSwitchPet(idx, player, temp);
    }
    case ACT_ESCAPE:
        return battleTryEscape();
    default:
        return false;
    }
}

void enemyAiTurn(Pet& enemy, Pet& our, Player& /*player*/)
{
    static std::mt19937 rng((std::random_device{})());
    std::uniform_int_distribution<int> skillRand(1, 3);

    std::cout << "\n-----敌方行动-----\n";
    msleep(600);

    int select = skillRand(rng);
    if (select == 1)
    {
        enemy.useSkill1(our);
    }
    else if (select == 2)
    {
        enemy.useSkill2(our);
    }
    else
    {
        enemy.useSkill3(our);
    }
}

bool battleUseItem(int /*itemIdx*/, Pet& /*our*/, Pet& /*enemy*/, Player& /*player*/)
{
    //业务待实现：读取player.bag，调用our.heal()等公有接口
    std::cout << "道具功能尚未实现\n";
    msleep(600);
    return false;
}

bool battleSwitchPet(int /*teamIdx*/, Player& /*player*/, Pet*& /*outActivePet*/)
{
    //业务待实现：访问player.team，输出出战宠物指针
    std::cout << "切换宠物尚未实现\n";
    msleep(600);
    return false;
}

bool battleTryEscape()
{
    static std::mt19937 rng((std::random_device{})());
    std::uniform_int_distribution<int> rollDist(0, 99);

    int roll = rollDist(rng);
    if (roll < 35)
    {
        std::cout << "\n逃跑成功！\n";
        msleep(800);
        return true;
    }
    std::cout << "\n逃跑失败！\n";
    msleep(800);
    return false;
}

void battleSettle(BattleResult res, Pet& our, Pet& enemy, Player& /*player*/)
{
    //战斗结束重置双方增益buff，调用公有接口
    our.resetBuff();
    enemy.resetBuff();

    if (res == BATTLE_WIN)
    {
        std::cout << "\n>>>>战斗胜利<<<<\n";
        //此处可写：player队伍调用addExp()增加经验
    }
    else if (res == BATTLE_LOSE)
    {
        std::cout << "\n>>>>战斗失败<<<<\n";
    }
    msleep(1200);
}

BattleResult battle(Pet& our, Pet& enemy, Player& player)
{
    while (true)
    {
        drawBattleUI(our, enemy, player);

        int op;
        std::cin >> op;
        BattleAction act;

        switch (op)
        {
        case 1: act = ACT_SKILL1; break;
        case 2: act = ACT_SKILL2; break;
        case 3: act = ACT_SKILL3; break;
        case 4: act = ACT_USE_ITEM; break;
        case 5: act = ACT_SWITCH_PET; break;
        case 6: act = ACT_ESCAPE; break;
        default:
            std::cout << "\n无效输入，请重新选择\n";
            msleep(600);
            continue;
        }

        bool ret = doPlayerAction(act, our, enemy, player);

        //逃跑直接跳出战斗循环
        if (act == ACT_ESCAPE && ret == true)
        {
            battleSettle(BATTLE_ESCAPE, our, enemy, player);
            return BATTLE_ESCAPE;
        }

        //敌方死亡判定，只读getHp()
        if (enemy.getHp() <= 0)
        {
            battleSettle(BATTLE_WIN, our, enemy, player);
            return BATTLE_WIN;
        }

        //敌方AI回合
        enemyAiTurn(enemy, our, player);

        //我方死亡判定
        if (our.getHp() <= 0)
        {
            battleSettle(BATTLE_LOSE, our, enemy, player);
            return BATTLE_LOSE;
        }
    }
}