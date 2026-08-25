#include <iostream>
#include "battle.h"
#include "data.h"
#include "save.h"
#include "tower.h"

using namespace std;

//==================== 剧情输出函数，仅main.cpp内部使用 static ====================
static void showPrologueStory()
{
    cout << "\n==================================================" << endl;
    cout << "                    序章：村落的嘱托                " << endl;
    cout << "==================================================\n" << endl;

    cout << "在宁静的元素村落中，火、水、草三种元素相融相生，滋养着整片大地，人类与元神精灵世代和睦共处。\n";
    cout << "你机缘巧合来到此地，遇见一位白发苍苍的老者，他便是守护村落的元素长老。\n\n";

    cout << "见你初来此地、对这片大陆一无所知，长老缓缓向你诉说尘封已久的往事：\n\n";

    cout << R"("很久以前，世间由元素之神执掌，火、水、草三元素循环制衡，维系着整片大陆的安宁。
可自邪祟渊主诞生之后，元素之神的力量被撕裂崩坏，神力化作三块珍贵的元神碎片，
分别封存于火焰高塔、碧波水塔、青森草塔之中。

如今渊主四处侵蚀三座高塔，妄图夺走全部元神碎片。一旦让他得逞，烈火将焚尽万林，
洪水将淹没大地，世间草木尽数凋零，整个世界将彻底覆灭。

早在多年前，渊主便降下封印，村落族人尽数被剥夺元神之力，再也无法守护这片土地。
少年，我将仅剩的伙伴水汐蜥托付于你，愿你携它踏遍三塔，夺回元神碎片，
斩断渊主的阴谋，拯救濒临崩坏的元素大陆。")" << "\n\n";

    cout << "听罢长老的嘱托，你肩负起整片大陆的希望，带着初始伙伴水汐蜥，\n";
    cout << "毅然离开了安稳的村落，踏上了闯塔寻碎片、对抗渊主的冒险之路。\n";
    cout << "==================================================\n";
    cout << "按下回车键继续...";
    cin.get();
    cin.get();
}

// 火焰高塔剧情
static void showTower1Story()
{
    cout << "\n==================================================" << endl;
    cout << "                第一座塔：火焰高塔                  " << endl;
    cout << "==================================================\n";
    cout << "火焰高塔，大地熔岩翻涌。渊主的力量渗透进来，守塔元神被怒火侵蚀，性情狂暴。\n";
    cout << "塔底Boss：炎狱狂狮（火系Boss）被渊主的怨气蛊惑，陷入狂暴。\n";
    cout << "【被动】血量低于30%触发【火狂暴】，自身攻击力暴涨。\n";
    cout << "\n即将开启火焰高塔的战斗……\n";
    cout << "按下回车键继续...";
    cin.get();
    cin.get();
}

// 火焰高塔通关后剧情
static void showTower1EndStory()
{
    cout << "\n>>> 击败炎狱狂狮！自动捕获这只Boss元神，获得【火之碎片】！\n\n";
    cout << "NPC残魂苏醒：\"渊主已经去往碧波水塔，小心它的力量……\"\n";
    cout << "按下回车键继续...";
    cin.get();
    cin.get();
}

// 碧波水塔剧情
static void showTower2Story()
{
    cout << "\n==================================================" << endl;
    cout << "                第二座塔：碧波水塔                  " << endl;
    cout << "==================================================\n";
    cout << "巨大水下高塔，永不停歇的波涛。渊主把绝望情绪灌入守塔元神。\n";
    cout << "塔底Boss：深渊海灵（水系Boss）。\n";
    cout << "【被动】Boss每回合回复15点HP。它被渊主蛊惑，认为只有洪水毁灭一切才是解脱。\n";
    cout << "\n即将开启碧波水塔的战斗……\n";
    cout << "按下回车键继续...";
    cin.get();
    cin.get();
}

// 碧波水塔通关后剧情
static void showTower2EndStory()
{
    cout << "\n>>> 击败深渊海灵！自动捕获Boss元神，获得【水之碎片】！\n\n";
    cout << "碎片发出微光，提示你前往最后一座：青森草塔。渊主本人已经亲临那里。\n";
    cout << "按下回车键继续...";
    cin.get();
    cin.get();
}

// 青森草塔剧情
static void showTower3Story()
{
    cout << "\n==================================================" << endl;
    cout << "                第三座塔：青森草塔                 " << endl;
    cout << "==================================================\n";
    cout << "茂密巨树构建的高塔。草木本该孕育生机，渊主在这里扭曲草系元神，让草木陷入病态。\n";
    cout << "塔底Boss：枯木古尊（草系Boss）。\n";
    cout << "【被动】【草系禁锢】，会跳过我方一回合行动。古尊被渊主欺骗，相信“毁灭才是新生”。\n";
    cout << "\n即将开启青森草塔的战斗……\n";
    cout << "按下回车键继续...";
    cin.get();
    cin.get();
}

