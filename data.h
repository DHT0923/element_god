#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

const int towerCount = 4;

enum towerId {
    towerNorth = 1,
    towerWest  = 2,
    towerSouth = 3,
    towerEast  = 4
};

struct Item {
    std::string name;
    int quantity;
    int effectValue;
    int price;
};

class Pet {
private:
    std::string name;
    std::string skillName1;
    std::string skillName2;
    std::string skillName3;
    std::string type;                 // "火", "水", "草"
    int level;
    int skillLevel1;                  // 1~5
    int skillLevel2;
    int skillLevel3;
    int exp;
    int hp;
    int maxHp;
    int attack;
    int dodging;                      // 基础闪避率（百分比）
    int skill1EffectValue;            // 技能1效果值（基础伤害系数）
    int skill2EffectValue;            // 技能2效果值（辅助数值基础）
    int skill3EffectValue;            // 技能3效果值（基础伤害系数）
    int energyPoints;
    int tempAttackBuff;               // 百分比，如 20 表示 +20%
    int tempDodgeBuff;                // 百分比，直接加到 dodging
    bool isAlive;

public:
    // 构造函数：所有数据由外部传入
    Pet(const std::string& name, const std::string& type, int level,
        const std::string& s1, const std::string& s2, const std::string& s3,
        int hp, int maxHp, int attack, int dodging,
        int skill1EffectValue, int skill2EffectValue, int skill3EffectValue);

    // ---- Getter ----
    std::string getName() const;
    std::string getType() const;
    std::string getSkillName1() const { return skillName1; }
    std::string getSkillName2() const { return skillName2; }
    std::string getSkillName3() const { return skillName3; }
    int getLevel() const;
    int getSkillLevel1() const;
    int getSkillLevel2() const;
    int getSkillLevel3() const;
    int getExp() const;
    int getHp() const;
    int getMaxHp() const;
    // 攻击力 = 基础攻击 * (1 + tempAttackBuff/100)
    int getAttack() const { return attack * (100 + tempAttackBuff) / 100; }
    // 闪避率 = 基础闪避 + tempDodgeBuff
    int getDodging() const { return dodging + tempDodgeBuff; }
    int getSkill1EffectValue() const;
    int getSkill2EffectValue() const;
    int getSkill3EffectValue() const;
    int getEnergyPoints() const;
    bool alive() const;

    // ---- 核心修改 ----
    void levelUp();
    void skill1LevelUp();
    void skill2LevelUp();
    void skill3LevelUp();
    void takeDamage(int damage);
    void resetBuff();
    void heal(int amount);
    void addExp(int exp);
    void useSkill1(Pet& target);
    void useSkill2(Pet& target);
    void useSkill3(Pet& target);
    void gainEnergy(int amount);
    void reduceEnergy(int amount);
    void addTempAttackBuff(int amount);   // amount 为百分比数值
    void addTempDodgeBuff(int amount);    // amount 为百分比数值

    static int expForLevel(int level);

    // 存档读档支持（成员B实现）
    friend void saveGame(Pet& pet);
    friend void loadGame(Pet& pet);
};

class Player {
private:
    std::string name;
    std::vector<Pet> team;
    std::vector<Item> bag;
    int gold;
    int currentTower;
    int currentRoom;
    int towerProgress[5];   // 下标1~4分别对应北西南东

public:
    Player(const std::string& name, int startingGold = 0);

    std::string getName() const;

    // ---- 金币 ----
    int getGold() const;
    void addGold(int amount);
    bool spendGold(int amount);

    // ---- 宠物 ----
    void addPet(const Pet& pet);
    void removePet(const std::string& petName);
    const std::vector<Pet>& getTeam() const;
    int findPetIndex(const std::string& petName) const;
    Pet* getFirstAlivePet();
    Pet* getNextAlivePet(int startIndex);   // 从 startIndex+1 开始找
    int getAlivePetCount() const;
    void healAllPets();
    void resetAllPetsBuff();

    // ---- 背包 ----
    const std::vector<Item>& getBag() const;
    void addItem(const Item& item);
    bool consumeItem(const std::string& itemName, int count = 1);
    int findItemIndex(const std::string& itemName) const;
    void removeItem(const std::string& itemName, int count = 1);

    // ---- 塔和房间 ----
    int getTowerProgress(int towerId) const;    // towerId: 1~4
    void setTowerProgress(int towerId, int progress);
    int getCurrentTower() const;
    void setCurrentTower(int towerId);
    int getCurrentRoom() const;
    void setCurrentRoom(int roomId);

    // ---- 迭代器 ----
    std::vector<Pet>::iterator teamBegin();
    std::vector<Pet>::iterator teamEnd();
    std::vector<Pet>::const_iterator teamBegin() const;
    std::vector<Pet>::const_iterator teamEnd() const;

    std::vector<Item>::iterator bagBegin();
    std::vector<Item>::iterator bagEnd();
    std::vector<Item>::const_iterator bagBegin() const;
    std::vector<Item>::const_iterator bagEnd() const;

    // 存档读档支持
    friend void saveGame(Player& player);
    friend void loadGame(Player& player);
};

// ---- 全局工具函数 ----
int calcDamage(const Pet& attacker, const Pet& defender, int skillType);
// skillType: 1=技能1, 3=技能3 (技能2不造成伤害)
void showHpBar(const Pet& pet);
void setColor(int colorCode);

#endif