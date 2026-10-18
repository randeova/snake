// ============================================================================
//  game.cpp —— 游戏规则的具体实现
//
//  这里装着"游戏的物理定律"：食物怎么出现、蛇怎么走、什么样的格子会死。
// ============================================================================

#include "snake/game.hpp"

#include <cstdlib>   // rand()

// ---- 随机生成食物：从所有空格里挑一个 --------------------------------------
// 为什么不用"随机撞，撞到蛇就重抽"？—— 蛇很长时循环次数不可控
void spawnFood() {
    std::vector<std::pair<int, int>> freeCells;
    freeCells.reserve(kWidth * kHeight);   // 先跟系统预订这么多格子

    // 第一步：把所有没有被蛇占据的格子收集起来
    for (int y = 0; y < kHeight; ++y) {
        for (int x = 0; x < kWidth; ++x) {
            bool occupied = false;              // 先假设没有被蛇占据
            for (const auto& s : g_snake) {     // 拿这一格去蛇身上逐个查
                if (s.first == x && s.second == y) {
                    occupied = true;
                    break;                      // 已经确定被占，不用再比
                }
            }
            if (!occupied) {
                freeCells.push_back({x, y});    // 空格 → 记进列表
            }
        }
    }

    if (freeCells.empty()) {
        return;   // 棋盘被蛇占满了（理论上你赢了！）
    }

    // 第二步：在空格里随机挑一个（rand 是 int，size() 是 size_t，要转类型）
    g_food = freeCells[rand() % (int)freeCells.size()];
}

// ---- 试走一步：算出新蛇头会落在哪一格（只算，不改任何状态）------------------
std::pair<int, int> nextHead() {
    std::pair<int, int> head = g_snake[0];
    switch (g_dir) {
        case UP:    head.second -= 1; break;
        case DOWN:  head.second += 1; break;
        case LEFT:  head.first  -= 1; break;
        case RIGHT: head.first  += 1; break;
    }
    return head;
}

// ---- 这一格会让我死吗？ ----------------------------------------------------
bool isDeadly(const std::pair<int, int>& head) {
    // ① 撞墙：走出棋盘
    if (head.first < 0 || head.first >= kWidth ||
        head.second < 0 || head.second >= kHeight) {
        return true;
    }
    // ② 撞自己：从 1 开始（0 号是蛇头自己，不用比）
    for (int i = 1; i < (int)g_snake.size(); ++i) {
        if (g_snake[i] == head) {
            return true;
        }
    }
    return false;
}

// ---- 走一步（一个"回合"的全部逻辑）----------------------------------------
// 返回 false 表示这一步撞死了。
//
// 顺序不能改：先算 → 再判 → 最后才改。
// 如果先改再判，撞死时就得写"回滚"代码，很容易漏。
bool stepGame() {
    // ① 应用玩家想走的方向
    g_dir = g_want;

    // ② 算出新头会落在哪（还没真的移动）
    const std::pair<int, int> head = nextHead();

    // ③ 能不能走？不能就走到了尽头
    if (isDeadly(head)) {
        g_gameOver = true;
        return false;
    }

    // ④ 这一格是不是食物？
    const bool ate = (head.first == g_food.first && head.second == g_food.second);

    // ⑤ 头进（永远做，长度临时 +1，玩家看不到）
    g_snake.insert(g_snake.begin(), head);

    // ⑥ 尾出（没吃到才缩；吃到了就保留 → 长度 +1）
    if (!ate) {
        g_snake.pop_back();
    } else {
        g_score += 10;
        spawnFood();     // 吃掉一个，马上补一个新的
    }
    return true;
}
