#include "data.h"
#include <cstdlib>
#include <algorithm>
#include <cmath>

using namespace std;

// ============================================================
// Pet 类实现
// ============================================================

// Pet构造函数：初始化宠物全部数据
Pet::Pet(const string& name, const string& type, int level,
         const string& s1, const string& s2, const string& s3,
         int hp, int maxHp, int attack, int dodging,
         int skill1EffectValue, int skill2EffectValue, int skill3EffectValue)
    : name(name), type(type), level(level),
      skillName1(s1), skillName2(s2), skillName3(s3),
      skillLevel1(1), skillLevel2(1), skillLevel3(1),
      exp(0),
      hp(hp), maxHp(maxHp),
      attack(attack), dodging(dodging),
      skill1EffectValue(skill1EffectValue),
      skill2EffectValue(skill2EffectValue),
      skill3EffectValue(skill3EffectValue),
      energyPoints(0), tempAttackBuff(0), tempDodgeBuff(0), isAlive(true)
{ }

// ----- Getter -----
string Pet::getName() const { return name; }
string Pet::getType() const { return type; }
int Pet::getLevel() const { return level; }
int Pet::getSkillLevel1() const { return skillLevel1; }
int Pet::getSkillLevel2() const { return skillLevel2; }
int Pet::getSkillLevel3() const { return skillLevel3; }
int Pet::getExp() const { return exp; }
int Pet::getHp() const { return hp; }
int Pet::getMaxHp() const { return maxHp; }
int Pet::getSkill1EffectValue() const { return skill1EffectValue; }
int Pet::getSkill2EffectValue() const { return skill2EffectValue; }
int Pet::getSkill3EffectValue() const { return skill3EffectValue; }
int Pet::getEnergyPoints() const { return energyPoints; }
bool Pet::alive() const { return isAlive; }

// ----- 技能等级提升（上限5）-----
void Pet::skill1LevelUp() { if (skillLevel1 < 5) ++skillLevel1; }
void Pet::skill2LevelUp() { if (skillLevel2 < 5) ++skillLevel2; }
void Pet::skill3LevelUp() { if (skillLevel3 < 5) ++skillLevel3; }

// ----- 受伤 -----
void // 接口：宠物受到伤害
Pet::takeDamage(int damage) {
    if (damage < 0) return;
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        isAlive = false;
    }
}

// ----- 治疗 -----
void // 接口：宠物回血
Pet::heal(int amount) {
    if (amount < 0) return;
    hp += amount;
    if (hp > maxHp) hp = maxHp;
    if (hp > 0) isAlive = true;
}

// ----- 增加经验 -----
void // 接口：增加经验并处理升级
Pet::addExp(int exp) {
    this->exp += exp;
    while (this->exp >= expForLevel(level) && level < 100) {
        this->exp -= expForLevel(level);
        levelUp();
    }
}

// ----- 升级 -----
void Pet::levelUp() {
    if (level >= 100) return;
    ++level;
    maxHp += 12;
    attack += 4;
    dodging += 2;
    hp = maxHp;
    exp = 0;
}

// ----- 重置临时增益 -----
void Pet::resetBuff() {
    tempAttackBuff = 0;
    tempDodgeBuff = 0;
}

// ----- 能量操作 -----
void Pet::gainEnergy(int amount) {
    if (amount > 0) energyPoints += amount;
}

void Pet::reduceEnergy(int amount) {
    if (amount > 0) {
        energyPoints -= amount;
        if (energyPoints < 0) energyPoints = 0;
    }
}

// ----- 临时增益修改 -----
void Pet::addTempAttackBuff(int amount) {
    if (amount > 0) tempAttackBuff += amount;
}

void Pet::addTempDodgeBuff(int amount) {
    if (amount > 0) tempDodgeBuff += amount;
}

// ----- 技能1：伤害技 -----
void // 接口：使用技能1攻击
Pet::useSkill1(Pet& target) {
    int damage = // 全局接口：计算战斗伤害
calcDamage(*this, target, 1);
    target.takeDamage(damage);
}

