#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <chrono>
#include <algorithm>
#include <random>
#include <map>
#include <set>
#include <queue>
#include <random>
#include <chrono>
#include <cmath>
#include <climits>
#include <atcoder/dsu>
#include <atcoder/mincostflow>
using namespace std;
using namespace atcoder;
using vec_int = vector<int>;
using P = pair<int, int>;
using P2 = pair<P, P>;
using T = tuple<int, int, int>;
using T2 = tuple<float, int, int>;
using T3 = tuple<float, int, int, int, int>;
using T4 = tuple<int, int, int, int>;
using T5 = tuple<int, int, int, int, int>;
using P3 = pair<int, vector<T4>>;
using TT = tuple<int, int, int>;
using ll = long long;
// using PQ = priority_queue<pair<float, int>, vector<pair<float,int>>, greater<pair<float, int>>>;
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
std::mt19937 mt{12345};
// std::uniform_int_distribution<int> dist(1, RAND_RANGE);
int RAND_ARR_LEN = 100000;
int RAND_RANGE = 1000000000;
std::uniform_int_distribution<int> dist(1, RAND_RANGE);

int N, K;
vector<vec_int> c2;
float TIME_LIMIT=3000;
bool is_dense;
vector<P> DIRS = {make_pair(0, -1), make_pair(-1, 0), make_pair(0, 1), make_pair(1, 0)};
vector<P> DIRS2 = {
    make_pair(0, -2), make_pair(-1, -1), make_pair(-2, 0), make_pair(-1, 1), make_pair(0, 2),
    make_pair(1, 1), make_pair(2, 0), make_pair(1, -1)
 };



class Rand
{
private:
    int count = 0;
    vec_int rand_arr;

public:
    Rand(){
        rep(i, RAND_ARR_LEN){
            rand_arr.push_back(dist(mt));
}
}
;
int get();
int get_rand(int from, int to);
float get_float();
}
;

int Rand::get()
{
    int num = rand_arr.at(count);
    count += 1;
    count %= RAND_ARR_LEN;
    return num;
}

int Rand::get_rand(int from, int to)
{
    int diff = to - from;
    int num = get() % diff;
    return num + from;
}

float Rand::get_float()
{
    // 0~1の間の一様乱数
    return (float)get() / (float)RAND_RANGE;
}

Rand ro;

float get_time(bool init)
{
    static std::chrono::system_clock::time_point start;
    if (init)
    {
        start = std::chrono::system_clock::now();
    }
    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); //処理に要した時間をミリ秒に変換
}


class Computer {
    public:
    int x, y;
    int id;
    int color;
    Computer(int xp, int yp, int Id, int Color){
        x = xp;
        y = yp;
        id = Id;
        color = Color;
    }
    vec_int find_neighbors(vector<vec_int> &board, vector<vec_int> &computer_id_map);
    void set_xy(int xx, int yy){x=xx; y=yy;};
    int distance_between(Computer other){
        return abs(x- other.x) + abs(y-other.y);
    };
    bool can_move(vector<vec_int> &board, vector<vec_int> &computer_id_map, vector<int> &computer_connec);
};

bool Computer::can_move(vector<vec_int> &board, vector<vec_int> &id_map, vector<int> &computer_connec){
    if(computer_connec.size()==0)return false;
    if(computer_connec.size()==1)return true;
    if(computer_connec.size()>=3)return false;
    vec_int neighbors = find_neighbors(board, id_map);
    int status = 0;
    for(auto id: computer_connec){
        rep(i, 4){
            if(neighbors.at(i)==id){
                status += (1<<i);
            }
        }
    }
    return status == 5 || status == 10;

}

vec_int Computer::find_neighbors(vector<vec_int> &board, vector<vec_int> &computer_id_map){
    vec_int result(4, -1);

    rep(kk, 4){
        int dx, dy; tie(dx, dy) = DIRS.at(kk);
        for(int i=1;i<=N;i++){
            int x2 = x+dx*i;
            int y2 = y+dy*i;
            if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)break;
            if(board.at(x2).at(y2)==0)continue;
            if(board.at(x2).at(y2)!=color)break;
            int neighbor_id = computer_id_map.at(x2).at(y2);
            result.at(kk)=neighbor_id; //方向kkの隣人はneighbor_id
            break;
        }
    }
    return result;
    //cerr<<"neighbor size:"<<neighbors.size()<<" id:"<<id<<endl;
}

vector<Computer> computers;
vector<Computer> best_computers;

class Board{
    public:
    int color;
    int second_color=-1;
    int rand_val;
    vector<vec_int> board;
    vector<vec_int> painted;
    vector<vec_int> computer_id_map;
    vector<T4> commands;
    dsu uf_tree;
    dsu uf_tree_region;
    vector<vec_int> computer_connection;
    vector<T4> other_computer_connection;

    Board(vector<vec_int> c, int Color);
    Board(){return;};
    void dfs_connect();
    void visualize();
    void cout_ans(vector<Computer> &best_computers);
    void cout_movie(vector<Computer> &best_computers);
    bool can_connect_cable(int x1, int y1, int x2, int y2);
    void paint_cable_tiles(int x1, int y1, int x2, int y2);
    void move_computer_(int x1, int y1, int x2, int y2);
    void cancel_move();
    void move_obstacle();
    void move_free_computers(int move_distance);
    void move_connected_computers(int move_distance);
    void connect_free_computers(int max_moves);
    void move_obstacle_strong(int limit, int limit_distance);
    int first_tree_commands = 0;
    P find_nearest_vacant_cell(int x, int y);
    P find_nearest_vacant_cell3(int x, int y, int x22, int y22);
    P find_nearest_vacant_cell2(int x, int y, set<P> &prohibited, vector<T4> &moves, int limit_distance);
    P find_nearest_vacant_cell4(int x, int y, set<P> &prohibited, vector<T4> &moves, int limit_distance, vector<vec_int> &painted);
    int res_ope(){return K*100-commands.size()-99;};
    void swap_pos(int x1, int y1, int x2, int y2); 
    void shrink_cluster();
    int calc_score();
    void connect_other_color(int color2);
    void gradual_ans();
    void move_computer_toward(int id1, int id2); //id1からid2の方に1マスだけ移動させる
    void resolve_separated_clusters(int max_size);
    void prepare_for_second_tree();

    // 最後の悪あがき
    int connect_other_computers();

    // 2番目の木に関係するもの
    void detect_regions();
    int connect_in_largest_region(int new_color);
    vector<Computer> second_computers; 
    vector<vec_int> computer_connection2;
    set<int> used_second_computers;
    int move_second_obstacle(int xpos, int ypos, int blocked_id, vector<Computer> &comps, vector<vec_int> &id_map, vector<vec_int> &influence_map, int new_color, vector<vec_int> &painted2);
    bool move_second_obstacle2(int xpos, int ypos, int blocked_id, vector<Computer> &comps, vector<vec_int> &id_map, vector<vec_int> &influence_map, int new_color, vector<vec_int> &painted2, int direction, set<int> &used, int limit_distance);
    bool can_connect_second_cable(int x1, int y1, int x2, int y2, vector<vec_int> &painted2);
    void paint_second_cable_tiles(int x1, int y1, int x2, int y2,  vector<vec_int> &painted2,  vector<vec_int> &influence_map);
    vec_int find_nearest_second_computers(vector<vec_int> &influence_map, int new_color, vector<vec_int> &id_map);
    vector<T4> bfs_second_nearest_influence_cell(vector<vec_int> &influence_map, int id, vector<Computer> &comps, bool crossing, vector<vec_int> &painted2);
    void move_second_computer_(int x1, int y1, int x2, int y2, int new_color, vector<vec_int> &id_map, vector<Computer> &comps);
    int move_second_obstacle_strong(int limit, int limit_distance, vector<vec_int> &painted2, set<int> &used_nodes, vector<vec_int> &id_map, vector<Computer> &comps);
    bool move_second_connected_computers(int move_distance, vector<Computer> &comps, vector<vec_int> &id_map, vector<vec_int> &second_computer_connection, dsu &second_uf_tree, vector<vec_int> &painted);
    void swap_pos2(int x1, int y1, int x2, int y2, vector<vec_int> &id_map, vector<Computer> &comps); 

    vector<int> largest_region;
    vector<vec_int> second_painted;


    // 大きな盤面限定の操作
    bool can_use_outside=true;
    void move_surrounding_computers(int region_size); //一番外側のますにあるコンピュータについて、一番外じゃないところの一番近いところに動かす
    int outside_region = -1;
    bool is_outside(int x, int y){
        int region = outside_region;
        if(x<region || x>=N-region || y<region || y>=N-region)return true;
        return false;
    }
    void arrange_outer_computers(int new_color, int ignores); // new_colorを外から2総目に、それ以外を1層目ないし3そう明光に移動させる


    // 盤面の改ざん
    int minimum_cost();
    void cout_mcf();


    private:
    void vacant_dfs(int cost, int vx, int vy, int pvx, int pvy, int computer_id, map<T4, int> &cache, vector<T4> &ans_moves, vector<T4> &moves); //cacheはvacantの位置とcomputerの位置について重複した状態が生じないように管理する
    set<P> cells_between(int id1, int id2);
    set<P> cells_between_second(int id1, int id2, vector<Computer> &comps);
    vector<T4> other_color_connections;
    void fill_vacant_at(int x, int y, int dest_x, int dest_y);


};


Board::Board(vector<vec_int> c, int Color){
    // computers = vector<Computer>(0);
    while(computers.size()>0)computers.pop_back();
    color = Color;
    board = c;
    painted = vector<vec_int>(N, vec_int(N, 0));
    second_painted = vector<vec_int>(N, vec_int(N, 0));
    computer_id_map = vector<vec_int>(N, vec_int(N, -1));

    rep(i, N){
        rep(j, N){
            if(c.at(i).at(j)==color){
                int id = computers.size();
                computer_id_map.at(i).at(j) = id;
                Computer comp = Computer(i, j, id, color);
                computers.push_back(comp);
            }
        }
    }
    uf_tree = dsu(computers.size());
    computer_connection2 = vector<vec_int>(100);
    rand_val = ro.get_rand(0, 5);
}


void Board::dfs_connect(){
    // 初期化する
    uf_tree = dsu(computers.size());
    painted = vector<vec_int>(N, vec_int(N, 0));
    computer_connection = vector<vec_int>(computers.size());

    vec_int used_computer(100, 0);
    vector<T> connect_candidates;
    rep(id, computers.size()){
        vec_int neighbors = computers.at(id).find_neighbors(board, computer_id_map);
        int x = computers.at(id).x;
        int y = computers.at(id).y;
        rep(kk, 4){
            if(neighbors.at(kk)==-1)continue;
            int id2 = neighbors.at(kk);
            int x2 = computers.at(id2).x;
            int y2 = computers.at(id2).y;
            int dist = abs(x-x2) + abs(y-y2);
            connect_candidates.push_back(make_tuple(dist, id, id2));
        }
    }
    sort(connect_candidates.begin(), connect_candidates.end());

    for(auto tmp: connect_candidates){
        int dist, id1, id2; tie(dist, id1, id2) = tmp;
        if(uf_tree.same(id1, id2))continue;
        int x1 = computers.at(id1).x;
        int y1 = computers.at(id1).y;
        int x2 = computers.at(id2).x;
        int y2 = computers.at(id2).y;
        if(!can_connect_cable(x1, y1, x2, y2))continue;

        uf_tree.merge(id1, id2);
        paint_cable_tiles(x1, y1, x2, y2);
        computer_connection.at(id1).push_back(id2);
        computer_connection.at(id2).push_back(id1);
    }
}

void Board::visualize(){
    // dfs_connect();
    cout<<N<<" "<<K<<endl;
    rep(i, N){
        rep(j, N){
            cout<<board.at(i).at(j);
        }
        cout<<endl;
    }


    cout<<100<<endl;
    rep(i, 100){
        cout<<computers.at(i).x<<" "<<computers.at(i).y<<endl;
    }
    vector<P> connections;
    rep(i, 100){
        for(auto id: computer_connection.at(i)){
            if(i<id){
                connections.push_back(make_pair(i, id));
            }
        }
    }

    cout<<connections.size()<<endl;
    rep(i, connections.size()){
        int a,b; tie(a,b) = connections.at(i);
        cout<<a<<" "<<b<<endl;
    }

    // 二色目
    cout<<second_computers.size()<<endl;
    for(auto computer: second_computers){
        cout<<computer.x<<" "<<computer.y<<endl;
    }
    vector<P> connections2;
    rep(i, second_computers.size()){
        for(auto id: computer_connection2.at(i)){
            if(i<id){
                connections2.push_back(make_pair(i, id));
            }
        }
    }

    cout<<connections2.size()<<endl;
    rep(i, connections2.size()){
        int a,b; tie(a,b) = connections2.at(i);
        cout<<a<<" "<<b<<endl;
    }


    cerr<<"commands:"<<commands.size()<<endl;
}

void Board::cout_ans(vector<Computer> &best_computers){
    int move_command_size = min(K*100, (int)commands.size());
    computers = best_computers;
    // dfs_connect();
    cout<<move_command_size<<endl;

    rep(i, move_command_size){
        int a, b, c, d; tie(a,b,c,d) = commands.at(i);
        cout<<a<<" "<<b<<" "<<c<<" "<<d<<endl;
    }

    set<P> connections;
    rep(i, 100){
        for(auto id: computer_connection.at(i)){
            if(i<id){
                connections.insert(make_pair(i, id));
            }
        }
    }
    set<P> connections2;
    rep(i, 100){
        for(auto id: computer_connection2.at(i)){
            if(i<id){
                connections2.insert(make_pair(i, id));
            }
        }
    }

    int count0 = 0;
    for(auto tmp: connections){
        int a,b; tie(a,b) = tmp;
        int x1 = best_computers.at(a).x;
        int y1 = best_computers.at(a).y;
        int x2 = best_computers.at(b).x;
        int y2 = best_computers.at(b).y;
        if(!((x1==x2||y1==y2)&&!(x1==x2&&y1==y2)))continue;
        count0++;
    }
    for(auto tmp: connections2){
        int a,b; tie(a,b) = tmp;
        int x1 = second_computers.at(a).x;
        int y1 = second_computers.at(a).y;
        int x2 = second_computers.at(b).x;
        int y2 = second_computers.at(b).y;
        if(!((x1==x2||y1==y2)&&!(x1==x2&&y1==y2)))continue;
        count0++;
    }
    for(auto tmp: other_computer_connection){
        int x1,y1,x2,y2; tie(x1,y1,x2,y2) = tmp;
        if(!((x1==x2||y1==y2)&&!(x1==x2&&y1==y2)))continue;
        count0++;
    }



    int connec_N = min(count0, K*100-move_command_size);
    cout<<connec_N<<endl;
    int count = 0;
    for(auto tmp: connections){
        int a,b; tie(a,b) = tmp;
        int x1 = best_computers.at(a).x;
        int y1 = best_computers.at(a).y;
        int x2 = best_computers.at(b).x;
        int y2 = best_computers.at(b).y;
        if(!((x1==x2||y1==y2)&&!(x1==x2&&y1==y2)))continue;
        cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
        count++;
        if(count>=connec_N)break;
    }
    int res = K*100-count-move_command_size;
    int count2=0;
    for(auto tmp: connections2){
        int a,b; tie(a,b) = tmp;
        int x1 = second_computers.at(a).x;
        int y1 = second_computers.at(a).y;
        int x2 = second_computers.at(b).x;
        int y2 = second_computers.at(b).y;
        if(!((x1==x2||y1==y2)&&!(x1==x2&&y1==y2)))continue;
        cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
        count2++;
        if(count2>=res)break;
    }
    res = K*100-count-move_command_size-count2;
    int count3 = 0;
    for(auto tmp: other_computer_connection){
        int x1,y1,x2,y2; tie(x1,y1,x2,y2) = tmp;
        if(!((x1==x2||y1==y2)&&!(x1==x2&&y1==y2)))continue;
        cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
        count3++;
        if(count3>=res)break;
    }

    cerr<<get_time(false)<<" "<<move_command_size + connec_N<<endl;
    // cerr<<"time:"<<get_time(false)<<" operation count:"<<move_command_size + connec_N<<endl;
}

