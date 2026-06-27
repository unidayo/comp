#define NDEBUG

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <cmath>

#include <iostream>
#include <sstream>
#include <array>
#include <valarray>
#include <bitset>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <random>
#include <chrono>
#include <ranges>

using namespace std;

chrono::system_clock::time_point startTime = chrono::system_clock::now();
constexpr int TimeLimit = 1980;

constexpr ranges::iota_view<int, int> range(int count) {
    return ranges::iota_view<int, int>(0, count);
}

constexpr ranges::iota_view<int, int> range(size_t count) {
    return ranges::iota_view<int, int>(0, (int)count);
}

template <typename T, size_t C>
constexpr ranges::iota_view<int, int> range(const array<T, C> &v) {
    return ranges::iota_view<int, int>(0, (int)v.size());
}

template <typename T>
constexpr ranges::iota_view<int, int> range(const vector<T> &v) {
    return ranges::iota_view<int, int>(0, (int)v.size());
}

constexpr int MAX_N = 40;
constexpr int CELL_COUNT = (MAX_N + 2) * (MAX_N + 2);

constexpr int UP = 0;
constexpr int DOWN = 1;
constexpr int LEFT = 2;
constexpr int RIGHT = 3;

struct World {
	int N;
	int ti;
	int tj;
	vector<vector<char>> b;

	void create() {
		cin >> N;
		cin >> ti;
		cin >> tj;
		b.resize(N);
		for (int i : range(N)) {
			b[i].resize(N);
			for (int j : range(N)) {
				cin >> b[i][j];
			}
		}
	}
};

struct Action {
	int n = 0;
	vector<pair<int, int>> yx;
};
struct Observation {
	int pi;
	int pj;
	int n;
	vector<pair<int, int>> yx;

	void read() {
		cin >> pi;
		cin >> pj;
		cin >> n;
		yx.resize(n);
		for (int i : range(n)) {
			cin >> yx[i].first >> yx[i].second;
		}
	}
};

inline string ActionString(const Action& action) {
	stringstream ss;
	if (action.n == -1) {
		ss << action.n;
	} else {
		ss << action.yx.size();
	}
	for (const auto& k : action.yx) {
		ss << " " << k.first << " " << k.second;
	}
	ss << endl;
	return ss.str();
}

constexpr int RotateLeft[] = { 2, 3, 1, 0 };
constexpr int RotateRight[] = { 3, 2, 0, 1 };

struct Pos {
	int x; 
	int y;

	auto operator<=>(const Pos&) const = default;

	Pos& operator+=(const Pos& p) {
		x += p.x;
		y += p.y;
		return *this;
	}

	Pos operator+(const Pos& p) const {
		return { x + p.x, y + p.y };
	}

	Pos operator-(const Pos& p) const {
		return { x - p.x, y - p.y };
	}

	static Pos fromPair(const pair<int, int>& p) {
		return { p.first, p.second };
	}

	int distance(const Pos& p) const {
		return abs(p.x - x) + abs(p.y - y);
	}

	Pos add(int dx, int dy, int dir) const {
		Pos p = *this;
		if (dir == RIGHT) {
			p.x += dx;
			p.y += dy;
		} else if (dir == LEFT) {
			p.x -= dx;
			p.y -= dy;
		} else if (dir == UP) {
			p.x += dy;
			p.y -= dx;
		} else {
			p.x -= dy;
			p.y += dx;
		}
		return p;
	}

	Pos add(int dx, int dy, int dir, bool reverse) const {
		Pos p = *this;
		if (dir == RIGHT) {
			p.x += dx;
			if (reverse) {
				p.y -= dy;
			} else {
				p.y += dy;
			}
		} else if (dir == LEFT) {
			p.x -= dx;
			if (reverse) {
				p.y += dy;
			} else {
				p.y -= dy;
			}
		} else if (dir == UP) {
			if (reverse) {
				p.x -= dy;
			} else {
				p.x += dy;
			}
			p.y -= dx;
		} else {
			if (reverse) {
				p.x += dy;
			} else {
				p.x -= dy;
			}
			p.y += dx;
		}
		return p;
	}

	static Pos dirPos(int dir) {
		if (dir == UP) {
			return Pos(0, -1);
		} else if (dir == DOWN) {
			return Pos(0, 1);
		} else if (dir == LEFT) {
			return Pos(-1, 0);
		} else {
			return Pos(1, 0);
		}
	}

	Pos addDir(int targetDir, int dir, bool reverse) const {
		Pos dirP = dirPos(targetDir);
		return add(dirP.x, dirP.y, dir, reverse);
	}

	bool isRight(Pos pos, int dir) const {
		if (dir == RIGHT) {
			return pos.x > x;
		} else if (dir == LEFT) {
			return pos.x < x;
		} else if (dir == UP) {
			return pos.y < y;
		} else {
			return pos.y > y;
		}
	}

	bool isLeft(Pos pos, int dir) const {
		if (dir == RIGHT) {
			return pos.x < x;
		} else if (dir == LEFT) {
			return pos.x > x;
		} else if (dir == UP) {
			return pos.y > y;
		} else {
			return pos.y < y;
		}
	}

	bool isDown(Pos pos, int dir, bool reverse) const {
		bool result;
		if (dir == RIGHT) {
			result = pos.y > y;
		} else if (dir == LEFT) {
			result = pos.y < y;
		} else if (dir == UP) {
			result = pos.x > x;
		} else {
			result = pos.x < x;
		}
		if (reverse) {
			return !result;
		} else {
			return result;
		}
	}

	bool isUp(Pos pos, int dir, bool reverse) const {
		bool result;
		if (dir == RIGHT) {
			result = pos.y < y;
		} else if (dir == LEFT) {
			result = pos.y > y;
		} else if (dir == UP) {
			result = pos.x < x;
		} else {
			result = pos.x > x;
		}
		if (reverse) {
			return !result;
		} else {
			return result;
		}
	}

	bool equalsX(Pos pos, int dir) const {
		if (dir == RIGHT || dir == LEFT) {
			return pos.x == x;
		} else {
			return pos.y == y;
		}
	}

	bool equalsY(Pos pos, int dir) const {
		if (dir == RIGHT || dir == LEFT) {
			return pos.y == y;
		} else {
			return pos.x == x;
		}
	}

};

template <typename T, int C>
struct StaticList {
	int c = 0;
	array<T, C> buf;

	void push_back(const T& t) {
		buf[c++] = t;
	}

	T& emplace_back() {
		return buf[c++];
	}

	void push_front(const T& t) {
		for (int i = c; i > 0; i--) {
			buf[i] = buf[i - 1];
		}
		buf[0] = t;
		c++;
	}

	void clear() {
		c = 0;
	}

	int size() const {
		return c;
	}

	void resize(int s) {
		c = s;
	}

	void remove(int index) {
		for (int i = index + 1; i < c; i++) {
			buf[i - 1] = buf[i];
		}
		c--;
	}

	void pop_back() {
		c--;
	}

	void pop_front() {
		remove(0);
	}

	T& back() {
		return buf[c - 1];
	}

	T* data() {
		return buf.data();
	}
	const T* data() const {
		return buf.data();
	}

	bool full() const {
		return c >= C;
	}

	T& operator[](int i) {
		return buf[i];
	}

	const T& operator[](int i) const {
		return buf[i];
	}

	auto begin() {
		return buf.begin();
	}
	auto end() {
		return buf.begin() + c;
	}
	auto begin() const {
		return buf.begin();
	}
	auto end() const {
		return buf.begin() + c;
	}
};

template<typename T, int C>
constexpr ranges::iota_view<int, int> range(const StaticList<T, C>& v) {
	return ranges::iota_view<int, int>(0, v.size());
}

template <typename T, int C>
struct Queue {
	int head = 0;
	int tail = 0;
	array<T, C> buf;

	void push_back(const T& t) {
		buf[tail++] = t;
	}

	T& emplace_back() {
		return buf[tail++];
	}

	void clear() {
		head = 0;
		tail = 0;
	}

	bool empty() const {
		return size() <= 0;
	}

	int size() const {
		return tail - head;
	}

	void pop_front() {
		head++;
	}

	T& operator[](int i) {
		return buf[head + i];
	}

