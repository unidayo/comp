#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef unsigned long long ull;

#define mp make_pair
#define fr first
#define sc second

template<class T> T T_INF(){ return 1000000000000000000; }
template<> int T_INF<int>(){ return 1000000000; }

const int rat=1;

const int MAX_N=6000;
const int MAX_L=102;

int n,k;
int a[11];
int x[MAX_N],y[MAX_N];
pair<pair<int,int>,int> pts_x[MAX_N];
pair<pair<int,int>,int> pts_y[MAX_N];

void output_score(int cnt){
    int sum=0;
    for(int i=1;i<=10;i++)sum+=a[i];
    cerr<<cnt*1000000/sum<<endl;
}

int c[MAX_L][MAX_L];
struct ans_xy{
    int cx,xs[MAX_L];
    int cy,ys[MAX_L];
    int score;
    int s[MAX_N],t[MAX_N];
    int c[MAX_L][MAX_L];
    int b[12];
    void calc_score(){
        for(int i=1;i<=cx;i++)for(int j=1;j<=cy;j++)c[i][j]=0;
        for(int i=0;i<n;i++){
            s[i]=upper_bound(xs,xs+cx,x[i])-xs;
            t[i]=upper_bound(ys,ys+cy,y[i])-ys;
            c[s[i]][t[i]]++;
        }
        for(int i=0;i<=10;i++)b[i]=0;
        for(int i=1;i<=cx;i++)for(int j=1;j<=cy;j++){
            if(c[i][j]<=10)b[c[i][j]]++;
            else b[11]++;
        }
        score=-b[11]*rat;
        for(int i=1;i<=10;i++)score+=min(a[i],b[i]);
    }
    void out(){
        cout<<cx-1+cy-1<<endl;
        for(int i=1;i<=cx-1;i++)printf("%d %d %d %d\n",xs[i],-100000,xs[i]-1,100000);
        for(int i=1;i<=cy-1;i++)printf("%d %d %d %d\n",-100000,ys[i],100000,ys[i]-1);
    }
    inline void inc_c(int s_,int t_){
        if(c[s_][t_]<=10)b[c[s_][t_]]--;
        else b[11]--;
        c[s_][t_]++;
        if(c[s_][t_]<=10)b[c[s_][t_]]++;
        else b[11]++;
    }
    inline void dec_c(int s_,int t_){
        if(c[s_][t_]<=10)b[c[s_][t_]]--;
        else b[11]--;
        c[s_][t_]--;
        if(c[s_][t_]<=10)b[c[s_][t_]]++;
        else b[11]++;
    }
    int SHIFT_x(int r,pair<bool,int> d=mp(false,0)){
        if(!d.fr){
            d.sc=xs[r-1]+rand()%(xs[r+1]-xs[r-1]+1)-xs[r];
        }
        if(d.sc==0)return 0;
        if(d.sc>0){
            int i=lower_bound(pts_x,pts_x+n,mp(mp(xs[r],-10000),-1))-pts_x;
            while(i<n&&pts_x[i].fr.fr<xs[r]+d.sc){
                int j=pts_x[i].sc;
                dec_c(s[j],t[j]);
                s[j]--;
                inc_c(s[j],t[j]);
                i++;
            }
            score=-b[11]*rat;
            for(int i=1;i<=10;i++)score+=min(a[i],b[i]);
        }
        else {
            int i=lower_bound(pts_x,pts_x+n,mp(mp(xs[r]+d.sc,-10000),-1))-pts_x;
            while(i<n&&pts_x[i].fr.fr<xs[r]){
                int j=pts_x[i].sc;
                dec_c(s[j],t[j]);
                s[j]++;
                inc_c(s[j],t[j]);
                i++;
            }
            score=-b[11]*rat;
            for(int i=1;i<=10;i++)score+=min(a[i],b[i]);
        }
        xs[r]+=d.sc;
        return d.sc;
    }
    int SHIFT_y(int r,pair<bool,int> d=mp(false,0)){
        if(!d.fr){
            d.sc=ys[r-1]+rand()%(ys[r+1]-ys[r-1]+1)-ys[r];
        }
        if(d.sc==0)return 0;
        if(d.sc>0){
            int i=lower_bound(pts_y,pts_y+n,mp(mp(ys[r],-10000),-1))-pts_y;
            while(i<n&&pts_y[i].fr.fr<ys[r]+d.sc){
                int j=pts_y[i].sc;
                dec_c(s[j],t[j]);
                t[j]--;
                inc_c(s[j],t[j]);
                i++;
            }
            score=-b[11]*rat;
            for(int i=1;i<=10;i++)score+=min(a[i],b[i]);
        }
        else {
            int i=lower_bound(pts_y,pts_y+n,mp(mp(ys[r]+d.sc,-10000),-1))-pts_y;
            while(i<n&&pts_y[i].fr.fr<ys[r]){
                int j=pts_y[i].sc;
                dec_c(s[j],t[j]);
                t[j]++;
                inc_c(s[j],t[j]);
                i++;
            }
            score=-b[11]*rat;
            for(int i=1;i<=10;i++)score+=min(a[i],b[i]);
        }
        ys[r]+=d.sc;
        return d.sc;
    }
    bool try_SHIFT(){
        int r=rand()%(cx-1+cy-1);
        int cur=score;
        int d;
        if(r<cx-1)d=SHIFT_x(r+1);
        else d=SHIFT_y(r-(cx-1)+1);
        if(score>=cur)return true;
        if(r<cx-1)SHIFT_x(r+1,mp(true,-d));
        else SHIFT_y(r-(cx-1)+1,mp(true,-d));
        return false;
    }
    bool f(int e1,int e2,double temp){
        if(e1>=e2)return true;
        double B=exp((e1-e2)/temp);
        //cerr<<e1<<" "<<e2<<" "<<temp<<" "<<(int)(B*1000000)<<endl;
        return rand()%1000000<B*1000000;
    }
    bool try_SHIFT2(double temp){
        int r=rand()%(cx-1+cy-1);
        int cur=score;
        int d;
        if(r<cx-1)d=SHIFT_x(r+1);
        else d=SHIFT_y(r-(cx-1)+1);
        if(f(score,cur,temp))return true;
        if(r<cx-1)SHIFT_x(r+1,mp(true,-d));
        else SHIFT_y(r-(cx-1)+1,mp(true,-d));
        return false;
    }
};

