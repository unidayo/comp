#pragma GCC optimize "-O3,omit-frame-pointer,inline,unroll-all-loops,fast-math"
#pragma GCC target "tune=native"

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

bool isTopCoder = false;

// -------------------------------------------------------------- #1 BOLIERPLATE ------------------------------
#define F0(i, n) for (int i = 0; i < n; i++)
#define FR(i, n) for (int i = n - 1; i >= 0; i--)
#define F1(i, n) for (int i = 1; i <= n; i++)
#define FI(i, j, n) for (int j = i + 1; j < n; j++)
#define CL(a, x) memset(x, a, sizeof(x));
#define SZ(x) ((int)x.size())
#define ASZ(x) sizeof(x);
const float pi = acos(-1.0);
const int inf = 1000000000;
const float MAXD = 100000000000.0;
typedef long long ll;
typedef unsigned long long ull;
const float EPS = 1e-9;

//@see https://stackoverflow.com/questions/33333363/built-in-mod-vs-custom-mod-function-improve-the-performance-of-modulus-op/33333636#33333636
inline size_t fast_mod(const size_t i, const size_t c)
{
  return i >= c ? i % c : i;
}

using namespace std;
string resDir = "/Users/erikkvanli/Downloads/tests/resDir";
string resFileName = "";
int dx[8] = {0, 1, 0, -1, -1, -1, 1, 1};
int dy[8] = {-1, 0, 1, 0, -1, 1, -1, 1};
vector<string> dname = {"U", "R", "D", "L"};
enum Direction
{
  LEFT = 1 << 0,  // 0001
  RIGHT = 1 << 1, // 0010
  UP = 1 << 2,    // 0100
  DOWN = 1 << 3,  // 1000
};

template <int N>
struct fastbitset
{
  static_assert(N >= 0 && N <= 64);
  static constexpr uint64_t kMask = (N == 64) ? ~0ull : ((1ull << N) - 1ull);
  uint64_t bits = 0;

  inline void set(int i, int v = 1) noexcept
  {
    const uint64_t m = 1ull << i;
    bits = (bits & ~m) | (uint64_t(!!v) << i);
    if constexpr (N < 64)
      bits &= kMask;
  }
  inline int get(int i) const noexcept
  {
    return int((bits >> i) & 1ull);
  }
  inline void flip(int i) noexcept
  {
    bits ^= (1ull << i);
    if constexpr (N < 64)
      bits &= kMask;
  }
  inline void invert() noexcept
  {
    bits = ~bits;
    if constexpr (N < 64)
      bits &= kMask;
  }
  inline void clear() noexcept
  {
    bits = 0;
  }
  inline uint32_t count() const noexcept
  {
    return __builtin_popcountll(bits);
  }
  inline bool any() const noexcept
  {
    return bits != 0;
  }
  inline bool none() const noexcept
  {
    return bits == 0;
  }
  inline uint64_t value()
  {
    return bits;
  }
};

string left_pad(string_view src, size_t width, char fill = ' ')
{
  if (src.size() >= width)
    return string(src); // already wide enough
  return string(width - src.size(), fill) + string(src);
}

struct Stopwatch
{
  float TIME_LIMIT;
  chrono::high_resolution_clock::time_point start_time;

  Stopwatch(const float TIME_LIMIT = 1)
      : TIME_LIMIT(TIME_LIMIT * 1000000LL),
        start_time(chrono::high_resolution_clock::now())
  {
  }

  int64_t elapsed_nanoseconds() const
  {
    return chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start_time).count();
  }

  int64_t elapsed_milliseconds() const
  {
    return chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start_time).count();
  }

  float elapsed_fraction() const
  {
    return static_cast<float>(elapsed_nanoseconds()) / TIME_LIMIT;
  }

  bool timeout() const
  {
    return elapsed_nanoseconds() >= TIME_LIMIT;
  }
};

struct XRand
{
  u_int32_t val;

  XRand(const u_int32_t v = 643263)
      : val(v)
  {
  }

  u_int32_t xorshift32()
  {
    val ^= val << 13;
    val ^= val >> 17;
    val ^= val << 5;
    return val;
  }

  u_int64_t next(const int m)
  { // [0, m)
    return static_cast<u_int64_t>(xorshift32()) * m >> 32;
  }

  float nextfloat()
  { // [0, 1)
    return static_cast<float>(xorshift32()) / (static_cast<u_int64_t>(1) << 32);
  }
};
XRand rnd;

struct IntPos
{
  int x, y;
  IntPos(int x = 0, int y = 0)
      : x(x),
        y(y)
  {
  }
  int manhattanDistance(const IntPos &other) const
  {
    return abs(x - other.x) + abs(y - other.y);
  }

  IntPos apply(int &delta, int times = 1)
  {
    if (delta == -1)
      return IntPos(x, y);
    return IntPos(x + dx[delta] * times, y + dy[delta] * times);
  }

  bool equals(IntPos other)
  {
    return x == other.x && y == other.y;
  }
};

struct Position
{
  float x, y;
  Position(float x = 0, float y = 0)
      : x(x),
        y(y)
  {
  }
  int manhattanDistance(const Position &other) const
  {
    return abs(x - other.x) + abs(y - other.y);
  }
  Position apply(const Position &other, const float &times)
  {
    return Position(x + other.x * times, y + other.y * times);
  }
  float euclideanDistance(const Position &other) const
  {
    return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
  }

  float euclideanDistance(const IntPos &other) const
  {
    return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
  }
  void print() const
  {
    cout << "(" << x << ", " << y << ")";
  }
};

int sign(int val)
{
  if (val == 0)
    return 0;
  if (val < 0)
    return -1;
  return 1;
}

int getDir(int x, int y, int nX, int nY)
{
  int deltaX = sign(nX - x);
  int deltaY = sign(nY - y);
  F0 (i, 4)
  {
    if (dx[i] == deltaX && dy[i] == deltaY)
      return i;
  }
  cerr << "INVALID POSITION" << endl;
  return -1;
}

