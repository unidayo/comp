#include <bits/stdc++.h>
#define r(i,n) for(int i=0;i<n;i++)
using namespace std;


typedef pair<int,int>P;


void ASSERT(bool A,string s){
    if(!A)return;
    exit(1);
}

unsigned long long xor128() {
    static unsigned long long rx = 123456789, ry = 362436069, rz = 521288629, rw = 88675123;
    unsigned long long rt = (rx ^ (rx << 11));
    rx = ry;
    ry = rz;
    rz = rw;
    return (rw = (rw ^ (rw >> 19)) ^ (rt ^ (rt >> 8)));
}
std::chrono::system_clock::time_point start;
std::chrono::system_clock::time_point owari;

double gettime(){
    owari = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(owari-start).count();
    return elapsed; 
}
int N = 1000;

int Dist[1000*2+1][1000*2+1];
int tx[1000*2+1],ty[1000*2+1];
bool used[1001];
vector<int> v;

int bigdiff[1000] ;

int score_push(int A){
    int s=A*2;
    int t=A*2+1;
    int MIN_A=1e9,idx_A=-1,MIN_A_after=-1;
    int MIN_B=1e9,idx_B=-1,MIN_B_prev=-1;
    r(i,(int)v.size()-1){
        int dif_A = Dist[v[i]][s] + Dist[s][v[i+1]] - Dist[v[i]][v[i+1]]; 
        int dif_B = Dist[v[i]][t] + Dist[t][v[i+1]] - Dist[v[i]][v[i+1]]; 
        if(dif_A < MIN_A){
            MIN_A = dif_A;
            idx_A = i;
            MIN_A_after = Dist[s][v[i+1]];
        }
        if(dif_B < MIN_B){
            MIN_B = dif_B;
            idx_B = i;
            MIN_B_prev = Dist[v[i]][t];
        }
    }
    if(idx_A > idx_B) return 1e9;
    if( idx_A==idx_B ) return MIN_A + MIN_B + Dist[s][t] - MIN_A_after - MIN_B_prev +  Dist[v[idx_A]][v[idx_A+1]];
    return MIN_A + MIN_B ;
}
void score_push_(int A){
    int s=A*2;
    int t=A*2+1;
    int MIN_A=1e9,idx_A=-1,MIN_A_after=-1;
    int MIN_B=1e9,idx_B=-1,MIN_B_prev=-1;
    r(i,(int)v.size()-1){
        int dif_A = Dist[v[i]][s] + Dist[s][v[i+1]] - Dist[v[i]][v[i+1]]; 
        int dif_B = Dist[v[i]][t] + Dist[t][v[i+1]] - Dist[v[i]][v[i+1]]; 
        if(dif_A < MIN_A){
            MIN_A = dif_A;
            idx_A = i;
            MIN_A_after = Dist[s][v[i+1]];
        }
        if(dif_B < MIN_B){
            MIN_B = dif_B;
            idx_B = i;
            MIN_B_prev = Dist[v[i]][t];
        }
    }
    //cerr<<MIN_A<<' '<<MIN_B<<endl;
    //cerr<<idx_A<<' '<<idx_B<<endl;
    //cerr<<A<<' '<<s<<' '<<t<<endl;
    v.insert(v.begin()+idx_A+1,s);
    v.insert(v.begin()+idx_B+2,t);
}

int BestPush(int score){
    int MIN = 1e9;
    int idx = -1;
    r(i,N){
        if(used[i])continue;
        if(bigdiff[i]) continue;
        int sco = score_push(i);
        if( sco < MIN ){
            MIN = sco;
            idx = i;
        }
    }
    used[idx] = 1;
    score_push_(idx);
    return score + MIN;
}

int RandomPush(int score){
    int MIN = 1e9;
    int idx = -1;
    while(1){
        idx = xor128()%1000;
        if(bigdiff[idx]) continue;
        while( used[idx] ) idx = xor128()%1000;
        MIN = score_push(idx);
        if(MIN!=1e9)break;
    }
    used[idx] = 1;
    score_push_(idx);
    //cerr<<MIN<<endl;
    return score + MIN;
}

int calc_score(){
    int sum = 0;
    r(i,(int)v.size()-1){
        sum += abs(tx[v[i]]-tx[v[i+1]]) + abs(ty[v[i]]-ty[v[i+1]]); 
    }
    return sum;
}

vector<int>tmp1;
int used_tmp[1000];
int tmpscore;

