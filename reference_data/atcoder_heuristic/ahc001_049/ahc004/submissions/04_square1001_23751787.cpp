#include <ctime>
#include <random>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;
int lenl, lenr; long long hlist[1024], hs[4096];
int table[11200000];
long long gethash(string str) {
	int L = str.size();
	long long res = 0;
	for (int i = 0; i < L; ++i) {
		res = (res << 3) | int(str[i] - 'A');
	}
	return res + (1LL << (3 * L));
}
int score(int N, int M, vector<string> answer) {
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			if (answer[i][j] == '.') {
				answer[i][j] = 'A';
			}
		}
	}
	int pos = 0;
	for (int i = lenl; i <= lenr; ++i) {
		for (int j = 0; j < N; ++j) {
			long long h = 0;
			for (int k = 0; k < i; ++k) {
				h = (h << 3) | int(answer[j][k] - 'A');
			}
			for (int k = 0; k < N; ++k) {
				hs[pos++] = h + (1LL << (3 * i));
				h = ((h << 3) | int(answer[j][(k + i) % N] - 'A')) & ((1LL << (3 * i)) - 1);
			}
		}
		for (int j = 0; j < N; ++j) {
			long long h = 0;
			for (int k = 0; k < i; ++k) {
				h = (h << 3) | int(answer[k][j] - 'A');
			}
			for (int k = 0; k < N; ++k) {
				hs[pos++] = h + (1LL << (3 * i));
				h = ((h << 3) | int(answer[(k + i) % N][j] - 'A')) & ((1LL << (3 * i)) - 1);
			}
		}
	}
	for (int i = 0; i < pos; ++i) {
		++table[hs[i] % 11111117];
	}
	int cnt = 0;
	for (int i = 0; i < M; ++i) {
		if (table[hlist[i] % 11111117] != 0) {
			++cnt;
		}
	}
	for (int i = 0; i < pos; ++i) {
		--table[hs[i] % 11111117];
	}
	return cnt;
}
int main() {
	int N, M;
	cin >> N >> M;
	vector<string> S(M);
	for (int i = 0; i < M; ++i) {
		cin >> S[i];
	}
	lenl = (1 << 30);
	lenr = -(1 << 30);
	for (int i = 0; i < M; ++i) {
		lenl = min(lenl, int(S[i].size()));
		lenr = max(lenr, int(S[i].size()));
		hlist[i] = gethash(S[i]);
	}
	vector<bool> maximal(M, true);
	for (int i = 0; i < M; ++i) {
		for (int j = 0; j < M; ++j) {
			if (i == j) continue;
			if (S[j].find(S[i]) != string::npos && (S[i] != S[j] || i > j)) {
				maximal[i] = false;
				break;
			}
		}
	}
	vector<int> maximal_pos;
	for (int i = 0; i < M; ++i) {
		if (maximal[i]) {
			maximal_pos.push_back(i);
		}
	}
	int K = maximal_pos.size();
	vector<string> T(K);
	vector<double> weight(K);
	for (int i = 0; i < K; ++i) {
		T[i] = S[maximal_pos[i]];
	}
	vector<vector<int> > kai_gokan(M);
	for (int i = 0; i < M; ++i) {
		for (int j = 0; j < K; ++j) {
			if (T[j].find(S[i]) != std::string::npos) {
				kai_gokan[i].push_back(j);
			}
		}
		for (int j : kai_gokan[i]) {
			weight[j] += 1.0 / kai_gokan[i].size();
		}
	}
	vector<vector<pair<int, int> > > advance(K);
	for (int i = 0; i < K; ++i) {
		for (int j = 0; j < K; ++j) {
			if (i == j) continue;
			for (int k = 0; k < T[i].size() && k < T[j].size(); ++k) {
				if (T[i].substr(T[i].size() - k) == T[j].substr(0, k)) {
					advance[i].push_back(make_pair(j, T[i].size() - k));
				}
			}
		}
	}
	vector<string> candidates;
	vector<bool> used(K, false);
	for (int i = 0; i < K; ++i) {
		if (used[i]) continue;
		vector<vector<double> > dp(N, vector<double>(K, -1.0e+99));
		dp[0][i] = weight[i];
		vector<vector<pair<int, int> > > pre(N, vector<pair<int, int> >(K, make_pair(-1, -1)));
		for (int j = 0; j < N; ++j) {
			for (int k = 0; k < K; ++k) {
				if (dp[j][k] == -1.0e+99) {
					continue;
				}
				for (pair<int, int> l : advance[k]) {
					int x = j + l.second, y = l.first;
					if (x >= N || y == i || used[y]) continue;
					if (x + T[y].size() <= N || (T[y].substr(N - x) == T[i].substr(0, x + T[y].size() - N))) {
						if (dp[x][y] < dp[j][k] + weight[y]) {
							dp[x][y] = dp[j][k] + weight[y];
							pre[x][y] = make_pair(j, k);
						}
					}
				}
			}
		}
		int inix = -1, iniy = -1; double opt_score = -1.0e+99;
		for (int j = 0; j < N; ++j) {
			for (int k = 0; k < K; ++k) {
				if (opt_score < dp[j][k]) {
					opt_score = dp[j][k];
					inix = j;
					iniy = k;
				}
			}
		}
		string gen(N, '.');
		while (inix != -1 && iniy != -1) {
			pair<int, int> nxt = pre[inix][iniy];
			for (int j = inix; j < inix + T[iniy].size(); ++j) {
				gen[j % N] = T[iniy][j - inix];
			}
			used[iniy] = true;
			inix = nxt.first;
			iniy = nxt.second;
		}
		candidates.push_back(gen);
	}
	vector<string> answer(N, string(N, '.'));
	for (int i = 0; i < N && i < candidates.size(); ++i) {
		answer[i] = candidates[i];
	}
	if (candidates.size() > N) {
		answer = candidates;
		mt19937 mt(1);
		vector<string> panswer = answer;
		vector<string> sanswer = answer;
		int pre_score = 0, max_score = 0;
		int iter = 0;
		while (true) {
			++iter;
			if (iter % 1000 == 0) {
				if (clock() >= (candidates.size() <= 50 ? 1.46 : 2.92) * CLOCKS_PER_SEC) {
					break;
				}
			}
			answer = panswer;
			int tp = (mt() % 4 != 0 ? 0 : 1), tx = -1, ty = -1;
			if (tp == 0) {
				do {
					tx = mt() % answer.size();
					ty = mt() % answer.size();
				} while (tx == ty);
				swap(answer[tx], answer[ty]);
			}
			else {
				tx = mt() % answer.size();
				ty = mt() % (N - 1) + 1;
				answer[tx] = answer[tx].substr(ty) + answer[tx].substr(0, ty);
			}
			string target = answer[N];
			for (int i = 0; i < N; ++i) {
				bool valid = true;
				for (int j = 0; j < N; ++j) {
					if (answer[(i + j) % N].find(target[j]) == string::npos) {
						valid = false;
						break;
					}
				}
				if (valid) {
					vector<string> nanswer(N);
					for (int j = 0; j < N; ++j) {
						nanswer[j] = answer[(i + j) % N];
					}
					for (int j = 0; j < N; ++j) {
						int pos = nanswer[j].find(target[j]);
						answer[j] = nanswer[j].substr(pos) + nanswer[j].substr(0, pos);
					}
					break;
				}
			}
			int ev = score(N, M, answer);
			if (pre_score <= ev + 1) {
				pre_score = ev;
				if (max_score < ev) {
					max_score = ev;
					sanswer = answer;
				}
				if (tp == 0) {
					swap(panswer[tx], panswer[ty]);
				}
				else {
					panswer[tx] = panswer[tx].substr(ty) + panswer[tx].substr(0, ty);
				}
			}
		}
		answer = sanswer;
	}
	if (candidates.size() <= 50) {
		int max_score = score(N, M, answer);
		vector<string> G = candidates;
		int C = G.size();
		vector<long long> bit3(512), bit4(4096);
		vector<vector<vector<int> > > pos3(512, vector<vector<int> >(C)), pos4(4096, vector<vector<int> >(C));
		for (int i = 0; i < C; ++i) {
			for (int j = 0; j < N; ++j) {
				if (G[i][j] == '.') {
					G[i][j] = 'A';
				}
			}
			for (int j = 0; j < N; ++j) {
				int h1 = int(G[i][j] - 'A') * 64 + int(G[i][(j + 1) % N] - 'A') * 8 + int(G[i][(j + 2) % N] - 'A');
				bit3[h1] |= 1LL << i;
				pos3[h1][i].push_back(j);
				int h2 = int(G[i][j] - 'A') * 512 + int(G[i][(j + 1) % N] - 'A') * 64 + int(G[i][(j + 2) % N] - 'A') * 8 + int(G[i][(j + 3) % N] - 'A');
				bit4[h2] |= 1LL << i;
				pos4[h2][i].push_back(j);
			}
		}
		for (int pa = 1; pa < C && clock() < 2.92 * CLOCKS_PER_SEC; ++pa) {
			for (int ra = 0; ra < N; ++ra) {
				for (int pb = 1; pb < C; ++pb) {
					if (pa == pb) continue;
					for (int rb = 0; rb < N; ++rb) {
						bool ok = true;
						for (int i = 0; i < N; ++i) {
							int h = int(G[0][i] - 'A') * 64 + int(G[pa][(i + ra) % N] - 'A') * 8 + int(G[pb][(i + rb) % N] - 'A');
							if ((bit3[h] & ~(1LL << 0) & ~(1LL << pa) & ~(1LL << pb)) == 0) {
								ok = false;
								break;
							}
						}
						if (ok) {
							vector<int> h3(N), cnt3(N);
							for (int i = 0; i < N; ++i) {
								h3[i] = int(G[0][i] - 'A') * 64 + int(G[pa][(i + ra) % N] - 'A') * 8 + int(G[pb][(i + rb) % N] - 'A');
								long long b = (bit3[h3[i]] & ~(1LL << 0) & ~(1LL << pa) & ~(1LL << pb));
								for (int j = 0; j < C; ++j) {
									if ((b >> j) & 1) {
										++cnt3[i];
									}
								}
							}
							vector<int> perm3(N);
							for (int i = 0; i < N; ++i) {
								perm3[i] = i;
							}
							sort(perm3.begin(), perm3.end(), [&](int va, int vb) { return cnt3[va] < cnt3[vb]; });
							vector<bool> used3(C, false);
							used3[0] = true; used3[pa] = true; used3[pb] = true;
							vector<string> answer3(N);
							bool failure3 = false;
							for (int i : perm3) {
								for (int j = 0; j < C; ++j) {
									if (!used3[j] && ((bit3[h3[i]] >> j) & 1)) {
										answer3[i] = G[j].substr(pos3[h3[i]][j][0]) + G[j].substr(0, pos3[h3[i]][j][0]);
										used3[j] = true;
										break;
									}
								}
								if (answer3[i] == "") {
									failure3 = true;
									break;
								}
							}
							if (!failure3) {
								int nxt_score = score(N, M, answer3);
								if (max_score < nxt_score) {
									max_score = nxt_score;
									answer = answer3;
								}
							}
							for (int pc = 1; pc < C; ++pc) {
								if (pa == pc || pb == pc) continue;
								for (int rc = 0; rc < N; ++rc) {
									bool ok2 = true;
									for (int i = 0; i < N; ++i) {
										int h = int(G[0][i] - 'A') * 512 + int(G[pa][(i + ra) % N] - 'A') * 64 + int(G[pb][(i + rb) % N] - 'A') * 8 + int(G[pc][(i + rc) % N] - 'A');
										if ((bit4[h] & ~(1LL << 0) & ~(1LL << pa) & ~(1LL << pb) & ~(1LL << pc)) == 0) {
											ok2 = false;
											break;
										}
									}
									if (ok2) {
										vector<int> h4(N), cnt4(N);
										for (int i = 0; i < N; ++i) {
											h4[i] = int(G[0][i] - 'A') * 512 + int(G[pa][(i + ra) % N] - 'A') * 64 + int(G[pb][(i + rb) % N] - 'A') * 8 + int(G[pc][(i + rc) % N] - 'A');
											long long b = (bit4[h4[i]] & ~(1LL << 0) & ~(1LL << pa) & ~(1LL << pb) & ~(1LL << pc));
											for (int j = 0; j < C; ++j) {
												if ((b >> j) & 1) {
													++cnt4[i];
												}
											}
										}
										vector<int> perm4(N);
										for (int i = 0; i < N; ++i) {
											perm4[i] = i;
										}
										sort(perm4.begin(), perm4.end(), [&](int va, int vb) { return cnt4[va] < cnt4[vb]; });
										vector<bool> used4(C, false);
										used4[0] = true; used4[pa] = true; used4[pb] = true;
										vector<string> answer4(N);
										bool failure4 = false;
										for (int i : perm4) {
											for (int j = 0; j < C; ++j) {
												if (!used4[j] && ((bit4[h4[i]] >> j) & 1)) {
													answer4[i] = G[j].substr(pos4[h4[i]][j][0]) + G[j].substr(0, pos4[h4[i]][j][0]);
													used4[j] = true;
													break;
												}
											}
											if (answer4[i] == "") {
												failure4 = true;
												break;
											}
										}
										if (!failure4) {
											int nxt_score4 = score(N, M, answer4);
											if (max_score < nxt_score4) {
												max_score = nxt_score4;
												answer = answer4;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < N; ++i) {
		cout << answer[i] << endl;
	}
	cerr << score(N, M, answer) << endl;
	return 0;
}