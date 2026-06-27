//~ while (clock()<=69*CLOCKS_PER_SEC)
//~ #pragma comment(linker, "/stack:200000000")
#pragma GCC optimize("O3")
//~ #pragma GCC target ("avx2")
//~ #pragma GCC optimize("Ofast")
//~ #pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")
//~ #pragma GCC optimize("unroll-loops")
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

#include <sys/time.h>

using namespace __gnu_pbds;
using namespace std;

template <typename T>
using ordered_set =
    tree<T, null_type, less<T>, rb_tree_tag, tree_order_statistics_node_update>;

#define sim template < class c
#define ris return * this
#define dor > debug & operator <<
#define eni(x) sim > typename \
  enable_if<sizeof dud<c>(0) x 1, debug&>::type operator<<(c i) {
sim > struct rge { c b, e; };
sim > rge<c> range(c i, c j) { return rge<c>{i, j}; }
sim > auto dud(c* x) -> decltype(cerr << *x, 0);
sim > char dud(...);
struct debug {
#ifdef LOCAL
~debug() { cerr << endl; }
eni(!=) cerr << boolalpha << i; ris; }
eni(==) ris << range(begin(i), end(i)); }
sim, class b dor(pair < b, c > d) {
  ris << "(" << d.first << ", " << d.second << ")";
}
sim dor(rge<c> d) {
  *this << "[";
  for (auto it = d.b; it != d.e; ++it)
    *this << ", " + 2 * (it == d.b) << *it;
  ris << "]";
}
#else
sim dor(const c&) { ris; }
#endif
};
#define imie(...) " [" << #__VA_ARGS__ ": " << (__VA_ARGS__) << "] "

#define shandom_ruffle random_shuffle

//for time
double pam_time;

void init_time()
{
	timeval tv;
	gettimeofday(&tv, 0);
	pam_time=tv.tv_sec+tv.tv_usec*1e-6;
}

inline double get_time()
{
	timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec+tv.tv_usec*1e-6 - pam_time;
}
//end of time

using ll=long long;
using pii=pair<int,int>;
using pll=pair<ll,ll>;
using vi=vector<int>;
using vll=vector<ll>;
const int nax=1007;
using ld=double;
const ld tl=2.92;
using zapis=vector<vi>;

const int n=20;
int m;

char wcz[nax];

int orym;
vi oryslo[nax];

vi slo[nax];

int out[nax][nax];

int wsobie[nax];

int los(int a, int b)
{
	return a+rand()%(b-a+1);
}

template <typename T> T wylosuj(const vector<T> &wek)
{
	return wek[los(0, (int)wek.size()-1)];
}

void wypisz()
{
	for (int i=0; i<n; i++)
	{
		for (int j=0; j<n; j++)
		{
			if (out[i][j]==-1)
				printf(".");
			else
				printf("%c", (char)(out[i][j]+'A'));
		}
		printf("\n");
	}
	exit(0);
}

void wypisz(zapis v)
{
	for (int i=0; i<n; i++)
	{
		for (int j=0; j<n; j++)
		{
			if (v[i][j]==-1)
				printf(".");
			else
				printf("%c", (char)(v[i][j]+'A'));
		}
		printf("\n");
	}
	exit(0);
}

int pasuje_ory(int a, int b, int kto, int poz)
{
	if (poz)
	{
		for (int i=0; i<(int)oryslo[kto].size(); i++)
		{
			if (oryslo[kto][i]!=out[a][(b+i)%n])
				return 0;
		}
	}
	else
	{
		for (int i=0; i<(int)oryslo[kto].size(); i++)
		{
			if (oryslo[kto][i]!=out[(a+i)%n][b])
				return 0;
		}
	}
	return 1;
}

int pasuje_ory_pra(int a, int b, int kto, int poz)
{
	if (poz)
	{
		for (int i=0; i<(int)oryslo[kto].size(); i++)
		{
			if (out[a][(b+i)%n]!=-1 && oryslo[kto][i]!=out[a][(b+i)%n])
				return 0;
		}
	}
	else
	{
		for (int i=0; i<(int)oryslo[kto].size(); i++)
		{
			if (out[(a+i)%n][b]!=-1 && oryslo[kto][i]!=out[(a+i)%n][b])
				return 0;
		}
	}
	return 1;
}