// ----- 技能2：辅助技（火加攻%，水加闪避，草回血%）-----
void // 接口：使用技能2辅助
Pet::useSkill2(Pet& target) {
    (void)target;   // 忽略目标，只作用于自己
    if (type == "火") {
        int percent = 15 + 5 * skillLevel2;   // 20%~40%
        addTempAttackBuff(percent);
        cout << name << " 使用了 " << skillName2 << "，下一次伤害提升 " << percent << "%！" << endl;
    }
    else if (type == "水") {
        int bonus = 10 + 5 * skillLevel2;     // 15%~35%
        addTempDodgeBuff(bonus);
        cout << name << " 使用了 " << skillName2 << "，闪避率提升 " << bonus << "%！" << endl;
    }
    else if (type == "草") {
        int percent = 15 + 5 * skillLevel2;   // 20%~40%
        int healAmount = maxHp * percent / 100;
        heal(healAmount);
        cout << name << " 使用了 " << skillName2 << "，恢复了 " << healAmount << " 点生命值！" << endl;
    }
}

// ----- 技能3：消耗3能量，高伤害 -----
void // 接口：使用技能3攻击
Pet::useSkill3(Pet& target) {
    if (energyPoints < 3) {
        cout << name << " 能量不足（需要3点），无法使用 " << skillName3 << "！" << endl;
        return;
    }
    reduceEnergy(3);
    int damage = calcDamage(*this, target, 3);
    target.takeDamage(damage);
}

// ----- 静态：升级所需经验 -----
int Pet::expForLevel(int level) {
    return 100 + (level - 1) * 20;
}

// ============================================================
// Player 类实现
// ============================================================

// 玩家构造函数
Player::Player(const string& name, int startingGold)
    : name(name), gold(startingGold), currentTower(0), currentRoom(0) {
    for (int i = 0; i < 5; ++i) towerProgress[i] = 0;
}

string Player::getName() const { return name; }

// ---- 金币 ----
int Player::getGold() const { return gold; }
void Player::addGold(int amount) { if (amount > 0) gold += amount; }
bool Player::spendGold(int amount) {
    if (amount < 0 || gold < amount) return false;
    gold -= amount;
    return true;
}

// ---- 宠物 ----
void // 接口：添加宠物
Player::addPet(const Pet& pet) {
    team.push_back(pet);
}

void Player::removePet(const string& petName) {
    auto it = find_if(team.begin(), team.end(),
        [&petName](const Pet& p) { return p.getName() == petName; });
    if (it != team.end()) {
        team.erase(it);
    }
}

const vector<Pet>& Player::getTeam() const { return team; }

int Player::findPetIndex(const string& petName) const {
    auto it = find_if(team.begin(), team.end(),
        [&petName](const Pet& p) { return p.getName() == petName; });
    if (it == team.end()) return -1;
    return static_cast<int>(distance(team.begin(), it));
}

Pet* Player::getFirstAlivePet() {
    auto it = find_if(team.begin(), team.end(),
        [](const Pet& p) { return p.alive(); });
    if (it == team.end()) return nullptr;
    return &(*it);
}

Pet* Player::getNextAlivePet(int startIndex) {
    if (startIndex < 0 || startIndex >= static_cast<int>(team.size()))
        return nullptr;
    // 从 startIndex+1 开始搜索
    auto it = find_if(team.begin() + startIndex + 1, team.end(),
        [](const Pet& p) { return p.alive(); });
    if (it == team.end()) return nullptr;
    return &(*it);
}

int Player::getAlivePetCount() const {
    return count_if(team.begin(), team.end(),
        [](const Pet& p) { return p.alive(); });
}

void Player::healAllPets() {
    for_each(team.begin(), team.end(),
        [](Pet& p) { p.heal(p.getMaxHp()); });
}

void Player::resetAllPetsBuff() {
    for_each(team.begin(), team.end(),
        [](Pet& p) { p.resetBuff(); });
}

// ---- 背包 ----
const vector<Item>& Player::getBag() const { return bag; }

void // 接口：添加背包物品
Player::addItem(const Item& item) {
    auto it = find_if(bag.begin(), bag.end(),
        [&item](const Item& i) { return i.name == item.name; });
    if (it != bag.end()) {
        it->quantity += item.quantity;
    } else {
        bag.push_back(item);
    }
}

bool Player::consumeItem(const string& itemName, int count) {
    int idx = findItemIndex(itemName);
    if (idx == -1) return false;
    if (bag[idx].quantity < count) return false;
    removeItem(itemName, count);
    return true;
}