class Svg
{
public:
  // Constructor
  Svg(int width, int height, float offsetX, float offsetY, float scale)
      : width_(width),
        height_(height),
        offsetX_(offsetX),
        offsetY_(offsetY),
        scale_(scale)
  {
  }

  // Add a circle element
  void addCircle(const Position &pos, float radius, const string &fillColor)
  {
    float scaledX = (pos.x - offsetX_) / scale_;
    float scaledY = (pos.y - offsetY_) / scale_;
    float scaledR = radius / scale_;
    string circle = "<circle cx=\"" + to_string(scaledX) + "\" cy=\"" + to_string(scaledY) + "\" r=\"" + to_string(scaledR) + "\" fill=\"" + fillColor + "\" />";
    elements_.push_back(circle);
  }

  // Add a rectangle element
  void addRect(const Position &pos, float rectWidth, float rectHeight, const string &fillColor)
  {
    float scaledX = (pos.x - offsetX_) / scale_;
    float scaledY = (pos.y - offsetY_) / scale_;
    float scaledW = rectWidth / scale_;
    float scaledH = rectHeight / scale_;
    string rect = "<rect x=\"" + to_string(scaledX) + "\" y=\"" + to_string(scaledY) + "\" width=\"" + to_string(scaledW) + "\" height=\"" + to_string(scaledH) + "\" fill=\"" + fillColor + "\" />";
    elements_.push_back(rect);
  }

  // Add a line element
  void addLine(const Position &p1, const Position &p2, int strokeWidth, const string &color)
  {
    float scaledX1 = (p1.x - offsetX_) / scale_;
    float scaledY1 = (p1.y - offsetY_) / scale_;
    float scaledX2 = (p2.x - offsetX_) / scale_;
    float scaledY2 = (p2.y - offsetY_) / scale_;
    string line = "<line x1=\"" + to_string(scaledX1) + "\" y1=\"" + to_string(scaledY1) + "\" x2=\"" + to_string(scaledX2) + "\" y2=\"" + to_string(scaledY2) + "\" stroke=\"" + color + "\" stroke-width=\"" + to_string(strokeWidth) + "\" />";
    elements_.push_back(line);
  }

  void drawGrid(int x, int y)
  {
    F0 (i, x + 1)
    {
      addLine(Position(0, i), Position(y, i), 1, "black");
    }
    F0 (i, y + 1)
    {
      addLine(Position(i, 0), Position(i, x), 1, "black");
    }
  }

  // Save all elements as a full SVG file
  void save(const string &fileName) const
  {
    ofstream outFile(fileName);
    if (!outFile.is_open())
    {
      // Handle file opening error if you like
      return;
    }

    // SVG header
    outFile << "<svg ";
    outFile << "width=\"" << width_ << "\" height=\"" << height_ << "\" ";
    outFile << "xmlns=\"http://www.w3.org/2000/svg\" ";
    outFile << "version=\"1.1\">\n";

    // Write all elements
    for (const auto &elem : elements_)
    {
      outFile << "  " << elem << "\n";
    }

    // SVG footer
    outFile << "</svg>\n";
    outFile.close();
  }

private:
  int width_;
  int height_;
  float offsetX_;
  float offsetY_;
  float scale_;

  vector<string> elements_;
};

#ifndef ZOBRIST_HASH_H
#define ZOBRIST_HASH_H
class ZobristHash
{
public:
  ZobristHash(size_t width, size_t height, size_t numTypes)
      : width_(width),
        height_(height),
        numTypes_(numTypes)
  {
    zobristTable_.resize(width_ * height_ * numTypes_);

    random_device rd;
    mt19937_64 generator(rd());

    for (auto &val : zobristTable_)
    {
      val = generator();
    }
  }

  uint64_t getKey(size_t x, size_t y, size_t type) const
  {
    size_t index = x + y * width_ + type * (width_ * height_);
    return zobristTable_[index];
  }

private:
  size_t width_;
  size_t height_;
  size_t numTypes_;
  vector<uint64_t> zobristTable_;
};

#endif // ZOBRIST_HASH_H

void writeSvg(vector<Position> &posses, bool includeNegative)
{
  int svgSize = 2000;
  float maxPos = 0;

  for (auto &a : posses)
  {
    float localMin = min(a.x, a.y);
    float localMax = max(abs(a.x), abs(a.y));

    maxPos = max(maxPos, localMax);
  }
  Svg svg(svgSize, svgSize, includeNegative ? -maxPos : 0, includeNegative ? -maxPos : 0, maxPos * (includeNegative ? 2 : 1) / (svgSize / (includeNegative ? 1 : 2)));
  if (includeNegative)
    svg.addCircle(Position(0, 0), 5, "red");
  F0 (i, posses.size())
  {
    svg.addCircle(posses[i], 5, "blue");
  }
  svg.save("MySvg_" + resFileName + ".svg");
}

bool fileExists(const string &filename)
{
  ifstream file(filename);
  return file.good();
}
string filename = "/Users/erikkvanli/Downloads/ac32/0000.txt";
bool isLocal = fileExists(filename);

// -------------------------------------------------------------- #1 VARIABLES ------------------------------
unordered_set<uint64_t> seen;
// unordered_map<uint64_t, int> mapSeen;
int forcedSeed = 0, evals = 0;
string seed = "-1";

// -------------------------------------------------------------- #2 CONSTANTS ------------------------------
const int MaxN = 20, MaxNN = MaxN * MaxN, MaxK = 400;

// -------------------------------------------------------------- #3 SIM ------------------------------------
vector<float> trainedValues;
vector<vector<float>> tv;
int N, K, T;
vector<IntPos> targets;
vector<fastbitset<4>> wallMap;
int MaxDepth = 5;
ZobristHash zh(MaxN, MaxN, 2);
vector<int> timesLeft;

int getHash(int x, int y)
{
  return x + y * N;
}
int getHash(IntPos &p)
{
  return p.x + p.y * N;
}

