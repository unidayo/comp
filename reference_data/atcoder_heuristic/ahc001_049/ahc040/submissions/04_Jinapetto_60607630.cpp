#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")
#pragma GCC target("avx2")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("O3")
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define ALL(obj) (obj).begin(),(obj).end()

//parameter
double high_start_temp_ratio = 0.005811754692873137;
double high_end_temp_ratio = 0.0002658481669913104;

double low_start_temp_ratio = 0.0001797083113069506;
double low_end_temp_ratio = 0;

int start_kinds_base = 7134; // start_kinds = x / n
int t_kinds_ratio = 5;//t:kinds = 1:x
double high_temp_time = 2.661518612039236;
double margin = 1.185733484815606;

int kinds_2 = 12363;
int kinds_3 = 5265;
int kinds_4 = 5473;
int kinds_5 = 6093;
int kinds_6 = 8867;

ll tate_score = 6176903;


int swap_prob = 9;
int rotate_prob = 1;
int nei_prob = 9;
int eq_dir_prob = 2;

int greedy_ver3_prob = 1;
int greedy_ver4_prob = 5;
int greedy_ver5_prob = 2;


void input_env(){
	char* c;

	c = getenv("kinds_2");
	if(c != nullptr){
		string s = c;
		kinds_2 = stoi(s);
	}
	c = getenv("kinds_3");
	if(c != nullptr){
		string s = c;
		kinds_3 = stoi(s);
	}
	c = getenv("kinds_4");
	if(c != nullptr){
		string s = c;
		kinds_4 = stoi(s);
	}
	c = getenv("kinds_5");
	if(c != nullptr){
		string s = c;
		kinds_5 = stoi(s);
	}
	c = getenv("kinds_6");
	if(c != nullptr){
		string s = c;
		kinds_6 = stoi(s);
	}
}


unsigned long xor128(void){
	static unsigned long x = 123456789,y = 362436069,z = 541288629,w = 88675123;
	unsigned long t;
	t = (x^(x<<11));
	x = y;y = z;z = w;
	return(w = (w^(w>>19))^(t^(t>>8)));
}

int n,t;
ll sigma;
vector<ll> w,h;
vector<ll> sigma_sum;

struct status{
	vector<vector<int>> idx;
	vector<bool> dir;
	vector<ll> w_sum;
	int gyou;
	vector<int> pos_i;
	ll right_sum = 0;
	ll right_sum2 = 0;
	vector<ll> h_sum;
	vector<ll> h_sum2;
	ll score = 0;
	double start_time,end_time;
	double start_temp,end_temp;
	vector<pair<int,int>> inv;
	ll raw_score;

	status(vector<vector<int>> &_idx,vector<bool> &_dir,double _start_time,double _end_time){
		idx = _idx;
		dir = _dir;
		start_time = _start_time;
		end_time = _end_time;
		gyou = idx.size();
		pos_i.assign(n,-1);
		h_sum.assign(gyou,0);
		h_sum2.assign(gyou,0);
		inv.assign(n,{-1,-1});


		rep(i,idx.size()){
			rep(j,idx[i].size()){
				inv[idx[i][j]] = make_pair(i,j);
				pos_i[idx[i][j]] = i;
			}
		}

		w_sum.assign(gyou,0);

		rep(i,n){
			auto [f,s] = inv[i];
			if(dir[i] == 0) w_sum[f] += w[i];
			else w_sum[f] += h[i];
		}

		rep(i,gyou){
			right_sum += w_sum[i];
			right_sum2 += w_sum[i]*w_sum[i];
		}

		rep(i,n){
			if(dir[i] == 0){
				h_sum[pos_i[i]] += h[i];
				h_sum2[pos_i[i]] += h[i]*h[i];
			}else{
				h_sum[pos_i[i]] += w[i];
				h_sum2[pos_i[i]] += w[i]*w[i];
			}
		}

		score += right_sum2/gyou - (right_sum/gyou)*(right_sum/gyou);
		rep(i,gyou) score += h_sum2[i]/idx[i].size() - (h_sum[i]/idx[i].size())*(h_sum[i]/idx[i].size());

		rep(i,n){
			if(dir[i] == (h[i] > w[i])) tate_score++;
		}

		start_temp = score*high_start_temp_ratio;
		end_temp = score*high_end_temp_ratio;

	}