ans_xy rand_ans_xy(int cx=40,int cy=40){
    ans_xy ret{};
    ret.cx=cx+1;
    for(int i=0;i<cx;i++)ret.xs[i]=rand()%20000-10000;
    ret.xs[cx]=-10000; ret.xs[cx+1]=10000;
    sort(ret.xs,ret.xs+cx+2);
    ret.cy=cy+1;
    for(int i=0;i<cy;i++)ret.ys[i]=rand()%20000-10000;
    ret.ys[cy]=-10000; ret.ys[cy+1]=10000;
    sort(ret.ys,ret.ys+cy+2);
    return ret;
}
ans_xy eqdiv(int cx=40,int cy=40){
    ans_xy ret{};
    ret.cx=cx+1;
    for(int i=0;i<cx;i++)ret.xs[i]=-10000+20000*(i+1)/ret.cx;
    ret.xs[cx]=-10000; ret.xs[cx+1]=10000;
    sort(ret.xs,ret.xs+cx+2);
    ret.cy=cy+1;
    for(int i=0;i<cy;i++)ret.ys[i]=-10000+20000*(i+1)/ret.cy;
    ret.ys[cy]=-10000; ret.ys[cy+1]=10000;
    sort(ret.ys,ret.ys+cy+2);
    return ret;
}

int main(){
    scanf("%d%d",&n,&k);
    for(int i=1;i<=10;i++)scanf("%d",&a[i]);
    for(int i=0;i<n;i++)scanf("%d%d",&x[i],&y[i]);
    
    for(int i=0;i<n;i++){
        pts_x[i]=mp(mp(x[i],y[i]),i);
        pts_y[i]=mp(mp(y[i],x[i]),i);
    }
    sort(pts_x,pts_x+n);
    sort(pts_y,pts_y+n);
    
    ans_xy best=eqdiv(40,40); best.calc_score();
    for(int a=3;a<=97;a++)for(int b=3;b<=97;b++){
        if(a+b>100||min(a,b)>15||a+b<80)continue;
        ans_xy temp=eqdiv(a,b);
        temp.calc_score();
        if(temp.score>best.score){
            best=temp;
            //cerr<<a<<" "<<b<<endl;
        }
    }
    /*while(clock()<1000000){
        int a=100,b=100;
        while(a+b>100){
            a=30+rand()%40;
            b=30+rand()%40;
        }
        ans_xy temp=rand_ans_xy(a,b);
        temp.calc_score();
        if(temp.score>best.score)best=temp;
    }*/
    int cnt_loop=0;
    int cc=0;
    clock_t cur_time=clock();
    while(1){
        if(cc==0){
            cur_time=clock();
            if(cur_time>2970000)break;
        }
        cnt_loop++;
        cc++; if(cc==10000)cc=0;
        //int cur=best.score;
        best.try_SHIFT2(pow(300.,(2000000.-(double)cur_time)/2000000.));
        /*if(best.score>cur){
            cerr<<clock()<<" "; output_score(best.score);
        }*/
    }
    cerr<<cnt_loop<<endl;
    best.out();
    output_score(best.score);
}



