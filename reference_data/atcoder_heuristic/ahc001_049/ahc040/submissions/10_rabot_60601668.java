import java.io.IOException;
import java.io.InputStream;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.NoSuchElementException;
import java.util.Objects;
import java.util.Random;

class Main {
  static final long TL = 2600;
  static final long TL2 = 2900;

  // Input Data
  Input in = null;
  FastScanner sc = null;
  InputStream stdin = null;
  PrintStream stdout = null;

  // Start time
  long startTime = 0;
  // Remaining time till timeLimit
  long timeRemain( long timeLimit ) { return timeLimit - elapsed(); }
  // Elapsed time from start
  long elapsed() { return System.currentTimeMillis() - startTime; }
  long timeLimit() { return TL; }
  long timeLimit2() { return TL2; }
  boolean isTimeOver() { return elapsed() >= timeLimit(); }
  boolean isEmergency() { return elapsed() >= timeLimit2(); }

  Random rnd = new Random(41L);

  static final String LF = "\n";

  public static void main(String[] args) throws IOException {
    Main main = new Main();
    main.stdin = System.in;
    main.stdout = System.out;
    main.solve();
  }

  Input readInput() throws IOException {
    sc = new FastScanner(stdin);
    Input in = new Input();
    in.readParameter(sc);
    return in;
  }

  public Result solve() throws IOException {
    this.startTime = System.currentTimeMillis();
    // インプットの読み込み
    in = readInput();
    in.preCalc();
    
    // Solver sol = (in.est_L <= 25000 && in.N >= 55) ? new Solver4(in, this) : new Solver1(in, this);
    Solver sol = (in.im.est_L <= 25000 && in.N >= 65) ? new Solver7(in, this) : new Solver8(in, this);
    // Solver sol = new Solver8(in, this);
    Result result = sol.solve();

    // printResult(result);
    return result;
  }

  public void printResult(Result result) {
    println(result.toString());
  }
  public void query(Query query) {
    println(query.toString());
    query.ret_w = sc.nextInt();
    query.ret_h = sc.nextInt();
  }
  public void println(String str) {
    stdout.println(str);
  }
  public void print(String str) {
    stdout.print(str);
  }
  int rnd(int bound) {
    return rnd.nextInt(bound);
  }
  double rndDouble() {
    return rnd.nextDouble();
  }

  public void debug(String str) {}
}

class Input {
  int N = 0;
  int T = 0;
  int sigma_i = 0;
  int[] w = null;
  int[] h = null;

  ItemManager im = null;
  static final int U = 100000;

  int id_master = 0;

  public void readParameter(FastScanner sc) {
    N = sc.nextInt();
    T = sc.nextInt();
    sigma_i = sc.nextInt();

    w = new int[N];
    h = new int[N];
    for (int i = 0; i < N; i++) {
      w[i] = sc.nextInt();
      h[i] = sc.nextInt();
    }
  }

  public void preCalc() {
    im = new ItemManager(this);
    im.recalc();
  }
  int id() {
    return id_master++;
  }
  <T> T last(ArrayList<T> array) {
    return (array == null || array.size() == 0) ? null : array.get(array.size() - 1);
  }
}

class Coordinate {
  static final int MAX = 1000000000;
  int[] coord = null;
  Coordinate() {
    this.coord = new int[2];
  }
  Coordinate(int x, int y) {
    this.coord = new int[] {x, y};
  }
  Coordinate(int[] c) {
    this.coord = Arrays.copyOf(c, 2);
  }
  Coordinate(Coordinate c) {
    this.coord = Arrays.copyOf(c.coord, 2);
  }

  int z(int ax) {
    return coord[ax];
  }
  void setz(int ax, int z) {
    coord[ax] = z;
  }
  void setmax(int ax, int z) {
    coord[ax] = Math.max(coord[ax], z);
  }
  long product() {
    return ((long)coord[0]) * ((long)coord[1]);
  }
  void add(int x, int y) {
    coord[0] = Math.min(coord[0] + x, MAX);
    coord[1] = Math.min(coord[1] + y, MAX);
  }
  @Override
  public String toString() {
    return String.format("x:%d,y:%d", coord[0], coord[1]);
  }
}

class LongCoordinate {
  long[] coord = null;
  LongCoordinate() {
    this.coord = new long[2];
  }
  LongCoordinate(long x, long y) {
    this.coord = new long[] {x, y};
  }
  LongCoordinate(long[] c) {
    this.coord = Arrays.copyOf(c, 2);
  }
  LongCoordinate(LongCoordinate c) {
    this.coord = Arrays.copyOf(c.coord, 2);
  }

  long z(int ax) {
    return coord[ax];
  }
  void setz(int ax, long z) {
    coord[ax] = z;
  }
  void setmax(int ax, long z) {
    coord[ax] = Math.max(coord[ax], z);
  }
  void add(long x, long y) {
    coord[0] += x;
    coord[1] += y;
  }
  @Override
  public String toString() {
    return String.format("x:%d,y:%d", coord[0], coord[1]);
  }
}
class Item {
  final ItemManager im;
  private int id = 0;

  private Coordinate size = null;
  private Coordinate size_sum = null;
  private int dir = 0; // w <= hとするための補正
  private LongCoordinate variance = null; // 分散
  private int sample_cnt = 1;

  double rank_ratio = 0; // 高さの低い順に並べたときに、幅の累計が全体幅合計から見てどのあたりにいるか
  double nr_rank_ratio = 0; // rank_ratioを近隣のみで評価

  Item(int id, int w, int h, ItemManager im) {
    this.im = im;
    this.id = id;
    this.size = new Coordinate(w, h);
    this.size_sum = new Coordinate(w, h);
    long v = im.var;
    this.variance = new LongCoordinate(v, v);
    this.dir = w <= h ? 0 : 1;
  }
  void addResult(int w, int h) {
    size_sum.add(w, h);
    sample_cnt++;
    variance = new LongCoordinate(im.var / sample_cnt, im.var / sample_cnt);
    size.setz(0, size_sum.z(0) / sample_cnt);
    size.setz(1, size_sum.z(1) / sample_cnt);
    dir = size.z(0) <= size.z(1) ? 0 : 1;
  }

  int p() {
    return id;
  }
  int r(int r) {
    return r ^ dir;
  }
  // a=0:w 1:h
  int original_size(int ax) {
    return size.z(ax ^ dir);
  }
  // static final int LIM1 = 97500;
  // static final int LIM2 = 102500;
  int size(int ax) {
    return Math.max(im.est_L, Math.min(Input.U, original_size(ax)));
    // int sz = original_size(ax);
    // if ( sz <= LIM1 ) return Math.max(im.est_L, sz);
    // if ( sz >= LIM2 ) return Input.U;
    // return (sz - LIM1) / 2 + LIM1;
  }
  int w() {
    return size(0);
  }
  int h() {
    return size(1);
  }
  long original_S() {
    return size.product();
  }
  long S() {
    return ((long)size(0)) * ((long)size(1));
  }
  int dsize() {
    return Math.abs(size(1) - size(0));
  }

  long variance(int ax) {
    return variance.z(ax ^ dir);
  }
  long x_variance() {
    return variance.z(dir);
  }
  @Override
  public String toString() {
    return String.format("id:%d, size:(%s), var:(%s)", id, size.toString(), variance.toString());
  }
}

