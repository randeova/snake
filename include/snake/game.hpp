#pragma once
// ============================================================================
//  game.hpp —— 游戏世界的"数据"和"规则"
//
//  ★ 头文件里【只有声明】，没有实现。
//    函数体写在对应的 game.cpp 里。
//
//  ★ 为什么拆？有三个具体好处：
//    1. 想知道"游戏有哪些东西"，只看这一个文件就够了（它是目录）
//    2. 改画面（render.cpp）时不会碰到规则（game.cpp），改错的机会更少
//    3. 改一个 .cpp 只重编那一个文件，不用重编全部（大项目能省几十分钟）
// ============================================================================

#include <utility>
#include <vector>

// ---- 1. 常量：棋盘有多少格 ----
constexpr int kWidth  = 50;   // 横向 50 格
constexpr int kHeight = 20;   // 纵向 20 格

// ---- 2. 类型 ----
// 方向只有四种，用枚举给它们起名字
enum Direction { UP, DOWN, LEFT, RIGHT };

// ---- 3. 全局状态 ----
// ★ 这里的 inline（C++17）很关键：
//   头文件会被多个 .cpp 包含，普通全局变量会"重复定义"（LNK2005）。
//   写 inline 之后，链接器会把所有 .cpp 里的同名定义合并成一份。
//
//   传统写法是：头文件里写 extern 声明，再在一个 .cpp 里给定义。
//   两种都对，inline 更省事。
inline Direction g_dir  = RIGHT;   // 现在正在走的方向
inline Direction g_want = RIGHT;   // 玩家最后按下的方向

// 蛇身：一个"动态数组"，每个元素是一节身体的坐标。
// ★ 第 0 个元素是【蛇头】，后面依次是身子，最后一节是尾巴。
inline std::vector<std::pair<int, int>> g_snake = {{3, 7}, {3, 8}, {3, 9}, {3, 10}};

// 食物：一个坐标点
inline std::pair<int, int> g_food = {12, 5};

inline int  g_score    = 0;      // 当前分数
inline bool g_gameOver = false;  // 游戏是否结束

// ---- 4. 规则（函数体在 game.cpp 里）----
void spawnFood();                                 // 随机生成食物
std::pair<int, int> nextHead();                   // 算出新头会落在哪（只算，不改状态）
bool isDeadly(const std::pair<int, int>& head);   // 这一格会让我死吗
bool stepGame();                                  // 走一步；返回 false = 撞死了
