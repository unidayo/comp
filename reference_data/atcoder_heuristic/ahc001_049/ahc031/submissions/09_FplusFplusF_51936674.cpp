#include <bits/stdc++.h>
using namespace std;
using pii=pair<int,int>;
using tii=tuple<int,int,int>;
using qii=tuple<int,int,int,int>;
using ll=long long;
using ld=long double;
const int INF=1e9;
const ll INF_ll=1e18;
#define rep(i,n) for (int i=0;i<(int)(n);i++)
#define all(v) v.begin(),v.end()
template<class T> inline bool chmin(T &a,T b){
    if(a>b){
        a=b;
        return true;
    }
    return false;
}
template<class T> inline bool chmax(T &a,T b){
    if(a<b){
        a=b;
        return true;
    }
    return false;
}
auto program_start=chrono::system_clock::now();
mt19937 mt;
int rand_int(int r,source_location loc=source_location::current()){  //[0,r)
    if(r==0){
        clog << loc.file_name() << ":(" << loc.line() << ":" << loc.column() << "):" << loc.function_name() << endl;
        exit(EXIT_FAILURE);
    }
    return mt()%r;
}
int rand_int(int l,int r){  //[l,r)
    return l+mt()%(r-l);
}
double rand_double(){  //[0.0,1.0]
    return (double)mt()/(double)mt19937::max();
}

constexpr int MA=19,MB=93,MC=9,MD=56,ME=78,MF=45;


int W,D,N;

vector<vector<int>> A;
struct rectangle{
    int si=-1,sj=-1,gi=-1,gj=-1;
};

bool operator==(const rectangle a,const rectangle b){
    return (a.si==b.si)&&(a.sj==b.sj)&&(a.gi==b.gi)&&(a.gj==b.gj);
}
namespace Solver{
    int to(int i,int j){
        return i*W+j;
    }
    vector<bitset<1000>> pre_ud_line(1000),pre_lr_line(1000),new_ud_line(1000),new_lr_line(1000);
    vector<bitset<1000>> seg_mask(1001);

    vector<vector<vector<int>>> seg_max_a;  //[l][r][i]=max(A_l_i,A_l+1_i,…,A_r-1_i)

    void init(){
        string s(1000,'0');
        rep(i,1001){
            bitset<1000> now(s);
            seg_mask[i]=now;
            if(i==1000) break;
            s[i]='1';
        }

        seg_max_a.assign(D+1,vector<vector<int>>(D+1,vector<int>(N,0)));   
        rep(l,D+1){
            for(int r=l;r<D;r++){
                rep(i,N){
                    seg_max_a[l][r+1][i]=max(seg_max_a[l][r][i],A[r][i]);
                }
            }
        }
    }
    ll diff_score(vector<vector<rectangle>> ans){  //枠のズレのコストだけ計算
        int sz=(int)ans.size();
        ll ret=0;
        rep(t,sz){
            if((int)ans[t].size()!=N) return INF_ll;
            rep(i,N){
                if(ans[t][i].si<0||W<ans[t][i].si) return INF_ll;
                if(ans[t][i].sj<0||W<ans[t][i].sj) return INF_ll;
                if(ans[t][i].gi<0||W<ans[t][i].gi) return INF_ll;
                if(ans[t][i].gj<0||W<ans[t][i].gj) return INF_ll;
                if(ans[t][i].gi<=ans[t][i].si) return INF_ll;
                if(ans[t][i].gj<=ans[t][i].sj) return INF_ll;
                rep(j,i){
                    if(max(ans[t][i].si,ans[t][j].si)<min(ans[t][i].gi,ans[t][j].gi)&&max(ans[t][i].sj,ans[t][j].sj)<min(ans[t][i].gj,ans[t][j].gj)) return INF_ll;
                }
            }
        }
        
        //lr,udは直線の向き
        vector<set<int>> ud_set(sz),lr_set(sz),seg_ud_set(sz),seg_lr_set(sz);
        rep(t,sz){
            rep(i,N){
                if(0<ans[t][i].sj) ud_set[t].insert(ans[t][i].sj);
                if(ans[t][i].gj<W) ud_set[t].insert(ans[t][i].gj);
                if(0<ans[t][i].si) lr_set[t].insert(ans[t][i].si);
                if(ans[t][i].gi<W) lr_set[t].insert(ans[t][i].gi);
            }
            if(t==0) continue;
            seg_ud_set[t].insert(all(ud_set[t]));
            seg_ud_set[t].insert(all(ud_set[t-1]));
            seg_lr_set[t].insert(all(lr_set[t]));
            seg_lr_set[t].insert(all(lr_set[t-1]));
        }
        rep(t,sz){
            rep(i,N){
                if(0<ans[t][i].sj) new_ud_line[ans[t][i].sj]|=(seg_mask[ans[t][i].gi-ans[t][i].si]>>ans[t][i].si);
                if(ans[t][i].gj<W) new_ud_line[ans[t][i].gj]|=(seg_mask[ans[t][i].gi-ans[t][i].si]>>ans[t][i].si);
                if(0<ans[t][i].si) new_lr_line[ans[t][i].si]|=(seg_mask[ans[t][i].gj-ans[t][i].sj]>>ans[t][i].sj);
                if(ans[t][i].gi<W) new_lr_line[ans[t][i].gi]|=(seg_mask[ans[t][i].gj-ans[t][i].sj]>>ans[t][i].sj);
            }
            if(t!=0){
                for(auto i:seg_ud_set[t]) ret+=(pre_ud_line[i]^new_ud_line[i]).count();
                for(auto i:seg_lr_set[t]){
                    ret+=(pre_lr_line[i]^new_lr_line[i]).count();
                }
            }
            swap(pre_ud_line,new_ud_line);
            swap(pre_lr_line,new_lr_line);
            if(t!=0){
                for(auto i:ud_set[t-1]) new_ud_line[i].reset();
                for(auto i:lr_set[t-1]) new_lr_line[i].reset();
                if(t==sz-1){
                    for(auto i:ud_set[t]) pre_ud_line[i].reset();
                    for(auto i:lr_set[t]) pre_lr_line[i].reset();
                }
            }
        }
        return ret+1;
    }
    ll score(vector<vector<rectangle>> ans){  //頑張って計算をするかセグメント木をうまく使うかでもっと高速になりそう
        if((int)ans.size()!=D) return INF_ll;
        ll ret=0;
        rep(t,D){
            if((int)ans[t].size()!=N) return INF_ll;
            rep(i,N){
                if(ans[t][i].si<0||W<ans[t][i].si) return INF_ll;
                if(ans[t][i].sj<0||W<ans[t][i].sj) return INF_ll;
                if(ans[t][i].gi<0||W<ans[t][i].gi) return INF_ll;
                if(ans[t][i].gj<0||W<ans[t][i].gj) return INF_ll;
                if(ans[t][i].gi<=ans[t][i].si) return INF_ll;
                if(ans[t][i].gj<=ans[t][i].sj) return INF_ll;
                rep(j,i){
                    if(max(ans[t][i].si,ans[t][j].si)<min(ans[t][i].gi,ans[t][j].gi)&&max(ans[t][i].sj,ans[t][j].sj)<min(ans[t][i].gj,ans[t][j].gj)) return INF_ll;
                }
                ret+=max(0ll,100ll*(A[t][i]-(ans[t][i].gi-ans[t][i].si)*(ans[t][i].gj-ans[t][i].sj)));
            }
        }
        //lr,udは直線の向き
        vector<set<int>> ud_set(D),lr_set(D),seg_ud_set(D),seg_lr_set(D);
        rep(t,D){
            rep(i,N){
                if(0<ans[t][i].sj) ud_set[t].insert(ans[t][i].sj);
                if(ans[t][i].gj<W) ud_set[t].insert(ans[t][i].gj);
                if(0<ans[t][i].si) lr_set[t].insert(ans[t][i].si);
                if(ans[t][i].gi<W) lr_set[t].insert(ans[t][i].gi);
            }
            if(t==0) continue;
            seg_ud_set[t].insert(all(ud_set[t]));
            seg_ud_set[t].insert(all(ud_set[t-1]));
            seg_lr_set[t].insert(all(lr_set[t]));
            seg_lr_set[t].insert(all(lr_set[t-1]));
        }
        rep(t,D){
            rep(i,N){
                if(0<ans[t][i].sj) new_ud_line[ans[t][i].sj]|=(seg_mask[ans[t][i].gi-ans[t][i].si]>>ans[t][i].si);
                if(ans[t][i].gj<W) new_ud_line[ans[t][i].gj]|=(seg_mask[ans[t][i].gi-ans[t][i].si]>>ans[t][i].si);
                if(0<ans[t][i].si) new_lr_line[ans[t][i].si]|=(seg_mask[ans[t][i].gj-ans[t][i].sj]>>ans[t][i].sj);
                if(ans[t][i].gi<W) new_lr_line[ans[t][i].gi]|=(seg_mask[ans[t][i].gj-ans[t][i].sj]>>ans[t][i].sj);
            }
            if(t!=0){
                for(auto i:seg_ud_set[t]) ret+=(pre_ud_line[i]^new_ud_line[i]).count();
                for(auto i:seg_lr_set[t]){
                    ret+=(pre_lr_line[i]^new_lr_line[i]).count();
                }
            }
            swap(pre_ud_line,new_ud_line);
            swap(pre_lr_line,new_lr_line);
            if(t!=0){
                for(auto i:ud_set[t-1]) new_ud_line[i].reset();
                for(auto i:lr_set[t-1]) new_lr_line[i].reset();
                if(t==D-1){
                    for(auto i:ud_set[t]) pre_ud_line[i].reset();
                    for(auto i:lr_set[t]) pre_lr_line[i].reset();
                }
            }
            
        }
        return ret+1;
    }
    vector<rectangle> push_width_x(vector<int> &width,vector<int> &a){
        vector<rectangle> ret(N);
        int sz=(int)width.size();
        vector<int> ord(sz);
        rep(i,sz) ord[i]=i;
        sort(all(ord),[&](int i,int j){return width[i]>width[j];});
        vector<int> nsi(sz,0);
        vector<int> nsj(sz,0);
        rep(i,sz-1) nsj[i+1]=nsj[i]+width[i];
        vector<int> last_push(sz,-1);
        for(int i=N-1;0<=i;i--){
            int x=-1;
            for(auto j:ord){
                int d=(a[i]+width[j]-1)/width[j];
                if(W<nsi[j]+d) continue;
                x=j;
                break;
            }
            int j=x;
            if(j==-1) return {};
            int d=(a[i]+width[j]-1)/width[j];
            ret[i]={nsi[j],nsj[j],nsi[j]+d,nsj[j]+width[j]};
            nsi[j]+=d;
            last_push[j]=i;
        }
        rep(i,sz){
            if(last_push[i]==-1) continue;
            ret[last_push[i]].gi=W;
        }
        return ret;
    }
    vector<rectangle> push_width_y(vector<int> &width,vector<int> &a){
        vector<rectangle> ret(N);
        int sz=(int)width.size();
        vector<int> nsi(sz,0);
        vector<int> nsj(sz,0);
        rep(i,sz-1) nsj[i+1]=nsj[i]+width[i];
        vector<int> last_push(sz,-1);
        for(int i=N-1;0<=i;i--){
            pii mn={INF,INF};
            rep(j,sz){
                int d=(a[i]+width[j]-1)/width[j];
                if(W<nsi[j]+d) continue;
                chmin(mn,{(W-nsi[j])*width[j],j});
                break;
            }
            int j=mn.second;
            if(j==INF) return {};
            int d=(a[i]+width[j]-1)/width[j];
            ret[i]={nsi[j],nsj[j],nsi[j]+d,nsj[j]+width[j]};
            nsi[j]+=d;
            last_push[j]=i;
        }
        rep(i,sz){
            if(last_push[i]==-1) continue;
            ret[last_push[i]].gi=W;
        }
        return ret;
    }
    vector<rectangle> push_width_z(vector<int> &width,vector<int> &a){
        vector<rectangle> ret(N);
        int sz=(int)width.size();
        vector<int> nsi(sz,0);
        vector<int> nsj(sz,0);
        rep(i,sz-1) nsj[i+1]=nsj[i]+width[i];
        vector<int> last_push(sz,-1);
        for(int i=N-1;0<=i;i--){
            pii mx={-INF,-INF};
            rep(j,sz){
                int d=(a[i]+width[j]-1)/width[j];
                if(W<nsi[j]+d) continue;
                chmax(mx,{(W-nsi[j])*width[j],j});
                break;
            }
            int j=mx.second;
            if(j==-INF) return {};
            int d=(a[i]+width[j]-1)/width[j];
            ret[i]={nsi[j],nsj[j],nsi[j]+d,nsj[j]+width[j]};
            nsi[j]+=d;
            last_push[j]=i;
        }
        rep(i,sz){
            if(last_push[i]==-1) continue;
            ret[last_push[i]].gi=W;
        }
        return ret;
    }
    vector<rectangle> push_width_a(vector<int> &width,vector<int> &a){
        vector<rectangle> ret(N);
        int sz=(int)width.size();
        vector<int> nsi(sz,0);
        vector<int> nsj(sz,0);
        rep(i,sz-1) nsj[i+1]=nsj[i]+width[i];
        vector<int> last_push(sz,-1);
        for(int i=N-1;0<=i;i--){
            pii mn={INF,INF};
            int mn2=INF;
            rep(j,sz){
                int d=(a[i]+width[j]-1)/width[j];
                if(W<nsi[j]+d) continue;
                if(nsi[j]==0) chmin(mn2,j);
                else chmin(mn,{nsi[j],j});
            }
            int j=mn.second;
            if(j==INF) j=mn2;
            if(j==INF) return {};
            int d=(a[i]+width[j]-1)/width[j];
            ret[i]={nsi[j],nsj[j],nsi[j]+d,nsj[j]+width[j]};
            nsi[j]+=d;
            last_push[j]=i;
        }
        rep(i,sz){
            if(last_push[i]==-1) continue;
            ret[last_push[i]].gi=W;
        }
        return ret;
    }
    vector<rectangle> push_width_b(vector<int> &width,vector<int> &a){
        vector<rectangle> ret(N);
        int sz=(int)width.size();
        vector<int> nsi(sz,0);
        vector<int> nsj(sz,0);
        rep(i,sz-1) nsj[i+1]=nsj[i]+width[i];
        vector<int> last_push(sz,-1);
        for(int i=N-1;0<=i;i--){
            pii mx={-INF,-INF};
            int mn2=-INF;
            rep(j,sz){
                int d=(a[i]+width[j]-1)/width[j];
                if(W<nsi[j]+d) continue;
                if(nsi[j]==0) chmax(mn2,j);
                else chmax(mx,{nsi[j],j});
            }
            int j=mx.second;
            if(j==-INF) j=mn2;
            if(j==-INF) return {};
            int d=(a[i]+width[j]-1)/width[j];
            ret[i]={nsi[j],nsj[j],nsi[j]+d,nsj[j]+width[j]};
            nsi[j]+=d;
            last_push[j]=i;
        }
        rep(i,sz){
            if(last_push[i]==-1) continue;
            ret[last_push[i]].gi=W;
        }
        return ret;
    }
    vector<rectangle> push_width_c(vector<int> &width,vector<int> &a){
        vector<rectangle> ret(N);
        int sz=(int)width.size();
        vector<int> nsi(sz,0);
        vector<int> nsj(sz,0);
        rep(i,sz-1) nsj[i+1]=nsj[i]+width[i];
        vector<int> last_push(sz,-1);
        for(int i=N-1;0<=i;i--){
            bool pushed=false;
            rep(j,sz){
                int d=(a[i]+width[j]-1)/width[j];
                if(W<nsi[j]+d) continue;
                ret[i]={nsi[j],nsj[j],nsi[j]+d,nsj[j]+width[j]};
                nsi[j]+=d;
                last_push[j]=i;
                pushed=true;
                break;
            }
            if(!pushed) return {};
        }
        rep(i,sz){
            if(last_push[i]==-1) continue;
            ret[last_push[i]].gi=W;
        }
        return ret;
    }