	const T& operator[](int i) const {
		return buf[head + i];
	}

	auto begin() {
		return buf.begin() + head;
	}
	auto end() {
		return buf.begin() + tail;
	}
	auto begin() const {
		return buf.begin() + head;
	}
	auto end() const {
		return buf.begin() + tail;
	}
};

template<int C>
struct ArraySet {
  int version;
  array<unsigned char, C> flags;

  ArraySet() {
    init();
  }

  void init() {
    version = 1;
    flags = {};
  }

  void clear() {
    version++;
    if (version == 256) {
      init();
    }
  }

  void insert(int index) {
    auto& f = flags[index];
    if (f != version) {
      f = (unsigned char)version;
    }
  }

  bool contains(int index) const {
    return flags[index] == version;
  }

  bool containsAndInsert(int index) {
    auto& f = flags[index];
    if (f == version) {
      return true;
    } else {
      f = (unsigned char)version;
      return false;
    }
  }

  void erase(int index) {
    if (flags[index] == version) {
      flags[index] = 0;
    }
  }
};

constexpr array<Pos, 4> DirPos{
  Pos(0, -1), Pos(0, 1), Pos(-1, 0), Pos(1, 0),
};

struct DirReverse {
  int dir;
  bool reverse;
};

constexpr array<DirReverse, 8> BaseDirReverseOrder{
  DirReverse{RIGHT, false}, {RIGHT, true}, {DOWN, false}, {DOWN, true}, {LEFT, false}, {LEFT, true},{UP, false}, {UP, true}
};

constexpr array<DirReverse, 4> BaseDirOrder{
  DirReverse{RIGHT, false}, {DOWN, false}, {LEFT, false}, {UP, false}
};

struct StateCell {
  bool filled;
  bool known;

  void setOut() {
    filled = true;
    known = true;
  }

  bool filledAndKnown() const {
    return filled && known;
  }
};

struct State {
  vector<StateCell> cells;
  double score;
  Pos pos;
  Pos destination;
  int destinationIndex;
  int n;

  StateCell& get(int x, int y) {
    return cells[y * n + x];
  }

  StateCell& get(const Pos& p) {
    return cells[p.y * n + p.x];
  }

  bool& filled(const Pos& p) {
    return get(p).filled;
  }

  bool& filled(int x, int y) {
    return get(x, y).filled;
  }
};

constexpr int FILL = 0;
constexpr int ROAD = 1;
constexpr int FILL_TO_ROAD = 2;

struct Operation {
  int type;
  Pos pos;
};

struct SaveData {
  Pos pos;
  int historyIndex;
};

struct Cell {
  bool immortal;
  bool filled;
  bool road;
};

struct Cells : public StaticList<Cell, CELL_COUNT> {
  Pos pos;
  vector<Operation> history;
  vector<SaveData> savePoint;
};

struct Agent {
  World w;
  Observation obs;

  bool first = true;
  bool error = false;
  int n = 0;
  int tryCounter = 0;
  Pos goal = {};
  int goalIndex;
  Pos roadLastPos;
  Cells cells;
  State initialState;
  State currentState;
  State nextState;
  State best;
  array<int, 3> bestParam;
  array<int, 3> currentParam = {9, 1, 0};
  vector<vector<Pos>> destinationListCandidates;
  vector<Pos>* destinationList;
  vector<Pos> placeOrder;
  StaticList<Pos, 30> fillList;
  StaticList<Pos, 30> fillForceList;
  StaticList<Pos, 30> roadList;

  ArraySet<CELL_COUNT> doneSet;
  StaticList<int, CELL_COUNT> distanceMap; 
  Queue<Pos, CELL_COUNT> route; 
  ArraySet<CELL_COUNT> routeSet; 
  ArraySet<CELL_COUNT> fillSet;
  ArraySet<CELL_COUNT> connectedSet;
  ArraySet<CELL_COUNT> passed; 
  vector<Pos> toHistory;
  vector<Pos> preToHistory;
  mt19937 mt{ 1 }; 

  int index(int x, int y) {
    return y * n + x;
  }

  int index(const Pos& pos) {
    return pos.y * n + pos.x;
  }

  bool isOut(int x, int y) {
    return x <= 0 || y <= 0 || x >= n - 1 || y >= n - 1;
  }

  Action plan() {
    Action action;
    if (first) {
      bestParam = currentParam;
      n = w.N + 2;
      goal = { w.tj + 1, w.ti + 1 };
      goalIndex = index(goal);
      State& s = initialState;
      s.score = 0;
      s.n = n;
      s.pos = { obs.pj + 1, obs.pi + 1 };
      s.destination = { -1, -1 };
      s.cells.resize(n * n);
      for (int y = 0; y < n; y++) {
        s.get(0, y).setOut();
        s.get(n - 1, y).setOut();
      }
      for (int x = 0; x < n; x++) {
        s.get(x, 0).setOut();
        s.get(x, n - 1).setOut();
      }
      for (int y = 0; y < w.N; y++) {
        for (int x = 0; x < w.N; x++) {
          if (w.b[y][x] == 'T') {
            s.get(x + 1, y + 1).filled = true;
          }
        }
      }
      currentState = s;

      setDestinationList(25); 

      for (int i = w.N - 1; i >= 0; i--) {
        Pos p(i, 1);
        while (p.x > 0) {
          placeOrder.push_back(p);
          p += Pos(-1, 1);
        }
      }
      for (int i = 2; i < w.N; i++) {
        Pos p(i, w.N);
        while (p.y > 0) {
          placeOrder.push_back(p);
          p += Pos(1, -1);
        }
      }
    }

    currentState.pos = { obs.pj + 1, obs.pi + 1 };
    for (auto p : obs.yx) {
      currentState.get(p.second + 1, p.first + 1).known = true;
    }

    if (first) {
      best = currentState;
      best.score = 1;
      for (int k : range(1000000000)) {
        tryCounter = k;
        if (isTimeLimit()) {
          break;
        }
        if (create()) {
          nextState = currentState;
          for (int y = 1; y <= w.N; y++) {
            for (int x = 1; x <= w.N; x++) {
              if (!initialState.get(x, y).filled && cells[index(x, y)].filled) {
                nextState.filled(x, y) = true;
              }
            }
          }
          double score = simulation(nextState);
          if (best.score < score) {
            best = nextState;
            best.score = score;
            bestParam = currentParam;
          } else {
            currentParam = bestParam;
          }
        }
      }
      for (int y = 1; y <= w.N; y++) {
        for (int x = 1; x <= w.N; x++) {
          if (!initialState.get(x, y).filled && best.cells[index(x, y)].filled) {
            currentState.filled(x, y) = true;
            action.yx.push_back({ y - 1, x - 1 });
          }
        }
      }

      first = false;
    }

    
    return action;
  }