	ll yaki_swap(int i1,int j1,int dir1,int i2,int j2,int dir2,double now_time){
		int p1 = idx[i1][j1];
		int p2 = idx[i2][j2];

		ll nright_sum = right_sum;

		ll nright_sum2 = right_sum2;
		nright_sum2 -= w_sum[i1]*w_sum[i1];
		nright_sum2 -= w_sum[i2]*w_sum[i2];

		ll nw_sum1 = w_sum[i1];
		ll nw_sum2 = w_sum[i2];

		ll nh_sum_1 = h_sum[i1];
		ll nh_sum2_1 = h_sum2[i1];
		ll nh_sum_2 = h_sum[i2];
		ll nh_sum2_2 = h_sum2[i2];

		//差分更新をするためのカスみたいなコード

		if(dir[p1] == 0) nright_sum -= w[p1], nw_sum1 -= w[p1], nh_sum_1 -= h[p1], nh_sum2_1 -= h[p1]*h[p1];
		else nright_sum -= h[p1], nw_sum1 -= h[p1], nh_sum_1 -= w[p1], nh_sum2_1 -= w[p1]*w[p1];

		if(dir[p2] == 0) nright_sum -= w[p2], nw_sum2 -= w[p2], nh_sum_2 -= h[p2], nh_sum2_2 -= h[p2]*h[p2];
		else nright_sum -= h[p2], nw_sum2 -= h[p2], nh_sum_2 -= w[p2], nh_sum2_2 -= w[p2]*w[p2];

		if(dir1 == 0) nright_sum += w[p1], nw_sum2 += w[p1], nh_sum_2 += h[p1], nh_sum2_2 += h[p1]*h[p1];
		else nright_sum += h[p1], nw_sum2 += h[p1], nh_sum_2 += w[p1], nh_sum2_2 += w[p1]*w[p1];

		if(dir2 == 0) nright_sum += w[p2], nw_sum1 += w[p2], nh_sum_1 += h[p2], nh_sum2_1 += h[p2]*h[p2];
		else nright_sum += h[p2], nw_sum1 += h[p2], nh_sum_1 += w[p2], nh_sum2_1 += w[p2]*w[p2];

		nright_sum2 += nw_sum1*nw_sum1 + nw_sum2*nw_sum2;

		ll nscore = score;
		nscore -= right_sum2/gyou - (right_sum/gyou)*(right_sum/gyou);
		nscore += nright_sum2/gyou - (nright_sum/gyou)*(nright_sum/gyou);
		
		nscore -= h_sum2[i1]/idx[i1].size() - (h_sum[i1]/idx[i1].size())*(h_sum[i1]/idx[i1].size());
		nscore += nh_sum2_1/idx[i1].size() - (nh_sum_1/idx[i1].size())*(nh_sum_1/idx[i1].size());

		nscore -= h_sum2[i2]/idx[i2].size() - (h_sum[i2]/idx[i2].size())*(h_sum[i2]/idx[i2].size());
		nscore += nh_sum2_2/idx[i2].size() - (nh_sum_2/idx[i2].size())*(nh_sum_2/idx[i2].size());

		if(dir[p1] == (h[p1] > w[p1])) nscore -= tate_score;
		if(dir[p2] == (h[p2] > w[p2])) nscore -= tate_score;
		if(dir1 == (h[p1] > w[p1])) nscore += tate_score;
		if(dir2 == (h[p2] > w[p2])) nscore += tate_score;

		if(!shift(score - nscore,now_time)) return -1;
		
		vector<int> pre_idx_1 = idx[i1];
		vector<int> pre_idx_2 = idx[i2];

		idx[i1].erase(idx[i1].begin() + j1);
		idx[i2].erase(idx[i2].begin() + j2);
		int cur = 0;
		while(cur != idx[i1].size() && idx[i1][cur] < p2) cur++;
		idx[i1].insert(idx[i1].begin() + cur,p2);

		cur = 0;
		while(cur != idx[i2].size() && idx[i2][cur] < p1) cur++;
		idx[i2].insert(idx[i2].begin() + cur,p1);

		bool pre_dir_1 = dir[p1];
		bool pre_dir_2 = dir[p2];

		dir[p1] = dir1;
		dir[p2] = dir2;

		vector<pair<int,pair<int,int>>> inv_change;
		inv_change.reserve(idx[i1].size() + idx[i2].size());

		rep(j,idx[i1].size()) if(inv[idx[i1][j]] != make_pair(i1,j)){
			inv_change.push_back({idx[i1][j],inv[idx[i1][j]]});
			inv[idx[i1][j]] = {i1,j};
		}

		rep(j,idx[i2].size()) if(inv[idx[i2][j]] != make_pair(i2,j)){
			inv_change.push_back({idx[i2][j],inv[idx[i2][j]]});
			inv[idx[i2][j]] = {i2,j};
		}

		vector<ll> pre_w_sum = w_sum;
		rep(i,gyou) w_sum[i] = 0;
		vector<int> vis_cnt(gyou,0);
		
		rep(i,n){
			auto [f,s] = inv[i];
			if(f != 0 && idx[f - 1].back() > i && w_sum[f] + sigma_sum[vis_cnt[f] + 1 + vis_cnt[f - 1]]*margin + (dir[i] == 0 ? w[i] : h[i]) > w_sum[f - 1]){
				w_sum = pre_w_sum;
				dir[p1] = pre_dir_1;
				dir[p2] = pre_dir_2;
				idx[i1] = pre_idx_1;
				idx[i2] = pre_idx_2;
				rep(j,inv_change.size()) inv[inv_change[j].first] = inv_change[j].second;
				return -1;
			}

			if(dir[i] == 0) w_sum[f] += w[i];
			else w_sum[f] += h[i];
			vis_cnt[f]++;
		}

		score = nscore;
		right_sum = nright_sum;
		right_sum2 = nright_sum2;
		h_sum[i1] = nh_sum_1;
		h_sum2[i1] = nh_sum2_1;
		h_sum[i2] = nh_sum_2;
		h_sum2[i2] = nh_sum2_2;
		return score;
	}