    vector<rectangle> push_width(vector<int> &width,vector<int> &a){
        vector<rectangle> ret=push_width_z(width,a);
        if(!ret.empty()) return ret;
        ret=push_width_x(width,a);
        if(!ret.empty()) return ret;
        ret=push_width_y(width,a);
        if(!ret.empty()) return ret;
        ret=push_width_a(width,a);
        if(!ret.empty()) return ret;
        ret=push_width_b(width,a);
        if(!ret.empty()) return ret;
        return push_width_c(width,a);
    }

    vector<vector<rectangle>> width_to_ans(vector<int> &width){
        vector<vector<rectangle>> ret(D);
        int l=0,r=0;
        while(l<D){
            while(r<D&&!push_width(width,seg_max_a[l][r+1]).empty()) r++;
            if(l==r) return {};
            vector<rectangle> now=push_width(width,seg_max_a[l][r]);
            while(l<r){
                ret[l]=now;
                l++;
            }
        }
        return ret;
    }

    namespace HC_ud{
        auto score_calc=[](vector<int> &width){
            vector<vector<rectangle>> ans=width_to_ans(width);
            ll true_score=score(ans);
            if(true_score==INF_ll) return 1e36;
            unordered_map<int,int> mp;
            rep(t,D){
                rep(i,N) mp[ans[t][i].gj-ans[t][i].sj]++;
            }
            double r=0;
            for(auto [k,v]:mp) r+=(double)k*k*v;
            return true_score*r*r;
        };
        
        constexpr int modify_type=2;
        vector<int> great,ok,miss;
        double pre_score=1e36;

        auto solution_update=[](int modify_idx,double new_score,int &update_k){
            if(new_score<=pre_score){
                
                if(pre_score!=new_score) update_k=0;
                if(pre_score<=new_score) ok[modify_idx]++;
                else great[modify_idx]++;
                pre_score=new_score;

                return true;
            }else{
                miss[modify_idx]++;
                return false;
            }
        };

        vector<vector<rectangle>> solve(vector<vector<int>> first_width_v){
            int v_sz=(int)first_width_v.size(),now_idx=0;
            double min_score=score_calc(first_width_v[now_idx]);
            vector<int> min_score_width=first_width_v[now_idx];
            great.assign(modify_type,0),ok.assign(modify_type,0),miss.assign(modify_type,0);
            while(true){
                auto now=chrono::system_clock::now();
                int ms=chrono::duration_cast<chrono::milliseconds>(now-program_start).count();
                if(500<ms) break;
                vector<int> width=first_width_v[now_idx];
                int w_sz=(int)width.size();
                if(w_sz==1) return width_to_ans(width);

                pre_score=score_calc(width);
                int cnt_loop=0,update_k=0;
                while(true){
                    cnt_loop++;
                    update_k++;
                    if(170<=update_k) break;
                    auto now2=chrono::system_clock::now();
                    int ms2=chrono::duration_cast<chrono::milliseconds>(now2-program_start).count();
                    if(500<ms2) break;
                    int r=rand_int(modify_type);
                    if(r==0){
                        int x=rand_int(w_sz),y=rand_int(w_sz);
                        if(min(width[x],width[y])<=1) continue;
                        int p=rand_int(1,min(width[x],width[y]));
                        if(W<=width[x]+p||width[y]-p<=0) continue;
                        width[x]+=p;
                        width[y]-=p;
                        double new_score=score_calc(width);
                        bool res=solution_update(0,new_score,update_k);
                        if(!res){
                            width[x]-=p;
                            width[y]+=p;
                        }
                    }else{
                        int x=rand_int(w_sz);
                        if(width[x]<=1) continue;
                        vector<int> new_width=width;
                        int new_push=rand_int(1,new_width[x]);
                        new_width[x]-=new_push;
                        new_width.push_back(new_push);
                        double new_score=score_calc(new_width);
                        bool res=solution_update(1,new_score,update_k);
                        if(res){
                            width=new_width;
                            w_sz=(int)width.size();
                        }
                    }
                    
                }
                //cerr << pre_score << " " <<update_k << endl;
                if(chmin(min_score,pre_score)) min_score_width=width;
                now_idx++;
                if(v_sz<=now_idx) now_idx=0;
            }
            /*int sum=0;
            rep(i,modify_type){
                cerr << great[i] << " " << ok[i] << " " << miss[i] << endl;
                sum+=great[i]+ok[i]+miss[i];
            }
            cerr << fixed << setprecision(10) << sum  << " " << min_score << " " << score(width_to_ans(min_score_width)) << endl;*/
            return width_to_ans(min_score_width);
        }
    }

