#include "battle.h"
#include <iostream>
#include <random>
#include <string>

#ifdef _WIN32
#include <windows.h>
#define CLEAR_SCREEN system("cls")
#else
#define CLEAR_SCREEN system("clear")
#endif

void drawBattleUI(const Pet& our, const Pet& enemy, const Player& player)
{
    CLEAR_SCREEN;
    std::cout << "========== 回合战斗 ==========\n";

    std::cout << "【敌方】" << enemy.name << "\n";
    std::cout << "HP:" << enemy.hp << "/" << enemy.maxHp << " ";
    std::cout << "[";
    for(int i = 0; i < enemy.hp * 10 / enemy.maxHp; i++) std::cout << "#";
    for(int i = enemy.hp *10 / enemy.maxHp; i <10; i++) std::cout << "-";
    std::cout << "]\n";

    if(enemy.isBoss && (1.0 * enemy.hp / enemy.maxHp) < 0.25)
    {
        std::cout << "!!! BOSS残血警告 !!!\n";
    }

    std::cout << "-----------------------------\n";

    std::cout << "【我方】" << our.name << "\n";
    std::cout << "HP:" << our.hp << "/" << our.maxHp << " ";
    std::cout << "[";
    for(int i = 0; i < our.hp *10 / our.maxHp; i++) std::cout << "#";
    for(int i = our.hp *10 / our.maxHp; i <10; i++) std::cout << "-";
    std::cout << "]\n";

    std::cout << "战绩点：" << player.score << "\n\n";
    std::cout << "1普攻 | 2增益buff | 3终结技(消耗3战绩)\n";
    std::cout << "4道具 | 5换精灵 | 6逃跑\n";
    std::cout << "请输入操作数字：";
}

void battleAttack(Pet& attacker, Pet& defender, Player& /*player*/)
{
    if(checkDodge(defender))
    {
        std::cout << defender.name << " 闪避攻击！\n";
        Sleep(600);
        return;
    }
    int dmg = calcDamage(attacker, defender);
    defender.hp -= dmg;
    std::cout << attacker.name << " 造成 " << dmg << " 伤害\n";
    Sleep(600);
    if(defender.hp < 0) defender.hp = 0;
}

void battleBuff(Pet& self)
{
    self.buff_atk += 2;
    std::cout << self.name << " 攻击力提升！\n";
    Sleep(600);
}

bool battleFinalSkill(Pet& our, Pet& enemy, Player& player)
{
    if(player.score < 3)
    {
        std::cout << "战绩点不足！\n";
        Sleep(800);
        return false;
    }
    player.score -= 3;
    if(checkDodge(enemy))
    {
        std::cout << "敌方闪避终结技！\n";
        Sleep(600);
        return true;
    }
    int dmg = calcDamageFinal(our, enemy);
    enemy.hp -= dmg;
    std::cout << "终结技！造成" << dmg << "高额伤害\n";
    Sleep(600);
    if(enemy.hp <0) enemy.hp =0;
    return true;
}

bool battleUseItem(Pet& /*our*/, Pet& /*enemy*/, Player& /*player*/, int /*itemId*/)
{
    std::cout << "道具功能待实现\n";
    Sleep(600);
    return true;
}

bool battleSwitchPet(Player& /*player*/, Pet*& /*outPet*/)
{
    std::cout << "切换精灵待实现\n";
    Sleep(600);
    return true;
}

bool battleTryEscape(const Pet& /*our*/, const Pet& /*enemy*/)
{
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0,99);
    int roll = dist(rng);
    if(roll <40)
    {
        std::cout << "逃跑成功\n";
        Sleep(600);
        return true;
    }
    std::cout << "逃跑失败！\n";
    Sleep(600);
    return false;
}

bool checkDodge(const Pet& defender)
{
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0,99);
    int r = dist(rng);
    return r < defender.dodgeRate;
}

