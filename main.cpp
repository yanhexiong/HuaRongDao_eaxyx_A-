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
//�������̵� �� �� ��
const int H = 5;
const int W = 4;
//����ͼ��λ������
const int y[] = { 0,100,200,300,400 };
const int x[] = { 0,100,200,300 };

/* �ƶ�����  �������� */
const int dx[] = { 0, 0, -1, 1 };
const int dy[] = { -1, 1,  0, 0 };
bool solved = false;
RECT textlocate = { 0,500,400,600 }, endlocate = {0,200,400,400};
IMAGE caocao, guanyu, machao, zu, zhaoyun, huangzhong,zhangfei;//����ͼƬ����
//C caocao
//G guanyu
//M machao
//H huangzhong
//Z zu
//Y zhaoyun
//F zhangfei
struct Block {
    //���� �� �� ����
    int width;
    int height;
    int x;
    int y;
    char t;//��������
    Block(int w, int h, int x_, int y_,char s) : width(w), height(h), x(x_), y(y_),t(s) {};
    //Block(const Block& b) = default;
    // �������Ƿ���ס(px, py)λ��
    bool isCovered(int px, int py) const {
        return x <= px && px < x + width && y <= py && py < y + height;
    };
    // ��������ĳ�����ƶ�1��
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
    }//תstring,�ͳ���������    
};