    vector<vector<rectangle>> solve_a(){
        vector<vector<int>> width(W+1);
        vector<int> cnt(W+1,0);
        for(int w=1;w<=W;w++){
            int sum=0;
            while(sum<W){
                if(sum+w<=W) width[w].push_back(w);
                else width[w].push_back(W-sum);
                sum+=w;
            }
            rep(t,D) if(!push_width(width[w],A[t]).empty()) cnt[w]++;
        }

        vector<ll> score_v;
        int cnt_charange=0;
        vector<vector<int>> width_v;
        for(int w=1;w<=W;w++){
            if(cnt[w]!=D) continue;
            cnt_charange++;
            if(10<=cnt_charange) break;

            auto now=chrono::system_clock::now();
            int ms=chrono::duration_cast<chrono::milliseconds>(now-program_start).count();
            if(2700<ms) break;
            vector<vector<rectangle>> now_ans=width_to_ans(width[w]);
            ll now_score=score(now_ans);
            score_v.push_back(now_score);
            width_v.push_back(width[w]);
        }
        if(width_v.empty()) return {};
        int sz=(int)width_v.size();
        vector<int> ord(sz);
        rep(i,sz) ord[i]=i;
        sort(all(ord),[&](int i,int j){return score_v[i]<score_v[j];});
        vector<vector<int>> first_width_v(sz);
        rep(i,sz) first_width_v[i]=width_v[ord[i]];
        return HC_ud::solve(first_width_v);
    }

    vector<vector<rectangle>> line_to_ans(int w_sz,vector<int> &width,vector<int> &sum_width,vector<vector<vector<int>>> &w_si){
        vector<vector<rectangle>> ret(D,vector<rectangle>(N,{-1,-1,-1,-1}));
        rep(t,D){
            vector<int> area;
            vector<rectangle> rectangles;
            rep(i,w_sz){
                rep(j,(int)w_si[t][i].size()-1){
                    area.push_back((w_si[t][i][j+1]-w_si[t][i][j])*width[i]);
                    rectangles.emplace_back(w_si[t][i][j],sum_width[i],w_si[t][i][j+1],sum_width[i+1]);
                }
            }
            int sz=(int)area.size();
            vector<int> idx(sz);
            rep(i,sz) idx[i]=i;
            sort(all(idx),[&](int i,int j){return area[i]<area[j];});
            vector<int> used(sz,false);
            int now_idx=0;
            rep(i,N){
                while(now_idx<sz&&area[idx[now_idx]]<A[t][i]) now_idx++;
                if(now_idx!=sz){
                    ret[t][i]=rectangles[idx[now_idx]];
                    used[idx[now_idx]]=true;
                    now_idx++;
                }
            }
            now_idx=0;
            rep(i,N){
                if(ret[t][i].si!=-1) continue;
                while(now_idx<sz&&used[idx[now_idx]]) now_idx++;
                ret[t][i]=rectangles[idx[now_idx]];
                used[idx[now_idx]]=true;
                now_idx++;
            }
        }
        return ret;
    }
    
    
    ll fake_score(vector<vector<rectangle>> &ans,vector<int> &width,int w_sz,vector<vector<vector<int>>> &w_si){
        if((int)ans.size()!=D) return INF_ll;
        ll ret=0;
        rep(t,D){
            if((int)ans[t].size()!=N) return INF_ll;
            rep(i,N){
                ret+=max(0ll,100ll*(A[t][i]-(ans[t][i].gi-ans[t][i].si)*(ans[t][i].gj-ans[t][i].sj)));
            }
        }
        for(int t=1;t<D;t++){
            vector<int> area;
            vector<rectangle> rectangles;
            rep(i,w_sz){
                int cover_cnt=0;
                rep(j,(int)w_si[t][i].size()){
                    int min_dist=INF;
                    rep(k,(int)w_si[t-1][i].size()){
                        chmin(min_dist,abs(w_si[t][i][j]-w_si[t-1][i][k]));
                    }
                    if(min_dist==0) cover_cnt++;
                }
                ret+=width[i]*((int)w_si[t-1][i].size()+(int)w_si[t][i].size()-cover_cnt*2);
            }
        }
        return ret+1;
    }
    

    namespace SA_lr{
        int w_sz;
        vector<int> width,sum_width;
        vector<vector<vector<int>>> w_si;
        vector<int> can_improve,can_improve_set;

        namespace line_to_ans_diff{
            vector<rectangle> ret;
            vector<int> area;
            vector<rectangle> rectangles;
            vector<int> idx;
            auto calc=[](int t)->vector<rectangle>{
                ret.assign(N,{-1,-1,-1,-1});
                area.clear();
                rectangles.clear();
                
                rep(i,w_sz){
                    rep(j,(int)w_si[t][i].size()-1){
                        area.push_back((w_si[t][i][j+1]-w_si[t][i][j])*width[i]);
                        rectangles.emplace_back(w_si[t][i][j],sum_width[i],w_si[t][i][j+1],sum_width[i+1]);
                    }
                }
                int sz=(int)area.size();
                idx.resize(sz);
                rep(i,sz) idx[i]=i;
                sort(all(idx),[&](int i,int j){return area[i]<area[j];});
                vector<int> used(sz,false);
                int now_idx=0;
                int cnt_over=0;
                rep(i,N){
                    while(now_idx<sz&&area[idx[now_idx]]<A[t][i]) now_idx++;
                    if(now_idx!=sz){
                        ret[i]=rectangles[idx[now_idx]];
                        used[idx[now_idx]]=true;
                        now_idx++;
                    }else{
                        cnt_over++;
                    }
                }
                if(cnt_over==0) return ret;
                else return {};
                now_idx=0;
            };
        }
        auto fake_score_diff=[](int t,int i,int si){  //siは値を渡す！
            ll ret=0;
            if(0<=t-1){
                auto itr=lower_bound(all(w_si[t-1][i]),si);
                if(si==(*itr)) ret-=width[i];
                else ret+=width[i];
            }
            if(t+1<D){
                auto itr=lower_bound(all(w_si[t+1][i]),si);
                if(si==(*itr)) ret-=width[i];
                else ret+=width[i];
            }
            return ret;
        };

        vector<vector<rectangle>> now_ans,min_score_ans;
        ll pre_score=INF_ll,min_score=INF_ll;
        vector<int> great,ok,miss;

        auto solution_update=[](int modify_idx,ll new_score,int t){
            auto now=chrono::system_clock::now();
            int ms=chrono::duration_cast<chrono::milliseconds>(now-program_start).count();
            double temp=40.0+(0.1-40.0)*(double)ms/2700;
            double prob=exp((double)(pre_score-new_score)/temp);
            if(chmin(min_score,new_score)) min_score_ans=now_ans;
            if(rand_double()<prob){
                if(pre_score<=new_score) ok[modify_idx]++;
                else great[modify_idx]++;
                pre_score=new_score;
                /*if(t==-1){
                    can_improve.assign(D,0);
                    rep(nt,D){
                        if(0<=t-1){
                            if(w_si[nt]!=w_si[nt-1]) can_improve[nt]=1;
                        }
                        if(!can_improve[t]&&t+1<D){
                            if(w_si[nt]!=w_si[nt+1]) can_improve[nt]=1;
                        }
                    }
                }else{
                    for(int nt=max(0,t-1);nt<=min(D-1,t+1);nt++){
                        can_improve[nt]=0;
                        if(0<=nt-1){
                            if(now_ans[nt]!=now_ans[nt-1]) can_improve[nt]=1;
                        }
                        if(!can_improve[nt]&&nt+1<D){
                            if(now_ans[nt]!=now_ans[nt+1]) can_improve[nt]=1;
                        }
                    }
                }
                can_improve_set.clear();
                rep(nt,D){
                    if(can_improve[nt]) can_improve_set.push_back(nt);
                }*/

                return true;
            }else{
                miss[modify_idx]++;
                return false;
            }
        };

        namespace modify_a{
            vector<int> pre_si;
            unordered_set<int> st;
            vector<rectangle> pre_ans;
            auto calc=[](int modify_idx,int t,int i,int j){
                int d=rand_int(2);
                if(d==0) d=-1;
                if(t==0) d=1;
                if(t==D-1) d=-1;
                pre_si=w_si[t][i];
                st.clear();
                for(auto k:w_si[t][i]) st.insert(k);
                vector<int> can_si;
                for(auto k:w_si[t+d][i]){
                    if(st.find(k)==st.end()) can_si.push_back(k);
                }
                if(can_si.empty()) return;
                ll pre_score_diff=fake_score_diff(t,i,w_si[t][i][j]);
                int new_si=can_si[rand_int((int)can_si.size())];
                w_si[t][i][j]=new_si;
                sort(all(w_si[t][i]));
                pre_ans=now_ans[t];
                now_ans[t]=line_to_ans_diff::calc(t);
                if(now_ans[t].empty()){
                    swap(w_si[t][i],pre_si);
                    
                    swap(now_ans[t],pre_ans);
                    return;
                }
                ll new_score_diff=fake_score_diff(t,i,new_si);
                ll new_score=pre_score-pre_score_diff+new_score_diff;
                bool res=solution_update(modify_idx,new_score,t);
                if(!res){
                    swap(w_si[t][i],pre_si);
                    
                    swap(now_ans[t],pre_ans);
                }
            };
        };

        namespace modify_b{
            vector<rectangle> pre_ans;
            auto calc=[](int modify_idx,int t,int i,int j){
                int ni=rand_int(w_sz),nsi=1+rand_int(W-1);
                bool can_push=true;
                for(auto si:w_si[t][ni]){
                    if(si==nsi){
                        can_push=false;
                        break;
                    }
                }
                if(!can_push) return;

                ll pre_score_diff=fake_score_diff(t,i,w_si[t][i][j]);

                int pre_si=w_si[t][i][j];
                w_si[t][i].erase(w_si[t][i].begin()+j);
                w_si[t][ni].push_back(nsi);
                sort(all(w_si[t][ni]));

                pre_ans=now_ans[t];

                

                now_ans[t]=line_to_ans_diff::calc(t);

                if(now_ans[t].empty()){
                    int idx=0;
                    for(auto k:w_si[t][ni]){
                        if(k==nsi) break;
                        idx++;
                    }
                    
                    w_si[t][ni].erase(w_si[t][ni].begin()+idx);
                    w_si[t][i].push_back(pre_si);
                    sort(all(w_si[t][i]));

                    swap(now_ans[t],pre_ans);
                    return;
                }

                ll new_score_diff=fake_score_diff(t,ni,nsi);
                ll new_score=pre_score-pre_score_diff+new_score_diff;

                bool res=solution_update(modify_idx,new_score,t);
                if(!res){
                    int idx=0;
                    for(auto k:w_si[t][ni]){
                        if(k==nsi) break;
                        idx++;
                    }
                    w_si[t][ni].erase(w_si[t][ni].begin()+idx);
                    w_si[t][i].push_back(pre_si);
                    sort(all(w_si[t][i]));

                    swap(now_ans[t],pre_ans);
                }
            };
        }

