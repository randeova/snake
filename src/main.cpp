// ============================================================================
//  贪吃蛇 v0.5 —— 第 5 步：拆成多个文件
//
//  ★ 现在 main.cpp 只干一件事：**按顺序把各部分串起来**。
//    规则在 game.cpp、画面在 render.cpp、键盘在 input.cpp。
//
//  一个回合长这样：
//      读键盘 → 时间到了吗 → 走一步 → 重画
// ============================================================================

#include <cstdlib>   // system(), srand(), time()
#include <ctime>//time()
#include <iostream>

#include <conio.h>    // _getch()（结束时等一下按键）
#include <windows.h>  // GetTickCount / DWORD / GetStdHandle

#include "snake/game.hpp"
#include "snake/input.hpp"
#include "snake/render.hpp"

int main() {
    // ---- 0. 控制台准备：打开 ANSI 支持 + 藏光标 + 清一次屏 ----
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD  mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    std::cout << "\033[?25l";   // 隐藏那个一直闪的光标
    system("cls");              // 只在启动时清一次，之后靠"整帧覆盖"

    // ---- 1. 开局 ----
    srand((unsigned int)time(nullptr));   // 随机数播种（只播一次）
    spawnFood();                          // 先放一个食物

    DWORD lastMove = GetTickCount();      // 上一次"真的走了一步"的时刻

    // ---- 2. 主循环 ----
    while (!g_gameOver) {
        // ① 读键盘（不阻塞：没键就往下走）
        if (handleInput()) {
            std::cout << "\033[?25h";   // 恢复光标
            return 0;                   // 玩家按 Q 退出
        }

        // ② 时间没到就跳过这一轮
        const DWORD now = GetTickCount();
        if (now - lastMove < 300) {
            continue;
        }
        lastMove = now;

        // ③ 走一步（规则全在 game.cpp 里）
        if (!stepGame()) {
            break;   // 撞墙 / 撞自己
        }

        // ④ 重画（画面全在 render.cpp 里）
        renderFrame();
    }

    // ---- 3. 结束画面 ----
    std::cout << "\033[?25h";      // 恢复光标
    std::cout << "\n\n";
    std::cout << "=============== GAME OVER ===============\n";
    std::cout << "Score : " << g_score << "\n";
    std::cout << "Length: " << g_snake.size() << "\n";
    std::cout << "Press any key to exit...\n";
    _getch();
    return 0;
}
