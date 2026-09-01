#include "save_load.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

int safeStoi(const std::string& str, int fallback = 0) {
    try {
        std::string trimmed = str;
        trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), '\r'), trimmed.end());
        return std::stoi(trimmed);
    } catch (...) {
        return fallback;
    }
}

void saveGame(Player& player) {
    std::ofstream out(kSaveFileName);
    if (!out.is_open()) {
        std::cout << ">> 错误：无法创建存档文件" << std::endl;
        return;
    }
    out << "# CppPetGame Save v1" << std::endl;
    out << "PlayerName " << player.name << std::endl;
    out << "Gold " << player.gold << std::endl;
    out << "CurrentTower " << player.currentTower << std::endl;
    out << "CurrentRoom " << player.currentRoom << std::endl;
    out << "TowerProgress";
    for (int i = 0; i < 5; ++i)
        out << " " << player.towerProgress[i];
    out << std::endl;

    out << "BagSize " << player.bag.size() << std::endl;
    for (const auto& item : player.bag) {
        out << "Item " << item.name << " " << item.quantity << " "
            << item.effectValue << " " << item.price << std::endl;
    }

    out << "TeamSize " << player.team.size() << std::endl;
    for (const auto& pet : player.team) {
        out << "Pet " << pet.name << " " << pet.type << " " << pet.level << " "
            << pet.hp << " " << pet.maxHp << " " << pet.attack << " "
            << pet.dodging << " " << pet.skill1EffectValue << " "
            << pet.skill2EffectValue << " " << pet.skill3EffectValue << " "
            << pet.exp << " " << pet.energyPoints << " "
            << (pet.isAlive ? 1 : 0) << " "
            << pet.skillLevel1 << " " << pet.skillLevel2 << " "
            << pet.skillLevel3 << " "
            << pet.skillName1 << " " << pet.skillName2 << " "
            << pet.skillName3 << std::endl;
    }
    out.close();
    std::cout << ">> 游戏已保存。" << std::endl;
}

bool loadGame(Player& player) {
    std::ifstream in(kSaveFileName);
    if (!in.is_open()) {
        return false;
    }

    Player tempPlayer("");
    std::string line;
    int expectedBagSize = 0;
    int expectedTeamSize = 0;

    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        if (key.empty() || key[0] == '#') continue;
        try {
            if (key == "PlayerName") {
                std::string name; iss >> name;
                tempPlayer.name = name;
            }
            else if (key == "Gold") {
                std::string val; iss >> val;
                tempPlayer.gold = safeStoi(val);
            }
            else if (key == "CurrentTower") {
                std::string val; iss >> val;
                tempPlayer.currentTower = safeStoi(val);
            }
            else if (key == "CurrentRoom") {
                std::string val; iss >> val;
                tempPlayer.currentRoom = safeStoi(val);
            }
            else if (key == "TowerProgress") {
                for (int i = 0; i < 5; ++i) {
                    std::string val;
                    if (iss >> val)
                        tempPlayer.towerProgress[i] = safeStoi(val);
                }
            }
            else if (key == "BagSize") {
                std::string val; iss >> val;
                expectedBagSize = safeStoi(val);
            }
            else if (key == "Item") {
                std::string iName, iQtyStr, iEvStr, iPriceStr;
                if (iss >> iName >> iQtyStr >> iEvStr >> iPriceStr) {
                    Item item;
                    item.name = iName;
                    item.quantity = safeStoi(iQtyStr);
                    item.effectValue = safeStoi(iEvStr);
                    item.price = safeStoi(iPriceStr);
                    tempPlayer.bag.push_back(item);
                }
            }
            else if (key == "TeamSize") {
                std::string val; iss >> val;
                expectedTeamSize = safeStoi(val);
            }
            else if (key == "Pet") {
                std::string pName, pType, pLvl, pHp, pMaxHp, pAtk, pDodge;
                std::string pS1EV, pS2EV, pS3EV, pExp, pEP, pAlive;
                std::string pSL1, pSL2, pSL3;
                std::string pSN1, pSN2, pSN3;
                if (iss >> pName >> pType >> pLvl >> pHp >> pMaxHp >> pAtk >> pDodge
                        >> pS1EV >> pS2EV >> pS3EV >> pExp >> pEP >> pAlive
                        >> pSL1 >> pSL2 >> pSL3 >> pSN1 >> pSN2 >> pSN3) {
                    Pet newPet(pName, pType, safeStoi(pLvl),
                               pSN1, pSN2, pSN3,
                               safeStoi(pHp), safeStoi(pMaxHp),
                               safeStoi(pAtk), safeStoi(pDodge),
                               safeStoi(pS1EV), safeStoi(pS2EV), safeStoi(pS3EV));
                    newPet.exp = safeStoi(pExp);
                    newPet.energyPoints = safeStoi(pEP);
                    newPet.isAlive = (safeStoi(pAlive) == 1);
                    newPet.skillLevel1 = safeStoi(pSL1);
                    newPet.skillLevel2 = safeStoi(pSL2);
                    newPet.skillLevel3 = safeStoi(pSL3);
                    tempPlayer.team.push_back(newPet);
                }
            }
        } catch (...) {
            in.close();
            return false;
        }
    }
    in.close();

    if (static_cast<int>(tempPlayer.bag.size()) != expectedBagSize ||
        static_cast<int>(tempPlayer.team.size()) != expectedTeamSize) {
        return false;
    }
    if (tempPlayer.team.empty() && tempPlayer.name.empty()) {
        return false;
    }
    player = tempPlayer;
    return true;
}