int ROT[1000];
void rot(){
    r(i,1000)swap(ROT[i],ROT[xor128()%1000]);
}
int score = 0;
void TMP(){
    tmp1=v;
    r(i,1000)used_tmp[i]=used[i];
    tmpscore = score;
}
void RollBack(){
    v=tmp1;
    r(i,1000)used[i]=used_tmp[i];
    score = tmpscore;
}

vector<int> MAX_v;
int MAX_used[1000];
int MAX_score;

void MEM_MAX(){
    MAX_v=v;
    r(i,1000)MAX_used[i]=used[i];
    MAX_score = score;


}

void solve(){
    r(i,1000)ROT[i]=i;
    used[N] = 0;
    v.emplace_back(N*2);
    v.emplace_back(N*2);
    r(i,50){
        score = BestPush(score);
    }

    MEM_MAX();

    r(i,5){
        score = 0;
        r(i,1000)used[i]=0;
        while((int)v.size()>2)v.erase(v.begin()+1);
        score = RandomPush(score);
        r(i,49){
            score = BestPush(score);
        }

        if(score < MAX_score){
            MEM_MAX();
        }

    }
    score = MAX_score;
    v = MAX_v;
    r(i,1000)used[i]=MAX_used[i];
    //cerr<<score<<endl;
    //exit(0);

    int MAX = score;

    MEM_MAX();

    vector<int> tim({1950});
    int iteration = 0;

    r(iter,(int)tim.size()){
        if(0)
        {
            score = 0;
            r(i,1000)used[i]=0;
            while((int)v.size()>2)v.erase(v.begin()+1);
            r(i,50){
                score = RandomPush(score);
            }
            cerr<<score<<endl;
        }

        while(gettime()<=tim[iter]){
            iteration++;
            //cerr<<"1"<<endl;
            TMP();
            rot();
            int cnt=0;
            int u = xor128()%4;
            if(u==0){
                r(i,((200-iteration)/5)){
                    while(used[ROT[cnt]]==0) cnt++;
                    used[ ROT[cnt] ] = 0;
                    int a=-1,b=-1;
                    r(j,(int)v.size()){
                        if(v[j]==ROT[cnt]*2) a=j;
                        if(v[j]==ROT[cnt]*2+1) b=j;
                    }
                    if(a==-1)cerr<<"a"<<endl,exit(1);
                    if(b==-1)cerr<<"a"<<endl,exit(1);
                    if(a>=b)exit(1);
                    v.erase(v.begin()+a);
                    v.erase(v.begin()+b-1);
                }
            }
            else{
                int a=(xor128()%100) + 1;
                int b= min((int)100,((int)a +(int)(xor128()%25)+5));
                //cerr<<a<<' '<<b<<endl;
                unordered_set<int>ord;
                vector<int>idx;
                for(int i=a;i<=b;i++){
                    used[v[i]/2]=0;
                    ord.insert(v[i]/2);
                }
                r(i,(int)v.size()){
                    if(ord.count(v[i]/2)){
                        idx.emplace_back(i);
                    }
                }
                reverse(idx.begin(),idx.end());
                r(i,(int)idx.size()){
                    v.erase(v.begin()+idx[i]);
                }
            }
            score = calc_score();

            while((int)v.size()<102)score = BestPush(score);
            if(score <= tmpscore ){
                if(score < MAX){
                    MAX=score;
                    MEM_MAX();
                    cerr<<"best score = "<<MAX<<"          "<<iteration<<endl;
                }
            }
            else{
                RollBack();
            }
        }
        cerr<<"ITER"<<iter<<"score = "<<score<<endl;
    }
    cerr<<"BEST_SCORE: "<<score<<endl;
}
void out(){
    cout<<50;
    r(i,N){
        if(MAX_used[i]==false)continue;
        cout<<' '<<i+1;
    }
    cout<<endl;
    cout<<102;
    r(i,(int)MAX_v.size())cout<<' '<<tx[MAX_v[i]]<<' '<<ty[MAX_v[i]];
    cout<<endl;
    //cerr<<calc_score()<<endl;
}

int main(){
    start = std::chrono::system_clock::now();
    r(i,N*2){
        cin>>tx[i]>>ty[i];
    }
    tx[N*2] = 400;
    ty[N*2] = 400;
    r(i,N*2+1){
        r(j,N*2+1){
            Dist[i][j] = abs(tx[i]-tx[j]) + abs(ty[i]-ty[j]);
        }
    }
    r(i,1000){
        if(Dist[i*2][i*2+1] >= 1000) bigdiff[i] = 1;
    }
   solve();
   out();
   cerr<<"Elapsed Time = "<<gettime()<<endl;
}