bool onBoard(int x, int y)
{
  return x >= 0 && y >= 0 && x < N && y < N;
}

bool onBoard(IntPos &p)
{
  return onBoard(p.x, p.y);
}

bool canGo(int &dir, IntPos &p)
{
  return dir == -1 ? true : !wallMap[getHash(p.x, p.y)].get(dir);
}

const int DijkstraSize = MaxN;
int dists[DijkstraSize * DijkstraSize][DijkstraSize * DijkstraSize];

int getDist(IntPos &a, IntPos &b)
{
  return dists[getHash(a)][getHash(b)];
}

struct Transition
{
  int16_t Color;
  int8_t State;
  int8_t Dir;
  bool used;
};

/// SSState
int vis[MaxNN], vz = 0;

struct DijkstraQueue
{
  int x, y;
  float dist;
};

void dijkstra(int startX, int startY)
{ // , vector<vector<float>> &heatMap
  // //cerr << "Start: " << startX << " " << startY << endl;
  vector<DijkstraQueue> queue = {{startX, startY, 1}};
  int amount = 0, idx = startX + startY * N;
  dists[idx][idx] = 0;
  while (queue.size() > 0)
  {
    vector<DijkstraQueue> next;
    F0 (i, queue.size())
    {
      F0 (j, 4) // TODO: 4 vs 8 dir
      {
        const int x2 = queue[i].x + dx[j];
        const int y2 = queue[i].y + dy[j];
        if (x2 < 0 || y2 < 0 || x2 >= N || y2 >= N)
          continue;                  // outside
        float depth = queue[i].dist; // + heatMap[x2][y2];

        int idx2 = x2 + y2 * N;
        IntPos p(queue[i].x, queue[i].y);
        // cerr << x2 << " " << y2 << " " << depth << endl;
        if (!canGo(j, p))
        {
          continue;
        }

        // //cerr << startX << " , " << startY << " D: " << depth << " (" << dists[idx][idx2] << ") " << " FROM: " << x2 << " , " << y2 << endl;
        if (dists[idx][idx2] <= depth)
          continue; // faster paths exist

        dists[idx][idx2] = depth;
        next.push_back({x2, y2, depth + 1});
        amount++;
      }
    }
    queue = next;
  }
}

void initializeDistances()
{ // vector<vector<float>> &heatMap
  for (int i = 0; i < DijkstraSize * DijkstraSize; ++i)
  {
    for (int j = 0; j < DijkstraSize * DijkstraSize; ++j)
    {
      dists[i][j] = inf;
    }
  }

  for (int x = 0; x < N; x++)
  {
    for (int y = 0; y < N; y++)
    {
      // TODO: Check if impassable start
      dijkstra(x, y);
    }
  }
}

// -------------------------------------------------------------- #4 AGENT --------------------------------

struct Solution
{
  int idx;
};

// -------------------------------------------------------------- #4 MAIN --------------------------------

int findTotalMoves()
{
  int moves = 0;
  for (int i = K - 2; i >= 0; i--)
  {
    moves += getDist(targets[i], targets[i + 1]);
    timesLeft.emplace_back(moves);
  }
  reverse(timesLeft.begin(), timesLeft.end());
  timesLeft.emplace_back(0);
  timesLeft.emplace_back(0);
  return moves;
}

void writeHeatMapSvg(const vector<vector<float>> &heat, const string &outFile = "heatmap.svg")
{
  int svgSize = 2000;
  // scale grid coords [0..gridSize] into [0..svgSize]
  float m = 0.0;
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      m = max(heat[i][j], m);
    }
  }

  Svg svg(svgSize, svgSize, 0.0, 0.0, 0.05);
  // cerr << gridSize << endl;
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      float v = heat[i][j];
      // cerr << v << endl;
      if (v <= 0.0)
      {
        svg.addRect(Position(i, j), 1.0, 1.0, "rgba(0,255,0,1)");
        continue;
      }
      // red with alpha=v
      ostringstream col;
      col << "rgba(255,0,0," << v / m << ")";
      svg.addRect(Position(i, j), 1.0, 1.0, col.str());
    }
  }

  svg.save(outFile);
}

void dfsHeat(vector<vector<float>> &heat, int target, IntPos p, int prevDir)
{
  int d = getDist(targets[target], p);
  heat[p.x][p.y]++;
  if (d == 0)
    return;
  if (prevDir != -1 && canGo(prevDir, p))
  {
    IntPos next = p.apply(prevDir);
    if (getDist(targets[target], next) < d)
    {
      dfsHeat(heat, target, next, prevDir);
      return;
    }
  }
  F0 (i, 4)
  {
    if (!canGo(i, p))
      continue;
    IntPos next = p.apply(i);
    if (getDist(targets[target], next) < d)
    {
      dfsHeat(heat, target, next, i);
    }
  }
}

vector<vector<float>> makeHeatMap()
{
  vector<vector<float>> heat;
  F0 (i, N)
  {
    vector<float> a;
    F0 (j, N)
    {
      a.emplace_back(0.0);
    }
    heat.emplace_back(a);
  }
  F0 (i, targets.size() - 1)
  {
    dfsHeat(heat, i + 1, targets[i], -1);
  }
  F0 (i, targets.size())
  {
    if (i == 0)
      continue;
    dfsHeat(heat, i, targets[i - 1], -1);
  }
  float m = 0.0;
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      m = max(heat[i][j], m);
    }
  }

  if (m > 0)
  {
    for (int i = 0; i < N; i++)
    {
      for (int j = 0; j < N; j++)
      {
        heat[i][j] /= m;
      }
    }
  }
  return heat;
}

float stopProba = 0.0;

int testBoard[MaxN * MaxN];