class ItemManager {
  final Input in;
  final int N;
  Item[] items = null;
  int est_L = 0;
  private long[] cum_S = null;
  private int[] cum_width = null;
  long var = 0;

  // 目標段数ごとのtarget_widthの範囲(最適とは限らない)
  int min_f = 0;
  int max_f = 0;
  int[] min_tw = null;
  int[] max_tw = null;

  ArrayList<Item> near_w = null;

  Comparator<Item> comp = (it0, it1) -> {
    if ( it0.h() != it1.h() ) {
      return it0.h() < it1.h() ? -1 : 1;
    } else {
      return it0.p() - it1.p();
    }
  };

  ItemManager(Input in) {
    this.in = in;
    this.N = in.N;
    this.var = ((long)in.sigma_i) * ((long)in.sigma_i);
    items = new Item[in.N];
    for (int i = 0; i < in.N; i++) {
      items[i] = new Item(i, in.w[i], in.h[i], this);
    }
  }
  Item item(int i) {
    return items[i];
  }

  void recalc() {
    // Lの見積もり
    int[] fib = new int[24];
    fib[0] = 1;
    fib[1] = 1;
    for (int i = 2; i < fib.length; i++) {
      fib[i] = fib[i - 1] + fib[i - 2];
    }
    int U = 100000;
    int L0 = 9999;
    int L1 = L0 + fib[23];
    HashMap<Integer, Double> prob = new HashMap<>();
    for (int t = fib.length - 3; t >= 1; t--) {
      int M1 = L0 + fib[t];
      int M2 = L1 - fib[t];
      if ( !prob.containsKey(M1) ) prob.put(M1, calcProb(M1, U));
      if ( !prob.containsKey(M2) ) prob.put(M2, calcProb(M2, U));
      if ( prob.get(M1) < prob.get(M2) ) {
        L0 = M1;
      } else {
        L1 = M2;
      }
    }
    est_L = L0 + 1;

    cum_S = new long[in.N + 1];
    cum_width = new int[in.N + 1];
    for (int i = 0; i < in.N; i++) {
      cum_S[i + 1] = items[i].S() + cum_S[i];
      cum_width[i + 1] = items[i].w() + cum_width[i];
    }

    Item[] all_sorted = sortedItem(0, in.N);
    long cum = 0;
    for (int p = 0; p < all_sorted.length; p++) {
      Item it = all_sorted[p];
      it.rank_ratio = (cum + 0.5 * it.w()) / total_width();
      cum += it.w();
    }
    int pc = (int)Math.ceil(Math.sqrt(in.N));
    for (int p0 = 0; p0 < in.N; p0++) {
      Item[] sorted = sortedItem(p0, Math.min(p0 + pc, in.N));
      cum = 0;
      for ( Item it : sorted ) {
        if ( it.p() == p0 ) {
          it.nr_rank_ratio = (cum + 0.5 * it.w()) / cum_width(p0, Math.min(p0 + pc, in.N));
          break;
        }
        cum += it.w();
      }
    }
    near_w = new ArrayList<>();
    for (int i = 0; i < N - 1; i++) {
      if ( Math.abs(item(i).w() - item(i + 1).w()) <= Math.sqrt(item(i).x_variance()) * 1.0 ) {
        near_w.add(item(i));
      }
    }
  }

  private Item[] sortedItem(int p0, int p1) {
    Item[] ret = new Item[p1 - p0];
    for (int p = p0; p < p1; p++) {
      ret[p - p0] = item(p);
    }
    Arrays.sort(ret, comp);
    return ret;
  }
  int rest_width(int p) {
    return cum_width[in.N] - cum_width[p];
  }
  double calcProb(int L, int U) {
    double ret = 0;
    for (int i = 0; i < in.N; i++) {
      Item it = item(i);
      ret += logcdf(L, U, it.original_size(0), Math.sqrt(it.variance(0)));
      ret += logcdf(L, U, it.original_size(1), Math.sqrt(it.variance(1)));
    }
    return ret;
  }
  double logcdf(int L, int U, int z, double sigma) {
    double nu = ((double)(U - z)) / sigma;
    double nl = ((double)(L - z)) / sigma;
    double p = (NormalDist.cdf(nl) - NormalDist.cdf(nu)) / (U - L);
    return p != 0 ? Math.log(p) : -1e15;
  }

  int total_S_root() {
    return (int)Math.round(Math.sqrt(total_S()));
  }
  long total_S() {
    return cum_S[in.N];
  }
  int total_width() {
    return cum_width[in.N];
  }
  long cum_S(int p) {
    return cum_S[p + 1];
  }
  // p以下の累積幅
  int cum_width(int p) {
    return cum_width[p + 1];
  }
  // [p0, p1) の累積幅
  int cum_width(int p0, int p1) {
    return cum_width[p1] - cum_width[p0];
  }

  void calcTargetWidthRange() {
    State state_0 = greedy(new StateSimple(this, total_S_root() / 2, 2), false);
    State state_1 = greedy(new StateSimple(this, total_S_root() * 2, 2), false);
    max_f = state_0.max_f;
    min_f = state_1.max_f;
    min_tw = new int[max_f + 1];
    Arrays.fill(min_tw, Integer.MAX_VALUE);
    max_tw = new int[max_f + 1];
    min_tw[min_f] = state_1.target_width;
    max_tw[min_f] = state_1.target_width;
    min_tw[max_f] = state_0.target_width;
    max_tw[max_f] = state_0.target_width;
    for (int f = min_f; f < max_f; f++) {
      int min = 0;
      int max = min_tw[f];
      if ( max == Integer.MAX_VALUE ) continue;
      for (int f1 = f + 1 ; f1 <= max_f && min == 0; f1++) {
        min = max_tw[f1];
      }
      assert min < max;
      while ( max - min > 1 ) {
        int mid = (min + max) / 2;
        State s = greedy(new StateSimple(this, mid, 2), false);
        min_tw[s.max_f] = Math.min(min_tw[s.max_f], mid);
        max_tw[s.max_f] = Math.max(max_tw[s.max_f], mid);
        if ( s.max_f > f ) {
          min = mid;
        } else {
          max = mid;
        }
      }
    }
  }

  State greedy(State state0, boolean set_v0) {
    State state = state0;
    while ( !state.isCompleted() ) {
      Command com = null;
      while ( com == null ) {
        if ( state.rest_item_width() <= state.rest_width() ) {
          com = state.createCommand(0);
        } else {
          Item it = state.curItem();
          int rf = 0;
          double ratio = state.last_x1(0) - state.last_x1(1) < est_L * 2.0 ? it.nr_rank_ratio : it.rank_ratio;
          if ( ratio <= 0.5 ) {
            rf = state.cur_f % 2 == 0 ? 1 : 0;
          } else {
            rf = state.cur_f % 2 == 0 ? 0 : 1;
          }
          if ( rf == 1 && !state.canAdd(rf, 0) ) {
            rf = 0;
          }
          if ( rf == 0 && !state.canAdd(rf, 0) ) {
            state.nextStage();
            continue;
          }
          assert state.canAdd(rf, 0);
          com = state.createCommand(rf, set_v0);
        }
      }
      state.add(com);
    }
    return state;
  }
}

class Command {
  Item it = null; // 対象item
  int r = 0; // 0:そのまま, 1:回転
  int d = 0; // 0:U, 1:L
  Command base = null; // 基準item
  boolean relative = true;
  Command prevCom = null;
  int f = 0;
  int hashCode = 0;

