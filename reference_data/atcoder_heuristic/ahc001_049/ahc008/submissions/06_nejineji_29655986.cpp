#include <bits/stdc++.h>
#define REP(i, x, y) for (ll i = x; i <= y; i++)
#define BIT(t) (1ll << (t))
#define PER(i, y, x) for (ll i = y; i >= x; i--)
#define vll vector<ll>
#define vvll vector<vector<ll>>
#define pll pair<ll, ll>
#define SIZE(v) int(v.size())
#define UNIQUE(v) v.erase(unique(v.begin(), v.end()), v.end());
using namespace std;
typedef int ll;
typedef long double ld;
//        ios::sync_with_stdio(false);
//        cin.tie(nullptr);
// #pragma GCC optimize("O3")
// #pragma GCC optimize("unroll-loops")

struct Timer{
    Timer(){
        START = std::chrono::system_clock::now();
    }
    std::chrono::system_clock::time_point START;
    double now(){
        return chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - START).count() * 1e-6;
    }
};

Timer timer;

unsigned int randxor()
{
    static unsigned int x=123456789,y=362436069,z=521288629,w=88675123;
    unsigned int t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}

int randint(int mx){
    return randxor() % mx;
}

double randdouble(){
    return (randxor() + 0.5) * (1.0 / UINT_MAX);
}

template <typename T>
void randshuffle(vector<T>& v){
    int n = v.size();
    PER(i,n-1,0){
        int j = randint(i+1);
        swap(v[i], v[j]);
    }    
}

void local_print(){
    #ifdef LOCALMACHINE
    cout << "----" << endl;
    #endif
}
template <typename T>
void local_print(string s, T x){
    #ifdef LOCALMACHINE
    cout << s << ":" << x << endl;
    #endif
}

int const LENGTH = 30;
vector<int> dx = {-1, 0, 1, 0};
vector<int> dy = {0, 1, 0, -1};
int const U = 0;
int const R = 1;
int const D = 2;
int const L = 3;
map<char, int> charToDir = {{'U', U}, {'R', R}, {'D', D}, {'L', L}};

int CNT = 0;
int const CAT = 4;

template<typename T>
void removeFromVector(vector<T>& v, T x){
    v.erase(find(v.begin(), v.end(), x));
}

struct Position{
    int x; int y;
    Position nextPos(int dir){
        return {x + dx[dir], y + dy[dir]};
    }
    // bool operator==(Position &rhs)
    // {
    //     return x == rhs.x && y == rhs.y;
    // };
    bool operator==(Position rhs)
    {
        return x == rhs.x && y == rhs.y;
    };
    bool operator!=(Position &rhs)
    {
        return !(x == rhs.x && y == rhs.y);
    };
    const bool operator<(Position const &rhs) const{
        return y * 100 + x < rhs.y * 100 + rhs.x;
    }
};

bool inRectangle(int x, int y, int x1, int y1, int x2, int y2){
    return x1 <= x && x < x2 && y1 <= y && y < y2;
}

bool inRectangle(Position p, int x1, int y1, int x2, int y2){
    return inRectangle(p.x, p.y, x1, y1, x2, y2);
}

bool inRectangle(Position p, Position p1, Position p2){
    int x1 = min(p1.x, p2.x);
    int y1 = min(p1.y, p2.y);
    int x2 = max(p1.x, p2.x);
    int y2 = max(p1.y, p2.y);
    return inRectangle(p.x, p.y, x1, y1, x2, y2);
}

int dirAboutX(Position start, Position goal){
    if(start.x > goal.x){
        return U;
    }else if(start.x < goal.x){
        return D;
    }
    assert(false);
}

int dirAboutY(Position start, Position goal){
    if(start.y > goal.y){
        return L;
    }else if(start.y < goal.y){
        return R;
    }
    assert(false);
}