        namespace modify_c{
            vector<rectangle> pre_ans;
            auto calc=[](int modify_idx,int t,int i){
                int i2=rand_int(w_sz);
                if(i==i2) return;
                ll pre_score_diff=0;
                for(auto si:w_si[t][i]) pre_score_diff+=fake_score_diff(t,i,si);
                for(auto si:w_si[t][i2]) pre_score_diff+=fake_score_diff(t,i2,si);

                swap(w_si[t][i],w_si[t][i2]);
                
                
                if(width[i]!=width[i2]){
                    pre_ans=now_ans[t];
                    if(pre_ans.empty()) return;
                    now_ans[t]=line_to_ans_diff::calc(t);
                    if(now_ans[t].empty()){
                        swap(w_si[t][i],w_si[t][i2]);

                        swap(now_ans[t],pre_ans);
                        return;
                    }
                }
                ll new_score_diff=0;
                for(auto si:w_si[t][i]) new_score_diff+=fake_score_diff(t,i,si);
                for(auto si:w_si[t][i2]) new_score_diff+=fake_score_diff(t,i2,si);

                ll new_score=pre_score-pre_score_diff+new_score_diff;
                bool res=solution_update(modify_idx,new_score,t);
                if(!res){
                    swap(w_si[t][i],w_si[t][i2]);

                    if(width[i]!=width[i2]) swap(now_ans[t],pre_ans);
                }
            };
        }


        namespace modify_d{
            vector<rectangle> pre_ans;
            auto calc=[](int modify_idx,int t,int i){
                int d=rand_int(2);
                if(d==0) d=-1;
                if(t==0) d=1;
                if(t==D-1) d=-1;
                int l=rand_int((int)w_si[t+d][i].size()),r=rand_int((int)w_si[t+d][i].size());
                if(r<l) swap(l,r);
                if(l==r||l==0||r==(int)w_si[t+d][i].size()-1) return;
                if((int)w_si[t][i].size()-(r-l+1)-1<=0) return;
                int s=rand_int((int)w_si[t][i].size()-(r-l+1)-1)+1;
                if(w_si[t+d][i][l]<=w_si[t][i][s-1]||w_si[t][i][s+r-l+1]<=w_si[t+d][i][r]) return;
                vector<int> pre_w_si=w_si[t][i];

                ll pre_score_diff=0;
                for(int k=l;k<=r;k++){
                    pre_score_diff+=fake_score_diff(t,i,w_si[t][i][s+k-l]);
                }
                for(int k=l;k<=r;k++){
                    w_si[t][i][s+k-l]=w_si[t+d][i][k];
                }
                
                pre_ans=now_ans[t];
                now_ans[t]=line_to_ans_diff::calc(t);
                if(now_ans[t].empty()){
                    for(int k=l;k<=r;k++) w_si[t][i][s+k-l]=pre_w_si[s+k-l];

                    swap(now_ans[t],pre_ans);
                    return;
                }

                ll new_score_diff=0;
                for(int k=l;k<=r;k++){
                    new_score_diff+=fake_score_diff(t,i,w_si[t][i][s+k-l]);
                }

                ll new_score=pre_score-pre_score_diff+new_score_diff;
                bool res=solution_update(modify_idx,new_score,t);
                if(!res){
                    for(int k=l;k<=r;k++) w_si[t][i][s+k-l]=pre_w_si[s+k-l];

                    swap(now_ans[t],pre_ans);
                }
            };
        }

        namespace modify_e{
            vector<rectangle> pre_ans;
            unordered_set<int> st;
            auto calc=[](int modify_idx,int t,int i,int j){
                int ni=rand_int(w_sz);
                st.clear();
                for(auto si:w_si[t][ni]) st.insert(si);
                vector<int> can_move;
                if(0<=t-1){
                    for(auto si:w_si[t-1][ni]){
                        if(st.find(si)==st.end()) can_move.push_back(si);
                    }
                }
                if(t+1<D){
                    for(auto si:w_si[t+1][ni]){
                        if(st.find(si)==st.end()) can_move.push_back(si);
                    }
                }
                if(can_move.empty()) return;

                ll pre_score_diff=fake_score_diff(t,i,w_si[t][i][j]);


                int nsi=can_move[rand_int((int)can_move.size())];
                int pre_si=w_si[t][i][j];
                w_si[t][i].erase(w_si[t][i].begin()+j);
                w_si[t][ni].push_back(nsi);
                sort(all(w_si[t][ni]));

                pre_ans=now_ans[t];
                now_ans[t]=line_to_ans_diff::calc(t);
                if(now_ans[t].empty()){
                    int idx=0;
                    for(auto k:w_si[t][ni]){
                        if(k==nsi) break;
                        idx++;
                    }
                    
                    w_si[t][ni].erase(w_si[t][ni].begin()+idx);
                    w_si[t][i].push_back(pre_si);
                    sort(all(w_si[t][i]));

                    swap(now_ans[t],pre_ans);
                    return;
                }

                ll new_score_diff=fake_score_diff(t,ni,nsi);

                ll new_score=pre_score-pre_score_diff+new_score_diff;


                bool res=solution_update(modify_idx,new_score,t);
                if(!res){
                    int idx=0;
                    for(auto k:w_si[t][ni]){
                        if(k==nsi) break;
                        idx++;
                    }
                    w_si[t][ni].erase(w_si[t][ni].begin()+idx);
                    w_si[t][i].push_back(pre_si);
                    sort(all(w_si[t][i]));

                    swap(now_ans[t],pre_ans);
                }
            };
        }

        namespace modify_f{
            vector<int> pre_w_si_i;
            vector<rectangle> pre_ans;
            auto calc=[](int modify_idx,int t,int i){
                pre_w_si_i=w_si[t][i];
                vector<int> h;
                rep(j,(int)w_si[t][i].size()-1){
                    h.push_back(w_si[t][i][j+1]-w_si[t][i][j]);
                }
                int sz=h.size();
                int l=rand_int(sz+1),r=rand_int(sz+1);
                if(r<l) swap(l,r);
                if(r-l<=1) return;

                ll pre_score_diff=0;
                for(int k=l;k<r;k++){
                    pre_score_diff+=fake_score_diff(t,i,w_si[t][i][k]);
                }

                shuffle(h.begin()+l,h.begin()+r,mt);

                w_si[t][i].assign(sz,0);
                int sum_j=0;
                rep(j,sz){
                    w_si[t][i][j]=sum_j;
                    sum_j+=h[j];
                }
                w_si[t][i].push_back(W);

                pre_ans=now_ans[t];
                now_ans[t]=line_to_ans_diff::calc(t);
                if(now_ans[t].empty()){
                    swap(w_si[t][i],pre_w_si_i);
                    swap(now_ans[t],pre_ans);
                    return;
                }

                ll new_score_diff=0;
                for(int k=l;k<r;k++){
                    new_score_diff+=fake_score_diff(t,i,w_si[t][i][k]);
                }

                ll new_score=pre_score-pre_score_diff+new_score_diff;
                bool res=solution_update(modify_idx,new_score,t);
                if(!res){
                    swap(w_si[t][i],pre_w_si_i);
                    swap(now_ans[t],pre_ans);
                }
            };
        }




        vector<vector<rectangle>> solve(int w_sz_,vector<int> width_,vector<int> sum_width_,vector<vector<vector<int>>> w_si_){

            w_sz=w_sz_;
            width=width_;
            sum_width=sum_width_;
            w_si=w_si_;


            min_score_ans=now_ans=line_to_ans(w_sz,width,sum_width,w_si);
            min_score=pre_score=fake_score(now_ans,width,w_sz,w_si);

            can_improve.assign(D,0);

            rep(t,D){
                if(0<=t-1){
                    if(w_si[t]!=w_si[t-1]) can_improve[t]=1;
                }
                if(!can_improve[t]&&t+1<D){
                    if(w_si[t]!=w_si[t+1]) can_improve[t]=1;
                }
                if(can_improve[t]){
                    can_improve_set.push_back(t);
                }
            }
            

            int modify_type=MA+MB+MC+MD+ME+MF;;

            great.assign(modify_type,0);
            ok.assign(modify_type,0);
            miss.assign(modify_type,0);
            int cnt_loop=0;
            while(true){
                //cerr << pre_score << endl;
                cnt_loop++;
                auto now=chrono::system_clock::now();
                int ms=chrono::duration_cast<chrono::milliseconds>(now-program_start).count();
                if(2700<ms) break;
                //if(can_improve_set.empty()) break;
                int t=rand_int(D);
                int i=rand_int(w_sz);
                int modify_idx=rand_int(modify_type);
                if(modify_idx<MA){
                    int j=rand_int((int)w_si[t][i].size());
                    if(w_si[t][i][j]==0||w_si[t][i][j]==W) continue;
                    modify_a::calc(0,t,i,j);
                }else if(modify_idx<MA+MB){
                    int j=rand_int((int)w_si[t][i].size());
                    if(w_si[t][i][j]==0||w_si[t][i][j]==W) continue;
                    modify_b::calc(1,t,i,j);
                }else if(modify_idx<MA+MB+MC){
                    modify_c::calc(2,t,i);
                }else if(modify_idx<MA+MB+MC+MD){
                    modify_d::calc(3,t,i);
                }else if(modify_idx<MA+MB+MC+MD+ME){
                    int j=rand_int((int)w_si[t][i].size());
                    if(w_si[t][i][j]==0||w_si[t][i][j]==W) continue;
                    modify_e::calc(4,t,i,j);
                }else{
                    modify_f::calc(5,t,i);
                }
            }
            /*cerr << cnt_loop << endl;
            int sum=0;
            rep(i,modify_type){
                cerr << great[i] << " " << ok[i] << " " << miss[i] << endl;
                sum+=great[i]+ok[i]+miss[i];
            }
            cerr << sum << " " << pre_score << endl;*/
            return min_score_ans;
        }
    };


    vector<vector<rectangle>> solve_abcd(vector<vector<rectangle>> first_ans){
        if(first_ans.empty()) return {};
        set<int> set_sum_width;
        rep(t,D){
            rep(i,N){
                set_sum_width.insert(first_ans[t][i].sj);
            }
        }
        vector<int> sum_width;  //Wも入っている
        for(auto i:set_sum_width) sum_width.push_back(i);
        sum_width.push_back(W);
        int w_sz=(int)sum_width.size()-1;
        vector<int> width(w_sz);
        rep(i,w_sz) width[i]=sum_width[i+1]-sum_width[i];
        vector<vector<vector<int>>> w_si(D,vector<vector<int>>(w_sz));  //先頭は0、末尾はW、中身はソートされている
        rep(t,D){
            rep(i,N){
                rep(j,w_sz){
                    if(first_ans[t][i].sj==sum_width[j]){
                        w_si[t][j].push_back(first_ans[t][i].si);
                    }
                }
            }
            rep(i,w_sz){
                sort(all(w_si[t][i]));
                if(w_si[t][i].empty()) w_si[t][i].push_back(0);
                w_si[t][i].push_back(W);
            }
        }
        
        return SA_lr::solve(w_sz,width,sum_width,w_si);
    }
}