struct State
{
  IntPos p;
  int state;
  int target = 0;
  int moves;
  int _extraMoves;
  int extraMoves;
  vector<Transition> transitions;
  int _maxC, _maxState;
  int _allocatedMaxState;
  int boardColor[MaxN * MaxN];
  int initBoard[MaxN * MaxN];
  int tId(int c, int t)
  {
    return c * _allocatedMaxState + t;
  }
  int getCurrentColor()
  {
    return boardColor[getHash(p)];
  }
  State()
  {
  }

  State(const State &other)
  {
    copyFrom(other);
  }

  State &operator=(const State &other)
  {
    if (this != &other)
    {
      copyFrom(other);
    }
    return *this;
  }

  int getScore()
  {
    int score = 0;
    F0 (i, _maxC)
    {
      bool all = true;
      F0 (j, _maxState)
      {
        if (transitions[tId(i, j)].used)
        {
          all = false;
          break;
        }
      }
      if (!all)
      {
        ++score;
      }
    }

    F0 (j, _maxState)
    {
      bool all = true;
      F0 (i, _maxC)
      {
        if (transitions[tId(i, j)].used)
        {
          all = false;
          break;
        }
      }
      if (!all)
      {
        ++score;
      }
    }
    return score;
  }

  State(int maxC, int maxState, int em)
  {
    _extraMoves = em;
    extraMoves = em;
    _maxC = maxC;
    _maxState = maxState;
    _allocatedMaxState = maxState;
    transitions.resize(maxC * maxState);
    F0 (i, maxC)
    {
      F0 (j, maxState)
      {
        Transition t;
        t.used = false;
        transitions[tId(i, j)] = t;
      }
    }
  }

  IntPos getTarget()
  {
    return targets[target];
  }

  bool hasTransition()
  {
    return boardColor[getHash(p)] >= 0 && transitions[tId(boardColor[getHash(p)], state)].used;
  }

  void addTransition(Transition t)
  {
    if (boardColor[getHash(p)] < 0)
    {
      cerr << "Setting transition on negative" << endl;
    }
    if (transitions[tId(boardColor[getHash(p)], state)].used)
    {
      cerr << "TRYING TO SET TRANSITION ON USED " << moves << " T: " << T << endl;
      return;
    }
    t.used = true;
    transitions[tId(boardColor[getHash(p)], state)] = t;
  }

  void doMove()
  {
    while (moves < T && extraMoves >= 0)
    {
      int color = boardColor[getHash(p)];
      if (color < 0)
        break;

      Transition &t = transitions[tId(color, state)];
      if (!t.used) // End state
        break;
      boardColor[getHash(p)] = t.Color;

      state = t.State;
      int dir = t.Dir;
      int currentD = getDist(p, targets[target]);
      if (canGo(dir, p))
      {
        p = p.apply(dir);
      }
      int nextD = getDist(p, targets[target]);
      if (currentD == nextD)
        --extraMoves;
      else if (nextD > currentD)
        extraMoves -= 2;
      ++moves;
      if (p.equals(targets[target]))
      {
        if (++target == K)
          break;
      }
    }
  }

  pair<int, IntPos> checkExistingPath(IntPos innerP, int maxMoves, int testState, bool allowSame, bool terminateOnTarget, int initialDist)
  {
    ++vz;
    // memcpy(testBoard, boardColor, N * N * sizeof(int));
    int innerMoves = initialDist;
    int innerTarget = target;
    int wasted = 0;
    while (innerMoves + moves < T)
    {
      if (innerMoves > maxMoves || vis[getHash(innerP)] == vz && !allowSame && rnd.nextfloat() < (1 - stopProba))
      {
        return {-1, innerP};
      }

      int color = vis[getHash(innerP)] == vz ? testBoard[getHash(innerP)] : boardColor[getHash(innerP)];
      vis[getHash(innerP)] = vz;

      if (color < 0 || !transitions[tId(color, testState)].used) // End state
        break;

      Transition &t = transitions[tId(color, testState)];
      testBoard[getHash(innerP)] = t.Color;
      int dir = t.Dir;
      int currentD = getDist(innerP, targets[innerTarget]);
      if (canGo(dir, innerP))
      {
        innerP = innerP.apply(dir);
      }

      int nextD = getDist(innerP, targets[innerTarget]);
      if (currentD == nextD)
        ++wasted;
      else if (nextD > currentD)
        wasted += 2;

      testState = t.State;
      innerMoves++;
      if (innerP.equals(targets[innerTarget]))
      {
        if (terminateOnTarget && rnd.nextfloat() < 0.8 || innerTarget == K)
          return {innerMoves, innerP};
      }

      if (extraMoves - wasted < 0)
        return {-1, innerP};
    }

    return {innerMoves, innerP};
  }

  int getTransitionColorId(int state, int color)
  {
    return (state * _maxC + color) * -1 - 2;
  }

  void updateTransitionColor(int v, int color)
  {
    if (v >= -1)
      return;
    F0 (i, N)
    {
      F0 (j, N)
      {
        if (boardColor[getHash(i, j)] == v)
          boardColor[getHash(i, j)] = color;
      }
    }
    auto id = getTransitionColorIdReverse(v);
    transitions[tId(id.second, id.first)].Color = color;
  }

  pair<int, int> getTransitionColorIdReverse(int v)
  {
    int combined = -(v + 2);
    int state = combined / _maxC;
    int color = combined % _maxC;
    return {state, color};
  }

  void reset(int maxC, int maxState, int initBoard[MaxN * MaxN])
  {
    extraMoves = _extraMoves;
    _maxC = maxC;
    _maxState = maxState;
    _allocatedMaxState = maxState;
    p = targets[0];
    target = 1;
    state = 0;
    moves = 0;

    if (transitions.size() < maxC * maxState)
    {
      transitions.resize(maxC * maxState);
    }

    F0 (i, _maxC)
    {
      F0 (j, _maxState)
      {
        transitions[tId(i, j)].used = false;
        transitions[tId(i, j)].Color = -1;
      }
    }

    memcpy(boardColor, initBoard, N * N * sizeof(int));
  }