void triggerEnemyPassive(Pet& our, Pet& enemy, Player& /*player*/)
{
    if(enemy.element == GRASS)
    {
        std::cout << "[草系被动]我方回合被跳过!\n";
        Sleep(600);
    }
    if(enemy.element == FIRE && enemy.hp < enemy.maxHp*0.3)
    {
        enemy.atk +=3;
        std::cout << "[火狂暴]敌方攻击力暴涨!\n";
        Sleep(600);
    }
    if(enemy.isBoss && enemy.element == WATER)
    {
        enemy.hp +=15;
        if(enemy.hp>enemy.maxHp) enemy.hp=enemy.maxHp;
        std::cout << "[水Boss被动]敌方回复15HP\n";
        Sleep(600);
    }
}

void petDeathHandle(Pet& pet)
{
    pet.hp = 0;
    pet.buff_atk = 0;
}

//战斗胜利：给奖励；**如果是Boss自动捕获加入玩家精灵**
void battleGiveReward(Player& player, Pet& defeatedEnemy)
{
    player.exp += defeatedEnemy.expDrop;
    player.gold += defeatedEnemy.goldDrop;
    player.score +=1;

    std::cout << "\n战斗胜利！获得经验:" << defeatedEnemy.expDrop
              << " 金币:" << defeatedEnemy.goldDrop << "\n";

    //Boss战败自动捕获，普通怪物不捕获
    if(defeatedEnemy.isBoss)
    {
        std::cout << "✨Boss被收服，加入你的精灵队伍！\n";
        // 恢复Boss血量再加入队伍
        defeatedEnemy.hp = defeatedEnemy.maxHp;
        // 假设player.pets是std::vector<Pet>，data.h中定义
        player.pets.push_back(defeatedEnemy);
    }
    Sleep(1000);
}

void battleClearAllBuff(Pet& a, Pet& b)
{
    a.buff_atk = 0;
    b.buff_atk = 0;
}

BattleResult battle(Pet& our, Pet& enemy, Player& player)
{
    while(true)
    {
        drawBattleUI(our, enemy, player);
        int op;
        std::cin >> op;
        BattleAction act;
        switch(op)
        {
        case 1: act=ACT_ATTACK;break;
        case 2: act=ACT_BUFF;break;
        case 3: act=ACT_FINAL_SKILL;break;
        case 4: act=ACT_USE_ITEM;break;
        case 5: act=ACT_SWITCH_PET;break;
        case 6: act=ACT_ESCAPE;break;
        default:
            std::cout << "无效输入\n";
            Sleep(600);
            continue;
        }

        bool skipOurTurn = false;
        switch(act)
        {
        case ACT_ATTACK:
            battleAttack(our, enemy, player);
            player.score +=1;
            break;
        case ACT_BUFF:
            battleBuff(our);
            player.score +=1;
            break;
        case ACT_FINAL_SKILL:
            battleFinalSkill(our, enemy, player);
            break;
        case ACT_USE_ITEM:
            battleUseItem(our, enemy, player,1);
            break;
        case ACT_SWITCH_PET:
        {
            Pet* np = nullptr;
            battleSwitchPet(player, np);
            if(np) our = *np;
            break;
        }
        case ACT_ESCAPE:
            if(battleTryEscape(our,enemy))
                return BATTLE_ESCAPE;
            break;
        }

        if(enemy.hp <= 0)
        {
            petDeathHandle(enemy);
            battleGiveReward(player,enemy);
            battleClearAllBuff(our,enemy);
            return BATTLE_WIN;
        }

        triggerEnemyPassive(our,enemy,player);

        if(!skipOurTurn)
        {
            std::cout << "\n---敌方行动---\n";
            Sleep(600);
            battleAttack(enemy, our, player);
        }

        if(our.hp <= 0)
        {
            std::cout << "我方精灵阵亡，战斗失败！\n";
            Sleep(1000);
            petDeathHandle(our);
            battleClearAllBuff(our,enemy);
            return BATTLE_LOSE;
        }
    }
}