void poloz_ory(int a, int b, int kto, int poz)
{
	if (poz)
	{
		for (int i=0; i<(int)oryslo[kto].size(); i++)
		{
			if (out[a][(b+i)%n]!=-1)
				assert(out[a][(b+i)%n]==oryslo[kto][i]);
			out[a][(b+i)%n]=oryslo[kto][i];
		}
	}
	else
	{
		for (int i=0; i<(int)oryslo[kto].size(); i++)
		{
			if (out[(a+i)%n][b]!=-1)
				assert(out[(a+i)%n][b]==oryslo[kto][i]);
			out[(a+i)%n][b]=oryslo[kto][i];
		}
	}
}

pii pasuje(int a, int b, int kto, int poz)
{
	int roz=0;
	int pus=0;
	if (poz)
	{
		for (int i=0; i<(int)slo[kto].size(); i++)
		{
			if (out[a][(b+i)%n]==-1)
				pus++;
			else if (slo[kto][i]!=out[a][(b+i)%n])
				roz++;
		}
	}
	else
	{
		for (int i=0; i<(int)slo[kto].size(); i++)
		{
			if (out[(a+i)%n][b]==-1)
				pus++;
			else if (slo[kto][i]!=out[(a+i)%n][b])
				roz++;
		}
	}
	return {roz, pus};
}

void poloz(int a, int b, int kto, int poz)
{
	if (poz)
	{
		for (int i=0; i<(int)slo[kto].size(); i++)
		{
			if (out[a][(b+i)%n]!=-1)
				assert(out[a][(b+i)%n]==slo[kto][i]);
			out[a][(b+i)%n]=slo[kto][i];
		}
	}
	else
	{
		for (int i=0; i<(int)slo[kto].size(); i++)
		{
			if (out[(a+i)%n][b]!=-1)
				assert(out[(a+i)%n][b]==slo[kto][i]);
			out[(a+i)%n][b]=slo[kto][i];
		}
	}
}

pii pasuje(pair<pii,int> lok, int v)
{
	return pasuje(lok.first.first, lok.first.second, v, lok.second);
}

void poloz(pair<pii,int> lok, int v)
{
	return poloz(lok.first.first, lok.first.second, v, lok.second);
}

vector<pair<pii,int>> mozliwe[nax];

zapis daj_zapis()
{
	vector<vi> ret;
	for (int i=0; i<n; i++)
	{
		vi ter;
		for (int j=0; j<n; j++)
			ter.push_back(out[i][j]);
		ret.push_back(ter);
	}
	return ret;
}

int solve()
{
	//~ int ret=0;
	vi jesz;
	for (int i=1; i<=m; i++)
		jesz.push_back(i);
	for (int i=0; i<n; i++)
		for (int j=0; j<n; j++)
			out[i][j]=-1;
	for (int h=1; h<=m; h++)
	{
		mozliwe[h].clear();
		for (int i=0; i<n; i++)
			for (int j=0; j<n; j++)
				for (int l=0; l<2; l++)
					mozliwe[h].push_back({{i, j}, l});
	}
	
	//~ int tryb=los(0, 1);
	while(!jesz.empty())
	{
		vector<pair<pii,int>> gdz;
		int najd=0;
		for (int i : jesz)
			najd=max(najd, (int)slo[i].size());
		int jakismam=0;
		vi res;
		for (int h=0; h<(int)jesz.size(); h++)
		{
			int v=jesz[h];
			if ((int)slo[v].size()!=najd)
			{
				gdz.push_back({{-1, -1}, -1});
				res.push_back(-1);
				continue;
			}
			pii best={nax, nax};
			vector<pair<pii,int>> tmp;
			for (int i=0; i<(int)mozliwe[v].size(); i++)
			{
				pii wez=pasuje(mozliwe[v][i], v);
				if (wez.first)
				{
					swap(mozliwe[v][i], mozliwe[v].back());
					mozliwe[v].pop_back();
					i--;
					continue;
				}
				if (wez<best)
				{
					best=wez;
					tmp.clear();
				}
				if (wez==best)
				{
					tmp.push_back(mozliwe[v][i]);
				}
			}
			if (best.first || (!best.second))
			{
				//~ ret+=(!best.first)*wsobie[v];
				swap(jesz[h], jesz.back());
				jesz.pop_back();
				h--;
				continue;
			}
			jakismam=1;
			gdz.push_back(wylosuj(tmp));
			res.push_back(best.second);
		}
		if (!jakismam || jesz.empty())
			continue;
		vi poz;
		vi sofar={nax, nax, nax};
		for (int i=0; i<(int)jesz.size(); i++)
		{
			vi tu;
			//~ if (tryb)
				//~ tu={-(int)slo[jesz[i]].size(), res[i], -wsobie[jesz[i]]};
			//~ else
			tu={-(int)slo[jesz[i]].size(), -wsobie[jesz[i]], res[i]};
			//~ vi tu={-(int)slo[jesz[i]].size(), res[i]};
			if (tu<sofar)
			{
				sofar=tu;
				poz.clear();
			}
			if (tu==sofar)
			{
				poz.push_back(i);
			}
		}
		int x=wylosuj(poz);
		poloz(gdz[x], jesz[x]);
	}
	int ret=0;
	vector<set<ll>> setel(14);
	for (int i=0; i<n; i++)
	{
		for (int j=0; j<n; j++)
		{
			{
				ll x=0;
				for (int l=0; l<12; l++)
				{
					x=x*8+out[i][(j+l)%n];
					setel[l+1].insert(x);
				}
			}
			{
				ll x=0;
				for (int l=0; l<12; l++)
				{
					x=x*8+out[(i+l)%n][j];
					setel[l+1].insert(x);
				}
			}
		}
	}
	for (int i=1; i<=orym; i++)
	{
		int ok=0;
		ll x=0;
		for (int j : oryslo[i])
			x=x*8+j;
		if (setel[oryslo[i].size()].count(x))
			ok=1;
		if (ok)
		{
			ret++;
		}
		else
		{
			for (int j=0; j<n && !ok; j++)
			{
				for (int l=0; l<n && !ok; l++)
				{
					for (int p=0; p<2 && !ok; p++)
					{
						if (pasuje_ory_pra(j, l, i, p))
						{
							ok=1;
							poloz_ory(j, l, i, p);
						}
					}
				}
			}
			if (ok)
			{
				ret++;
			}
		}
	}
	return ret;
}