void Board::cout_movie(vector<Computer> &best_computers){
    cout_ans(best_computers);
    set<P> connections;
    rep(i, 100){
        for(auto id: computer_connection.at(i)){
            if(i<id){
                connections.insert(make_pair(i, id));
            }
        }
    }
    set<P> connections2;
    rep(i, 100){
        for(auto id: computer_connection2.at(i)){
            if(i<id){
                connections2.insert(make_pair(i, id));
            }
        }
    }



    for(int i=1;i<=first_tree_commands;i++){
        cout<<i<<endl;


        rep(j, i){
            int a, b, c, d; tie(a,b,c,d) = commands.at(j);
            cout<<a<<" "<<b<<" "<<c<<" "<<d<<endl;
        }
        cout<<0<<endl;
    }

    for(int i=1;i<=connections.size();i++){
        cout<<first_tree_commands<<endl;
        rep(j, first_tree_commands){
            int a, b, c, d; tie(a,b,c,d) = commands.at(j);
            cout<<a<<" "<<b<<" "<<c<<" "<<d<<endl;
        }
        cout<<i<<endl;
        int ind = 0;
        for(auto tmp: connections){
            int a,b; tie(a,b) = tmp;
            int x1 = best_computers.at(a).x;
            int y1 = best_computers.at(a).y;
            int x2 = best_computers.at(b).x;
            int y2 = best_computers.at(b).y;
            cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
            ind++;
            if(ind==i)break;
        }
    }

    for(int i=first_tree_commands+1;i<=commands.size();i++){
        cout<<i<<endl;
        rep(j, i){
            int a, b, c, d; tie(a,b,c,d) = commands.at(j);
            cout<<a<<" "<<b<<" "<<c<<" "<<d<<endl;
        }
        cout<<connections.size()<<endl;
        for(auto tmp: connections){
            int a,b; tie(a,b) = tmp;
            int x1 = best_computers.at(a).x;
            int y1 = best_computers.at(a).y;
            int x2 = best_computers.at(b).x;
            int y2 = best_computers.at(b).y;
            cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
        }
    }

    for(int i=1;i<=connections2.size();i++){
        cout<<commands.size()<<endl;
        rep(j, commands.size()){
            int a, b, c, d; tie(a,b,c,d) = commands.at(j);
            cout<<a<<" "<<b<<" "<<c<<" "<<d<<endl;
        }

        cout<<connections.size()+i<<endl;
        for(auto tmp: connections){
            int a,b; tie(a,b) = tmp;
            int x1 = best_computers.at(a).x;
            int y1 = best_computers.at(a).y;
            int x2 = best_computers.at(b).x;
            int y2 = best_computers.at(b).y;
            cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
        }
        int ind = 0;
        for(auto tmp: connections2){
            int a,b; tie(a,b) = tmp;
            int x1 = second_computers.at(a).x;
            int y1 = second_computers.at(a).y;
            int x2 = second_computers.at(b).x;
            int y2 = second_computers.at(b).y;
            cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
            ind++;
            if(ind==i)break;
        }
    }

 


    /*
    int move_command_size = min(K*100, (int)commands.size());
    computers = best_computers;
    // dfs_connect();
    cout<<move_command_size<<endl;

    rep(i, move_command_size){
        int a, b, c, d; tie(a,b,c,d) = commands.at(i);
        cout<<a<<" "<<b<<" "<<c<<" "<<d<<endl;
    }

    set<P> connections;
    rep(i, 100){
        for(auto id: computer_connection.at(i)){
            if(i<id){
                connections.insert(make_pair(i, id));
            }
        }
    }
    set<P> connections2;
    rep(i, 100){
        for(auto id: computer_connection2.at(i)){
            if(i<id){
                connections2.insert(make_pair(i, id));
            }
        }
    }

    int connec_N = min((int)connections.size()+(int)connections2.size(), K*100-move_command_size);
    cout<<connec_N<<endl;
    int count = 0;
    for(auto tmp: connections){
        int a,b; tie(a,b) = tmp;
        int x1 = best_computers.at(a).x;
        int y1 = best_computers.at(a).y;
        int x2 = best_computers.at(b).x;
        int y2 = best_computers.at(b).y;
        cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
        count++;
        if(count>=connec_N)break;
    }
    int res = K*100-count-move_command_size;
    int count2=0;
    for(auto tmp: connections2){
        int a,b; tie(a,b) = tmp;
        int x1 = second_computers.at(a).x;
        int y1 = second_computers.at(a).y;
        int x2 = second_computers.at(b).x;
        int y2 = second_computers.at(b).y;
        cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
        count2++;
        if(count2>=res)break;
    }
    cerr<<"time:"<<get_time(false)<<" operation count:"<<move_command_size + connec_N<<endl;
    */
}

bool Board::can_connect_cable(int x1, int y1, int x2, int y2){
    if(second_color==-1){
        if(!can_use_outside&&rand_val>=1){
            if(x1==x2){
                int minval = min(x1, N-1-x1);
                if(minval<=5&&abs(y1-y2)>minval+2){
                    // cerr<<"false:"<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
                    return false;
                }
            }else if(y1==y2){
                int minval =min(y1, N-1-y1);
                // if(minval==outside_region&&abs(x1-x2)>3){
                if(minval<=5&&abs(y1-y2)>minval+2){
                    // cerr<<"false:"<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
                    return false;
                }
            }
        }

    }
    int flag = 0;
    if(x1==x2){
        int x=x1;
        int ymin = min(y1, y2);
        int ymax = max(y1, y2);
        for(int y=ymin+1;y<ymax;y++){
            if(painted.at(x).at(y)==1){
                flag=1;
                break;
            }
        }
    }else{
        int y=y1;
        int xmin = min(x1, x2);
        int xmax = max(x1, x2);
        for(int x=xmin+1;x<xmax;x++){
            if(painted.at(x).at(y)==1){
                flag=1;
                break;
            }
        }
    }
    return flag == 0;

    /* (x1, y1), (x2, y2)の間のタイルに既にケーブルが敷き詰められているかをチェックして、コンピュータをつなげるかを調べる*/
    /*
    int pos_x = x1;
    int pos_y = y1;
    int flag = 0;
    while(pos_x!=x2 || pos_y!=y2){
        pos_x += dx;
        pos_y += dy;
        if(painted.at(pos_x).at(pos_y)==1){
            flag = 1;
            break;
        }
    }
    return flag == 0;
    */
}





void Board::paint_cable_tiles(int x1, int y1, int x2, int y2){
    if(x1==x2){
        int x=x1;
        int ymin = min(y1, y2);
        int ymax = max(y1, y2);
        for(int y=ymin;y<=ymax;y++){
            painted.at(x).at(y) = 1;
        }
    }else{
        int y=y1;
        int xmin = min(x1, x2);
        int xmax = max(x1, x2);
        for(int x=xmin;x<=xmax;x++){
            painted.at(x).at(y) = 1;
        }
    }
    /*
    int pos_x = x1;
    int pos_y = y1;
    painted.at(x1).at(y1) = 1;
    while(pos_x!=x2 || pos_y!=y2){
        pos_x += dx;
        pos_y += dy;
        painted.at(pos_x).at(pos_y) = 1;
    }
    */
}


void Board::move_computer_(int x1, int y1, int x2, int y2){
    if(board.at(x1).at(y1)==0 || board.at(x2).at(y2)!=0){
        string error_message = "cannnot move! " + to_string(x1) + " "+ to_string(y1) + " " + to_string(x2) + " " + to_string(y2);
        rep(i, N){
            rep(j, N){
                cerr<<board.at(i).at(j);
            }
            cerr<<endl;
        }
        throw runtime_error(error_message);
    }
    if(board.at(x1).at(y1) == color){
        int id = computer_id_map.at(x1).at(y1);
        computer_id_map.at(x1).at(y1) = -1;
        computer_id_map.at(x2).at(y2) = id;
        computers.at(id).x = x2;
        computers.at(id).y = y2;
    }
    board.at(x2).at(y2) = board.at(x1).at(y1);
    board.at(x1).at(y1) = 0;
}

void Board::cancel_move(){
    T4 tmp = commands.at(commands.size()-1);
    int a, b, c, d; tie(a, b, c, d) = tmp;
    move_computer_(c, d, a, b);
    commands.pop_back();
}

void Board::move_obstacle(){
    vector<P> dirs = {make_pair(1, 0), make_pair(0, 1)};
    vector<T5> operation_candidates;
    rep(x, N){
        rep(y, N){
            if(board.at(x).at(y)==color||board.at(x).at(y)==0)continue;
            rep(k, 2){
                int dx, dy; tie(dx, dy) = dirs.at(k); // k=0の時: 縦方向の探索, 1の時: 横方向の探索
                int ind = 1;
                int flag1 = 0;
                int computer1 = -1;
                while(true){
                    int x2 = x + dx*ind;
                    int y2 = y + dy*ind;
                    if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)break;
                    if(painted.at(x2).at(y2)==1&&board.at(x2).at(y2)==0)break;
                    if(board.at(x2).at(y2)!=0&&board.at(x2).at(y2)!=color)break;
                    if(board.at(x2).at(y2)==color){
                        flag1 = 1;
                        computer1 = computer_id_map.at(x2).at(y2);
                        break;
                    }else{
                        ind++;
                    }
                }
                if(x==4&&y==6){
                }
                if(flag1==0)continue;

                int flag2 = 0;
                ind = -1;
                int computer2 = -1;
                while(true){
                    int x2 = x + dx*ind;
                    int y2 = y + dy*ind;
                    if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)break;
                    if(painted.at(x2).at(y2)==1&&board.at(x2).at(y2)==0)break;
                    if(board.at(x2).at(y2)!=0&&board.at(x2).at(y2)!=color)break;
                    if(board.at(x2).at(y2)==color){
                        flag2 = 1;
                        computer2 = computer_id_map.at(x2).at(y2);
                        break;
                    }else{
                        ind--;
                    }
                }
                if(flag2==0)continue;
                if(uf_tree.same(computer1, computer2))continue;
                operation_candidates.push_back(make_tuple(k, x, y, computer1, computer2));
                break;
            }
        }
    }
    // cerr<<"operation_candidates:"<<operation_candidates.size()<<endl;
    for(auto tmp: operation_candidates){
        int direc, x, y, id1, id2; tie(direc, x, y, id1, id2)= tmp;
        // cerr<<direc<<" "<<x<<" "<<y<<" "<<id1<<" "<<id2<<endl;
        // cerr<<computers.at(id1).x<<" "<<computers.at(id1).y<<endl;
        // cerr<<computers.at(id2).x<<" "<<computers.at(id2).y<<endl;
       //direc = 0の時にはコンピュータを横方向に移動させたい
       //direc = 1の時にはコンピュータを縦方向に移動させたい
       for(int i=0;i<2;i++){
           int dx, dy; tie(dx, dy) = DIRS.at(i*2+direc);
           int x2 = x + dx;
           int y2 = y + dy;
           if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)break;
           if(board.at(x2).at(y2)==0){
               move_computer_(x, y, x2, y2);
               commands.push_back(make_tuple(x, y, x2, y2));
               paint_cable_tiles(computers.at(id1).x, computers.at(id1).y, computers.at(id2).x, computers.at(id2).y);
               goto OUT;
           }
       }
       OUT:
       continue;
    }
}


void Board::move_free_computers(int move_distance){
    vector<T4> move_commands;
    set<P> locked;

    vec_int x_vec;
    vec_int y_vec;
    rep(i, N)x_vec.push_back(i);
    rep(i, N)y_vec.push_back(i);
    shuffle(x_vec.begin(), x_vec.end(), mt);
    shuffle(y_vec.begin(), y_vec.end(), mt);


    rep(ii, N)rep(jj, N){
        int i = x_vec.at(ii);
        int j = y_vec.at(jj);
        if(locked.count(make_pair(i, j)))continue;
        if(board.at(i).at(j)==color){
            int id = computer_id_map.at(i).at(j);
            if(uf_tree.size(id)!=1)continue;
            queue<T4> que;
            que.emplace(i, j, -1, -1);
            map<P, P> prev_map;

            while(!que.empty()){
                int x, y, px, py; tie(x, y, px, py) = que.front(); que.pop();
                if(prev_map.count(make_pair(x, y)))continue;
                if(board.at(x).at(y)!=0&&!(x==i&&y==j))continue;
                prev_map[make_pair(x, y)] = make_pair(px, py);
                if(x!=i||y!=j){
                    swap_pos(i, j, x, y); 
                }

                // 移動したcomputerの隣に何があるかをチェックする
                vec_int neighbors = computers.at(id).find_neighbors(board, computer_id_map);
                int connect_id = -1;
                set<int> connected_computer_ids;
                rep(kk, 4){
                    int next_id = neighbors.at(kk);
                    if(next_id==-1)continue;
                    if(can_connect_cable(x, y, computers.at(next_id).x, computers.at(next_id).y)||painted.at(x).at(y)==1){
                        connect_id = next_id;
                        locked.insert(make_pair(computers.at(next_id).x, computers.at(next_id).y));
                        connected_computer_ids.insert(next_id);
                        // break;
                    }
                }
                // 盤面を元に戻す
                if(x!=i||y!=j){
                    swap_pos(x, y, i, j);
                }

                int outside_flag = 0;
                if(!can_use_outside){
                    if(is_outside(x, y))outside_flag=1;
                }

                // 移動さきで結合できればそれでOK
                if(connect_id != -1&&outside_flag==0){
                    int posx = x;
                    int posy = y;
                    vector<T4> tmp_vec;
                    while(true){
                        P prev = prev_map[make_pair(posx, posy)];
                        if(prev == make_pair(-1, -1))break;
                        tmp_vec.push_back(make_tuple(prev.first, prev.second, posx, posy));
                        posx = prev.first;
                        posy = prev.second;
                    }
                    reverse(tmp_vec.begin(), tmp_vec.end());
                    rep(i, tmp_vec.size()){
                        // move_commands.push_back(tmp_vec.at(i));
                        int a, b, c,d;tie(a,b,c,d) = tmp_vec.at(i);
                        move_computer_(a,b,c,d);
                        commands.push_back(tmp_vec.at(i));
                        for(auto next_id:connected_computer_ids){
                            uf_tree.merge(next_id, id);
                        }
                        dfs_connect();
                    }
                    break;
                }

                // 移動距離がmove_distance以上の時にはそれ以上遠くに行かない
                if(abs(i-x)+abs(j-y)>=move_distance)continue;

                // 次の候補を入れる
                rep(k, 4){
                    int dx, dy; tie(dx, dy) = DIRS.at(k);
                    int x2 = x+dx;
                    int y2 = y+dy;
                    if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)continue;
                    if(prev_map.count(make_pair(x2, y2)))continue;
                    if(board.at(x2).at(y2)!=0)continue;
                    que.emplace(x2, y2, x, y);
                }
            }
 
        }
    }
    /*
    for(auto tmp: move_commands){
        int a, b, c,d;tie(a,b,c,d) = tmp;
        // 連結成分になっている可能性がある
        if(board.at(c).at(d)!=0)continue; //別のやつが先に移動してきてしまっている件
        move_computer_(a,b,c,d);
        commands.push_back(tmp);
        dfs_connect();
    }
    */

}

void Board::move_connected_computers(int move_distance){
    // 既に他のコンピュータと結合されているコンピュータを動かして、他のグループとくっつけられないかを試す
    vector<T4> move_commands;
    vec_int id_vec;
    rep(i, computers.size())id_vec.push_back(i);
    shuffle(id_vec.begin(), id_vec.end(), mt);

    rep(idd, computers.size()){
        int id = id_vec.at(idd);
        if(computer_connection.at(id).size()!=1)continue;
        //  if(!computers.at(id).can_move(board, computer_id_map, computer_connection.at(id)))continue;
        // vec_int original_neighbors = computers.at(id).find_neighbors(board, computer_id_map);
        int group_id = uf_tree.leader(id);
        int i = computers.at(id).x;
        int j = computers.at(id).y;

        queue<T4> que;
        que.emplace(i, j, -1, -1);
        map<P, P> prev_map;

        while(!que.empty()){
            int x, y, px, py; tie(x, y, px, py) = que.front(); que.pop();
            if(prev_map.count(make_pair(x, y)))continue;
            if(board.at(x).at(y)!=0&&!(x==i&&y==j))continue;

            prev_map[make_pair(x, y)] = make_pair(px, py);

            int outside_flag = 0;
            if(!can_use_outside){
                if(is_outside(x, y))outside_flag=1;
            }


            if((x!=i||y!=j)&&outside_flag==0){ // 元のところでは探索をしない
                swap_pos(i, j, x, y);
                // 移動した先が元のグループとつながっているかをチェックする
                vec_int neighbors = computers.at(id).find_neighbors(board, computer_id_map);
                int flag1 = 0;
                int connect_id = -1;
                rep(kk, 4){
                    int next_id = neighbors.at(kk);
                    if(next_id==-1)continue;

                    if(group_id == uf_tree.leader(next_id)){
                        flag1 = 1;
                    }else{
                        if(can_connect_cable(x, y, computers.at(next_id).x, computers.at(next_id).y)||painted.at(x).at(y)==1){
                            connect_id = next_id;
                        }
                    }
                }
                swap_pos(x, y, i, j);

                // 移動さきで結合できればそれでOK
                if(connect_id != -1 && flag1==1){
                    // move_commands.push_back(make_tuple(i, j, x, y));
                    int posx = x;
                    int posy = y;
                    vector<T4> tmp_vec;
                    while(true){
                        P prev = prev_map[make_pair(posx, posy)];
                        if(prev == make_pair(-1, -1))break;
                        tmp_vec.push_back(make_tuple(prev.first, prev.second, posx, posy));
                        posx = prev.first;
                        posy = prev.second;
                    }
                    reverse(tmp_vec.begin(), tmp_vec.end());
                    rep(i, tmp_vec.size()){
                        move_commands.push_back(tmp_vec.at(i));
                    }

                    for(auto tmp: tmp_vec){
                        int a, b, c,d;tie(a,b,c,d) = tmp;
                        move_computer_(a,b,c,d);
                        commands.push_back(tmp);
                    }
                    dfs_connect();
                    break;
                }
            }

            // 移動距離がmove_distance以上の時にはそれ以上遠くに行かない
            if(abs(i-x)+abs(j-y)>=move_distance)continue;

            // 次の候補を入れる
            rep(k, 4){
                int dx, dy; tie(dx, dy) = DIRS.at(k);
                int x2 = x+dx;
                int y2 = y+dy;
                if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)continue;
                if(prev_map.count(make_pair(x2, y2)))continue;
                if(board.at(x2).at(y2)!=0)continue;
                que.emplace(x2, y2, x, y);
            }
        }
 
    }
    for(auto tmp: move_commands){
        // commands.push_back(tmp);
        // int a, b, c,d;tie(a,b,c,d) = tmp;
        // if(board.at(c).at(d)!=0)continue; //別のやつが先に移動してきてしまっている件
        // move_computer_(a,b,c,d);
        // dfs_connect();
    }
}