  void removeColor(int c)
  {
    F0 (x, N)
    {
      F0 (y, N)
      {
        if (boardColor[getHash(x, y)] == _maxC - 1)
        {
          boardColor[getHash(x, y)] = c;
        }
        if (initBoard[getHash(x, y)] == _maxC - 1)
        {
          initBoard[getHash(x, y)] = c;
        }
      }
    }
    F0 (i, _maxC)
    {
      F0 (j, _maxState)
      {
        if (transitions[tId(i, j)].Color == _maxC - 1)
          transitions[tId(i, j)].Color = c;
      }
    }

    F0 (i, _maxState)
    {
      swap(transitions[tId(c, i)], transitions[tId(_maxC - 1, i)]);
    }

    --_maxC;
  }

  void removeState(int s)
  {
    F0 (c, _maxC)
    {
      swap(transitions[tId(c, s)], transitions[tId(c, _maxState - 1)]);
    }

    F0 (i, _maxC)
    {
      F0 (j, _maxState)
      {
        if (transitions[tId(i, j)].State == _maxState - 1)
          transitions[tId(i, j)].State = s;
      }
    }
    --_maxState;
  }

  void copyFrom(const State &other)
  {
    p = other.p;
    state = other.state;
    target = other.target;
    moves = other.moves;
    _extraMoves = other._extraMoves;
    extraMoves = other.extraMoves;
    _maxC = other._maxC;
    _maxState = other._maxState;
    _allocatedMaxState = other._allocatedMaxState;
    transitions = other.transitions;
    memcpy(boardColor, other.boardColor, N * N * sizeof(int));
    memcpy(initBoard, other.initBoard, N * N * sizeof(int));
  }
};

void relabelState(State &s)
{
  bool changed = true;
  while (changed)
  {
    changed = false;
    F0 (i, s._maxC)
    {
      bool all = true;
      F0 (j, s._maxState)
      {
        if (s.transitions[s.tId(i, j)].used)
        {
          all = false;
          break;
        }
      }
      if (all)
      {
        changed = true;
        s.removeColor(i);
        break;
      }
    }

    F0 (j, s._maxState)
    {
      bool all = true;
      F0 (i, s._maxC)
      {
        if (s.transitions[s.tId(i, j)].used)
        {
          all = false;
          break;
        }
      }
      if (all)
      {
        changed = true;
        s.removeState(j);
        break;
      }
    }
  }
}

int colorUsages[1000];
int stateUsages[1000];
int emptyStates[200], esn = 0;
int doGreedy(State &s, int maxState, int maxColor)
{
  memset(colorUsages, 0, maxColor * sizeof(int));
  memset(stateUsages, 0, maxState * sizeof(int));
  memset(s.initBoard, -1, N * N * sizeof(int));
  bool randomized = rnd.nextfloat() < 0.1;
  bool stuck = rnd.nextfloat() < 0.9;

  s.reset(maxColor, maxState, s.initBoard);
  int allowSameCellMovement = rnd.nextfloat() < 0.5f && randomized;
  float usageScore = 0.05f;
  int stopProbaMulti = rnd.next(4) + 1;
  while (s.target < K && s.extraMoves >= 0 && s.moves < T)
  {
    if (s.hasTransition())
    {
      s.doMove();
      continue;
    }

    IntPos target = s.getTarget();
    int DistScore = rnd.next(3) + 1;
    int prevDist = getDist(s.p, target);
    if (s.boardColor[getHash(s.p)] < 0)
    {
      IntPos closer = s.p;
      F0 (i, 4)
      {
        if (!canGo(i, s.p))
          continue;
        IntPos nxt = s.p.apply(i);
        if (getDist(nxt, target) < prevDist)
          closer = nxt;
      }
      float colorMoveAddon = rnd.nextfloat();
      int prevColor = s.boardColor[getHash(s.p)];
      int bestColor = rnd.next(maxColor);

      int bestScore = 0;
      bool autoMove = true;
      if (rnd.nextfloat() < 0.999f)
        F0 (i, s._maxC)
        {
          if (rnd.nextfloat() < 0.9 && !s.transitions[s.tId(i, s.state)].used)
            continue;

          s.boardColor[getHash(s.p)] = i;
          int testState = s.state;
          auto [moves, pos] = !s.transitions[s.tId(i, s.state)].used ? s.checkExistingPath(closer, prevDist + min((rnd.nextfloat() < 0.9f && !stuck) || !randomized ? 0 : (int)rnd.next(N), s.extraMoves), testState, allowSameCellMovement, rnd.nextfloat() < stopProba * stopProbaMulti && stuck, 1) : s.checkExistingPath(s.p, prevDist + min((rnd.nextfloat() < 0.9f && !stuck) || !randomized ? 0 : (int)rnd.next(N), s.extraMoves), testState, allowSameCellMovement, rnd.nextfloat() < stopProba * stopProbaMulti && stuck, 0);
          if (bestColor == i && moves <= 1)
          {
            bestColor = rnd.next(s._maxC);
          }
          if (moves <= 0)
          {
            continue;
          }
          int newDist = getDist(pos, target);
          float score = (prevDist - newDist);
          score -= colorUsages[i] * usageScore;
          if (!s.transitions[s.tId(i, s.state)].used)
            score--;

          if (prevDist - newDist == moves)
          { // Exact number of moves - all going closer
            score += moves * colorMoveAddon;
          }
          else if (prevDist - newDist - moves + s.extraMoves >= 0)
          { // Some wasted moves
          }
          else
          {
            continue;
          }
          score += rnd.nextfloat();
          if (score > bestScore)
          {
            bestScore = score;
            bestColor = i;
          }
        }

      s.updateTransitionColor(prevColor, bestColor);
      if (prevColor == -1)
        s.initBoard[getHash(s.p)] = bestColor;
      s.boardColor[getHash(s.p)] = bestColor;
      ++colorUsages[bestColor];
      continue;
    }

    float best = -inf;
    int bestDir = rnd.next(4);
    int bestState = rnd.next(s._maxState);

    int prevTarget = s.target;
    F0 (i, 4)
    {
      IntPos nxt = !canGo(i, s.p) ? s.p : s.p.apply(i);
      if (getDist(nxt, targets[s.target]) > prevDist && rnd.nextfloat() < 0.995f)
        continue;
      esn = 0;

      F0 (j, maxState)
      {
        if (rnd.nextfloat() < 0.05f)
          continue;

        bool empty = false;
        if (s.boardColor[getHash(nxt)] < 0 || !s.transitions[s.tId(s.boardColor[getHash(nxt)], j)].used)
        {
          empty = true;
          // if(esn > 0){
          //   emptyStates[esn++] = j;
          //   continue;
          // }
        }
        int testState = j;
        auto [moves, pos] = empty ? make_pair(1, nxt) : s.checkExistingPath(nxt, prevDist + min((!stuck) || !randomized ? 0 : (int)rnd.next(N), s.extraMoves), testState, allowSameCellMovement, rnd.nextfloat() < stopProba * stopProbaMulti && stuck, 1);

        if (moves <= 0)
          continue;
        int newDist = getDist(pos, target);
        float score = (prevDist - newDist) * DistScore;
        score -= stateUsages[j] * usageScore;
        if (s.boardColor[getHash(pos)] < -1)
          score += 0.5f;
        else if (s.boardColor[getHash(pos)] == -1)
          score += 0.25f;
        else
          score -= 0.5f;

        if (prevDist - newDist == moves)
        {                                                    // Exact number of moves - all going closer
          score += moves * 2 + (prevDist - newDist - moves); // TODO:
        }
        else if (prevDist - newDist - moves + s.extraMoves >= 0 && rnd.nextfloat() < stopProba)
        {                                                    // Some wasted moves
          score += moves + (prevDist - newDist - moves) * 1; // TODO: Degree
        }
        else
        {
          continue;
        }
        score += rnd.nextfloat();
        if (score > best)
        {
          if (!empty)
            esn = 0;
          else
          {
            emptyStates[esn++] = j;
          }
          best = score;
          bestDir = i;
          bestState = j;
        }
      }

      if (esn > 0)
        bestState = emptyStates[rnd.next(esn)];
    }

    ++stateUsages[bestState];
    s.addTransition({(int16_t)(rnd.nextfloat() < stopProba * 3 ? s.getTransitionColorId(s.state, s.boardColor[getHash(s.p)]) : rnd.next(maxColor)), (int8_t)bestState, (int8_t)bestDir, true});
    s.doMove();
  }

  if (s.target < K)
  {
    return inf;
  }

  bool changed = true;
  while (changed)
  {
    changed = false;
    F0 (i, s._maxC)
    {
      bool all = true;
      F0 (j, s._maxState)
      {
        if (s.transitions[s.tId(i, j)].used)
        {
          all = false;
          break;
        }
      }
      if (all)
      {
        changed = true;
        s.removeColor(i);
        break;
      }
    }

    F0 (j, s._maxState)
    {
      bool all = true;
      F0 (i, s._maxC)
      {
        if (s.transitions[s.tId(i, j)].used)
        {
          all = false;
          break;
        }
      }
      if (all)
      {
        changed = true;
        s.removeState(j);
        break;
      }
    }
  }

  return s._maxC + s._maxState;
}

