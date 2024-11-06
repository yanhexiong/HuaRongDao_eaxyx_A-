#include <vector>
#include <string>
#include <unordered_set>
#include <queue>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <memory>
#include <cstring>
#include <easyx.h>
#include <conio.h>
using namespace std;
//定义棋盘的 长 和 宽
const int H = 5;
const int W = 4;
//生成图像位置坐标
const int y[] = { 0,100,200,300,400 };
const int x[] = { 0,100,200,300 };

/* 移动方向  上下左右 */
const int dx[] = { 0, 0, -1, 1 };
const int dy[] = { -1, 1,  0, 0 };
bool solved = false;
RECT textlocate = { 0,500,400,600 }, endlocate = {0,200,400,400};
IMAGE caocao, guanyu, machao, zu, zhaoyun, huangzhong,zhangfei;//定义图片变量
//C caocao
//G guanyu
//M machao
//H huangzhong
//Z zu
//Y zhaoyun
//F zhangfei
struct Block {
    //棋子 长 宽 坐标
    int width;
    int height;
    int x;
    int y;
    char t;//棋子种类
    Block(int w, int h, int x_, int y_,char s) : width(w), height(h), x(x_), y(y_),t(s) {};
    //Block(const Block& b) = default;
    // 该棋子是否遮住(px, py)位置
    bool isCovered(int px, int py) const {
        return x <= px && px < x + width && y <= py && py < y + height;
    };
    // 该棋子在某方向移动1格
    void move(int dir) {
        x = x + dx[dir];
        y = y + dy[dir];
    };
    string tostr() {
        string s;
        s.push_back(t);
        s.push_back(x + '0');
        s.push_back(y + '0');
        return s;
    }//转string,送出棋子坐标    
};

