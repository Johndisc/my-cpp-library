#include <algorithm>
#include <iostream>
#include <mutex>
#include <queue>
#include <stack>
#include <fstream>

#define BDFS_MAX_DEPTH 10

using namespace std;

struct Edge1 {
    int u;
    int v;
    Edge1(){ u = v = 0; }
    Edge1(int u,int v):u(u),v(v){}
};

vector<int> offset{0,3,4,7,9,10,11,13,14,16,17,18,19};
vector<int> neighbor{4,8,10, 7, 5,9,11, 0,7, 3, 9, 2,10, 11, 6,10, 1, 3, 1};
vector<bool> active_bits(12, true);

bool is_end;
int current_vid, last_vid, cur_depth, order, current_depth;
stack<int> dfs_stack;
queue<Edge1> FIFO;

int scan()
{
    for (int i = current_vid; i < last_vid; i++) {
        if (active_bits[i]) {
            active_bits[i] = false;
//            cout << i << endl;
            dfs_stack.push(i);
            return i;
        }
    }
    return -1;
}

void putsEdge(Edge1 edge)
{
    printf("%d: (%d, %d)\n", order++, edge.u, edge.v);
}

int fetch_neighbor(int cid)
{
    int start_offset, end_offset;
    start_offset = offset[cid];
    end_offset = offset[cid + 1];
    for (int i = start_offset; i < end_offset; ++i) {
        if (active_bits[neighbor[i]])
            return neighbor[i];
    }
    return -1;
}

void rebdfs(int cid, int depth) {
    int start_offset = offset[cid];
    int end_offset = offset[cid + 1];
    for (int i = start_offset; i < end_offset; ++i) {
        putsEdge(Edge1(cid, neighbor[i]));
        if (active_bits[neighbor[i]] && depth < BDFS_MAX_DEPTH) {
            active_bits[neighbor[i]] = false;
            rebdfs(neighbor[i], depth + 1);
        }
    }
}

void bdfs()
{
    int start_offset, end_offset;
    bool depin;
    Edge1 edge;
    int cid, nb, src;
    cur_depth = 1;
    while (!dfs_stack.empty())
    {
        cid = dfs_stack.top();
        start_offset = offset[cid];
        end_offset = offset[cid + 1];
        for (int i = start_offset; i < end_offset; ++i) {
            if (active_bits[neighbor[i]])
                dfs_stack.push(nb);
        }
        if (nb != -1) {
            dfs_stack.push(nb);
            putsEdge(Edge1(cid, nb));
        }
        depin = false;
        for (int i = start_offset; i < end_offset; ++i) {
            if (cur_depth < BDFS_MAX_DEPTH && active_bits[neighbor[i]]) {     //只入队，不遍历
                active_bits[neighbor[i]] = false;
//                dfs_stack.push(Edge(edge.v, neighbor[i]));
                depin = true;
            }
            else
                putsEdge(Edge1(edge.v, neighbor[i]));
        }
        if (depin)
            cur_depth++;
        else
            cur_depth--;
    }
}

void runBDFS()
{
    order = current_vid = 0;
    last_vid = 12;
    is_end = false;
    int vid = scan();
    while (vid != -1) {
        rebdfs(vid, 0);
        vid = scan();
    }
}

void parse()
{
    string filename = "../com-lj.ungraph.txt";
    ifstream ifs;
    ifs.open(filename, ios::in);
    string s;
    for (int i = 0; i < 2; ++i) {
        getline(ifs, s);
    }
    getline(ifs, s);
    unsigned pos = s.find(' ');
    vector <string> split;
    while (pos != string::npos) {
        split.push_back(s.substr(0, pos));
        s = s.substr(pos + 1);
        pos = s.find(' ');
    }
    split.push_back(s);
    int vnum = stoi(split[2]);
    int e_num = stoi(split[4]);
    cout << vnum << " " << e_num << endl;

    getline(ifs, s);

    ifs.close();
}