P Board::find_nearest_vacant_cell(int x, int y){
    queue<P> que;
    que.emplace(x, y);
    set<P> visited;
    while(!que.empty()){
        int xpos, ypos; tie(xpos, ypos) = que.front(); que.pop();
        if(visited.count(make_pair(xpos, ypos)))continue;
        visited.insert(make_pair(xpos, ypos));
        if(board.at(xpos).at(ypos)==0)return make_pair(xpos,ypos);

        rep(kk, 4){
            int dx, dy; tie(dx, dy) = DIRS.at(kk);
            int x2 = xpos+dx;
            int y2 = ypos+dy;
            if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)continue;
            if(painted.at(x2).at(y2)==1)continue;
            if(board.at(x2).at(y2)==color)continue;
            if(visited.count(make_pair(x2, y2)))continue;
            que.emplace(x2, y2);
        }
    }
    return make_pair(-1, -1);
}


P Board::find_nearest_vacant_cell3(int x, int y, int x22, int y22){
    // 元々のメソッドではcolorのますを通らないようにしていたが、
    queue<P> que;
    que.emplace(x, y);
    set<P> visited;
    while(!que.empty()){
        int xpos, ypos; tie(xpos, ypos) = que.front(); que.pop();
        if(visited.count(make_pair(xpos, ypos)))continue;
        visited.insert(make_pair(xpos, ypos));
        if(board.at(xpos).at(ypos)==0)return make_pair(xpos,ypos);

        rep(kk, 4){
            int dx, dy; tie(dx, dy) = DIRS.at(kk);
            int x2 = xpos+dx;
            int y2 = ypos+dy;
            if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)continue;
            if(x2==x22&&y2==y22)continue;
            if(painted.at(x2).at(y2)==1)continue;
            if(visited.count(make_pair(x2, y2)))continue;
            if(board.at(xpos).at(ypos)==color&&is_outside(x2, y2)&&board.at(x2).at(y2)==0&&!is_outside(xpos, ypos))continue;
            que.emplace(x2, y2);
        }
    }
    return make_pair(-1, -1);
}


P Board::find_nearest_vacant_cell2(int x, int y, set<P> &prohibited, vector<T4> &moves, int limit_distance){
    queue<T5> que;
    que.emplace(0, x, y, -1, -1);
    map<P, P> visited;
    while(!que.empty()){
        int cost, xpos, ypos, px, py; tie(cost, xpos, ypos, px, py) = que.front(); que.pop();
        if(visited.count(make_pair(xpos, ypos)))continue;
        // visited.insert(make_pair(xpos, ypos));
        visited[make_pair(xpos, ypos)] = make_pair(px, py);

        if(board.at(xpos).at(ypos)==0&&!prohibited.count(make_pair(xpos, ypos))){
            int x1 = xpos;
            int y1 = ypos;
            while(x1!=x||y1!=y){
                int x2, y2; tie(x2, y2) = visited[make_pair(x1, y1)] ;
                moves.push_back(make_tuple(x1, y1, x2, y2));
                x1 = x2;
                y1 = y2;
            }
            // reverse(moves.begin(), moves.end());
            return make_pair(xpos,ypos);
        }
        if(cost>=limit_distance)continue;
        rep(kk, 4){
            int dx, dy; tie(dx, dy) = DIRS.at(kk);
            int x2 = xpos+dx;
            int y2 = ypos+dy;
            if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)continue;
            if(painted.at(x2).at(y2)==1)continue;
            if(board.at(x2).at(y2)==color)continue;
            if(visited.count(make_pair(x2, y2)))continue;
            que.emplace(cost+1, x2, y2, xpos, ypos);
        }
    }
    return make_pair(-1, -1);
}

P Board::find_nearest_vacant_cell4(int x, int y, set<P> &prohibited, vector<T4> &moves, int limit_distance, vector<vec_int> &painted2){
    // 2つ目の木を作るときに障害物を取り除く際に使う
    queue<T5> que;
    que.emplace(0, x, y, -1, -1);
    map<P, P> visited;
    while(!que.empty()){
        int cost, xpos, ypos, px, py; tie(cost, xpos, ypos, px, py) = que.front(); que.pop();
        if(visited.count(make_pair(xpos, ypos)))continue;
        // visited.insert(make_pair(xpos, ypos));
        visited[make_pair(xpos, ypos)] = make_pair(px, py);

        if(board.at(xpos).at(ypos)==0&&!prohibited.count(make_pair(xpos, ypos))){
            int x1 = xpos;
            int y1 = ypos;
            while(x1!=x||y1!=y){
                int x2, y2; tie(x2, y2) = visited[make_pair(x1, y1)] ;
                moves.push_back(make_tuple(x1, y1, x2, y2));
                x1 = x2;
                y1 = y2;
            }
            // reverse(moves.begin(), moves.end());
            return make_pair(xpos,ypos);
        }
        if(cost>=limit_distance)continue;
        rep(kk, 4){
            int dx, dy; tie(dx, dy) = DIRS.at(kk);
            int x2 = xpos+dx;
            int y2 = ypos+dy;
            if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)continue;
            if(painted.at(x2).at(y2)==1)continue;
            if(painted2.at(x2).at(y2)==1)continue;
            if(board.at(x2).at(y2)==color)continue;
            if(board.at(x2).at(y2)==second_color)continue;
            if(visited.count(make_pair(x2, y2)))continue;
            que.emplace(cost+1, x2, y2, xpos, ypos);
        }
    }
    return make_pair(-1, -1);
}




void Board::swap_pos(int x1, int y1, int x2, int y2){
    int color = board.at(x1).at(y1);
    board.at(x1).at(y1) = board.at(x2).at(y2);
    board.at(x2).at(y2) = color;

    int id = computer_id_map.at(x1).at(y1);
    int id2 = computer_id_map.at(x2).at(y2);
    computer_id_map.at(x1).at(y1) = id2;
    computer_id_map.at(x2).at(y2) = id;

    if(id!=-1){
        computers.at(id).set_xy(x2,y2);
    }
    if(id2!=-1){
        computers.at(id2).set_xy(x1,y1);
    }
}

void Board::swap_pos2(int x1, int y1, int x2, int y2, vector<vec_int> &id_map, vector<Computer> &comps){
    int color = board.at(x1).at(y1);
    board.at(x1).at(y1) = board.at(x2).at(y2);
    board.at(x2).at(y2) = color;

    int id = id_map.at(x1).at(y1);
    int id2 = id_map.at(x2).at(y2);
    id_map.at(x1).at(y1) = id2;
    id_map.at(x2).at(y2) = id;

    if(id!=-1){
        comps.at(id).set_xy(x2,y2);
    }
    if(id2!=-1){
        comps.at(id2).set_xy(x1,y1);
    }
}


void Board::connect_free_computers(int max_moves){
    rep(id, computers.size()){
        if(uf_tree.size(id)!=1)continue;
        int xpos = computers.at(id).x;
        int ypos = computers.at(id).y;
        P vacant_cell = find_nearest_vacant_cell(xpos, ypos);
        int vx, vy; tie(vx, vy) = vacant_cell;
        // cerr<<"id:"<<id<<" vacant_cell:"<<vx<<" "<<vy<<endl;
        if(vx==-1)continue; //到達可能な位置に空白のセルがない
        vector<T4> ans_moves;
        map<T4, int> cache;
        vector<T4> moves;
        vacant_dfs(0, vx, vy, -1, -1, id, cache, ans_moves, moves);
        // cerr<<"id:"<<id<<" moves:"<<ans_moves.size()<<endl;
        if(ans_moves.size()<=max_moves){
            for(auto tmp: ans_moves){
                int x1, y1, x2, y2; tie(x1, y1, x2, y2) = tmp;
                if(board.at(x1).at(y1)==0&&board.at(x2).at(y2)==0)continue;
                commands.push_back(make_tuple(x2, y2, x1, y1)); // 空白を動かしているので、逆側にみないといけない
                move_computer_(x2, y2, x1, y1);
            }
            dfs_connect();
        }
    }
}


void Board::vacant_dfs(int cost, int vx, int vy, int pvx, int pvy, int computer_id, map<T4, int> &cache, vector<T4> &ans_moves, vector<T4> &moves){
    if(ans_moves.size()>0&&cost>=ans_moves.size())return;
    int x = computers.at(computer_id).x;
    int y = computers.at(computer_id).y;
    /*
    if(vx==0&&vy==31){
        cerr<<"cost:"<<cost<<" "<<vx<<" "<<vy<<" "<<pvx<<" "<<pvy<<" "<<computer_id<<endl;
        cerr<<"computer pos:"<<x<<" "<<y<<endl;
    }
    */
    if(pvx!=-1){ //pvx =-1になるのは最初だけ
        swap_pos(pvx, pvy, vx, vy);
        T4 command = make_tuple(pvx, pvy, vx, vy);
        moves.push_back(command);
        T4 state = make_tuple(vx, vy, computers.at(computer_id).x, computers.at(computer_id).y);
        if(cache.count(state)){
            if(cache[state]<=cost){
                swap_pos(vx, vy, pvx, pvy);
                moves.pop_back();
                return;
            }
        }

        cache[state] = cost;
            vec_int neighbors = computers.at(computer_id).find_neighbors(board, computer_id_map);
            int connect_id = -1;
            rep(kk, 4){
                int next_id = neighbors.at(kk);
                if(next_id==-1)continue;
                if(uf_tree.leader(next_id)==uf_tree.leader(computer_id))continue;
                if(can_connect_cable(vx, vy, computers.at(next_id).x, computers.at(next_id).y)||painted.at(pvx).at(pvy)==1){
                    connect_id = next_id;
                    break;
                }
            }
            if(connect_id!=-1){
                if(ans_moves.size()==0||ans_moves.size()>moves.size()){
                    ans_moves = moves;
                }
                moves.pop_back();
                swap_pos(vx, vy, pvx, pvy);
                return ;
            }
        // }
    }

    rep(k, 4){
        if(cost>=11)continue;
        int dx, dy; tie(dx, dy) = DIRS.at(k);
        int vx2 = vx+dx;
        int vy2 = vy+dy;
        if (vx2 < 0 || vx2 >= N || vy2 < 0 || vy2 >= N)continue;
        if (painted.at(vx2).at(vy2)==1)continue;
        if(vx2 == pvx && vy2 == pvy)continue;
        vacant_dfs(cost+1, vx2, vy2, vx, vy, computer_id, cache, ans_moves, moves);
    }

    if(pvx!=-1){
        moves.pop_back();
        swap_pos(vx, vy, pvx, pvy);
    }
    return ;
}

void Board::move_obstacle_strong(int limit, int limit_distance){
    vector<P> dirs = {make_pair(1, 0), make_pair(0, 1)};
    vector<T5> operation_candidates;
    vec_int x_vec;
    vec_int y_vec;
    rep(i, N)x_vec.push_back(i);
    rep(i, N)y_vec.push_back(i);
    shuffle(x_vec.begin(), x_vec.end(), mt);
    shuffle(y_vec.begin(), y_vec.end(), mt);

    rep(xx, N){
        rep(yy, N){
            int x = x_vec.at(xx);
            int y = y_vec.at(yy);
            if(board.at(x).at(y)==color||board.at(x).at(y)==0)continue;
            rep(k, 2){
                vector<P> destinations; //空白を埋めたいせるの座標
                destinations.push_back(make_pair(x, y));
                int other_count = 0;
                int dx, dy; tie(dx, dy) = dirs.at(k); // k=0の時: 縦方向の探索, 1の時: 横方向の探索
                int ind = 1;
                int flag1 = 0;
                int computer1 = -1;
                while(true){
                    int x2 = x + dx*ind;
                    int y2 = y + dy*ind;
                    if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)break;
                    if(painted.at(x2).at(y2)==1&&board.at(x2).at(y2)==0)break;
                    if(board.at(x2).at(y2)!=0&&board.at(x2).at(y2)!=color){
                        other_count++;
                        destinations.push_back(make_pair(x2, y2));
                    }
                    if(other_count>=limit)break;
                    if(board.at(x2).at(y2)==color){
                        flag1 = 1;
                        computer1 = computer_id_map.at(x2).at(y2);
                        break;
                    }else{
                        ind++;
                    }
                }

                if(flag1==0)continue;

                int flag2 = 0;
                ind = -1;
                int computer2 = -1;
                while(true){
                    int x2 = x + dx*ind;
                    int y2 = y + dy*ind;
                    if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)break;
                    if(painted.at(x2).at(y2)==1&&board.at(x2).at(y2)==0)break;
                    // if(board.at(x2).at(y2)!=0&&board.at(x2).at(y2)!=color)break;
                    if(board.at(x2).at(y2)!=0&&board.at(x2).at(y2)!=color){
                        other_count++;
                        destinations.push_back(make_pair(x2,y2));
                    }
                    if(other_count>=limit)break;
                    if(board.at(x2).at(y2)==color){
                        flag2 = 1;
                        computer2 = computer_id_map.at(x2).at(y2);
                        break;
                    }else{
                        ind--;
                    }
                }

                if(flag2==0)continue;
                if(uf_tree.same(computer1, computer2))continue;

                set<P> prohibited_cells = cells_between(computer1, computer2);
                int move_count = 0;
                int ok_flag = 1;
                for(auto pos: destinations){
                    int xx, yy; tie(xx, yy) = pos;
                    vector<T4> moves;
                    P vacant = find_nearest_vacant_cell2(xx, yy, prohibited_cells, moves, limit_distance);
                    if(vacant.first==-1){
                        ok_flag=0;
                        break;
                    }
                    int flag = 0;
                    int px, py;
                    for(auto tmp: moves){
                        int x1, y1, x2, y2; tie(x1, y1, x2, y2) = tmp;
                        if(board.at(x1).at(y1)==0&&board.at(x2).at(y2)==0){
                            px = x1;
                            py = y1;
                            flag = 1;
                            continue;
                        }
                        commands.push_back(make_tuple(x2, y2, x1, y1)); 
                        move_computer_(x2, y2, x1, y1);
                        move_count++;

                        if(flag==1){
                            commands.push_back(make_tuple(x1, y1, px, py)); 
                            move_computer_(x1, y1, px, py);
                            move_count++;
                            flag = 0;
                        }
                    }
                }
                if(ok_flag==0){
                    rep(iii, move_count)cancel_move();
                    continue;
                }
                int dist = computers.at(computer1).distance_between(computers.at(computer2));

                if(is_dense&&dist>1){
                    // 密度が高い時にはできるだけ間を詰めるようにする
                    if(uf_tree.size(computer1)==1){
                        rep(kk, dist-1){
                            move_computer_toward(computer1, computer2);
                        }
                    }else if(uf_tree.size(computer2)==1){
                        rep(kk, dist-1){
                            move_computer_toward(computer2, computer1);
                        }
                    }
                }
                dfs_connect();
                break;
            }
        }
    }

}

int Board::move_second_obstacle_strong(int limit, int limit_distance, vector<vec_int> &painted2, set<int> &used_nodes, vector<vec_int> &id_map, vector<Computer> &comps){
    vector<P> dirs = {make_pair(1, 0), make_pair(0, 1)};
    vector<T5> operation_candidates;
    vec_int x_vec;
    vec_int y_vec;
    rep(i, N)x_vec.push_back(i);
    rep(i, N)y_vec.push_back(i);
    shuffle(x_vec.begin(), x_vec.end(), mt);
    shuffle(y_vec.begin(), y_vec.end(), mt);

    rep(xx, N){
        rep(yy, N){
            int x = x_vec.at(xx);
            int y = y_vec.at(yy);
            if(!can_use_outside&&(x==0||x==N-1||y==0||y==N-1))continue;
            if(board.at(x).at(y)==color||board.at(x).at(y)==second_color||board.at(x).at(y)==0)continue;
            rep(k, 2){
                vector<P> destinations; //空白を埋めたいせるの座標
                destinations.push_back(make_pair(x, y));
                int other_count = 0;
                int dx, dy; tie(dx, dy) = dirs.at(k); // k=0の時: 縦方向の探索, 1の時: 横方向の探索
                int ind = 1;
                int flag1 = 0;
                int computer1 = -1;
                while(true){
                    int x2 = x + dx*ind;
                    int y2 = y + dy*ind;
                    if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)break;
                    if(painted.at(x2).at(y2)==1&&board.at(x2).at(y2)==0)break;
                    if(painted2.at(x2).at(y2)==1&&board.at(x2).at(y2)==0)break;
                    if(board.at(x2).at(y2)==color)break;

                    if(board.at(x2).at(y2)!=0&&board.at(x2).at(y2)!=color&&board.at(x2).at(y2)!=second_color){
                        other_count++;
                        destinations.push_back(make_pair(x2, y2));
                    }
                    if(other_count>=limit)break;
                    if(board.at(x2).at(y2)==second_color){
                        flag1 = 1;
                        computer1 = id_map.at(x2).at(y2);
                        break;
                    }else{
                        ind++;
                    }
                }

                if(flag1==0)continue;

                int flag2 = 0;
                ind = -1;
                int computer2 = -1;
                while(true){
                    int x2 = x + dx*ind;
                    int y2 = y + dy*ind;
                    if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)break;
                    if(painted.at(x2).at(y2)==1&&board.at(x2).at(y2)==0)break;
                    if(painted2.at(x2).at(y2)==1&&board.at(x2).at(y2)==0)break;
                    if(board.at(x2).at(y2)==color)break;
                    if(board.at(x2).at(y2)!=0&&board.at(x2).at(y2)!=color&&board.at(x2).at(y2)!=second_color){
                        other_count++;
                        destinations.push_back(make_pair(x2,y2));
                    }
                    if(other_count>=limit)break;
                    if(board.at(x2).at(y2)==second_color){
                        flag2 = 1;
                        computer2 = id_map.at(x2).at(y2);
                        break;
                    }else{
                        ind--;
                    }
                }

                if(flag2==0)continue;
                // if(uf_tree.same(computer1, computer2))continue;
                if(used_nodes.count(computer1)&&used_nodes.count(computer2))continue;
                if(!(used_nodes.count(computer1)||used_nodes.count(computer2)))continue;

                set<P> prohibited_cells = cells_between_second(computer1, computer2, comps);
                int move_count = 0;
                int ok_flag = 1;
                for(auto pos: destinations){
                    int xx, yy; tie(xx, yy) = pos;
                    vector<T4> moves;
                    P vacant = find_nearest_vacant_cell4(xx, yy, prohibited_cells, moves, limit_distance, painted2);
                    if(vacant.first==-1){
                        ok_flag=0;
                        break;
                    }
                    /*
                    cerr<<"prohibited:"<<endl;
                    for(auto ss: prohibited_cells){
                        cerr<<ss.first<<" "<<ss.second<<endl;
                    }
                    cerr<<vacant.first<<" "<<vacant.second<<endl;
                    */
                    int flag = 0;
                    int px, py;
                    for(auto tmp: moves){
                        int x1, y1, x2, y2; tie(x1, y1, x2, y2) = tmp;
                        if(board.at(x1).at(y1)==0&&board.at(x2).at(y2)==0){
                            px = x1;
                            py = y1;
                            flag = 1;
                            continue;
                        }
                        commands.push_back(make_tuple(x2, y2, x1, y1)); 
                        move_computer_(x2, y2, x1, y1);
                        move_count++;

                        if(flag==1){
                            commands.push_back(make_tuple(x1, y1, px, py)); 
                            move_computer_(x1, y1, px, py);
                            move_count++;
                            flag = 0;
                        }
                    }
                }
                if(ok_flag==0){
                    rep(iii, move_count)cancel_move();
                    continue;
                }
                // int dist = computers.at(computer1).distance_between(computers.at(computer2));

                /*
                if(is_dense&&dist>1){
                    // 密度が高い時にはできるだけ間を詰めるようにする
                    if(uf_tree.size(computer1)==1){
                        rep(kk, dist-1){
                            move_computer_toward(computer1, computer2);
                        }
                    }else if(uf_tree.size(computer2)==1){
                        rep(kk, dist-1){
                            move_computer_toward(computer2, computer1);
                        }
                    }
                }
                */
                // dfs_connect();
                return move_count;
            }
        }
    }
    return 0;

}