  // 実行結果情報
  Coordinate z0 = null;
  private long x_variance = 0;
  double eval = 0;

  Command(Item it, int r, int d, Command base) {
    this.it = it;
    this.r = r;
    this.d = d;
    this.base = base;
    if ( d == 0 ) {
      this.z0 = new Coordinate(base == null ? 0 : base.x1(), -1);
      this.x_variance = (base == null ? 0 : base.x_variance()) + it.variance(r);
    } else {
      this.z0 = new Coordinate(-1, base == null ? 0 : base.y1());
      //TODO: varianceは必要に応じて対応
    }
    this.hashCode = Objects.hash(it.p(), r, d, b());
  }
  Command(Item it, int r, int d, Command base, int f, Command prevCom) {
    this(it, r, d, base);
    this.f = f;
    this.prevCom = prevCom;
  }
  Command(Item it, int r, int d, Command base, boolean relative) {
    this(it, r, d, base);
    this.relative = relative;
  }

  // Command(Command from) {
  //   this.it = from.it;
  //   this.r = from.r;
  //   this.d = from.d;
  //   this.base = from.base;
  //   this.hashCode = from.hashCode;
  //   this.z0 = from.z0;
  //   this.x_variance = from.x_variance;
  //   this.eval = from.eval;
  // }

  void setResult(int v0) {
    this.z0.setmax(1 - d, v0);
  }
  void setEval(double eval) {
    this.eval = eval;
  }
  int z1(int ax) {
    return z0.z(ax) + size(ax);
  }
  // void rotate() {
  //   this.r = 1 ^ r;
  //   this.hashCode = Objects.hash(it.p(), r, d, b());
  // }
  // void recalc() {
  //   if ( d == 0 ) {
  //     this.z0 = new Coordinate(base == null ? 0 : base.x1(), -1);
  //     this.x_variance = (base == null ? 0 : base.x_variance()) + it.x_variance();
  //   } else {
  //     this.z0 = new Coordinate(-1, base == null ? 0 : base.y1());
  //     //TODO: varianceは必要に応じて対応
  //   }
  // }
  // int error() { // 誤差
  //   return (int)Math.round(Math.sqrt(u_variance));
  // }

  public int p() { return it.p(); }
  public int r() { return relative ? it.r(r) : r; }
  public int d() { return d; }
  public int b() { return base == null ? -1 : base.p(); }

  public double eval() {
    return eval;
  }

  @Override
  public int hashCode() {
    return hashCode;
  }
  @Override
  public boolean equals(Object o) {
    if ( o instanceof Command ) {
      Command oc = (Command)o;
      return it.p() == oc.it.p() && r == oc.r && d == oc.d && b() == oc.b();
    } else {
      return false;
    }
  }

  int size(int ax) {
    return it.size(ax ^ r);
  }
  int w() {
    return size(0);
  }
  int h() {
    return size(1);
  }
  // int item_dsize() {
  //   return it.dsize();
  // }
  // int itemSize(int ax) {
  //   return it.size(ax ^ r);
  // }
  int u0() {
    if ( base == null ) return 0; // 起点がない場合は0
    return base.z1(d);
  }
  int u1() {
    return u0() + size(d);
  }
  int dv() {
    return size(1 - d);
  }

  int x0() {
    return z0.z(0);
  }
  int x0e(double sigma) {
    return (int)Math.round(x0() + sigma * Math.sqrt((base == null ? 0 : base.x_variance())));
  }
  int x1() {
    return x0() + size(0);
  }
  int x1e(double sigma) {
    return (int)Math.round(x1() + sigma * Math.sqrt(x_variance()));
  }
  int y0() {
    return z0.z(1);
  }
  int y1() {
    return y0() + size(1);
  }
  // このオブジェクトのx1がcom0のx1より確実に右にあるか？
  boolean isStrictlyRightTo(Command com0, double sigma) {
    if ( com0 == null ) return true;
    long var_sum = com0.x_variance() + this.x_variance;
    return com0.x1() + sigma * Math.sqrt(var_sum) <= this.x1();
  }
  // このオブジェクトがcom0とx方向のオーバーラップを持つか
  boolean hasOverlap(Command com0, double sigma) {
    if ( com0 == null ) return false;
    long var_sum0 = com0.x_variance() + this.x0_variance();
    if ( com0.x1() + sigma * Math.sqrt(var_sum0) <= this.x0() ) return false;
    long var_sum1 = com0.x0_variance() + this.x_variance();
    return com0.x0() < this.x1() + sigma * Math.sqrt(var_sum1);
  }
  // このオブジェクトの後ろにItemを付け足してtarget_widthに収まるか？
  boolean canAdd(Item it, int r, int target_width) {
    return x1() + it.size(r) <= target_width;
  }
  // このオブジェクトの後ろにItemを付け足してprevの右端より左側で収まるか？
  // boolean canAdd(Item it, int r, Command prev, double sigma) {
  //   if ( prev == null ) return false;
  //   long var_sum = prev.x_variance() + it.variance(r);
  //   return this.x1() + it.size(r) + sigma * Math.sqrt(var_sum) <= prev.x1();
  // }
  // baseコマンドの右側にItemを付け足してこのオブジェクトの右端にかぶらないか
  boolean checkRightIfAdd(Command base, Item it, int r, double sigma) {
    int x1 = (base == null ? 0 : base.x1()) + it.size(r);
    long var_sum = (base == null ? 0 : base.x_variance()) + it.variance(r) + this.x_variance();
    return x1 + sigma * Math.sqrt(var_sum) <= this.x1();
  }

  // x1側のvariance
  long x_variance() { // variance
    return x_variance;
  }
  // x0側のvariance
  long x0_variance() {
    return base == null ? 0 : base.x_variance();
  }

  @Override
  public String toString() {
    return String.format("%d %d %s %d", it.p(), r(), d == 0 ? "U" : "L", base == null ? -1 : base.p());
  }
}

class Query {
  ArrayList<Command> array = null;
  int ret_w = 0;
  int ret_h = 0;

  // hidden
  int real_w = 0;
  int real_h = 0;
  int real_score = 0;

  int hashCode = 0;

  Query() {
    array = new ArrayList<>();
  }
  void add(Command com) {
    array.add(com);
    hashCode = 31 * hashCode + com.hashCode();
  }
  int score() {
    return real_score != 0 ? real_score : (ret_w + ret_h);
  }
  long S() {
    return real_score != 0 ? ((long)real_w) * ((long)real_h) : (((long)ret_w) * ((long)ret_h));
  }

  @Override
  public int hashCode() {
    return hashCode;
  }

  @Override
  public boolean equals(Object o) {
    return hashCode() == o.hashCode();
  }

  @Override
  public String toString() {
    StringBuilder out = new StringBuilder();
    out.append(array.size());
    for (Command com : array) {
      out.append("\n");
      out.append(com.toString());
    }
    return out.toString();
  }
}

abstract class State {
  static final double SIGMA0 = 0.20; // 高さ計算時にオーバーラップとみなす判定で使用するsigma
  static final double SIGMA1 = 3.00; // 前の行の追い越し禁止判定で使用するsigma
  final ItemManager im;
  Command lastCommand = null;
  Item cur_item = null;
  int cur_f = 0; // 段数
  int max_f = 0;
  int target_width = 0;
  int right_h = 0;
  Coordinate maxz = null;
  int floors = 0;