	ll yaki_rotate(int i1,int j1,double now_time){
		int p1 = idx[i1][j1];

		ll nright_sum = right_sum;

		ll nright_sum2 = right_sum2;
		nright_sum2 -= w_sum[i1]*w_sum[i1];

		ll nw_sum1 = w_sum[i1];

		ll nh_sum_1 = h_sum[i1];
		ll nh_sum2_1 = h_sum2[i1];

		//差分更新をするためのカスみたいなコード

		if(dir[p1] == 0){
			nright_sum -= w[p1], nw_sum1 -= w[p1], nh_sum_1 -= h[p1], nh_sum2_1 -= h[p1]*h[p1];
			nright_sum += h[p1], nw_sum1 += h[p1], nh_sum_1 += w[p1], nh_sum2_1 += w[p1]*w[p1];
		}else{
			nright_sum -= h[p1], nw_sum1 -= h[p1], nh_sum_1 -= w[p1], nh_sum2_1 -= w[p1]*w[p1];
			nright_sum += w[p1], nw_sum1 += w[p1], nh_sum_1 += h[p1], nh_sum2_1 += h[p1]*h[p1];
		}

		nright_sum2 += nw_sum1*nw_sum1;

		ll nscore = score;
		nscore -= right_sum2/gyou - (right_sum/gyou)*(right_sum/gyou);
		nscore += nright_sum2/gyou - (nright_sum/gyou)*(nright_sum/gyou);
		
		nscore -= h_sum2[i1]/idx[i1].size() - (h_sum[i1]/idx[i1].size())*(h_sum[i1]/idx[i1].size());
		nscore += nh_sum2_1/idx[i1].size() - (nh_sum_1/idx[i1].size())*(nh_sum_1/idx[i1].size());

		if(dir[p1] == (h[p1] > w[p1])) nscore -= tate_score;
		else nscore += tate_score;

		if(!shift(score - nscore,now_time)) return -1;

		dir[p1] = 1 - dir[p1];

		vector<ll> pre_w_sum = w_sum;
		rep(i,gyou) w_sum[i] = 0;
		vector<int> vis_cnt(gyou,0);
		
		rep(i,n){
			auto [f,s] = inv[i];
			if(f != 0 && idx[f - 1].back() > i && w_sum[f] + sigma_sum[vis_cnt[f] + 1 + vis_cnt[f - 1]]*margin + (dir[i] == 0 ? w[i] : h[i]) > w_sum[f - 1]){
				w_sum = pre_w_sum;
				dir[p1] = 1 - dir[p1];
				return -1;
			}

			if(dir[i] == 0) w_sum[f] += w[i];
			else w_sum[f] += h[i];
			vis_cnt[f]++;
		}

		score = nscore;
		right_sum = nright_sum;
		right_sum2 = nright_sum2;
		h_sum[i1] = nh_sum_1;
		h_sum2[i1] = nh_sum2_1;
		return score;
	}