int Player::findItemIndex(const string& itemName) const {
    auto it = find_if(bag.begin(), bag.end(),
        [&itemName](const Item& i) { return i.name == itemName; });
    if (it == bag.end()) return -1;
    return static_cast<int>(distance(bag.begin(), it));
}

void Player::removeItem(const string& itemName, int count) {
    int idx = findItemIndex(itemName);
    if (idx == -1) return;
    bag[idx].quantity -= count;
    if (bag[idx].quantity <= 0) {
        bag.erase(bag.begin() + idx);
    }
}

// ---- 塔和房间 ----
int Player::getTowerProgress(int towerId) const {
    if (towerId < 1 || towerId > 4) return -1;
    return towerProgress[towerId];
}

void Player::setTowerProgress(int towerId, int progress) {
    if (towerId < 1 || towerId > 4) return;
    towerProgress[towerId] = progress;
}

int Player::getCurrentTower() const { return currentTower; }
void Player::setCurrentTower(int towerId) { currentTower = towerId; }
int Player::getCurrentRoom() const { return currentRoom; }
void Player::setCurrentRoom(int roomId) { currentRoom = roomId; }

// ---- 迭代器 ----
vector<Pet>::iterator Player::teamBegin() { return team.begin(); }
vector<Pet>::iterator Player::teamEnd() { return team.end(); }
vector<Pet>::const_iterator Player::teamBegin() const { return team.begin(); }
vector<Pet>::const_iterator Player::teamEnd() const { return team.end(); }

vector<Item>::iterator Player::bagBegin() { return bag.begin(); }
vector<Item>::iterator Player::bagEnd() { return bag.end(); }
vector<Item>::const_iterator Player::bagBegin() const { return bag.begin(); }
vector<Item>::const_iterator Player::bagEnd() const { return bag.end(); }

// ============================================================
// 全局工具函数
// ============================================================

int calcDamage(const Pet& attacker, const Pet& defender, int skillType) {
    // 1. 闪避判定：只看防守方
    int hitRate = 100 - defender.getDodging();  // getDodging 已包含临时增益
    if (hitRate > 95) hitRate = 95;
    if (hitRate < 5) hitRate = 5;
    if ((rand() % 100) >= hitRate) return 0;    // 未命中

    // 2. 基础伤害
    double base = attacker.getAttack();  // 已包含 tempAttackBuff
    if (skillType == 3) base *= 1.5;     // 技能3倍率

    // 3. 属性克制（仅看攻击方）
    const string& atkType = attacker.getType();
    const string& defType = defender.getType();
    double typeRate = 1.0;
    // 克制链：水→火，火→草，草→水
    if ((atkType == "水" && defType == "火") ||
        (atkType == "火" && defType == "草") ||
        (atkType == "草" && defType == "水")) {
        typeRate = 1.4;
    }
    // 被克制
    else if ((atkType == "火" && defType == "水") ||
             (atkType == "草" && defType == "火") ||
             (atkType == "水" && defType == "草")) {
        typeRate = 0.7;
    }
    base *= typeRate;

    // 4. 随机波动 ±5%
    int damage = static_cast<int>(base * (95 + rand() % 11) / 100);

    // 5. 保底
    if (damage < 1) damage = 1;
    return damage;
}

void // 全局接口：显示宠物血条
showHpBar(const Pet& pet) {
    const int barWidth = 20;
    int filled = static_cast<int>((double)pet.getHp() / pet.getMaxHp() * barWidth);
    if (filled < 0) filled = 0;
    if (filled > barWidth) filled = barWidth;

    string bar(barWidth, '░');
    for (int i = 0; i < filled; ++i) bar[i] = '█';

    double ratio = (double)pet.getHp() / pet.getMaxHp();
    if (ratio > 0.6) setColor(2);
    else if (ratio > 0.3) setColor(6);
    else setColor(4);

    cout << "[" << bar << "] " << pet.getHp() << "/" << pet.getMaxHp() << " HP";
    setColor(7);
    cout << endl;
}

#ifdef _WIN32
#include <windows.h>
void setColor(int colorCode) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, colorCode);
}
#else
void setColor(int colorCode) {
    cout << "\033[1;3" << colorCode << "m";
}
#endif