  State(ItemManager im, int target_width, int floors) {
    this.im = im;
    this.cur_f = 0;
    this.target_width = target_width;
    this.maxz = new Coordinate();
    cur_item = im.item(0);
    this.floors = floors;
  }
  State(State from) {
    this.im = from.im;
    this.cur_item = from.cur_item;
    this.cur_f = from.cur_f;
    this.target_width = from.target_width;
    this.right_h = from.right_h;
    this.maxz = new Coordinate(from.maxz);
    this.lastCommand = from.lastCommand;
  }
  @Override
  public abstract State clone();

  Item curItem() {
    return cur_item;
  }
  boolean isCompleted() {
    return cur_item == null;
  }

  // rf: 現在段数からの相対段数(0=現在段)
  public final void add(Command com) {
    _add(com);
    cur_item = com.p() + 1 < im.N ? im.item(com.p() + 1) : null;
    maxz.setmax(0, com.x1());
    maxz.setmax(1, com.y1());
    lastCommand = com;
    max_f = Math.max(max_f, com.f);
  }
  // rf: 相対段数、 r:向き
  public final boolean canAdd(int rf, int r) {
    Command last = last(rf);
    if ( rf == 0 ) {
      if ( last == null ) return true;
      return last.canAdd(cur_item, r, target_width);
    } else {
      Command last0 = last(rf - 1);
      if ( last0 == null ) return false;
      return last0.checkRightIfAdd(last, cur_item, r, SIGMA1);
    }
  }
  public final Command createCommand(int rf, boolean set_v0) {
    return createCommand(rf, 0, set_v0);
  }
  public final Command createCommand(int rf, int r, boolean set_v0) {
    Command ret = new Command(cur_item, r, 0, last(rf), cur_f + rf, lastCommand);
    if ( set_v0 ) set_v0(ret);
    return ret;
  }
  public final Command createCommand(int rf) {
    return createCommand(rf, 0, true);
  }
  public final Command createCommand(int rf, int r) {
    return createCommand(rf, r, true);
  }
  public final int rf(int f) {
    return f - cur_f;
  }
  public final int last_x1(int rf) {
    Command last = last(rf);
    return last == null ? 0 : last.x1();
  }
  public final void nextStage() {
    int last_x0 = last_x1(-1);
    int last_x1 = last_x1(0);
    right_h = last_x0 < last_x1 ? right_h : last(-1).y1();
    _nextStage();
    cur_f++;
  }
  // 現在行の残り幅
  public final int rest_width() {
    int ret = 0;
    for (int rf = 0; rf < floors - 1; rf++) {
      ret += rest_width1(rf);
    }
    return ret;
  }
  public final int rest_width1(int rf) {
    return target_width - last_x1(rf);
  }
  // 残りのitem幅
  public final int rest_item_width() {
    return cur_item == null ? 0 : im.rest_width(cur_item.p());
  }
  // スコア
  public final int score() {
    return maxz.z(0) + maxz.z(1);
  }
  public final Query createQuery() {
    ArrayList<Command> arr = new ArrayList<>();
    for ( Command com = lastCommand ; com != null ; com = com.prevCom ) {
      arr.add(com);
    }
    Query query = new Query();
    for ( int i = 0 ; i < arr.size() ; i++ ) {
      query.add(arr.get(arr.size() - i - 1));
    }
    return query;
  }
  public abstract Command last(int rf);
  protected abstract void _add(Command com);
  protected abstract void _nextStage();
  protected abstract void set_v0(Command com);
}

class StateSimple extends State {
  Command[] floor_last = null;
  StateSimple(ItemManager im, int target_width, int floors) {
    super(im, target_width, floors);
    floor_last = new Command[floors + 1];
  }
  StateSimple(StateSimple from) {
    super(from);
    this.floor_last = Arrays.copyOf(from.floor_last, from.floor_last.length);
    this.floors = from.floors;
  }
  @Override
  public State clone() {
    return new StateSimple(this);
  }
  private int _fi(int rf) {
    return (rf + cur_f + 1) % floor_last.length;
  }
  @Override
  public Command last(int rf) {
    return floor_last[_fi(rf)];
  }
  @Override
  protected void _add(Command com) {
    floor_last[_fi(rf(com.f))] = com;
  }
  @Override
  protected void _nextStage() {
    floor_last[_fi(-1)] = null;
  }
  @Override
  protected void set_v0(Command com) {
    int rf = rf(com.f);
    Command com0 = last(rf - 1);
    int last0_x1 = com0 == null ? 0 : com0.x1();
    int v0 = last0_x1 <= com.x1() ? right_h : 0;
    for ( ; com0 != null ; com0 = com0.base ) {
      if ( !com.hasOverlap(com0, SIGMA0) ) {
        if ( com.x1() <= com0.x0() ) continue;
        break;
      }
      v0 = Math.max(v0, com0.y1());
    }
    com.setResult(v0);
  }
}

class StateArray extends State {
  ArrayList<Command>[] floor = null;
  StateArray(ItemManager im, int target_width, int floors) {
    super(im, target_width, floors);
    floor = new ArrayList[floors + 1];
  }
  StateArray(StateArray from) {
    super(from);
    floor = new ArrayList[floors + 1];
    for (int i = 0; i < floor.length; i++) {
      if ( from.floor[i] == null ) continue;
      floor[i] = new ArrayList<>(from.floor[i]);
    }
    this.floors = from.floors;
  }
  @Override
  public State clone() {
    return new StateArray(this);
  }
  private int _fi(int rf) {
    return (rf + cur_f + 1) % floor.length;
  }
  @Override
  public Command last(int rf) {
    int fi = _fi(rf);
    return floor[fi] == null || floor[fi].size() == 0 ? null : floor[fi].get(floor[fi].size() - 1);
  }
  @Override
  protected void _add(Command com) {
    int fi = _fi(rf(com.f));
    if ( floor[fi] == null ) floor[fi] = new ArrayList<>();
    floor[fi].add(com);
  }
  @Override
  protected void _nextStage() {
    floor[_fi(-1)] = null;
  }
  @Override
  protected void set_v0(Command com) {
    int rf = rf(com.f);
    int fi = _fi(rf - 1);
    int v0 = 0;
    if ( floor[fi] != null ) {
      for (int i = 0; i < floor[fi].size(); i++) {
        Command c = floor[fi].get(i);
        if ( !com.hasOverlap(c, SIGMA0) ) {
          if ( c.x1() <= com.x0() ) continue;
          break;
        }
        v0 = Math.max(v0, c.y1());
      }
    }
    com.setResult(v0);
  }
}

abstract class Solver {
  final Input in;
  final Main main;
  final ItemManager im;
  Solver(Input in, Main main) {
    this.in = in;
    this.main = main;
    this.im = in.im;
  }
  abstract Result solve();

  Result preQuery(Result res) {
    // int MIN_T = in.sigma_i <= 4000 ? in.T : 30;
    int MIN_T = 19;
    int REST_T = Math.min(in.T, MIN_T);
    for (int i = 0; i < in.T - REST_T; i++) {
      Query query = new Query();
      Item it = im.item(i % in.N);
      // main.println(String.format("# before: item= %s", it.toString()));
      query.add(new Command(it, 0, 0, null, false));
      main.query(query);
      it.addResult(query.ret_w, query.ret_h);
      // main.println(String.format("# feedback: w= %d, h= %d", query.ret_w, query.ret_h));
      // main.println(String.format("# after: item= %s", it.toString()));
      res.add(query);
    }
    im.recalc();
    return res;
  }