	bool shift(double scoredist,double now_time){
		long long INF = 1e18;
		double temp = start_temp + (end_temp - start_temp) * (now_time-start_time) / end_time;  //線形でstart_tempからend_tempに変化する。
		double prob = exp(((double)scoredist)/temp); //scoredistが正のときは1負のときは1未満
		return (prob > (xor128()%INF)/(double)INF);
	}

	void calc_raw_score(){
		vector<pair<int,int>> inv(n);
		rep(i,idx.size())rep(j,idx[i].size()) inv[idx[i][j]] = {i,j};
		vector<ll> l(n),r(n),u(n),d(n);
		rep(i,n){
			auto [f,s] = inv[i];
			if(s == 0) l[i] = 0;
			else l[i] = r[idx[f][s - 1]];
			
			if(dir[i] == 0) r[i] = l[i] + w[i];
			else r[i] = l[i] + h[i];

			for(int j = 0;j < i;j++)if(l[i] < r[j] && l[j] < r[i]){
				u[i] = max(u[i],d[j]);
			}

			if(dir[i] == 0) d[i] = u[i] + h[i];
			else d[i] = u[i] + w[i];
		}

		ll r_mx = 0,d_mx = 0;
		rep(i,n){
			r_mx = max(r_mx,r[i]);
			d_mx = max(d_mx,d[i]);
		}
		raw_score = r_mx + d_mx;
	}
};

void input();
pair<vector<vector<int>>,vector<bool>> greedy_ver3(int gyou);
void yaki_ver5(int gyou,double start_time,double end_time,int start_kinds,int end_kinds);
pair<ll,ll> out(status &sta);
void accurate(int cnt);
void calc_sigma_sum();
pair<int,int> calc_kinds();

int main(){
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	// input_env();
	input();
	double cur_time = (double)clock()/CLOCKS_PER_SEC;
	auto [start_kinds,end_kinds] = calc_kinds();
	accurate(t - end_kinds);
	calc_sigma_sum();
	yaki_ver5(sqrt(n),cur_time,2.9,start_kinds,end_kinds);
	return 0;
}

void input(){
	cin >> n >> t >> sigma;
	w.assign(n,0);
	h.assign(n,0);
	rep(i,n) cin >> w[i] >> h[i];
}

pair<vector<vector<int>>,vector<bool>> greedy_ver2(int gyou){
	//各長方形の短辺の最大値
	ll smaller_mx = 0;
	rep(i,n) smaller_mx = max(smaller_mx,min(h[i],w[i]));
	//gyou(配置する列）に対する最小の横幅を二分探索
	ll  ng = 0,ok = 1e13;
	while(ok - ng > 1){
		ll mid = (ok + ng)/2;
		if(mid < smaller_mx){
			ng = mid;
			continue;
		}
		int cnt = 1;
		ll w_sum = 0;
		rep(i,n){
			if(w_sum + min(h[i],w[i]) <= mid) w_sum += min(h[i],w[i]);
			else{
				cnt++;
				w_sum = min(h[i],w[i]);
			}
		}
		if(cnt <= gyou) ok = mid;
		else ng = mid;
	}

	vector<vector<int>> idx(gyou);
	vector<bool> dir(n);
	ll w_lim = ok;
	ll w_sum = 0;
	int cur_gyou = 0;
	rep(i,n){
		if(h[i] > w[i]){
			if(w_sum + w[i] > w_lim){
				w_sum = 0;
				cur_gyou++;
			}
			idx[cur_gyou].push_back(i);
			dir[i] = 0;
			w_sum += w[i];
		}else{
			if(w_sum + h[i] > w_lim){
				w_sum = 0;
				cur_gyou++;
			}
			idx[cur_gyou].push_back(i);
			dir[i] = 1;
			w_sum += h[i];
		}
	}
	return {idx,dir};
}