string cyc(string s)
{
	string ret=s;
	for (int i=0; i<n; i++)
	{
		s.push_back(s[0]);
		s.erase(s.begin());
		ret=min(ret, s);
	}
	return ret;
}

void scalanie()
{
	m=orym;
	for (int i=1; i<=m; i++)
	{
		slo[i]=oryslo[i];
		wsobie[i]=1;
	}
	shandom_ruffle(slo+1, slo+1+m);
	const int lim=los(4, 6);
	//~ const int lim=los(5, 7);
	//~ const int lim=7;
	int ile=los(1, 3);
	for (int powt=0; powt<ile; powt++)
	{
		for (int i=1; i<=m; i++)
		{
			for (int j=1; j<i; j++)
			{
				int ri=slo[i].size();
				int rj=slo[j].size();
				pii naj{0, 0};
				for (int l=0; l<n; l++)
				{
					int ok=1;
					int prz=0;
					for (int p=0; p<rj; p++)
					{
						int g=(p+l)%n;
						if (g>=0 && g<ri)
						{
							prz++;
							if (slo[j][p]!=slo[i][g])
								ok=0;
						}
					}
					if (ok)
						naj=max(naj, {prz, l});
				}
				if (naj.first<lim && !(ri==rj && naj.first==ri))
					continue;
				vi co(n, -1);
				for (int l=0; l<ri; l++)
					co[l]=slo[i][l];
				for (int l=0; l<rj; l++)
					co[(l+naj.second)%n]=slo[j][l];
				int g=0;
				for (int l=0; l<n; l++)
					if (co[l]>=0 && co[(l-1+n)%n]==-1)
						g=l;
				vi now;
				for (int l=0; l<n && co[(g+l)%n]!=-1; l++)
					now.push_back(co[(g+l)%n]);
				wsobie[j]+=wsobie[i];
				slo[j]=now;
				wsobie[i]=wsobie[m];
				slo[i]=slo[m];
				m--;
				i--;
				break;
			}
		}
	}
	//~ debug() << "z " << orym << " na " << m << " bo " << imie(lim) << " " << imie(ile);
	//~ vi rozy;
	//~ for (int i=1; i<=m; i++)
		//~ rozy.push_back(slo[i].size());
	//~ debug() << rozy;
}

int bt_juz[nax];

int lim_ruch;
int nope;

vector<pii> gdz[8];