int manhattan(Position p1, Position p2){
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

struct Animal{
    Position pos; int type; bool captured; vector<int> dirsToMove = {U, L};
    void move(char c){
        pos.x += dx[charToDir[c]];
        pos.y += dy[charToDir[c]];
    }
    void move(string s){
        for(auto c:s){
            move(c);
        }
    }
};

struct Action{
    int actionNum; int dir;
};

bool onBoard(Position p){
    int x = p.x; int y = p.y;
    return inRectangle(x, y, 0, 0, LENGTH, LENGTH);
}

struct Human{
    Position pos;
    int nextX(int dir){
        return pos.x + dx[dir];
    }
    int nextY(int dir){
        return pos.y + dy[dir];
    }
    Position nextPos(int dir){
        return {nextX(dir), nextY(dir)};
    }
};

struct State{
    vector<Animal> animals;
    vector<Human> humans;
    int animalNum = 0;
    int humanNum = 0;
    vector<Position> squaresWhichWillBeMovedTo;
    vector<Position> squaresWichWillBeBlocked;
    bitset<LENGTH*LENGTH> grid = 0;
    void addAnimal(int x, int y, int type){
        animals.push_back({x, y, type, false});
        animalNum++;
    }
    void addHuman(int x, int y){
        humans.push_back({x, y});
        humanNum++;
    }
    bool isBlocked(Position p){
        int x = p.x; int y = p.y;
        REP(i,0,SIZE(squaresWichWillBeBlocked)-1){
            if(squaresWichWillBeBlocked[i] == p){
                return true;
            }
        }
        return !onBoard(p) || grid[x*LENGTH+y];
    }
    bool isEmpty(Position p){
        if(isBlocked(p)) return false;
        for(auto animal:animals){
            if(p == animal.pos) return false;
        }
        for(auto human:humans){
            if(p == human.pos) return false;
        }
        for(auto pos:squaresWhichWillBeMovedTo){
            if(p == pos) return false;
        }
        for(auto pos:squaresWichWillBeBlocked){
            if(p == pos) return false;
        }
        return true;
    }
    bool isBlockable(Position p){
        if(isBlocked(p)) return false;
        for(auto animal:animals){
            if(manhattan(p, animal.pos) <= 1) return false;
        }
        for(auto human:humans){
            if(p == human.pos) return false;
        }
        for(auto pos:squaresWhichWillBeMovedTo){
            if(p == pos) return false;
        }
        for(auto pos:squaresWichWillBeBlocked){
            if(p == pos) return false;
        }
        return true;
    }
    void putBlock(Position p){
        assert(isBlockable(p));
        int x = p.x; int y = p.y;
        grid[x*LENGTH+y] = 1;
    }
    bool isEmptyFromHuman(Human human, int dir){
        return isEmpty(human.nextPos(dir));
    }
    bool isBlockableFromHuman(Human human, int dir){
        return isBlockable(human.nextPos(dir));
    }
    void humanAct(Human& human, Action action){
        int actionNum = action.actionNum;
        int dir = action.dir;
        if(actionNum == 0)return ;
        Position nextPos = human.nextPos(dir);
        if(actionNum == 1){
            human.pos = nextPos;
        }
        if(actionNum == 2){
            putBlock(nextPos);
        }
    }

};


char actionToChar(Action action){
    int actionNum = action.actionNum;
    int dir = action.dir;
    if(actionNum == 0){return '.';}
    if(actionNum == 1){
        if(dir == U){return 'U';}
        if(dir == D){return 'D';}
        if(dir == L){return 'L';}
        if(dir == R){return 'R';}
    }
    if(actionNum == 2){
        if(dir == U){return 'u';}
        if(dir == D){return 'd';}
        if(dir == L){return 'l';}
        if(dir == R){return 'r';}
    }
    assert(false);
}

struct NextActions{
    vector<Action> actions;
    int humanNum;
    NextActions(int n){
        humanNum = n;
        actions = vector<Action>(n, Action{0,0});
        
    } 
    void setIthAction(int i, Action action, State& state){
        actions[i] = action;
        Human human = state.humans[i];
        int dir = action.dir;
        if(action.actionNum == 1){
            state.squaresWhichWillBeMovedTo.push_back(human.nextPos(dir));
        }
        if(action.actionNum == 2){
            state.squaresWichWillBeBlocked.push_back(human.nextPos(dir));
        }
    }
};

char startXCondition(int animalNum, int humanNum){
    if(humanNum == 5 && animalNum <= 17){
        return 'A';
    }else if(humanNum == 6 && animalNum >= 11){
        return 'A';
    }else if(humanNum == 7 && animalNum >= 18){
        return 'A';
    }else if(humanNum == 5 && animalNum >= 18){
        return 'C';
    }
    return 'B';
}

int const WALL_LENGTH = 10;
int const ROOM_NUM_IN_ROW = 10;
int const WALL_BREATH = 3;
struct WallState{
    vector<int> startX;
    vector<int> startY;
    vector<int> uncreatedWallIds;
    vector<Position> upperStartPos;
    vector<Position> lowerStartPos;
    int notStartedWallNum = 0;
    int notBuildedWallNum = 0; 
    bool isCreated = false;
    WallState(){}   
    WallState(int animalNum, int humanNum){
    if(startXCondition(animalNum, humanNum) == 'A'){
            startX = //{0, 7, 14, 21, 29};
                        //{3, 9, 15, 21, 27};
                        //{4, 11, 18, 25};
                        {4, 11, 18, 25}  ;   
    }else if(startXCondition(animalNum, humanNum) == 'B'){
            startX = //{0, 5, 11, 17, 23, 29};
                    {3, 9, 15, 21, 27};
                    //{3, 8, 12, 17, 22, 27};       
    }else{
            startX = {7, 14, 22};
    }
        for(int i = 1;i < LENGTH; i += WALL_BREATH){
            startY.push_back(i);
            upperStartPos.push_back({0, i});
            lowerStartPos.push_back({LENGTH - 1, i});
            notStartedWallNum++;
            notBuildedWallNum++;
        }
        REP(i,0,SIZE(startY)-1){
            uncreatedWallIds.push_back(i);
        }
    }
    int distanceFromPath(Position pos){
        int minDistance = 1e9;
        for(int x: startX){
            minDistance = min(minDistance, abs(x - pos.x));
        }
        return minDistance;
    }
    bool shouldBeBlocked(Position pos){
        if(distanceFromPath(pos) == 1 && pos.y % WALL_BREATH == 1){
            return pos.y != 1;
        }else if(distanceFromPath(pos) >= 2 && pos.y % WALL_BREATH == 2){
            if(pos.y == LENGTH - 1 &&  distanceFromPath(pos) >= 3){
                return false;
            }
            return true;
        }else if(pos.y == 0 && distanceFromPath(pos) == 1){
            return true;
        }
        return false;
    }
};

struct Room{
    int roomId;
    bool isCreated;
    bool isClosed;
    Position entrance;
    Position exit;
    int dirFromEntrance;
    int dirFromExit;
    int x1; int x2; int y1; int y2;
    bool isDoubleEntrance;
    Room(){}
    Room(int roomId_){
        roomId = roomId_;
    }
    Room(int roomId_, bool isDoubleEntrance_, Position entrance_, int dirFromEntrance_, Position exit_, int dirFromExit_, int x1_, int y1_, int x2_, int y2_){
        roomId = roomId_;
        isCreated = false;
        isClosed = false;
        entrance = entrance_;
        dirFromEntrance = dirFromEntrance_;
        exit = exit_;
        dirFromExit = dirFromExit_;
        x1 = x1_; x2 = x2_; y1 = y1_; y2 = y2_;
        isDoubleEntrance = isDoubleEntrance_;
    }
    bool inThisRoom(Position p){
        return inRectangle(p, x1, y1, x2, y2);
    }
    bool canNotEscapeFrom(Position pos, State& state, int dir){
        return manhattan(pos, entrance) <= 1 || manhattan(pos, exit) <= 1 || state.isBlocked(pos) || state.isBlocked(pos.nextPos(dir)) || pos.nextPos(dir).nextPos(dir) == entrance || pos.nextPos(dir).nextPos(dir) == exit;
    }
    bool canEscapeFrom(Position pos, State& state, int dir){
        return !canNotEscapeFrom(pos, state, dir);
    }
    bool checkIsCreated(State& state){
        if(isCreated){
            return true;
        }
        REP(x, x1, x2-1){
            Position pos1 = {x, y1}, pos2 = {x, y2 - 1};
            if(canEscapeFrom(pos1, state,  L) || canEscapeFrom(pos2, state, R)){
                return false;
            }
        }
        REP(y, y1, y2 - 1){
            Position pos1 = {x1, y}, pos2 = {x2 - 1, y};
            if(canEscapeFrom(pos1, state, U) || canEscapeFrom(pos2, state, D)){
                return false;
            }
        }
        isCreated = true;
        return true;
    }
    int hasHumans(vector<Human> humans){
        int cnt = 0;
        REP(i,0,SIZE(humans)-1){
            cnt += inThisRoom(humans[i].pos);
        }
        return cnt;
    }
    int hasUncapturedAnimals(vector<Animal> animals){
        int cnt = 0;
        REP(i,0,SIZE(animals)-1){
            cnt += (inThisRoom(animals[i].pos) && !animals[i].captured);
        }
        return cnt;
    }
    vector<int> animalsIdsInRoom(vector<Animal> animals){
        vector<int> ans;
        REP(i,0,SIZE(animals) - 1){
            if(inThisRoom(animals[i].pos)){
                ans.push_back(i);
            }
        }
        return ans;
    }
    Position getCurrentEntrance(State& state){
        return state.isBlocked(entrance) ? entrance.nextPos(L) : entrance;
    }
    Position getCurrentExit(State& state){
        return state.isBlocked(exit) ? exit.nextPos(L) : exit; 
    }
};

Action moveToGoal(Position start, Position goal, State& state){
    if(start.x != goal.x){
        int dir = dirAboutX(start, goal);
        if(!state.isBlocked(start.nextPos(dir))){
            return {1, dir};
        }
    }
    if(start.y != goal.y){
        int dir = dirAboutY(start, goal);
        if(!state.isBlocked(start.nextPos(dir))){
            return {1, dir};
        }
    }
    return {0,0};
}

bool thereIsAMan(Position pos, vector<Human>& humans){
    return humans.end() != find_if(humans.begin(), humans.end(), [&pos](Human human){return pos == human.pos;});
}

tuple<int, int> getShortestPathsDirAndLength(Position start, Position goal, State& state){
    if(start == goal){
        return {-1, 0};
    }
    bitset<LENGTH*LENGTH> visited = 0;
    auto isVisited = [&visited](Position pos){
        return visited[pos.x * LENGTH + pos.y];
    };
    auto visit = [&visited](Position pos){
        visited[pos.x * LENGTH + pos.y] = 1;
        return;
    };
    visit(start);
    queue<tuple<int,int,Position>> q;
    REP(dir,0,3){
        Position nextPos = start.nextPos(dir);
        if(state.isBlocked(nextPos) && nextPos == goal){
            return {-1, 0};
        }
        if(state.isBlocked(nextPos)){
            continue;
        }
        q.push({dir, 1, nextPos});
        visit(nextPos);
        if(nextPos == goal){
            return {dir, 1};
        }
    }
    while(!q.empty()){
        auto [firstDir, dist, curPos] = q.front();
        q.pop();
        REP(dir,0,3){
            Position nextPos = curPos.nextPos(dir);
            if(nextPos == goal){
                return {firstDir, dist};
            }
            if(state.isBlocked(nextPos) || isVisited(nextPos)){
                continue;
            }
            q.push({firstDir, dist + 1, nextPos});
            visit(nextPos);
        }
    }
    return {-1, 1e9};
}

Action moveToGoalByShortestPath(Position curPos, Position goalPos, State state){
    int dir = get<0>(getShortestPathsDirAndLength(curPos, goalPos, state));
    return dir >= 0 ? Action({1, dir}) : Action({0, 0});
}

Room standardRoomFactory(int row, int upper, int lower, int col){
    int roomId = row * ROOM_NUM_IN_ROW + col;
    Position entrance = {upper, col * WALL_BREATH};
    int dirFromEntrance = D;
    Position exit = {lower, col * WALL_BREATH};
    int dirFromExit = U;
    int x1 = upper + 1;
    int x2 = lower;
    int y1 = col * WALL_BREATH;
    int y2 = col * WALL_BREATH + WALL_BREATH - 1;
    if(col == 0){
        entrance.y++;
        exit.y++;
    }
    if(col == ROOM_NUM_IN_ROW - 1){
        if(x1 != 0){
            x1++;
        }
        if(x2 != LENGTH){
            x2--;
        }
        y2++;
    }
    if(!onBoard(entrance)){
        swap(entrance, exit);
        swap(dirFromEntrance, dirFromExit);
    }
    if(!onBoard(exit)){
        exit = {100, 100};
        return Room(roomId, false, entrance, dirFromEntrance, exit, dirFromExit, x1, y1, x2, y2);
    }else{
        return Room(roomId, true, entrance, dirFromEntrance, exit, dirFromExit, x1, y1, x2, y2);
    }
}

vector<Room> standardRoomsFactory(WallState& wallState){
    vector<Room> ans;
    vector<int> pathsX;
    pathsX.push_back(-1);
    for(auto x:wallState.startX){
        pathsX.push_back(x);
    }
    pathsX.push_back(LENGTH);
    REP(i, 0, SIZE(pathsX) - 2){
        int upper = pathsX[i];
        int lower = pathsX[i+1];
        if(lower - upper <= 1) continue;
        REP(j, 0, ROOM_NUM_IN_ROW - 1){
            ans.push_back(standardRoomFactory(i, upper, lower, j));
        }
    }
    return ans;
}

int getRoomId(vector<Room>& rooms, Position pos){
    int ans = find_if(rooms.begin(), rooms.end(), [pos](Room room){return room.inThisRoom(pos);}) - rooms.begin();
    return ans == rooms.end() - rooms.begin() ? -1 : ans;
}

vector<int> getRoomIdsAboveAndBelow(vector<Room>& rooms, Position pos){
    vector<int> ans;
    int above = getRoomId(rooms, pos.nextPos(U).nextPos(U));
    if(above != -1){
        ans.push_back(above);
    }
    int below = getRoomId(rooms, pos.nextPos(D).nextPos(D));
    if(below != -1){
        ans.push_back(below);
    }
    return ans;
}

tuple<Action, bool> actionToBuildAndIsFinished(Position curPos, Position goalPos, int front, State& state, WallState& wallState){
    bool goDown = true;
    REP(dir,0,3){
        if(dir == front) continue;
        Position nextPos = curPos.nextPos(dir);
        if(wallState.shouldBeBlocked(nextPos) && !state.isBlocked(nextPos)){
            goDown = false;
            if(state.isBlockable(nextPos)){
                return {Action({2, dir}), false};
            }
        }
    }
    if(goDown){
        Action nextAction = moveToGoalByShortestPath(curPos, goalPos, state);
        bool isFinished = (curPos == goalPos);
        return {nextAction, isFinished};
    }else{
        return {Action({0,0}), false};
    }
}

vector<pair<vector<int>, bool>> createWallSchedule(WallState& wallState, vector<Position>& poses){
    int humanNum = poses.size();
    int wallNum = wallState.lowerStartPos.size();
    int twoWallCreatingManNum = wallNum - humanNum;
    vector<pair<Position, int>> posesAndIds;
    REP(i, 0, SIZE(poses) - 1){
        posesAndIds.push_back({poses[i], i});
    }
    sort(posesAndIds.begin(), posesAndIds.end());
    vector<pair<vector<int>, bool>> ans(humanNum);
    vector<vector<int>> dp(humanNum + 1, vector<int>(twoWallCreatingManNum + 1, 1e9));
    vector<vector<pair<int,int>>> frm(humanNum+1, vector<pair<int,int>>(twoWallCreatingManNum + 1));
    dp[humanNum][0] = 0;
    PER(i, humanNum, 1){
        int posY = posesAndIds[i - 1].first.y;
        REP(j, 0, twoWallCreatingManNum){
            int wallId = wallNum - 1 - (humanNum - i) - j;
            int nextValue = dp[i][j] + abs(wallState.startY[wallId] - posY);
            if(nextValue < dp[i - 1][j]){
                dp[i - 1][j] = nextValue;
                frm[i - 1][j] = {i, j};
            }
            if(j == twoWallCreatingManNum) continue;
            if(nextValue < dp[i - 1][j + 1]){
                dp[i - 1][j + 1] = nextValue;
                frm[i - 1][j + 1] = {i, j};
            }
        }
    }
    pair<int, int> cur = {0, twoWallCreatingManNum};
    int curWall = 0;
    REP(i,0,humanNum - 1){
        Position humanPos = posesAndIds[i].first;
        int humanId = posesAndIds[i].second;
        ans[humanId].first.push_back(curWall);
        curWall++;
        pair<int,int> nex = frm[cur.first][cur.second];
        if(nex.second != cur.second){
            ans[humanId].first.push_back(curWall);
            curWall++;
            reverse(ans[humanId].first.begin(), ans[humanId].first.end());
        }
        ans[humanId].second = (humanPos.x < LENGTH / 2);
        cur = nex;
    }
    // dp[i][j] = iまでやって、j回使った
    return ans;
}

struct Mission{
    int humanId;
    bool isFinished;
    string missionName;
    Position startPos;
    Position goalPos;
    Room targetRoom;
    int mode = 0;
    stack<int> wallIds;
    int partnerId;
    bool isGoingToEntrance;
    int fromTop;
    Mission(){}
    Mission(string missionName_, int humanId_, Position targetPos_){
        if(missionName_ == "Default"){
            missionName = missionName_;
            isFinished = true;
            humanId = humanId_;
            goalPos = targetPos_;
            return;
        }
        assert(false);
        
    }
    Mission(string missionName_, vector<int> wallIds_, bool startIsUpper, int humanId_, WallState& wallState){
        if(missionName_ == "BuildWall"){
            missionName = missionName_;
            humanId = humanId_;
            int firstWallId = wallIds_[0];
            REP(i, 1, SIZE(wallIds_) - 1){
                wallIds.push(wallIds_[i]);
            }
            Position upperStartPos = wallState.upperStartPos[firstWallId];
            Position lowerStartPos = wallState.lowerStartPos[firstWallId];
            startPos = startIsUpper ? upperStartPos: lowerStartPos;
            goalPos = startIsUpper ? lowerStartPos : upperStartPos;
            mode = 0;
            isFinished = false;
            return ;
        }
        assert(false);
    }
    Mission(string missionName_, Position goalPos_, Room targetRoom_, int humanId_, int partnerId_, int isGoingToEntrance_){
        if(missionName_ == "PincerAttack"){
            missionName = missionName_;
            targetRoom = targetRoom_;
            goalPos = goalPos_;
            isFinished = false;
            mode = 0;
            humanId = humanId_;
            partnerId = partnerId_;
            isGoingToEntrance = isGoingToEntrance_;
        }else{
            assert(false);
        }
    }
    Mission(string missionName_, Position goalPos_, Room targetRoom_, int humanId_){
        if(missionName_ == "SingleAttack"){
            missionName = missionName_;
            targetRoom = targetRoom_;
            goalPos = goalPos_;
            isFinished = false;
            mode = 0;
            humanId = humanId_;            
        }else{
            assert(false);
        }
    }
    Mission(string missionName_, int humanId_, int fromTop_, Room targetRoom_, Position targetPos_){
        if(missionName_ == "DriveAll"){
            missionName = missionName_;
            humanId = humanId_;
            fromTop = fromTop_;
            targetRoom = targetRoom_;
            goalPos = targetPos_;
        }else{
            assert(false);
        }
    }
    Action generateAction(State& state, WallState& wallState, vector<Mission>& missions){
        Human human = state.humans[humanId];
        Position curPos = human.pos;
        if(missionName == "Default"){
            if(curPos == goalPos){
                return {0,0};
            }
            return moveToGoalByShortestPath(curPos, goalPos, state);
            //return curPos == goalPos ? Action({0,0}) : moveToGoal(curPos, goalPos, state);
        }
        if(missionName == "BuildWall"){
            //mode 0: moving, 1 : building
            if(mode == 0){
                if(manhattan(curPos, startPos) <= 1){
                    mode = 1;
                }
                return moveToGoalByShortestPath(curPos, startPos, state);
            }
            if(mode == 1){
                int front = startPos.x < goalPos.x ? D : U;
                auto [nextAction, finishFlag] = actionToBuildAndIsFinished(curPos, goalPos, front, state, wallState);
                //isFinished = finishFlag;
                if(finishFlag && wallIds.empty()){
                    isFinished = true;
                }else if(finishFlag){
                    mode = 0;
                    int nextWall = wallIds.top();
                    Position nextUpperStartPos = wallState.upperStartPos[nextWall];
                    Position nextLowerStartPos = wallState.lowerStartPos[nextWall];
                    wallIds.pop();
                    startPos = manhattan(curPos, nextUpperStartPos) < manhattan(curPos, nextLowerStartPos) ? nextUpperStartPos : nextLowerStartPos;
                    goalPos = manhattan(curPos, nextUpperStartPos) < manhattan(curPos, nextLowerStartPos) ? nextLowerStartPos : nextUpperStartPos;
                }
                return nextAction;
            }
        }
        if(missionName == "PincerAttack"){
            if(curPos != goalPos){
                return moveToGoalByShortestPath(curPos, goalPos, state);
            }
            else{
                Mission partnersMission = missions[partnerId];
                Position partnersPos = state.humans[partnerId].pos;
                int myDir = isGoingToEntrance ? targetRoom.dirFromEntrance : targetRoom.dirFromExit;
                int partnersDir = isGoingToEntrance ? targetRoom.dirFromExit : targetRoom.dirFromEntrance;
                if(!targetRoom.hasHumans(state.humans) && partnersMission.goalPos == partnersPos && state.isBlockable(curPos.nextPos(myDir)) && (state.isBlocked(partnersPos.nextPos(partnersDir)) || state.isBlockable(partnersPos.nextPos(partnersDir)))){
                    vector<int> targetAnimalIds = targetRoom.animalsIdsInRoom(state.animals);
                    for(int id: targetAnimalIds){
                        state.animals[id].captured = true;
                    }
                    isFinished = true;
                    targetRoom.isClosed = true;
                    return {2, myDir};
                }
                return {0,0};
            }
        }
        if(missionName == "SingleAttack"){
            if(curPos != goalPos){
                return moveToGoalByShortestPath(curPos, goalPos, state);
            }else{
                if(!targetRoom.hasHumans(state.humans) && state.isBlockable(curPos.nextPos(targetRoom.dirFromEntrance))){
                    vector<int> targetAnimalIds = targetRoom.animalsIdsInRoom(state.animals);
                    for(int id: targetAnimalIds){
                        state.animals[id].captured = true;
                    }
                    isFinished = true;
                    targetRoom.isClosed = true;
                    return {2, targetRoom.dirFromEntrance};
                }else{
                    return {0, 0};
                }
            }
        }
        if(missionName == "DriveAll"){
            int posX = wallState.startX[fromTop];
            if(targetRoom.roomId == -1){
                int goalPosY = goalPos.y;
                Position targetPos = {posX, goalPosY};
                if(curPos.x != targetPos.x){
                    targetPos.y = curPos.y;
                }
                int dir = get<0>(getShortestPathsDirAndLength(curPos, targetPos, state));
                return dir != -1 ? Action({1, dir}) : Action({0, 0});
            }else{
                int goalPosY = targetRoom.getCurrentEntrance(state).y;
                Position targetPos = {posX, goalPosY};
                if(curPos != targetPos){
                    if(curPos.x != targetPos.x){
                        targetPos.y = curPos.y;
                    }
                    int dir = get<0>(getShortestPathsDirAndLength(curPos, targetPos, state));
                    return dir != -1 ? Action({1, dir}) : Action({0, 0});
                }else if(curPos == targetRoom.getCurrentEntrance(state)){
                    if(!targetRoom.hasHumans(state.humans) && state.isBlockable(curPos.nextPos(targetRoom.dirFromEntrance)) && (state.isBlocked(targetRoom.getCurrentExit(state).nextPos(targetRoom.dirFromExit)) || state.isBlockable(targetRoom.getCurrentExit(state).nextPos(targetRoom.dirFromExit))) && thereIsAMan(targetRoom.getCurrentExit(state), state.humans)){
                        vector<int> targetAnimalIds = targetRoom.animalsIdsInRoom(state.animals);
                        for(int id: targetAnimalIds){
                            state.animals[id].captured = true;
                        }
                        targetRoom.isClosed = true;
                        return {2, targetRoom.dirFromEntrance};
                    }else{
                        return {0, 0};
                    }
                }else if(curPos == targetRoom.getCurrentExit(state)){
                    if(!targetRoom.hasHumans(state.humans) && state.isBlockable(curPos.nextPos(targetRoom.dirFromExit)) && (state.isBlocked(targetRoom.getCurrentEntrance(state).nextPos(targetRoom.dirFromEntrance)) || state.isBlockable(targetRoom.getCurrentEntrance(state).nextPos(targetRoom.dirFromEntrance))) && thereIsAMan(targetRoom.getCurrentEntrance(state), state.humans)){
                        return {2, targetRoom.dirFromExit};
                    }else{
                        return {0, 0};
                    }
                }else{
                    return {0,0};
                }
            }
        }
        assert(false);
    }
};

Position nextPosByRoom(Position pos, int dir, WallState& wallState){
    vector<int> pathX = wallState.startX;
    int curPathXId = find_if(pathX.begin(), pathX.end(), [&pos](int x){return abs(pos.x - x) <= 1;}) - pathX.begin();
    if(curPathXId == SIZE(pathX)){
        return pos;
    }
    if(dir == -1){
        return {pathX[curPathXId], pos.y};
    }
    if(dir == U || dir == D){
        int nextPathXId = curPathXId + dx[dir];
        if(0 <= nextPathXId && nextPathXId < SIZE(pathX)){
            return {pathX[nextPathXId], pos.y};
        }else{
            return {-100, -100};
        }
    }else{
        return {pathX[curPathXId], pos.y + dy[dir] * WALL_BREATH};
    }
    return {-100, -100};
}

void pushNewPosOnBoard(vector<Position>& vec, Position pos){
    if(!onBoard(pos) || pos.y >= LENGTH - 1){
        return ;
    }
    if(find(vec.begin(), vec.end(), pos) == vec.end()){
        vec.push_back(pos);
    }
}

vector<Position> posesAroundCurrent(int animalId, vector<Room>& rooms, WallState& wallState, State& state, int uncapturedAnimalsNum){
    Animal animal = state.animals[animalId];
    Position curPos = animal.pos;
    int roomId = getRoomId(rooms, curPos);
    vector<Position> ans;
    if(roomId == -1){
        ans.push_back(nextPosByRoom(curPos, -1, wallState));
        Position upperPos = nextPosByRoom(curPos, U, wallState);
        pushNewPosOnBoard(ans, upperPos);
        Position lowerPos = nextPosByRoom(curPos, D, wallState);
        pushNewPosOnBoard(ans, lowerPos);
    }else if(rooms[roomId].isDoubleEntrance){
        ans.push_back(rooms[roomId].getCurrentEntrance(state));
        ans.push_back(rooms[roomId].getCurrentExit(state));
    }else{
        ans.push_back(rooms[roomId].getCurrentEntrance(state));
    }
    int wantedPosNum = state.humanNum / uncapturedAnimalsNum;
    if(SIZE(ans) >= wantedPosNum){
        return ans;
    }
    stack<Position> st;
    for(Position p: ans){
        st.push(p);
    }
    vector<int> animalDirs = animal.type == CAT ? animal.dirsToMove :vector<int>({0, 1, 2, 3});
    while(!st.empty()){
        Position curPos = st.top();
        st.pop();
        for(int dir: animalDirs){
            Position nextPos = nextPosByRoom(curPos, dir, wallState);
            pushNewPosOnBoard(ans, nextPos);
            if(SIZE(ans) >= wantedPosNum){
                return ans;
            }
        }
    }
    return ans;
}

struct Solver{
    State state;
    vector<Mission> missions;
    WallState wallState;
    vector<Room> rooms;
    Solver(State st){
        state = st;
        missions = vector<Mission>(state.humanNum);
        wallState = WallState(state.animalNum, state.humanNum);
        REP(i,0,state.humanNum-1){
            Position pos = {wallState.startX[1], LENGTH / 2};
            missions[i] = Mission("Default", i, pos);
        } 
        rooms = standardRoomsFactory(wallState);
        REP(i, 0, SIZE(wallState.startX) - 1){
            int roomId = ROOM_NUM_IN_ROW * (SIZE(wallState.startX)) + i;
            int x = wallState.startX[i];
            rooms.push_back(Room(roomId, false, Position({x, LENGTH-4}), R, Position({-1,-1}), L, x-1, LENGTH-2, x+2, LENGTH));
        }
    }
    void setupMissions(){
        vector<int> finishedMissionIds;
        REP(i,0,SIZE(missions)-1){
            if(missions[i].isFinished || missions[i].missionName != "BuildWall"){
                finishedMissionIds.push_back(i);
            }
        }
        if(finishedMissionIds.empty()){
            return;
        }
        if(!wallState.isCreated){
            wallState.isCreated = true;
            vector<Position> poses;
            for(Human human: state.humans){
                poses.push_back(human.pos);
            }
            vector<pair<vector<int>, bool>> wallsEachIsResponsibleFor = createWallSchedule(wallState, poses);
            REP(i, 0, state.humanNum - 1){
                missions[i] = Mission("BuildWall", wallsEachIsResponsibleFor[i].first, wallsEachIsResponsibleFor[i].second, i, wallState);
            }
            finishedMissionIds.clear();
            // vector<int>& uncreatedWallIds = wallState.uncreatedWallIds;
            // while(!uncreatedWallIds.empty() && !finishedMissionIds.empty()){
            //     int minDistance = 1e9;
            //     int minWallId = -1;
            //     int minHumanId = -1;
            //     for(int wallId: uncreatedWallIds){
            //         for(int humanId: finishedMissionIds){
            //             Position curPos = state.humans[humanId].pos;
            //             Position startPos = wallState.upperStartPos[wallId];
            //             Position goalPos = wallState.lowerStartPos[wallId];
            //             int curDistance = min(manhattan(curPos, startPos), manhattan(curPos, goalPos));
            //             if(curDistance < minDistance){
            //                 minDistance = curDistance;
            //                 minWallId = wallId;
            //                 minHumanId = humanId;
            //             }
            //         }
            //     }
            //     Position curPos = state.humans[minHumanId].pos;
            //     Position startPos = wallState.upperStartPos[minWallId];
            //     Position goalPos = wallState.lowerStartPos[minWallId];
            //     if(manhattan(curPos, startPos) > manhattan(curPos, goalPos)){
            //         swap(startPos, goalPos);
            //     }
            //     missions[minHumanId] = Mission("BuildWall", startPos, goalPos, minHumanId);
            //     removeFromVector(uncreatedWallIds, minWallId);
            //     removeFromVector(finishedMissionIds, minHumanId);
            // }
            // for(int missionId: finishedMissionIds){
            //     Position pos = {wallState.startX[1], LENGTH / 2};
            //     missions[missionId] = Mission("Default", missionId, pos);
            //}        
        }else if(!finishedMissionIds.empty()){
            vector<Position> goals;
            vector<int> targetRoomIds;
            REP(i,0,SIZE(rooms)-1){
                if(rooms[i].hasUncapturedAnimals(state.animals) && rooms[i].checkIsCreated(state)){
                    targetRoomIds.push_back(i);
                }
            }
            vector<int> unCapturedAnimalsIds;
            REP(i,0,state.animalNum-1){
                if(!state.animals[i].captured){
                    unCapturedAnimalsIds.push_back(i);
                }
            }
            int uncapturedAnimalsNum = size(unCapturedAnimalsIds);
            while(!finishedMissionIds.empty() && !targetRoomIds.empty()){
                int min_distance = 1e9;
                int targetRoomId = -1;
                int minJ;
                int minK = 0;
                auto & humans = state.humans;
                for(int i: targetRoomIds){
                    Room& room = rooms[i];
                    REP(j,0,SIZE(finishedMissionIds)-1){
                        int humanJ = finishedMissionIds[j];
                        if(room.isDoubleEntrance){
                            REP(k,0,SIZE(finishedMissionIds)-1){
                                if(j == k) continue;
                                int humanK = finishedMissionIds[k];
                                int tmp = max(manhattan(humans[humanJ].pos, room.getCurrentEntrance(state)), manhattan(humans[humanK].pos, room.getCurrentExit(state)));
                                if(tmp < min_distance){
                                    min_distance = tmp;
                                    targetRoomId = i;
                                    minJ = j;
                                    minK = k;
                                }
                            }
                        }else{
                            int tmp = manhattan(humans[humanJ].pos, room.getCurrentEntrance(state));
                            if(tmp < min_distance){
                                min_distance = tmp;
                                targetRoomId = i;
                                minJ = j;
                            }
                        }
                    }
                }
                if(targetRoomId == -1){
                    break;
                }
                Room& targetRoom = rooms[targetRoomId];
                if(targetRoom.isDoubleEntrance){
                    int humanJ = finishedMissionIds[minJ];
                    int humanK = finishedMissionIds[minK];
                    missions[humanJ] = Mission("PincerAttack", targetRoom.getCurrentEntrance(state), targetRoom, humanJ, humanK, true);
                    missions[humanK] = Mission("PincerAttack", targetRoom.getCurrentExit(state), targetRoom, humanK, humanJ, false);
                    goals.push_back(rooms[targetRoomId].getCurrentEntrance(state));
                    goals.push_back(rooms[targetRoomId].getCurrentExit(state));
                    removeFromVector(targetRoomIds, targetRoomId);
                    removeFromVector(finishedMissionIds, humanJ);
                    removeFromVector(finishedMissionIds, humanK);
                }else{
                    int humanJ = finishedMissionIds[minJ];
                    missions[humanJ] = Mission("SingleAttack", rooms[targetRoomId].getCurrentEntrance(state), targetRoom, humanJ);
                    goals.push_back(rooms[targetRoomId].getCurrentEntrance(state));
                    removeFromVector(targetRoomIds, targetRoomId);
                    removeFromVector(finishedMissionIds, humanJ);                    
                }
                // vector<int> animalsIdInTargetRoom = targetRoom.animalsIdsInRoom(state.animals);
                // for(auto animalId: animalsIdInTargetRoom){
                //     removeFromVector(unCapturedAnimalsIds, animalId);
                // }
            }
            for(auto missionId: finishedMissionIds){
                Position goalPos = state.humans[missionId].pos;
                int minDistance = 1e9;
                for(int animalId: unCapturedAnimalsIds){
                    auto posesAroundAnimal = posesAroundCurrent(animalId, rooms, wallState, state, uncapturedAnimalsNum);
                    for(Position curPos: posesAroundAnimal){
                        if(find(goals.begin(), goals.end(), curPos) != goals.end()) continue;
                        int dist = manhattan(state.humans[missionId].pos, curPos);
                        if(dist < minDistance){
                            minDistance = dist;
                            goalPos = curPos;
                        }
                    }
                }
                if(minDistance == 1e9){
                    for(int animalId: unCapturedAnimalsIds){
                    auto posesAroundAnimal = posesAroundCurrent(animalId, rooms, wallState, state, uncapturedAnimalsNum);
                        for(Position curPos: posesAroundAnimal){
                            int dist = manhattan(state.humans[missionId].pos, curPos);
                            if(dist < minDistance){
                                minDistance = dist;
                                goalPos = curPos;
                            }
                        }
                    }
                }
                missions[missionId] = Mission("Default", missionId, goalPos);
                goals.push_back(goalPos);
            }
            finishedMissionIds.clear();

        }
        for(auto missionId: finishedMissionIds){
            Position pos = state.humans[missionId].pos;
            int minDistance = 1e9;
            REP(animalId, 0, state.animalNum-1){
                if(!state.animals[animalId].captured){
                    Animal animal = state.animals[animalId];
                    auto posesAroundAnimal = posesAroundCurrent(animalId, rooms, wallState, state, 3);
                    for(Position curPos: posesAroundAnimal){
                        int dist = manhattan(state.humans[missionId].pos, curPos);
                        if(dist < minDistance){
                            minDistance = dist;
                            pos = curPos;
                        }
                    }
                }
            }
            missions[missionId] = Mission("Default", missionId, pos);
        }
    }
    vector<Action> generateNextAction(){
        setupMissions();
        cout << "# " ;
        for(auto mission: missions){
            cout << mission.missionName << " (" << mission.goalPos.x << ", " << mission.goalPos.y << ") ";
        }
        cout << endl;
        NextActions ans(state.humanNum);
        REP(i, 0, state.humanNum-1){
            Action nextAction = missions[i].generateAction(state, wallState, missions);
            ans.setIthAction(i, nextAction, state);
        }
        return ans.actions;
    }

    string generateNextCommand(){
        vector<Action> actions = generateNextAction();
        string ans;
        state.squaresWhichWillBeMovedTo.clear();
        state.squaresWichWillBeBlocked.clear();
        for(int i = 0; i < SIZE(actions); i++){
            state.humanAct(state.humans[i], actions[i]);
            ans.push_back(actionToChar(actions[i]));
        }
        return ans;
    }
    void remember(Animal& animal, char c){
        if(animal.type < 3) return;
        int dir = charToDir[c];
        int curDir = animal.dirsToMove[dir % 2];
        if(state.isBlocked(animal.pos.nextPos(curDir))){
            return;
        }
        animal.dirsToMove[dir % 2] = dir;
    }

    void solve(){
        REP(i,1,300){
            cout << "# turn" << i << endl; 
            string command = generateNextCommand();
            cout << command << endl;
            REP(i,0,state.animalNum-1){
                string s;
                cin >> s;
                state.animals[i].move(s);
                remember(state.animals[i], s[0]);
            }
        }
    }
};

int main(){
    int n,m;
    cin >> n;
    State state;
    REP(i,1,n){
        int a,b,c;
        cin >> a >> b >> c;
        a--; b--; c--;
        state.addAnimal(a, b, c);
    }
    cin >> m;
    REP(i,1,m){
        int a, b;
        cin >> a >> b;
        a--; b--;
        state.addHuman(a, b);
    }
    Solver solver = Solver(state);
    solver.solve();
}