#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <chrono>
#include <thread>

// 跨平台清屏
#ifdef _WIN32
#define CLEAR_SCREEN system("cls")
#else
#define CLEAR_SCREEN system("clear")
#endif

// 统一毫秒延时，Windows / Mac / Linux 全部通用
void msleep(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// ========== data.h 模拟部分 ==========
#define FIRE 1
#define WATER 2
#define GRASS 3

struct Pet
{
    std::string name;
    int hp;
    int maxHp;
    int atk;
    int dodgeRate;
    int element;
    bool isBoss;
    int buff_atk;
    int expDrop;
    int goldDrop;
};

struct Player
{
    int score;
    int exp;
    int gold;
    std::vector<Pet> pets;
};

int calcDamage(const Pet& attacker, const Pet& defender)
{
    int base = attacker.atk + attacker.buff_atk;
    float mul = 1.0f;
    if(attacker.element == FIRE && defender.element == GRASS) mul = 2.0f;
    if(attacker.element == GRASS && defender.element == WATER) mul = 2.0f;
    if(attacker.element == WATER && defender.element == FIRE) mul = 2.0f;

    if(attacker.element == FIRE && defender.element == WATER) mul = 0.5f;
    if(attacker.element == GRASS && defender.element == FIRE) mul = 0.5f;
    if(attacker.element == WATER && defender.element == GRASS) mul = 0.5f;
    return static_cast<int>(base * mul);
}

int calcDamageFinal(const Pet& attacker, const Pet& defender)
{
    return calcDamage(attacker, defender) * 2;
}

// ========== battle.h 模拟部分 ==========
enum BattleResult
{
    BATTLE_WIN,
    BATTLE_LOSE,
    BATTLE_ESCAPE
};

enum BattleAction
{
    ACT_ATTACK,
    ACT_BUFF,
    ACT_FINAL_SKILL,
    ACT_USE_ITEM,
    ACT_SWITCH_PET,
    ACT_ESCAPE
};

BattleResult battle(Pet& our, Pet& enemy, Player& player);
void drawBattleUI(const Pet& our, const Pet& enemy, const Player& player);
void battleAttack(Pet& attacker, Pet& defender, Player& player);
void battleBuff(Pet& self);
bool battleFinalSkill(Pet& our, Pet& enemy, Player& player);
bool battleUseItem(Pet& our, Pet& enemy, Player& player, int itemId);
bool battleSwitchPet(Player& player, Pet*& outPet);
bool battleTryEscape(const Pet& our, const Pet& enemy);
bool checkDodge(const Pet& defender);
void triggerEnemyPassive(Pet& our, Pet& enemy, Player& player);
void petDeathHandle(Pet& pet);
void battleGiveReward(Player& player, Pet& defeatedEnemy);
void battleClearAllBuff(Pet& a, Pet& b);

// ========== battle.cpp 模拟部分 ==========
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
        msleep(600);
        return;
    }
    int dmg = calcDamage(attacker, defender);
    defender.hp -= dmg;
    std::cout << attacker.name << " 造成 " << dmg << " 伤害\n";
    msleep(600);
    if(defender.hp < 0) defender.hp = 0;
}

void battleBuff(Pet& self)
{
    self.buff_atk += 2;
    std::cout << self.name << " 攻击力提升！\n";
    msleep(600);
}

bool battleFinalSkill(Pet& our, Pet& enemy, Player& player)
{
    if(player.score < 3)
    {
        std::cout << "战绩点不足！\n";
        msleep(800);
        return false;
    }
    player.score -= 3;
    if(checkDodge(enemy))
    {
        std::cout << "敌方闪避终结技！\n";
        msleep(600);
        return true;
    }
    int dmg = calcDamageFinal(our, enemy);
    enemy.hp -= dmg;
    std::cout << "终结技！造成" << dmg << "高额伤害\n";
    msleep(600);
    if(enemy.hp <0) enemy.hp =0;
    return true;
}

bool battleUseItem(Pet& /*our*/, Pet& /*enemy*/, Player& /*player*/, int /*itemId*/)
{
    std::cout << "道具功能待实现\n";
    msleep(600);
    return true;
}

bool battleSwitchPet(Player& /*player*/, Pet*& /*outPet*/)
{
    std::cout << "切换精灵待实现\n";
    msleep(600);
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
        msleep(600);
        return true;
    }
    std::cout << "逃跑失败！\n";
    msleep(600);
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
        msleep(600);
    }
    if(enemy.element == FIRE && enemy.hp < enemy.maxHp*0.3)
    {
        enemy.atk +=3;
        std::cout << "[火狂暴]敌方攻击力暴涨!\n";
        msleep(600);
    }
    if(enemy.isBoss && enemy.element == WATER)
    {
        enemy.hp +=15;
        if(enemy.hp>enemy.maxHp) enemy.hp=enemy.maxHp;
        std::cout << "[水Boss被动]敌方回复15HP\n";
        msleep(600);
    }
}

void petDeathHandle(Pet& pet)
{
    pet.hp = 0;
    pet.buff_atk = 0;
}

void battleGiveReward(Player& player, Pet& defeatedEnemy)
{
    player.exp += defeatedEnemy.expDrop;
    player.gold += defeatedEnemy.goldDrop;
    player.score +=1;

    std::cout << "\n战斗胜利！获得经验:" << defeatedEnemy.expDrop
              << " 金币:" << defeatedEnemy.goldDrop << "\n";

    if(defeatedEnemy.isBoss)
    {
        std::cout << "✨Boss被收服，加入你的精灵队伍！\n";
        defeatedEnemy.hp = defeatedEnemy.maxHp;
        player.pets.push_back(defeatedEnemy);
    }
    msleep(1000);
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
            msleep(600);
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
            msleep(600);
            battleAttack(enemy, our, player);
        }

        if(our.hp <= 0)
        {
            std::cout << "我方精灵阵亡，战斗失败！\n";
            msleep(1000);
            petDeathHandle(our);
            battleClearAllBuff(our,enemy);
            return BATTLE_LOSE;
        }
    }
}

// ========== main测试入口 ==========
int main()
{
    Pet playerPet = {"水汐蜥",100,100,12,0,WATER,false,0,0,0};
    Pet bossPet = {"炎狱狂狮",120,120,14,0,FIRE,true,0,80,50};

    Player player{0,0,0,{}};

    std::cout << "=====预览测试：对战Boss炎狱狂狮=====\n";
    std::cout << "按回车继续...";
    std::cin.get();

    BattleResult res = battle(playerPet, bossPet, player);

    if(res == BATTLE_WIN)
    {
        std::cout << "\n>>>战斗胜利！你的队伍现有精灵数量：" << player.pets.size() << "\n";
        if(!player.pets.empty())
        {
            std::cout << "已捕获Boss名字：" << player.pets[0].name << "\n";
        }
    }
    else if(res == BATTLE_ESCAPE)
    {
        std::cout << "\n>>>你逃跑了\n";
    }
    else
    {
        std::cout << "\n>>>战斗失败\n";
    }
    std::cout << "\n按回车退出...";
    std::cin.get();
    return 0;
}