set<P> Board::cells_between(int id1, int id2){
    int x1 = computers.at(id1).x;
    int y1 = computers.at(id1).y;
    int x2 = computers.at(id2).x;
    int y2 = computers.at(id2).y;

    set<P> result;

    if(x1==x2){
        int x=x1;
        int ymin = min(y1, y2);
        int ymax = max(y1, y2);
        for(int y=ymin;y<=ymax;y++){
            result.insert(make_pair(x, y));
        }
    }else{
        int y=y1;
        int xmin = min(x1, x2);
        int xmax = max(x1, x2);
        for(int x=xmin;x<=xmax;x++){
            result.insert(make_pair(x, y));
        }
    }
    return result;
 
}
set<P> Board::cells_between_second(int id1, int id2, vector<Computer> &comps){
    int x1 = comps.at(id1).x;
    int y1 = comps.at(id1).y;
    int x2 = comps.at(id2).x;
    int y2 = comps.at(id2).y;

    set<P> result;

    if(x1==x2){
        int x=x1;
        int ymin = min(y1, y2);
        int ymax = max(y1, y2);
        for(int y=ymin;y<=ymax;y++){
            result.insert(make_pair(x, y));
        }
    }else{
        int y=y1;
        int xmin = min(x1, x2);
        int xmax = max(x1, x2);
        for(int x=xmin;x<=xmax;x++){
            result.insert(make_pair(x, y));
        }
    }
    return result;
 
}
set<P> cells_between3(int x1, int y1, int x2, int y2){

    set<P> result;

    if(x1==x2){
        int x=x1;
        int ymin = min(y1, y2);
        int ymax = max(y1, y2);
        for(int y=ymin;y<=ymax;y++){
            result.insert(make_pair(x, y));
        }
    }else{
        int y=y1;
        int xmin = min(x1, x2);
        int xmax = max(x1, x2);
        for(int x=xmin;x<=xmax;x++){
            result.insert(make_pair(x, y));
        }
    }
    return result;
 
}

void Board::shrink_cluster(){
    //if(uf_tree.groups().size()==1)return;

    vector<vec_int> groups = uf_tree.groups();
    for(auto group: groups){
        // if(group.size()>80)continue;
        for(auto id1: group){
            int x1 = computers.at(id1).x;
            int y1 = computers.at(id1).y;
            if(computer_connection.at(id1).size()==1){
                int id2 = computer_connection.at(id1).at(0);
                int x2 = computers.at(id2).x;
                int y2 = computers.at(id2).y;
                if(abs(x1-x2)+abs(y1-y2)>1){
                    int x, y;
                    if(x1==x2){
                        x = x1;
                        if(y1>y2){
                            y = y1-1;
                        }else{
                            y = y1+1;
                        }
                    }else if(y1==y2){
                        y = y1;
                        if(x1>x2){
                            x = x1-1;
                        }else{
                            x = x1+1;
                        }
                    }
                    move_computer_(x1, y1, x, y);
                    commands.push_back(make_tuple(x1, y1, x, y));
                    dfs_connect();
                    return ;
                }            
            }
        }
    }
    for(auto group: groups){
        for(auto id1: group){
            int x1 = computers.at(id1).x;
            int y1 = computers.at(id1).y;
            if(computer_connection.at(id1).size()==1){
                int id2 = computer_connection.at(id1).at(0);
                int x2 = computers.at(id2).x;
                int y2 = computers.at(id2).y;
                /*
                if(id1==5){
                    cerr<<"id2:"<<id2<<" "<<x2<<" "<<y2<<endl;
                    cerr<<computer_connection.at(id2).size()<<endl;
                    rep(i, N){
                        rep(j, N){
                            cerr<<painted.at(i).at(j);
                        }
                        cerr<<endl;
                    }
                    cerr<<" aaa"<<endl;
                    rep(i, N){
                        rep(j, N){
                            cerr<<board.at(i).at(j);
                        }
                        cerr<<endl;
                    }
                }*/

                if(abs(x1-x2)+abs(y1-y2)==1){
                    if(computer_connection.at(id2).size()==1)continue;
                    int id3;
                    for(auto id: computer_connection.at(id2)){
                        if(id==id1)continue;
                        id3 = id;
                    }
                    int x3 = computers.at(id3).x;
                    int y3 = computers.at(id3).y;

                    if(abs(x3-x2)+abs(y3-y2)>1){
                        int x, y;
                        if(x3==x2){
                            x = x2;
                            if(y3>y2){
                                y = y2+1;
                            }else{
                                y = y2-1;
                            }
                        }else if(y3==y2){
                            y = y2;
                            if(x3>x2){
                                x = x2+1;
                            }else{
                                x = x2-1;
                            }
                        }
                        move_computer_(x2, y2, x, y);
                        commands.push_back(make_tuple(x2, y2, x, y));
                        move_computer_(x1, y1, x2, y2);
                        commands.push_back(make_tuple(x1, y1, x2, y2));
                        dfs_connect();
                        return ;
                    }
                }
            }
        }
    }
    for(auto group: groups){
        for(auto id1: group){
            int x1 = computers.at(id1).x;
            int y1 = computers.at(id1).y;
            if(computer_connection.at(id1).size()==1){
                int id2 = computer_connection.at(id1).at(0);
                int x2 = computers.at(id2).x;
                int y2 = computers.at(id2).y;
                if(abs(x1-x2)+abs(y1-y2)==1){
                    if(computer_connection.at(id2).size()==1)continue;
                    int id3;
                    for(auto id: computer_connection.at(id2)){
                        if(id==id1)continue;
                        id3 = id;
                    }
                    int x3 = computers.at(id3).x;
                    int y3 = computers.at(id3).y;

                    if(abs(x3-x2)+abs(y3-y2)==1){
                        if(computer_connection.at(id3).size()==1)continue;
                        int id4;
                        for(auto id: computer_connection.at(id3)){
                            if(id==id2)continue;
                            id4 = id;
                        }
                        int x4 = computers.at(id4).x;
                        int y4 = computers.at(id4).y;

                        if(abs(x4-x3)+abs(y4-y3)>1){
                            int x, y;
                            if(x4==x3){
                                x = x3;
                                if(y4>y3){
                                    y = y3+1;
                                }else{
                                    y = y3-1;
                                }
                            }else if(y4==y3){
                                y = y3;
                                if(x4>x3){
                                    x = x3+1;
                                }else{
                                    x = x3-1;
                                }
                            }
                            move_computer_(x3, y3, x, y);
                            commands.push_back(make_tuple(x3, y3, x, y));
                            move_computer_(x2, y2, x3, y3);
                            commands.push_back(make_tuple(x2, y2, x3, y3));
                            move_computer_(x1, y1, x2, y2);
                            commands.push_back(make_tuple(x1, y1, x2, y2));
                            dfs_connect();
                            return ;
                        }

                    }
                }
            }
        }
    }
}

int Board::calc_score(){
    int result = 0;
    if(commands.size()>K*100-99){
        int res = K*100-commands.size();
        if(res<0)return 0;
        return (res*(res+1))/2;
    }
    vector<vec_int> groups = uf_tree.groups();
    for(auto group: groups){
        int sz = group.size();
        result += (sz*(sz-1))/2;
    }
    return result;

}

void Board::connect_other_color(int color2){
    dsu uf(100);
    vector<Computer> comps;
    vector<vec_int> computer_id_map2(N, vec_int(N, -1));
    rep(i, N)rep(j, N){
        if(board.at(i).at(j)==color2){
            Computer tmp = Computer(i, j, comps.size(), color2);
            computer_id_map2.at(i).at(j) = comps.size();
            comps.push_back(tmp);
        }
    }

    vector<T> connect_candidates;
    rep(id, comps.size()){
        vec_int neighbors = comps.at(id).find_neighbors(board, computer_id_map2);
        int x = comps.at(id).x;
        int y = comps.at(id).y;
        rep(kk, 4){
            if(neighbors.at(kk)==-1)continue;
            int id2 = neighbors.at(kk);
            int x2 = comps.at(id2).x;
            int y2 = comps.at(id2).y;
            int dist = abs(x-x2) + abs(y-y2);
            connect_candidates.push_back(make_tuple(dist, id, id2));
        }
    }
    sort(connect_candidates.begin(), connect_candidates.end());
    vector<vec_int> comp_connection(100);
    for(auto tmp: connect_candidates){
        int dist, id1, id2; tie(dist, id1, id2) = tmp;
        if(uf.same(id1, id2))continue;
        int x1 = comps.at(id1).x;
        int y1 = comps.at(id1).y;
        int x2 = comps.at(id2).x;
        int y2 = comps.at(id2).y;
        if(!can_connect_cable(x1, y1, x2, y2))continue;

        uf.merge(id1, id2);
        paint_cable_tiles(x1, y1, x2, y2);
        comp_connection.at(id1).push_back(id2);
        comp_connection.at(id2).push_back(id1);
    }

    rep(id1, 100){
        for(int id2: comp_connection.at(id1)){
            if(id1>id2)continue;
            int x1 = comps.at(id1).x;
            int y1 = comps.at(id1).y;
            int x2 = comps.at(id2).x;
            int y2 = comps.at(id2).y;
            other_color_connections.push_back(make_tuple(x1, y1, x2, y2));
        }
    }
}

void Board::gradual_ans(){
    rep(i, min(K*100, (int)commands.size()+1)){
        cout<<i<<endl;
        rep(j, i){
            int a, b, c, d; tie(a,b,c,d) = commands.at(j);
            cout<<a<<" "<<b<<" "<<c<<" "<<d<<endl;
        }
        cout<<0<<endl;
    }
}

void Board::move_computer_toward(int id1, int id2){
    int x1 = computers.at(id1).x;
    int y1 = computers.at(id1).y;
    int x2 = computers.at(id2).x;
    int y2 = computers.at(id2).y;

    int x, y;
    if(x1==x2){
        x = x1;
        if(y1<y2){
            y = y1+1;
        }else{
            y = y1-1;
        }
    }else{
        y = y1;
        if(x1<x2){
            x = x1+1;
        }else{
            x = x1-1;
        }
    }
    if(board.at(x).at(y)!=0)return;

    commands.push_back(make_tuple(x1, y1, x, y)); 
    move_computer_(x1, y1, x, y);
}

void Board::resolve_separated_clusters(int max_size){
    if(uf_tree.groups().size()==1)return;

    vector<vec_int> groups = uf_tree.groups();
    for(auto group: groups){
        if(group.size()>max_size)continue;
        int group_id = uf_tree.leader(group.at(0));

        int best_move_len = INT_MAX;
        vector<T4> best_moves;
        for(auto id: group){
            vector<T4> ans_moves;
            map<T4, int> cache;
            vector<T4> moves;
            int xpos = computers.at(id).x;
            int ypos = computers.at(id).y;
            P vacant_cell = find_nearest_vacant_cell(xpos, ypos);
            int vx, vy; tie(vx, vy) = vacant_cell;
            painted.at(xpos).at(ypos) = 0;
            vacant_dfs(0, vx, vy, -1, -1, id, cache, ans_moves, moves);
            painted.at(xpos).at(ypos) = 1;
            if(vx==-1)continue;
            if(ans_moves.size()>0&&best_move_len>ans_moves.size()){
                best_move_len = ans_moves.size();
                best_moves = ans_moves;
            }
        }
        // cerr<<" moves:"<<best_moves.size()<<endl;
        if(best_moves.size()>0){
        for(auto tmp: best_moves){
            int x1, y1, x2, y2; tie(x1, y1, x2, y2) = tmp;
            if(board.at(x1).at(y1)==0&&board.at(x2).at(y2)==0)continue;
            commands.push_back(make_tuple(x2, y2, x1, y1)); // 空白を動かしているので、逆側にみないといけない
            move_computer_(x2, y2, x1, y1);
        }
        dfs_connect();
        return ;
        }
    }
}


void Board::prepare_for_second_tree(){
    vector<vec_int> groups = uf_tree.groups();
    for(auto group: groups){
        for(auto id1: group){
            int x1 = computers.at(id1).x;
            int y1 = computers.at(id1).y;
            if(computer_connection.at(id1).size()==1){
                cerr<<"candidate:"<<id1<<endl;
                int id2 = computer_connection.at(id1).at(0);
                int x2 = computers.at(id2).x;
                int y2 = computers.at(id2).y;
                    int x, y;
                    if(x1==x2){
                        x = x1;
                        if(y1>y2){
                            y = y1-1;
                        }else{
                            y = y1+1;
                        }
                    }else if(y1==y2){
                        y = y1;
                        if(x1>x2){
                            x = x1-1;
                        }else{
                            x = x1+1;
                        }
                    }
                    cerr<<"id1:"<<id1<<endl;
                    rep(kk, 4){
                        int dx, dy; tie(dx, dy) = DIRS.at(kk);
                        int x2 = x1+dx;
                        int y2 = y1+dy;
                        if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)continue;
                        if(x2==x&&y2==y)continue;
                        if(board.at(x2).at(y2)!=0)continue;
                        if(painted.at(x2).at(y2)==1){
                            move_computer_(x1, y1, x2, y2);
                            commands.push_back(make_tuple(x1, y1, x2, y2));
                            dfs_connect();
                            goto OUT;
                        }
                    }
                    /*
                    move_computer_(x1, y1, x, y);
                    commands.push_back(make_tuple(x1, y1, x, y));
                    dfs_connect();
                    */
            }
            OUT:
            continue;
        }
    }
}

void Board::detect_regions(){
    uf_tree_region = dsu(N*N);
    vec_int visited(N*N, 0);

    int first_painted_pos=-1;
    rep(i, N)rep(j, N){
        if(painted.at(i).at(j)==1){
            if(first_painted_pos==-1){
                first_painted_pos = i*N+j;
            }else{
                uf_tree_region.merge(i*N+j, first_painted_pos);
            }
            continue;
        }
        int id = i * N + j;
        if(visited.at(id)==1)continue;

        queue<int> que;
        que.emplace(id);
        while(!que.empty()){
            int pos = que.front(); que.pop();
            if(visited.at(pos)==1)continue;
            visited.at(pos) = 1;
            int x = pos/N;
            int y = pos%N;

            rep(k, 4){
               int dx, dy; tie(dx, dy) = DIRS.at(k);
               int x2 = x + dx;
               int y2 = y + dy;
               if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)continue;
               if(painted.at(x2).at(y2))continue;
               int pos2 = x2*N + y2;
               uf_tree_region.merge(pos, pos2);
               que.push(pos2);
            }
        }
    }

    vector<vec_int> groups = uf_tree_region.groups();
    int group_size = 0;
    for(auto group: groups){
        if(uf_tree_region.leader(group.at(0))==uf_tree_region.leader(first_painted_pos))continue;
        if(group.size()>group_size){
            group_size = group.size();
            largest_region = group;
        }
    }
}

bool Board::can_connect_second_cable(int x1, int y1, int x2, int y2, vector<vec_int>&painted2){
    int flag = 0;
    if(x1==x2){
        int x=x1;
        int ymin = min(y1, y2);
        int ymax = max(y1, y2);
        for(int y=ymin+1;y<ymax;y++){
            /*
            if(x1==12){
                cerr<<painted2.at(x).at(y)<<" "<<x<<" "<<y<<endl;
            }
            */
            if(painted.at(x).at(y)==1 || painted2.at(x).at(y)==1){
                flag=1;
                break;
            }
        }
    }else if(y1==y2){
        int y=y1;
        int xmin = min(x1, x2);
        int xmax = max(x1, x2);
        for(int x=xmin+1;x<xmax;x++){
            if(painted.at(x).at(y)==1 || painted2.at(x).at(y)==1){
                flag=1;
                break;
            }
        }
    }else{
        return false;
    }
    return flag == 0;

}