  Result execQuery(Result res, ArrayList<Candidate> candidates) {
    Candidate ecand = new Candidate(0, new Query(), 0);
    for (int t = 0; res.querySize() < in.T ; t++) {
      Candidate cand = ((t == 0 || !main.isEmergency()) && t < candidates.size()) ? candidates.get(t) : ecand;
      main.println(String.format("# estimateScore=%d, iter=%d", cand.edgeSum, cand.id));
      main.query(cand.query);
      res.add(cand.query);
    }
    return res;
  }
}

class Solver7 extends Solver {
  Solver7(Input in, Main main) {
    super(in, main);
  }

  @Override
  Result solve() {
    Result res = new Result(in);
    res = preQuery(res);
    int REST_T = in.T - res.querySize();
    HashSet<Candidate> candidates = new HashSet<>();
    ArrayList<Candidate> candArray = new ArrayList<>();
    int iterations = 0;
    int best_last_score = Integer.MAX_VALUE;
    while ( !main.isTimeOver() ) {
      iterations++;

      int target_width = 0;
      State state = null;
      int FLOORS = 0;
      if ( candArray.size() == REST_T && main.rndDouble() < 0.20 ) {
        state = createState0(candArray);
        target_width = state.target_width + main.rnd(50000) - 25000;
        FLOORS = state.floors;
      } else {
        // FLOORS = 2 + main.rnd(2);
        FLOORS = 3;
        double rr = 0.24;
        target_width = (int)(Math.sqrt(im.total_S()) * (main.rndDouble() * rr + 1.0 - rr / 2));
        // target_width = (int)(Math.sqrt(im.total_S()) * (main.rndDouble() * 0.3 + 0.90));
        state = new StateArray(im, target_width, FLOORS);
      }
      int[] dth = new int[FLOORS];
      int[] th0 = new int[FLOORS];  // th0 〜 th0+dth の乱数生成用
      if ( FLOORS == 2 ) {
        dth[0] = (Input.U - im.est_L) * 35 / 100;
        dth[1] = (Input.U - im.est_L) * 30 / 100;
        th0[0] = (Input.U - im.est_L) * 65 / 100 + im.est_L;
        th0[1] = (Input.U - im.est_L) * 50 / 100 + im.est_L;
      } else {
        int a = main.rndDouble() < 0.65 ? 1 : 0;
        dth[0] = (Input.U - im.est_L) * 35 / 100;
        dth[1 + a] = (Input.U - im.est_L) * 25 / 100;
        dth[2 - a] = (Input.U - im.est_L) * 35 / 100;
        th0[0] = (Input.U - im.est_L) * 66 / 100 + im.est_L;
        th0[1 + a] = (Input.U - im.est_L) * 45 / 100 + im.est_L;
        th0[2 - a] = (Input.U - im.est_L) * 70 / 100 + im.est_L;
      }
      int[] target_height = new int[FLOORS];
      for (int ri = 0; ri < FLOORS; ri++) {
        target_height[ri] = th0[ri] + main.rnd(dth[ri]);
      }

      Integer[] PRIOR = new Integer[FLOORS];
      for (int i = 0 ; i < FLOORS; i++) PRIOR[i] = i;
      int[] diff = new int[FLOORS];
      Comparator<Integer> comp = (j0, j1) -> {
        if ( diff[j0] != diff[j1] ) {
          return diff[j0] < diff[j1] ? -1 : 1;
        } else {
          return j0 - j1;
        }
      };

      LOOP:while (!state.isCompleted()) {
        Item it = state.curItem();
        int ha = it.h();
        int ri = state.cur_f % FLOORS;

        // int rf = 0;
        if ( state.rest_width1(0) < im.est_L ) {
          state.nextStage();
          continue;
        }
        Command com = null;
        if (state.rest_item_width() <= state.rest_width1(0)) {
          com = state.createCommand(0);
          if ( com.y1() > state.maxz.z(1) && state.rest_item_width() + it.h() - it.w() <= state.rest_width1(0) ) {
            com = state.createCommand(0, 1);
          }
        } else {
          for (int i = 0; i < FLOORS; i++) {
            diff[i] = Math.abs(ha - target_height[(ri + i) % FLOORS]);
          }
          Arrays.sort(PRIOR, comp);
          for (int rf : PRIOR) {
            if ( rf == 2 && state.rest_item_width() <= state.rest_width() ) continue;
            if ( rf == 0 ) {
              if ( !state.canAdd(rf, 0) ) {
                state.nextStage();
                target_height[ri] = th0[ri] + main.rnd(dth[ri]);
                continue LOOP;
              }
              com = state.createCommand(rf);
            } else {
              if ( !state.canAdd(rf, 0) ) {
                // rfに入れられないので、見送り
              } else {
                com = state.createCommand(rf);
              }
            }
            if ( com != null ) break;
          }
        }
        state.add(com);
        if ( state.score() > best_last_score ) break;
      }
      if ( candArray.size() < REST_T - 1 || state.score() < candArray.get(candArray.size() - 1).edgeSum ) {
        Candidate cand = new Candidate(state, iterations);
        if ( !candidates.contains(cand) ) {
          candidates.add(cand);
          candArray.add(cand);
          Collections.sort(candArray);
          while ( candArray.size() > REST_T ) {
            candArray.remove(candArray.size() - 1);
          }
        }
        if ( candArray.size() == REST_T ) {
          best_last_score = candArray.get(candArray.size() - 1).edgeSum;
        }
      }
    }
    // ArrayList<Candidate> candidates_arr = new ArrayList<>(candidates);
    // Collections.sort(candidates_arr);
    res = execQuery(res, candArray);
    res.stats().iterations = iterations;
    return res;
  }

  State createState0(ArrayList<Candidate> candArray) {
    Candidate cand0 = candArray.get(main.rnd(candArray.size()) / 3);
    // Candidate cand0 = candArray.get(main.rnd(10));
    // Candidate cand0 = candArray.get(main.rnd(5));
    // State state = new StateArray(im, cand0.target_width, 2);
    State state = new StateArray(im, cand0.target_width, cand0.FLOORS);
    int A = (int)Math.round(main.rnd(im.N) * 0.50 + im.N * 0.18);
    for (int i = 0; i < A; i++) {
      Command com0 = cand0.query.array.get(i);
      assert state.cur_f <= com0.f;
      if ( com0.f > state.cur_f + cand0.FLOORS - 1 ) {
      // if ( com0.f > state.cur_f + 1 ) {
        state.nextStage();
      }
      assert com0.f <= state.cur_f + cand0.FLOORS - 1;
      // assert com0.f <= state.cur_f + 1;
      Command com = state.createCommand(com0.f - state.cur_f, com0.r);
      state.add(com);
    }
    return state;
  }

