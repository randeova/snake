// ============================================================================
//  贪吃蛇 v0.3 —— 第 3 步：键盘控制 + 画面不抖
//
//  相比 v0.2，这一版多了三件事：
//    1. 键盘控制：WASD 转向、Q 退出（_kbhit + _getch 非阻塞读键）
//    2. 禁止 180° 掉头：g_dir（正在走的）和 g_want（想走的）分离
//    3. 画面不抖：光标归位 + 整帧一次性输出，代替 system("cls") + 逐格重画
//
//  注意：这一版【还没有】边界判定和食物判定，蛇会一直往前跑出棋盘（U4 解决）。
//
//  小提示：程序输出的文字一律用英文（Windows 控制台默认 GBK，输出中文会乱码）。
// ============================================================================

#include <iostream>
#include <string>
#include <utility>
#include <vector>
//U2新添加
#include <cstdlib>    // system()  —— 清屏要用  随机种子rand()/srand()
#include <windows.h>  // GetTickCount() / DWORD / GetStdHandle —— 时间与控制台

//U3新添加
#include <conio.h>    // _kbhit() 和 _getch() 在这里（不是 windows.h）
//U4新添加
#include<ctime>//time()--给随机数播种

namespace {

// ---- 1. 游戏世界的"常量"：棋盘有多少格 ------------------------------------
const int kWidth  = 50;   // 横向 50 格
const int kHeight = 20;   // 纵向 20 格

// ---- 2. 游戏世界的"状态"：现在蛇在哪、食物在哪 -----------------------------
// 坐标用 {x, y} 表示，x 向右增大，y 向下增大。

//方向只有四种。用枚举给他们起名字
enum Direction{ UP,DOWN,LEFT,RIGHT };
Direction g_dir=RIGHT;//现在正在走的方向
Direction g_want=RIGHT;//玩家最后按下的方向
// 蛇身：一个"动态数组"，每个元素是一节身体的坐标。
// ★ 第 0 个元素是【蛇头】，后面依次是身子，最后一节是尾巴。
std::vector<std::pair<int, int>> g_snake = {{3, 7}, {3, 8}, {3, 9}, {3, 10}};

// 食物：一个坐标点。
std::pair<int, int> g_food = {12, 5};
//添加两个全局变量：分数和游戏是否结束
int g_score=0;
bool g_gameOver=false;

//从所有空格里面（除去蛇占据的）随机挑选一个当食物
//为什么随机撞或者撞到蛇就重新抽一个格了？---蛇很长时循环次数不可控
void spawnFood()
{
    std::vector<std::pair<int ,int>>freeCells;
    freeCells.reserve(kWidth*kHeight);//最多这么多格子//使用库函数先跟系统预订这么多格子
    //第一步：把所有没有被蛇占据的格子收集起来
    for(int y=0;y<kHeight;++y)
    {
        for(int x=0;x<kWidth;++x)
        {
            bool occupied=false;//先假设没有被蛇占据
            for(const auto&s:g_snake)//这里的是范围for范围就是g_snake//const只读不改变蛇，auto让系统自己推断类型，引用来拷贝
            {
                if(s.first==x&&s.second==y)
                {
                    occupied=true;
                    break;
                }
            }
            if(!occupied)
            {
                freeCells.push_back({x,y});//没有被占据就保存这个格子保存在freeCells这个盒子里面
            }
        }
    }
    if(freeCells.empty())
    {
        return ;//棋盘被蛇占满了（理论上你赢了！）
    }
    //第二步：在空格里随机挑一个作为食物的地址（rand是int,size()是size_t,要转类型）
    g_food=freeCells[rand()%(int)freeCells.size()];
}


// ---- 3. 画一格：这一格到底该显示什么？ -------------------------------------
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

//算新头+判死

//试走一步：算出新蛇头会落在哪一个（只算，不改变任何状态）
std::pair<int ,int>nextHead()
{
    std::pair<int,int>head=g_snake[0];
    switch(g_dir)
    {
        case UP:
        head.second-=1;
        break;
        case DOWN:
        head.second+=1;
        break;
        case LEFT:
        head.first-=1;
        break;
        case RIGHT:
        head.first+=1;
        break;
    }
    return head;
}
//这一格会让我死吗？
bool isDeadly(const std::pair<int ,int>&head)
{
    //1.撞墙：走出棋盘
    if(head.first<0||head.first>=kWidth||head.second<0||head.second>=kHeight)
    {
        return true;
    }
    //2.撞到自己：从1开始（0是蛇头自己，不用比）
    for(int i=1;i<(int)g_snake.size();++i)
    {
        if(g_snake[i]==head)//判断条件就是这两个盒子内的x,y是否相同
        {
            return true;
        }
    }
    return false;
}

// ---- 4. 把整帧画面拼成一个字符串（不再直接输出）----------------------------
// 为什么要"先拼好、再一次性输出"：
//   逐格 cout 会让终端从上往下一行行地画，一帧要几十毫秒；
//   你的眼睛看到的是"上半截已经是新画面、下半截还是旧的" —— 这就是"跳"。
std::string render() {
    std::string buf;
    buf.reserve((kWidth * 2 + 1) * (kHeight+1));   // 每格 2 字符，每行末尾 1 个换行    kHeight多加一行用来存储分数
    //第一行画分数（结尾补空格，防止分数变化时留下残影）
    buf+="score:"+std::to_string(g_score)+"\n";

    for (int y = 0; y < kHeight; ++y) {           // 外层循环：每一行
        for (int x = 0; x < kWidth; ++x) {        // 内层循环：这一行的每一格
            buf += cellAt(x, y);                  // 攒进字符串，先不输出
        }
        if (y != kHeight - 1) {
            buf += '\n';                          // 最后一行不加换行，防止整屏向上滚动
        }
    }
    return buf;
}

}  // namespace