struct BeamState
{
  BeamState(State cpy)
  {
    s.copyFrom(cpy);
  }
  float score;
  State s;
};

void scoreBeam(BeamState &beam)
{
  beam.score = 0;
  if (beam.s.target == K)
    beam.score = 100000;
  else
    beam.score = -timesLeft[beam.s.target] + beam.s.extraMoves * 0.5 - getDist(beam.s.p, targets[beam.s.target]);

  F0 (i, beam.s._maxC)
  {
    bool all = true;
    F0 (j, beam.s._maxState)
    {
      if (beam.s.transitions[beam.s.tId(i, j)].used)
      {
        all = false;
        break;
      }
    }
    if (all)
    {
      beam.score += 1.0f * beam.s.target / (float)K;
    }
  }

  F0 (j, beam.s._maxState)
  {
    bool all = true;
    F0 (i, beam.s._maxC)
    {
      if (beam.s.transitions[beam.s.tId(i, j)].used)
      {
        all = false;
        break;
      }
    }
    if (all)
    {
      beam.score += 1.0f * beam.s.target / (float)K;
    }
  }
  beam.score += rnd.nextfloat() * 0.9f;
}

const int MAX_POOL_SIZE = 1000;
BeamState *beamPool[MAX_POOL_SIZE];
int poolSize = 0;

BeamState *getBeamFromPool(const State &s)
{
  if (poolSize > 0)
  {
    BeamState *b = beamPool[--poolSize];
    b->s.copyFrom(s);
    b->score = 0;
    return b;
  }
  return new BeamState(s);
}

void returnBeamToPool(BeamState *b)
{
  if (poolSize < MAX_POOL_SIZE)
  {
    beamPool[poolSize++] = b;
  }
  else
  {
    delete b;
  }
}

void clearBeamPool()
{
  for (int i = 0; i < poolSize; i++)
  {
    delete beamPool[i];
  }
  poolSize = 0;
}

struct BeamCompare
{
  bool operator()(const BeamState *a, const BeamState *b) const
  {
    return a->score > b->score;
  }
};

priority_queue<BeamState *, vector<BeamState *>, BeamCompare> nextBeamsHeap;
vector<BeamState *> beams;
int BEAM_WIDTH = 20;

void addToNextBeams(BeamState *beam)
{
  if (nextBeamsHeap.size() < BEAM_WIDTH)
  {
    nextBeamsHeap.push(beam);
    return;
  }

  if (beam->score <= nextBeamsHeap.top()->score)
  {
    returnBeamToPool(beam);
    return;
  }

  returnBeamToPool(nextBeamsHeap.top());
  nextBeamsHeap.pop();
  nextBeamsHeap.push(beam);
}

