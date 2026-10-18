// ============================================================================
//  render.cpp —— 画面输出
//
//  只负责"把状态画成字符"。不修改任何游戏状态。
// ============================================================================

#include "snake/render.hpp"

#include <iostream>

#include "snake/game.hpp"

// ---- 画一格：这一格到底该显示什么？ ----------------------------------------
// ★ 判断顺序 = 优先级：越特殊的规则越靠前。
//   "蛇头"比"蛇身"特殊，所以蛇头必须【在 for 循环之前】判断，
//   否则蛇头会被"是蛇身吗"这一关先拦下，永远显示不出 H。
std::string cellAt(int x, int y) {
    if (x == g_food.first && y == g_food.second) {
        return " *";   // ① 食物
    }
    if (x == g_snake[0].first && y == g_snake[0].second) {
        return " H";   // ② 蛇头（必须是 g_snake[0]）
    }
    for (const auto& segment : g_snake) {
        if (x == segment.first && y == segment.second) {
            return " o";   // ③ 蛇身
        }
    }
    return " .";   // ④ 空地
}

// ---- 把整帧画面拼成一个字符串 ----------------------------------------------
// 为什么不直接逐格 cout：
//   终端只能从上往下一行行地画，一帧要几十毫秒；
//   眼睛看到"上半截是新画面、下半截还是旧的" —— 这就是"跳"。
std::string render() {
    std::string buf;
    buf.reserve((kWidth * 2 + 1) * (kHeight + 1));   // 多一行放分数

    // 第一行画分数（结尾补空格，防止分数变化时留下残影）
    buf += "Score: " + std::to_string(g_score) + "    \n";

    for (int y = 0; y < kHeight; ++y) {
        for (int x = 0; x < kWidth; ++x) {
            buf += cellAt(x, y);         // 攒进字符串，先不输出
        }
        if (y != kHeight - 1) {
            buf += '\n';                 // 最后一行不加换行，防止整屏向上滚动
        }
    }
    return buf;
}

// ---- 输出到屏幕：光标归位 + 整帧覆盖 ---------------------------------------
void renderFrame() {
    std::cout << "\033[H" << render();   // \033[H = 光标回到左上角
    std::cout.flush();                   // 立刻真正写出去，别留在缓冲区
}