pair<vector<vector<int>>,vector<bool>> greedy_ver3(int gyou){
	vector<bool> dir(n);
	rep(i,n) dir[i] = xor128()%2;
	//各長方形の短辺の最大値
	ll smaller_mx = 0;
	rep(i,n) smaller_mx = max(smaller_mx,(dir[i] == 0 ? w[i] : h[i]));
	//gyou(配置する列）に対する最小の横幅を二分探索
	ll  ng = 0,ok = 1e13;
	while(ok - ng > 1){
		ll mid = (ok + ng)/2;
		if(mid < smaller_mx){
			ng = mid;
			continue;
		}
		int cnt = 1;
		ll w_sum = 0;
		rep(i,n){
			ll val = (dir[i] == 0 ? w[i] : h[i]);
			if(w_sum + val <= mid) w_sum += val;
			else{
				cnt++;
				w_sum = val;
			}
		}
		if(cnt <= gyou) ok = mid;
		else ng = mid;
	}
	vector<vector<int>> idx(gyou);
	ll w_lim = ok;
	ll w_sum = 0;
	int cur_gyou = 0;
	rep(i,n){
		if(!dir[i]){
			if(w_sum + w[i] > w_lim){
				w_sum = 0;
				cur_gyou++;
			}
			idx[cur_gyou].push_back(i);
			w_sum += w[i];
		}else{
			if(w_sum + h[i] > w_lim){
				w_sum = 0;
				cur_gyou++;
			}
			idx[cur_gyou].push_back(i);
			w_sum += h[i];
		}
	}
	return {idx,dir};
}

pair<vector<vector<int>>,vector<bool>> greedy_ver4(int gyou){
	vector<bool> dir(n);
	rep(i,n) dir[i] = xor128()%2;
	ll w_ave = 0;
	ll right_ave = 0;
	rep(i,n) right_ave += (dir[i] == 0 ? w[i] : h[i]);
	w_ave = right_ave/n;
	right_ave = (right_ave/gyou + (w_ave)/2);

	vector<ll> w_sum(gyou,0);
	vector<vector<int>> idx(gyou);
	vector<int> vis_cnt(gyou,0);
	vector<bool> fin(gyou,false);
	rep(i,n){
		vector<int> candi;
		rep(j,gyou)if(!fin[j]){
			if(j != 0 && !fin[j - 1] && w_sum[j] + sigma_sum[vis_cnt[j] + 1 + vis_cnt[j - 1]]*2 + (dir[i] == 0 ? w[i] : h[i]) > w_sum[j - 1]) break;
			if(w_sum[j] == 0){
				candi.push_back(j);
				break;
			}
			candi.push_back(j);
		}
		if(candi.size() == 0)rep(k,gyou)if(!fin[k]){
			candi.push_back(k);
			break;
		}
		if(candi.size() == 0) candi.push_back(gyou - 1);
		int j = candi[xor128()%candi.size()];
		w_sum[j] += (dir[i] == 0 ? w[i] : h[i]);
		vis_cnt[j]++;
		idx[j].push_back(i);
		if(w_sum[j] + w_ave > right_ave) fin[j] = true;
	}
	return {idx,dir};
}

pair<vector<vector<int>>,vector<bool>> greedy_ver5(int gyou){
	vector<bool> dir(n);
	rep(i,n) dir[i] = xor128()%2;
	ll w_ave = 0;
	ll right_ave = 0;
	rep(i,n) right_ave += (dir[i] == 0 ? w[i] : h[i]);
	w_ave = right_ave/n;
	right_ave = (right_ave/gyou + (w_ave)/2);

	vector<ll> w_sum(gyou,0);
	vector<vector<int>> idx(gyou);
	vector<int> vis_cnt(gyou,0);
	vector<bool> fin(gyou,false);
	rep(i,n){
		vector<int> candi;
		rep(j,gyou)if(!fin[j]){
			if(j != 0 && !fin[j - 1] && w_sum[j] + sigma_sum[vis_cnt[j] + 1 + vis_cnt[j - 1]]*2 + (dir[i] == 0 ? w[i] : h[i]) > w_sum[j - 1]) break;
			if(w_sum[j] == 0){
				candi.push_back(j);
				break;
			}
			candi.push_back(j);
		}
		if(candi.size() == 0)rep(k,gyou)if(!fin[k]){
			candi.push_back(k);
			break;
		}
		if(candi.size() == 0) candi.push_back(gyou - 1);
		int j = candi.back();
		w_sum[j] += (dir[i] == 0 ? w[i] : h[i]);
		vis_cnt[j]++;
		idx[j].push_back(i);
		if(w_sum[j] + w_ave > right_ave) fin[j] = true;
	}
	return {idx,dir};
}

