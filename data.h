#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

const int towerCount =  4;

enum towerId { towerNorth, towerWest, towerSouth, towerEast };

struct Item {
    std::string name;                       // 物品名称
    int quantity;                           // 数量
    int effectValue;                        // 效果值
    int price;                              // 价格
};

class Pet {
    private:
        std::string name;                   // 宠物名称
        std::string skillName1;             // 技能1名称
        std::string skillName2;             // 技能2名称
        std::string skillName3;             // 技能3名称
        std::string type;                   // 属性 "火" "水" "草"
        int level;                          // 等级
        int skillLevel1;                    // 技能1等级
        int skillLevel2;                    // 技能2等级
        int skillLevel3;                    // 技能3等级
        int exp;                            // 当前经验值
        int hp;                             // 当前生命值
        int maxHp;                          // 最大生命值
        int attack;                         // 攻击力
        int dodging;                        // 闪避率
        int skill1EffectValue;              // 技能1效果值
        int skill2EffectValue;              // 技能2效果值
        int skill3EffectValue;              // 技能3效果值
        int energyPoints;                   // 能量点数
        bool isAlive;                       // 是否存活
    
    public:
        Pet(const std::string& name, const std::string& type, int level =1);
        //--------Getter--------
        std::string getName() const;
        std::string getType() const;
        int getLevel() const;
        int getSkillLevel1() const;
        int getSkillLevel2() const;
        int getSkillLevel3() const;
        int getExp() const;
        int getHp() const;
        int getMaxHp() const;
        int getAttack() const;
        int getDodging() const;
        int getSkill1EffectValue() const;
        int getSkill2EffectValue() const;
        int getSkill3EffectValue() const;
        int getEnergyPoints() const;
        bool isAlive() const;
        //--------Setter--------
        void levelUp();
        void skill1LevelUp();
        void skill2LevelUp();
        void skill3LevelUp();
        void takeDamage(int damage);            // 受到伤害
        void resetBuff();                       // 重置增益效果
        void heal(int amount);                  // 恢复生命值
        void addExp(int exp);                   // 增加经验值
        void revive();                          // 回城后回满血
        void useSkill1(Pet& target);            // 使用技能1
        void useSkill2(Pet& target);            // 使用技能2
        void useSkill3(Pet& target);            // 使用技能3
        void gainEnergy(int amount);            // 增加能量点数
        void reduceEnergy(int amount);          // 减少能量点数

        static int expForLevel(int level);      // 计算升级所需经验值
};

class Player {
    private:
        std::string name;                       // 玩家名称
        std::vector<Pet> team;                  // 宠物队伍
        std::vector<Item> bag;                  // 背包
        int gold;                               // 金币数量
        int currentTower;                       // 当前所在塔的编号
        int currentRoom;                        // 当前所在房间的编号
        int towerProgress[4];                   // 每个塔的进度 0=未通关，1=已通关（下标0=北，1=西，2=南，3=东）
    public:
        Player()
};