void Board::paint_second_cable_tiles(int x1, int y1, int x2, int y2,  vector<vec_int> &painted2,  vector<vec_int> &influence_map){
    if(x1==x2){
        int x=x1;
        int ymin = min(y1, y2);
        int ymax = max(y1, y2);
        for(int y=ymin;y<=ymax;y++){
            painted2.at(x).at(y) = 1;
            if(y>ymin&&y<ymax){
                influence_map.at(x).at(y) = 1;
            }
        }
    }else{
        int y=y1;
        int xmin = min(x1, x2);
        int xmax = max(x1, x2);
        for(int x=xmin;x<=xmax;x++){
            painted2.at(x).at(y) = 1;

            if(x>xmin&&x<xmax){
                influence_map.at(x).at(y) = 1;
            }
        }
    }
}


vector<int> Board::find_nearest_second_computers(vector<vec_int> &influence_map, int new_color, vector<vec_int> &id_map){

    vector<int> result;
    queue<P> que;
    rep(i, N)rep(j, N){
        if(influence_map.at(i).at(j)==1){//1が入っているところは影響が及ぼせるところ
            que.emplace(i, j);
        }
    }
    int distance = 0;

    vector<vec_int> distance_map(N, vec_int(N, -1));

    while(result.size()==0){
        int que_size = que.size();
        if(que_size==0)break;

        rep(ii, que_size){
            int x, y; tie(x, y)= que.front(); que.pop();
            if(distance_map.at(x).at(y)!=-1)continue;
            distance_map.at(x).at(y) = distance;
            if(board.at(x).at(y)==new_color){
                result.push_back(id_map.at(x).at(y));
            }
            for(int k=0;k<4;k++){
                int dx, dy; tie(dx, dy) = DIRS.at(k);
                int x2 = x + dx;
                int y2 = y + dy;
                if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)continue;
                if(distance_map.at(x2).at(y2)!=-1)continue;
                if(board.at(x2).at(y2)!=0&&board.at(x2).at(y2)!=new_color)continue;
                if(influence_map.at(x2).at(y2)==2)continue; //既に繋がっているコンピュータ
                que.emplace(x2, y2);
            }
        }

        if(result.size()>0){
            // cerr<<"distance:"<<distance<<endl;
            return result;
        }
        distance++;
    }
    return result;
}


vector<T4> Board::bfs_second_nearest_influence_cell(vector<vec_int> &influence_map, int id, vector<Computer> &comps, bool crossing, vector<vec_int> &painted2){
    int x0 = comps.at(id).x;
    int y0 = comps.at(id).y;
    vector<vector<P>> visited(N, vector<P>(N, make_pair(-1, -1)));
    vector<T4> result;

    queue<T5> que;
    que.emplace(0, x0, y0, -1, -1);
    int shortest_distance = INT_MAX;
    vector<P> candidates;
    while(!que.empty()){
        int distance, x, y, px, py; tie(distance, x, y, px, py)=que.front(); que.pop();
        if(distance>shortest_distance)continue;
        // cerr<<x<<" "<<y<<" "<<px<<" "<<py<<endl;
        if(visited.at(x).at(y)!=make_pair(-1,-1))continue;
        visited.at(x).at(y) = make_pair(px, py);
        if(influence_map.at(x).at(y)==1){
            shortest_distance = min(shortest_distance, distance);
            candidates.push_back(make_pair(x, y));
            continue;
            /*
            // 終了
            int x1 = x;
            int y1 = y;

            while(true){
                int x2, y2; tie(x2, y2)= visited.at(x1).at(y1);
                if(x2==-1)break;
                result.push_back(make_tuple(x2, y2, x1, y1));
                x1 = x2;
                y1 = y2;
            }
            reverse(result.begin(), result.end());
            break;
            */
        }
        rep(k, 4){
            int dx, dy; tie(dx, dy) = DIRS.at(k);
            int x2 = x+dx;
            int y2 = y+dy;
            if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)continue;
            if(board.at(x2).at(y2)!=0)continue;
            if(visited.at(x2).at(y2)!=make_pair(-1,-1))continue;
            if(x2==x0&&y2==y0)continue;
            if(!crossing){
                //越境を許さない場合
                if(painted.at(x2).at(y2)==1)continue;
            }
            que.emplace(distance+1, x2, y2, x, y);
        }
    }
    if(candidates.size()==0)return result;

    int best_x = -1;
    int best_y = -1;
    int increment = -11;
    for(auto pos: candidates){
        int x1 = pos.first;
        int y1 = pos.second;
        int count = 0;
        rep(k, 4){
            int dx, dy; tie(dx, dy) = DIRS.at(k);

            int tmp = 1;
            while(true){
                int x2 = x1+dx*tmp;
                int y2 = y1+dy*tmp;
                if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)break;
                if(board.at(x2).at(y2)!=0&&!(x2==x0&&y2==y0))break;
                if(painted.at(x2).at(y2)==1)break;
                if(painted2.at(x2).at(y2)==1)break;
                if(influence_map.at(x2).at(y2)!=1)count++;
                tmp++;
            }
        }
        if(increment<count){
            increment = count;
            best_x = x1;
            best_y = y1;
            // cerr<<"increment:"<<count;
        }
    }

    int x1 = best_x;
    int y1 = best_y;

    while(true){
        int x2, y2; tie(x2, y2)= visited.at(x1).at(y1);
        if(x2==-1)break;
        result.push_back(make_tuple(x2, y2, x1, y1));
        x1 = x2;
        y1 = y2;
    }
    reverse(result.begin(), result.end());

    return result;
}

void Board::move_second_computer_(int x1, int y1, int x2, int y2, int new_color, vector<vec_int> &id_map, vector<Computer> &comps){
    if(board.at(x1).at(y1)==0 || board.at(x2).at(y2)!=0){
        string error_message = "cannnot move! " + to_string(x1) + " "+ to_string(y1) + " " + to_string(x2) + " " + to_string(y2);
        rep(i, N){
            rep(j, N){
                cerr<<board.at(i).at(j);
            }
            cerr<<endl;
        }
        throw runtime_error(error_message);
    }
    if(board.at(x1).at(y1) == new_color){
        int id = id_map.at(x1).at(y1);
        id_map.at(x1).at(y1) = -1;
        id_map.at(x2).at(y2) = id;
        comps.at(id).x = x2;
        comps.at(id).y = y2;
    }
    board.at(x2).at(y2) = board.at(x1).at(y1);
    board.at(x1).at(y1) = 0;
}

int Board::connect_in_largest_region(int new_color){
    int largest_region_id = uf_tree_region.leader(largest_region.at(0));
    second_color = new_color;

    int res = res_ope();
    // new_colorのコンピュータを取りまとめる
    vector<Computer> comps;
    vector<vec_int> id_map(N, vec_int(N, -1));
    rep(i,N){
        rep(j,N){
            if(board.at(i).at(j)==new_color){
                int id = comps.size();
                Computer compu = Computer(i, j, id, new_color);
                id_map.at(i).at(j) = id;
                comps.push_back(compu);
            }
        }
    }

    // 最も大きな連結成分を探す
    dsu connected_dsu(100);
    rep(i, N){
        rep(j, N){
            if(uf_tree_region.leader(i*N+j)!=largest_region_id)continue;
            if(board.at(i).at(j)!=new_color)continue;
            int start_id = id_map.at(i).at(j);

            queue<P> que;
            que.emplace(start_id, -1);
            int count = 0;

            set<int> used0;
            set<P> used_pair;
            while(que.size()>0&&res>0){
                int id, prev; tie(id, prev) = que.front(); que.pop();
                // if(used_pair.count(make_pair(id, prev))||used_pair.count(make_pair(prev, id))) continue;
                // if(connected.at(id)==1)continue;
                // connected.at(id) = 1;
                if(used0.count(id))continue;
                int x1 = comps.at(id).x;
                int y1 = comps.at(id).y;
                used0.insert(id);
                if(prev!=-1){
                    connected_dsu.merge(id, prev);
                }
                vec_int neighbors = comps.at(id).find_neighbors(board, id_map);
                for(int i=0;i<4;i++){
                    if(neighbors.at(i)==-1){
                        continue;
                    }
                    if(used0.count(neighbors.at(i)))continue;
                    int id2 = neighbors.at(i);
                    int x2 = comps.at(id2).x;
                    int y2 = comps.at(id2).y;
                    if(!can_connect_cable(x1, y1, x2, y2))continue;
                    que.emplace(id2, id);
                }
            }
        }
    }
    // cerr<<"connected size:"<<connected.groups().size()<<endl;
    vector<vec_int> groups = connected_dsu.groups();
    int largest_group_size = 0;
    vec_int largest_group;
    vec_int region_node;
    for(auto group: groups){
        if(group.size()>largest_group_size){
            largest_group_size = group.size();
            largest_group = group;
        }
        for(auto node:group)region_node.push_back(node);
    }

    // 最大のグループから探索を始めて徐々にくっつけて大きくしていく
    computer_connection2 = vector<vec_int>(comps.size());
    vec_int connected(comps.size(), 0);
    vector<vec_int> influence_map(N, vec_int(N, 0));
    queue<P> que;
    // ランダム性を持たせる
    int start_node;
    if(ro.get_rand(0, 2)==0){
        start_node= region_node.at(ro.get_rand(0, region_node.size()));
    }else{
        start_node = largest_group.at(0);
    }
    que.emplace(start_node, -1);
    int count = 0;
    set<int> used;
    
    vector<vec_int> painted2(N, vec_int(N, 0));
    int loop_count = 0;
    set<int> best_used;
    int best_count = 0;
    vector<vec_int> best_connection = vector<vec_int>(comps.size());
    vector<T4> best_commands;
    vector<Computer> best_comps = comps;
    vector<vec_int> best_board = board;
    vector<vec_int> best_painted = painted2;
    dsu second_uf_tree(100);

    while(res>0){
        /// cerr<<"loop_count:"<<loop_count<<" res:"<<res<<endl;
        // 最初にくっつけていく: 
        int connection_count = 0;
        influence_map = vector<vec_int>(N, vec_int(N, 0));
        while(que.size()>0){
            // cerr<<"loop_count:"<<loop_count<<" "<<res<<endl;
            int id, prev; tie(id, prev) = que.front(); que.pop();
            // cerr<<"id:"<<id<<" prev:"<<prev<<endl;
            if(connected.at(id)==1)continue;
            connected.at(id) = 1;
            int x1 = comps.at(id).x;
            int y1 = comps.at(id).y;
            used.insert(id);
            count++;
            if(prev!=-1){
                int px = comps.at(prev).x;
                int py = comps.at(prev).y;
                if(!can_connect_second_cable(x1, y1, px, py, painted2))continue;
                paint_second_cable_tiles(px, py, x1, y1, painted2, influence_map);
                used.insert(id);
                used.insert(prev);
                second_uf_tree.merge(id, prev);
                computer_connection2.at(id).push_back(prev);
                computer_connection2.at(prev).push_back(id);
                connection_count++;
                res--;
                if(res==0)goto OUT;
            }
            vec_int neighbors = comps.at(id).find_neighbors(board, id_map);
            for(int i=0;i<4;i++){
                if(neighbors.at(i)==-1){
                    continue;
                }
                // cerr<<"neighbor:"<<neighbors.at(i)<<endl;
                if(connected.at(neighbors.at(i))==1)continue;
                int id2 = neighbors.at(i);
                int x2 = comps.at(id2).x;
                int y2 = comps.at(id2).y;
                if(board.at(x2).at(y2)!=new_color)continue;
                // cerr<<"ok1"<<endl;
                if(!can_connect_cable(x1, y1, x2, y2))continue;
                // cerr<<"ok2"<<endl;
                if(!can_connect_second_cable(x1, y1, x2, y2, painted2))continue;
                // cerr<<"ok3"<<endl;
                que.emplace(id2, id);
            }
        }
        if(best_count<connection_count){
            best_count = connection_count;
            best_connection = computer_connection2;
            best_used = used;
            best_commands = commands;
            best_comps = comps;
            best_board = board;
            best_painted = painted2;
        }
        // if(loop_count==5)break;
        // if(loop_count==2)break;

            // 障害物をどかしてくっつけられないかを調べる
        vector<T4> obstacles;
        for(auto id1: used){
            int x1 = comps.at(id1).x;
            int y1 = comps.at(id1).y;
            influence_map.at(x1).at(y1) = 2; // 2はコンピュータがおいてあるところ

            //influence mapを作る
            /*
            if(x1==41&&y1==41){
                    cerr<<"board:"<<endl;
                    rep(i, N){
                        rep(j, N){
                            cerr<<influence_map.at(i).at(j);
                        }
                        cerr<<endl;
                    }
            }
            */

            rep(k, 4){
                int dx, dy; tie(dx, dy) = DIRS.at(k);
                int distance = 1;
                while(true){
                    int x2 = x1+dx*distance;
                    int y2 = y1+dy*distance;
                    if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)break;
                    if(painted.at(x2).at(y2)==1)break;
                    if(painted2.at(x2).at(y2)==1)break;
                    if(board.at(x2).at(y2)==new_color)break;
                    if(board.at(x2).at(y2)==0){
                        influence_map.at(x2).at(y2) = 1;
                        distance++;
                        continue;
                    }else{
                        obstacles.push_back(make_tuple(x2, y2, id1, k)); //id1の流れを堰き止める邪魔者になっている
                        break;
                    }
                }
            }
        }
        for(auto pos: obstacles){
            int xpos, ypos, block_id, direction; tie(xpos, ypos, block_id, direction) = pos;
            int counterpart = move_second_obstacle(xpos, ypos, block_id, comps, id_map, influence_map, new_color, painted2);
            if(used.count(counterpart)){
                cancel_move();
                continue;
            }
            if(counterpart>=0){
                // que.emplace(counterpart, block_id);
                que.emplace(start_node, -1);
                second_uf_tree = dsu(100);
                // influence_map = vector<vec_int>(N, vec_int(N, 0));
                set<int> new_used;
                used = new_used;
                connected = vector<int>(comps.size(), 0);
                // res += connection_count;
                res = res_ope();
                computer_connection2 = vector<vec_int>(comps.size());
                painted2 = vector<vec_int> (N, vec_int(N, 0));
                count=0;

                if(res==0) goto OUT;
                break;
            }
        }




        // 既に結合されているやつを動かしたときに新しい結合ができればOK
        if(que.size()==0&&false){
        if(move_second_connected_computers(1, comps, id_map, computer_connection2, second_uf_tree, painted2)){
            que.emplace(start_node, -1);
            second_uf_tree = dsu(100);
            // influence_map = vector<vec_int>(N, vec_int(N, 0));
            set<int> new_used;
            used = new_used;
            connected = vector<int>(comps.size(), 0);
            // res += connection_count;
            res = res_ope();
            computer_connection2 = vector<vec_int>(comps.size());
            painted2 = vector<vec_int> (N, vec_int(N, 0));
            count=0;
            continue;
        }
        }



        // 一回の移動でくっつくやつは先につけてしまう
        if(que.size()==0){
            loop_count++;
            vector<int> nearest_ids = find_nearest_second_computers(influence_map, new_color, id_map);
            // cerr<<"nearest_ids:"<<nearest_ids.size()<<endl;
            int flag =0;
            for(auto id11: nearest_ids){
                if(used.count(id11))continue;
                vector<T4> coms = bfs_second_nearest_influence_cell(influence_map, id11, comps, false, painted2);
                if(coms.size()==0)continue;
                if(coms.size()>1)continue;
                for(auto command: coms){
                    int a, b, c, d; tie(a, b, c, d) = command;
                    move_second_computer_(a, b, c, d, new_color, id_map, comps);
                    commands.push_back(make_tuple(a, b, c, d));
                    res--;
                    if(res<=0)goto OUT;
                }
                flag = 1;
                break;
                // que.push(id11);
            }
            if(flag == 1){
                que.emplace(start_node, -1);
                second_uf_tree = dsu(100);
                // influence_map = vector<vec_int>(N, vec_int(N, 0));
                set<int> new_used;
                used = new_used;
                connected = vector<int>(comps.size(), 0);
                // res += connection_count;
                res = res_ope();
                computer_connection2 = vector<vec_int>(comps.size());
                painted2 = vector<vec_int> (N, vec_int(N, 0));
                count=0;
                continue;
            }
        }



        // 障害物を一個どかすけれど、そのときに先読みして、どかした結果次に1回の移動で繋げられればOK
        if(que.size()==0){
            for(auto pos: obstacles){
                int xpos, ypos, block_id, direction; tie(xpos, ypos, block_id, direction) = pos;
               if(move_second_obstacle2(xpos, ypos, block_id, comps, id_map, influence_map, new_color, painted2, direction, used, 1)){ //1回の移動
                    res--;
                    if(res==0) goto OUT;

                    que.emplace(start_node, -1);
                    second_uf_tree = dsu(100);
                    // influence_map = vector<vec_int>(N, vec_int(N, 0));
                    set<int> new_used;
                    used = new_used;
                    connected = vector<int>(comps.size(), 0);
                    // res += connection_count;
                    res = res_ope();
                    computer_connection2 = vector<vec_int>(comps.size());
                    painted2 = vector<vec_int> (N, vec_int(N, 0));
                    count=0;

                   break;
                }
            }
        }

        // 2回障害物を移動する場合
        if(que.size()==0){
        int move_count = move_second_obstacle_strong(2, 5, painted, used, id_map, comps);
        if(move_count>0&&move_count<=res){
            res-=move_count;
            que.emplace(start_node, -1);
            second_uf_tree = dsu(100);
            // influence_map = vector<vec_int>(N, vec_int(N, 0));
            set<int> new_used;
            used = new_used;
            connected = vector<int>(comps.size(), 0);
            // res += connection_count;
            res = res_ope();
            computer_connection2 = vector<vec_int>(comps.size());
            painted2 = vector<vec_int> (N, vec_int(N, 0));
            count=0;
        }else{
            rep(iii, move_count){
                cancel_move();
            }
        }
        }

        // 障害物を一個どかすけれど、そのときに先読みして、どかした結果次に1回の移動で繋げられればOK
        if(que.size()==0){
            for(auto pos: obstacles){
                int xpos, ypos, block_id, direction; tie(xpos, ypos, block_id, direction) = pos;
               if(move_second_obstacle2(xpos, ypos, block_id, comps, id_map, influence_map, new_color, painted2, direction, used, 2)){ //1回の移動
                    res--;
                    if(res==0) goto OUT;

                    que.emplace(start_node, -1);
                    second_uf_tree = dsu(100);
                    // influence_map = vector<vec_int>(N, vec_int(N, 0));
                    set<int> new_used;
                    used = new_used;
                    connected = vector<int>(comps.size(), 0);
                    // res += connection_count;
                    res = res_ope();
                    computer_connection2 = vector<vec_int>(comps.size());
                    painted2 = vector<vec_int> (N, vec_int(N, 0));
                    count=0;

                   break;
                }
            }
        }


        /*
        if(que.size()==0){
            // NOTE: ここが本当に必要かは後で検証する
            for(auto pos: obstacles){
                int xpos, ypos, block_id, direction; tie(xpos, ypos, block_id, direction) = pos;
               if(move_second_obstacle2(xpos, ypos, block_id, comps, id_map, influence_map, new_color, painted2, direction, used, 2)){ //1回の移動
                    res--;
                    if(res==0) goto OUT;

                    que.emplace(start_node, -1);
            second_uf_tree = dsu(100);
                    // influence_map = vector<vec_int>(N, vec_int(N, 0));
                    set<int> new_used;
                    used = new_used;
                    connected = vector<int>(comps.size(), 0);
                    // res += connection_count;
                    res = res_ope();
                    computer_connection2 = vector<vec_int>(comps.size());
                    painted2 = vector<vec_int> (N, vec_int(N, 0));
                    count=0;

                   break;
                }
            }
        }
        */
        /*
        if(que.size()==0){
            for(auto pos: obstacles){
                int xpos, ypos, block_id, direction; tie(xpos, ypos, block_id, direction) = pos;
               if(move_second_obstacle2(xpos, ypos, block_id, comps, id_map, influence_map, new_color, painted2, direction, used, 3)){ //1回の移動
                    res--;
                    if(res==0) goto OUT;

                    que.emplace(start_node, -1);
                    // influence_map = vector<vec_int>(N, vec_int(N, 0));
                    set<int> new_used;
                    used = new_used;
                    connected = vector<int>(comps.size(), 0);
                    // res += connection_count;
                    res = res_ope();
                    computer_connection2 = vector<vec_int>(comps.size());
                    painted2 = vector<vec_int> (N, vec_int(N, 0));
                    count=0;

                   break;
                }
            }
        }
        */



        // cerr<<"que size:"<<que.size()<<" res:"<<res<<endl;

        if(que.size()==0){
            // 越境をまず許さないようにする
            loop_count++;
            vector<int> nearest_ids = find_nearest_second_computers(influence_map, new_color, id_map);
            // cerr<<"nearest_ids:"<<nearest_ids.size()<<endl;
            int flag =0;
            for(auto id11: nearest_ids){
                if(used.count(id11))continue;
                vector<T4> coms = bfs_second_nearest_influence_cell(influence_map, id11, comps, false, painted2);
                if(coms.size()==0)continue;
                if(coms.size()>=8)continue;
                for(auto command: coms){
                    int a, b, c, d; tie(a, b, c, d) = command;
                    move_second_computer_(a, b, c, d, new_color, id_map, comps);
                    commands.push_back(make_tuple(a, b, c, d));
                    res--;
                    if(res<=0)goto OUT;
                }
                flag = 1;
                break;
                // que.push(id11);
            }
            if(flag == 1){
                que.emplace(start_node, -1);
                second_uf_tree = dsu(100);
                // influence_map = vector<vec_int>(N, vec_int(N, 0));
                set<int> new_used;
                used = new_used;
                connected = vector<int>(comps.size(), 0);
                // res += connection_count;
                res = res_ope();
                computer_connection2 = vector<vec_int>(comps.size());
                painted2 = vector<vec_int> (N, vec_int(N, 0));
                count=0;
            }
        }

        if(que.size()==0){
            // NOTE: ここが本当に必要かは後で検証する
            for(auto pos: obstacles){
                int xpos, ypos, block_id, direction; tie(xpos, ypos, block_id, direction) = pos;
               if(move_second_obstacle2(xpos, ypos, block_id, comps, id_map, influence_map, new_color, painted2, direction, used, 7)){ //1回の移動
                    res--;
                    if(res==0) goto OUT;

                    que.emplace(start_node, -1);
                    second_uf_tree = dsu(100);
                    // influence_map = vector<vec_int>(N, vec_int(N, 0));
                    set<int> new_used;
                    used = new_used;
                    connected = vector<int>(comps.size(), 0);
                    // res += connection_count;
                    res = res_ope();
                    computer_connection2 = vector<vec_int>(comps.size());
                    painted2 = vector<vec_int> (N, vec_int(N, 0));
                    count=0;

                   break;
                }
            }
        }

        if(que.size()==0){
            // 越境を許す
            loop_count++;
            vector<int> nearest_ids = find_nearest_second_computers(influence_map, new_color, id_map);
            // cerr<<"nearest_ids:"<<nearest_ids.size()<<endl;
            int flag =0;
            for(auto id11: nearest_ids){
                if(used.count(id11))continue;
                vector<T4> coms = bfs_second_nearest_influence_cell(influence_map, id11, comps, true, painted2);
                if(coms.size()==0)continue;
                for(auto command: coms){
                    int a, b, c, d; tie(a, b, c, d) = command;
                    move_second_computer_(a, b, c, d, new_color, id_map, comps);
                    commands.push_back(make_tuple(a, b, c, d));
                    res--;
                    if(res<=0)goto OUT;
                }
                flag = 1;
                break;
                // que.push(id11);
            }
            if(flag == 1){
                que.emplace(start_node, -1);
                second_uf_tree = dsu(100);
                // influence_map = vector<vec_int>(N, vec_int(N, 0));
                set<int> new_used;
                used = new_used;
                connected = vector<int>(comps.size(), 0);
                // res += connection_count;
                res = res_ope();
                computer_connection2 = vector<vec_int>(comps.size());
                painted2 = vector<vec_int> (N, vec_int(N, 0));
                count=0;
            }
        }

        if(que.size()==0){
            break;
        }
    }
    OUT:
    second_computers = best_comps;
    used_second_computers = best_used;
    computer_connection2 = best_connection;
    commands = best_commands;
    board = best_board;
    second_painted = best_painted;

    return (best_count*(best_count+1))/2;



