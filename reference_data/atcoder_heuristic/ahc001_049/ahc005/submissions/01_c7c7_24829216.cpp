#include <bits/stdc++.h>
using namespace std;
#define r(i,n) for(int i=0;i<n;i++)

std::chrono::system_clock::time_point start;
std::chrono::system_clock::time_point owari;

double gettime(){
owari = std::chrono::system_clock::now();

// erapsed = ms
double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(owari-start).count();
return elapsed;
}
typedef pair<int,int>P;

int n,sx,sy;
string s[100];
int A[100][100];

int N;
vector<P>injection;
int inj[100][100];
int C[600][600];
string walk[600][600];

int TN;
vector<vector<int>>T;

bool in(int a,int b){ return 0<=a&&a<n&&0<=b&&b<n; }

void input();
void make_graph();
void make_tate();

int YYY,XXX,TTT,CCC;
#define F first
#define S second
unsigned long long xor128() {
    static unsigned long long rx = 123456789, ry = 362436069, rz = 521288629, rw = 88675123;
    unsigned long long rt = (rx ^ (rx << 11));
    rx = ry;
    ry = rz;
    rz = rw;
    return (rw = (rw ^ (rw >> 19)) ^ (rt ^ (rt >> 8)));
}

inline int s_score(int a,int b){
    return C[a][b];
}

inline int GetID(vector<P> &v,int i){
    return T[v[i].F][v[i].S];
}

int score(vector<P> &v){
    int sum = 0;
    for(int i=0;i<(int)v.size()-1;i++){
        sum += C[T[v[i].F][v[i].S]][T[v[i+1].F][v[i+1].S]];
    }
    return sum;
}

int scoremax(vector<P> &v){
    int sum = 0;
    for(int i=0;i<(int)v.size()-1;i++){
        sum = max(sum,C[T[v[i].F][v[i].S]][T[v[i+1].F][v[i+1].S]]);
    }
    return sum;
}

int NA(vector<P> &v){
    int id = (xor128()%TN)+1;
    P p = v[id];
    //cerr<<id<<endl;

    int a = s_score( GetID(v,id-1), GetID(v,id+1) );
    int b = s_score( GetID(v,id-1), GetID(v,id) );
    int c = s_score( GetID(v,id), GetID(v,id+1) );

    int SA = a-b-c;
    int MINI = SA;
    //cerr<<SA<<endl;
    v.erase(v.begin()+id);


    int MIN = 1e9;
    int id1 = -1;
    int id2 = -1;
    r(i,TN+1)if(i){
        r(j,(int)T[p.F].size()){
            int a = s_score( GetID(v,i-1), GetID(v,i) );
            int b = s_score( GetID(v,i-1), T[p.F][j] );
            int c = s_score( T[p.F][j]   , GetID(v,i) );
            if( b+c-a <= MIN ){
                MIN = b+c-a;
                id1=i;
                id2=j;
            }
        }
    }
    v.insert(v.begin()+id1,P(p.F,id2));

    SA += MIN;

    return SA;
}

vector<P> tmp;

int DA(vector<P> &v){
    tmp = v;
    int AAA = score(v);

    int sz = (xor128()%(TN-1))+1;
    vector<P>p1;

    r(k,sz){
        int id = (xor128()%(TN-k))+1;
        p1.emplace_back(v[id]);
        v.erase(v.begin()+id);
    }

    r(k,sz){
        P p = p1[k];
        int MIN = 1e9;
        int id1 = -1;
        int id2 = -1;
        r(i,TN+1 - sz+k)if(i){
            r(j,(int)T[p.F].size()){
                int a = s_score( GetID(v,i-1), GetID(v,i) );
                int b = s_score( GetID(v,i-1), T[p.F][j] );
                int c = s_score( T[p.F][j]   , GetID(v,i) );
                if( b+c-a <= MIN ){
                    MIN = b+c-a;
                    id1=i;
                    id2=j;
                }
            }
        }
        v.insert(v.begin()+id1,P(p.F,id2));
    }

    int BBB = score(v);
    int SA = BBB-AAA;

    return SA;
}

void solve(){
    vector<P>v,ANS;
    v.emplace_back(P(TTT,CCC));
    r(i,TN) v.emplace_back(i,0);
    v.emplace_back(P(TTT,CCC));
    cerr<<v.size()<<endl;
    int cur = score(v);
    int MAX = cur;
    int cnt = 0;
        while(gettime()<=1960){

            int div = xor128()%60?1:0;
            int SA = div?NA(v):DA(v);

            if(SA <= 0 || cnt>=7){
                cur += SA;
                if(div==0){
                    cnt=0;
                }
            }
            else{
                if(div == 0){
                    v = tmp;
                    cnt++;
                }
            }
            if(MAX>cur){
                MAX = cur;
                ANS = v;
                cerr<<MAX<<endl;
            }
        }

    
    v=ANS;
    cerr<<score(v)<<endl;
    cerr<<GetID(v,0)<<endl;
    cerr<<GetID(v,TN+1)<<endl;
    r(i,TN+1){
        int a = GetID(v,i);
        int b = GetID(v,i+1);
        cout<<walk[a][b];
    }
    cout<<endl;
    cerr<<inj[sy][sx]<<endl;
    cerr<<"end"<<endl;
    cerr<<scoremax(v)<<endl;
}