void finalizeNewBeam(BeamState *nxt)
{
  while (nxt->s.target < K && nxt->s.extraMoves >= 0 && nxt->s.moves < T)
  {
    if (nxt->s.hasTransition())
    {
      nxt->s.doMove();
    }
    else
    {
      break;
    }
  }

  if (nxt->s.extraMoves < 0)
  {
    returnBeamToPool(nxt);
    return;
  }

  scoreBeam(*nxt);
  addToNextBeams(nxt);
}

void createChildBeam(BeamState *parent, int state, int dir)
{
  BeamState *nxt = getBeamFromPool(parent->s);

  nxt->s.addTransition({(int16_t)nxt->s.getTransitionColorId(nxt->s.state, nxt->s.boardColor[getHash(nxt->s.p)]), (int8_t)state, (int8_t)dir, true});
  finalizeNewBeam(nxt);
}

void createChildBeamColor(BeamState *parent, int color, int prevColor)
{
  BeamState *nxt = getBeamFromPool(parent->s);

  nxt->s.updateTransitionColor(prevColor, color);
  if (prevColor == -1)
    nxt->s.initBoard[getHash(nxt->s.p)] = color;
  nxt->s.boardColor[getHash(nxt->s.p)] = color;
  finalizeNewBeam(nxt);
}

void expandBeamColor(BeamState *beam)
{
  auto target = beam->s.getTarget();
  int prevDist = getDist(beam->s.p, target);
  int prevColor = beam->s.boardColor[getHash(beam->s.p)];

  int bestScore = 0;
  bool autoMove = true;
  F0 (i, beam->s._maxC)
  {
    if (rnd.nextfloat() < 0.05)
      continue;
    createChildBeamColor(beam, i, prevColor);
  }
}

void expandBeam(BeamState *beam)
{
  if (beam->s.boardColor[getHash(beam->s.p)] < 0)
  {
    expandBeamColor(beam);
    return;
  }
  auto target = beam->s.getTarget();
  int prevDist = getDist(beam->s.p, target);
  F0 (i, 4)
  {
    IntPos nxt = !canGo(i, beam->s.p) ? beam->s.p : beam->s.p.apply(i);
    esn = 0;
    if (rnd.nextfloat() < 0.05)
      continue;
    F0 (j, beam->s._maxState)
    {
      if (rnd.nextfloat() < 0.05)
        continue;
      bool empty = false;
      if (beam->s.boardColor[getHash(nxt)] < 0 || !beam->s.transitions[beam->s.tId(beam->s.boardColor[getHash(nxt)], j)].used)
      {
        emptyStates[esn++] = j;
        continue;
      }

      createChildBeam(beam, j, i);
    }

    if (esn > 0)
    {
      createChildBeam(beam, emptyStates[rnd.next(esn)], i);
    }
  }
}
int failsInARow = -1;
int doBeam(State &s, int maxState, int maxColor, Stopwatch &sw, bool solved, int bestScore)
{
  for (auto *b : beams)
  {
    returnBeamToPool(b);
  }
  while (!nextBeamsHeap.empty())
  {
    returnBeamToPool(nextBeamsHeap.top());
    nextBeamsHeap.pop();
  }
  beams.clear();

  if (stopProba < 0.5 && T > 2500 || !solved || T > 5000)
  {
    clearBeamPool();
    return doGreedy(s, maxState, maxColor);
  }
  ++failsInARow;

  BEAM_WIDTH = (5000 / T) + 2;
  BEAM_WIDTH += failsInARow * failsInARow;
  BEAM_WIDTH = min(BEAM_WIDTH, 200);
  // cerr << BEAM_WIDTH << " C: " << maxColor << " S: " << maxState << endl;

  memset(s.initBoard, -1, N * N * sizeof(int));

  beams.reserve(BEAM_WIDTH);
  BeamState *initial = getBeamFromPool(s);
  initial->s.reset(maxColor, maxState, s.initBoard);
  initial->score = 0;
  beams.emplace_back(initial);

  while (beams.size() > 0)
  {
    if (sw.timeout())
    {
      return inf;
    }

    for (int i = 0; i < min(BEAM_WIDTH, SZ(beams)); i++)
    {
      expandBeam(beams[i]);
    }

    vector<BeamState *> nextBeams;
    nextBeams.reserve(BEAM_WIDTH);
    BeamState *bestBeam = nullptr;

    while (!nextBeamsHeap.empty())
    {
      BeamState *beam = nextBeamsHeap.top();
      nextBeamsHeap.pop();
      nextBeams.push_back(beam);

      // if (beam->s.target == K)
      // {
      //   cerr << beam->s.getScore() << " " << bestScore << endl;
      // }

      if (beam->s.target == K && beam->s.getScore() < bestScore)
      {
        bestScore = beam->s.getScore();
        bestBeam = beam;
      }
    }

    if (bestBeam != nullptr)
    {
      relabelState(bestBeam->s);
      s = bestBeam->s;
      for (auto *b : nextBeams)
      {
        returnBeamToPool(b);
      }
      failsInARow = -1;
      return bestScore;
    }

    for (auto *b : beams)
    {
      returnBeamToPool(b);
    }
    beams = nextBeams;
  }

  return inf;
}