namespace Solver2{
    int to(int i,int j){
        return i*W+j;
    }
    vector<bitset<1000>> pre_ud_line(1000),pre_lr_line(1000),new_ud_line(1000),new_lr_line(1000);
    vector<bitset<1000>> seg_mask(1001);

    vector<vector<vector<int>>> seg_max_a;  //[l][r][i]=max(A_l_i,A_l+1_i,…,A_r-1_i)

    void init(){
        string s(1000,'0');
        rep(i,1001){
            bitset<1000> now(s);
            seg_mask[i]=now;
            if(i==1000) break;
            s[i]='1';
        }

        seg_max_a.assign(D+1,vector<vector<int>>(D+1,vector<int>(N,0)));   
        rep(l,D+1){
            for(int r=l;r<D;r++){
                rep(i,N){
                    seg_max_a[l][r+1][i]=max(seg_max_a[l][r][i],A[r][i]);
                }
            }
        }
    }
    ll diff_score(vector<vector<rectangle>> ans){  //枠のズレのコストだけ計算
        int sz=(int)ans.size();
        ll ret=0;
        rep(t,sz){
            if((int)ans[t].size()!=N) return INF_ll;
            rep(i,N){
                if(ans[t][i].si<0||W<ans[t][i].si) return INF_ll;
                if(ans[t][i].sj<0||W<ans[t][i].sj) return INF_ll;
                if(ans[t][i].gi<0||W<ans[t][i].gi) return INF_ll;
                if(ans[t][i].gj<0||W<ans[t][i].gj) return INF_ll;
                if(ans[t][i].gi<=ans[t][i].si) return INF_ll;
                if(ans[t][i].gj<=ans[t][i].sj) return INF_ll;
                rep(j,i){
                    if(max(ans[t][i].si,ans[t][j].si)<min(ans[t][i].gi,ans[t][j].gi)&&max(ans[t][i].sj,ans[t][j].sj)<min(ans[t][i].gj,ans[t][j].gj)) return INF_ll;
                }
            }
        }
        
        //lr,udは直線の向き
        vector<set<int>> ud_set(sz),lr_set(sz),seg_ud_set(sz),seg_lr_set(sz);
        rep(t,sz){
            rep(i,N){
                if(0<ans[t][i].sj) ud_set[t].insert(ans[t][i].sj);
                if(ans[t][i].gj<W) ud_set[t].insert(ans[t][i].gj);
                if(0<ans[t][i].si) lr_set[t].insert(ans[t][i].si);
                if(ans[t][i].gi<W) lr_set[t].insert(ans[t][i].gi);
            }
            if(t==0) continue;
            seg_ud_set[t].insert(all(ud_set[t]));
            seg_ud_set[t].insert(all(ud_set[t-1]));
            seg_lr_set[t].insert(all(lr_set[t]));
            seg_lr_set[t].insert(all(lr_set[t-1]));
        }
        rep(t,sz){
            rep(i,N){
                if(0<ans[t][i].sj) new_ud_line[ans[t][i].sj]|=(seg_mask[ans[t][i].gi-ans[t][i].si]>>ans[t][i].si);
                if(ans[t][i].gj<W) new_ud_line[ans[t][i].gj]|=(seg_mask[ans[t][i].gi-ans[t][i].si]>>ans[t][i].si);
                if(0<ans[t][i].si) new_lr_line[ans[t][i].si]|=(seg_mask[ans[t][i].gj-ans[t][i].sj]>>ans[t][i].sj);
                if(ans[t][i].gi<W) new_lr_line[ans[t][i].gi]|=(seg_mask[ans[t][i].gj-ans[t][i].sj]>>ans[t][i].sj);
            }
            if(t!=0){
                for(auto i:seg_ud_set[t]) ret+=(pre_ud_line[i]^new_ud_line[i]).count();
                for(auto i:seg_lr_set[t]){
                    ret+=(pre_lr_line[i]^new_lr_line[i]).count();
                }
            }
            swap(pre_ud_line,new_ud_line);
            swap(pre_lr_line,new_lr_line);
            if(t!=0){
                for(auto i:ud_set[t-1]) new_ud_line[i].reset();
                for(auto i:lr_set[t-1]) new_lr_line[i].reset();
                if(t==sz-1){
                    for(auto i:ud_set[t]) pre_ud_line[i].reset();
                    for(auto i:lr_set[t]) pre_lr_line[i].reset();
                }
            }
        }
        return ret+1;
    }
    ll score(vector<vector<rectangle>> ans){  //頑張って計算をするかセグメント木をうまく使うかでもっと高速になりそう
        if((int)ans.size()!=D) return INF_ll;
        ll ret=0;
        rep(t,D){
            if((int)ans[t].size()!=N) return INF_ll;
            rep(i,N){
                if(ans[t][i].si<0||W<ans[t][i].si) return INF_ll;
                if(ans[t][i].sj<0||W<ans[t][i].sj) return INF_ll;
                if(ans[t][i].gi<0||W<ans[t][i].gi) return INF_ll;
                if(ans[t][i].gj<0||W<ans[t][i].gj) return INF_ll;
                if(ans[t][i].gi<=ans[t][i].si) return INF_ll;
                if(ans[t][i].gj<=ans[t][i].sj) return INF_ll;
                rep(j,i){
                    if(max(ans[t][i].si,ans[t][j].si)<min(ans[t][i].gi,ans[t][j].gi)&&max(ans[t][i].sj,ans[t][j].sj)<min(ans[t][i].gj,ans[t][j].gj)) return INF_ll;
                }
                ret+=max(0ll,100ll*(A[t][i]-(ans[t][i].gi-ans[t][i].si)*(ans[t][i].gj-ans[t][i].sj)));
            }
        }
        //lr,udは直線の向き
        vector<set<int>> ud_set(D),lr_set(D),seg_ud_set(D),seg_lr_set(D);
        rep(t,D){
            rep(i,N){
                if(0<ans[t][i].sj) ud_set[t].insert(ans[t][i].sj);
                if(ans[t][i].gj<W) ud_set[t].insert(ans[t][i].gj);
                if(0<ans[t][i].si) lr_set[t].insert(ans[t][i].si);
                if(ans[t][i].gi<W) lr_set[t].insert(ans[t][i].gi);
            }
            if(t==0) continue;
            seg_ud_set[t].insert(all(ud_set[t]));
            seg_ud_set[t].insert(all(ud_set[t-1]));
            seg_lr_set[t].insert(all(lr_set[t]));
            seg_lr_set[t].insert(all(lr_set[t-1]));
        }
        rep(t,D){
            rep(i,N){
                if(0<ans[t][i].sj) new_ud_line[ans[t][i].sj]|=(seg_mask[ans[t][i].gi-ans[t][i].si]>>ans[t][i].si);
                if(ans[t][i].gj<W) new_ud_line[ans[t][i].gj]|=(seg_mask[ans[t][i].gi-ans[t][i].si]>>ans[t][i].si);
                if(0<ans[t][i].si) new_lr_line[ans[t][i].si]|=(seg_mask[ans[t][i].gj-ans[t][i].sj]>>ans[t][i].sj);
                if(ans[t][i].gi<W) new_lr_line[ans[t][i].gi]|=(seg_mask[ans[t][i].gj-ans[t][i].sj]>>ans[t][i].sj);
            }
            if(t!=0){
                for(auto i:seg_ud_set[t]) ret+=(pre_ud_line[i]^new_ud_line[i]).count();
                for(auto i:seg_lr_set[t]){
                    ret+=(pre_lr_line[i]^new_lr_line[i]).count();
                }
            }
            swap(pre_ud_line,new_ud_line);
            swap(pre_lr_line,new_lr_line);
            if(t!=0){
                for(auto i:ud_set[t-1]) new_ud_line[i].reset();
                for(auto i:lr_set[t-1]) new_lr_line[i].reset();
                if(t==D-1){
                    for(auto i:ud_set[t]) pre_ud_line[i].reset();
                    for(auto i:lr_set[t]) pre_lr_line[i].reset();
                }
            }
            
        }
        return ret+1;
    }

    ll score_only_area_t(vector<rectangle> &ans,vector<int> &a){
        ll ret=0;
        if((int)ans.size()!=N) return INF_ll;
        if((int)a.size()!=N) return INF_ll;
        rep(i,N){
            ret+=max(0ll,100ll*(a[i]-(ans[i].gi-ans[i].si)*(ans[i].gj-ans[i].sj)));
        }
        return ret+1;
    }