class Status {
public:
    vector<Block> blocks;
    shared_ptr<Status> prev;//指向上一个图案
    int c_pos;//曹操在数组中的位置
    int step; // 步数
    Status(vector<Block> bs) : blocks(bs), step(0) {
        for (int i = 0; i < bs.size(); i++) {
            if (bs[i].t == 'C') { c_pos = i; break; }
        }//记录曹操棋子在数组中的序号，便于后面判断是否走到终点
        prev = nullptr;//初始化指针，初始棋盘的回溯指针是空指针
    };
    Status(const Status& rhs) = default;
    // 步数
    int get_step() const { return step; };
    // 后续可能的状态
    vector<shared_ptr<Status>> getNextStatus() const;
    // 移动第bid棋子在dir方向
    void move(int dir, int bid);
    // 第bid棋子能不能在dir方向移动
    bool canMove(int dir, int bid) const;
    // 判定矩形区域(x,y,w,h)是否全部是空，没有棋子占据（除第bid个棋子之外）
    int getdis()const;
    bool isRectFree(int x, int y, int w, int h, int bid) const;
    // 判定是否完成
    bool is_done() const { return blocks[c_pos].x == 1 && blocks[c_pos].y == 3; };
    // 取得当前状态的表示，用于判定是否重复
    string getPattern() const {
        ostringstream os;
        vector<string> out;
        for (auto s : blocks) {
            out.push_back(s.tostr());
        }
        //字符串排序，用于判重
        sort(out.begin(), out.end());
        for (auto s : out) {os << s ;}
        return  os.str();
    };
    void showstatus() {
        //画出棋局
        cleardevice();
        setlinestyle(PS_SOLID, 3);
        setlinecolor(BLACK);
        BeginBatchDraw();
        rectangle(0, 0, 400, 500);
        for (auto s : blocks) {
            //C caocao
            //G guanyu
            //M machao
            //H huangzhong
            //Z zu
            //Y zhaoyun
            //F zhangfei
            //判断棋子的类型进行输出
            if(s.t=='C'){ putimage(x[s.x], y[s.y], &caocao); }
            if (s.t == 'G') { putimage(x[s.x], y[s.y], &guanyu); }
            if (s.t == 'M') { putimage(x[s.x], y[s.y], &machao); }
            if (s.t == 'H') { putimage(x[s.x], y[s.y], &huangzhong); }
            if (s.t == 'Z') { putimage(x[s.x], y[s.y], &zu); }
            if (s.t == 'Y') { putimage(x[s.x], y[s.y], &zhaoyun); }
            if (s.t == 'F') { putimage(x[s.x], y[s.y], &zhangfei); }
        }
        setlinecolor(YELLOW);
        rectangle(100, 300, 300, 500);
        TCHAR c[30];
        sprintf_s(c, "当前步数：%d", step);
        drawtext(c, &textlocate, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        EndBatchDraw();
        Sleep(500);
    }
};
int Status::getdis()const {
    //欧拉距离
    return (blocks[c_pos].x - 1) * (blocks[c_pos].x - 1) + (blocks[c_pos].y - 3) * (blocks[c_pos].y - 3);
}
bool Status::isRectFree(int x, int y, int w, int h, int bid) const {
    for (int i = 0; i < w; i++)
        for (int j = 0; j < h; j++)
            for (int k = 0; k < blocks.size(); k++) {
                if (k == bid) continue;
                if (blocks[k].isCovered(x + i, y + j)) return false;
            }
    return true;
}

bool Status::canMove(int dir, int bid) const {
    Block b = blocks[bid];
    int newx = b.x + dx[dir];
    int newy = b.y + dy[dir];
    if (newx < 0 || newy < 0) return false;//越界判断
    if (newx + b.width > W || newy + b.height > H) return false;//越界判断 
    return isRectFree(newx, newy, b.width, b.height, bid);//判断第bid个棋子在新的移动方向上是否可以移动（有没有被其他棋子挡住）
}
void Status::move(int dir, int bid) {
    Block& b = blocks[bid];
    b.move(dir);
    step++;//加步数
}
vector<shared_ptr<Status>> Status::getNextStatus() const {
    vector<shared_ptr<Status>> vec;
    shared_ptr<Status> now = make_shared<Status>(*this);//记录现在的棋局
    for (int k = 0; k < blocks.size(); k++) {
        for (int dir = 0; dir < 4; dir++) {
            //找到能移动的棋子，并尝试上下左右移动
            if (canMove(dir, k)) {
                //可以移动
                auto ns = make_shared<Status>(*now);
                ns->move(dir, k);
                ns->prev = now;//回溯指针，用于后续解法展示回溯
                vec.push_back(ns);//存放可行的棋盘
            }
        }
    }
    return vec;
}
//优先队列比较函数
struct cmp {
    bool operator()(shared_ptr<Status>& a, shared_ptr<Status>& b) const {
        //优先出队步数和距离终点和最小的
        //getstep相当于A*的g，到终点的距离相当于h
        /*
        if (a->get_step() == b->get_step()) {
            return a->getdis() > b->getdis();
        }
        else {
            return  a->get_step() > b->get_step();
        }
        */
        return a->getdis() + a->get_step() > b->getdis() + b->get_step();//优化
    }
};
shared_ptr<Status> final_pattern;//用于记录最后的布局，便于往前回溯
void getSolution(shared_ptr<Status> init_status) {
    priority_queue<shared_ptr<Status>, vector<shared_ptr<Status>>, cmp> unprocessed;//优先队列存放未处理的情况
    unordered_set<string> pattern_processed;//存放处理过的情况，用于排重
    unprocessed.push(init_status);
    pattern_processed.insert(init_status->getPattern());
    while (!unprocessed.empty()) {
        auto s = unprocessed.top();
        //cout<<s->getPattern();
        //cout<<s->step<<endl;
        //cout<<s.get()->get_step()<<" ";
        unprocessed.pop();
        if (s->is_done()) {
            final_pattern = s;//记录最后结果，后面使用指针回溯
            solved = true;
            cout <<"最短步数：" << s->get_step() << endl;
            break;
        }
        for (auto ns : s->getNextStatus()) {
            //ns：nextstatus表示当前棋子排布的后一种情况
            auto np = ns->getPattern();//用于排重
            if (pattern_processed.find(np) == pattern_processed.end()) {
                unprocessed.push(ns);
                pattern_processed.insert(np);
            }
        }
    } 
    if (!solved) {//cout << "No solution.\n";cout << "0";
    }
}
void startup() {
    //初始化
    initgraph(400, 600);
    setbkcolor(WHITE);
    cleardevice();
    setlinestyle(PS_SOLID, 3);
    //加载图片
    loadimage(&caocao, _T("./png/caocao.png"));
    loadimage(&guanyu, _T("./png/guanyu.png"));
    loadimage(&zhaoyun, _T("./png/zhaoyun.png"));
    loadimage(&zu, _T("./png/zu.png"));
    loadimage(&machao, _T("./png/machao.png"));
    loadimage(&zhangfei, _T("./png/zhangfei.png")); 
    loadimage(&huangzhong, _T("./png/huangzhong.png"));
    settextstyle(50, 0, _T("宋体")); // 设置文字大小、字体					
    settextcolor(BLACK);
    setbkmode(TRANSPARENT);
}
vector<Block> input() {
    printf("华容道：\n请按以下格式输入棋子：TXY，其中T是棋子的类型，X,Y是棋子左上角在棋盘的坐标：\n");
    printf("有如下几种棋子类型：\nC 曹操 G 关羽 M 马超 H 黄忠 Z 卒 Y 赵云 F 张飞\n");
    printf("坐标示意图请看图形化界面：\n");
    printf("下面开始输入，多个棋子用空格分隔，只允许输入一行\n");
    initgraph(1080, 1080);
    IMAGE im;
    //说明图片
    loadimage(&im, _T("./说明.png"));
    putimage(0, 0, &im);
    vector<Block> bs;
    string s, st;
    getline(cin, s);
    stringstream ss(s);
    while (1) {
        string st;
        ss >> st;
        if (ss.fail()) { break; }
        //C caocao
        //G guanyu
        //M machao
        //H huangzhong
        //Z zu
        //Y zhaoyun
        //F zhangfei
        if (st[0] == 'C') { bs.push_back({ 2,2,st[1] - '0',st[2] - '0' ,'C' }); }
        if (st[0] == 'F' || st[0] == 'M' || st[0] == 'Y' || st[0] == 'H') { bs.push_back({ 1,2,st[1] - '0',st[2] - '0' ,st[0] }); }
        if (st[0] == 'G') { bs.push_back({ 2,1,st[1] - '0',st[2] - '0' ,'G' }); }
        if (st[0] == 'Z') { bs.push_back({ 1,1,st[1] - '0',st[2] - '0' ,'Z' }); }
    }
    return bs;
}
void calculatesolution(vector<Block> bs) {
    auto init_status = make_shared<Status>(bs);
    init_status->showstatus();
    getSolution(init_status);//求解
}
void showprocess(Status bs) {
    bs.showstatus();
    if (solved) {
        vector<shared_ptr<Status>> game;
        auto a = final_pattern;
        while (a != nullptr) {
            game.push_back(a);
            a = a->prev;//回溯棋盘到栈
        }
        while (!game.empty()) {
            game.back()->showstatus();//输出棋盘
            //cout << game.back()->getPattern() << endl;
            game.pop_back();
        }
    }
    settextcolor(RED);
    settextstyle(80, 0, _T("宋体"));
    drawtext(_T("游戏结束"), &endlocate, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    _getch();
}
int main() {
    vector<Block> bs{};
    bs = input();//输入棋盘布局
    startup();
    calculatesolution(bs);//计算棋盘解法
    Status a = Status(bs);//记录初始棋盘，用于初始展示
    showprocess(a);//展示解法
    return 0;
}