  bool isTimeLimit() {
    return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - startTime).count() >= TimeLimit;
  }

  void setDestinationList(int count) {
    vector<vector<int>> indicesList;
    indicesList.resize(4);
    for (int i : range(count)) {
      indicesList[i * indicesList.size() / count].push_back(i);
    }
    for (int i : range(indicesList)) {
      if (i % 2 == 1) {
        reverse(indicesList[i].begin(), indicesList[i].end());
      }
    }
    vector<int> indices;
    while ((int)indices.size() < count) {
      for (int i : range(indicesList)) {
        int idx;
        if (i % 2 == 0) {
          idx = i / 2;
        } else {
          idx = indicesList.size() - 1 - i / 2;
        }
        auto& list = indicesList[idx];
        if (list.size() > 0) {
          indices.push_back(list.back());
          list.pop_back();
        }
      }
    }

    int destCount = w.N * w.N - 1;
    destinationListCandidates.clear();
    destinationListCandidates.resize(count);
    for (int i : range(destinationListCandidates)) {
      auto& list = destinationListCandidates[i];
      int endIndex = destCount * (2 * indices[i] + 1) / (2 * count);
      for (int y = 1; y <= w.N; y++) {
        for (int x = 1; x <= w.N; x++) {
          Pos p(x, y);
          if (p == goal || p == initialState.pos) {
            continue;
          }
          list.push_back(p);
        }
      }
      shuffle(list.begin(), list.end(), mt);
      list.insert(list.begin() + endIndex, goal);
    }
  }

  bool canGoal() {
    StaticList<Pos, CELL_COUNT> posStack;
    posStack.push_back(goal);
    auto& done = doneSet;
    done.clear();
    done.insert(index(goal));
    while (posStack.size() > 0) {
      Pos pos = posStack.back();
      posStack.pop_back();
      for (Pos dirPos : DirPos) {
        Pos p = pos + dirPos;
        int i = index(p);
        if (cells[i].filled) {
          continue;
        }
        if (done.containsAndInsert(i)) {
          continue;
        }
        if (p == initialState.pos) {
          return true;
        }
        posStack.push_back(p);
      }
    }
    return false;
  }

  Cell& getCell(const Pos& pos) {
    return cells[index(pos)];
  }

  Cell& getCell(int x, int y) {
    return cells[index(x, y)];
  }

  bool inGrid(int x, int y) {
    return 0 < x && x <= w.N && 0 < y && y <= w.N;
  }

  bool inGrid(Pos p) {
    return inGrid(p.x, p.y);
  }

  bool filled(int x, int y) {
    if (inGrid(x, y)) {
      return cells[index(x, y)].filled;
    } else {
      return true;
    }
  }

  bool filled(const Pos& pos) {
    return filled(pos.x, pos.y);
  }

  bool filled(int dx, int dy, DirReverse dr) {
    Pos pos = cells.pos.add(dx, dy, dr.dir, dr.reverse);
    return filled(pos);
  }

  bool filledDir(int dir, DirReverse dr) {
    Pos pos = cells.pos.addDir(dir, dr.dir, dr.reverse);
    return filled(pos);
  }

  bool empty(int x, int y) {
    if (inGrid(x, y)) {
      Cell& c = getCell(x, y);
      return !c.filled && !c.road;
    } else {
      return false;
    }
  }

  bool empty(const Pos& pos) {
    return empty(pos.x, pos.y);
  }

  bool empty(int dx, int dy, DirReverse dr) {
    Pos pos = cells.pos.add(dx, dy, dr.dir, dr.reverse);
    return empty(pos);
  }

  bool emptyDir(int dir, DirReverse dr) {
    Pos pos = cells.pos.addDir(dir, dr.dir, dr.reverse);
    return empty(pos);
  }

  bool immortal(const Pos& pos) {
    if (inGrid(pos)) {
      return cells[index(pos)].immortal;
    } else {
      return true;
    }
  }

  bool immortal(int dx, int dy, DirReverse dr) {
    Pos pos = cells.pos.add(dx, dy, dr.dir, dr.reverse);
    return immortal(pos);
  }

  bool canFillToRoad(const Pos& pos) {
    if (inGrid(pos)) {
      Cell& c = getCell(pos);
      return c.filled && !c.immortal;
    } else {
      return false;
    }
  }

  bool road(int x, int y) {
    if (inGrid(x, y)) {
      Cell& c = getCell(x, y);
      return c.road;
    } else {
      return false;
    }
  }

  bool road(const Pos& pos) {
    return road(pos.x, pos.y);
  }

  bool road(int dx, int dy, DirReverse dr) {
    Pos pos = cells.pos.add(dx, dy, dr.dir, dr.reverse);
    return road(pos);
  }

  void setFill(const Pos& pos) {
    int i = index(pos);
    Cell& c = cells[i];
    if (!c.filled) {
      if (c.road || currentState.cells[i].known) {
        error = true;
      } else {
        cells.history.push_back({ FILL, pos });
        c.filled = true;
      }
    }
  }

  void setFill(int x, int y) {
    setFill(Pos(x, y));
  }

  void setFill(int dx, int dy, DirReverse dr) {
    Pos pos = cells.pos.add(dx, dy, dr.dir, dr.reverse);
    setFill(pos);
  }

  void setFillDir(int dir, DirReverse dr) {
    Pos pos = cells.pos.addDir(dir, dr.dir, dr.reverse);
    setFill(pos);
  }

  bool canFillForce(const Pos& pos) {
    return pos != goal && pos != currentState.pos;
  }

  bool canFillForce(int dx, int dy, DirReverse dr) {
    Pos pos = cells.pos.add(dx, dy, dr.dir, dr.reverse);
    return canFillForce(pos);
  }

  void setFillForce(const Pos& pos) {
    int i = index(pos);
    Cell& c = cells[i];
    if (!c.filled && pos != goal && !currentState.cells[i].known) {
      cells.history.push_back({ FILL, pos });
      c.filled = true;
      c.road = false;
    }
  }

  void setFillForce(int dx, int dy, DirReverse dr) {
    Pos pos = cells.pos.add(dx, dy, dr.dir, dr.reverse);
    setFillForce(pos);
  }

  void setFillIfEmpty(const Pos& pos) {
    if (empty(pos)) {
      setFill(pos);
    }
  }

  void setFillIfEmpty(int dx, int dy, DirReverse dr) {
    Pos pos = cells.pos.add(dx, dy, dr.dir, dr.reverse);
    setFillIfEmpty(pos);
  }

  void setFillIfEmptyD(int dx, int dy) {
    Pos p = cells.pos + Pos(dx, dy);
    setFillIfEmpty(p);
  }

  void setRoadOnly(const Pos& pos) {
    if (!road(pos)) {
      Cell& c = getCell(pos);
      if (c.filled) {
        error = true;
      } else {
        cells.history.push_back({ ROAD, pos });
        c.road = true;
      }
    }
  }

  void setRoadOnly(int x, int y) {
    setRoadOnly(Pos(x, y));
  }

  void setRoadOnly(int dx, int dy, DirReverse dr) {
    Pos pos = cells.pos.add(dx, dy, dr.dir, dr.reverse);
    setRoadOnly(pos);
  }

  void setFillAround() {
    for (Pos dirP : DirPos) {
      Pos p = cells.pos + dirP;
      Cell& c = getCell(p);
      if (!c.filled && !c.road) {
        cells.history.push_back({ FILL, p });
        c.filled = true;
        fillSet.insert(index(p));
      }
    }
  }

  void setRoad(const Pos& pos, bool fillAround = true) {
    setRoadOnly(pos);
    if (fillAround) {
      setFillAround();
    }
    cells.pos = pos;
  }

  void setRoad(int dx, int dy, DirReverse dr, bool fillAround = true) {
    Pos pos = cells.pos.add(dx, dy, dr.dir, dr.reverse);
    setRoad(pos, fillAround);
  }

  void setRoadDir(int dir, DirReverse dr, bool fillAround = true) {
    Pos pos = cells.pos.addDir(dir, dr.dir, dr.reverse);
    setRoad(pos, fillAround);
  }

  void setRoadIfEmpty(const Pos& pos, bool fillAround = true) {
    if (empty(pos)) {
      setRoad(pos, fillAround);
    } else if (fillAround) {
      cells.pos = pos;
    }
  }

  void fillToRoad(const Pos& pos, bool fillAround = true) {
    Cell& c = getCell(pos);
    cells.history.push_back({ FILL_TO_ROAD, pos });
    c.filled = false;
    c.road = true;
    if (fillAround) {
      setFillAround();
    }
    cells.pos = pos;
  }

  void fillToRoad(int dx, int dy, DirReverse dr) {
    Pos pos = cells.pos.add(dx, dy, dr.dir, dr.reverse);
    fillToRoad(pos, false);
  }

  int save() {
    int i = cells.savePoint.size();
    cells.savePoint.push_back({ cells.pos, (int)cells.history.size() });
    return i;
  }

  void load(int saveIndex) {
    SaveData data = cells.savePoint[saveIndex];
    cells.pos = data.pos;
    for (int i = (int)cells.history.size() - 1; i >= data.historyIndex; i--) {
      const Operation& o = cells.history[i];
      if (o.type == FILL) {
        cells[index(o.pos)].filled = false;
      } else if (o.type == ROAD) {
        cells[index(o.pos)].road = false;
      } else {
        cells[index(o.pos)].road = false;
        cells[index(o.pos)].filled = true;
      }
    }
    cells.history.resize(data.historyIndex);
    error = false;
  }

  bool isRight(Pos basePos, DirReverse dr) {
    return basePos.isRight(cells.pos, dr.dir);
  }

  bool isLeft(Pos basePos, DirReverse dr) {
    return basePos.isLeft(cells.pos, dr.dir);
  }

  bool isUp(Pos basePos, DirReverse dr) {
    return basePos.isUp(cells.pos, dr.dir, dr.reverse);
  }

  bool isDown(Pos basePos, DirReverse dr) {
    return basePos.isDown(cells.pos, dr.dir, dr.reverse);
  }

  bool equalsX(Pos pos, DirReverse dr) {
    return cells.pos.equalsX(pos, dr.dir);
  }

  bool equalsY(Pos pos, DirReverse dr) {
    return cells.pos.equalsY(pos, dr.dir);
  }

  int reversePos(int p) {
    return w.N - p + 1;
  }

  int axisDistance(Pos p0, Pos p1) {
    Pos d = p0 - p1;
    return min(abs(d.x + d.y), abs(d.x - d.y));
  }

  int posDir(Pos pos) {
    if (pos.x + pos.y <= w.N) {
      if (pos.x < pos.y) {
        return LEFT;
      } else {
        return UP;
      }
    } else {
      if (pos.x < pos.y) {
        return DOWN;
      } else {
        return RIGHT;
      }
    }
  }

  int neighborRoadCount(Pos pos) {
    int count = 0;
    for (Pos dirPos : DirPos) {
      if (road(pos + dirPos)) {
        count++;
      }
    }
    return count;
  }

  int neighborNotFilledCount(Pos pos) {
    int count = 0;
    for (Pos dirPos : DirPos) {
      if (!filled(pos + dirPos)) {
        count++;
      }
    }
    return count;
  }

  Pos nearEmpty(Pos pos) {
    if (empty(pos) && connectedSet.contains(index(pos))) {
      return pos;
    }
    for (int k = 1; k <= 10; k++) {
      for (DirReverse dr : BaseDirOrder) {
        for (int i = 0; i < k; i++) {
          Pos p = pos.add(k - i, -i, dr.dir);
          if (empty(p) && connectedSet.contains(index(p))) {
            return p;
          }
        }
      }
    }
    return { -1, -1 };
  }

  array<DirReverse, 8> order() {
    array<DirReverse, 8> ord = BaseDirReverseOrder;
    shuffle(ord.begin(), ord.end(), mt);
    return ord;
  }

  array<DirReverse, 4> orderDir() {
    array<DirReverse, 4> ord = BaseDirOrder;
    shuffle(ord.begin(), ord.end(), mt);
    return ord;
  }

  bool nextBool() {
    return mt() % 2;
  }

  bool create() {
    error = false;
    cells.resize(n * n);
    for (int i : range(currentState.cells)) {
      Cell& c = cells[i];
      c.filled = currentState.cells[i].filled;
      c.immortal = c.filled;
      c.road = false;
    }
    cells[index(currentState.pos)].road = true;
    cells[goalIndex].road = true;
    cells.history.clear();
    cells.savePoint.clear();

    int ti = tryCounter % 16;
    if (ti == 2 || ti == 10) {
      if (!opening2()) {
        return false;
      }
    } else if (ti == 3 || ti == 11) {
      if (!opening3(false)) {
        return false;
      }
    } else if (ti == 4 || ti == 12) {
      if (!opening4()) {
        return false;
      }
    } else if (ti == 5 || ti == 13) {
      if (!opening5()) {
        return false;
      }
    } else if (ti == 6 || ti == 14) {
      if (!opening6()) {
        return false;
      }
    }  else {
      if (!opening1() && !opening2() && !opening3(false) && !opening3(true) && !opening4() && !opening5() && !opening6() && !opening7()) {
        if (tryCounter < 100 || tryCounter % 8 != 0) {
          return false;
        }
      }
    }

    if (mt() % 3 == 0) {
      currentParam[0] = 5 + mt() % 10;
    }
    int lineCount = 1 + w.N / currentParam[0]; 
    int xy = cells.pos.x + cells.pos.y;
    int baseXy = w.N + 1;
    if (nextBool()) {
      currentParam[1] = -2 + mt() % 7;
    }
    int dBase = lineCount * 3 / 2 + currentParam[1]; 
    int targetXy;
    bool isDR = (xy < baseXy);
    if (isDR) {
      targetXy = baseXy - dBase;
    } else {
      targetXy = baseXy + dBase;
    }
    int dxy = targetXy - xy;
    if (nextBool()) {
      currentParam[2] = -2 + mt() % 5;
    }
    int dx = dxy / 2 + currentParam[2]; 
    Pos toPos = cells.pos + Pos(dx, dxy - dx);
    for (int i = 1; i <= 16; i++) {
      if (empty(toPos)) {
        break;
      }
      if (i % 2 == 0) {
        toPos += Pos(-i, i);
      } else {
        toPos += Pos(i, -i);
      }
    }
    if (!empty(toPos)) {
      return false;
    }
    toHistory.clear();
    if (!to(cells.pos, toPos)) {
      return false;
    }
    preToHistory = toHistory;

    if (!centralRoad(lineCount, isDR)) {
      return false;
    }

    alongToFill(currentState.pos, RIGHT, false);

    place1();
    if (mt() % 3 != 0) {
      place4();
    }
    place2();
    place3();
    place5();
    place6(); 
    place7();

    setDistanceMap(roadLastPos, goal, true, false);
    if (distanceMap[index(goal)] < 0) {
      return false;
    }
    updateRoute(roadLastPos, goal);
    routeSet.clear();
    for (int i : range(min(route.size(), 2 + (int)(mt() % 3)))) { 
      routeSet.insert(index(route[i]));
    }
    routeSet.insert(index(goal));

    Pos basePos;
    StaticList<Pos, 4> neighborPos;
    for (int i = 0; i < route.size() / 2; i++) {
      basePos = route[i + route.size() / 2];
      neighborPos.clear();
      for (Pos dirPos : DirPos) {
        if (!filled(basePos + dirPos)) {
          neighborPos.push_back(basePos + dirPos);
        }
      }
      if (neighborPos.size() == 2) {
        break;
      }
    }
    if (neighborPos.size() != 2) {
      return false;
    }

    for (int k : range(CELL_COUNT)) {
      auto& distMap = distanceMap;
      distMap.resize(n * n);
      fill(distMap.begin(), distMap.end(), -1);
      distMap[index(basePos)] = 0;
      auto& done = doneSet;
      done.clear();
      done.insert(index(basePos));
      array<Queue<Pos, CELL_COUNT>, 2> q;
      array<StaticList<Pos, CELL_COUNT>, 2> posList;
      for (int j : range(2)) {
        q[j].push_back(neighborPos[j]);
        done.insert(index(neighborPos[j]));
        distMap[index(neighborPos[j])] = 0;
      }
      while (q[0].size() + q[1].size() > 0) {
        for (int j : range(2)) {
          if (q[j].empty()) {
            continue;
          }
          Pos p = q[j][0];
          q[j].pop_front();
          posList[j].push_back(p);
          int distance = distMap[index(p)];
          for (const Pos& dirP : DirPos) {
            Pos p2 = p + dirP;
            int i = index(p2);
            if (cells[i].filled) {
              continue;
            }
            if (done.containsAndInsert(i)) {
              continue;
            }
            distMap[i] = distance + 1;
            q[j].push_back(p2);
          }
        }
      }

      auto tryConnect = [&]() {
        for (int j : range(2)) {
          auto& list = posList[j ^ (k % 2)];
          for (int m = list.size() - 1; m >= 0; m--) {
            Pos pos = list[m];
            if (routeSet.contains(index(pos))) {
              continue;
            }
            for (Pos dirPos : DirPos) {
              Pos p = pos + dirPos;
              if (neighborNotFilledCount(p) >= 3) {
                continue;
              }
              if (canFillToRoad(p)) {
                for (Pos dirPos2 : DirPos) {
                  Pos p2 = p + dirPos2;
                  if (!filled(p2) && distMap[index(p2)] < 0) {
                    fillToRoad(p, false);
                    setRoadIfEmpty(p2, false);
                    return true;
                  }
                }
              }
            }
          }
        }
        return false;
      };
      if (!tryConnect()) {
        break;
      }
    }

    if (error || !canGoal()) {
      return false;
    }
    return true;
  }

  void pushFill(int dx, int dy) {
    fillList.push_back(Pos(dx, dy));
  }

  void pushFillForce(int dx, int dy) {
    fillForceList.push_back(Pos(dx, dy));
  }

  void pushRoad(int dx, int dy) {
    roadList.push_back(Pos(dx, dy));
  }

  bool listCanPlace(Pos basePos) {
    for (Pos p : fillList) {
      if (road(basePos + p)) {
        return false;
      }
    }
    for (Pos p : fillForceList) {
      if (!canFillForce(basePos + p)) {
        return false;
      }
    }
    for (Pos p : roadList) {
      if (!empty(basePos + p)) {
        return false;
      }
    }
    return true;
  }

  bool listCanPlace(Pos basePos, DirReverse dr) {
    cells.pos = basePos;
    for (Pos p : fillList) {
      if (road(p.x, p.y, dr)) {
        return false;
      }
    }
    for (Pos p : fillForceList) {
      if (!canFillForce(p.x, p.y, dr)) {
        return false;
      }
    }
    for (Pos p : roadList) {
      if (!empty(p.x, p.y, dr)) {
        return false;
      }
    }
    return true;
  }

  void listPlace(Pos basePos) {
    for (Pos p : fillList) {
      setFillIfEmpty(basePos + p);
    }
    for (Pos p : fillForceList) {
      setFillForce(basePos + p);
    }
    for (Pos p : roadList) {
      setRoadOnly(basePos + p);
    }
  }

  void listPlace(Pos basePos, DirReverse dr) {
    cells.pos = basePos;
    for (Pos p : fillList) {
      setFillIfEmpty(p.x, p.y, dr);
    }
    for (Pos p : fillForceList) {
      setFillForce(p.x, p.y, dr);
    }
    for (Pos p : roadList) {
      setRoadOnly(p.x, p.y, dr);
    }
  }

  void placeStart() {
    fillList.clear();
    fillForceList.clear();
    roadList.clear();
  }

  void place1() {
    placeStart();
    pushFill(2, -1);
    pushFill(3, 0);
    pushFill(1, 1);
    pushFill(4, 1);
    pushFill(-1, 2);
    pushFill(2, 2);
    pushFill(3, 2);
    pushFill(0, 3);
    pushFill(2, 3);
    pushFill(1, 4);
    pushRoad(0, 0);
    pushRoad(1, 0);
    pushRoad(2, 0);
    pushRoad(0, 1);
    pushRoad(0, 2);
    pushRoad(2, 1);
    pushRoad(1, 2);

    auto canPlace = [&](Pos pos) {
      if (pos.x <= 0 || pos.x >= w.N - 2 || pos.y <= 0 || pos.y >= w.N - 2) {
        return false;
      }
      if (!listCanPlace(pos)) {
        return false;
      }
      return (!filled(pos.x + 1, pos.y - 2) || !filled(pos.x - 2, pos.y + 1)) && (!filled(pos.x + 3, pos.y + 3) || !filled(pos.x + 2, pos.y + 4) || !filled(pos.x + 4, pos.y + 2));
    };
    for (int k : range(2)) {
      for (Pos p : placeOrder) {
        if (canPlace(p)) {
          if (k == 0) {
            if (canPlace(p + Pos(-3, 3))) {
              listPlace(p);
              listPlace(p + Pos(-3, 3));
              setRoadIfEmpty(p + Pos(-1, 1), false);
              setRoadIfEmpty(p + Pos(-2, 1), false);
              setRoadIfEmpty(p + Pos(-2, 2), false);
            }
          } else {
            listPlace(p);
          }
        }
      }
    }
  }

  void place2() {
    placeStart();
    pushFill(0, 1);
    pushFill(1, 2);
    pushFill(2, 0);
    pushFill(2, 3);
    pushFill(3, 1);
    pushFill(3, 2);
    pushRoad(1, 0);
    pushRoad(1, 1);
    pushRoad(2, 1);
    pushRoad(2, 2);

    for (auto dr : order()) {
      for (Pos p : placeOrder) {
        if (!listCanPlace(p, dr)) {
          continue;
        }
        if (!filled(0, 0, dr) || !filled(1, -1, dr)) {
          listPlace(p, dr);
        }
      }
    }
  }

  void place3() {
    placeStart();
    pushFill(1, 0);

    for (auto dr : order()) {
      for (Pos p : placeOrder) {
        if (!listCanPlace(p, dr)) {
          continue;
        }
        if (filled(1, -1, dr) && !filled(0, 0, dr) && !filled(2, 0, dr) && !filled(0, 1, dr) && !filled(1, 1, dr) && !filled(2, 1, dr)) {
          listPlace(p, dr);
        }
      }
    }
  }

  void place4() {
    placeStart();
    pushFill(3, 0);
    pushFill(1, 1);
    pushFill(4, 1);
    pushFill(2, 2);
    pushFill(3, 2);
    pushFill(0, 3);
    pushFill(2, 3);
    pushFill(1, 4);
    pushRoad(0, 0);
    pushRoad(1, 0);
    pushRoad(2, 0);
    pushRoad(0, 1);
    pushRoad(0, 2);
    pushRoad(2, 1);
    pushRoad(1, 2);

    for (auto dr : orderDir()) {
      for (int k : range(2)) {
        for (Pos p : placeOrder) {
          if (listCanPlace(p, dr)) {
            if (k == 0) {
              if (listCanPlace(p + Pos(-3, 3), dr)) {
                listPlace(p, dr);
                listPlace(p + Pos(-3, 3), dr);
              }
            } else {
              listPlace(p, dr);
            }
          }
        }
      }
    }
  }

  void place5() {
    placeStart();
    pushFillForce(0, 0);

    for (auto dr : orderDir()) {
      for (Pos p : placeOrder) {
        if (!listCanPlace(p, dr)) {
          continue;
        }
        if (!filled(0, -1, dr) && !filled(1, -1, dr) && !filled(1, 0, dr) && !filled(1, 1, dr) && !filled(0, 1, dr) && !filled(-1, 1, dr) && !filled(-1, 0, dr)) {
          listPlace(p, dr);
        }
      }
    }
  }

  void place6() {
    placeStart();
    pushFillForce(0, 0);

    for (auto dr : orderDir()) {
      for (Pos p : placeOrder) {
        if (!listCanPlace(p, dr)) {
          continue;
        }
        if (filled(0, -1, dr) && filled(-1, 0, dr) && !filled(1, 0, dr) && !filled(1, 1, dr) && !filled(0, 1, dr)) {
          listPlace(p, dr);
        }
      }
    }
  }

  void place7() {
    placeStart();
    pushFill(0, 0);

    for (auto dr : orderDir()) {
      for (Pos p : placeOrder) {
        if (!listCanPlace(p, dr)) {
          continue;
        }
        if (filled(0, -1, dr) && filled(-1, 0, dr) && filled(1, 0, dr) && !filled(-1, 1, dr) && !filled(0, 1, dr) && !filled(1, 1, dr) && (!filled(-1, -1, dr) || !filled(1, -1, dr) || !filled(0, -2, dr))) {
          listPlace(p, dr);
        }
      }
    }
  }

  

  bool centralRoad(int lineCount, bool isDR) {
    int si = save();
    int lastK = (tryCounter >= 50 ? 4 : 3);
    array<bool, 2> urOrder = { false, true }; 
    if (isDR && mt() % 2 == 0) {
      urOrder = { true, false };
    }
    for (int k = 0; k < lastK; k++) {
      for (bool isUR : urOrder) {
        bool ur = isUR;
        toHistory = preToHistory;
        for (int i : range(lineCount - max(0, k - 1))) {
          Pos toPos = cells.pos;
          int dir = 0;
          if (i != 0) {
            dir = posDir(toPos);
            int d = 2 + k;
            if (dir == UP || dir == DOWN) {
              if (isDR) {
                toPos += Pos(d, 0);
                toPos.x = min(toPos.x, w.N);
                setFillIfEmptyD(-1, 0);
              } else {
                toPos += Pos(-d, 0);
                toPos.x = max(toPos.x, 0);
                setFillIfEmptyD(1, 0);
              }
            } else {
              if (isDR) {
                toPos += Pos(0, d);
                toPos.y = min(toPos.y, w.N);
                setFillIfEmptyD(0, -1);
              } else {
                toPos += Pos(0, -d);
                toPos.y = max(toPos.y, 0);
                setFillIfEmptyD(0, 1);
              }
            }
          }
          setConnected(cells.pos);
          if (k >= 1 && !connectedSet.contains(index(toPos)) && !immortal(toPos) && toPos != goal) {
            if (!to(cells.pos, toPos, true)) {
              load(si);
              break;
            }
          }
          Pos dp;
          if (ur) {
            dp = Pos(1, -1);
            while (toPos.x < w.N && toPos.y > 1) {
              toPos += dp;
            }
          } else {
            dp = Pos(-1, 1);
            while (toPos.x > 1 && toPos.y < w.N) {
              toPos += dp;
            }
          }
          dp.x = -dp.x;
          dp.y = -dp.y;
          if (k >= 2) {
            for (int j = 0; j < 20; j++) {
              if (connectedSet.contains(index(toPos))) {
                break;
              }
              toPos += dp;
              if (!inGrid(toPos)) {
                break;
              }
            }
          }
          for (int j = 0; j < k; j++) {
            toPos += dp;
          }
          Pos pre = toPos;
          toPos = nearEmpty(pre);
          if (toPos.x < 0) {
            auto tryConnect = [&]() {
              for (int m = 0; m < 5; m++) {
                for (int ix : range(min(30, (int)toHistory.size()))) {
                  Pos pos = toHistory[(int)toHistory.size() - 1 - ix];
                  for (Pos dirPos : DirPos) {
                    Pos p = pos + dirPos;
                    if (neighborRoadCount(p) >= 2) {
                      continue;
                    }
                    if (canFillToRoad(p)) {
                      for (Pos dirPos2 : DirPos) {
                        Pos p2 = p + dirPos2;
                        if (empty(p2) && !connectedSet.contains(index(p2))) {
                          cells.pos = pos;
                          fillToRoad(p);
                          setConnected(cells.pos);
                          toPos = nearEmpty(pre);
                          if (toPos.x >= 0) {
                            return true;
                          }
                        }
                      }
                    }
                  }
                }
              }
              return false;
            };
            if (!tryConnect()) {
              load(si);
              break;
            }
          }
          if (!connectedSet.contains(index(toPos))) {
            load(si);
            break;
          }
          toHistory.clear();
          if (i >= 2) {
            int si2 = save();
            bool right = (ur != isDR);
            if (!alongTo(cells.pos, toPos, dir, right)) {
              load(si2);
              if (!alongTo(cells.pos, toPos, RotateRight[dir], right)) {
                load(si2);
                if (!alongTo(cells.pos, toPos, RotateLeft[dir], right)) {
                  load(si);
                  break;
                }
              }
            }
          } else {
            if (!to(cells.pos, toPos)) {
              load(si);
              break;
            }
          }

          ur = !ur;
          if (i == lineCount - 1) {
            if (toHistory.size() > 0) {
              roadLastPos = toHistory.back();
            } else {
              roadLastPos = { 1, 1 };
            }
            return true;
          }
        }
      }
    }
    return false;
  }

  bool opening1() {
    cells.pos = goal;
    int si = save();
    for (auto dr : order()) {
      if (empty(1, 0, dr) && empty(1, -1, dr)) {
        setRoadDir(RIGHT, dr);
        setRoadDir(UP, dr);

        constexpr int MaxCount = 30;
        int dir = UP;
        for (int k : range(MaxCount)) {
          if (emptyDir(RotateLeft[dir], dr)) {
            dir = RotateLeft[dir];
            setRoadDir(dir, dr);
          } else if (emptyDir(dir, dr)) {
            setRoadDir(dir, dr);
          } else if (emptyDir(RotateRight[dir], dr)) {
            dir = RotateRight[dir];
            setRoadDir(dir, dr);
          } else {
            error = true;
            break;
          }
          if (isRight(goal, dr) && isDown(goal, dr)) {
            if (empty(1, -1, dr) && empty(1, 0, dr)) {
              setRoadDir(RIGHT, dr);
            }
            if (empty(0, -1, dr) && empty(1, -1, dr)) {
              setRoadDir(UP, dr);
            }
            break;
          }
          if (k == MaxCount - 1) {
            error = true;
          }
        }

        if (!error) {
          error = !canGoal();
        }
        if (error) {
          load(si);
        } else {
          return true;
        }
      }
    }
    return false;
  }

  bool opening2() {
    cells.pos = goal;
    int si = save();
    for (auto dr : order()) {
      if (empty(1, 0, dr) && empty(1, -1, dr) && empty(2, -1, dr)) {
        setRoadDir(RIGHT, dr);
        setRoadDir(UP, dr);
        setRoadDir(RIGHT, dr);

        constexpr int MaxCount = 30;
        int dir = RIGHT;
        for (int k : range(MaxCount)) {
          if (emptyDir(RotateRight[dir], dr)) {
            dir = RotateRight[dir];
            setRoadDir(dir, dr);
          } else if (emptyDir(dir, dr)) {
            setRoadDir(dir, dr);
          } else if (emptyDir(RotateLeft[dir], dr)) {
            dir = RotateLeft[dir];
            setRoadDir(dir, dr);
          } else {
            error = true;
            break;
          }
          if (isLeft(goal, dr) && isDown(goal, dr)) {
            if (empty(0, -1, dr) && empty(-1, -1, dr)) {
              setRoadDir(UP, dr);
              setRoadDir(LEFT, dr);
            }
            break;
          }
          if (k == MaxCount - 1) {
            error = true;
          }
        }

        if (!error) {
          error = !canGoal();
        }
        if (error) {
          load(si);
        } else {
          return true;
        }
      }
    }
    return false;
  }

  bool opening3(bool second) {
    cells.pos = goal;
    int si = save();
    for (auto dr : order()) {
      if (immortal(-1, 0, dr) && immortal(-1, -1, dr) && empty(0, -1, dr) && empty(1, -1, dr) && empty(1, -2, dr)) {
        setRoadDir(UP, dr);
        setRoadDir(RIGHT, dr);
        setRoadDir(UP, dr);

        constexpr int MaxCount = 30;
        int dir = UP;
        for (int k : range(MaxCount)) {
          if (emptyDir(RotateRight[dir], dr)) {
            dir = RotateRight[dir];
            setRoadDir(dir, dr);
          } else if (emptyDir(dir, dr)) {
            setRoadDir(dir, dr);
          } else if (emptyDir(RotateLeft[dir], dr)) {
            dir = RotateLeft[dir];
            setRoadDir(dir, dr);
          } else {
            error = true;
            break;
          }
          if (second) {
            if (isRight(goal, dr) && equalsY(goal, dr)) {
              break;
            }
          } else {
            if (isRight(goal, dr) && isDown(goal, dr)) {
              if (empty(-1, 0, dr) && empty(-1, 1, dr) && empty(-1, 2, dr)) {
                setRoadDir(LEFT, dr);
                setRoadDir(DOWN, dr);
              }
              break;
            }
          }
          if (k == MaxCount - 1) {
            error = true;
          }
        }

        if (!error) {
          error = !canGoal();
        }
        if (error) {
          load(si);
        } else {
          return true;
        }
      }
    }
    return false;
  }

  bool opening4() {
    cells.pos = goal;
    int si = save();
    for (auto dr : order()) {
      if (immortal(-1, 0, dr) && immortal(-1, -1, dr) && empty(1, 0, dr) && empty(1, 1, dr) && empty(2, 1, dr) && empty(3, 1, dr)) {
        setRoadDir(RIGHT, dr);
        setRoadDir(DOWN, dr);
        setRoadDir(RIGHT, dr);
        setRoadDir(RIGHT, dr);

        constexpr int MaxCount = 30;
        int dir = UP;
        for (int k : range(MaxCount)) {
          if (emptyDir(RotateLeft[dir], dr)) {
            dir = RotateLeft[dir];
            setRoadDir(dir, dr);
          } else if (emptyDir(dir, dr)) {
            setRoadDir(dir, dr);
          } else if (emptyDir(RotateRight[dir], dr)) {
            dir = RotateRight[dir];
            setRoadDir(dir, dr);
          } else {
            error = true;
            break;
          }
          if (isUp(goal, dr) && equalsX(goal, dr)) {
            break;
          }
          if (k == MaxCount - 1) {
            error = true;
          }
        }

        if (!error) {
          error = !canGoal();
        }
        if (error) {
          load(si);
        } else {
          return true;
        }
      }
    }
    return false;
  }

  bool opening5() {
    cells.pos = goal;
    int si = save();
    for (auto dr : order()) {
      if (immortal(-1, 0, dr) && immortal(0, 1, dr) && immortal(1, 1, dr) && empty(1, 0, dr) && empty(2, 0, dr) && empty(2, -1, dr)) {
        setRoadDir(RIGHT, dr);
        setRoadDir(RIGHT, dr);
        setRoadDir(UP, dr);

        constexpr int MaxCount = 30;
        int dir = UP;
        for (int k : range(MaxCount)) {
          if (emptyDir(RotateLeft[dir], dr)) {
            dir = RotateLeft[dir];
            setRoadDir(dir, dr);
          } else if (emptyDir(dir, dr)) {
            setRoadDir(dir, dr);
          } else if (emptyDir(RotateRight[dir], dr)) {
            dir = RotateRight[dir];
            setRoadDir(dir, dr);
          } else {
            error = true;
            break;
          }
          if (isUp(goal, dr) && equalsX(goal, dr)) {
            break;
          }
          if (k == MaxCount - 1) {
            error = true;
          }
        }

        if (!error) {
          error = !canGoal();
        }
        if (error) {
          load(si);
        } else {
          return true;
        }
      }
    }
    return false;
  }

  bool opening6() {
    cells.pos = goal;
    int si = save();
    for (auto dr : order()) {
      if (empty(1, 0, dr) && empty(1, -1, dr)) {
        setRoadDir(RIGHT, dr);
        setRoadDir(UP, dr);

        if (!error) {
          error = !canGoal();
        }
        if (error) {
          load(si);
        } else {
          return true;
        }
      }
    }
    return false;
  }

  bool opening7() {
    cells.pos = goal;
    int si = save();
    for (auto dr : order()) {
      if (immortal(-1, 0, dr) && immortal(-1, -1, dr) && immortal(1, -1, dr) && immortal(1, 1, dr) && empty(1, 0, dr) && empty(2, 0, dr)) {
        setRoadDir(RIGHT, dr);
        setRoadDir(RIGHT, dr);

        if (!error) {
          error = !canGoal();
        }
        if (error) {
          load(si);
        } else {
          return true;
        }
      }
    }
    return false;
  }

  bool to(Pos fromPos, Pos toPos, bool force = false) {
    if (!setDistanceMap(toPos, fromPos, false, force)) {
      return false;
    }
    cells.pos = fromPos;
    int preDir = -1;
    int preAd = axisDistance(cells.pos, toPos);
    for (int d = distanceMap[index(fromPos)] - 1; d >= 0; d--) {
      int candidateDir = -1;
      for (int dir : range(4)) {
        Pos dirP = DirPos[dir];
        Pos p = cells.pos + dirP;
        if (distanceMap[index(p)] == d) {
          if (axisDistance(p, toPos) < preAd) {
            candidateDir = dir;
            break;
          }
          if (dir != preDir) {
            candidateDir = dir;
          } else if (candidateDir == -1) {
            candidateDir = dir;
          }
        }
      }
      Pos p = cells.pos + DirPos[candidateDir];
      if (force && filled(p)) {
        fillToRoad(p);
      } else {
        setRoad(p);
      }
      toHistory.push_back(p);
      preDir = candidateDir;
      preAd = axisDistance(cells.pos, toPos);
    }
    return true;
  }

  bool alongToFill(Pos fromPos, int preDir, bool right) {
    fillSet.clear();
    cells.pos = fromPos;
    int count = w.N * 5;
    for (int i = 0; i < count; i++) {
      int newDir = right ? RotateRight[preDir] : RotateLeft[preDir];
      Pos p = cells.pos + DirPos[newDir];
      if (!filled(p)) {
        setRoadIfEmpty(p);
        preDir = newDir;
        continue;
      }
      if (fillSet.contains(index(p))) {
        fillToRoad(p);
        preDir = newDir;
        continue;
      }
      if (!immortal(p)) {
        fillToRoad(p);
        return true;
      }

      p = cells.pos + DirPos[preDir];
      if (!filled(p)) {
        setRoadIfEmpty(p);
        continue;
      }
      if (fillSet.contains(index(p))) {
        fillToRoad(p);
        continue;
      }
      if (!immortal(p)) {
        fillToRoad(p);
        return true;
      }
      preDir ^= 1;
    }
    return false;
  }

  bool alongTo(Pos fromPos, Pos toPos, int preDir, bool right) {
    fillSet.clear();
    cells.pos = fromPos;
    int counter = 0;
    while (toPos.distance(cells.pos) > 2 && toPos.x != cells.pos.x && toPos.y != cells.pos.y) {
      counter++;
      if (counter >= w.N * 8) {
        return false;
      }
      int newDir = right ? RotateRight[preDir] : RotateLeft[preDir];
      Pos p = cells.pos + DirPos[newDir];
      if (!filled(p)) {
        setRoadIfEmpty(p);
        toHistory.push_back(p);
        preDir = newDir;
        continue;
      }
      if (fillSet.contains(index(p))) {
        fillToRoad(p);
        toHistory.push_back(p);
        preDir = newDir;
        continue;
      }

      p = cells.pos + DirPos[preDir];
      if (!filled(p)) {
        setRoadIfEmpty(p);
        toHistory.push_back(p);
        continue;
      }
      if (fillSet.contains(index(p))) {
        fillToRoad(p);
        toHistory.push_back(p);
        continue;
      }
      preDir ^= 1;
    }

    if (toPos.distance(cells.pos) == 2) {
      int newDir = right ? RotateRight[preDir] : RotateLeft[preDir];
      Pos p = cells.pos + DirPos[newDir];
      if (!filled(p) && toPos.distance(p) == 1) {
        setRoadIfEmpty(p);
        toHistory.push_back(p);
      } else {
        p = cells.pos + DirPos[preDir];
        if (!filled(p) && toPos.distance(p) == 1) {
          setRoadIfEmpty(p);
          toHistory.push_back(p);
        } else {
          preDir = (newDir ^ 1);
          p = cells.pos + DirPos[preDir];
          if (!filled(p) && toPos.distance(p) == 1) {
            setRoadIfEmpty(p);
            toHistory.push_back(p);
          } else {
            return false;
          }
        }
      }

      setRoadIfEmpty(toPos);
      toHistory.push_back(p);
    } else {
      int si = save();
      if (!to(cells.pos, toPos)) {
        load(si);
        return to(cells.pos, toPos, true);
      }
    }
    return true;
  }

  double simulation(State& s) {
    int turnSum = 0;
    int simulationCount = (int)destinationListCandidates.size();
    for (int i : range(simulationCount)) {
      if (isTimeLimit()) {
        return 0;
      }
      destinationList = &destinationListCandidates[i];
      turnSum += simulationSingle(s);
      if (3 <= i && i < simulationCount - 1) {
        constexpr double firstRate = 0.9;
        double currentScore = turnSum / (double)(i + 1);
        double doneRate = (i + 1) / (double)simulationCount;
        if (currentScore < best.score * (firstRate + doneRate * (1.0 - firstRate))) {
          return 0;
        }
      }
    }
    return turnSum / (double)simulationCount; 
  }

  int simulationSingle(State& state) {
    State s = state;
    s.destinationIndex = 0; 
    s.destination = destinationList->at(s.destinationIndex);

    passed.clear();
    updateDistanceMap(s); 
    int turn = 0;
    while (s.pos != goal) {
      step(s, turn);
      turn++;
    }
    return turn;
  }

  void step(State& s, int turn) {
    bool updated = false; 
    if (!passed.containsAndInsert(index(s.pos))) {
      bool goalFound = false;
      bool destinationFound = false;
      for (const Pos& dirP : DirPos) {
        Pos p = s.pos;
        while (true) {
          p += dirP;
          StateCell& c = s.get(p);
          if (!c.known) {
            c.known = true;
            if (p == goal) {
              goalFound = true; 
            } else if (p == s.destination) {
              destinationFound = true; 
            }
            if (c.filled) {
              if (routeSet.contains(index(p))) {
                updated = true;
              }
              break;
            }
          }
          if (c.filled) {
            break;
          }
        }
      }

      if (goalFound) {
        s.destination = goal;
        updated = true;
      } else if (destinationFound) {
        s.destination = { -1, -1 };
      }
    }

    if (s.destination.x == -1) {
      updated = true;
      do {
        s.destinationIndex++;
        s.destination = destinationList->at(s.destinationIndex);
      } while (s.get(s.destination).known);
    }

    if (updated) {
      int maxD = -1;
      if (turn < w.N * w.N) {
        maxD = maxDistance(s);
      }
      bool arrive = true;
      if (maxD < 0) {
        arrive = updateDistanceMap(s);
      } else {
        if (route.size() == 0) {
          arrive = updateDistanceMapMax(s, maxD);
        }
      }
      if (!arrive) {
        s.destination = { -1, -1 };
        step(s, turn);
        return;
      }
    }

    s.pos = route[0];
    route.pop_front();
  }

  int maxDistance(State& s) {
    Pos startPos = s.pos;
    Pos goalPos = s.destination;
    route.clear();
    Pos current = startPos;
    int d = startPos.distance(goalPos);
    int last = d + 4;
    int result = -1;
    for (int k = 0; k < last; k++) {
      if (current == goalPos) {
        result = k;
        break;
      }
      Pos p = current;
      if (current.y != goalPos.y) {
        if (current.y < goalPos.y) {
          p.y++;
        } else {
          p.y--;
        }
        if (!s.cells[index(p)].filledAndKnown()) {
          current = p;
          continue;
        }
        p = current;
      }
      if (current.x != goalPos.x) {
        if (current.x < goalPos.x) {
          p.x++;
        } else {
          p.x--;
        }
        if (!s.cells[index(p)].filledAndKnown()) {
          current = p;
          continue;
        }
        p = current;
      }
      if (current.y == goalPos.y) {
        p.y--;
        if (!s.cells[index(p)].filledAndKnown()) {
          current = p;
          continue;
        }
        p.y += 2;
        if (!s.cells[index(p)].filledAndKnown()) {
          current = p;
          continue;
        }
        p = current;
      }
      if (current.x == goalPos.x) {
        p.x--;
        if (!s.cells[index(p)].filledAndKnown()) {
          current = p;
          continue;
        }
        p.x += 2;
        if (!s.cells[index(p)].filledAndKnown()) {
          current = p;
          continue;
        }
      }
      return result;
    }
    if (result == d) {
      routeSet.clear();
      current = startPos;
      for (int k = 0; k < d; k++) {
        Pos p = current;
        if (current.y != goalPos.y) {
          if (current.y < goalPos.y) {
            p.y++;
          } else {
            p.y--;
          }
          if (!s.cells[index(p)].filledAndKnown()) {
            current = p;
            route.push_back(current);
            routeSet.insert(index(current));
            continue;
          }
        }
        if (current.x < goalPos.x) {
          current.x++;
        } else {
          current.x--;
        }
        route.push_back(current);
        routeSet.insert(index(current));
      }
    }
    return result;
  }

  bool updateDistanceMap(State& s) {
    auto& distMap = distanceMap;
    distMap.resize(n * n);
    fill(distMap.begin(), distMap.end(), -1);
    distMap[index(s.destination)] = 0;
    Queue<Pos, CELL_COUNT> q;
    q.push_back(s.destination);
    auto& done = doneSet;
    done.clear();
    done.insert(index(s.destination));
    while (!q.empty()) {
      Pos p = q[0];
      q.pop_front();
      int distance = distMap[index(p)];
      for (const Pos& dirP : DirPos) {
        Pos p2 = p + dirP;
        int i = index(p2);
        if (s.cells[i].filledAndKnown()) {
          continue;
        }
        if (done.containsAndInsert(i)) {
          continue;
        }
        distMap[i] = distance + 1;
        if (p2 == s.pos) {
          updateRoute(s.destination, s.pos);
          return true;
        }
        q.push_back(p2);
      }
    }
    return false;
  }

  bool updateDistanceMapMax(State& s, int maxDistance) {
    auto& distMap = distanceMap;
    distMap.resize(n * n);
    fill(distMap.begin(), distMap.end(), -1);
    distMap[index(s.destination)] = 0;
    Queue<Pos, CELL_COUNT> q;
    q.push_back(s.destination);
    auto& done = doneSet;
    done.clear();
    done.insert(index(s.destination));
    while (!q.empty()) {
      Pos p = q[0];
      q.pop_front();
      int distance = distMap[index(p)];
      for (const Pos& dirP : DirPos) {
        Pos p2 = p + dirP;
        int i = index(p2);
        if (s.cells[i].filledAndKnown()) {
          continue;
        }
        if (done.containsAndInsert(i)) {
          continue;
        }
        distMap[i] = distance + 1;
        if (distMap[i] + p2.distance(s.pos) > maxDistance) {
          continue; 
        }
        if (p2 == s.pos) {
          updateRoute(s.destination, s.pos);
          return true;
        }
        q.push_back(p2);
      }
    }
    return false;
  }

  void updateRoute(Pos fromPos, Pos toPos) {
    route.clear();
    routeSet.clear();
    int distance = distanceMap[index(toPos)];
    Pos currentPos = toPos;
    while (currentPos != fromPos) {
      for (int dir = 0; dir < 4; dir++) {
        Pos p = currentPos + DirPos[dir];
        int i = index(p);
        int d = distanceMap[i];
        if (d == distance - 1) {
          currentPos = p;
          distance = d;
          route.push_back(p);
          routeSet.insert(i);
          break;
        }
      }
    }
  }

  bool setDistanceMap(Pos fromPos, Pos toPos, bool canRoad, bool force) {
    auto& distMap = distanceMap;
    distMap.resize(n * n);
    fill(distMap.begin(), distMap.end(), -1);
    distMap[index(fromPos)] = 0;
    Queue<Pos, CELL_COUNT> q;
    q.push_back(fromPos);
    auto& done = doneSet;
    done.clear();
    done.insert(index(fromPos));
    while (!q.empty()) {
      Pos p = q[0];
      q.pop_front();
      int distance = distMap[index(p)];
      for (const Pos& dirP : DirPos) {
        Pos p2 = p + dirP;
        int i = index(p2);
        Cell& c = cells[i];
        if (p2 == toPos) {
          distMap[i] = distance + 1;
          return true;
        }
        if (force) {
          if (c.immortal) {
            continue;
          }
        } else if (canRoad) {
          if (c.filled) {
            continue;
          }
        } else {
          if (c.filled || c.road) {
            continue;
          }
        }

        if (done.containsAndInsert(i)) {
          continue;
        }
        distMap[i] = distance + 1;
        q.push_back(p2);
      }
    }
    return false;
  }

  void setConnected(Pos fromPos) {
    Queue<Pos, CELL_COUNT> q;
    q.push_back(fromPos);
    auto& done = connectedSet;
    done.clear();
    done.insert(index(fromPos));
    while (!q.empty()) {
      Pos p = q[0];
      q.pop_front();
      for (const Pos& dirP : DirPos) {
        Pos p2 = p + dirP;
        int i = index(p2);
        Cell& c = cells[i];
        if (c.filled || c.road) {
          continue;
        }
        if (done.containsAndInsert(i)) {
          continue;
        }
        q.push_back(p2);
      }
    }
  }
};


int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	Agent* agent = new Agent();
	World& w = agent->w;
	w.create();

	Observation& obs = agent->obs;
	obs.read();

	Action action = agent->plan();
	cout << ActionString(action) << flush;

	obs.read();
	cout << "-1" << endl;

	quick_exit(0);
}