    vector<rectangle> push_width_x(vector<int> &width,vector<int> &a){
        vector<rectangle> ret(N);
        int sz=(int)width.size();
        vector<int> ord(sz);
        rep(i,sz) ord[i]=i;
        sort(all(ord),[&](int i,int j){return width[i]>width[j];});
        vector<int> nsi(sz,0);
        vector<int> nsj(sz,0);
        rep(i,sz-1) nsj[i+1]=nsj[i]+width[i];
        vector<int> last_push(sz,-1);
        for(int i=N-1;0<=i;i--){
            int mn=INF,mn2=INF;
            for(auto j:ord){
                int d=(a[i]+width[j]-1)/width[j];
                if(nsi[j]==W) continue;
                if(W<nsi[j]+d) chmin(mn2,j);
                else chmin(mn,j);
            }
            int j=mn;
            if(j==INF) j=mn2;
            if(j==INF) return {};
            int d=(a[i]+width[j]-1)/width[j];
            ret[i]={nsi[j],nsj[j],min(W,nsi[j]+d),nsj[j]+width[j]};
            nsi[j]=min(W,nsi[j]+d);
            last_push[j]=i;
        }
        rep(i,sz){
            if(last_push[i]==-1) continue;
            ret[last_push[i]].gi=W;
        }
        return ret;
    }
    vector<rectangle> push_width_y(vector<int> &width,vector<int> &a){
        vector<rectangle> ret(N);
        int sz=(int)width.size();
        vector<int> nsi(sz,0);
        vector<int> nsj(sz,0);
        rep(i,sz-1) nsj[i+1]=nsj[i]+width[i];
        vector<int> last_push(sz,-1);
        for(int i=N-1;0<=i;i--){
            pii mn={INF,INF};
            pii mx={-INF,-INF};
            rep(j,sz){
                int d=(a[i]+width[j]-1)/width[j];
                if(nsi[j]==W) continue;
                if(W<nsi[j]+d) chmax(mx,{(W-nsi[j])*width[j],j});
                else chmin(mn,{(W-nsi[j])*width[j],j});
            }
            int j=mn.second;
            if(j==INF) j=mx.second;
            if(j==-INF) return {};
            int d=(a[i]+width[j]-1)/width[j];
            ret[i]={nsi[j],nsj[j],min(W,nsi[j]+d),nsj[j]+width[j]};
            nsi[j]=min(W,nsi[j]+d);
            last_push[j]=i;
        }
        rep(i,sz){
            if(last_push[i]==-1) continue;
            ret[last_push[i]].gi=W;
        }
        return ret;
    }
    vector<rectangle> push_width_z(vector<int> &width,vector<int> &a){
        vector<rectangle> ret(N);
        int sz=(int)width.size();
        vector<int> nsi(sz,0);
        vector<int> nsj(sz,0);
        rep(i,sz-1) nsj[i+1]=nsj[i]+width[i];
        vector<int> last_push(sz,-1);
        for(int i=N-1;0<=i;i--){
            pii mx={-INF,-INF},mx2={-INF,-INF};
            rep(j,sz){
                int d=(a[i]+width[j]-1)/width[j];
                if(nsi[j]==W) continue;
                if(W<nsi[j]+d) chmax(mx2,{(W-nsi[j])*width[j],j});
                else chmax(mx,{(W-nsi[j])*width[j],j});
            }
            int j=mx.second;
            if(j==-INF) j=mx2.second;
            if(j==-INF) return {};
            int d=(a[i]+width[j]-1)/width[j];
            ret[i]={nsi[j],nsj[j],min(W,nsi[j]+d),nsj[j]+width[j]};
            nsi[j]=min(W,nsi[j]+d);
            last_push[j]=i;
        }
        rep(i,sz){
            if(last_push[i]==-1) continue;
            ret[last_push[i]].gi=W;
        }
        return ret;
    }
    vector<rectangle> push_width_a(vector<int> &width,vector<int> &a){
        vector<rectangle> ret(N);
        int sz=(int)width.size();
        vector<int> nsi(sz,0);
        vector<int> nsj(sz,0);
        rep(i,sz-1) nsj[i+1]=nsj[i]+width[i];
        vector<int> last_push(sz,-1);
        for(int i=N-1;0<=i;i--){
            pii mn={INF,INF},mn2={INF,INF};
            int zero_idx=INF;
            rep(j,sz){
                int d=(a[i]+width[j]-1)/width[j];
                if(W<nsi[j]+d){
                    if(nsi[j]!=W) chmin(mn2,{nsj[j],j});
                    continue;
                }
                if(nsi[j]==0) chmin(zero_idx,j);
                else chmin(mn,{nsi[j],j});
            }
            int j=mn.second;
            if(j==INF) j=zero_idx;
            if(j==INF){
                j=mn2.second;
                if(j==INF) return {};
                ret[i]={nsi[j],nsj[j],W,nsj[j]+width[j]};
                nsi[j]=W;
            }else{
                int d=(a[i]+width[j]-1)/width[j];
                ret[i]={nsi[j],nsj[j],nsi[j]+d,nsj[j]+width[j]};
                nsi[j]+=d;
            }
            last_push[j]=i;
        }
        rep(i,sz){
            if(last_push[i]==-1) continue;
            ret[last_push[i]].gi=W;
        }
        return ret;
    }

    vector<rectangle> push_width(vector<int> &width,vector<int> &a){
        vector<rectangle> ret;
        ll min_score=INF_ll+1ll;
        auto v={push_width_x,push_width_y,push_width_z,push_width_a};
        for(auto f:v){
            vector<rectangle> now=f(width,a);
            ll now_score=score_only_area_t(now,a);
            if(chmin(min_score,now_score)) ret=now;
        }
        return ret;
    }

    vector<vector<rectangle>> width_to_ans(vector<int> &width){
        vector<vector<rectangle>> ret(D);
        int l=0,r=0;
        while(l<D){
            while(r<D&&!push_width(width,seg_max_a[l][r+1]).empty()) r++;
            if(l==r) return {};
            vector<rectangle> now=push_width(width,seg_max_a[l][r]);
            while(l<r){
                ret[l]=now;
                l++;
            }
        }
        return ret;
    }
    

    namespace HC_ud{
        auto score_calc=[](vector<int> &width){
            vector<vector<rectangle>> ans=width_to_ans(width);
            ll true_score=score(ans);
            return true_score;
        };
        
        constexpr int modify_type=3;
        vector<int> great,ok,miss;
        double pre_score=1e36;

        auto solution_update=[](int modify_idx,double new_score,int &update_k){
            if(new_score<=pre_score){
                
                if(pre_score!=new_score) update_k=0;
                if(pre_score<=new_score) ok[modify_idx]++;
                else great[modify_idx]++;
                pre_score=new_score;

                return true;
            }else{
                miss[modify_idx]++;
                return false;
            }
        };

        vector<vector<rectangle>> solve(vector<vector<int>> first_width_v){
            int v_sz=(int)first_width_v.size(),now_idx=0;
            double min_score=score_calc(first_width_v[now_idx]);
            vector<int> min_score_width=first_width_v[now_idx];
            
            great.assign(modify_type,0),ok.assign(modify_type,0),miss.assign(modify_type,0);
            while(true){
                auto now=chrono::system_clock::now();
                int ms=chrono::duration_cast<chrono::milliseconds>(now-program_start).count();
                if(1000<ms) break;
                vector<int> width=first_width_v[now_idx];
                int w_sz=(int)width.size();
                if(w_sz==1) return width_to_ans(width);

                pre_score=score_calc(width);
                int cnt_loop=0,update_k=0;
                while(true){
                    
                    
                    cnt_loop++;
                    update_k++;
                    if(1000<=update_k) break;
                    auto now2=chrono::system_clock::now();
                    int ms2=chrono::duration_cast<chrono::milliseconds>(now2-program_start).count();
                    if(1000<ms2) break;
                    int r=rand_int(modify_type);
                    if(r==0){
                        int x=rand_int(w_sz),y=rand_int(w_sz);
                        if(min(width[x],width[y])<=1) continue;
                        int p=rand_int(1,min(width[x],width[y]));
                        if(W<=width[x]+p||width[y]-p<=0) continue;
                        width[x]+=p;
                        width[y]-=p;
                        double new_score=score_calc(width);
                        bool res=solution_update(0,new_score,update_k);
                        if(!res){
                            width[x]-=p;
                            width[y]+=p;
                        }
                    }else if(r==1){
                        int x=rand_int(w_sz);
                        if(width[x]<=1) continue;
                        vector<int> new_width=width;
                        int new_push=rand_int(1,new_width[x]);
                        new_width[x]-=new_push;
                        new_width.push_back(new_push);
                        double new_score=score_calc(new_width);
                        bool res=solution_update(1,new_score,update_k);
                        if(res){
                            width=new_width;
                            w_sz=(int)width.size();
                        }
                    }else{
                        int x=rand_int(w_sz),y=rand_int(w_sz);
                        vector<int> new_width;
                        rep(i,w_sz){
                            if(i==x||i==y) continue;
                            new_width.push_back(width[i]);
                        }
                        new_width.push_back(width[x]+width[y]);
                        double new_score=score_calc(new_width);
                        bool res=solution_update(2,new_score,update_k);
                        if(res){
                            width=new_width;
                            w_sz=(int)width.size();
                        }
                    }
                    
                }
                //cerr << pre_score << " " <<update_k << endl;
                if(chmin(min_score,pre_score)) min_score_width=width;
                now_idx++;
                if(v_sz<=now_idx) now_idx=0;
            }
            /*int sum=0;
            rep(i,modify_type){
                cerr << great[i] << " " << ok[i] << " " << miss[i] << endl;
                sum+=great[i]+ok[i]+miss[i];
            }
            cerr << fixed << setprecision(10) << sum  << "," << min_score << "," << score(width_to_ans(min_score_width)) << endl;*/
            vector<vector<rectangle>> ans=width_to_ans(min_score_width);
            ll area_cost=0;
            rep(t,D) area_cost+=score_only_area_t(ans[t],A[t]);
            if(area_cost==0){
                //Solverへ移行(危険！)
                Solver::init();
                Solver::solve_abcd(ans);
                exit(0);
            }
            return ans;
        }
    }

    vector<vector<rectangle>> solve_a(){
        vector<vector<int>> width(W+1);
        vector<int> cnt(W+1,0);
        for(int w=1;w<=W;w++){
            int sum=0;
            while(sum<W){
                if(sum+w<=W) width[w].push_back(w);
                else width[w].push_back(W-sum);
                sum+=w;
            }
            rep(t,D) if(!push_width(width[w],A[t]).empty()) cnt[w]++;
        }

        vector<ll> score_v;
        int cnt_charange=0;
        vector<vector<int>> width_v;
        for(int w=1;w<=W;w++){
            if(cnt[w]!=D) continue;
            /*cnt_charange++;
            if(10<=cnt_charange) break;*/

            auto now=chrono::system_clock::now();
            int ms=chrono::duration_cast<chrono::milliseconds>(now-program_start).count();
            if(2700<ms) break;
            vector<vector<rectangle>> now_ans=width_to_ans(width[w]);
            ll now_score=score(now_ans);
            score_v.push_back(now_score);
            width_v.push_back(width[w]);
        }
        if(width_v.empty()) return {};
        int sz=(int)width_v.size();
        vector<int> ord(sz);
        rep(i,sz) ord[i]=i;
        sort(all(ord),[&](int i,int j){return score_v[i]<score_v[j];});
        vector<vector<int>> first_width_v(min(10,sz));
        rep(i,min(10,sz)) first_width_v[i]=width_v[ord[i]];
        return HC_ud::solve(first_width_v);
    }
    
