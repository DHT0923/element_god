# 元神：元素精灵冒险

Windows / macOS 控制台回合制小游戏。主角出生自带一只水属性精灵，城镇位于世界地图中心；使用 `W`、`A`、`S`、`D` 移动至东、西、南、北四座高塔。每座塔均有可自由探索的格子地图、小怪、奇遇、宝箱、生命泉和首领。击败首领即可捕获它；没有城镇地牢。

属性链为：**水克火、火克草、草克水**。

## 编译（Windows / MinGW g++）

```bat
g++ -std=c++20 -O2 -Wall -Wextra main.cpp data.cpp save_load.cpp battle.cpp tower.cpp -o element_god.exe
element_god.exe
```

macOS：

```bash
clang++ -std=c++20 -O2 -Wall -Wextra main.cpp data.cpp save_load.cpp battle.cpp tower.cpp -o element_god
./element_god
```

建议在支持 UTF-8 的终端中运行（Windows Terminal 或先执行 `chcp 65001`）。存档会写在程序同目录的 `savegame.txt`。

## 模块分工

- `data.*`：精灵、玩家、属性克制和道具等基础数据（A）
- `save_load.*`：文本存档与读档（B）
- `battle.*`：回合制战斗（C）
- `tower.*`：四座高塔的房间和奖励（D）
- `main.cpp`：城镇菜单和总流程（E）