class Status {
public:
    vector<Block> blocks;
    shared_ptr<Status> prev;//ָ����һ��ͼ��
    int c_pos;//�ܲ��������е�λ��
    int step; // ����
    Status(vector<Block> bs) : blocks(bs), step(0) {
        for (int i = 0; i < bs.size(); i++) {
            if (bs[i].t == 'C') { c_pos = i; break; }
        }//��¼�ܲ������������е���ţ����ں����ж��Ƿ��ߵ��յ�
        prev = nullptr;//��ʼ��ָ�룬��ʼ���̵Ļ���ָ���ǿ�ָ��
    };
    Status(const Status& rhs) = default;
    // ����
    int get_step() const { return step; };
    // �������ܵ�״̬
    vector<shared_ptr<Status>> getNextStatus() const;
    // �ƶ���bid������dir����
    void move(int dir, int bid);
    // ��bid�����ܲ�����dir�����ƶ�
    bool canMove(int dir, int bid) const;
    // �ж���������(x,y,w,h)�Ƿ�ȫ���ǿգ�û������ռ�ݣ�����bid������֮�⣩
    int getdis()const;
    bool isRectFree(int x, int y, int w, int h, int bid) const;
    // �ж��Ƿ����
    bool is_done() const { return blocks[c_pos].x == 1 && blocks[c_pos].y == 3; };
    // ȡ�õ�ǰ״̬�ı�ʾ�������ж��Ƿ��ظ�
    string getPattern() const {
        ostringstream os;
        vector<string> out;
        for (auto s : blocks) {
            out.push_back(s.tostr());
        }
        //�ַ���������������
        sort(out.begin(), out.end());
        for (auto s : out) {os << s ;}
        return  os.str();
    };
    void showstatus() {
        //�������
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
            //�ж����ӵ����ͽ������
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
        sprintf_s(c, "��ǰ������%d", step);
        drawtext(c, &textlocate, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        EndBatchDraw();
        Sleep(500);
    }
};
int Status::getdis()const {
    //ŷ������
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
    if (newx < 0 || newy < 0) return false;//Խ���ж�
    if (newx + b.width > W || newy + b.height > H) return false;//Խ���ж� 
    return isRectFree(newx, newy, b.width, b.height, bid);//�жϵ�bid���������µ��ƶ��������Ƿ�����ƶ�����û�б��������ӵ�ס��
}
void Status::move(int dir, int bid) {
    Block& b = blocks[bid];
    b.move(dir);
    step++;//�Ӳ���
}
vector<shared_ptr<Status>> Status::getNextStatus() const {
    vector<shared_ptr<Status>> vec;
    shared_ptr<Status> now = make_shared<Status>(*this);//��¼���ڵ����
    for (int k = 0; k < blocks.size(); k++) {
        for (int dir = 0; dir < 4; dir++) {
            //�ҵ����ƶ������ӣ����������������ƶ�
            if (canMove(dir, k)) {
                //�����ƶ�
                auto ns = make_shared<Status>(*now);
                ns->move(dir, k);
                ns->prev = now;//����ָ�룬���ں����ⷨչʾ����
                vec.push_back(ns);//��ſ��е�����
            }
        }
    }
    return vec;
}
//���ȶ��бȽϺ���
struct cmp {
    bool operator()(shared_ptr<Status>& a, shared_ptr<Status>& b) const {
        //���ȳ��Ӳ����;����յ����С��
        //getstep�൱��A*��g�����յ�ľ����൱��h
        /*
        if (a->get_step() == b->get_step()) {
            return a->getdis() > b->getdis();
        }
        else {
            return  a->get_step() > b->get_step();
        }
        */
        return a->getdis() + a->get_step() > b->getdis() + b->get_step();//�Ż�
    }
};
shared_ptr<Status> final_pattern;//���ڼ�¼���Ĳ��֣�������ǰ����
void getSolution(shared_ptr<Status> init_status) {
    priority_queue<shared_ptr<Status>, vector<shared_ptr<Status>>, cmp> unprocessed;//���ȶ��д��δ��������
    unordered_set<string> pattern_processed;//��Ŵ�������������������
    unprocessed.push(init_status);
    pattern_processed.insert(init_status->getPattern());
    while (!unprocessed.empty()) {
        auto s = unprocessed.top();
        //cout<<s->getPattern();
        //cout<<s->step<<endl;
        //cout<<s.get()->get_step()<<" ";
        unprocessed.pop();
        if (s->is_done()) {
            final_pattern = s;//��¼�����������ʹ��ָ�����
            solved = true;
            cout <<"��̲�����" << s->get_step() << endl;
            break;
        }
        for (auto ns : s->getNextStatus()) {
            //ns��nextstatus��ʾ��ǰ�����Ų��ĺ�һ�����
            auto np = ns->getPattern();//��������
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
    //��ʼ��
    initgraph(400, 600);
    setbkcolor(WHITE);
    cleardevice();
    setlinestyle(PS_SOLID, 3);
    //����ͼƬ
    loadimage(&caocao, _T("./png/caocao.png"));
    loadimage(&guanyu, _T("./png/guanyu.png"));
    loadimage(&zhaoyun, _T("./png/zhaoyun.png"));
    loadimage(&zu, _T("./png/zu.png"));
    loadimage(&machao, _T("./png/machao.png"));
    loadimage(&zhangfei, _T("./png/zhangfei.png")); 
    loadimage(&huangzhong, _T("./png/huangzhong.png"));
    settextstyle(50, 0, _T("����")); // �������ִ�С������					
    settextcolor(BLACK);
    setbkmode(TRANSPARENT);
}
vector<Block> input() {
    printf("���ݵ���\n�밴���¸�ʽ�������ӣ�TXY������T�����ӵ����ͣ�X,Y���������Ͻ������̵����꣺\n");
    printf("�����¼����������ͣ�\nC �ܲ� G ���� M �� H ���� Z �� Y ���� F �ŷ�\n");
    printf("����ʾ��ͼ�뿴ͼ�λ����棺\n");
    printf("���濪ʼ���룬��������ÿո�ָ���ֻ��������һ��\n");
    initgraph(1080, 1080);
    IMAGE im;
    //˵��ͼƬ
    loadimage(&im, _T("./˵��.png"));
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
    getSolution(init_status);//���
}
void showprocess(Status bs) {
    bs.showstatus();
    if (solved) {
        vector<shared_ptr<Status>> game;
        auto a = final_pattern;
        while (a != nullptr) {
            game.push_back(a);
            a = a->prev;//�������̵�ջ
        }
        while (!game.empty()) {
            game.back()->showstatus();//�������
            //cout << game.back()->getPattern() << endl;
            game.pop_back();
        }
    }
    settextcolor(RED);
    settextstyle(80, 0, _T("����"));
    drawtext(_T("��Ϸ����"), &endlocate, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    _getch();
}
int main() {
    vector<Block> bs{};
    bs = input();//�������̲���
    startup();
    calculatesolution(bs);//�������̽ⷨ
    Status a = Status(bs);//��¼��ʼ���̣����ڳ�ʼչʾ
    showprocess(a);//չʾ�ⷨ
    return 0;
}