/*
    for(auto rpos: largest_region){
        int x = rpos/N;
        int y = rpos%N;
        if(board.at(x).at(y)==new_color){
            int id = comps.size();
            Computer compu = Computer(x, y, id, new_color);
            id_map.at(x).at(y) = id;
            comps.push_back(compu);
        }
    }
    cerr<<"comps.size:"<<comps.size()<<endl;
    if(comps.size()==0)return 0;
    computer_connection2 = vector<vec_int>(comps.size());

    vec_int connected(comps.size(), 0);
    vector<vec_int> influence_map(N, vec_int(N, 0));
    queue<P> que;
    que.emplace(0, -1);
    int count = 0;

    set<int> used;
    set<P> used_pair;
    while(que.size()>0&&res>0){
        int id, prev; tie(id, prev) = que.front(); que.pop();
        // if(used_pair.count(make_pair(id, prev))||used_pair.count(make_pair(prev, id))) continue;
        if(connected.at(id)==1)continue;
        connected.at(id) = 1;
        int x1 = comps.at(id).x;
        int y1 = comps.at(id).y;
        used.insert(id);
        count++;
        if(prev!=-1){
            computer_connection2.at(id).push_back(prev);
            computer_connection2.at(prev).push_back(id);
            res--;
            if(res==0)goto OUT;
        }
        vec_int neighbors = comps.at(id).find_neighbors(board, id_map);
        for(int i=0;i<4;i++){
            if(neighbors.at(i)==-1){
                continue;
            }
            if(connected.at(neighbors.at(i))==1)continue;
            int id2 = neighbors.at(i);
            int x2 = comps.at(id2).x;
            int y2 = comps.at(id2).y;
            if(!can_connect_cable(x1, y1, x2, y2))continue;
            que.emplace(id2, id);
        }
    }
    OUT:
    second_computers = comps;
    used_second_computers = used;
    return (count*(count-1))/2;
    */
}

int Board::move_second_obstacle(int xpos, int ypos, int blocked_id, vector<Computer> &comps, vector<vec_int> &id_map, vector<vec_int> &influence_map, int new_color, vector<vec_int> &painted2){
    vector<P> dirs = {make_pair(1, 0), make_pair(0, 1)};
    vector<T5> operation_candidates;
//     rep(x, N){
//         rep(y, N){
    int x = xpos;
    int y = ypos;
    if(!can_use_outside&&(x==0||x==N-1||y==0||y==N-1))return -1;
    rep(k, 2){
        int dx, dy; tie(dx, dy) = dirs.at(k); // k=0の時: 縦方向の探索, 1の時: 横方向の探索
        int ind = 1;
        int flag1 = 0;
        int computer1 = -1;
        while(true){
            int x2 = x + dx*ind;
            int y2 = y + dy*ind;
            if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)break;
            if(painted.at(x2).at(y2)==1&&board.at(x2).at(y2)==0)break;
            if(board.at(x2).at(y2)!=0&&board.at(x2).at(y2)!=new_color)break;
            if(board.at(x2).at(y2)==new_color){
                flag1 = 1;
                computer1 = id_map.at(x2).at(y2);
                break;
            }else{
                ind++;
            }
        }
        if(flag1==0)continue;

        int flag2 = 0;
        ind = -1;
        int computer2 = -1;
        while(true){
            int x2 = x + dx*ind;
            int y2 = y + dy*ind;
            if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)break;
            if(painted.at(x2).at(y2)==1&&board.at(x2).at(y2)==0)break;
            if(board.at(x2).at(y2)!=0&&board.at(x2).at(y2)!=new_color)break;
            if(board.at(x2).at(y2)==new_color){
                flag2 = 1;
                computer2 = id_map.at(x2).at(y2);
                break;
            }else{
                ind--;
            }
        }
        if(flag2==0)continue;
        // cerr<<"counter1:"<<computer1<<" counter2:"<<computer2<<endl;
        if(computer1 == blocked_id|| computer2==blocked_id){
           operation_candidates.push_back(make_tuple(k, x, y, computer1, computer2));
           break;
        }
        // if(uf_tree.same(computer1, computer2))continue;
        // operation_candidates.push_back(make_tuple(k, x, y, computer1, computer2));
        // break;
    }
//         }
//     }
    // cerr<<"operation_candidates:"<<operation_candidates.size()<<endl;
    for(auto tmp: operation_candidates){
        int direc, x, y, id1, id2; tie(direc, x, y, id1, id2)= tmp;
        // cerr<<direc<<" "<<x<<" "<<y<<" "<<id1<<" "<<id2<<endl;
        // cerr<<computers.at(id1).x<<" "<<computers.at(id1).y<<endl;
        // cerr<<computers.at(id2).x<<" "<<computers.at(id2).y<<endl;
       //direc = 0の時にはコンピュータを横方向に移動させたい
       //direc = 1の時にはコンピュータを縦方向に移動させたい
       for(int i=0;i<2;i++){
           int dx, dy; tie(dx, dy) = DIRS.at(i*2+direc);
           int x2 = x + dx;
           int y2 = y + dy;
           if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)break;
           if(board.at(x2).at(y2)==0&&influence_map.at(x2).at(y2)==0&&painted.at(x2).at(y2)==0&&painted2.at(x2).at(y2)==0){
               move_computer_(x, y, x2, y2);
               commands.push_back(make_tuple(x, y, x2, y2));
               if(id1==blocked_id)return id2;
               if(id2==blocked_id)return id1;
               // paint_cable_tiles(computers.at(id1).x, computers.at(id1).y, computers.at(id2).x, computers.at(id2).y);
               goto OUT;
           }
       }
       OUT:
       continue;
    }
    return -1;
}

bool Board::move_second_obstacle2(int xpos, int ypos, int blocked_id, vector<Computer> &comps, vector<vec_int> &id_map, vector<vec_int> &influence_map, int new_color, vector<vec_int> &painted2, int direction, set<int> &used, int limit_distance){
    // 障害物をどかすけれど、どかした結果、influencemapの一個横に浮いているコンピュータがあったらOK
    vector<P> dirs = {make_pair(1, 0), make_pair(0, 1)};
    vector<T5> operation_candidates;
    int x = xpos;
    int y = ypos;
    int dx, dy; tie(dx, dy) = DIRS.at(direction);
    int ind = 1;
    while(true){
        int x2 = x+dx*ind;
        int y2 = y+dy*ind;
        ind++;
        if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)break;
        if(painted.at(x2).at(y2)==1&&board.at(x2).at(y2)==0)break;
        if(board.at(x2).at(y2)!=0)break;
        if(limit_distance==1){
            rep(k, 4){
                //周辺にnew colorのますがないか
                if(k%2==direction%2)continue;
                int dx2, dy2; tie(dx2, dy2) = DIRS.at(k);
                int x3 = x2+dx2;
                int y3 = y2+dy2;
                if (x3 < 0 || x3 >= N || y3 < 0 || y3 >= N)continue;
                if(board.at(x3).at(y3)!=new_color)continue;
                int id3 = id_map.at(x3).at(y3);
                if(used.count(id3))continue;

                // というわけでここまできたら別方向にこの石をどかす
                rep(jk, 4){
                    if(jk%2==direction%2)continue;
                    int dx3, dy3; tie(dx3, dy3) = DIRS.at(jk);
                    int x4 = x+dx3;
                    int y4 = y+dy3;
                    if (x4 < 0 || x4 >= N || y4 < 0 || y4 >= N)continue;;
 
                    if(board.at(x4).at(y4)==0&&influence_map.at(x4).at(y4)==0&&painted.at(x4).at(y4)==0&&painted2.at(x4).at(y4)==0){
                        move_second_computer_(x, y, x4, y4, second_color, id_map, comps);
                        commands.push_back(make_tuple(x, y, x4, y4));
                        return true;
                    }
                }
            }
        }else if(limit_distance>=2){
            // 周辺2マス以内の空白を辿っていける位置にnew_colorで使っていないやつがあればおk
            queue<T5> que;
            que.emplace(0, x, y, -1, -1);

            map<P, P> visited;
            while(!que.empty()){
                int dist, xpos, ypos, px, py; tie(dist, xpos, ypos, px, py) = que.front(); que.pop();
                if(visited.count(make_pair(xpos, ypos)))continue;
                visited[make_pair(xpos, ypos)] = make_pair(px, py);
                if(board.at(xpos).at(ypos)==new_color&&!used.count(id_map.at(xpos).at(ypos))){
                    // コンピュータをどかす
                    rep(jk, 4){
                        if(jk%2==direction%2)continue;
                        int dx3, dy3; tie(dx3, dy3) = DIRS.at(jk);
                        int x4 = x+dx3;
                        int y4 = y+dy3;
                        if (x4 < 0 || x4 >= N || y4 < 0 || y4 >= N)continue;;
    
                        if(board.at(x4).at(y4)==0&&influence_map.at(x4).at(y4)==0&&painted.at(x4).at(y4)==0&&painted2.at(x4).at(y4)==0){
                            move_second_computer_(x, y, x4, y4, second_color, id_map, comps);
                            commands.push_back(make_tuple(x, y, x4, y4));
                            return true;
                        }
                    }
                }
                if(board.at(xpos).at(ypos)!=0&&!(xpos==x&&ypos==y))continue;
                if(painted.at(xpos).at(ypos)!=0||painted2.at(xpos).at(ypos)!=0)continue;
                if(dist==limit_distance)continue;
                rep(k, 4){
                    //周辺にnew colorのますがないか
                    // if(k%2==direction%2)continue;
                    int dx2, dy2; tie(dx2, dy2) = DIRS.at(k);
                    int x3 = xpos+dx2;
                    int y3 = ypos+dy2;
                    if (x3 < 0 || x3 >= N || y3 < 0 || y3 >= N)continue;
                    if(painted2.at(x3).at(y3)!=0)continue;
                    if(board.at(x3).at(y3)!=0&&board.at(x3).at(y3)!=new_color)continue;
                    que.emplace(dist+1, x3, y3, xpos, ypos);

                }
            }
            
            /*
            rep(k, 4){
                //周辺にnew colorのますがないか
                if(k%2==direction%2)continue;
                int dx2, dy2; tie(dx2, dy2) = DIRS.at(k);
                int x3 = x2+dx2;
                int y3 = y2+dy2;
                if (x3 < 0 || x3 >= N || y3 < 0 || y3 >= N)continue;
                if(board.at(x3).at(y3)!=new_color)continue;
                int id3 = id_map.at(x3).at(y3);
                if(used.count(id3))continue;

                // というわけでここまできたら別方向にこの石をどかす
                rep(jk, 4){
                    if(jk%2==direction%2)continue;
                    int dx3, dy3; tie(dx3, dy3) = DIRS.at(jk);
                    int x4 = x+dx3;
                    int y4 = y+dy3;
                    if (x4 < 0 || x4 >= N || y4 < 0 || y4 >= N)continue;;
 
                    if(board.at(x4).at(y4)==0&&influence_map.at(x4).at(y4)==0&&painted.at(x4).at(y4)==0&&painted2.at(x4).at(y4)==0){
                        move_second_computer_(x, y, x4, y4, second_color, id_map, comps);
                        commands.push_back(make_tuple(x, y, x4, y4));
                        return true;
                    }
                }
            }
            */
        }

    }
    /*
    for(auto tmp: operation_candidates){
        int direc, x, y, id1, id2; tie(direc, x, y, id1, id2)= tmp;
        // cerr<<direc<<" "<<x<<" "<<y<<" "<<id1<<" "<<id2<<endl;
        // cerr<<computers.at(id1).x<<" "<<computers.at(id1).y<<endl;
        // cerr<<computers.at(id2).x<<" "<<computers.at(id2).y<<endl;
       //direc = 0の時にはコンピュータを横方向に移動させたい
       //direc = 1の時にはコンピュータを縦方向に移動させたい
       for(int i=0;i<2;i++){
           int dx, dy; tie(dx, dy) = DIRS.at(i*2+direc);
           int x2 = x + dx;
           int y2 = y + dy;
           if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)break;
           if(board.at(x2).at(y2)==0&&influence_map.at(x2).at(y2)==0&&painted.at(x2).at(y2)==0&&painted2.at(x2).at(y2)==0){
               move_computer_(x, y, x2, y2);
               commands.push_back(make_tuple(x, y, x2, y2));
               if(id1==blocked_id)return id2;
               if(id2==blocked_id)return id1;
               // paint_cable_tiles(computers.at(id1).x, computers.at(id1).y, computers.at(id2).x, computers.at(id2).y);
               goto OUT;
           }
       }
       OUT:
       continue;
    }
    */
    return false;
}