int bt(int v)
{
	lim_ruch++;
	if (!(lim_ruch%100) && get_time()>2.7)
		nope=1;
	if (nope)
		return 0;
	
	//~ if (lim_ruch>5000000)
		//~ return 0;
	if (v==m)
	{
		debug() << imie(lim_ruch);
		return 1;
	}
	if (!(v&1))
	{
		int rzo=v/2;
		for (int i=1; i<=m; i++)
		{
			if (bt_juz[i])
				continue;
			for (int j=0; j<n; j++)
			{
				if (!v && (i>1 || j>0))
					continue;
				if (v && slo[i][j]!=out[rzo][0])
					continue;
				if (rzo>1 && j!=n-1 && slo[i][j+1]!=out[rzo][1])
					continue;
				int ok=1;
				int w=j;
				for (int l=0; l<rzo; l++)
				{
					if (out[rzo][l]!=slo[i][w])
					{
						ok=0;
						break;
					}
					w++;
					if (w==n)
						w=0;
				}
				if (!ok)
					continue;
				for (int l=rzo; l<n; l++)
					out[rzo][l]=slo[i][(j+l)%n];
				bt_juz[i]=1;
				if (bt(v+1))
					return 1;
				bt_juz[i]=0;
				//~ for (int l=rzo; l<n; l++)
					//~ out[rzo][l]=-1;
			}
		}
	}
	else
	{
		int kol=v/2;
		//~ for (pii h : gdz[i][j])
		for (int i=1; i<=m; i++)
		{
			if (bt_juz[i])
				continue;
			for (int j=0; j<n; j++)
			{
				if (slo[i][j]!=out[0][kol])
					continue;
				if (kol && j!=n-1 && slo[i][j+1]!=out[1][kol])
					continue;
				int ok=1;
				int w=j;
				for (int l=0; l<=kol; l++)
				{
					if (out[l][kol]!=slo[i][w])
					{
						ok=0;
						break;
					}
					w++;
					if (w==n)
						w=0;
				}
				if (!ok)
					continue;
				for (int l=kol+1; l<n; l++)
					out[l][kol]=slo[i][(j+l)%n];
				bt_juz[i]=1;
				if (bt(v+1))
					return 1;
				bt_juz[i]=0;
				//~ for (int l=kol+1; l<n; l++)
					//~ out[l][kol]=-1;
			}
		}
	}
	return 0;
}

int proba_opta()
{
	if (m!=2*n)
		return -1;
	for (int i=1; i<=m; i++)
		if ((int)slo[i].size()!=n)
			return -1;
	static int walilem=0;
	if (walilem)
		return -1;
	walilem=1;
	vi wys(8);
	for (int i=1; i<=m; i++)
		for (int j : slo[i])
			wys[j]++;
	int g=0;
	for (int i=1; i<8; i++)
		if (wys[i]<wys[g])
			g=i;
	{
		for (int i=1; i<=m && slo[1][0]!=g; i++)
		{
			for (int j=0; j<n && slo[1][0]!=g; j++)
			{
				if (slo[i][j]==g)
				{
					swap(slo[i], slo[1]);
					while(slo[1][0]!=g)
					{
						slo[1].push_back(slo[1][0]);
						slo[1].erase(slo[1].begin());
					}
				}
			}
		}
	}
	for (int i=1; i<=m; i++)
		for (int j=0; j<n; j++)
			gdz[slo[i][j]].push_back({i, j});
	for (int i=0; i<n; i++)
		for (int j=0; j<n; j++)
			out[i][j]=-1;
	if (bt(0))
		return orym;
	debug() << imie(lim_ruch);
	return -1;
}

bool mniej(vi &a, vi &b)
{
	return a.size()<b.size();
}

int main()
{
	init_time();
	scanf("%d", &m);
	scanf("%d", &m);
	int najk=nax;
	int najd=-nax;
	orym=m;
	for (int i=1; i<=m; i++)
	{
		scanf("%s", wcz);
		for (int j=0; wcz[j]; j++)
			slo[i].push_back({wcz[j]-'A'});
		najk=min(najk, (int)slo[i].size());
		najd=max(najd, (int)slo[i].size());
		oryslo[i]=slo[i];
	}
	sort(oryslo+1, oryslo+1+m, mniej);
	int best=-1;
	zapis jak;
	int proby=0;
	const int czesto=10;
	for (int h=0; (h==0 || (get_time()*(1+1.1/h)<tl)); h++)
	{
		if (!(h%czesto))
			scalanie();
		proby++;
		int wez=solve();
		if (wez>best)
		{
			best=wez;
			jak=daj_zapis();
		}
		if (!(h%czesto))
		{
			int wez=proba_opta();
			if (wez>best)
			{
				best=wez;
				jak=daj_zapis();
			}
		}
		if (best==orym)
			break;
	}
	debug() << imie(proby);
	debug() << imie(najk) << imie(najd);
	debug() << llround(best*1e8/orym);
	wypisz(jak);
	return 0;
}
//~ 4702097519
//~ 4957888861
//~ 5260071477
//~ 5425457044
//~ 5545506303
//~ 6201698108
//~ 6561336830
//~ 6812698215