  State createSwapState(ArrayList<Candidate> candArray) {
    Candidate cand0 = candArray.get(main.rnd(candArray.size()) / 2);
    ArrayList<Command> swap_cand = new ArrayList<>();
    for (Item it : im.near_w) {
      Command com0 = cand0.query.array.get(it.p());
      Command com1 = cand0.query.array.get(it.p() + 1);
      if ( com0.f != com1.f && com0.r() == 0 && com1.r() == 0 ) {
        swap_cand.add(com0);
      }
    }
    if ( swap_cand.size() == 0 ) return null;
    Command scom = swap_cand.get(main.rnd(swap_cand.size()));
    int[] ns = new int[im.N / 2];
    for ( Command com : cand0.query.array ) {
      ns[com.f] = com.p();
    }
    State state = new StateArray(im, cand0.target_width, cand0.FLOORS);
    for (int i = 0; i < im.N; i++) {
      if ( i == scom.p() ) {
        Command com0 = cand0.query.array.get(i);
        Command com1 = cand0.query.array.get(i + 1);
        if ( com1.f > state.cur_f + cand0.FLOORS - 1 ) return null;
        if ( !state.canAdd(com1.f - state.cur_f, com0.r) ) return null;
        state.add(state.createCommand(com1.f - state.cur_f, com0.r));
        if ( ns[com1.f] == com1.p() ) state.nextStage();

        if ( com0.f > state.cur_f + cand0.FLOORS - 1 ) return null;
        if ( !state.canAdd(com0.f - state.cur_f, com1.r) ) return null;
        state.add(state.createCommand(com0.f - state.cur_f, com1.r));
        if ( ns[com0.f] == com0.p() ) state.nextStage();
        i++;
      } else {
        Command com0 = cand0.query.array.get(i);
        assert com0.f <= state.cur_f + cand0.FLOORS - 1;
        if ( !state.canAdd(com0.f - state.cur_f, com0.r) ) return null;
        Command com = state.createCommand(com0.f - state.cur_f, com0.r);
        state.add(com);
        if ( ns[com0.f] == com0.p() ) {
          state.nextStage();
        }
      }
    }
    return state;
  }
}

class Solver8 extends Solver {
  Solver8(Input in, Main main) {
    super(in, main);
  }

  static final int PENA_MAX = Input.U * 200;
  static final int PENA_INF = Input.U * 400;
  @Override
  Result solve() {
    Result res = new Result(in);
    res = preQuery(res);
    int REST_T = in.T - res.querySize();
    HashSet<Candidate> candidates = new HashSet<>();
    ArrayList<Candidate> candArray = new ArrayList<>();
    int iterations = 0;
    int best_last_score = Integer.MAX_VALUE;
    while ( !main.isTimeOver() ) {
      iterations++;

      int target_width = 0;
      State state = null;
      int FLOORS = 0;
      int target_floors = 0;
      int[] target_y = null;
      if ( candArray.size() == REST_T && main.rndDouble() < 0.20 ) {
        state = createState0(candArray);
        target_width = state.target_width + main.rnd(50000) - 25000;
        FLOORS = state.floors;
        target_floors = FLOORS;
      } else {
        // if ( main.rndDouble() < 0.8 ) {
          double rr = 0.25;
          target_width = (int)(Math.sqrt(im.total_S()) * (main.rndDouble() * rr + 1.0 - rr / 2));
          target_floors = im.total_width() / target_width + 1;
        // } else {
        //   target_floors = (im.total_width() / im.total_S_root()) + main.rnd(2);
        //   target_width = (int)Math.round(im.total_width() / target_floors + main.rndDouble() * im.est_L * 0.4 + 2000.0);
        // }
        FLOORS = target_floors;
        state = new StateSimple(im, target_width, FLOORS);
      }
      target_y = new int[target_floors * 2];
      ArrayList<Integer> sample_h = new ArrayList<>(target_floors);
      int RH = Input.U - im.est_L;
      for (int f = 0; f < target_floors; f++) {
        sample_h.add(Math.max(main.rnd(RH), main.rnd(RH)) + im.est_L);
      }
      // Collections.sort(sample_h);
      int[] target_hr = new int[target_floors];
      // int r0 = target_floors - main.rnd(3) - 1;
      // target_hr[0] = sample_h.remove(r0);
      // target_hr[1] = target_hr[0] + sample_h.remove(0);
      for (int f = 0; f < target_floors; f++) {
        target_hr[f] = sample_h.remove(main.rnd(sample_h.size())) + (f == 0 ? 0 : target_hr[f - 1]);
      }
      // double goal_r = candArray.size() > 0 ? candArray.get(0).edgeSum
      double target_ymax = Math.round(im.total_S() * 1.1 / target_width);
      if ( candArray.size() > 0 ) {
        target_ymax = (candArray.get(0).edgeSum - target_width) * 0.998;
      }
      for (int f = 0; f < target_floors; f++) {
        target_y[f] = (int)Math.round(target_ymax * target_hr[f] / target_hr[target_floors - 1]);
      }
      for (int f = target_floors; f < target_y.length; f++) {
        target_y[f] = target_y[f - 1] + 1;
      }

      Integer[] PRIOR = new Integer[FLOORS];
      for (int i = 0 ; i < FLOORS; i++) PRIOR[i] = i;
      int[] diff = new int[FLOORS];
      Command[] candCom = new Command[FLOORS];
      Comparator<Integer> comp = (j0, j1) -> {
        if ( diff[j0] != diff[j1] ) {
          return diff[j0] < diff[j1] ? -1 : 1;
        } else {
          return j0 - j1;
        }
      };

      LOOP:while (!state.isCompleted()) {
        Item it = state.curItem();
        int f = state.cur_f;

        if ( state.rest_width1(0) < im.est_L ) {
          state.nextStage();
          continue;
        }
        Command com = null;
        if (state.rest_item_width() <= state.rest_width1(0)) {
          com = state.createCommand(0);
          if ( com.y1() > state.maxz.z(1) && state.rest_item_width() + it.h() - it.w() <= state.rest_width1(0) ) {
            com = state.createCommand(0, 1);
          }
        } else {
          int rw = 0;
          int riw = state.rest_item_width();
          for (int rf = 0; rf < FLOORS; rf++) {
            if ( f + rf >= target_y.length ) {
              diff[rf] = PENA_MAX + rf;
              candCom[rf] = null;
            } else if ( riw <= rw ) {
              diff[rf] = PENA_INF + rf;
              candCom[rf] = null;
            } else {
              if ( state.canAdd(rf, 0) ) {
                candCom[rf] = state.createCommand(rf);
                if ( f + rf < target_floors - 1 || candCom[rf].y1() <= target_y[f + rf]) {
                  diff[rf] = Math.abs(candCom[rf].y1() - target_y[f + rf]);
                } else {
                  diff[rf] = PENA_MAX + rf;
                }
              } else {
                diff[rf] = PENA_MAX + rf;
                candCom[rf] = null;
              }
            }
            rw += state.rest_width1(rf);
          }
          Arrays.sort(PRIOR, comp);
          for (int rf : PRIOR) {
            if ( rf == 0 ) {
              if ( !state.canAdd(rf, 0) ) {
                state.nextStage();
                continue LOOP;
              }
              com = candCom[rf] == null ? state.createCommand(rf) : candCom[rf];
            } else {
              if ( !state.canAdd(rf, 0) ) {
                // assert false;
                // rfに入れられないので、見送り
              } else {
                com = candCom[rf] == null ? state.createCommand(rf) : candCom[rf];
              }
            }
            if ( com != null ) break;
          }
        }
        state.add(com);
        if ( state.score() > best_last_score ) break;
      }
      if ( candArray.size() < REST_T - 1 || state.score() < candArray.get(candArray.size() - 1).edgeSum ) {
        Candidate cand = new Candidate(state, iterations);
        if ( !candidates.contains(cand) ) {
          candidates.add(cand);
          candArray.add(cand);
          Collections.sort(candArray);
          while ( candArray.size() > REST_T ) {
            candArray.remove(candArray.size() - 1);
          }
        }
        if ( candArray.size() == REST_T ) {
          best_last_score = candArray.get(candArray.size() - 1).edgeSum;
        }
      }
    }
    res = execQuery(res, candArray);
    res.stats().iterations = iterations;
    return res;
  }