    vector<vector<rectangle>> solve_e(){
        vector<vector<int>> dp,pre;
        vector<int> div_a(N,0),div_b(N,0);
        vector<vector<vector<rectangle>>> ans_div(D,vector<vector<rectangle>>(W/2+1));
        vector<vector<int>> not_over(D,vector<int>(W/2+1,0));
        rep(t,D){
            rep(w,W/2+1){
                if(w%100==0){
                    auto now=chrono::system_clock::now();
                    int ms=chrono::duration_cast<chrono::milliseconds>(now-program_start).count();
                    if(2700<ms) break;
                }
                rep(i,N){
                    if(0<w) div_a[i]=(A[t][i]+w-1)/w;
                    div_b[i]=(A[t][i]+(W-w)-1)/(W-w);
                }
                dp.assign(N+1,vector<int>(W+1,INF));
                pre.assign(N+1,vector<int>(W+1,INF));
                dp[0][0]=0;
                rep(i,N){
                    rep(j,W+1){
                        if(0<w&&j+div_a[i]<=W){
                            if(chmin(dp[i+1][j+div_a[i]],dp[i][j])) pre[i+1][j+div_a[i]]=j;
                        }
                        if(chmin(dp[i+1][j],dp[i][j]+div_b[i])) pre[i+1][j]=j;
                    }
                }
                int mn=INF,nj=-1;
                rep(i,W+1){
                    if(chmin(mn,dp[N][i])) nj=i;
                }
                if(mn<=W) not_over[t][w]=true;
                vector<rectangle> now_ans(N);
                vector<int> group(N,-1);
                for(ll i=N;1<=i;i--){
                    if(nj==pre[i][nj]) group[i-1]=1;
                    else group[i-1]=0;
                    nj=pre[i][nj];
                }
                int last_0_idx=-1,last_1_idx=-1;
                rep(i,N){
                    if(group[i]==0) last_0_idx=i;
                    if(group[i]==1) last_1_idx=i;
                }
                int nsi=0;
                rep(i,N){
                    if(group[i]==1) continue;
                    now_ans[i]={nsi,0,nsi+div_a[i],w};
                    if(i==last_0_idx) now_ans[i]={nsi,0,W,w};
                    nsi+=div_a[i];
                }
                nsi=0;
                rep(i,N){
                    if(group[i]==0) continue;
                    now_ans[i]={nsi,w,nsi+div_b[i],W};
                    if(i==last_1_idx) now_ans[i]={nsi,w,W,W};
                    nsi+=div_b[i];
                }
                ans_div[t][w]=now_ans;
            }
        }
        vector<int> now_idx(D,0);
        vector<int> cnt(W/2+1,0);
        rep(i,D){
            rep(j,W/2+1){
                if(not_over[i][j]==0) continue;
                cnt[j]++;
                now_idx[i]=j;
            }
        }
        rep(i,W/2+1){
            if(cnt[i]==D){
                rep(j,D) now_idx[j]=i;
            }
        }
        
        auto back_ans=[&](){
            vector<vector<rectangle>> ret(D);
            rep(i,D){
                ret[i]=ans_div[i][now_idx[i]];
            }
            return ret;
        };
        ll pre_score=score(back_ans());
        while(true){
            auto now=chrono::system_clock::now();
            int ms=chrono::duration_cast<chrono::milliseconds>(now-program_start).count();
            if(2700<ms) break;
            int x=rand_int(D),y=rand_int(W/2+1);
            int p=now_idx[x];
            now_idx[x]=y;
            ll new_score=score(back_ans());
            if(new_score<=pre_score){
                pre_score=new_score;
            }else{
                now_idx[x]=p;
            }
        }
        vector<vector<rectangle>> ans=back_ans();
        return ans;
    }

    vector<vector<rectangle>> line_to_ans(int w_sz,vector<int> &width,vector<int> &sum_width,vector<vector<vector<int>>> &w_si){
        vector<vector<rectangle>> ret(D,vector<rectangle>(N,{-1,-1,-1,-1}));
        rep(t,D){
            vector<int> area;
            vector<rectangle> rectangles;
            rep(i,w_sz){
                rep(j,(int)w_si[t][i].size()-1){
                    area.push_back((w_si[t][i][j+1]-w_si[t][i][j])*width[i]);
                    rectangles.emplace_back(w_si[t][i][j],sum_width[i],w_si[t][i][j+1],sum_width[i+1]);
                }
            }
            int sz=(int)area.size();
            vector<int> idx(sz);
            rep(i,sz) idx[i]=i;
            sort(all(idx),[&](int i,int j){return area[i]<area[j];});
            rep(i,N){
                ret[t][N-1-i]=rectangles[idx[sz-1-i]];
            }
        }
        return ret;
    }
    
    ll fake_score(vector<vector<rectangle>> &ans,vector<int> &width,int w_sz,vector<vector<vector<int>>> &w_si){
        if((int)ans.size()!=D) return INF_ll;
        ll ret=0;
        rep(t,D){
            if((int)ans[t].size()!=N) return INF_ll;
            rep(i,N){
                ret+=max(0ll,100ll*(A[t][i]-(ans[t][i].gi-ans[t][i].si)*(ans[t][i].gj-ans[t][i].sj)));
            }
        }
        for(int t=1;t<D;t++){
            vector<int> area;
            vector<rectangle> rectangles;
            rep(i,w_sz){
                int cover_cnt=0;
                rep(j,(int)w_si[t][i].size()){
                    int min_dist=INF;
                    rep(k,(int)w_si[t-1][i].size()){
                        chmin(min_dist,abs(w_si[t][i][j]-w_si[t-1][i][k]));
                    }
                    if(min_dist==0) cover_cnt++;
                }
                ret+=width[i]*((int)w_si[t-1][i].size()+(int)w_si[t][i].size()-cover_cnt*2);
            }
        }
        return ret+1;
    }


    namespace HC{
        int w_sz;
        vector<int> width,sum_width;
        vector<vector<vector<int>>> w_si;
        constexpr int modify_type=7;
        vector<vector<rectangle>> now_ans;
        vector<ll> score_diff_list;
        ll pre_score=INF_ll;
        vector<int> great,ok,miss;

        namespace line_to_ans_diff{
            vector<rectangle> ret;
            vector<int> area;
            vector<rectangle> rectangles;
            vector<int> idx;
            vector<rectangle> calc(int t){
                ret.assign(N,{-1,-1,-1,-1});
                area.clear();
                rectangles.clear();
                
                rep(i,w_sz){
                    rep(j,(int)w_si[t][i].size()-1){
                        area.push_back((w_si[t][i][j+1]-w_si[t][i][j])*width[i]);
                        rectangles.emplace_back(w_si[t][i][j],sum_width[i],w_si[t][i][j+1],sum_width[i+1]);
                    }
                }
                int sz=(int)area.size();
                idx.resize(sz);
                rep(i,sz) idx[i]=i;
                sort(all(idx),[&](int i,int j){return area[i]<area[j];});
                rep(i,N){
                    ret[N-1-i]=rectangles[idx[sz-1-i]];
                }
                return ret;
            };
        }

        ll fake_score_diff(int t){
            if((int)now_ans.size()!=D) return INF_ll;
            ll ret=0;
            if((int)now_ans[t].size()!=N) return INF_ll;
            rep(i,N){
                ret+=max(0ll,100ll*(A[t][i]-(now_ans[t][i].gi-now_ans[t][i].si)*(now_ans[t][i].gj-now_ans[t][i].sj)));
            }
            if(t==0) return ret;
            rep(i,w_sz){
                int cover_cnt=0;
                for(int j=1;j<(int)w_si[t][i].size()-1;j++){
                    auto itr=lower_bound(all(w_si[t-1][i]),w_si[t][i][j]);
                    if(w_si[t][i][j]==(*itr)) cover_cnt++;
                }
                ret+=width[i]*((int)w_si[t-1][i].size()+(int)w_si[t][i].size()-cover_cnt*2);
            }
            return ret;
        };

        bool solution_update(int modify_idx,ll new_score){
            if(new_score<=pre_score){
                if(pre_score<=new_score) ok[modify_idx]++;
                else great[modify_idx]++;
                pre_score=new_score;
                return true;
            }else{
                miss[modify_idx]++;
                return false;
            }
        };

        namespace modify_a{
            vector<int> pre_si;
            unordered_set<int> st;
            vector<rectangle> pre_ans;
            void calc(int modify_idx,int t,int i,int j){
                int d=rand_int(2);
                if(d==0) d=-1;
                if(t==0) d=1;
                if(t==D-1) d=-1;
                pre_si=w_si[t][i];
                st.clear();
                for(auto k:w_si[t][i]) st.insert(k);
                vector<int> can_si;
                for(auto k:w_si[t+d][i]){
                    if(st.find(k)==st.end()) can_si.push_back(k);
                }
                if(can_si.empty()) return;
                w_si[t][i][j]=can_si[rand_int((int)can_si.size())];
                sort(all(w_si[t][i]));
                pre_ans=now_ans[t];
                now_ans[t]=line_to_ans_diff::calc(t);
                if(now_ans[t].empty()){
                    swap(w_si[t][i],pre_si);
                    
                    swap(now_ans[t],pre_ans);
                    return;
                }
                ll new_score_diff=fake_score_diff(t);
                ll new_score_diff2=0;
                if(t+1<D) new_score_diff2=fake_score_diff(t+1);
                ll new_score=pre_score-score_diff_list[t]+new_score_diff;
                if(t+1<D) new_score=new_score-score_diff_list[t+1]+new_score_diff2;
                bool res=solution_update(modify_idx,new_score);
                if(res){
                    score_diff_list[t]=new_score_diff;
                    if(t+1<D) score_diff_list[t+1]=new_score_diff2;
                }else{
                    swap(w_si[t][i],pre_si);
                    
                    swap(now_ans[t],pre_ans);
                }
            };
        };
        namespace modify_b{
            vector<rectangle> pre_ans;
            void calc(int modify_idx,int t,int i,int j){
                int ni=rand_int(w_sz),nsi=1+rand_int(W-1);
                bool can_push=true;
                for(auto si:w_si[t][ni]){
                    if(si==nsi){
                        can_push=false;
                        break;
                    }
                }
                if(!can_push) return;

                int pre_si=w_si[t][i][j];
                w_si[t][i].erase(w_si[t][i].begin()+j);
                w_si[t][ni].push_back(nsi);
                sort(all(w_si[t][ni]));

                pre_ans=now_ans[t];
                now_ans[t]=line_to_ans_diff::calc(t);
                if(now_ans[t].empty()){
                    int idx=0;
                    for(auto k:w_si[t][ni]){
                        if(k==nsi) break;
                        idx++;
                    }
                    
                    w_si[t][ni].erase(w_si[t][ni].begin()+idx);
                    w_si[t][i].push_back(pre_si);
                    sort(all(w_si[t][i]));

                    swap(now_ans[t],pre_ans);
                }

                ll new_score_diff=fake_score_diff(t);
                ll new_score_diff2=0;
                if(t+1<D) new_score_diff2=fake_score_diff(t+1);
                ll new_score=pre_score-score_diff_list[t]+new_score_diff;
                if(t+1<D) new_score=new_score-score_diff_list[t+1]+new_score_diff2;
                bool res=solution_update(modify_idx,new_score);
                if(res){
                    score_diff_list[t]=new_score_diff;
                    if(t+1<D) score_diff_list[t+1]=new_score_diff2;
                }else{
                    int idx=0;
                    for(auto k:w_si[t][ni]){
                        if(k==nsi) break;
                        idx++;
                    }
                    w_si[t][ni].erase(w_si[t][ni].begin()+idx);
                    w_si[t][i].push_back(pre_si);
                    sort(all(w_si[t][i]));

                    swap(now_ans[t],pre_ans);
                }
            };
        }