void yaki_ver5(int gyou,double start_time,double end_time,int start_kinds,int end_kinds){
	vector<status> sta;
	pair<vector<vector<int>>,vector<bool>> ret = greedy_ver2(gyou);
	sta.push_back(status(ret.first,ret.second,start_time,end_time));
	rep(i,start_kinds){
		int select_greedy = xor128()%(greedy_ver3_prob + greedy_ver4_prob + greedy_ver5_prob);
		if(select_greedy < greedy_ver3_prob) ret = greedy_ver3(gyou);
		else if(select_greedy < greedy_ver3_prob + greedy_ver4_prob) ret = greedy_ver4(gyou);
		else ret = greedy_ver5(gyou);
		sta.push_back(status(ret.first,ret.second,start_time,high_temp_time));
	}

	double now_time = (double)clock()/CLOCKS_PER_SEC;
	int yaki_cnt = 0;

	while(1){
		yaki_cnt++;
		if(yaki_cnt % 100 == 0) now_time = (double)clock()/CLOCKS_PER_SEC;
		if(now_time > high_temp_time) break;
		rep(j,start_kinds){
			int op = xor128()%(swap_prob + rotate_prob + nei_prob + eq_dir_prob);
			if(op < swap_prob){//swap
				int i1 = xor128()%gyou;
				int i2 = xor128()%gyou;
				if(i1 == i2) continue;
				int j1 = xor128()%sta[j].idx[i1].size();
				int j2 = xor128()%sta[j].idx[i2].size();
				int dir1 = xor128()%2;
				int dir2 = xor128()%2;
				sta[j].yaki_swap(i1,j1,dir1,i2,j2,dir2,now_time);
			}else if(op < swap_prob + rotate_prob){
				int i1 = xor128()%gyou;
				int j1 = xor128()%sta[j].idx[i1].size();
				sta[j].yaki_rotate(i1,j1,now_time);
			}else if(op < swap_prob + rotate_prob + nei_prob){
				int i1 = xor128()%(gyou - 1);
				int i2 = i1 + 1;
				int j1 = xor128()%sta[j].idx[i1].size();
				int j2 = xor128()%sta[j].idx[i2].size();
				int dir1 = xor128()%2;
				int dir2 = xor128()%2;
				sta[j].yaki_swap(i1,j1,dir1,i2,j2,dir2,now_time);
			}else{
				int i1 = xor128()%gyou;
				int i2 = xor128()%gyou;
				if(i1 == i2) continue;
				int j1 = xor128()%sta[j].idx[i1].size();
				int j2 = xor128()%sta[j].idx[i2].size();
				int dir1 = sta[j].dir[sta[j].idx[i1][j1]];
				int dir2 = sta[j].dir[sta[j].idx[i2][j2]];
				sta[j].yaki_swap(i1,j1,dir1,i2,j2,dir2,now_time);
			}
		}
	}
		
	rep(i,start_kinds){
		sta[i].start_time = high_temp_time;
		sta[i].end_time = end_time;
		sta[i].start_temp = sta[i].score*low_start_temp_ratio;
		sta[i].end_temp = sta[i].score*low_end_temp_ratio;
	}
	

	while(1){
		yaki_cnt++;
		if(yaki_cnt % 100 == 0) now_time = (double)clock()/CLOCKS_PER_SEC;
		if(now_time > end_time) break;
		rep(j,start_kinds){
			int op = xor128()%(swap_prob + rotate_prob);
			if(op < swap_prob){//swap
				int i1 = xor128()%gyou;
				int i2 = xor128()%gyou;
				if(i1 == i2) continue;
				int j1 = xor128()%sta[j].idx[i1].size();
				int j2 = xor128()%sta[j].idx[i2].size();
				int dir1 = xor128()%2;
				int dir2 = xor128()%2;
				sta[j].yaki_swap(i1,j1,dir1,i2,j2,dir2,now_time);
			}else if(op < swap_prob + rotate_prob){
				int i1 = xor128()%gyou;
				int j1 = xor128()%sta[j].idx[i1].size();
				sta[j].yaki_rotate(i1,j1,now_time);
			}
		}
	}

	rep(i,start_kinds){
		sta[i].calc_raw_score();
	}

	sort(ALL(sta),[&](status &a,status &b){
		return a.raw_score < b.raw_score;
	});
	sta.erase(unique(sta.begin(),sta.end(),[&](status &a,status &b){
		return (a.idx == b.idx && a.dir == b.dir);
	}),sta.end());

	rep(i,end_kinds){
		if(sta.size() <= i) out(sta[0]);
		else out(sta[i]);
	}
}