  State createState0(ArrayList<Candidate> candArray) {
    Candidate cand0 = candArray.get(main.rnd(candArray.size()) / 3);
    State state = new StateSimple(im, cand0.target_width, cand0.FLOORS);
    double UPPER = 0.68;
    double LOWER = 0.18;
    int A = (int)Math.round(main.rnd(im.N) * (UPPER - LOWER) + im.N * LOWER);
    for (int i = 0; i < A; i++) {
      Command com0 = cand0.query.array.get(i);
      assert state.cur_f <= com0.f;
      if ( com0.f > state.cur_f + cand0.FLOORS - 1 ) {
        state.nextStage();
      }
      assert com0.f <= state.cur_f + cand0.FLOORS - 1;
      Command com = state.createCommand(com0.f - state.cur_f, com0.r);
      state.add(com);
    }
    return state;
  }
}

class Candidate implements Comparable<Candidate> {
  Query query = null;
  int edgeSum = 0;
  int target_width = 0;
  int FLOORS = 0;
  int id = 0;

  int hashCode = 0;

  Candidate(int edgeSum, Query query, int id) {
    this.query = query;
    this.edgeSum = edgeSum;
    this.id = id;
    this.hashCode = Objects.hash(query, edgeSum);
  }
  Candidate(State state, int id) {
    this.query = state.createQuery();
    this.edgeSum = state.score();
    this.target_width = state.target_width;
    this.FLOORS = state.floors;
    this.id = id;
    this.hashCode = Objects.hash(query, edgeSum);
  }

  @Override
  public int hashCode() {
    return hashCode;
  }
  @Override
  public boolean equals(Object o) {
    if ( o instanceof Candidate ) {
      Candidate oc = (Candidate)o;
      return query.hashCode() == oc.query.hashCode() && edgeSum == oc.edgeSum;
    } else {
      return false;
    }
  }

  @Override
  public int compareTo(Candidate o) {
    if ( edgeSum != o.edgeSum ) {
      return edgeSum < o.edgeSum ? -1 : 1;
    } else {
      return id - o.id;
    }
  }
}

class Result {
  final Input in;
  ResultStats stats = null;
  ArrayList<Query> queries = null;

  Result(Input in) {
    this.in = in;
    this.queries = new ArrayList<>();
  }

  void add(Query query) {
    queries.add(query);
    createStatsIfNotExists();
    if ( query.score() < stats.score ) {
      stats.score = query.score();
      stats.best_iter = queries.size();
      stats.S = query.S();
    }
  }
  int querySize() {
    return queries.size();
  }
  ResultStats stats() {
    createStatsIfNotExists();
    return stats;
  }
  private void createStatsIfNotExists() {
    if ( stats == null ) stats = new ResultStats();
  }

  // @Override
  // long eval() {
  //   return stats == null ? 0 : stats.score;
  // }