        namespace modify_c{
            vector<rectangle> pre_ans;
            void calc(int modify_idx,int t,int i){
                int i2=rand_int(w_sz);
                if(i==i2) return;

                swap(w_si[t][i],w_si[t][i2]);
                
                
                if(width[i]!=width[i2]){
                    pre_ans=now_ans[t];
                    if(pre_ans.empty()) return;
                    now_ans[t]=line_to_ans_diff::calc(t);
                    if(now_ans[t].empty()){
                        swap(w_si[t][i],w_si[t][i2]);

                        swap(now_ans[t],pre_ans);
                        return;
                    }
                }

                ll new_score_diff=fake_score_diff(t);
                ll new_score_diff2=0;
                if(t+1<D) new_score_diff2=fake_score_diff(t+1);
                ll new_score=pre_score-score_diff_list[t]+new_score_diff;
                if(t+1<D) new_score=new_score-score_diff_list[t+1]+new_score_diff2;
                bool res=solution_update(modify_idx,new_score);
                if(res){
                    score_diff_list[t]=new_score_diff;
                    if(t+1<D) score_diff_list[t+1]=new_score_diff2;
                }else{
                    swap(w_si[t][i],w_si[t][i2]);

                    if(width[i]!=width[i2]) swap(now_ans[t],pre_ans);
                }
            };
        }


        namespace modify_d{
            vector<rectangle> pre_ans;
            void calc(int modify_idx,int t,int i){
                int d=rand_int(2);
                if(d==0) d=-1;
                if(t==0) d=1;
                if(t==D-1) d=-1;
                int l=rand_int((int)w_si[t+d][i].size()),r=rand_int((int)w_si[t+d][i].size());
                if(r<l) swap(l,r);
                if(l==r||l==0||r==(int)w_si[t+d][i].size()-1) return;
                if((int)w_si[t][i].size()-(r-l+1)-1<=0) return;
                int s=rand_int((int)w_si[t][i].size()-(r-l+1)-1)+1;
                if(w_si[t+d][i][l]<=w_si[t][i][s-1]||w_si[t][i][s+r-l+1]<=w_si[t+d][i][r]) return;
                vector<int> pre_w_si=w_si[t][i];
                for(int k=l;k<=r;k++) w_si[t][i][s+k-l]=w_si[t+d][i][k];
                
                pre_ans=now_ans[t];
                now_ans[t]=line_to_ans_diff::calc(t);
                if(now_ans[t].empty()){
                    for(int k=l;k<=r;k++) w_si[t][i][s+k-l]=pre_w_si[s+k-l];

                    swap(now_ans[t],pre_ans);
                    return;
                }
                ll new_score_diff=fake_score_diff(t);
                ll new_score_diff2=0;
                if(t+1<D) new_score_diff2=fake_score_diff(t+1);
                ll new_score=pre_score-score_diff_list[t]+new_score_diff;
                if(t+1<D) new_score=new_score-score_diff_list[t+1]+new_score_diff2;
                bool res=solution_update(modify_idx,new_score);
                if(res){
                    score_diff_list[t]=new_score_diff;
                    if(t+1<D) score_diff_list[t+1]=new_score_diff2;
                }else{
                    for(int k=l;k<=r;k++) w_si[t][i][s+k-l]=pre_w_si[s+k-l];

                    swap(now_ans[t],pre_ans);
                }
            };
        }
        namespace modify_e{
            vector<rectangle> pre_ans;
            void calc(int modify_idx,int t,int i,int j){
                if(rand_int(2)==0&&w_si[t][i][j-1]<w_si[t][i][j]-1){

                    w_si[t][i][j]--;

                    pre_ans=now_ans[t];
                    now_ans[t]=line_to_ans_diff::calc(t);
                    if(now_ans[t].empty()){
                        swap(now_ans[t],pre_ans);
                        return;
                    }
                    ll new_score_diff=fake_score_diff(t);
                    ll new_score_diff2=0;
                    if(t+1<D) new_score_diff2=fake_score_diff(t+1);
                    ll new_score=pre_score-score_diff_list[t]+new_score_diff;
                    if(t+1<D) new_score=new_score-score_diff_list[t+1]+new_score_diff2;

                    bool res=solution_update(modify_idx,new_score);
                    if(res){
                        score_diff_list[t]=new_score_diff;
                        if(t+1<D) score_diff_list[t+1]=new_score_diff2;
                    }else{
                        w_si[t][i][j]++;

                        swap(now_ans[t],pre_ans);
                    }

                }else if(w_si[t][i][j]+1<w_si[t][i][j+1]){

                    w_si[t][i][j]++;
                    
                    pre_ans=now_ans[t];
                    now_ans[t]=line_to_ans_diff::calc(t);
                    if(now_ans[t].empty()){
                        swap(now_ans[t],pre_ans);
                        return;
                    }
                    ll new_score_diff=fake_score_diff(t);
                    ll new_score_diff2=0;
                    if(t+1<D) new_score_diff2=fake_score_diff(t+1);
                    ll new_score=pre_score-score_diff_list[t]+new_score_diff;
                    if(t+1<D) new_score=new_score-score_diff_list[t+1]+new_score_diff2;

                    bool res=solution_update(modify_idx,new_score);
                    if(res){
                        score_diff_list[t]=new_score_diff;

                        score_diff_list[t]=new_score_diff;
                        if(t+1<D) score_diff_list[t+1]=new_score_diff2;
                    }else{
                        w_si[t][i][j]--;
                        
                        swap(now_ans[t],pre_ans);
                    }

                }
            }
        }

        vector<vector<rectangle>> solve(int w_sz_,vector<int> width_,vector<int> sum_width_,vector<vector<vector<int>>> w_si_){

            w_sz=w_sz_;
            width=width_;
            sum_width=sum_width_;
            w_si=w_si_;


            now_ans=line_to_ans(w_sz,width,sum_width,w_si);
            pre_score=fake_score(now_ans,width,w_sz,w_si);
            score_diff_list.assign(D,0);
            rep(i,D) score_diff_list[i]=fake_score_diff(i);

            great.assign(modify_type,0);
            ok.assign(modify_type,0);
            miss.assign(modify_type,0);
            
            int cnt_loop=0;
            while(true){
                cnt_loop++;
                auto now=chrono::system_clock::now();
                int ms=chrono::duration_cast<chrono::milliseconds>(now-program_start).count();
                if(2700<ms) break;
                int t=rand_int(D),i=rand_int(w_sz);
                int modify_idx=rand_int(modify_type+5);
                if(modify_type<=modify_idx){
                    int j=rand_int((int)w_si[t][i].size());
                    if(w_si[t][i][j]==0||w_si[t][i][j]==W) continue;
                    modify_e::calc(4,t,i,j);
                }
                if(modify_idx==3||modify_idx==4){
                    modify_c::calc(2,t,i);
                }else if(modify_idx==5||modify_idx==6){
                    modify_d::calc(3,t,i);
                }
                
                int j=rand_int((int)w_si[t][i].size());
                if(w_si[t][i][j]==0||w_si[t][i][j]==W) continue;
                
                if(modify_idx==0){
                    modify_a::calc(0,t,i,j);
                }else{  
                    modify_b::calc(1,t,i,j);
                }
            }
            /*cerr << cnt_loop << endl;
            int sum=0;
            rep(i,modify_type){
                cerr << great[i] << " " << ok[i] << " " << miss[i] << endl;
                sum+=great[i]+ok[i]+miss[i];
            }
            cerr << sum << " " << pre_score << endl;*/
            return line_to_ans(w_sz,width,sum_width,w_si);
        }
    };


    vector<vector<rectangle>> solve_abcd(vector<vector<rectangle>> first_ans){
        if(first_ans.empty()) return {};
        set<int> set_sum_width;
        rep(t,D){
            rep(i,N){
                set_sum_width.insert(first_ans[t][i].sj);
            }
        }
        vector<int> sum_width;  //Wも入っている
        for(auto i:set_sum_width) sum_width.push_back(i);
        sum_width.push_back(W);
        int w_sz=(int)sum_width.size()-1;
        vector<int> width(w_sz);
        rep(i,w_sz) width[i]=sum_width[i+1]-sum_width[i];
        vector<vector<vector<int>>> w_si(D,vector<vector<int>>(w_sz));  //先頭は0、末尾はW、中身はソートされている
        rep(t,D){
            rep(i,N){
                rep(j,w_sz){
                    if(first_ans[t][i].sj==sum_width[j]){
                        w_si[t][j].push_back(first_ans[t][i].si);
                    }
                }
            }
            rep(i,w_sz){
                sort(all(w_si[t][i]));
                if(w_si[t][i].empty()) w_si[t][i].push_back(0);
                w_si[t][i].push_back(W);
            }
        }
        
        
        return HC::solve(w_sz,width,sum_width,w_si);
    }
    vector<vector<rectangle>> solve(){
        init();
        auto ans=solve_a();
        if(INF_ll<=score(ans)) ans=solve_e();
        else ans=solve_abcd(ans);
        return ans;
    }
}

namespace Solver_first{
    void solve(){
        Solver::init();
        vector<vector<rectangle>> ans=Solver::solve_a();
        if(INF_ll<=Solver::score(ans)) ans=Solver2::solve();
        else ans=Solver::solve_abcd(ans);
        rep(t,D){
            rep(i,N){
                cout << ans[t][i].si << ' ' << ans[t][i].sj << ' ' << ans[t][i].gi << ' ' << ans[t][i].gj << '\n';
            }
        }
        /*auto now=chrono::system_clock::now();
        int ms=chrono::duration_cast<chrono::milliseconds>(now-program_start).count();
        cerr << Solver::score(ans) << ' ' << ms << '\n';*/
    }
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> W >> D >> N;
    A.resize(D,vector<int>(N));
    rep(i,D){
        rep(j,N){
            cin >> A[i][j];
        }
    }
    Solver_first::solve();
}