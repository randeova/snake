// ============================================================================
//  input.cpp —— 键盘输入
//
//  只做"按键 → 意图"的翻译。它不移动蛇，也不画图。
// ============================================================================

#include "snake/input.hpp"

#include <conio.h>   // _kbhit() / _getch()

#include "snake/game.hpp"

bool handleInput() {
    if (!_kbhit()) {
        return false;   // 没有键，立刻返回（不阻塞）
    }

    const int key = _getch();

    switch (key) {
        case 'w': case 'W':
            if (g_dir != DOWN) g_want = UP;      // 不能直接掉头
            break;

        case 's': case 'S':
            if (g_dir != UP) g_want = DOWN;
            break;

        case 'a': case 'A':
            if (g_dir != RIGHT) g_want = LEFT;
            break;

        case 'd': case 'D':
            if (g_dir != LEFT) g_want = RIGHT;
            break;

        case 'q': case 'Q':
            return true;   // 玩家想退出
    }

    return false;
}
