#include "battle.h"
#include <iostream>
#include <random>
#include <chrono>
#include <thread>

// 跨平台延时
static void msleep(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// 跨平台清屏
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
              << "  属性：" << enemy.getType()
              << "  Lv." << enemy.getLevel() << "\n";
    showHpBar(enemy);
    std::cout << "能量：" << enemy.getEnergyPoints() << "\n";
    std::cout << "----------------------------\n";
    std::cout << "【我方】" << our.getName()
              << "  属性：" << our.getType()
              << "  Lv." << our.getLevel() << "\n";
    showHpBar(our);
    std::cout << "能量：" << our.getEnergyPoints() << "\n";

    std::cout << "\n1-" << our.getSkillName1()
              << "   2-" << our.getSkillName2()
              << "   3-" << our.getSkillName3() << "\n";
    std::cout << "4-使用道具  5-切换宠物  6-逃跑\n";
    std::cout << "请输入操作数字：";
}

// 执行玩家行动
// 返回true：行动完成，消耗回合；false：执行失败，不消耗回合
bool doPlayerAction(BattleAction action, Pet& our, Pet& enemy, Player& player, Pet*& outActivePet)
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
        // 能量不足判断放在data层useSkill3内部，这里只管调用
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
        return battleSwitchPet(idx, player, outActivePet);
    }
    case ACT_ESCAPE:
        return battleTryEscape();
    default:
        return false;
    }
}

// 敌方AI随机选择1‑3技能行动
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

// 使用背包道具：只实现回血，完全调用player公开接口
bool battleUseItem(int itemIdx, Pet& target, Pet& /*enemy*/, Player& player)
{
    const std::vector<Item>& bag = player.getBag();
    // 下标合法性校验
    if (itemIdx < 0 || itemIdx >= static_cast<int>(bag.size()))
    {
        std::cout << "\n错误：背包下标无效！\n";
        msleep(700);
        return false;
    }
    const Item& item = bag[itemIdx];
    // consumeItem按名字消耗，内部处理数量扣减、自动删除空条目
    bool ok = player.consumeItem(item.name, 1);
    if (!ok)
    {
        std::cout << "\n道具消耗失败！\n";
        msleep(700);
        return false;
    }
    target.heal(item.effectValue);
    std::cout << "\n使用【" << item.name << "】，恢复 " << item.effectValue << " HP！\n";
    msleep(700);
    return true;
}

// 切换出战宠物；不使用const_cast，通过非const迭代器获取指针
bool battleSwitchPet(int teamIdx, Player& player, Pet*& outActivePet)
{
    const std::vector<Pet>& team = player.getTeam();
    if (teamIdx < 0 || teamIdx >= static_cast<int>(team.size()))
    {
        std::cout << "\n错误：队伍下标超出范围！\n";
        msleep(700);
        return false;
    }

    int realIdx = player.findPetIndex(team[teamIdx].getName());
    if (realIdx == -1)
    {
        std::cout << "\n找不到该宠物\n";
        msleep(700);
        return false;
    }
    Pet* targetPet = nullptr;
    {
        int i = 0;
        for (auto it = player.teamBegin(); it != player.teamEnd(); ++it, ++i)
        {
            if (i == realIdx)
            {
                targetPet = &(*it);
                break;
            }
        }
    }
    if (targetPet == nullptr)
    {
        std::cout << "\n获取宠物指针失败\n";
        msleep(700);
        return false;
    }
    // 不能切换死亡宠物
    if (!targetPet->alive())
    {
        std::cout << "\n该宠物已经阵亡，无法出战！\n";
        msleep(700);
        return false;
    }
    // 不能切换当前已经出战的宠物
    if (outActivePet == targetPet)
    {
        std::cout << "\n已经是该宠物出战！\n";
        msleep(700);
        return false;
    }

    outActivePet = targetPet;
    std::cout << "\n切换出战宠物 → " << targetPet->getName() << "\n";
    msleep(700);
    return true;
}

// 逃跑概率35%成功
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

// 战斗结算：胜利给经验；逃跑不清除buff，胜利/失败清除双方buff
void battleSettle(BattleResult res, Pet& our, Pet& enemy, Player& player)
{
    if (res != BATTLE_ESCAPE)
    {
        our.resetBuff();
        enemy.resetBuff();
    }
    if (res == BATTLE_WIN)
    {
        std::cout << "\n>>>>战斗胜利<<<<\n";
        int expGain = enemy.getLevel() * 25;
        our.addExp(expGain);
        std::cout << our.getName() << " 获得 " << expGain << " 经验！\n";
    }
    else if (res == BATTLE_LOSE)
    {
        std::cout << "\n>>>>战斗失败<<<<\n";
    }
    msleep(1200);
}

BattleResult battle(Pet& our, Pet& enemy, Player& player)
{
    Pet* activePet = &our;
    while (true)
    {
        // 空指针防御
        if (activePet == nullptr)
        {
            battleSettle(BATTLE_LOSE, our, enemy, player);
            return BATTLE_LOSE;
        }

        drawBattleUI(*activePet, enemy, player);
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

        bool ret = doPlayerAction(act, *activePet, enemy, player, activePet);

        // 逃跑成功直接结束战斗
        if (act == ACT_ESCAPE && ret == true)
        {
            battleSettle(BATTLE_ESCAPE, *activePet, enemy, player);
            return BATTLE_ESCAPE;
        }

        if (ret && (act == ACT_SWITCH_PET || act == ACT_USE_ITEM))
        {
            // 换宠、使用道具：属于完整回合，不攻击，直接能量+1，敌方行动
            activePet->gainEnergy(1);
            enemy.gainEnergy(1);
            enemyAiTurn(enemy, *activePet, player);
        }
        else if (ret)
        {
            // 释放技能：我方打完，先看敌方是否死亡
            if (!enemy.alive())
            {
                battleSettle(BATTLE_WIN, *activePet, enemy, player);
                return BATTLE_WIN;
            }
            activePet->gainEnergy(1);
            enemy.gainEnergy(1);
            enemyAiTurn(enemy, *activePet, player);
        }
        // ret==false：行动失败，不消耗回合，直接向下走到阵亡判断，回到循环开头

        // 我方出战宠物阵亡，尝试自动切换队伍下一只存活宠物
        if (!activePet->alive())
        {
            std::cout << "\n" << activePet->getName() << " 倒下了！\n";
            msleep(800);
            int curIdx = player.findPetIndex(activePet->getName());
            Pet* nextAlive = player.getNextAlivePet(curIdx);
            if (nextAlive != nullptr)
            {
                std::cout << "派出 " << nextAlive->getName() << "！\n";
                activePet = nextAlive;
                msleep(800);
                continue;
            }
            else
            {
                // 全队没有存活宠物，战斗失败
                battleSettle(BATTLE_LOSE, *activePet, enemy, player);
                return BATTLE_LOSE;
            }
        }
    }
}