void Board::move_surrounding_computers(int region_size){
    //　これは最初にcolorのコンピュータを外周から排除するメソッド
    // outside_region = ro.get_rand(1, 3);
    outside_region = region_size;
    can_use_outside = false;
    rep(i, computers.size()){
        int x = computers.at(i).x;
        int y = computers.at(i).y;
        if(!is_outside(x, y))continue;

        queue<T4> que;
        que.emplace(x, y, -1, -1);
        map<P, P> pos_map;

        int flag = 0;

        while(que.size()>0){
            int x1, y1, px, py; tie(x1, y1, px, py) = que.front(); que.pop();
            // cerr<<x1<<" "<<y1<<" "<<px<<" "<<py<<endl;
            if(pos_map.count(make_pair(x1, y1)))continue;
            pos_map[make_pair(x1, y1)] = make_pair(px, py);
            if(!is_outside(x1, y1)){

                int pos_x = x1;
                int pos_y = y1;
                vector<T4> coms;
                while(true){
                    int x2, y2; tie(x2, y2) = pos_map[make_pair(pos_x, pos_y)];
                    if(x2==-1)break;
                    coms.push_back(make_tuple(x2, y2, pos_x, pos_y));
                    pos_x = x2;
                    pos_y = y2;
                }
                if(coms.size()>5)break;
                reverse(coms.begin(), coms.end());

                for(auto com: coms){
                    int a, b, c, d; tie(a,b,c,d) = com;
                    move_computer_(a, b, c, d);
                    commands.push_back(make_tuple(a, b, c, d));
                }
                flag = 1;
                break;
            }

            rep(kk, 4){
                int dx, dy; tie(dx, dy) = DIRS.at(kk);
                int x2 = x1+dx;
                int y2 = y1+dy;
                if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)continue;
                if(pos_map.count(make_pair(x2, y2)))continue;
                if(board.at(x2).at(y2)!=0)continue;
                que.emplace(x2, y2, x1, y1);
            }
        }
        if(flag==1)continue;
        // まず一番端から追いやる
        if(x==0||x==N-1){
            int dest_x = -1;
            int dest_y = -1;
            if(x==0){
                dest_x = 1;
                dest_y = y;
            }else if(x==N-1){
                dest_x = N-2;
                dest_y = y;
            }
            if(board.at(dest_x).at(dest_y)!=0){
                fill_vacant_at(x, y, dest_x, dest_y);
            }
            move_computer_(x,y,dest_x,dest_y);
            commands.push_back(make_tuple(x,y,dest_x,dest_y));
            x = dest_x;
        }
        if(y==0||y==N-1){
            int dest_x = -1;
            int dest_y = -1;
            if(y==0){
                dest_x = x;
                dest_y = 1;
            }else if(y==N-1){
                dest_x = x;
                dest_y = N-2;
            }
            if(board.at(dest_x).at(dest_y)!=0){
                fill_vacant_at(x, y, dest_x, dest_y);
            }
            move_computer_(x,y,dest_x,dest_y);
            commands.push_back(make_tuple(x,y,dest_x,dest_y));
            y = dest_y;
        }

        // 次にその次の層に対応する
        if(x==1||x==N-2){
            int dest_x = -1;
            int dest_y = -1;
            if(x==1){
                dest_x = 2;
                dest_y = y;
            }else if(x==N-2){
                dest_x = N-3;
                dest_y = y;
            }
            if(board.at(dest_x).at(dest_y)!=0){
                fill_vacant_at(x, y, dest_x, dest_y);
            }
            move_computer_(x,y,dest_x,dest_y);
            commands.push_back(make_tuple(x,y,dest_x,dest_y));
            x = dest_x;
        }
        if(y==1||y==N-2){
            int dest_x = -1;
            int dest_y = -1;
            if(y==1){
                dest_x = x;
                dest_y = 2;
            }else if(y==N-2){
                dest_x = x;
                dest_y = N-3;
            }
            if(board.at(dest_x).at(dest_y)!=0){
                fill_vacant_at(x, y, dest_x, dest_y);
            }
            move_computer_(x,y,dest_x,dest_y);
            commands.push_back(make_tuple(x,y,dest_x,dest_y));
            y = dest_y;
        }
    }
}

void Board::arrange_outer_computers(int new_color, int ignores){
    can_use_outside = false;

    for(int x=0;x<N;x++){
        for(int y=0;y<N;y++){
            if(!is_outside(x, y))continue;
            if(board.at(x).at(y)==0||board.at(x).at(y)==new_color)continue;
            if(x==0||x==N-1||y==0||y==N-1)continue;

            int x2 = -1;
            int y2 = -1;
            if(x==1||x==N-2){
                if(x==1){
                    x2 = 0;
                }else{
                    x2 = N-1;
                }
                y2 = y;
            }else if(y==1||y==N-2){
                x2 = x;
                if(y==1){
                    y2 = 0;
                }else{
                    y2 = N-1;
                }
            }
            if(board.at(x2).at(y2)==0&&painted.at(x2).at(y2)==0){
                move_computer_(x, y, x2, y2);
                commands.push_back(make_tuple(x, y, x2, y2));
            }
        }
    }


    // 外周をいい感じにする
    for(int x=0;x<N;x++){
        for(int y=0;y<N;y++){
            if(!is_outside(x, y))continue;
            int val =board.at(x).at(y); 
            if(val==0||val==color)continue;
            if(val==new_color&&(x==0||x==N-1||y==0||y==N-1))continue;

            queue<T4> que;
            que.emplace(x, y, -1, -1);
            map<P, P> pos_map;

            while(que.size()>0){
                int x1, y1, px, py; tie(x1, y1, px, py) = que.front(); que.pop();
                // cerr<<x1<<" "<<y1<<" "<<px<<" "<<py<<endl;
                if(pos_map.count(make_pair(x1, y1)))continue;
                pos_map[make_pair(x1, y1)] = make_pair(px, py);
                int flag0 = 0;
                if(val==new_color){
                    if(outside_region==2){
                        if((x1==1||x1==N-2||y1==1||y1==N-2)&&!(x1==0||x1==N-1||y1==0||y1==N-1))flag0=1;
                    }else{
                        if((x1==0||x1==N-1||y1==0||y1==N-1))flag0=1;
                    }
                }else{
                    if(outside_region==2){
                        if(!is_outside(x1,y1)||x1==0||x1==N-1||y1==0||y1==N-1)flag0=1;
                    }else{
                        if(!is_outside(x1,y1)){
                            flag0=1;
                        }
                    }
                }
                // if(!is_outside(x1, y1)){
                if(flag0==1&&painted.at(x1).at(y1)==0){
                    int pos_x = x1;
                    int pos_y = y1;
                    vector<T4> coms;
                    while(true){
                        int x2, y2; tie(x2, y2) = pos_map[make_pair(pos_x, pos_y)];
                        if(x2==-1)break;
                        coms.push_back(make_tuple(x2, y2, pos_x, pos_y));
                        pos_x = x2;
                        pos_y = y2;
                    }
                    reverse(coms.begin(), coms.end());

                    for(auto com: coms){
                        int a, b, c, d; tie(a,b,c,d) = com;
                        move_computer_(a, b, c, d);
                        commands.push_back(make_tuple(a, b, c, d));
                    }
                    break;
                }

                rep(kk, 4){
                    int dx, dy; tie(dx, dy) = DIRS.at(kk);
                    int x2 = x1+dx;
                    int y2 = y1+dy;
                    if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)continue;
                    if(pos_map.count(make_pair(x2, y2)))continue;
                    if(board.at(x2).at(y2)!=0)continue;
                    que.emplace(x2, y2, x1, y1);
                }
            }
        }
    }

    // vector<int> x_vec = {1, 1, N-2, N-2};
    // vector<int> y_vec = {1, N-2, 1, N-2};
    vector<int> x_vec, y_vec;
    if(outside_region==1){
        x_vec = {0, 0, N-1, N-1};
        y_vec = {0, N-1, 0, N-1};
    }else{
        x_vec = {1, 1, N-2, N-2};
        y_vec = {1, N-2, 1, N-2};
    }


    rep(ii, 4){
       if(ii==ignores)continue;
       int goal_x = x_vec.at(ii);
       int goal_y = y_vec.at(ii);
       // if(!is_outside(x, y))continue;
       int val =board.at(goal_x).at(goal_y); 
       if(val==new_color)continue;
       if(val!=0&&val!=new_color)continue;

        queue<T4> que;
        que.emplace(goal_x, goal_y, -1, -1);
        map<P, P> pos_map;

        while(que.size()>0){
            int x1, y1, px, py; tie(x1, y1, px, py) = que.front(); que.pop();
            if(pos_map.count(make_pair(x1, y1)))continue;
            pos_map[make_pair(x1, y1)] = make_pair(px, py);
            if(board.at(x1).at(y1)==new_color){
                // cerr<<"ok"<<endl;
                int pos_x = x1;
                int pos_y = y1;
                vector<T4> coms;
                while(true){
                    int x2, y2; tie(x2, y2) = pos_map[make_pair(pos_x, pos_y)];
                    if(x2==-1)break;
                    coms.push_back(make_tuple(pos_x, pos_y, x2, y2));
                    pos_x = x2;
                    pos_y = y2;
                }
                // reverse(coms.begin(), coms.end());

                for(auto com: coms){
                    int a, b, c, d; tie(a,b,c,d) = com;
                    move_computer_(a, b, c, d);
                    commands.push_back(make_tuple(a, b, c, d));
                }
                break;
            }

            rep(kk, 4){
                int dx, dy; tie(dx, dy) = DIRS.at(kk);
                int x2 = x1+dx;
                int y2 = y1+dy;
                if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)continue;
                if(pos_map.count(make_pair(x2, y2)))continue;
                if(board.at(x2).at(y2)!=0&&board.at(x2).at(y2)!=new_color)continue;
                que.emplace(x2, y2, x1, y1);
            }
        }
    }
}

int min_cost_matching(vector<vec_int> &board, int color){
    vector<P> original_pos;
    vector<P> after_pos;
    rep(i, N)rep(j, N){
        if(c2.at(i).at(j)==color){
            original_pos.push_back(make_pair(i, j));
        }
        if(board.at(i).at(j)==color){
            after_pos.push_back(make_pair(i, j));
        }
    }
    mcf_graph<int, int> graph(202);
    rep(i, 100){
        rep(j, 100){
            int x1, y1; tie(x1, y1) = original_pos.at(i);
            int x2, y2; tie(x2, y2) = after_pos.at(j);
            int distance = abs(x2-x1)+abs(y2-y1);
            graph.add_edge(i, 100+j, 1, distance);
        }
    }

    rep(i, 100){
        graph.add_edge(200, i, 1, 0);
        graph.add_edge(100+i, 201, 1, 0);
    }

    P flow_result = graph.flow(200, 201);
    int result = flow_result.second;
    vector<T5> costs;
    rep(i, 100){
        rep(j, 100){
            auto edge_status = graph.get_edge(i*100+j);
            if(edge_status.flow>0){
                int x1, y1; tie(x1, y1) = original_pos.at(i);
                int x2, y2; tie(x2, y2) = after_pos.at(j);
                costs.push_back(make_tuple(edge_status.cost, x1, y1, x2, y2));
            }
        }
    }
    sort(costs.begin(), costs.end());
    reverse(costs.begin(), costs.end());
    //cerr<<"----------color:::"<<color<<"-------"<<endl;
    /*
    for(auto tmp: costs){
        int a, x1, y1, x2, y2; tie(a, x1, y1, x2, y2) = tmp;
        cerr<<"cost:"<<a<<" from:"<<x1<<" "<<y1<<" to:"<<x2<<" "<<y2<<endl;
    }
    */



    return result;
}


int cout_min_cost_matching(vector<vec_int> &board, int color){
    vector<P> original_pos;
    vector<P> after_pos;
    rep(i, N)rep(j, N){
        if(c2.at(i).at(j)==color){
            original_pos.push_back(make_pair(i, j));
        }
        if(board.at(i).at(j)==color){
            after_pos.push_back(make_pair(i, j));
        }
    }
    mcf_graph<int, int> graph(202);
    rep(i, 100){
        rep(j, 100){
            int x1, y1; tie(x1, y1) = original_pos.at(i);
            int x2, y2; tie(x2, y2) = after_pos.at(j);
            int distance = abs(x2-x1)+abs(y2-y1);
            graph.add_edge(i, 100+j, 1, distance);
        }
    }

    rep(i, 100){
        graph.add_edge(200, i, 1, 0);
        graph.add_edge(100+i, 201, 1, 0);
    }

    P flow_result = graph.flow(200, 201);
    int result = flow_result.second;
    vector<T5> costs;
    rep(i, 100){
        rep(j, 100){
            auto edge_status = graph.get_edge(i*100+j);
            if(edge_status.flow>0){
                int x1, y1; tie(x1, y1) = original_pos.at(i);
                int x2, y2; tie(x2, y2) = after_pos.at(j);
                costs.push_back(make_tuple(edge_status.cost, x1, y1, x2, y2));
            }
        }
    }
    sort(costs.begin(), costs.end());
    reverse(costs.begin(), costs.end());
    //cerr<<"----------color:::"<<color<<"-------"<<endl;
    /*
    for(auto tmp: costs){
        int a, x1, y1, x2, y2; tie(a, x1, y1, x2, y2) = tmp;
        cerr<<"cost:"<<a<<" from:"<<x1<<" "<<y1<<" to:"<<x2<<" "<<y2<<endl;
    }
    */
    cout<<100<<endl;
    for(auto tmp: costs){
        int a, x1, y1, x2, y2; tie(a, x1, y1, x2, y2) = tmp;
        cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
    }

    return result;
}



int Board::minimum_cost(){
    if(board.size()==0)return INT_MAX;
    int result = 0;
    // rep(i, K){
    //     result += min_cost_matching(board, i+1);
    // }
    result += min_cost_matching(board, color);
    result += min_cost_matching(board, second_color);

    return result;

}

void Board::cout_mcf(){
    /*
    cout<<N<<" "<<K<<endl;
    rep(i, N){
        rep(j, N){
            cout<<board.at(i).at(j);
        }
        cout<<endl;
    }
    */
   visualize();

    cout_min_cost_matching(board, color);
    cout_min_cost_matching(board, second_color);
}

void Board::fill_vacant_at(int x, int y, int dest_x, int dest_y){
    P vacant_cell = find_nearest_vacant_cell3(dest_x, dest_y, x, y);
    // cerr<<"vacant:"<<vacant_cell.first<<" "<<vacant_cell.second<<endl;

    int vx, vy; tie(vx,vy) = vacant_cell;
    queue<T4> que;
    que.emplace(vx, vy, -1, -1);
    map<P, P> visited_map;
    while(!que.empty()){
        int posx, posy, px, py; tie(posx, posy, px, py) = que.front(); que.pop();
        // cerr<<"posx,posy:"<<posx<<" "<<posy<<endl;
        if(visited_map.count(make_pair(posx, posy)))continue;
        visited_map[make_pair(posx, posy)] = make_pair(px, py);
        // if(board.at(posx).at(posy)==0){
        if(posx==dest_x&&posy==dest_y){
            vector<T4> tmp_vec; 
            while(true){
                int xx, yy; 
                tie(xx, yy) = visited_map[make_pair(posx, posy)];
                if(xx==-1)break;
                tmp_vec.push_back(make_tuple(posx, posy, xx, yy));
                posx = xx;
                posy = yy;
            }
            reverse(tmp_vec.begin(), tmp_vec.end());
            for(auto tmp: tmp_vec){
                int a, b, c,d;tie(a,b,c,d) = tmp;
                if(board.at(a).at(b)==0&&board.at(c).at(d)==0)continue;
                move_computer_(a,b,c,d);
                commands.push_back(tmp);
            }

            break;
        }
        rep(k, 4){
            int dx, dy; tie(dx, dy) = DIRS.at(k);
            int x3 = posx+dx;
            int y3 = posy+dy;
            if(x3 ==x&&y3==y)continue;
            if (x3 < 0 || x3 >= N || y3 < 0 || y3 >= N)continue;
            if(visited_map.count(make_pair(x3, y3)))continue;
            if(board.at(x3).at(y3)==color&&is_outside(posx, posy)&&!is_outside(x3, y3))continue;
            que.emplace(x3, y3, posx, posy);
        }
    }
}