// 青森草塔通关后，渊主登场剧情
static void showTower3EndAndFinalBossStory()
{
    cout << "\n>>> 击败枯木古尊！自动捕获Boss元神，获得【草之碎片】！\n\n";
    cout << "三块碎片汇聚在一起，渊主现身！\n\n";
    cout << R"(渊主："可笑，你收集碎片，却不知道这些本就是我的力量。
交出碎片，否则我将亲手终结这个世界。")" << "\n\n";
    cout << "三块元神碎片融合，渊主借用三元素碎片力量变身，开启最终决战！\n";
    cout << "【渊主】融合火水草三元素力量的终极Boss，会随机触发：火狂暴、水系回血、草禁锢跳过回合。\n";
    cout << "\n即将开启最终决战！\n";
    cout << "按下回车键继续...";
    cin.get();
    cin.get();
}

// 主菜单打印
static void printMainMenu()
{
    cout << "\n========== 塔防游戏 主菜单 ==========\n";
    cout << " 1. 开始新游戏（从头推进剧情）\n";
    cout << " 2. 查看玩家信息数据\n";
    cout << " 3. 塔防游戏对局\n";
    cout << " 4. 保存游戏存档\n";
    cout << " 5. 读取游戏存档\n";
    cout << " 0. 退出游戏\n";
    cout << "=====================================\n";
    cout << "请输入功能编号：";
}

static void printBattleMenu()
{
    cout << "\n---------- 战斗系统 ----------\n";
    cout << "1. 开始一场战斗\n";
    cout << "2. 查看战斗日志\n";
    cout << "0. 返回上一级菜单\n";
    cout << "请选择：";
}

static void printTowerMenu()
{
    cout << "\n---------- 塔防对局 ----------\n";
    cout << "1. 开启塔防对局\n";
    cout << "2. 查看塔防状态\n";
    cout << "0. 返回上一级菜单\n";
    cout << "请选择：";
}

static void battleSubLoop()
{
    int op;
    while (true)
    {
        printBattleMenu();
        cin >> op;
        if (!cin)
        {
            cin.clear();
            cin.ignore(1024, '\n');
            cout << "输入非法，请输入数字！\n";
            continue;
        }
        switch (op)
        {
        case 1:
            battleStart();
            break;
        case 2:
            battleShowLog();
            break;
        case 0:
            return;
        default:
            cout << "无效选项，请重新选择。\n";
            break;
        }
    }
}

static void towerSubLoop()
{
    int op;
    while (true)
    {
        printTowerMenu();
        cin >> op;
        if (!cin)
        {
            cin.clear();
            cin.ignore(1024, '\n');
            cout << "输入非法，请输入数字！\n";
            continue;
        }
        switch (op)
        {
        case 1:
            towerGameStart();
            break;
        case 2:
            towerShowStatus();
            break;
        case 0:
            return;
        default:
            cout << "无效选项，请重新选择。\n";
            break;
        }
    }
}

//==================== 完整新游戏剧情流程入口 ====================
static void startNewGameStoryFlow()
{
    // 1.序章
    showPrologueStory();

    // 2.火焰高塔第1关剧情 + 调用tower模块开启第一塔
    showTower1Story();
    towerStartLevel1();    // tower模块接口：火焰高塔
    showTower1EndStory();

    // 3.碧波水塔第2关
    showTower2Story();
    towerStartLevel2();    // tower模块接口：碧波水塔
    showTower2EndStory();

    // 4.青森草塔第3关
    showTower3Story();
    towerStartLevel3();    // tower模块接口：青森草塔
    showTower3EndAndFinalBossStory();

    // 5.最终决战，调用tower/battle最终Boss接口
    towerStartFinalBoss();
    cout << "\n>>> 主线剧情全部结束！\n";
}

int main()
{
    int select;
    cout << "========== 塔防游戏 程序启动 ==========\n";

    while (true)
    {
        printMainMenu();
        cin >> select;

        if (!cin)
        {
            cin.clear();
            cin.ignore(1024, '\n');
            cout << "\n输入格式错误，请输入数字编号！\n";
            continue;
        }

        switch (select)
        {
        case 1:
            // 开始新游戏：完整剧情流程
            startNewGameStoryFlow();
            break;
        case 2:
            dataShowPlayerInfo();
            break;
        case 3:
            towerSubLoop();
            break;
        case 4:
            saveGame();
            cout << ">> 已执行存档保存\n";
            break;
        case 5:
            loadGame();
            cout << ">> 已执行存档读取\n";
            break;
        case 0:
            cout << "\n游戏退出，感谢游玩。\n";
            return 0;
        default:
            cout << "\n无效的菜单编号，请重新输入！\n";
            break;
        }
    }
    return 0;
}