pair<ll,ll> out(status &sta){
	cout << "# raw_score = " << sta.raw_score << endl;
	vector<pair<int,int>> inv(n);
	rep(i,sta.idx.size()){
		rep(j,sta.idx[i].size()) inv[sta.idx[i][j]] = {i,j};
	}
	cout << n << endl;
	rep(i,n){
		if(sta.dir[i] == 0){
			if(inv[i].second != 0){
				cout << i << ' ' << 0 << ' ' << 'U' << ' ' << sta.idx[inv[i].first][inv[i].second - 1] << endl;
			}else{
				cout << i << ' ' << 0 << ' ' << 'U' << ' ' << -1 << endl;
			}
		}else{
			if(inv[i].second != 0){
				cout << i << ' ' << 1 << ' ' << 'U' << ' ' << sta.idx[inv[i].first][inv[i].second - 1] << endl;
			}else{
				cout << i << ' ' << 1 << ' ' << 'U' << ' ' << -1 << endl;
			}
		}
	}
	ll ret1,ret2;
	return {ret1,ret2};
}

void accurate(int cnt){
	sigma = round(sigma/sqrt(cnt/n + 1));
	vector<vector<ll>> h_v(n),w_v(n);
	rep(i,n){
		h_v[i].push_back(h[i]);
		w_v[i].push_back(w[i]);
	}
	rep(_i,cnt){
		int i = _i%n;
		cout << 1 << endl;
		cout << i << ' ' << 0 << ' ' << 'U' << ' ' << -1 << endl;
		ll ret_h,ret_w;
		cin >> ret_w >> ret_h;
		h_v[i].push_back(ret_h);
		w_v[i].push_back(ret_w);
	}
	rep(i,n){
		ll sum_h = 0;
		ll sum_w = 0;
		rep(j,h_v[i].size()){
			sum_h += h_v[i][j];
			sum_w += w_v[i][j];
		}
		h[i] = sum_h/(int)h_v[i].size();
		w[i] = sum_w/(int)w_v[i].size();
	}
}

void calc_sigma_sum(){
	sigma_sum.assign(2*n + 10,0);
	rep(i,2*n + 10) sigma_sum[i] = i*sigma*sigma;
	rep(i,2*n + 10) sigma_sum[i] = sqrt(sigma_sum[i]);
}

pair<int,int> calc_kinds(){
	ll all_sum = 0;
	ll all_sum2 = 0;
	rep(i,n) all_sum += h[i] + w[i];
	rep(i,n) all_sum2 += h[i]*h[i] + w[i]*w[i];
	ll all_sigma = all_sum2/(2*n) - (all_sum/(2*n))*(all_sum/(2*n));

	ll all_ave = all_sum/(2*n);

	double gosa = (double)sigma/all_ave;

	int start_kinds;
	int end_kinds;

	if(gosa <= 0.02){
		end_kinds = t/3;
	}else if(gosa <= 0.04){
		end_kinds = t/4;
	}else if(gosa <= 0.06){
		end_kinds = t/5;
	}else if(gosa <= 0.08){
		end_kinds = t/5;
	}else{
		end_kinds = t/5;
	}

	
	if(all_sigma < 300000000){
		start_kinds = kinds_2/n;
	}else if(all_sigma < 400000000){
		start_kinds = kinds_3/n;
	}else if(all_sigma < 500000000){
		start_kinds = kinds_4/n;
	}else if(all_sigma < 600000000){
		start_kinds = kinds_5/n;
	}else{
		start_kinds = kinds_6/n;
	}
	return {max(start_kinds,end_kinds),min(t,end_kinds)};
}