int Board::connect_other_computers(){
    int result = 0;
    /* 残りの操作回数を数える */
    int move_command_size = min(K*100, (int)commands.size());
    set<P> connections;
    rep(i, 100){
        for(auto id: computer_connection.at(i)){
            if(i<id){
                connections.insert(make_pair(i, id));
            }
        }
    }
    set<P> connections2;
    rep(i, 100){
        for(auto id: computer_connection2.at(i)){
            if(i<id){
                connections2.insert(make_pair(i, id));
            }
        }
    }


    int connec_N = min((int)connections.size()+(int)connections2.size(), K*100-move_command_size);

    int res_num = 100*K - move_command_size - connec_N;
    
    vector<P3> candidates;
    for(int colo = 1; colo<=K; colo++){
        if(color==colo)continue;

        vector<Computer> comps;
        vector<vec_int> id_map(N, vec_int(N, -1));
        rep(i,N){
            rep(j,N){
                if(board.at(i).at(j)==colo){
                    int id = comps.size();
                    Computer compu = Computer(i, j, id, colo);
                    id_map.at(i).at(j) = id;
                    comps.push_back(compu);
                }
            }
        }

        dsu connected_dsu(100);
        rep(i, N){
            rep(j, N){
                if(board.at(i).at(j)!=colo)continue;
                int start_id = id_map.at(i).at(j);

                queue<P> que;
                que.emplace(start_id, -1);
                int count = 0;

                set<int> used0;
                set<P> used_pair;
                while(que.size()>0){
                    int id, prev; tie(id, prev) = que.front(); que.pop();
                    if(used0.count(id))continue;
                    int x1 = comps.at(id).x;
                    int y1 = comps.at(id).y;
                    if(painted.at(x1).at(y1)==1)continue;
                    if(second_painted.at(x1).at(y1)==1)continue;
                    used0.insert(id);
                    if(prev!=-1){
                        if(!connected_dsu.same(id, prev)){
                            connected_dsu.merge(id, prev);
                            used_pair.insert(make_pair(id, prev));
                        }
                    }
                    vec_int neighbors = comps.at(id).find_neighbors(board, id_map);
                    for(int i=0;i<4;i++){
                        if(neighbors.at(i)==-1){
                            continue;
                        }
                        if(used0.count(neighbors.at(i)))continue;
                        int id2 = neighbors.at(i);
                        int x2 = comps.at(id2).x;
                        int y2 = comps.at(id2).y;
                        if(!can_connect_cable(x1, y1, x2, y2))continue;
                        if(!can_connect_second_cable(x1, y1, x2, y2, second_painted))continue;
                        que.emplace(id2, id);
                    }
                }
                if(used_pair.size()>0){
                    vector<T4> opes;
                    for(auto node_pair: used_pair){
                        int id1, id2; tie(id1, id2) = node_pair;
                        int x1 = comps.at(id1).x;
                        int y1 = comps.at(id1).y;
                        int x2 = comps.at(id2).x;
                        int y2 = comps.at(id2).y;
                        opes.push_back(make_tuple(x1, y1, x2, y2));
                    }
                    candidates.push_back(make_pair(used_pair.size(), opes));
                }
            }
        }
    }
    sort(candidates.begin(), candidates.end());
    reverse(candidates.begin(), candidates.end());

    for(auto tmp: candidates){
        int length; vector<T4> opes; tie(length, opes) = tmp; 
        if(res_num<length)continue;
        int flag = 0;
        for(auto ope: opes){
            int x1, y1, x2, y2; tie(x1, y1, x2, y2) = ope;
            set<P> cells = cells_between3(x1, y1,x2,y2);
            for(auto cell: cells){
                int xx, yy; tie(xx, yy) = cell;
                if(painted.at(xx).at(yy)==1)flag = 1;
                if(second_painted.at(xx).at(yy)==1)flag = 1;
            }
            if(can_connect_second_cable(x1, y1, x2, y2, second_painted))continue;

            flag = 1;
        }
        if(flag==1)continue;
        for(auto ope: opes){
            int x1, y1, x2, y2; tie(x1, y1, x2, y2) = ope;
            if(!can_connect_second_cable(x1, y1, x2, y2, second_painted))continue;
            other_computer_connection.push_back(ope);
            res_num--;
            if(x1==x2){
                int x=x1;
                int ymin = min(y1, y2);
                int ymax = max(y1, y2);
                for(int y=ymin;y<=ymax;y++){
                    second_painted.at(x).at(y) = 1;
                }
            }else{
                int y=y1;
                int xmin = min(x1, x2);
                int xmax = max(x1, x2);
                for(int x=xmin;x<=xmax;x++){
                    second_painted.at(x).at(y) = 1;
                }
            }
        }
        // res_num -= length;
    }

    return result;
}


bool Board::move_second_connected_computers(int move_distance, vector<Computer> &comps, vector<vec_int> &id_map, vector<vec_int> &second_computer_connection, dsu &second_uf_tree, vector<vec_int> &painted2){
    // 既に他のコンピュータと結合されているコンピュータを動かして、他のグループとくっつけられないかを試す
    vector<T4> move_commands;
    vec_int id_vec;
    rep(i, comps.size())id_vec.push_back(i);
    shuffle(id_vec.begin(), id_vec.end(), mt);

    rep(idd, comps.size()){
        int id = id_vec.at(idd);
        // if(second_computer_connection.at(id).size()!=1)continue;
        if(!comps.at(id).can_move(board, id_map, second_computer_connection.at(id)))continue;
        // vec_int original_neighbors = computers.at(id).find_neighbors(board, computer_id_map);
        int group_id = second_uf_tree.leader(id);
        int i = comps.at(id).x;
        int j = comps.at(id).y;

        queue<T4> que;
        que.emplace(i, j, -1, -1);
        map<P, P> prev_map;

        while(!que.empty()){
            int x, y, px, py; tie(x, y, px, py) = que.front(); que.pop();
            if(prev_map.count(make_pair(x, y)))continue;
            if(board.at(x).at(y)!=0&&!(x==i&&y==j))continue;

            prev_map[make_pair(x, y)] = make_pair(px, py);


            if((x!=i||y!=j)){ // 元のところでは探索をしない
                swap_pos2(i, j, x, y, id_map, comps);
                // 移動した先が元のグループとつながっているかをチェックする
                vec_int neighbors = comps.at(id).find_neighbors(board, id_map);
                int flag1 = 0;
                int connect_id = -1;
                rep(kk, 4){
                    int next_id = neighbors.at(kk);
                    if(next_id==-1)continue;

                    if(group_id == second_uf_tree.leader(next_id)){
                        // 移動先が同じクラスタに所属している
                        flag1 = 1;
                    }else{
                        if((can_connect_second_cable(x, y, computers.at(next_id).x, computers.at(next_id).y, painted2)||painted2.at(x).at(y)==1)&&painted.at(x).at(y)==0){
                            connect_id = next_id;
                        }
                    }
                }
                swap_pos2(x, y, i, j, id_map, comps);

                // 移動さきで結合できればそれでOK
                if(connect_id != -1 && flag1==1){
                    // move_commands.push_back(make_tuple(i, j, x, y));
                    int posx = x;
                    int posy = y;
                    vector<T4> tmp_vec;
                    while(true){
                        P prev = prev_map[make_pair(posx, posy)];
                        if(prev == make_pair(-1, -1))break;
                        tmp_vec.push_back(make_tuple(prev.first, prev.second, posx, posy));
                        posx = prev.first;
                        posy = prev.second;
                    }
                    reverse(tmp_vec.begin(), tmp_vec.end());
                    rep(i, tmp_vec.size()){
                        move_commands.push_back(tmp_vec.at(i));
                    }

                    for(auto tmp: tmp_vec){
                        int a, b, c,d;tie(a,b,c,d) = tmp;
                        move_second_computer_(a,b,c,d,second_color, id_map, comps);
                        commands.push_back(tmp);
                    }
                    // dfs_connect();
                    return true;
                }
            }

            // 移動距離がmove_distance以上の時にはそれ以上遠くに行かない
            if(abs(i-x)+abs(j-y)>=move_distance)continue;

            // 次の候補を入れる
            rep(k, 4){
                int dx, dy; tie(dx, dy) = DIRS.at(k);
                int x2 = x+dx;
                int y2 = y+dy;
                if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= N)continue;
                if(prev_map.count(make_pair(x2, y2)))continue;
                if(board.at(x2).at(y2)!=0)continue;
                que.emplace(x2, y2, x, y);
            }
        }
 
    }
    for(auto tmp: move_commands){
        // commands.push_back(tmp);
        // int a, b, c,d;tie(a,b,c,d) = tmp;
        // if(board.at(c).at(d)!=0)continue; //別のやつが先に移動してきてしまっている件
        // move_computer_(a,b,c,d);
        // dfs_connect();
    }
    return false;
}



int main()
{
    get_time(true);
    cin >> N;
    cin >> K;
    vector<vec_int> c;
    c = vector<vec_int>(N, vec_int(N, 0));
    rep(i, N)
    {
        string tmp;
        cin >> tmp;
        rep(j, N){
            c.at(i).at(j) = (int)((int)tmp.at(j) - (int)'0');
        }
    }
    c2 = c;
    float density =(float)((float)K*100.)/(float)(N*N);
    if(density>0.65)is_dense = true;

    Board best_board;
    int best_score = -10000;
    int count = 0;
    vector<vec_int> color_score(K);
    int selected_color=-1;
    int selected_outside_region=-1;
    int non_select_flag = 0;
    while(true){
        rep(color, K){
            if(selected_color!=-1&&selected_color!=color)continue;
            count++;
            float ct = get_time(false);
            if(ct>TIME_LIMIT*0.95)goto FINISH;
            // Board board = Board(c, 1+color);
            // cerr<<"K="<<1+color<<endl;
            Board board = Board(c, 1+color);
            // Board board = Board(c, 1);
            // if((((N>=24&&K>=3&&(selected_outside_region!=-1||count<6*K))||(N>=25&&K==2&&(selected_outside_region!=-1||count<6*K)))&&ro.get_rand(0, 3)==0)||(N>=30&&K>=4)||(N>=25&&K==5&&ro.get_rand(0, 3)==0)){
            if((selected_outside_region!=-1 || (density<0.65&&ro.get_rand(0,2)==0) )&&density<0.8){
                board.move_surrounding_computers(2);
            }
            /*
            rep(i, N){
                rep(j,N){
                    cerr<<board.board.at(i).at(j);
                }
                cerr<<endl;
            }

            return 0;
            */
            board.dfs_connect();
    
            board.move_free_computers(1);
            board.dfs_connect();
            board.move_free_computers(1);
            board.dfs_connect();
            // 障害物を一つどかせばいい場合には退かしてみる
            // board.move_obstacle();
            board.move_obstacle_strong(1, 1);
            board.dfs_connect();
            // 移動させられるコンピュータを動かす, この場合他と連結していないものを指す
            board.move_free_computers(1);
            board.dfs_connect();
            board.move_free_computers(1);
            board.dfs_connect();
            board.move_free_computers(2);
            board.dfs_connect();
            board.move_free_computers(4);
            board.dfs_connect();
            board.move_free_computers(2);
            board.dfs_connect();
            if(get_time(false)>TIME_LIMIT*0.96)goto FINISH;
    
            // 障害物を退かしていい感じにならないかもう一度チェックする
            // board.move_obstacle();
            board.move_obstacle_strong(1, 1);
            board.dfs_connect();
            // cerr<<"KKKKcommand size1:"<<board.commands.size()<<endl;
    
            // 他のコンピュータとケーブルでつながっているけれど自由度があるコンピュータを移動させる
            board.move_connected_computers(1);
            board.move_connected_computers(2);
            board.move_connected_computers(3);
            board.move_connected_computers(5);
            board.move_free_computers(10);
            board.dfs_connect();

            if(ro.get_rand(0,3)==0)board.move_obstacle_strong(1, 10);
            board.connect_free_computers(4);
            board.move_obstacle_strong(1, 10);
            if(get_time(false)>TIME_LIMIT*0.96)goto FINISH;
    
            // 浮いてしまっているコンピュータをDFSで無理やりくっつける
            board.connect_free_computers(200);
            board.connect_free_computers(100);
    
            // 障害物をより強く取り除く
            if(board.uf_tree.groups().size()>1){
                board.move_obstacle_strong(1, 6);
            }
            if(board.uf_tree.groups().size()>1){
                board.move_obstacle_strong(1, 10);
            }
            if(board.uf_tree.groups().size()>1){
                board.move_obstacle_strong(1, 10);
            }
            if(get_time(false)>TIME_LIMIT*0.96)goto FINISH;
            int num = ro.get_rand(0, 60);
            if(selected_color!=-1)num=ro.get_rand(0, 30);
            if(density>0.9)num=100;

            rep(i, num){
            if(board.uf_tree.groups().size()>1&&density>0.5||ro.get_float()<0.2){
                board.shrink_cluster();
                board.move_obstacle_strong(1, 6);
                board.move_connected_computers(2);
            }
            }
            if(board.uf_tree.groups().size()>1){
                board.move_obstacle_strong(1, 15);
            }
            if(board.uf_tree.groups().size()>1){
                board.move_obstacle_strong(1, 15);
            }
            // cerr<<"command size11:"<<board.commands.size()<<endl;
            // cerr<<"command size4:"<<board.commands.size()<<endl;
            // board.move_obstacle_strong(2, 3);
            if(board.uf_tree.groups().size()>1){
                board.move_obstacle_strong(2, 6);
            }
            if(board.uf_tree.groups().size()>1){
                board.move_obstacle_strong(3, 8);
            }
            if(board.uf_tree.groups().size()>1){
                board.move_obstacle_strong(1, 10);
            }
            if(board.uf_tree.groups().size()>1){
                board.move_obstacle_strong(1, 10);
            }
            if(board.uf_tree.groups().size()>1){
                board.move_connected_computers(2);
            }
            rep(i, 20){
                if(board.uf_tree.groups().size()>1){
                board.resolve_separated_clusters(6);
                }
            }
            if(board.uf_tree.groups().size()>1){
                board.move_obstacle_strong(1, 10);
            }
            if(board.uf_tree.groups().size()>1){
                board.move_obstacle_strong(3, 8);
            }
            /*
            board.resolve_separated_clusters(3);
            board.resolve_separated_clusters(3);
            board.resolve_separated_clusters(3);
            board.resolve_separated_clusters(3);
            */
            // cerr<<"command size5:"<<board.commands.size()<<endl;

            if(board.commands.size()>K*100-99){
                non_select_flag = 1;
            }
 
            int score = board.calc_score();
            board.first_tree_commands = board.commands.size();
            board.detect_regions();
            // cerr<<"region num:"<<board.uf_tree_region.groups().size()<<" largest region size:"<<board.largest_region.size()<<endl;

            Board ans_board = board;
            int additional_score = 0;
            rep(jj, 5){
                rep(i, K){
                    if(get_time(false)>TIME_LIMIT*0.96)goto FINISH0;
                    if(i+1 == board.color)continue;
                    Board tmp_board = board;
                    if(!tmp_board.can_use_outside){
                        tmp_board.arrange_outer_computers(i+1, jj);
                    }
                    int gain = tmp_board.connect_in_largest_region(i+1);

                    gain += tmp_board.connect_other_computers();
                    if(gain>additional_score){
                        additional_score = gain;
                        ans_board = tmp_board;
                    }
                }
            }
            // cerr<<"count:"<<count<<" color:"<<color+1<<endl;
            // cerr<<"score:"<<score+additional_score<<endl;

           color_score.at(color).push_back(score+additional_score);
           /*
            if(count==K*5){
                int best_color = -1;
                int best_score = 0;

                rep(kk, K){
                    int max_sc = 0;
                    rep(jj, 5)max_sc = max(max_sc, color_score.at(kk).at(jj));
                    if(max_sc>best_score){
                        best_score = max_sc;
                        best_color = kk;
                    }
                }
                if(non_select_flag==0){
                    selected_color=best_color;
                }
            }
            */


            FINISH0:
            // if(additional_score+score>best_score&&(ans_board.commands.size()<best_board.commands.size()||ans_board.commands.size()==0)){
            // if(additional_score+score>=best_score&&best_board.minimum_cost()>ans_board.minimum_cost()){
            if(additional_score+score>=best_score){
                best_score = additional_score+score;
                best_board = ans_board;
                best_computers = computers;
                /*
                cerr<<"painted"<<endl;
                rep(i, N){
                    rep(j, N){
                        cerr<<ans_board.painted.at(i).at(j);
                    }
                    cerr<<endl;
                }
                */
               /*
                if(additional_score+score == best_score && ans_board.commands.size()<best_board.commands.size()){
                    best_score = additional_score+score;
                    best_board = ans_board;
                    best_computers = computers;
                }else if(additional_score+score>best_score){
                    best_score = additional_score+score;
                    best_board = ans_board;
                    best_computers = computers;
                }
                */
            }
            if(count==K*10){
                if(non_select_flag==0){
                    selected_color=best_board.color-1;
                    selected_outside_region=best_board.outside_region;
                }
            }

            /*
            if(score>best_score){
                best_score = score;
                best_board = board;
                best_computers = computers;
            }else if(score==best_score){
                if(board.commands.size()<=K*100-99&&false){
                    if(best_board.largest_region.size()<board.largest_region.size()){
                        best_score = score;
                        best_board = board;
                        best_computers = computers;
                    }
                }else{
                    if(best_board.commands.size()>board.commands.size()){
                        best_score = score;
                        best_board = board;
                        best_computers = computers;
                    }
                }
            }
            */
        }
    }
    FINISH:
    computers = best_computers;
    // cerr<<"N,K:"<<N<<" "<<K<<" out:"<<best_board.outside_region<<" "<<best_board.rand_val;
    // best_board.connect_other_computers();
    // cerr<<best_board.other_computer_connection.size()<<endl;
    // cerr<<best_score<<endl;
    best_board.cout_ans(best_computers);



    return 0;
}