// ---- 5. 程序入口：主循环（game loop） --------------------------------------
int main() {
    // ---- 0. 让控制台支持 ANSI 转义序列（画面不抖的关键之一）----
    // 光标归位 \033[H、隐藏/恢复光标 \033[?25l / \033[?25h 都要靠它。
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);   // 是 Handle（结尾小写 L）
    DWORD  mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    std::cout << "\033[?25l";   // 隐藏那个一直闪的光标（结尾是小写字母 L）
    system("cls");              // 只在启动时清一次屏，之后靠"整帧覆盖"

    // ---- 时间控制的二个变量 ----
    // lastMove：上一次"真的走了一步"的时刻（毫秒）
    // frames  ：主循环转了几圈（只用来观察，不参与游戏逻辑）
    DWORD     lastMove = GetTickCount();//DWORD是windows版的无符号整数
    long long frames   = 0;
    //随机数播种：
    srand((unsigned int)time(nullptr));
    spawnFood();//开局先放一个食物

    while (!g_gameOver) {
        //读键盘：有键就处理，没键就直接往下走
        if(_kbhit())
        {
            int key=_getch();
            switch(key)
            {
                case 'w':case 'W':
                if(g_dir!=DOWN)
                g_want=UP;//不能直接掉头
                break;

                case 's':case 'S':
                if(g_dir!=UP)
                g_want=DOWN;
                break;

                case 'a':case 'A':
                if(g_dir!=RIGHT)
                g_want=LEFT;
                break;

                case 'd':case 'D':
                if(g_dir!=LEFT)
                g_want=RIGHT;
                break;

                case 'q':case 'Q':
                std::cout << "\033[?25h";   // 恢复光标，否则这个窗口的光标会一直消失
                return 0;//退出游戏

            }
        }

        // ---- ① 检查时间：现在离上次移动够 300 毫秒了吗？----
        DWORD now = GetTickCount();
        if (now - lastMove < 300) {
            ++frames;
            continue;          // 没到时间：这一轮什么都不做，回去再问
        }
        lastMove = now;        // 时间到了：更新"上次移动"的时刻

        //走到这里，才把想要的方向“变成”正在走的方向
        g_dir=g_want;
         //先算新蛇头会落在那（还没有真正的移动）
       const std::pair<int,int>head=nextHead();
        //判断能不能走？不能走就结束
        if(isDeadly(head))
        {
            g_gameOver=true;
            break;
        }
        //这一格是不是食物？
        const bool ate=(head.first==g_food.first&&head.second==g_food.second);
           // ---- ⑥ 头进（永远做）----
           //长度临时增加1，玩家看不到，渲染在这一步之后才执行
        g_snake.insert(g_snake.begin(), head);//insert这个函数的增加方式和顺序表插入一样

        // ---- ⑦ 尾出（没吃到才缩；吃到了就保留 → 长度 +1）----
        if (!ate) {
            g_snake.pop_back();
        } else {
            g_score += 10;
            spawnFood();
        }
        // ---- ④ 重画：光标回左上角 + 整帧覆盖（不再清屏，所以不闪）----
        std::cout << "\033[H" << render();
        std::cout.flush();     // 立刻真正写出去，别留在缓冲区里

    }
     // ---- 游戏结束 ----
    std::cout << "\033[?25h";      // 恢复光标
    std::cout << "\n\n";           // 挪到棋盘下方
    std::cout << "=============== GAME OVER ===============\n";
    std::cout << "Score : " << g_score << "\n";
    std::cout << "Length: " << g_snake.size() << "\n";
    std::cout << "Press any key to exit...\n";
    _getch();
    return 0;
}