void mainInner()
{
  Stopwatch sw(1900);
  wallMap.assign(MaxNN, fastbitset<4>());
  tv = vector<vector<float>>{
    // TRAINBELOW
    {1, 0.8037865462012612, 1, 1.0170093740878752, 1, 1, 1, 0.9728706012503018, 1, 1, 0.9182343794227712, 1.0262690255347333, 1, 0.9740130973552384, 0.9089177836417349},
  };
  trainedValues = tv[0];
  // READ

  cin >> N >> K >> T;
  // cerr << "N: " << N << " K: " << K << " T: " << T << endl;
  int walls = 0;
  // vertical walls
  for (int i = 0; i < N; i++)
  {
    string v;
    cin >> v;
    for (int j = 0; j < N - 1; j++)
    {
      if (v[j] == '1')
      {
        int id1 = getHash(j, i);     // (x=j, y=i)
        int id2 = getHash(j + 1, i); // (x=j+1, y=i)
        wallMap[id1].set(1);         // right
        wallMap[id2].set(3);         // left
        walls++;
      }
    }
  }

  // horizontal walls
  for (int i = 0; i < N - 1; i++)
  {
    string h;
    cin >> h;
    for (int j = 0; j < N; j++)
    {
      if (h[j] == '1')
      {
        int id1 = getHash(j, i);     // (x=j, y=i)
        int id2 = getHash(j, i + 1); // (x=j, y=i+1)
        wallMap[id1].set(2);         // down
        wallMap[id2].set(0);         // up
        walls++;
      }
    }
  }

  F0 (x, N)
  {
    F0 (y, N)
    {
      int id = getHash(x, y);
      F0 (i, 4)
      {
        IntPos p(x, y);
        IntPos p2 = p.apply(i);
        if (!onBoard(p2))
        {
          wallMap[id].set(i);
        }
      }
    }
  }

  F0 (i, K)
  {
    int x;
    int y;
    cin >> y >> x;
    // cerr << "Target: " << x << " , " << y << endl;
    targets.emplace_back(IntPos(x, y));
  }

  initializeDistances();

  evals = 0;
  int neededMoves = findTotalMoves();
  stopProba = min<float>(0.95, (T / (float)neededMoves) - 0.8);
  cerr << "Proba: " << stopProba << " T: " << T << " needed: " << neededMoves << endl;
  int extraMoves = T - neededMoves;
  auto heatMap = makeHeatMap();
  int initialSize = min(50, (int)sqrt(neededMoves) + 3);
  int size1 = initialSize;
  int size2 = size1;
  State bestState(size1, size2, extraMoves);
  State innerState(size1, size2, extraMoves);
  int bestScore = doBeam(bestState, size1, size2, sw, false, bestScore);
  cerr << "Score: " << bestScore << " MaxC: " << size1 << " MaxState: " << size2 << " Moves: " << neededMoves << " Time: " << sw.elapsed_milliseconds() << endl;

  if (bestScore < inf)
  {
    size1 = bestState._maxState;
    size2 = bestState._maxC;
  }

  bool stuck = false;
  while (!sw.timeout())
  {
    if (bestScore == inf && sw.elapsed_milliseconds() > 1000)
    {
      initialSize = (int)sqrt(neededMoves) + 3;
      size1 = initialSize;
      size2 = size1;
      bestState = State(size1, size2, extraMoves);
      innerState = State(size1, size2, extraMoves);
    }
    evals++;
    int trans = 0;

    int firstSize = size1 - rnd.next(4) + 1;
    int secondSize = size2 - rnd.next(4) + 1;
    while (firstSize < 1 || secondSize < 1 || firstSize + secondSize >= bestScore || firstSize + secondSize < size1 + size2 - 2)
    {
      firstSize = size1 - rnd.next(4) + 1;
      secondSize = size2 - rnd.next(4) + 1;
    }

    auto score = doBeam(innerState, firstSize, secondSize, sw, bestScore < inf, bestScore);
    // cerr << "Moves: " << innerState.moves << " Score: " << score << endl;
    // cerr << sw.elapsed_milliseconds() << " Size: " << size1 << " " << size2 << endl;
    if (score < bestScore)
    {
      size1 = innerState._maxState;
      size2 = innerState._maxC;
      stuck = false;
    }
    else
    {
      stuck = rnd.nextfloat() < 0.9;
    }

    if (score < bestScore)
    {
      cerr << "Score: " << score << " MaxC: " << innerState._maxC << " MaxState: " << innerState._maxState << " Moves: " << neededMoves << " Time: " << sw.elapsed_milliseconds() << " Evals: " << evals << endl;
      bestScore = score;
      swap(bestState, innerState);
    }
  }

  int rules = 0;
  F0 (i, bestState._maxC)
  {
    F0 (j, bestState._maxState)
    {
      if (bestState.transitions[bestState.tId(i, j)].used)
        rules++;
    }
  }
  cerr << "Evals: " << evals << endl;
  cout << bestState._maxC << " " << bestState._maxState << " " << rules << endl;
  F0 (i, N)
  {
    F0 (j, N)
    {
      cout << (bestState.initBoard[getHash(j, i)] < 0 ? 0 : bestState.initBoard[getHash(j, i)]) << " ";
    }
    cout << endl;
  }

  F0 (i, bestState._maxC)
  {
    F0 (j, bestState._maxState)
    {
      if (bestState.transitions[bestState.tId(i, j)].used)
      {
        if (bestState.transitions[bestState.tId(i, j)].Color >= bestState._maxC)
          cerr << "Color error: " << (int)bestState.transitions[bestState.tId(i, j)].Color << endl;
        if (bestState.transitions[bestState.tId(i, j)].State >= bestState._maxState)
          cerr << "State error: " << (int)bestState.transitions[bestState.tId(i, j)].State << endl;

        cout << i << " " << j << " " << (bestState.transitions[bestState.tId(i, j)].Color < 0 ? 0 : (int)bestState.transitions[bestState.tId(i, j)].Color) << " " << (int)bestState.transitions[bestState.tId(i, j)].State << " " << dname[bestState.transitions[bestState.tId(i, j)].Dir] << endl;
      }
    }
  }
}

int main(int argc, char *argv[])
{
  // allBeams.reserve(50000);
  if (argc >= 2)
  {
    string testId = left_pad(argv[1], 4, '0');
    string baseName = "/Users/erikkvanli/Repos/aLocalTraining/AC56/in/";
    string inp = baseName + testId + ".txt";
    ignore = freopen(inp.c_str(), "r", stdin);
  }
  mainInner();

  return 0;
}