  @Override
  public String toString() {
    StringBuilder out = new StringBuilder();
    //TODO: 結果出力
    return out.toString();
  }
}
class ResultStats {
  int caseNum = 0;
  long elapsed = 0;
  int iterations = 0;
  long score = Long.MAX_VALUE;
  int best_iter = 0;
  long S = 0;
  double occupancy = 0;
}
class NormalDist {
  private static final double [] CDF = {
    5.000e-01,4.960e-01,4.920e-01,4.880e-01,4.840e-01,4.801e-01,4.761e-01,4.721e-01,4.681e-01,4.641e-01,4.602e-01,4.562e-01,4.522e-01,4.483e-01,4.443e-01,4.404e-01,4.364e-01,4.325e-01,4.286e-01,4.247e-01,4.207e-01,4.168e-01,4.129e-01,4.090e-01,4.052e-01,4.013e-01,3.974e-01,3.936e-01,3.897e-01,3.859e-01,3.821e-01,3.783e-01,3.745e-01,3.707e-01,3.669e-01,3.632e-01,3.594e-01,3.557e-01,3.520e-01,3.483e-01,3.446e-01,3.409e-01,3.372e-01,3.336e-01,3.300e-01,3.264e-01,3.228e-01,3.192e-01,3.156e-01,3.121e-01,3.085e-01,3.050e-01,3.015e-01,2.981e-01,2.946e-01,2.912e-01,2.877e-01,2.843e-01,2.810e-01,2.776e-01,2.743e-01,2.709e-01,2.676e-01,2.643e-01,2.611e-01,2.578e-01,2.546e-01,2.514e-01,2.483e-01,2.451e-01,2.420e-01,2.389e-01,2.358e-01,2.327e-01,2.296e-01,2.266e-01,2.236e-01,2.206e-01,2.177e-01,2.148e-01,2.119e-01,2.090e-01,2.061e-01,2.033e-01,2.005e-01,1.977e-01,1.949e-01,1.922e-01,1.894e-01,1.867e-01,1.841e-01,1.814e-01,1.788e-01,1.762e-01,1.736e-01,1.711e-01,1.685e-01,1.660e-01,1.635e-01,1.611e-01,1.587e-01,1.562e-01,1.539e-01,1.515e-01,1.492e-01,1.469e-01,1.446e-01,1.423e-01,1.401e-01,1.379e-01,1.357e-01,1.335e-01,1.314e-01,1.292e-01,1.271e-01,1.251e-01,1.230e-01,1.210e-01,1.190e-01,1.170e-01,1.151e-01,1.131e-01,1.112e-01,1.093e-01,1.075e-01,1.056e-01,1.038e-01,1.020e-01,1.003e-01,9.853e-02,9.680e-02,9.510e-02,9.342e-02,9.176e-02,9.012e-02,8.851e-02,8.691e-02,8.534e-02,8.379e-02,8.226e-02,8.076e-02,7.927e-02,7.780e-02,7.636e-02,7.493e-02,7.353e-02,7.215e-02,7.078e-02,6.944e-02,6.811e-02,6.681e-02,6.552e-02,6.426e-02,6.301e-02,6.178e-02,6.057e-02,5.938e-02,5.821e-02,5.705e-02,5.592e-02,5.480e-02,5.370e-02,5.262e-02,5.155e-02,5.050e-02,4.947e-02,4.846e-02,4.746e-02,4.648e-02,4.551e-02,4.457e-02,4.363e-02,4.272e-02,4.182e-02,4.093e-02,4.006e-02,3.920e-02,3.836e-02,3.754e-02,3.673e-02,3.593e-02,3.515e-02,3.438e-02,3.362e-02,3.288e-02,3.216e-02,3.144e-02,3.074e-02,3.005e-02,2.938e-02,2.872e-02,2.807e-02,2.743e-02,2.680e-02,2.619e-02,2.559e-02,2.500e-02,2.442e-02,2.385e-02,2.330e-02,2.275e-02,2.222e-02,2.169e-02,2.118e-02,2.068e-02,2.018e-02,1.970e-02,1.923e-02,1.876e-02,1.831e-02,1.786e-02,1.743e-02,1.700e-02,1.659e-02,1.618e-02,1.578e-02,1.539e-02,1.500e-02,1.463e-02,1.426e-02,1.390e-02,1.355e-02,1.321e-02,1.287e-02,1.255e-02,1.222e-02,1.191e-02,1.160e-02,1.130e-02,1.101e-02,1.072e-02,1.044e-02,1.017e-02,9.903e-03,9.642e-03,9.387e-03,9.137e-03,8.894e-03,8.656e-03,8.424e-03,8.198e-03,7.976e-03,7.760e-03,7.549e-03,7.344e-03,7.143e-03,6.947e-03,6.756e-03,6.569e-03,6.387e-03,6.210e-03,6.037e-03,5.868e-03,5.703e-03,5.543e-03,5.386e-03,5.234e-03,5.085e-03,4.940e-03,4.799e-03,4.661e-03,4.527e-03,4.396e-03,4.269e-03,4.145e-03,4.025e-03,3.907e-03,3.793e-03,3.681e-03,3.573e-03,3.467e-03,3.364e-03,3.264e-03,3.167e-03,3.072e-03,2.980e-03,2.890e-03,2.803e-03,2.718e-03,2.635e-03,2.555e-03,2.477e-03,2.401e-03,2.327e-03,2.256e-03,2.186e-03,2.118e-03,2.052e-03,1.988e-03,1.926e-03,1.866e-03,1.807e-03,1.750e-03,1.695e-03,1.641e-03,1.589e-03,1.538e-03,1.489e-03,1.441e-03,1.395e-03,1.350e-03,1.306e-03,1.264e-03,1.223e-03,1.183e-03,1.144e-03,1.107e-03,1.070e-03,1.035e-03,1.001e-03,9.676e-04,9.354e-04,9.043e-04,8.740e-04,8.447e-04,8.164e-04,7.888e-04,7.622e-04,7.364e-04,7.114e-04,6.871e-04,6.637e-04,6.410e-04,6.190e-04,5.976e-04,5.770e-04,5.571e-04,5.377e-04,5.190e-04,5.009e-04,4.834e-04,4.665e-04,4.501e-04,4.342e-04,4.189e-04,4.041e-04,3.897e-04,3.758e-04,3.624e-04,3.495e-04,3.369e-04,3.248e-04,3.131e-04,3.018e-04,2.909e-04,2.803e-04,2.701e-04,2.602e-04,2.507e-04,2.415e-04,2.326e-04,2.241e-04,2.158e-04,2.078e-04,2.001e-04,1.926e-04,1.854e-04,1.785e-04,1.718e-04,1.653e-04,1.591e-04,1.531e-04,1.473e-04,1.417e-04,1.363e-04,1.311e-04,1.261e-04,1.213e-04,1.166e-04,1.121e-04,1.078e-04,1.036e-04,9.961e-05,9.574e-05,9.201e-05,8.842e-05,8.496e-05,8.162e-05,7.841e-05,7.532e-05,7.235e-05,6.948e-05,6.673e-05,6.407e-05,6.152e-05,5.906e-05,5.669e-05,5.442e-05,5.223e-05,5.012e-05,4.810e-05,4.615e-05,4.427e-05,4.247e-05,4.074e-05,3.908e-05,3.747e-05,3.594e-05,3.446e-05,3.304e-05,3.167e-05,3.036e-05,2.910e-05,2.789e-05,2.673e-05,2.561e-05,2.454e-05,2.351e-05,2.252e-05,2.157e-05,2.066e-05,1.978e-05,1.894e-05,1.814e-05,1.737e-05,1.662e-05,1.591e-05,1.523e-05,1.458e-05,1.395e-05,1.335e-05,1.277e-05,1.222e-05,1.168e-05,1.118e-05,1.069e-05,1.022e-05,9.774e-06,9.345e-06,8.934e-06,8.540e-06,8.163e-06,7.801e-06,7.455e-06,7.124e-06,6.807e-06,6.503e-06,6.212e-06,5.934e-06,5.668e-06,5.413e-06,5.169e-06,4.935e-06,4.712e-06,4.498e-06,4.294e-06,4.098e-06,3.911e-06,3.732e-06,3.561e-06,3.398e-06,3.241e-06,3.092e-06,2.949e-06,2.813e-06,2.682e-06,2.558e-06,2.439e-06,2.325e-06,2.216e-06,2.112e-06,2.013e-06,1.919e-06,1.828e-06,1.742e-06,1.660e-06,1.581e-06,1.506e-06,1.434e-06,1.366e-06,1.301e-06,1.239e-06,1.179e-06,1.123e-06,1.069e-06,1.017e-06,9.680e-07,9.211e-07,8.765e-07,8.339e-07,7.933e-07,7.547e-07,7.178e-07,6.827e-07,6.492e-07,6.173e-07,5.869e-07,5.580e-07,5.304e-07,5.042e-07,4.792e-07,4.554e-07,4.327e-07,4.111e-07,3.906e-07,3.711e-07,3.525e-07,3.348e-07,3.179e-07,3.019e-07,2.867e-07,2.722e-07,2.584e-07,2.452e-07,2.328e-07,2.209e-07,2.096e-07,1.989e-07,1.887e-07,1.790e-07
  };

  static double cdf(double sigma) {
    return sigma >= 0 ? _cdf(sigma) : 1.0 - _cdf(-sigma);
  }
  private static double _cdf(double asigma) {
    if ( asigma >= 5.0 ) return CDF[500];
    double a100 = asigma * 100.0;
    int i0 = (int)a100;
    return CDF[i0] * (i0 + 1 - a100) + CDF[i0 + 1] * (a100 - i0);
  }
}

class FastScanner {
  private final InputStream in;
  private final byte[] buf = new byte[1024];
  private int ptr = 0;
  private int buflen = 0;
  FastScanner( InputStream source ) { this.in = source; }
  private boolean hasNextByte() {
    if ( ptr < buflen ) return true;
    else {
      ptr = 0;
      try { buflen = in.read(buf); } catch (IOException e) { e.printStackTrace(); }
      if ( buflen <= 0 ) return false;
    }
    return true;
  } 
  private int readByte() { if ( hasNextByte() ) return buf[ptr++]; else return -1; } 
  private boolean isPrintableChar( int c ) { return 33 <= c && c <= 126; }
  private boolean isNumeric( int c ) { return '0' <= c && c <= '9'; }
  private void skipToNextPrintableChar() { while ( hasNextByte() && !isPrintableChar(buf[ptr]) ) ptr++; }
  public boolean hasNext() { skipToNextPrintableChar(); return hasNextByte(); }
  public String next() {
    if ( !hasNext() ) throw new NoSuchElementException();
    StringBuilder ret = new StringBuilder();
    int b = readByte();
    while ( isPrintableChar(b) ) { ret.appendCodePoint(b); b = readByte(); }
    return ret.toString();
  }
  public long nextLong() {
    if ( !hasNext() ) throw new NoSuchElementException();
    long ret = 0;
    int b = readByte();
    boolean negative = false;
    if ( b == '-' ) { negative = true; if ( hasNextByte() ) b = readByte(); }
    if ( !isNumeric(b) ) throw new NumberFormatException();
    while ( true ) {
      if ( isNumeric(b) ) ret = ret * 10 + b - '0';
      else if ( b == -1 || !isPrintableChar(b) ) return negative ? -ret : ret;
      else throw new NumberFormatException();
      b = readByte();
    }
  }
  public int nextInt() { return (int)nextLong(); }
  public double nextDouble() { return Double.parseDouble(next()); }
}
