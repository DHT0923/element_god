#include "save_load.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

// --- 辅助函数：安全的字符串转整数 ---
// 防止存档被手动修改成乱码导致 stoi 崩溃
int safeStoi(const std::string& str, int fallback = 0) {
    try {
        // 去除可能的回车符
        std::string trimmed = str;
        trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), '\r'), trimmed.end());
        return std::stoi(trimmed);
    } catch (...) {
        return fallback;
    }
}

//--核心实现：保存游戏 
void saveGame(Player& player){
	std::ofstream out(kSaveFileName);
	if(!out.is_open()){
		std::cout<<">>错误：无法创建存档文件"<<std::endl;
		return; 
	}
}
    //写入文件头 
    out << "# CppPetGame Save v1" << std::endl
    
    // 2. 写入 Player 基础数据
    out << "PlayerName " << player.name << std::endl;
    out << "Money " << player.money << std::endl;
    
    // 3. 写入塔进度状态 (替代旧的全局变量 northClear 等)
    // 使用 player 内部的数组和变量
    out << "CurrentTower " << player.currentTower << std::endl;
    out << "CurrentRoom " << player.currentRoom << std::endl;
    
    out << "TowerProgress " << player.towerProgress[0] << " " 
                           << player.towerProgress[1] << " " 
                           << player.towerProgress[2] << " " 
                           << player.towerProgress[3] << std::endl;
    
    
    
    // 4. 写入背包数据
    out << "BagSize " << player.bag.size() << std::endl;
    for (const auto& item : player.bag) {
        // Item 结构体假设包含 name, quantity, price 等
        out << "Item " << item.name << " " << item.quantity << " " << item.price << std::endl;
    }
    
    // 5. 写入精灵队伍数据
    out << "TeamSize " << player.team.size() << std::endl;
    for (const auto& pet : player.team) {
        // Pet 是私有成员，但本函数是 friend，所以可以直接访问
       
		//Pet [名字] [属性] [等级]
		// [HP] [MaxHP] [Attack] [Dodging] [Exp] [Alive] [技能等级1-3]
	      out << "Pet " 
            << pet.name << " " 
            << pet.type << " " 
            << pet.level << " "
            << pet.hp << " " 
            << pet.maxHp << " " 
            << pet.attack << " "
            << pet.dodging << " " // 适配字段名变更
            << pet.exp << " "
            << (pet.alive ? 1 : 0) << " " // 适配 isAlive -> alive
            << pet.skillLevel1 << " "
            << pet.skillLevel2 << " "
            << pet.skillLevel3 << std::endl;
    }

    out.close();
    std::cout << ">> 游戏已保存。" << std::endl;
}
	// --- 核心实现：读取游戏 ---
   bool loadGame(Player& player) {
    std::ifstream in(kSaveFileName);
    if (!in.is_open()) {
        return false; // 文件不存在
    }
    
    //--- 原子性加载策略 ---
    // 1. 创建临时变量存储读取的数据
    Player tempPlayer; 
    // 清空 tempPlayer 的 team 和 bag
    tempPlayer.team.clear();
    tempPlayer.bag.clear();
    tempPlayer.money = 0;
    tempPlayer.currentTower = 0;
    tempPlayer.currentRoom = 0;
    
    // 初始化塔进度为0
    for(int i=0; i<4; ++i) tempPlayer.towerProgress[i] = 0;

    std::string line;
    int lineCount = 0;
    
    // 用于校验的数据
    int expectedBagSize = 0;
    int expectedTeamSize = 0;

    while (std::getline(in, line)) {
        lineCount++;
        std::istringstream iss(line);
        std::string key;
        iss >> key;

        // 跳过注释和空行
        if (key.empty() || key[0] == '#') continue;

        try {
            if (key == "PlayerName") {
                std::string name; iss >> name;
                tempPlayer.name = name;
            } 
            else if (key == "Money") {
                std::string val; iss >> val;
                tempPlayer.money = safeStoi(val);
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
                // 读取 4 个整数
                for(int i=0; i<4; ++i) {
                    std::string val; 
                    if(iss >> val) {
                        tempPlayer.towerProgress[i] = safeStoi(val);
                    }
                }
            }
            else if (key == "BagSize") {
                std::string val; iss >> val;
                expectedBagSize = safeStoi(val);
            }
            else if (key == "Item") {
                // 读取 Item 数据
                std::string iName, iQtyStr, iPriceStr;
                if (iss >> iName >> iQtyStr >> iPriceStr) {
                    Item item;
                    item.name = iName;
                    item.quantity = safeStoi(iQtyStr);
                    item.price = safeStoi(iPriceStr);
                    tempPlayer.bag.push_back(item);
                }
            }
            else if (key == "TeamSize") {
                std::string val; iss >> val;
                expectedTeamSize = safeStoi(val);
            }
            else if (key == "Pet") {
                // 读取 Pet 数据
                // 顺序：Name Type Level HP MaxHP Atk Dodge Exp Alive S1 S2 S3
                std::string pName, pType, pLvl, pHp, pMaxHp, pAtk, pDodge, pExp, pAlive, pS1, pS2, pS3;
                if (iss >> pName >> pType >> pLvl >> pHp >> pMaxHp >> pAtk >> pDodge >> pExp >> pAlive >> pS1 >> pS2 >> pS3) {
                    
                    // 1. 先用构造函数创建对象 (必须步骤)
                    // 注意：Pet 构造函数可能需要 name, type, level
                    Pet newPet(pName, pType, safeStoi(pLvl));
                    
                    // 2. 利用 Friend 权限，手动覆盖其他私有成员
                    // 因为构造函数可能只初始化了部分值，或者我们需要恢复存档时的精确状态
                    newPet.hp = safeStoi(pHp);
                    newPet.maxHp = safeStoi(pMaxHp);
                    newPet.attack = safeStoi(pAtk);
                    newPet.dodging = safeStoi(pDodge);
                    newPet.exp = safeStoi(pExp);
                    newPet.alive = (safeStoi(pAlive) == 1);
                    newPet.skillLevel1 = safeStoi(pS1);
                    newPet.skillLevel2 = safeStoi(pS2);
                    newPet.skillLevel3 = safeStoi(pS3);
                    
                    // 3. 加入队伍
                    tempPlayer.team.push_back(newPet);
                }
            }
        } catch (...) {
            // 解析过程中出现任何异常，视为损坏
            in.close();
            return false;
        }
    }
    in.close();

    // --- 校验数据一致性 ---
    if (tempPlayer.bag.size() != expectedBagSize || tempPlayer.team.size() != expectedTeamSize) {
        return false; // 数量对不上，说明存档损坏
    }
    if (tempPlayer.team.empty() && tempPlayer.name.empty()) {
        return false; // 空存档
    }

    // --- 提交更改 ---
    // 只有全部读取并校验成功，才将 tempPlayer 的数据赋值给真正的 player
    player = tempPlayer;
    
    return true;
}