int main(){
    start = std::chrono::system_clock::now();
    memset(inj,-1,sizeof(inj));
    input();
    make_graph();
    make_tate();
    solve();
}


void input(){
    cin>>n>>sy>>sx;
    r(i,n)cin>>s[i];
    r(i,n){
        r(j,n){
            int c = s[i][j]=='#'? -1 :s[i][j]-'0';
            A[i][j] = c;
        }
    }
}
int dy[]={-1,0,1,0};
int dx[]={0,1,0,-1};

char DIR1[]={'U','R','D','L'};

void dij(int i,int j){
    int used[n][n] = {};
    string path[n][n];
    r(i,n)r(j,n)used[i][j] = 1e9,path[i][j]="";
    priority_queue<P,vector<P>,greater<P> >q;
    q.push(P(0,i*10000+j));
    used[i][j] = 0; 
    while(!q.empty()){
        int c = q.top().first;
        int y = q.top().second/10000;
        int x = q.top().second%10000;
        q.pop();
        if( used[y][x] < c ) continue;
        r(k,4){
            int ny=y+dy[k];
            int nx=x+dx[k];
            if(!in(ny,nx))continue;
            if(A[ny][nx]==-1)continue;
            int nc = c+A[ny][nx];
            if(used[ny][nx] <= nc) continue;
            used[ny][nx] = nc;
            path[ny][nx] = path[y][x] + DIR1[k];
            q.push(P(nc,ny*10000+nx));
        }
    }
    r(y,n){
        r(x,n){
            if(inj[y][x]!=-1){
                if(A[y][x]==-1) exit(1);
                C[inj[i][j]][inj[y][x]] = used[y][x];
                walk[inj[i][j]][inj[y][x]] = path[y][x];
            }
        }
    }


}

void make_graph(){
    N = 1;
    inj[sy][sx] = 0;
    injection.emplace_back(P(sy,sx));
    r(i,n){
        r(j,n){
            if(i==sy&&j==sx) continue;
            if(A[i][j]==-1)continue;
            set<int> cnt;
            r(k,4){
                int y=i+dy[k];
                int x=j+dx[k];
                if(!in(y,x))continue;
                if(A[y][x]!=-1)cnt.insert(k%2);
            }
            if(cnt.size() == 2){
                N++;
                injection.emplace_back(P(i,j));
                inj[i][j] = N-1;
            }
        }
    }
    cerr<<"Num of injection = "<<N<<endl;
    r(i,n){
        r(j,n){
            if(inj[i][j]==-1) continue;
            dij(i,j);
        }
    }
    r(i,10){
        r(j,10){
            cerr<<C[i][j]<<' ';
        }
        cerr<<endl;
    }
}


void make_tate(){
    int used[n][n];
    memset(used,0,sizeof(used));
    r(i,n){
        r(j,n-1){
            if(A[i][j]!=-1 && A[i][j+1]!=-1 && used[i][j]==0 ){
                int y=i,x=j;
                vector<int>v;
                int cnt=0;
                while(1){
                    used[y][x]=1;
                    if(inj[y][x]!=-1){
                        if(y==sy && x==sx){
                            CCC = cnt;
                            YYY = y;
                            XXX = x;
                            TTT = TN;
                        }
                        v.emplace_back(inj[y][x]);
                        cnt++;
                    }
                    x++;
                    if(!in(y,x))break;
                    if(A[y][x]==-1)break;
                }
                if((int)v.size()==0)exit(1);
                T.emplace_back(v);
                TN++;
            }
        }
    }
    cerr<<TN<<endl;
    cerr<<n<<endl;

    memset(used,0,sizeof(used));
    r(i,n-1){
        r(j,n){
            if(A[i][j]!=-1 && A[i+1][j]!=-1 && used[i][j]==0){
                int y=i,x=j;
                vector<int>v;
                int cnt=0;
                while(1){
                    used[y][x]=1;
                    if(inj[y][x]!=-1){
                        if(y==sy && x==sx){
                            CCC = cnt;
                            YYY = y;
                            XXX = x;
                            TTT = TN;
                        }
                        v.emplace_back(inj[y][x]);cnt++;
                    }
                    y++;
                    if(!in(y,x))break;
                    if(A[y][x]==-1)break;
                }
                if((int)v.size()==0)exit(1);
                T.emplace_back(v);
                TN++;
            }
        }
    }
    cerr<<TN<<endl;

}