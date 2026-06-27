import java.io.IOException;
import java.io.InputStream;
import java.io.PrintStream;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.BitSet;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.NoSuchElementException;
import java.util.Random;

class Main {
  static final long TL = 2300;

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
  boolean isTimeOver() { return elapsed() >= timeLimit(); }

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
    
    Result best = null;
    // Solver sol = null;
    // sol = new GridSolver(in, this);
    // best = sol.solve(best);

    ArrayList<Solver> solvers = createSolverList(in);
    for ( Solver sol : solvers ) {
      if ( best != null && isTimeOver() ) break;
      best = sol.solve(best);
      // if ( best != null ) break;
    }

    printResult(best);
    return best;
  }

  public void printResult(Result result) {
    println(result.toString());
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

  ArrayList<Solver> createSolverList(Input in) {
    int ai = Math.min((in.LA - 600) / 100, 5);
    int bi = in.LB - 4;
    String s = SOL_MAP[ai][bi];
    ArrayList<Solver> ret = new ArrayList<>();
    for ( int i = 0 ; i < s.length() ; i++ ) {
      ret.add(createSolver(s.charAt(i), in));
    }
    return ret;
  }

  Solver createSolver(char s, Input in) {
    if ( s == 'F' ) {
      return new HubSolver(in, this, new HubFixed1Structure(in.g, this));
    } else if ( s == 'S' ) {
      return new HubSolver(in, this, new HubSquareStructure(in.g, this));
    } else if ( s == 'T' ) {
      return new HubSolver(in, this, new HubTriangleStructure(in.g, this));
    } else if ( s == 'L' ) {
      return new HubSolver(in, this, new HubLine1Structure(in.g, this));
    } else if ( s == 'W' ) {
      return new HubSolver(in, this, new HubLongLineStructure(in.g, this));
    } else {
      return new GridSolver(in, this);
    }
  }

  static final String[][] SOL_MAP = {
    {"NSLTWF","NLTSWF","SNLTWF","LSNTFW","FSLNTW","SLNWFT","LFWSNT","LFSWNT","FLWSTN","FLWSTN","FLTWSN","FLWTSN","FTLWSN","TFLWSN","FTLWSN","FTLWSN","FTLWSN","TFLWSN","TFSLWN","FTLWSN","FTLWNS"},
    {"NLSTWF","NLSTWF","SNTLWF","LSTNWF","STFLNW","SLFTWN","FLSWTN","FLSTWN","FLWSTN","FLTWSN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTSLWN","FTLWSN","FTLWSN","FTLWSN","FTSWLN"},
    {"LTNSWF","NLSTWF","LNSTWF","TLSWNF","TSLFNW","TSLWFN","SLFTWN","FLWSTN","FLTSWN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTSLWN","FTSWLN","FTLWSN","FTLWSN","FTSLWN"},
    {"NLTSWF","TSLNWF","TLSNWF","LSTNWF","TLSNFW","TSLFWN","TSFLWN","FLTSWN","FTLSWN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTSLWN","FTLWSN","FTWLSN","FTSLWN"},
    {"LSTWNF","TLSWNF","LTSWNF","TLSWNF","SLWTFN","TLSWFN","TLSFWN","TFLSWN","FTLSWN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTWLSN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","TFLWSN","FTLWSN"},
    {"LTSWNF","LTSWNF","LSTWNF","LTSWNF","TSLWFN","SLTFWN","TSLWFN","FTLSWN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTLWSN","FTLSWN","FTLWSN","FTLSWN","FTLWSN","FTLWSN","FTLWSN"}
  };
}
class Input {
  int N = 0;
  int M = 0;
  int T = 0;
  int LA = 0;
  int LB = 0;
  int[] u = null;
  int[] v = null;
  int[] t = null;
  int[] x = null;
  int[] y = null;

  Graph g = null;
  TargetInfo[] tinfo = null;

  Integer[] integers = null;
  static final int MAX_C = 100000;

  public void readParameter(FastScanner sc) {
    N = sc.nextInt();
    M = sc.nextInt();
    T = sc.nextInt();
    LA = sc.nextInt();
    LB = sc.nextInt();
    u = new int[M];
    v = new int[M];
    for (int i = 0; i < M; i++) {
      u[i] = sc.nextInt();
      v[i] = sc.nextInt();
    }
    t = new int[T];
    for (int i = 0; i < T; i++) {
      t[i] = sc.nextInt();
    }
    x = new int[N];
    y = new int[N];
    for (int i = 0; i < N; i++) {
      x[i] = sc.nextInt();
      y[i] = sc.nextInt();
    }
  }

  public void preCalc() {
    g = new Graph(this);
    g.prepare();

    tinfo = new TargetInfo[T + 1];
    tinfo[0] = new TargetInfo(g.town[0], null, g.town[t[0]], 0);
    g.town[0].targetInfo.add(tinfo[0]);
    for (int i = 0; i < T; i++) {
      tinfo[i + 1] = new TargetInfo(g.town[t[i]], i == 0 ? g.town[0] : g.town[t[i - 1]], i + 1 < T ? g.town[t[i + 1]] : null, i + 1);
      g.town[t[i]].targetInfo.add(tinfo[i + 1]);
    }

    integers = new Integer[MAX_C];
    for (int i = 0; i < MAX_C; i++) integers[i] = i;
  }
}

class Graph {
  final Input in;
  final Town[] town;
  final int N;
  ArrayList<Edge> allEdges = null;
  Edge[][] edge_table = null;
  HashSet<Town> boundary = null;
  Face boundary_face = null;
  ArrayList<Face> allFaces = null;
  ArrayList<Edge> main_boundary_cycle = null;
  Town[] corner = null; // 0:左上, 1:右上, 2:左下, 3:右下

  // final ToIntFunction<Edge> E2DIST = (e) -> {
  //   return 100 - e.to.targetCount();
  // };

  Town[] town_sorted = null;
  UnionFind townUf = null;
  UnionFind faceUf = null;
  int maxConnectedCnt = 0;
  int connectedCnt1 = 0;

  int maxConnectedFaceCnt = 0;

  private DistMap[] dmap = null;
  int min_MaxDepth = 99;

  Graph(Input in) {
    this.in = in;
    this.N = in.N;
    this.town = new Town[in.N];
    for (int i = 0; i < in.N; i++) {
      town[i] = new Town(i, in.x[i], in.y[i]);
    }
    allEdges = new ArrayList<>();
    edge_table = new Edge[N][N];
    for (int i = 0; i < in.M; i++) {
      Edge e0 = new Edge(town[in.u[i]], town[in.v[i]], allEdges.size());
      Edge e1 = new Edge(town[in.v[i]], town[in.u[i]], allEdges.size() + 1);
      e0.rev = e1;
      e1.rev = e0;
      town[e0.from.id].edge.add(e0);
      town[e1.from.id].edge.add(e1);
      allEdges.add(e0);
      allEdges.add(e1);
      edge_table[e0.from.id][e0.to.id] = e0;
      edge_table[e1.from.id][e1.to.id] = e1;
    }
    dmap = new DistMap[in.N];
    for (int i = 0; i < N; i++) {
      dmap[i] = new DistMap(this, town[i]);
    }
    town_sorted = Arrays.copyOf(town, N);
    Arrays.sort(town_sorted, (t0, t1) -> {
      if ( t0.dist0 != t1.dist0 ) {
        return t0.dist0 < t1.dist0 ? -1 : 1;
      } else {
        return t0.id - t1.id;
      }
    });
    // for (Town t : town) {
    //   dmap[t.id] = new DistMap(this);
    //   dmap[t.id].createMap(t);
    //   min_MaxDepth = Math.min(min_MaxDepth, dmap[t.id].maxDepth);
    // }

  }
  // DistMap dmap(Town t) {
  //   if ( dmap[t.id] == null ) {
  //     dmap[t.id] = new DistMap(this);
  //     dmap[t.id].createMap(t);
  //     min_MaxDepth = Math.min(min_MaxDepth, dmap[t.id].maxDepth);
  //   }
  //   return dmap[t.id];
  // }
  int dist(Town t0, Town t) {
    return dmap[t0.id].dist(t);
  }
  ArrayList<Town> parent(Town t0, Town t) {
    return dmap[t0.id].parent(t);
  }
  ArrayList<Town> child(Town t0, Town t) {
    return dmap[t0.id].child(t);
  }
  int maxDepth(Town t0) {
    return dmap[t0.id].maxDepth();
  }
  Town maxTgtAncParent(Town t0, Town t) {
    return dmap[t0.id].maxTgtAncParent(t);
  }
  Town nearest(Town t0, HashSet<Town> goal) {
    return dmap[t0.id].nearest(goal);
  }

  void prepare() {
    for (int i = 0; i < in.N; i++) {
      town[i].sort_adj();
    }

    // create clockwise edge
    for ( Town t : town ) {
      for (int i = 0; i < t.edge.size(); i++) {
        t.edge(i).rev.cwe = t.edge(i + 1);
        t.edge(i + 1).rev.ucwe = t.edge(i);
      }
    }
    // create face
    allFaces = new ArrayList<>();
    for ( Edge e0 : allEdges ) {
      if ( e0.face != null ) continue;
      Face f = new Face(allFaces.size());
      f.addEdge(e0);
      while ( true ) {
        Edge e_last = f.lastEdge();
        if ( f.firstEdge() == e_last.cwe ) break;
        f.addEdge(e_last.cwe); 
      }
      f.calcG();
      allFaces.add(f);
    }

    // boundary
    Town t0 = town[0];
    for (Town t : town) {
      if ( t.x < t0.x ) t0 = t;
    }
    boundary_face = t0.edge(0).face;
    boundary = new HashSet<>();
    for ( Edge e : boundary_face.cycle ) boundary.add(e.to);

    // 連結成分を整理する
    townUf = new UnionFind(N);
    for ( Edge e : allEdges ) {
      if ( e.from.id > e.to.id ) continue;
      if ( e.face == e.rev.face ) continue;
      townUf.unite(e.from.id, e.to.id);
    }
    for ( int i = 0 ; i < N ; i++ ) {
      maxConnectedCnt = Math.max(maxConnectedCnt, townUf.size(i));
    }
    // boolean[] checked = new boolean[N];
    for (int i = 0; i < N; i++) {
      // if ( checked[townUf.root(i)] ) continue;
      if ( townUf.size(i) == maxConnectedCnt ) continue;
      if ( townUf.size(i) == 1 ) connectedCnt1++;
    }

    faceUf = new UnionFind(allFaces.size());
    for ( Face f : allFaces ) {
      if ( f == boundary_face ) continue;
      for ( Edge e : f.cycle ) {
        if ( e.rev.face == boundary_face ) continue;
        faceUf.unite(f.id, e.rev.face.id);
      }
    }
    for ( int i = 0 ; i < allFaces.size() ; i++ ) {
      maxConnectedFaceCnt = Math.max(maxConnectedFaceCnt, faceUf.size(i));
    }
    main_boundary_cycle = new ArrayList<>();
    for (int i = 0; i < boundary_face.cycle.size(); i++) {
      Edge e = boundary_face.cycle.get(i);
      if ( isMainLandEdgeByFace(e) ) main_boundary_cycle.add(e);
    }

    corner = new Town[4];
    int[][] coord = new int[][] {{0, 0}, {1000, 0}, {0, 1000}, {1000, 1000}};
    for ( Edge e : main_boundary_cycle ) {
      Town t = e.to;
      for (int k = 0; k < 4; k++) {
        if ( corner[k] == null || t.dist(coord[k][0], coord[k][1]) < corner[k].dist(coord[k][0], coord[k][1]) ) {
          corner[k] = t;
        }
      }
    }

    calcAps();
  }

  Edge edge(Town from, Town to) {
    return edge_table[from.id][to.id];
  }

  boolean isMainLandByBridge(Town t) {
    return townUf.size(t.id) == maxConnectedCnt;
  }
  boolean isMainLandEdgeByFace(Edge e) {
    return faceUf.size(e.face.id) == maxConnectedFaceCnt || faceUf.size(e.rev.face.id) == maxConnectedFaceCnt;
  }

  ArrayList<Town> simpleBFS(int ti0, int ti1) {
    ArrayDeque<Step> queue = new ArrayDeque<>();
    Step[] map = new Step[in.N];
    map[ti0] = new Step(ti0, null, 0);
    queue.add(map[ti0]);
    while ( queue.size() > 0 ) {
      Step s = queue.pollFirst();
      if ( s.i == ti1 ) break; 
      for ( Edge e : town[s.i].edge ) {
        Town to = e.to;
        if ( map[to.id] == null ) {
          map[to.id] = new Step(to.id, s, s.d + 1);
          queue.add(map[to.id]);
        }
      }
    }
    return map[ti1].createTownRoute();
  }

  ArrayList<Town> createPath(Town t0, Town t1, boolean includeStart) {
    ArrayList<Town> ret = new ArrayList<>();
    Town cur = t1;
    while ( cur != null ) {
      ret.add(cur);
      cur = maxTgtAncParent(t0, cur);
    }
    if ( !includeStart ) ret.remove(ret.size() - 1);
    Collections.reverse(ret);
    return ret;
  }
  ArrayList<Town> createPath(Town t0, Town t1) {
    return createPath(t0, t1, true);
  }


  Town findNearest(int x, int y) {
    int best_d = Integer.MAX_VALUE;
    Town ret = null;
    for ( Town t : boundary ) {
      int d = t.dist2(x, y);
      if ( d < best_d ) {
        best_d = d;
        ret = t;
      }
    }
    return ret;
  }

  class Step implements Comparable<Step> {
    int i = 0;
    int d = 0;
    Step parent = null;

    Step(int i, Step parent, int d) {
      this.i = i;
      this.parent = parent;
      this.d = d;
    }
    ArrayList<Town> createTownRoute() {
      ArrayList<Town> ret = null;
      if ( parent != null ) {
        ret = parent.createTownRoute();
      } else {
        ret = new ArrayList<>();
      }
      ret.add(town[i]);
      return ret;
    }
    ArrayList<Step> createStepRoute() {
      ArrayList<Step> ret = null;
      if ( parent != null ) {
        ret = parent.createStepRoute();
      } else {
        ret = new ArrayList<>();
      }
      ret.add(this);
      return ret;
    }

    @Override
    public int compareTo(Step o) {
      if ( d != o.d ) {
        return d - o.d;
      } else {
        return i - o.i;
      }
    }
  }

  // 関節点を求める
  void calcAps() {
    boolean[] visited = new boolean[N];
    int[] ord = new int[N];
    int[] low = new int[N];
    dfsForAps(town[0], 0, null, ord, low, visited);
  }

  BitSet aps = null;
  int[] ord = null;
  int[] low = null;
  int dfsForAps(Town t, int k, Town par, int[] ord, int[] low, boolean[] visited) {
    visited[t.id] = true;
    ord[t.id] = k++;
    low[t.id] = ord[t.id];
    boolean flg = false;
    int cnt = 0;
    for ( Edge e : t.edge ) {
      Town a = e.to;
      if ( !visited[a.id] ) {
        cnt++;
        k = dfsForAps(a, k, t, ord, low, visited);
        low[t.id] = Math.min(low[t.id], low[a.id]);
        if ( par != null && ord[t.id] <= low[a.id] ) flg = true;
      } else if ( a != par ) {
        low[t.id] = Math.min(low[t.id], ord[a.id]);
      }
    }
    if ( par == null && cnt >= 2 ) flg = true;
    if ( flg ) t.aps = true;
    return k;
  }
}

class Edge {
  final Town from;
  final Town to;
  final int id;
  Edge rev = null;
  Edge cwe = null; // clockwise edge   this.to == cwe.from
  Edge ucwe = null; // unti clockwise edge   this.to == ucwe.from
  Face face = null;
  Edge(Town from, Town to, int id) {
    this.from = from;
    this.to = to;
    this.id = id;
  }
}
class Face {
  ArrayList<Edge> cycle = null;
  int id = 0;
  int gx = 0;
  int gy = 0;
  Face(int id) {
    this.id = id;
    cycle = new ArrayList<>();
  }
  @Override
  public int hashCode() {
    return id;
  }
  @Override
  public boolean equals(Object o) {
    if ( o instanceof Face ) {
      return id == ((Face)o).id;
    }
    return false;
  }
  void addEdge(Edge e) {
    cycle.add(e);
    e.face = this;
  }
  Edge lastEdge() {
    return cycle.get(cycle.size() - 1);
  }
  Edge firstEdge() {
    return cycle.get(0);
  }
  void calcG() {
    int tot_x = 0;
    int tot_y = 0;
    for ( Edge e : cycle ) {
      tot_x += e.to.x;
      tot_y += e.to.y;
    }
    gx = tot_x / cycle.size();
    gy = tot_y / cycle.size();
  }
}

class Town {
  int id = 0;
  int x = 0;
  int y = 0;
  ArrayList<Edge> edge = null;
  ArrayList<TargetInfo> targetInfo = null;
  double dist0 = 0;
  double theta0 = 0;
  boolean aps = false; // 関節点

  Town(int id, int x, int y) {
    this.id = id;
    this.x = x;
    this.y = y;
    this.edge = new ArrayList<>();
    this.targetInfo = new ArrayList<>();
    this.dist0 = dist(500, 500);
    this.theta0 = Math.atan2(this.y - 500, this.x - 500);
  }
  @Override
  public int hashCode() {
    return id;
  }
  @Override
  public boolean equals(Object o) {
    if ( o instanceof Town ) {
      return id == ((Town)o).id;
    }
    return false;
  }
  Edge edge(int i) {
    return edge.get(i % edge.size());
  }
  void sort_adj() {
    Collections.sort(edge, (e0, e1) -> {
      Town t0 = e0.to;
      Town t1 = e1.to;
      double th0 = Math.atan2(t0.y - y, t0.x - x);
      double th1 = Math.atan2(t1.y - y, t1.x - x);
      if ( th0 != th1 ) {
        return th0 < th1 ? -1 : 1;
      } else {
        return t0.id - t1.id;
      }
    });
  }
  int targetCount() {
    return targetInfo.size();
  }
  boolean isTarget() {
    return targetInfo.size() > 0;
  }
  int dist2(Town t) {
    return dist2(t.x, t.y);
  }
  int dist2(int x0, int y0) {
    return (x0 - x) * (x0 - x) + (y0 - y) * (y0 - y);
  }
  double dist(int x0, int y0) {
    return Math.sqrt(dist2(x0, y0));
  }
  @Override
  public String toString() {
    return String.format("id:%d,x:%d,y:%d,#adj:%d", id, x, y, edge.size());
  }
}
class TargetInfo {
  final Town prev;
  final Town next;
  final Town target;
  final int id;
  int dist2 = 0;
  TargetInfo(Town target, Town prev, Town next, int id) {
    this.prev = prev;
    this.next = next;
    this.target = target;
    this.id = id;
    if ( prev != null ) {
      dist2 = prev.dist2(target);
    }
  }
}

class DistMap {
  final Graph g;
  final Town t0;
  int[] depth = null;
  int[] ancTgtCnt = null;
  int maxDepth = 0;
  ArrayList<Town>[] parent = null;
  Town[] maxTgtAncParent = null;
  ArrayList<Town>[] child = null;
  boolean isComplete = false;
  DistMap(Graph g, Town t0) {
    this.g = g;
    this.t0 = t0;
  }

  ArrayDeque<Town> queue = null;
  private void search(Town goal_t) {
    if ( goal_t != null && isSearched(goal_t) ) return;
    if ( depth == null ) {
      depth = new int[g.N];
      ancTgtCnt = new int[g.N];
      Arrays.fill(ancTgtCnt, -1);
      parent = new ArrayList[g.N];
      child = new ArrayList[g.N];
      queue = new ArrayDeque<>();
      maxTgtAncParent = new Town[g.N];
      Arrays.fill(depth, Integer.MAX_VALUE);
      depth[t0.id] = 0;
      ancTgtCnt[t0.id] = 0;
      queue.addLast(t0);
      maxDepth = 0;
    }
    while ( queue.size() > 0 ) {
      Town t = queue.pollFirst();
      if ( t.isTarget() ) ancTgtCnt[t.id]++;
      int d = depth[t.id];
      maxDepth = Math.max(maxDepth, d);
      for ( Edge e : t.edge ) {
        boolean deep = false;
        if ( d + 1 < depth[e.to.id] ) {
          depth[e.to.id] = d + 1;
          queue.addLast(e.to);
          deep = true;
        } else if ( d + 1 == depth[e.to.id] ) {
          deep = true;
        }
        if ( deep ) {
          addParent(t, e.to);
          if ( ancTgtCnt[t.id] > ancTgtCnt[e.to.id] ) {
            ancTgtCnt[e.to.id] = ancTgtCnt[t.id];
            maxTgtAncParent[e.to.id] = t;
          }
        }
      }
      if ( goal_t != null && t.id == goal_t.id ) {
        return;
      }
    }
    isComplete = true;
  }
  Town nearest(HashSet<Town> goal) {
    if ( goal != null ) {
      Town best = null;
      for ( Town t : goal ) {
        if ( isSearched(t) ) {
          if ( best == null || dist(t) < dist(best) ) {
            best = t;
          }
        }
      }
      if ( best != null ) return best;
    }
    if ( depth == null ) {
      depth = new int[g.N];
      ancTgtCnt = new int[g.N];
      Arrays.fill(ancTgtCnt, -1);
      parent = new ArrayList[g.N];
      child = new ArrayList[g.N];
      queue = new ArrayDeque<>();
      maxTgtAncParent = new Town[g.N];
      Arrays.fill(depth, Integer.MAX_VALUE);
      depth[t0.id] = 0;
      ancTgtCnt[t0.id] = 0;
      queue.addLast(t0);
      maxDepth = 0;
    }
    while ( queue.size() > 0 ) {
      Town t = queue.pollFirst();
      if ( t.isTarget() ) ancTgtCnt[t.id]++;
      int d = depth[t.id];
      maxDepth = Math.max(maxDepth, d);
      for ( Edge e : t.edge ) {
        boolean deep = false;
        if ( d + 1 < depth[e.to.id] ) {
          depth[e.to.id] = d + 1;
          queue.addLast(e.to);
          deep = true;
        } else if ( d + 1 == depth[e.to.id] ) {
          deep = true;
        }
        if ( deep ) {
          addParent(t, e.to);
          if ( ancTgtCnt[t.id] > ancTgtCnt[e.to.id] ) {
            ancTgtCnt[e.to.id] = ancTgtCnt[t.id];
            maxTgtAncParent[e.to.id] = t;
          }
        }
      }
      if ( goal != null && goal.contains(t) ) {
        return t;
      }
    }
    isComplete = true;
    return null;
  }

  private void addParent(Town p, Town c) {
    if ( parent[c.id] == null ) parent[c.id] = new ArrayList<>();
    parent[c.id].add(p);
    if ( child[p.id] == null ) child[p.id] = new ArrayList<>();
    child[p.id].add(c);
  }
  private boolean isSearched(Town t) {
    return depth != null && depth[t.id] < Integer.MAX_VALUE;
  }
  int dist(Town t) {
    if ( !isSearched(t) ) search(t);
    return depth[t.id];
  }
  int maxDepth() {
    if ( !isComplete ) search((Town)null);
    return maxDepth;
  }
  Town maxTgtAncParent(Town t) {
    if ( !isSearched(t) ) search(t);
    assert depth[t.id] == 0 || maxTgtAncParent[t.id] != null;
    return maxTgtAncParent[t.id];
  }
  ArrayList<Town> parent(Town t) {
    if ( !isSearched(t) ) search(t);
    return parent[t.id];
  }
  ArrayList<Town> child(Town t) {
    if ( !isSearched(t) ) search(t);
    return child[t.id];
  }
}
interface Command {
}
class Signal implements Command {
  int l = 0;
  int PA = 0;
  int PB = 0;
  int si = 0;

  Signal(int l, int PA, int PB) {
    this.l = l;
    this.PA = PA;
    this.PB = PB;
  }
  Signal(int l, int PA, int PB, int si) {
    this(l, PA, PB);
    this.si = si;
  }

  @Override
  public String toString() {
    return String.format("s %d %d %d", l, PA, PB);
  }
}
class Move implements Command {
  int v = 0;
  Move(int v) {
    this.v = v;
  }
  @Override
  public String toString() {
    return String.format("m %d", v);
  }
}

class Controller {
  Graph g;
  int LA = 0;
  int[] A = null; // size=LA, value= town_id
  int[][] aid_map = null; // size=N*各townの登場回数, value= Aのid

  private FastBitSet fixedTid = null;
  private int fixedCnt = 0;
  private int[] fixedFid = null;  // tid -> fid 0-indexed
  private ArrayList<Town> fixedTown = null; // fid -> town
  private Town[] fixedAdjTown = null; // faid -> town
  private int[] fixedAdjId = null; // tid -> faid
  private int fixedAdjCnt = 0;

  Controller(Input in, int[] A) {
    this.g = in.g;
    this.A = A;
    this.LA = in.LA;
    ArrayList<Integer>[] arr = new ArrayList[in.N];
    for (int i = 0; i < in.N; i++) arr[i] = new ArrayList<>();
    for (int i = 0; i < in.LA; i++) arr[A[i]].add(i);
    aid_map = new int[in.N][];
    for (int i = 0; i < in.N; i++) {
      aid_map[i] = new int[arr[i].size()];
      for (int j = 0; j < arr[i].size(); j++) aid_map[i][j] = arr[i].get(j);
    }
    fixedTid = new FastBitSet(in.N);
    fixedTown = new ArrayList<>();
  }
  int[] aid(Town t) {
    return aid_map[t.id];
  }
  Town town(int aid) {
    return g.town[A[aid]];
  }
  void setFixed(Town t) {
    if ( !fixedTid.get(t.id) ) {
      fixedTid.set(t.id);
      fixedTown.add(t);
      fixedCnt++;
    }
  }
  void unsetFixed(Town t) {
    if ( fixedTid.get(t.id) ) {
      fixedTid.clear(t.id);
      fixedTown.remove(t);
      fixedCnt--;
    }
  }
  boolean isFixed(Town t) {
    return fixedTid.get(t.id);
  }
  int fixedCnt() {
    return fixedCnt;
  }

  void fixedCommit() {
    // fixedの隣接townを求める
    HashSet<Town> adj = new HashSet<>();
    fixedFid = new int[g.N];
    Arrays.fill(fixedFid, -1);
    for ( int i = 0 ; i < fixedTown.size() ; i++ ) {
      Town t = fixedTown.get(i);
      fixedFid[t.id] = i;
      for ( Edge e : t.edge ) {
        if ( isFixed(e.to) ) continue;
        adj.add(e.to);
      }
    }
    fixedAdjTown = (Town[]) adj.toArray(new Town[adj.size()]);
    fixedAdjCnt = fixedAdjTown.length;
    fixedAdjId = new int[g.N];
    Arrays.fill(fixedAdjId, -1);
    for ( int i = 0 ; i < fixedAdjCnt ; i++ ) {
      Town t = fixedAdjTown[i];
      fixedAdjId[t.id] = i;
    }
  }
  int fidForFixed(Town t) {
    assert isFixed(t);
    return fixedFid[t.id];
  }
  Town fixedTownByFid(int fid) {
    return fixedTown.get(fid);
  }
  int faidForFixedAdj(Town t) {
    return fixedAdjId[t.id];
  }
  Town fixedAdjTownByFaid(int faid) {
    return fixedAdjTown[faid];
  }

  int vaidForFixed(Town ftown, Town atown) {
    return g.in.LA + fidForFixed(ftown) * fixedAdjCnt + faidForFixedAdj(atown);
  }
  Town fixedTownByVaid(int vaid) {
    int fid = (vaid - g.in.LA) / fixedAdjCnt;
    return fixedTownByFid(fid);
  }
  Town fixedAdjTownByVaid(int vaid) {
    int faid = (vaid - g.in.LA) % fixedAdjCnt;
    return fixedAdjTownByFaid(faid);
  }
  int virtualLA() {
    return g.in.LA + fixedCnt * fixedAdjCnt;
  }
  int fixedAdjCnt() {
    return fixedAdjCnt;
  }

  Town adjNonFixedTown(Town t0) {
    assert isFixed(t0);
    ArrayDeque<Town> queue = new ArrayDeque<>();
    FastBitSet visited = new FastBitSet(g.N);
    visited.set(t0.id);
    queue.add(t0);
    while ( queue.size() > 0 ) {
      Town t = queue.pollFirst();
      assert isFixed(t);
      for ( Edge e : t.edge ) {
        if ( visited.get(e.to.id) ) continue;
        if ( aid_map[e.to.id].length == 0 ) continue;
        if ( isFixed(e.to) ) {
          queue.addLast(e.to);
          visited.set(e.to.id);
        } else {
          return e.to;
        }
      }
    }
    assert false;
    return null;
  }
  
  // boolean isAdjacent(int i0, int ti) {
  //   return (i0 > 0 && A[i0 - 1] == ti) || (i0 < LA - 1 && A[i0 + 1] == ti);
  // }
}
class ArrayElement implements Comparable<ArrayElement> {
  Town t = null;
  int aeid = 0;
  int eval = 0; // 大きい方が必要な点

  ArrayElement a0 = null; // 前後のelement
  ArrayElement a1 = null; // 前後のelement
  ArrayElement(Town t, int aeid, int eval) {
    this.t = t;
    this.aeid = aeid;
    this.eval = eval;
  }

  ArrayElement next(ArrayElement prev) {
    return a0 == prev ? a1 : a0;
  }
  ArrayElement prev(ArrayElement next) {
    return a1 == next ? a0 : a1;
  }

  void addNext(ArrayElement next) {
    assert this.t.id != next.t.id;
    this._addNext(next);
    next._addPrev(this);
  }
  void addPrev(ArrayElement prev) {
    assert this.t.id != prev.t.id;
    this._addPrev(prev);
    prev._addNext(this);
  }
  private void _addNext(ArrayElement next) {
    if ( a1 == null ) {
      a1 = next;
    } else {
      assert a0 == null;
      a0 = next;
    }
  }
  private void _addPrev(ArrayElement prev) {
    if ( a0 == null ) {
      a0 = prev;
    } else {
      assert a1 == null;
      a1 = prev;
    }
  }
  void removePrev(ArrayElement prev) {
    this._removePrev(prev);
    prev._removeNext(this);
  }
  void removeNext(ArrayElement next) {
    this._removeNext(next);
    next._removeNext(this);
  }
  private void _removePrev(ArrayElement prev) {
    if ( a0 == prev ) {
      a0 = null;
    } else {
      assert a1 == prev;
      a1 = null;
    }
  }
  private void _removeNext(ArrayElement next) {
    if ( a1 == next ) {
      a1 = null;
    } else {
      assert a0 == next;
      a0 = null;
    }
  }

  @Override
  public boolean equals(Object o) {
    if ( o instanceof ArrayElement ) {
      return aeid == ((ArrayElement)o).aeid;
    }
    return false;
  }
  @Override
  public int compareTo(ArrayElement o) {
    if ( eval != o.eval ) {
      return eval < o.eval ? -1 : 1;
    } else {
      return -aeid + o.aeid;
    }
  }
}
class ArraySegment {
  ArrayElement first = null;
  ArrayElement last = null;
  int size = 0;
  ArraySegment() {
  }

  void addNext(ArrayElement a) {
    if ( last == null ) {
      assert first == null;
      first = a;
      last = a;
    } else {
      assert first != null;
      last.addNext(a);
      last = a;
    }
    size++;
    assert first == null || first.a0 == null || first.a1 == null;
    assert last == null || last.a0 == null || last.a1 == null;
  }
  void addPrev(ArrayElement a) {
    if ( first == null ) {
      assert last == null;
      first = a;
      last = a;
    } else {
      assert last != null;
      first.addPrev(a);
      first = a;
    }
    size++;
    assert first == null || first.a0 == null || first.a1 == null;
    assert last == null || last.a0 == null || last.a1 == null;
  }
  void reverse() {
    ArrayElement tmp = first;
    first = last;
    last = tmp;
  }
  void join(ArraySegment seg) {
    this.last.addNext(seg.first);
    this.last = seg.last;
    assert first == null || first.a0 == null || first.a1 == null;
    assert last == null || last.a0 == null || last.a1 == null;
    size += seg.size;
  }
  ArrayElement popFirst() {
    ArrayElement ret = null;
    if ( first != null ) {
      ArrayElement next = first.next(null);
      if ( next != null ) {
        first.removeNext(next);
        ret = first;
        first = next;
        assert first == null || first.a0 == null || first.a1 == null;
      } else {
        ret = first;
        first = null;
        last = null;
      }
      size--;
    } else {
      assert last == null;
      ret = null;
    }
    assert first == null || first.a0 == null || first.a1 == null;
    assert last == null || last.a0 == null || last.a1 == null;
    return ret;
  }
  ArrayElement popLast() {
    ArrayElement ret = null;
    if ( last != null ) {
      ArrayElement prev = last.prev(null);
      if ( prev != null ) {
        last.removePrev(prev);
        ret = last;
        last = prev;
        assert last == null || last.a0 == null || last.a1 == null;
      } else {
        ret = last;
        first = null;
        last = null;
      }
      size--;
    } else {
      assert first == null;
      ret = null;
    }
    assert first == null || first.a0 == null || first.a1 == null;
    assert last == null || last.a0 == null || last.a1 == null;
    return ret;
  }

  ArrayElement otherEnd(ArrayElement end) {
    return end.t.id == first.t.id ? last : first;
  }
}

class ArrayElementManager {
  final Graph g;
  ArrayList<ArraySegment> segment = null;
  int segmentSize = 0;

  FastBitSet town_used = null;
  FastBitSet edge_used = null;
  int aeid = 0;
  ArrayDeque<ArrayElement> queue = new ArrayDeque<>();

  ArrayElementManager(Graph g) {
    this.g = g;
    this.town_used = new FastBitSet(g.N);
    this.edge_used = new FastBitSet(g.allEdges.size());
    this.segment = new ArrayList<>();
  }
  ArrayElement createElement(Town t, int eval) {
    if ( !town_used.get(t.id) ) town_used.set(t.id);
    return new ArrayElement(t, aeid++, eval);
  }
  ArraySegment createSegmentFromPath(ArrayList<Town> arr, int base_eval) {
    ArraySegment seg = new ArraySegment();
    queue.clear();
    ArrayElement prev = null;
    for ( Town t : arr ) {
      int ev = base_eval;
      if ( !town_used.get(t.id) ) ev = 0;
      ArrayElement ae = createElement(t, ev);
      boolean firstEdge = false;
      if ( prev != null && g.edge(prev.t, t) != null ) {
        Edge e = normal(g.edge(prev.t, t));
        if ( edge_used.get(e.id) ) {
          if ( queue.size() == 0 ) queue.add(prev);
          queue.addLast(ae);
        } else {
          firstEdge = true;
        }
        edge_used.set(e.id);
      } else {
        firstEdge = true;
      }
      if ( firstEdge ) {
        int edge_cont_pena = queue.size() * 100;
        while ( queue.size() > 0 ) {
          queue.poll().eval -= edge_cont_pena;
        }
      }
      seg.addNext(ae);
      prev = ae;
    }
    int edge_cont_pena = queue.size() * 100;
    while ( queue.size() > 0 ) {
      queue.poll().eval -= edge_cont_pena;
    }
    return seg;
  }

  void addSegment(ArraySegment seg) {
    assert seg.size != 0;
    segment.add(seg);
    segmentSize += seg.size;
  }
  void addAllSegment(ArrayList<ArraySegment> segs) {
    for ( ArraySegment seg : segs ) addSegment(seg);
  }
  private Edge normal(Edge e) {
    return e.from.id < e.to.id ? e : e.rev;
  }

  // void reduceEndpoint() {
  //   ArraySegment[] seg_arr = new ArraySegment[g.N];
  //   ArrayList<ArraySegment> narr = new ArrayList<>();
  //   int n_tot_size = 0;
  //   for ( ArraySegment seg : segment) {
  //     ArrayElement ae0 = seg.first;
  //     ArrayElement ae1 = seg.last;
  //     if ( seg_arr[ae0.t.id] != null ) {
  //       // ae0側の結合処理
  //       ArraySegment seg0 = seg_arr[ae0.t.id];
  //       if ( seg0.last.t.id != ae0.t.id ) seg0.reverse();
  //       assert seg0.last.t.id == ae0.t.id;
  //       seg.popFirst();
  //       seg0.join(seg);
  //       seg_arr[ae0.t.id] = null;
  //       seg = seg0;
  //       if ( seg.last.t.id == ae1.t.id ) {
  //         seg_arr[ae1.t.id] = seg;
  //         continue;
  //       }
  //       //TODO: lastの処理
  //       if ( seg_arr[ae1.t.id] != null && seg_arr[ae1.t.id] != seg ) {
  //         // ae1側の結合処理
  //         ArraySegment seg1 = seg_arr[ae1.t.id];
  //         if ( seg1.last.t.id != ae1.t.id ) seg1.reverse();
  //         assert seg1.last.t.id == ae1.t.id;

  //       }
  //     }
  //   }
  // }

  void reduceEndpoint() {
    ArrayList<ArraySegment>[] seg_arr = new ArrayList[g.N];
    ArrayList<ArraySegment> nsegment = new ArrayList<>();
    for ( ArraySegment seg : segment ) {
      if ( seg.size == 1 ) {
        nsegment.add(seg);
        continue;
      }
      assert seg.first != seg.last;
      if ( seg_arr[seg.first.t.id] == null ) seg_arr[seg.first.t.id] = new ArrayList<>();
      if ( seg_arr[seg.last.t.id] == null ) seg_arr[seg.last.t.id] = new ArrayList<>();
      seg_arr[seg.first.t.id].add(seg);
      seg_arr[seg.last.t.id].add(seg);
    }

    for ( int ti = 0 ; ti < g.N ; ti++ ) {
      if ( seg_arr[ti] == null ) continue;
      // if ( seg_arr[ti].size() <= 1 ) continue;
      LOOP:for ( int i0 = 0 ; i0 < seg_arr[ti].size() ; i0++ ) {
        ArraySegment seg0 = seg_arr[ti].get(i0);
        if ( seg0 == null ) continue;
        ArrayElement a0 = seg0.first.t.id == ti ? seg0.first : seg0.last;
        assert a0.t.id == ti;
        ArrayElement b0 = seg0.otherEnd(a0);
        for ( Edge e : g.town[ti].edge ) {
          if ( seg_arr[e.to.id] == null ) continue;
          int ti1 = e.to.id;
          for (int i1 = 0; i1 < seg_arr[ti1].size() ; i1++) {
            ArraySegment seg1 = seg_arr[ti1].get(i1);
            if ( seg1 == null || seg1 == seg0 ) continue;
            ArrayElement a1 = seg1.first.t.id == ti1 ? seg1.first : seg1.last;
            assert a1.t.id == ti1;
            if ( seg1.otherEnd(a1).t.id == b0.t.id ) continue;
            ArrayElement b1 = seg1.otherEnd(a1);
            if ( seg0.first == a0 ) seg0.reverse();
            if ( seg1.last == a1 ) seg1.reverse();
            // seg1.popFirst();
            assert seg0.last == a0;
            assert seg1.first == a1;
            seg0.join(seg1);
            assert seg0.first.a0 == null || seg0.first.a1 == null;
            assert seg0.last.a0 == null || seg0.last.a1 == null;
            // segmentSize--;
            seg_arr[ti].set(i0, null);
            seg_arr[ti1].set(i1, null);
            int i2 = seg_arr[b1.t.id].indexOf(seg1);
            seg_arr[b1.t.id].set(i2, seg0);
            // for ( int ti2 = 0 ; ti2 < g.N ; ti2++ ) {
            //   if ( seg_arr[ti2] == null ) continue;
            //   for ( ArraySegment seg : seg_arr[ti2] ) {
            //     if ( seg == null ) continue;
            //     if ( seg.first != null && seg.first.a0 != null && seg.first.a1 != null ) {
            //       System.err.println("Error!!");
            //     }
            //     if ( seg.last != null && seg.last.a0 != null && seg.last.a1 != null ) {
            //       System.err.println("Error!!");
            //     }
            //   }
            // }
            continue LOOP;
          }
        }
      }
    }
    for ( int ti = 0 ; ti < g.N ; ti++ ) {
      if ( seg_arr[ti] == null ) continue;
      for ( ArraySegment seg : seg_arr[ti] ) {
        if ( seg == null ) continue;
        ArrayElement a0 = seg.first.t.id == ti ? seg.first : seg.last;
        ArrayElement b0 = seg.otherEnd(a0);
        seg_arr[b0.t.id].remove(seg);
        nsegment.add(seg);
      }
    }
    segment = nsegment;
  }
}

class GraphStructure {
  final Input in;
  final Graph g;
  final Main main;

  ArrayElementManager aem = null;
  // ArrayList<Town> atown = null;

  GraphStructure(Graph g, Main main) {
    this.g = g;
    this.in = g.in;
    this.main = main;
    // this.atown = new ArrayList<>();
    this.aem = new ArrayElementManager(g);
  }

  int[] createA() {
    int[] ret = new int[in.LA];
    // for (int i = 0; i < Math.min(in.LA, atown.size()); i++) {
    //   ret[i] = atown.get(i).id;
    // }
    int ai = 0;
    for ( ArraySegment seg : aem.segment ) {
      ArrayElement e0 = null;
      ArrayElement e = seg.first;
      while ( e != null ) {
        ret[ai++] = e.t.id;
        ArrayElement next = e.next(e0);
        e0 = e;
        e = next;
      }
    }
    return ret;
  }
  void createDirectPath(int rest, int limitLB, HashSet<Town> fixedTown) {
    // HashSet<Town> fixedTownHash = new HashSet<>(fixedTown);
    HashSet<Town> b = new HashSet<>();
    Town cur = g.town[0];
    int buf = 0;
    ArraySegment seg = new ArraySegment();
    for (int i = 0; i < in.T; i++) {
      if ( rest <= 0 ) break;
      Town goal = g.town[in.t[i]];
      Town t0 = b.size() > 0 ? g.nearest(goal, b) : cur;
      ArrayList<Town> path = g.createPath(goal, t0);
      for (int j = path.size() - 2; j >= 0; j--) {
        if ( rest <= 0 ) break;
        Town t = path.get(j);
        if ( !fixedTown.contains(t) ) {
          if ( buf >= limitLB ) {
            buf = 0;
            b.clear();
          }
          b.add(t);
          buf++;
          rest--;
          // atown.add(t);
          seg.addNext(aem.createElement(t, 0));
        }
        cur = t;
      }
    }
    aem.addSegment(seg);
  }

  String memo() {
    return "";
  }
}

class GridStructure extends GraphStructure {
  // ArrayElementManager aem = null;

  int DIVx = 0;
  int DIVy = 0;
  int[] DIV = null;
  ArrayList<Town> outerEdge = null;
  FastBitSet outerEdgeBs = null;

  GridStructure(Graph g, Main main, int DIVx, int DIVy) {
    super(g, main);
    this.DIVx = DIVx;
    this.DIVy = DIVy;
    this.DIV = new int[]{DIVx, DIVy};
    // this.aem = new ArrayElementManager(g);
  }

  int[] createGrid(int limitLA) {
    // 周囲のhighway作成
    ArrayList<Town>[] out_highway = createOuterHighway();
    ArrayList<Town>[][] grid_highway = new ArrayList[2][];
    grid_highway[0] = new ArrayList[DIVy + 2];
    grid_highway[1] = new ArrayList[DIVx + 2];
    grid_highway[0][0] = out_highway[0];
    grid_highway[0][DIVy + 1] = out_highway[1];
    grid_highway[1][0] = out_highway[2];
    grid_highway[1][DIVx + 1] = out_highway[3];

    int[][] divi = new int[2][];
    int[][] divp = new int[2][];
    for (int dir = 0; dir < 2; dir++) {
      divi[dir] = new int[DIV[dir]];
      divp[dir] = new int[DIV[dir]];
      ArrayDeque<int[]> queue = new ArrayDeque<>();
      queue.add(new int[]{0, DIV[dir] + 1, 0});
      int i = 0;
      while ( queue.size() > 0 ) {
        int[] a = queue.pollFirst();
        int m = (a[1] - a[0]) / 2 + a[0];
        divi[dir][i] = m;
        divp[dir][i++] = a[2] + 1;
        if ( m - a[0] > 1 ) queue.addLast(new int[] {a[0], m, a[2] + 1});
        if ( a[1] - m > 1 ) queue.addLast(new int[] {m, a[1], a[2] + 1});
      }
    }

    // gridのhighway作成
    for (int i = 0; i < Math.max(DIVx, DIVy); i++) {
      for (int dir = 0; dir < 2; dir++) { // 0:h方向の線を引く, 1:v方向の線を引く
        if ( i >= DIV[1 - dir] ) continue;
        int k = divi[1 - dir][i];
        Town t0 = divTown(grid_highway[1 - dir][0], k, DIV[1 - dir]);
        Town t1 = divTown(grid_highway[1 - dir][(DIV[dir] + 1) / 2], k, DIV[1 - dir]);
        Town t2 = divTown(grid_highway[1 - dir][DIV[dir] + 1], k, DIV[1 - dir]);
        grid_highway[dir][k] = createMidHighway(t0, t1, t2, divp[1 - dir][i]);
      }
    }

    // treeの作成
    FastBitSet targetBs = new FastBitSet(g.N);
    FastBitSet excludeBs = aem.town_used.clone();
    for ( Town t : g.town ) {
      if ( t.isTarget() && !excludeBs.get(t.id) ) targetBs.set(t.id);
    }
    // outerの外側のtree
    FastBitSet rootBs = new FastBitSet(g.N);
    TreeCreatorForGrid tc = new TreeCreatorForGrid(g, aem);
    addRootBs(outerEdge, 0, outerEdge.size() + 1, rootBs, excludeBs);
    aem.addAllSegment(tc.createTree(rootBs, targetBs, excludeBs));

    // dirで分割が細かい側の方向の直線優先
    int[] div_ord = DIVx <= DIVy ? new int[]{0, 1} : new int[]{1, 0};
    int[] watershed =new int[]{(DIVx + 1) / 2, (DIVy + 1) / 2};
    for ( int dir : div_ord ) {
      rootBs = new FastBitSet(g.N);
      // 中心から遠ざかる方向
      for (int i = 0; i < DIV[1 - dir] + 2; i++) {
        ArrayList<Town> path = grid_highway[dir][i];
        if ( i <= watershed[1 - dir] ) {
          int i0 = dir == 0 ? 0 : path.size() - 1;
          int i1 = dir == 0 ? path.size() - 1 : 0;
          addRootBs(path, i0, i1, rootBs, excludeBs);
        }
        if ( i >= watershed[1 - dir] ) {
          int i0 = dir == 0 ? path.size() - 1 : 0;
          int i1 = dir == 0 ? 0 : path.size() - 1;
          addRootBs(path, i0, i1, rootBs, excludeBs);
        }
      }
      aem.addAllSegment(tc.createTree(rootBs, targetBs, excludeBs));
      rootBs = new FastBitSet(g.N);
      // 中心に近づく方向
      for (int i = 0; i < DIV[1 - dir] + 2; i++) {
        ArrayList<Town> path = grid_highway[dir][i];
        if ( i < watershed[1 - dir] ) {
          int i0 = dir == 0 ? path.size() - 1 : 0;
          int i1 = dir == 0 ? 0 : path.size() - 1;
          addRootBs(path, i0, i1, rootBs, excludeBs);
        }
        if ( i > watershed[1 - dir] ) {
          int i0 = dir == 0 ? 0 : path.size() - 1;
          int i1 = dir == 0 ? path.size() - 1 : 0;
          addRootBs(path, i0, i1, rootBs, excludeBs);
        }
      }
      aem.addAllSegment(tc.createTree(rootBs, targetBs, excludeBs));
    }
    addRootBs(outerEdge, outerEdge.size() + 1, 0, rootBs, excludeBs);
    aem.addAllSegment(tc.createTree(rootBs, targetBs, excludeBs));
    // aem.reduceEndpoint();

    assert targetBs.cardinality() == 0;
    HashSet<ArrayElement> exclude = new HashSet<>();
    if ( aem.segmentSize < limitLA ) {
      createDirectPath(limitLA - aem.segmentSize);
      aem.reduceEndpoint();
    }

    ArrayList<ArrayElement> allElem = new ArrayList<>();
    for ( ArraySegment seg : aem.segment ) {
      ArrayElement prev = null;
      ArrayElement cur = seg.first;
      while ( cur != null ) {
        allElem.add(cur);
        ArrayElement next = cur.next(prev);
        prev = cur;
        cur = next;
      }
    }
     if ( aem.segmentSize > limitLA ) {
      ArrayList<ArrayElement> sortedElem = new ArrayList<>(allElem);
      Collections.sort(sortedElem);
      for (int i = 0; i < aem.segmentSize - limitLA; i++) {
        assert sortedElem.get(i).eval < 0;
        exclude.add(sortedElem.get(i));
      }
    }
    int[] retA = new int[in.LA];
    int ai = 0;
    for ( ArrayElement ae : allElem ) {
      if ( exclude.contains(ae) ) continue;
      retA[ai++] = ae.t.id;
    }
    assert ai <= limitLA;
    return retA;

    //  divp順に実行。全部同時にやっても良いかも
    // outerの内側
    // TreeCreatorForGrid tc = new TreeCreatorForGrid(g, aem);
    // ArrayList<ArraySegment> treeSeg = tc.createTree(rootBs, targetBs, excludeBs);
  }

  void createDirectPath(int rest) {
    // super.createDirectPath(rest, in.LB, new HashSet<>());
    ArrayList<TargetInfo> infoList = new ArrayList<>();
    for ( Town t : g.town ) {
      for ( TargetInfo info : t.targetInfo ) {
        if ( info.prev != null ) {
          infoList.add(info);
        }
      }
    }
    Collections.sort(infoList, (ti0, ti1) -> {
      if ( ti0.dist2 != ti1.dist2 ) {
        return ti0.dist2 < ti1.dist2 ? 1 : -1;
      } else {
        return ti0.id - ti1.id;
      }
    });
    for ( TargetInfo info : infoList ) {
      ArraySegment seg = new ArraySegment();
      ArrayList<Town> path = g.createPath(info.prev, info.target);
      for ( int i = 1 ; i < path.size() && rest > 0 ; i++) {
        seg.addNext(aem.createElement(path.get(i), -100000000));
        rest--;
      }
      if ( seg.size > 0 ) aem.addSegment(seg);
      if ( rest <= 0 ) break;
    }
    // assert atown.size() == in.LA;
  }

  void addRootBs(ArrayList<Town> path, int i0, int i1, FastBitSet rootBs, FastBitSet excludeBs) {
    if ( Math.abs(i1 - i0) <= 1 ) return;
    int di = i0 <= i1 ? 1 : -1;
    int di2 = di * 2;
    for (int i = i0; ; i += di) {
      Town t0 = path.get(i % path.size());
      Town t1 = path.get((i + di) % path.size());
      Town t2 = path.get((i + di2) % path.size());
      for ( Edge e = g.edge(t0, t1).cwe ; e.to != t2 ; e = e.rev.cwe ) {
        if ( !excludeBs.get(e.to.id) ) {
          rootBs.set(e.to.id);
        }
      }
      if ( i + di2 == i1 ) break;
    }
  }

  // pathをdiv+1個の線分に分割した時のdi番目の点(0-indexed) pathの両端はカウントしない
  Town divTown(ArrayList<Town> path, int di, int div) {
    if ( path == null ) return null;
    return path.get((di + 1) * (path.size() - 1) / (div + 1));
  }

  // 0:上、1:下、2:左、3:右
  ArrayList<Town>[] createOuterHighway() {
    ArrayList<Town>[] ret = new ArrayList[4];
    outerEdgeBs = new FastBitSet(g.N);
    int[][] k = new int[][] {{0, 1}, {3, 2}, {0, 2}, {3, 1}};
    for (int ki = 0; ki < 4; ki++) {
      Town t0 = g.corner[k[ki][0]];
      Town t1 = g.corner[k[ki][1]];
      ret[ki] = g.createPath(t0, t1);
      for ( Town t : ret[ki] ) outerEdgeBs.set(t.id);
    }
    outerEdge = new ArrayList<>();
    for (int i = 0; i < ret[0].size() - 1; i++) outerEdge.add(ret[0].get(i));
    for (int i = ret[3].size() - 1; i >= 1; i--) outerEdge.add(ret[3].get(i));
    for (int i = 0; i < ret[1].size() - 1; i++) outerEdge.add(ret[1].get(i));
    for (int i = ret[2].size() - 1; i >= 1; i--) outerEdge.add(ret[2].get(i));
    aem.addSegment(aem.createSegmentFromPath(outerEdge, -1000));

    Collections.reverse(ret[1]);
    Collections.reverse(ret[3]);
    return ret;
  }

  ArrayList<Town> createMidHighway(Town t0, Town t1, Town t2, int pri) {
    ArrayList<Town> p = null;
    if ( t1 == null ) {
      p = g.createPath(t0, t2);
    } else {
      p = g.createPath(t1, t0);
      ArrayList<Town> p2 = g.createPath(t1, t2);
      Collections.reverse(p);
      p.remove(p.size() - 1);
      p.addAll(p2);
    }
    ArrayList<Town> path = new ArrayList<>();
    int i0 = 0;
    while ( i0 + 1 < p.size() && outerEdgeBs.get(p.get(i0 + 1).id) ) i0++;
    int i1 = p.size() - 1;
    while ( i1 - 1 >= i0 && outerEdgeBs.get(p.get(i1 - 1).id) ) i1--;
    for (int i = i0; i <= i1; i++) path.add(p.get(i));

    ArrayList<Town> arr = new ArrayList<>();
    for (int i = 1; i < path.size() - 1; i++) arr.add(path.get(i));
    if ( arr.size() > 0 ) {
      aem.addSegment(aem.createSegmentFromPath(arr, -pri * 1000));
    }
    return path;
  }

}

abstract class HubStructure extends GraphStructure {
  TreeCreatorForHub tc = null;

  int limitLA = 0;
  int limitLB = 0;
  ArrayList<Town> highway = null;
  ArrayList<Town> firstRoot = null;
  ArrayList<Town> fixedTown = null;
  int treeRest = 0;

  HubStructure(Graph g, Main main) {
    super(g, main);
    fixedTown = new ArrayList<>();
    highway = new ArrayList<>();
    firstRoot = new ArrayList<>();
  }

  int estimate_score() {
    return tc == null ? -1 : tc.estimate_score;
  }

  @Override
  String memo() {
    return String.format("est=%d,restA=%d", estimate_score(), treeRest);
  }

  abstract String name();
  abstract boolean createStructure();

  void createHub() {
    // fixedTown, highway, firstRootの生成
    // createHub1();
    boolean created = createStructure();
    if ( !created ) {
      return;
    }

    FastBitSet excludeBs = new FastBitSet(g.N);
    for ( Town t : fixedTown ) {
      // fixedTown.add(t);
      excludeBs.set(t.id);
    }
    for ( Town t : highway ) {
      excludeBs.set(t.id);
    }

    FastBitSet hubBs = new FastBitSet(g.N);
    for ( Town t : firstRoot ) {
      hubBs.set(t.id);
    }
    // FastBitSet hubBs = new FastBitSet(g.N);
    // for ( Town t : fixedTown ) {
    //   for ( Edge e : t.edge ) {
    //     if ( fixedTown.contains(e.to) ) continue; //TODO: excludeを除いた方が良いかも
    //     hubBs.set(e.to.id);
    //   }
    // }

    FastBitSet targetBs = createTargetBs(excludeBs);
    tc = new TreeCreatorForHub(g);
    boolean ret = tc.createTree(hubBs, targetBs, excludeBs, limitLA, limitLB);

    if ( ret ) {
      tc.addAllSegment(aem);
      if ( tc.totSize < limitLA ) {
        treeRest = limitLA - tc.totSize;
        createDirectPath(limitLA - tc.totSize, limitLB, new HashSet<>(fixedTown));
      }
      ArraySegment hseg = new ArraySegment();
      for ( Town t : highway ) {
        hseg.addNext(aem.createElement(t, 0));
      }
      if ( hseg.size > 0 ) aem.addSegment(hseg);
      aem.reduceEndpoint();
      // for ( Town t : highway ) atown.add(t); setment化する
      ArraySegment dummySeg = new ArraySegment();
      while ( aem.segmentSize + dummySeg.size < in.LA - fixedTown.size() ) dummySeg.addNext(aem.createElement(g.town[0], 0));
      if ( dummySeg.size != 0 ) aem.addSegment(dummySeg);
      assert aem.segmentSize + fixedTown.size() == in.LA;
    }
  }

  @Override
  int[] createA() {
    int[] ret = super.createA();
    int ai = in.LA - fixedTown.size();
    for ( Town t : fixedTown ) ret[ai++] = t.id;
    return ret;
  }

  // 1 townのみをHubとする場合の評価
  int evalHubTown(Town t0, int LB) {
    if ( !g.isMainLandByBridge(t0) ) return Integer.MIN_VALUE; // 孤立している場合は対象外
    int ret = 0;
    for ( Edge e : t0.edge ) {
      if ( e.face != e.rev.face ) {
        ret += 200;
        ret += e.to.edge.size() * 50;
      }
    }
    if ( g.maxDepth(t0) + 1 <= LB ) ret += 2000;
    return (LB - g.maxDepth(t0)) * 20 + ret;
  }

  // FastBitSet createHubRoot(ArrayList<Town> hub) {
  //   FastBitSet hubBs = new FastBitSet(in.N);
  //   for ( Town t : hub ) hubBs.set(t.id);
  //   assert hub.size() != 0;
  //   return hubBs;
  // }
  FastBitSet createTargetBs(FastBitSet excludeBs) {
    FastBitSet targetBs = new FastBitSet(in.N);
    for ( Town t : g.town ) {
      if ( t.isTarget() && !excludeBs.get(t.id) ) targetBs.set(t.id);
    }
    return targetBs;
  }
}
class HubFixed1Structure extends HubStructure {
  HubFixed1Structure(Graph g, Main main) {
    super(g, main);
  }

  @Override
  String name() {
    return "HF1";
  }

  @Override
  boolean createStructure() {
    int i = 0;
    int best_eval = Integer.MIN_VALUE;
    Town best_town = null;
    for ( Town t : g.town_sorted ) {
      int eval = evalHubTown(t, in.LB);
      if ( eval == Integer.MIN_VALUE ) continue;
      if ( eval > best_eval ) {
        best_town = t;
        best_eval = eval;
      }

      if ( i++ > 10 ) break;
    }
    if ( best_town == null ) return false;
    fixedTown.add(best_town);
    limitLA = in.LA - fixedTown.size() - highway.size();
    limitLB = in.LB - 1;

    // FastBitSet hubBs = new FastBitSet(g.N);
    for ( Town t : fixedTown ) {
      for ( Edge e : t.edge ) {
        if ( fixedTown.contains(e.to) ) continue; //TODO: excludeを除いた方が良いかも
        // hubBs.set(e.to.id);
        firstRoot.add(e.to);
      }
    }
    return true;
  }
}


class HubLine1Structure extends HubStructure {
  HubLine1Structure(Graph g, Main main) {
    super(g, main);
  }

  @Override
  String name() {
    return "HL1";
  }

  @Override
  boolean createStructure() {
    ArrayList<Town> path0 = g.createPath(g.corner[0], g.corner[2]); // 左辺
    ArrayList<Town> path1 = g.createPath(g.corner[1], g.corner[3]); // 右辺
    Town t0 = path0.get(path0.size() / 2);
    Town t1 = path1.get(path1.size() / 2);
    ArrayList<Town> path = g.createPath(t0, t1);

    int ic = path.size() / 2;
    int i0 = ic - in.LB / 2;
    int i1 = i0 + in.LB - 1;
    i0 = Math.max(0, i0);
    i1 = Math.min(i1, path.size() - 1);

    HashSet<Town> used = new HashSet<>();
    for (int i = i0; i <= i1; i++) {
      used.add(path.get(i));
      highway.add(path.get(i));
    }
    // ArraySegment seg = new ArraySegment();
    for ( Town t : highway ) {
      for ( Edge e : t.edge ) {
        if ( used.contains(e.to) ) continue;
        firstRoot.add(e.to);
        used.add(e.to);
        // seg.addNext(aem.createElement(e.to, 0));
      }
    }
    // aem.addSegment(seg);
    limitLA = in.LA - fixedTown.size() - highway.size();
    limitLB = in.LB;
    return true;
  }
}


class HubLongLineStructure extends HubStructure {
  HubLongLineStructure(Graph g, Main main) {
    super(g, main);
  }

  @Override
  String name() {
    return "HWL";
  }

  @Override
  boolean createStructure() {
    ArrayList<Town> path0 = g.createPath(g.corner[0], g.corner[2]); // 左辺
    ArrayList<Town> path1 = g.createPath(g.corner[1], g.corner[3]); // 右辺
    Town t0 = path0.get(path0.size() / 2);
    Town t1 = path1.get(path1.size() / 2);
    ArrayList<Town> path = g.createPath(t0, t1);

    // int ic = path.size() / 2;
    // int i0 = ic - in.LB / 2;
    // int i1 = i0 + in.LB - 1;
    // i0 = Math.max(0, i0);
    // i1 = Math.min(i1, path.size() - 1);
    int i0 = 0;
    int i1 = path.size() - 1;

    HashSet<Town> used = new HashSet<>();
    for (int i = i0; i <= i1; i++) {
      used.add(path.get(i));
      highway.add(path.get(i));
    }
    // ArraySegment seg = new ArraySegment();
    for ( Town t : highway ) {
      for ( Edge e : t.edge ) {
        if ( used.contains(e.to) ) continue;
        firstRoot.add(e.to);
        used.add(e.to);
        // seg.addNext(aem.createElement(e.to, 0));
      }
    }
    // aem.addSegment(seg);
    limitLA = in.LA - fixedTown.size() - highway.size();
    limitLB = in.LB;
    return true;
  }
}


class HubSquareStructure extends HubStructure {
  HubSquareStructure(Graph g, Main main) {
    super(g, main);
  }

  @Override
  String name() {
    return "HSQ";
  }

  @Override
  boolean createStructure() {
    ArrayList<Town> path0 = g.createPath(g.corner[0], g.corner[3]); // 対角線1 左上→右下
    ArrayList<Town> path1 = g.createPath(g.corner[1], g.corner[2]); // 対角線2 右上→左下
    // HashSet<Town> mem1 = new HashSet<>(path1);
    // int ci0 = 0;
    // while ( ci0 < path0.size() && !mem1.contains(path0.get(ci0)) ) ci0++;
    // if ( ci0 >= path0.size() ) return false;
    // Town cen = path0.get(ci0);
    // int ci1 = 0;
    // while ( ci1 < path1.size() && path1.get(ci1) != cen ) ci1++;
    // if ( ci1 >= path1.size() ) return false;
    // assert cen == path1.get(ci1);
    int di0 = di(path0.size());
    int di1 = di(path1.size());

    Town[] v = new Town[] {
      path0.get(di0), path1.get(di1), path0.get(path0.size() - di0 - 1), path1.get(path1.size() - di1 - 1)
    };

    ArrayList<Town>[] pathEdge = new ArrayList[4];
    for (int i = 0; i < 4; i++) {
      pathEdge[i] = g.createPath(v[i], v[(i + 1) % 4]);
    }



    // Town t0 = path0.get(path0.size() / 2);
    // Town t1 = path1.get(path1.size() / 2);
    // ArrayList<Town> path = g.createPath(t0, t1);

    // int ic = path.size() / 2;
    // int i0 = ic - in.LB / 2;
    // int i1 = i0 + in.LB - 1;
    // i0 = Math.max(0, i0);
    // i1 = Math.min(i1, path.size() - 1);

    HashSet<Town> used = new HashSet<>();
    // ArrayList<Town> path = new ArrayList<>();
    for ( ArrayList<Town> p : pathEdge ) {
      for (int i = 0; i < p.size() - 1; i++) {
        used.add(p.get(i));
        highway.add(p.get(i));
      }
    }
    int dup = Math.min(highway.size(), in.LB - 1);
    for (int i = 0; i < dup; i++) {
      highway.add(highway.get(i)); // 少し重複させる
    }

    // for (int i = i0; i <= i1; i++) {
    //   used.add(path.get(i));
    //   highway.add(path.get(i));
    // }
    // ArraySegment seg = new ArraySegment();
    for ( Town t : highway ) {
      for ( Edge e : t.edge ) {
        if ( used.contains(e.to) ) continue;
        firstRoot.add(e.to);
        used.add(e.to);
      }
    }
    // aem.addSegment(seg);
    limitLA = in.LA - fixedTown.size() - highway.size();
    limitLB = in.LB;
    return true;
  }

  int di(int pathLen) {
    if ( pathLen <= in.LB * 2 ) {
      return pathLen / 4;
    } else if ( pathLen <= in.LB * 2.4 ) {
      return pathLen / 2 - 2 * in.LB / 3;
    } else if ( pathLen <= in.LB * 4 ) {
      return in.LB;
    } else {
      return pathLen / 4;
    }
  }
}


class HubTriangleStructure extends HubStructure {
  HubTriangleStructure(Graph g, Main main) {
    super(g, main);
  }

  @Override
  String name() {
    return "HT1";
  }

  @Override
  boolean createStructure() {
    int i = 0;
    int best_eval = Integer.MIN_VALUE;
    Face best_face = null;
    HashSet<Face> checked = new HashSet<>();
    for ( Town t : g.town_sorted ) {
      for ( Edge e : t.edge ) {
        Face f = e.face;
        if ( checked.contains(f) ) continue;
        checked.add(f);

        int eval = Integer.MAX_VALUE;
        for ( Edge c : f.cycle ) {
          eval = Math.min(eval, evalHubTown(c.to, in.LB));
          if ( eval == Integer.MIN_VALUE ) continue;
        }
        if ( eval > best_eval ) {
          best_face = f;
          best_eval = eval;
        }
  
        if ( i++ > 5 ) break;
      }
    }
    if ( best_face == null ) return false;
    for ( Edge c : best_face.cycle ) {
      firstRoot.add(c.to);
    }
    limitLA = in.LA - fixedTown.size() - highway.size();
    limitLB = in.LB;
    return true;
  }
}


class TreeCreator {
  final Input in;
  final Graph g;
  TreeCreator(Graph g) {
    this.g = g;
    this.in = g.in;
  }

  class Node<T> {
    ArrayList<T> parent = null;
    ArrayList<T> child = null;
    Town t = null;
    int depth = 0;

    Node(Town t, int depth) {
      this.t = t;
      this.depth = depth;
      this.parent = new ArrayList<>();
      this.child = new ArrayList<>();
    }
    void addChild(T ch) {
      this.child.add(ch);
    }
    protected void addParent(T p) {
      this.parent.add(p);
    }
    @Override
    public String toString() {
      return t.toString();
    }
  }
}

class TreeCreatorForGrid extends TreeCreator {
  ArrayElementManager aem = null;
  Node[] nodes = null;

  TreeCreatorForGrid(Graph g, ArrayElementManager aem) {
    super(g);
    this.aem = aem;
    this.init();
  }

  void init() {
    this.nodes = new Node[g.N];
  }

  // rootNodesから、excludeBsを通らずにtreeを作成
  // excludeBsは通った(通り得た)townの分を完了時に追加する
  // targetBsは通ったtownの分を完了時に削除する
  ArrayList<ArraySegment> createTree(FastBitSet rootBs, FastBitSet targetBs, FastBitSet excludeBs) {
    ArrayList<Node> rootNodes = new ArrayList<>();
    for ( int i = rootBs.nextSetBit(0); i >= 0 ; i = rootBs.nextSetBit(i + 1) ) {
      if ( nodes[i] == null ) {
        nodes[i] = new Node(g.town[i], 0);
        rootNodes.add(nodes[i]);
      }
      if ( i == Integer.MAX_VALUE ) break;
    }
    Collections.sort(rootNodes, (n0, n1) -> {
      if ( n0.t.dist0 != n1.t.dist0 ) {
        return n0.t.dist0 < n1.t.dist0 ? -1 : 1;
      } else {
        return n0.t.id - n1.t.id;
      }
    });
    ArrayDeque<Node> queue = new ArrayDeque<>(rootNodes);
    while ( queue.size() > 0 ) {
      Node n = queue.pollFirst();
      for ( Edge e : n.t.edge ) {
        Town a = e.to;
        if ( excludeBs.get(a.id) ) continue;
        if ( nodes[a.id] == null ) {
          nodes[a.id] = new Node(a, n.depth + 1);
          n.addChild(nodes[a.id]);
          n.updateChildBestParent(nodes[a.id]);
          queue.addLast(nodes[a.id]);
        } else if ( n.depth + 1 ==  nodes[a.id].depth ) {
          n.addChild(nodes[a.id]);
          if ( n.bestChildCnt + 1 < nodes[a.id].bestParent.bestChildCnt ) {
            n.updateChildBestParent(nodes[a.id]);
          }
        }
      }
    }
    for ( Node n : rootNodes ) n.calc();
    for ( Node n : rootNodes ) {
      n.updateBs(targetBs, excludeBs);
    }

    // 戻り用のtree構成
    ArrayList<ArraySegment> ret = new ArrayList<>();
    for ( Node n : rootNodes ) {
      if ( n.childTargetCnt == 0 ) continue;
      ArraySegment seg = new ArraySegment();
      n.createArraySegment(seg, 0, -100000);
      ret.add(seg);
    }
    return ret;
  }

  class Node extends TreeCreator.Node<Node> {
    Node bestParent = null;
    int bestChildCnt = 0;
    int maxDepth = 0;
    int childTargetCnt = 0;
    int childCnt = 0;

    ArrayList<Node> bestChild = null;

    Node(Town t, int depth) {
      super(t, depth);
    }
    void addChild(Node ch) {
      super.addChild(ch);
      ch.addParent(this);
    }
    void updateChildBestParent(Node ch) {
      if ( ch.bestParent != null ) {
        ch.bestParent.bestChildCnt--;
      }
      ch.bestParent = this;
      this.bestChildCnt++;
    }

    void calc() {
      maxDepth = depth;
      childTargetCnt = t.isTarget() ? 1 : 0;
      childCnt = 1;
      bestChild = new ArrayList<>();
      for ( Node ch : child ) {
        if ( ch.bestParent != this ) continue;
        bestChild.add(ch);
        ch.calc();
        maxDepth = Math.max(maxDepth, ch.maxDepth);
        childTargetCnt += ch.childTargetCnt;
        childCnt += ch.childCnt;
      }
      Collections.sort(bestChild, (c0, c1) -> {
        if ( c0.childTargetCnt != c1.childTargetCnt ) {
          return -c0.childTargetCnt + c1.childTargetCnt;
        } else if ( c0.maxDepth != c1.maxDepth ) {
          return -c0.maxDepth + c1.maxDepth;
        } else {
          return c0.t.id - c1.t.id;
        }
      });
    }

    void updateBs(FastBitSet targetBs, FastBitSet excludeBs) {
      excludeBs.set(t.id);
      if ( targetBs.get(t.id) ) targetBs.clear(t.id);
      for ( Node ch : bestChild ) {
        ch.updateBs(targetBs, excludeBs);
      }
    }

    void createArraySegment(ArraySegment segment, int dir, int base_eval) {
      int ev = childCnt > 0 ? 0 : -base_eval - aem.aeid;
      ArrayElement ae = aem.createElement(t, ev);
      if ( dir == 0 ) {
        segment.addNext(ae);
      } else {
        segment.addPrev(ae);
      }
      int cnt = 0;
      for ( Node ch : bestChild ) {
        if ( cnt > 0 && ch.childTargetCnt == 0 ) continue;
        ch.createArraySegment(segment, dir, base_eval);
        cnt++;
        if ( depth == 0 && dir == 0 ) dir = 1;
      }
    }
  }
}

class TreeCreatorForHub extends TreeCreator {
  TargetManager tgm = null;
  
  ArrayList<TreePair> treePairs = null;
  int totSize = 0;
  int estimate_score = 0;

  TreeCreatorForHub(Graph g) {
    super(g);
  }

  // rootBs: ルート候補点
  // targetBs: roots接続が必要な対象
  // excludeBs: 埋める必要がない点
  boolean createTree(FastBitSet rootBs, FastBitSet targetBs, FastBitSet excludeBs, int limitLA, int limitLB) {
    treePairs = new ArrayList<>();
    estimate_score = pena(targetBs);
    while ( targetBs.cardinality() > 0 ) {
      boolean ret = _createTree(rootBs, targetBs, excludeBs, limitLB);
      estimate_score += 2 * pena(targetBs);
      rootBs = new FastBitSet(g.N);
      if ( !ret ) {
        estimate_score = -1;
        return false;
      }
      for ( int i = excludeBs.nextSetBit(0); i >= 0 ; i = excludeBs.nextSetBit(i + 1) ) {
        if ( i == Integer.MAX_VALUE ) break;
        for ( Edge e : g.town[i].edge ) {
          if ( excludeBs.get(e.to.id) ) continue;
          rootBs.set(e.to.id);
        }
      }
    }
    reduce(limitLA, limitLB);
    return true;
  }
  int pena(FastBitSet targetBs) {
    int ret = 0;
    for ( int i = targetBs.nextSetBit(0); i >= 0 ; i = targetBs.nextSetBit(i + 1) ) {
      if ( i == Integer.MAX_VALUE ) break;
      ret += g.town[i].targetCount();
    }
    return ret;
  }

  // rootBs: ルート候補点
  // targetBs: roots接続が必要な対象
  // excludeBs: 埋める必要がない点
  private boolean _createTree(FastBitSet rootBs, FastBitSet targetBs, FastBitSet excludeBs, int limitLB) {
    tgm = new TargetManager(targetBs);
    LayeredGraph lg = new LayeredGraph(rootBs, excludeBs);
    lg.prepare(limitLB);
    int THR = in.LB <= 9 ? 3 : 6;
    int THR2 = in.LB >= 21 ? 6 : -1;
    // if ( in.LB >= 19 ) THR = 8;

    HashSet<TreePair>[] tpmap = new HashSet[in.N];
    for (int i = 0; i < in.N; i++) tpmap[i] = new HashSet<>();
    HashSet<TreePair> allTp = new HashSet<>();

    ArrayList<Node> target = new ArrayList<>();
    for ( Town t : tgm.target ) {
      if ( lg.nodes[t.id] != null ) target.add(lg.nodes[t.id]);
    }
    Collections.sort(target, (n0, n1) -> {
      if ( n0.depth != n1.depth ) {
        return -n0.depth + n1.depth;
      } else {
        return n0.t.id - n1.t.id;
      }
    });
    FastBitSet used = new FastBitSet(tgm.cnt);
    int tpid = 0;
    LOOP: for ( Node n : target ) {
      if ( used.get(tgm.tgid(n.t)) ) continue;

      Node cur = n;
      ArrayList<Node> tree = new ArrayList<>();
      while ( cur != null ) {
        tree.add(cur);
        if ( tgm.isTarget(cur.t) ) used.set(tgm.tgid(cur.t));
        Node best = null;
        int best_eval = -1;
        for ( Node p : cur.parent ) {
          int eval = p.upTarget.excludesCount(used);
          if ( best == null || eval > best_eval ) {
            best = p;
            best_eval = eval;
          }
        }
        if ( best != null && !tgm.isTarget(best.t) ) {
          // 足せるところに足す
          for ( Node p : cur.parent ) {
            for ( TreePair tp : tpmap[p.t.id] ) {
              if ( p.depth > THR2 && tp.canAdd1(tree.size(), limitLB) ) {
                Collections.reverse(tree);
                tp.add1(tree);
              } else if ( p.depth > THR2 && tp.canAdd2(tree.size(), limitLB) ) {
                Collections.reverse(tree);
                tp.add2(tree);
              } else if ( p.depth > THR && tp.canPair(p, tree.size(), limitLB) ) {
                Collections.reverse(tree);
                tp.setPair(tree, p);
              } else {
                continue;
              }
              // 足した場合は完了
              if ( tp.isFixed(limitLB) ) {
                for ( Node r : tp.tree1 ) {
                  tpmap[r.t.id].remove(tp);
                }
                for ( Node r : tp.tree2 ) {
                  tpmap[r.t.id].remove(tp);
                }
                allTp.add(tp);
              }
              continue LOOP;
            }
          }
        }
        cur = best;
      }
      Collections.reverse(tree);
      TreePair tp = new TreePair(tree, tpid++);
      for ( Node r : tp.tree1 ) {
        tpmap[r.t.id].add(tp);
      }
      if ( tp.tree2 != null ) {
        for ( Node r : tp.tree2 ) {
          tpmap[r.t.id].add(tp);
        }
      }
    }
    for ( Town t : tgm.target ) {
      for ( TreePair tp : tpmap[t.id] ) {
        allTp.add(tp);
      }
    }
    if ( allTp.size() == 0 ) return false;
    for ( TreePair tp : allTp ) {
      tp.updateBitSet(targetBs, excludeBs);
      treePairs.add(tp);
    }
    return true;
  }

  void reduce(int limitLA, int limitLB) {
    // treePairs = new ArrayList<>(allTp);
    totSize = 0;
    treePairs = mergeTreePairs(treePairs, limitLB);
    for ( TreePair tp : treePairs ) {
      totSize += tp.actualSize();
    }
    if ( totSize > limitLA ) {
      int[] cnt = countAllNodes(treePairs);
      reduce(cnt, totSize - limitLA);
      totSize = limitLA;
    }
  }

  boolean isValid(int limitLA) {
    return treePairs != null && totSize <= limitLA;
  }

  int[] countAllNodes(ArrayList<TreePair> treePairs) {
    int[] cnt = new int[g.N];
    for ( TreePair tp : treePairs ) {
      for ( Node n : tp.tree1 ) cnt[n.t.id]++;
      if ( tp.tree2 != null ) {
        for ( Node n : tp.tree2 ) cnt[n.t.id]++;
      }
    }
    return cnt;
  }

  private void reduce(int[] cnt, int rest) {
    for ( TreePair tp : treePairs ) {
      int dupNode = 0;
      int singleTgtNode = 0;
      for ( Node n : tp.tree1 ) {
        if ( cnt[n.t.id] > 1 ) dupNode++;
        if ( n.t.isTarget() && cnt[n.t.id] == 1 ) singleTgtNode++;
      }
      if ( tp.tree2 != null ) {
        for ( Node n : tp.tree2 ) {
          if ( cnt[n.t.id] > 1 ) dupNode++;
          if ( n.t.isTarget() && cnt[n.t.id] == 1 ) singleTgtNode++;
        }
      }
      tp.reduceEval = dupNode > 0 ? (((double)singleTgtNode) / dupNode) : 100000;
    }
    Collections.sort(treePairs, (tp1, tp2) -> {
      if ( tp1.reduceEval != tp2.reduceEval ) {
        return tp1.reduceEval < tp2.reduceEval ? -1 : 1;
      } else {
        return tp1.id - tp2.id;
      }
    });
    for ( TreePair tp : treePairs ) {
      if ( rest <= 0 ) break;
      for ( Node n : tp.tree1 ) {
        if ( cnt[n.t.id] > 1 && rest > 0 ) {
          tp.reduce(n);
          cnt[n.t.id]--;
          rest--;
        } else if ( n.t.isTarget() ) {
          estimate_score += 2 * n.t.targetCount();
        }
      }
      if ( tp.tree2 != null ) {
        for ( Node n : tp.tree2 ) {
          if ( cnt[n.t.id] > 1 && rest > 0 ) {
            tp.reduce(n);
            cnt[n.t.id]--;
            rest--;
          } else if ( n.t.isTarget() ) {
            estimate_score += 2 * n.t.targetCount();
          }
        }
      }
    }
    assert rest == 0;
  }

  class TargetManager {
    FastBitSet bs = null; // town idベース
    ArrayList<Town> target = null;

    int cnt = 0;
    int[] tgid = null; // tid(N) -> tgid(cnt)
    TargetManager(FastBitSet targetBs) {
      this.bs = targetBs;
      target = new ArrayList<>();
      tgid = new int[in.N];
      Arrays.fill(tgid, -1);
      for ( int i = bs.nextSetBit(0); i >= 0 ; i = bs.nextSetBit(i + 1) ) {
        tgid[i] = target.size();
        target.add(g.town[i]);
        if ( i == Integer.MAX_VALUE ) break;
      }
      cnt = target.size();
      
    }
    Town town(int tgid) {
      return target.get(tgid);
    }
    int tid(int tgid) {
      return town(tgid).id;
    }
    int tgid(Town t) {
      return tgid[t.id];
    }
    int tgid(int tid) {
      return tgid[tid];
    }
    boolean isTarget(Town t) {
      return bs.get(t.id);
    }
  }

  class LayeredGraph {
    FastBitSet rootBs;
    FastBitSet excludeBs;
    Node[] nodes = null;
    ArrayList<Node>[] layeredNodes = null; // 全Node(不要Node含まず)
    ArrayList<Node>[] allLayeredNodes = null; // 全Node(不要Node含む)
    // int[] depth = null; // 各Townのdepth (root: depth = 0, exclude: depth = -1)
    LayeredGraph(FastBitSet rootBs, FastBitSet excludeBs) {
      this.rootBs = rootBs;
      this.excludeBs = excludeBs;
      nodes = new Node[in.N];
    }
    void prepare(int limitLB) {
      Arrays.fill(nodes, null);
      ArrayDeque<Node> queue = new ArrayDeque<>();
      for ( int i = rootBs.nextSetBit(0); i >= 0 ; i = rootBs.nextSetBit(i + 1) ) {
        nodes[i] = new Node(g.town[i], 0);
        queue.addLast(nodes[i]);
      }
      int maxDepth = 0;
      while ( queue.size() > 0 ) {
        Node n = queue.pollFirst();
        maxDepth = Math.max(maxDepth, n.depth);
        if ( n.depth + 1 >= limitLB ) continue;
        for ( Edge e : n.t.edge ) {
          Town a = e.to;
          if ( excludeBs.get(a.id) ) continue;
          if ( nodes[a.id] == null ) {
            nodes[a.id] = new Node(a, n.depth + 1);
            n.addChild(nodes[a.id]);
            queue.addLast(nodes[a.id]);
          } else if ( nodes[a.id].depth == n.depth + 1 ) {
            n.addChild(nodes[a.id]);
          }
        }
      }

      allLayeredNodes = new ArrayList[maxDepth + 1];
      layeredNodes = new ArrayList[maxDepth + 1];
      for (int d = 0; d <= maxDepth; d++) {
        allLayeredNodes[d] = new ArrayList<>();
        layeredNodes[d] = new ArrayList<>();
      }
      for ( Node n : nodes ) {
        if ( n == null ) continue;
        if ( tgm.isTarget(n.t) ) n.setTarget();
        allLayeredNodes[n.depth].add(n);
      }
      // Targetの上流・下流情報を生成
      for (int d = 1; d <= maxDepth; d++) {
        for ( Node n : allLayeredNodes[d] ) {
          for ( Node p : n.parent ) {
            n.upTarget.or(p.upTarget);
          }
        }
      }
      for (int d = maxDepth - 1; d >= 0; d--) {
        for ( Node n : allLayeredNodes[d] ) {
          for ( Node c : n.child ) {
            n.downTarget.or(c.downTarget);
          }
        }
      }
      for ( Node n : nodes ) {
        if ( n == null ) continue;
        if ( n.downTarget.cardinality() != 0 ) {
          layeredNodes[n.depth].add(n);
        }
      }
    }
  }

  class Node extends TreeCreator.Node<Node> {
    FastBitSet upTarget = null; // 上流にあるtarget(tgid) 自身含む
    FastBitSet downTarget = null; // 下流にあるtarget(tgid) 自身含む
    // boolean invalid = false;

    Node(Town t, int depth) {
      super(t, depth);
      this.upTarget = new FastBitSet(tgm.cnt);
      this.downTarget = new FastBitSet(tgm.cnt);
    }
    void addChild(Node ch) {
      super.addChild(ch);
      ch.addParent(this);
    }
    void setTarget() {
      upTarget.set(tgm.tgid(t));
      downTarget.set(tgm.tgid(t));
    }
  }

  class TreePair {
    ArrayList<Node> tree1 = null; // 先頭がroot(depth = 0)
    ArrayList<Node> tree2 = null; // tree2(rev) - tree1 の形でrootを共有するtreeをつなげる
    HashMap<Town, Integer> tree1Idx = null;
    Node joinNode = null;
    int id = 0;

    HashMap<Town, Integer> reduceTown = null;
    double reduceEval = 0;

    TreePair(ArrayList<Node> tree1, int id) {
      this.tree1 = tree1;
      this.tree1Idx = new HashMap<>();
      this.id = id;
      int i = 0;
      for ( Node n : tree1 ) {
        tree1Idx.put(n.t, i++);
      }
    }
    boolean canPair(Node joinNode, int size2, int limitLB) {
      if ( tree2 != null ) return false;
      int idx1 = tree1Idx.get(joinNode.t);
      return size2 + idx1 + 1 <= limitLB;
    }
    void setPair(ArrayList<Node> tree2, Node joinNode) {
      this.tree2 = tree2;
      this.joinNode = joinNode;
    }
    int size1() {
      return tree1.size();
    }
    // 仮想的なサイズ
    int size2() {
      if ( tree2 == null ) return 0;
      return tree2.size() + tree1Idx.get(joinNode.t) + 1;
    }
    // 実際の全体のサイズ
    int actualSize() {
      return tree1.size() + (tree2 != null ? tree2.size() : 0);
    }
    boolean canAdd1(int addSize, int limitLB) {
      return size1() + addSize <= limitLB;
    }
    boolean canAdd2(int addSize, int limitLB) {
      if ( tree2 == null ) return false;
      return size2() + addSize <= limitLB;
    }
    void add1(ArrayList<Node> add1) {
      for ( Node n : add1 ) {
        tree1Idx.put(n.t, tree1.size());
        tree1.add(n);
      }
    }
    void add2(ArrayList<Node> add2) {
      tree2.addAll(add2);
    }
    boolean isFixed(int limitLB) {
      return tree2 != null && size1() >= limitLB && size2() >= limitLB;
    }
    void reduce(Node n) {
      if ( reduceTown == null ) reduceTown = new HashMap<>();
      // assert !reduceTown.contains(n.t);
      if ( reduceTown.containsKey(n.t) ) {
        reduceTown.put(n.t, reduceTown.get(n.t) + 1);
      } else {
        reduceTown.put(n.t, 1);
      }
      // reduceTown.add(n.t);
    }
    boolean isReduced(Node n) {
      return reduceTown != null && (reduceTown.containsKey(n.t) && reduceTown.get(n.t) >= 1);
    }
    void updateBitSet(FastBitSet targetBs, FastBitSet excludeBs) {
      if ( tree2 != null ) {
        for ( Node n : tree2 ) {
          if ( targetBs.get(n.t.id) ) targetBs.clear(n.t.id);
          if ( !excludeBs.get(n.t.id) ) excludeBs.set(n.t.id);
        }
      } 
      for ( Node n : tree1 ) {
        if ( targetBs.get(n.t.id) ) targetBs.clear(n.t.id);
        if ( !excludeBs.get(n.t.id) ) excludeBs.set(n.t.id);
    }
    }

    @Override
    public int hashCode() {
      return id;
    }
    @Override
    public boolean equals(Object o) {
      if ( o instanceof TreePair ) {
        return id == ((TreePair)o).id;
      }
      return false;
    }

    ArrayList<Town> toArray() {
      ArrayList<Town> ret = new ArrayList<>();
      if ( tree2 != null ) {
        for ( Node n : tree2 ) {
          if ( isReduced(n) ) {
            reduceTown.put(n.t, reduceTown.get(n.t) - 1);
          } else {
            ret.add(n.t);
          }
        }
        Collections.reverse(ret);
      } 
      for ( Node n : tree1 ) {
        if ( isReduced(n) ) {
          reduceTown.put(n.t, reduceTown.get(n.t) - 1);
        } else {
          ret.add(n.t);
        }
    }
      return ret; 
    }
  }
  ArrayList<TreePair> mergeTreePairs(ArrayList<TreePair> tpArray, int limitLB) {
    HashMap<Town, ArrayList<TreePair>> candMap = new HashMap<>();
    ArrayList<TreePair> ret = new ArrayList<>();
    for ( TreePair tp : tpArray ) {
      if ( tp.tree2 == null ) {
        Town t = tp.tree1.get(0).t;
        if ( !candMap.containsKey(t) ) {
          ArrayList<TreePair> arr = new ArrayList<>();
          arr.add(tp);
          candMap.put(t, arr);
        } else {
          candMap.get(t).add(tp);
        }
      } else {
        ret.add(tp);
      }
    }
    for ( ArrayList<TreePair> candArr : candMap.values() ) {
      if ( candArr.size() == 1 ) {
        ret.add(candArr.get(0));
        continue;
      }
      ArrayList<int[]> infoArr = new ArrayList<>();
      for (int c0 = 0; c0 < candArr.size(); c0++) {
        for (int c1 = c0 + 1; c1 < candArr.size(); c1++) {
          ArrayList<Node> tree0 = candArr.get(c0).tree1;
          ArrayList<Node> tree1 = candArr.get(c1).tree1;
          int eval = 0;
          for (int i = 0; i < Math.min(limitLB - 1, Math.min(tree0.size(), tree1.size())); i++) {
            if ( tree0.get(i).t.id == tree1.get(i).t.id ) {
              eval++;
            } else {
              break;
            }
          }
          infoArr.add(new int[] {eval, c0, c1});
        }
      }
      boolean[] merged = new boolean[candArr.size()];
      Collections.sort(infoArr, (in0, in1) -> {
        if ( in0[0] != in1[0] ) {
          return -in0[0] + in1[0];
        } else if ( in0[1] != in1[1] ) {
          return in0[1] - in1[1];
        } else {
          return in0[2] - in1[2];
        }
      });
      for ( int[] info : infoArr ) {
        int c0 = info[1];
        int c1 = info[2];
        ArrayList<Node> orgTree = candArr.get(c1).tree1;
        if ( merged[c0] || merged[c1] ) continue;
        ArrayList<Node> newTree = new ArrayList<>();
        for ( int i = info[0] ; i < orgTree.size(); i++ ) newTree.add(orgTree.get(i));
        candArr.get(c0).setPair(newTree, candArr.get(c0).tree1.get(info[0] - 1));
        merged[c0] = true;
        merged[c1] = true;
        ret.add(candArr.get(c0));
      }
      for ( int c = 0 ; c < candArr.size() ; c++ ) {
        if ( !merged[c] ) {
          ret.add(candArr.get(c));
        }
      }
    }
    return ret;
  }

  void addAllSegment(ArrayElementManager aem) {
    for ( TreePair tp : treePairs ) {
      ArrayList<Town> arr = tp.toArray();
      ArraySegment seg = new ArraySegment();
      for ( Town t : arr ) {
        if ( seg.last != null && seg.last.t.id == t.id ) continue;
        seg.addNext(aem.createElement(t, 0));
      }
      aem.addSegment(seg);
    }
  }
}

class SignalSearch {
  final Input in;
  final Main main;
  final Graph g;
  int K = 0;

  private int limitLB = 0;
  DPMap dm = null;

  SignalSearch(Input in, Main main) {
    this.in = in;
    this.g = in.g;
    this.main = main;
  }
  void clearTable() {
    if ( dm == null ) dm = new DPMap();
    dm.clear();
  }

  ArrayList<Step> search(ArrayList<Step> s0, Town t1, Controller cont, GraphStructure strc) {
    this.K = cont.virtualLA() * 2;
    clearTable();
    this.limitLB = in.LB - cont.fixedCnt();
    ArrayList<Step> ret = _search(s0, t1, cont, strc);
    return ret;
  }

  private ArrayList<Step> _search(ArrayList<Step> s0, Town t1, Controller cont, GraphStructure strc) {
    ArrayList<Step> ret = new ArrayList<>();
    ArrayDeque<Step> queue0 = new ArrayDeque<>();
    ArrayDeque<Step> queue1 = new ArrayDeque<>();

    int minC = Input.MAX_C;
    for ( Step s : s0 ) {
      if ( compareEval(s) > 0 ) {
        minC = Math.min(minC, s.C);
      }
    }
    for ( Step s : s0 ) {
      if ( compareEval(s) > 0 ) {
        put(s, cont);
        if ( s.C == minC ) {
          queue0.add(s);
        } else {
          queue1.add(s);
        }
        if ( s.town(cont).id == t1.id ) {
          ret.add(s);
        }
      }
    }
    int target_C = Input.MAX_C;
    while ( true ) {
      if ( queue0.size() == 0 ) {
        queue0 = queue1;
        queue1 = new ArrayDeque<>();
      }
      if ( queue0.size() == 0 ) break;
      Step s = queue0.pollFirst();
      if ( getCurrent(s) != s ) continue;

      // Town t = g.town[cont.A[s.aid]];
      Town t = s.town(cont);
      if ( t.id == t1.id ) {
        target_C = Math.min(target_C, s.C);
      }
      if ( s.C >= target_C + 1 ) { // ここを1にすると改善する(時間はかかる)
        break;
      }
      if ( s.C > dm.getMinC(t.id) ) {
        continue;
      }
      for ( Edge e : t.edge ) {
        Town nt = e.to;
        if ( s.C > dm.getMinC(nt.id) ) continue;
        if ( cont.isFixed(nt) ) { // 常時点灯信号の場合
          int n_aid = cont.vaidForFixed(nt, s.townAdj(cont));
          int n_eval = s.eval(n_aid, s.dir);
          if ( compareEval(n_aid, s.dir, s.C, n_eval) > 0 ) {
            Step ns = new Step(s, n_aid);
            put(ns, cont);
            queue0.addLast(ns);
          }
          continue;
        }
        for ( int n_aid : cont.aid(nt) ) {
          if ( s.canExtend(n_aid) ) {
            // sを拡張して次に進む
            int n_eval = s.eval(n_aid, s.dir);
            if ( compareEval(n_aid, s.dir, s.C, n_eval) > 0 ) {
              Step ns = new Step(s, n_aid, s.dir);
              assert compareEval(ns) > 0;
              put(ns, cont);
              queue0.addLast(ns);
            }
            continue;
          }

          // back拡張
          Step back_s = s.prev_si_Step();
          boolean extended = false;
          if ( back_s != null && back_s.canExtend(n_aid) && s.b1 - s.b0 == limitLB - 1 ) {
            // 現在の信号を左寄せ(back_sを右寄せ)
            if ( back_s.b1 == limitLB - 1 ) {
              for (int n_dir = 0; n_dir <= 1; n_dir++) {
                int prev_w = s.aid1 - s.aid0 + 1;
                int n_b0 = Math.max(prev_w, back_s.b0);
                int n_b1 = limitLB - 1;
                int n_aid0 = 0;
                int n_aid1 = 0;
                if ( n_dir == 0 ) {
                  n_aid1 = Math.min(back_s.max_aid1(), n_aid + (n_b1 - n_b0)); // 最大拡張
                  if ( n_aid1 < back_s.aid1 ) continue; // 拡張不可
                  n_aid0 = Math.max(n_aid1 - (n_b1 - n_b0), 0); // 左端
                  // assert n_aid0 <= back_s.aid0;
                  n_aid1 = n_aid0 + (n_b1 - n_b0); // 拡張可能なら再拡張
                } else {
                  n_aid1 = Math.max(back_s.aid1, n_aid); // 一旦右寄せ
                  n_aid0 = Math.max(n_aid1 - (n_b1 - n_b0), 0); // その場合の左端
                  if ( n_aid < n_aid0 ) continue; // 拡張不可
                  n_aid1 = n_aid0 + (n_b1 - n_b0); // 拡張可能なら再拡張
                }
                if ( n_aid < n_aid0 || n_aid1 < n_aid ) continue;
                assert n_b0 > 0 && n_b0 < limitLB;
                int n_eval = n_dir == 0 ? n_aid1 - n_aid : n_aid - n_aid0;
                assert n_eval >= 0;
                if ( compareEval(n_aid, n_dir, s.C, n_eval) > 0 ) {
                  Step ns = new Step(s, n_aid, n_dir, n_aid0, n_aid1, n_b0);
                  // Step ns = new Step(s, n_aid, n_dir, s.C + 1);
                  assert compareEval(ns) > 0;
                  put(ns, cont);
                  // queue.addLast(ns);
                  queue0.addLast(ns);
                  extended = true;
                }
              }
            }
            // 現在の信号を右寄せ(back_sを左寄せ)
            if ( back_s.b0 == 0 ) {
              for (int n_dir = 0; n_dir <= 1; n_dir++) {
                int prev_w = s.aid1 - s.aid0 + 1;
                int n_b0 = 0;
                int n_b1 = Math.min(limitLB - 1 - prev_w, back_s.b1);
                int n_aid0 = 0;
                int n_aid1 = 0;
                if ( n_dir == 0 ) {
                  n_aid0 = back_s.aid0; // 一旦左寄せ
                  n_aid1 = Math.min(n_aid0 + (n_b1 - n_b0), in.LA - 1); // その場合の右端
                  if ( n_aid1 < n_aid ) continue;
                  n_aid0 = n_aid1 - (n_b1 - n_b0);
                } else {
                  n_aid0 = Math.max(back_s.min_aid0(), n_aid - (n_b1 - n_b0)); // 最大拡張
                  if ( n_aid0 > back_s.aid0 ) continue; // 拡張不可
                  n_aid1 = Math.min(n_aid0 + (n_b1 - n_b0), in.LA - 1);
                  // assert n_aid1 >= back_s.aid1;
                  n_aid0 = n_aid1 - (n_b1 - n_b0); // 拡張可能なら再拡張
                }
                if ( n_aid < n_aid0 || n_aid1 < n_aid ) continue;
                assert n_b1 < limitLB - 1 && n_b0 >= 0;
                int n_eval = n_dir == 0 ? n_aid1 - n_aid : n_aid - n_aid0;
                assert n_eval >= 0;
                if ( compareEval(n_aid, n_dir, s.C, n_eval) > 0 ) {
                  Step ns = new Step(s, n_aid, n_dir, n_aid0, n_aid1, n_b0);
                  // Step ns = new Step(s, n_aid, n_dir, s.C + 1);
                  assert compareEval(ns) > 0;
                  put(ns, cont);
                  // queue.addLast(ns);
                  queue0.addLast(ns);
                  extended = true;
                }
              }
            }
          }
          if ( extended ) continue;

          if ( s.C + 1 > Math.min(dm.getMinC(nt.id), target_C) ) continue;
          for (int n_dir = 0; n_dir <= 1; n_dir++) {
            // 新規の信号変更を行なって次に進む
            if ( compareEval(n_aid, n_dir, s.C + 1, limitLB - 1) > 0 ) {
              Step ns = new Step(s, n_aid, n_dir, s.C + 1);
              assert compareEval(ns) > 0;
              put(ns, cont);
              queue1.addLast(ns);
            }
          }
        }
      }
    }
    if ( cont.isFixed(t1) ) {
      for ( int faid = 0 ; faid < cont.fixedAdjCnt() ; faid++ ) {
        int aid = cont.vaidForFixed(t1, cont.fixedAdjTownByFaid(faid));
        for (int dir = 0; dir <= 1; dir++) {
          int dpid = dpid(aid, dir);
          Step s = dm.getBest(dpid);
          if ( s != null && s.C <= target_C + 0 ) {
            ret.add(s);
          }
        }
      }
    } else {
      for ( int aid : cont.aid(t1) ) {
        for (int dir = 0; dir <= 1; dir++) {
          int dpid = dpid(aid, dir);
          Step s = dm.getBest(dpid);
          if ( s != null && s.C <= target_C + 0 ) {
            ret.add(s);
          }
        }
      }
    }
    return ret;
  }

  ArrayList<Step> createInitStep(Controller cont) {
    Town t0 = g.town[0];
    ArrayList<Step> ret = new ArrayList<>();
    if ( cont.isFixed(t0) ) {
      Town escapeTown = cont.adjNonFixedTown(t0);
      int dummyAid = cont.aid(escapeTown)[0];
      Step s = new Step(dummyAid, 0, cont.vaidForFixed(t0, cont.fixedAdjTownByFaid(0)));
      ret.add(s);
    } else {
      for ( int aid : cont.aid(t0) ) {
        Step s = new Step(aid, 0);
        ret.add(s);
        break;
      }
    }
    return ret;
  }

  ArrayList<Command> createBestCommand(ArrayList<Step> steps, Controller cont) {
    Step cs = steps.get(0);
    for ( Step s : steps ) {
      if ( s.C < cs.C ) cs = s;
    }
    // Signal sig = null;
    // int sigC = Integer.MAX_VALUE;
    ArrayDeque<Command> com = new ArrayDeque<>();
    Town ct = null;
    Signal oldest = null;
    Signal[] sig = new Signal[cs.C + 1];
    // while ( cs != null ) {
    //   if ( sigC != cs.C ) {
    //     if ( sig != null ) com.addFirst(sig);
    //     sigC = cs.C;
    //     sig = new Signal(cs.aid1 - cs.aid0 + 1, cs.aid0, 0);//
    //   }
    //   if ( ct != cs.town(cont) ) {
    //     com.addFirst(new Move(cs.town(cont).id));
    //   }

    //   cs = cs.parent;
    // }
    int sigSi = Integer.MAX_VALUE;
    int stepC = Integer.MAX_VALUE;
    while ( cs != null ) {
      // main.debug(cs.toString());
      if ( sigSi != cs.si || stepC != cs.C ) {
        // if ( sig != null ) com.addFirst(sig);
        if ( sigSi != Integer.MAX_VALUE && sigSi == stepC ) {
          com.addFirst(sig[sigSi]);
        }
        sigSi = cs.si;
        stepC = cs.C;
        // sig = new Signal(cs.aid1 - cs.aid0 + 1, cs.aid0, 0);//
        sig[cs.si] = createSignal(cs, oldest);
        if ( oldest != null && oldest.si > cs.si ) oldest = null;
        if ( cs.si != cs.C ) oldest = sig[cs.si];
      }
      if ( ct != cs.town(cont) ) {
        com.addFirst(new Move(cs.town(cont).id));
      }

      cs = cs.parent;
    }
    ArrayList<Command> ret = new ArrayList<>();
    if ( cont.fixedCnt() > 0 ) {
      ret.add(new Signal(cont.fixedCnt(), in.LA - cont.fixedCnt(), in.LB - cont.fixedCnt()));
    }
    boolean move_flg = false;
    for ( Command c : com ) {
      if ( !move_flg && c instanceof Move ) {
        Move mv = (Move)c;
        if ( mv.v == 0 ) {
          continue;
        } else {
          move_flg = true;
        }
      }
      ret.add(c);
    }
    return ret;
  }

  Signal createSignal(Step s, Signal oldest) {
    if ( oldest != null && oldest.si < s.si ) {
      // あとで戻る場合
      int len = s.aid1 - s.aid0 + 1;
      if ( oldest.PB == 0 ) {
        // oldestが左寄せ、それまでは右寄せ
        return new Signal(len, s.aid0, limitLB - len, s.si);
      } else {
        // oldestが右寄せ、それまでは左寄せ
        return new Signal(len, s.aid0, 0, s.si);
      }
    } else if ( oldest != null && oldest.si == s.si ) {
      if ( oldest.si != s.C ) {
        return oldest; // 何もしない
      } else {
        // 最終補正
        return extend(oldest);
        // // oldestの生成。一旦必要な範囲のみ作成
        // int len = s.aid1 - s.aid0 + 1;
        // if ( oldest.PB == 0 ) {
        //   // oldestは左寄せ
        //   return new Signal(len, s.aid0, 0, s.si);
        // } else {
        //   // oldestは右寄せ
        //   return new Signal(len, s.aid0, limitLB - len, s.si);
        // }
      }
    } else {
      // return new Signal(s.aid1 - s.aid0 + 1, s.aid0, 0);
      return createSignal(s);
    }
  }
  Signal createSignal(Step s) {
    if ( s.si != s.C ) {
      // oldestの生成。一旦必要な範囲のみ作成
      int len = s.aid1 - s.aid0 + 1;
      if ( s.b0 == 0 ) {
        // oldestは左寄せ
        return new Signal(len, s.aid0, 0, s.si);
      } else {
        // oldestは右寄せ
        return new Signal(len, s.aid0, limitLB - len, s.si);
      }
    } else {
      return new Signal(s.aid1 - s.aid0 + 1, s.aid0, 0);
    }
  }
  Signal extend(Signal sig) {
    if ( sig.PB == 0 ) {
      // 右方向に可能な限り拡張
      int len = Math.min(limitLB, in.LA - sig.PA);
      return new Signal(len, sig.PA, 0, sig.si);
    } else {
      // 左方向に可能な限り拡張
      int aid1 = sig.PA + sig.l - 1;
      int len = Math.min(limitLB, aid1 + 1);
      int aid0 = aid1 - len + 1;
      int b0 = limitLB - len;
      return new Signal(len, aid0, b0, sig.si);
    }
  }

  Step getCurrent(Step s) {
    return dm.getBest(s.vdpid());
  }
  void put(Step s, Controller cont) {
    // void put(Step s, Town t) {
    dm.setBest(s.vdpid(), s);
    Town t = s.townAdj(cont);
    if ( !s.isFixed() || s.C != 0 ) {
      // 最初にfixedから始まった場合の対策
      dm.setMinC(t.id, Math.min(s.C, dm.getMinC(t.id)));
    }
  }
  int compareEval(Step s) { // old.compareEval(Step s)
    Step old = dm.getBest(s.vdpid());
    if ( old == null ) return 1;
    if ( old != null && s.C > old.C ) return -1;
    return old.compareTo(s);
  }
  int compareEval(int vaid, int dir, int C, int eval) {
    int dpid = dpid(vaid, dir);
    Step old = dm.getBest(dpid);
    if ( old == null ) return 1;
    if ( old != null && C > old.C ) return -1;
    int this_priority = priority(vaid, dir, C);
    return old.compareTo(this_priority, eval);
  }

  int dpid(int vaid, int dir) {
    return (vaid << 1) + dir;
  }
  int priority(int vaid, int dir, int C) {
    return (C << 12) | (vaid << 1) | dir;
  }

  class Step implements Comparable<Step> {
    int aid = 0; // controller上の位置
    int dir = 0; // 向き 0:正方向、1:負方向
    int aid0 = 0;
    int aid1 = 0;
    int C = 0;
    Step parent = null;
    // Step prevC_step = null;
    Step step0_C_si = null;

    int dpid = 0;
    int priority = 0;
    int eval = 0; // 拡張余地。大きい方が良い

    int fixed_aid = -1;
    int fixed_dpid = -1;

    int b0 = 0;
    int b1 = 0;
    int si = 0;

    // 初期生成
    Step(int aid, int dir) {
      this.aid = aid;
      this.dir = dir;
      this.aid0 = aid;
      this.aid1 = aid;
      this.C = 0;
      this.parent = null;
      // this.prevC_step = null;
      this.step0_C_si = this;

      this.b0 = 0;
      this.b1 = limitLB - 1;
      this.si = 0;

      init();
    }
    // 初期生成(fixed)
    Step(int aid, int dir, int fixed_aid) { // aidは実質ダミー
      this.aid = aid;
      this.dir = dir;
      this.aid0 = aid;
      this.aid1 = aid;
      this.C = 0;
      this.parent = null;
      // this.prevC_step = null;
      this.step0_C_si = this;

      this.fixed_aid = fixed_aid;
      this.fixed_dpid = dpid(fixed_aid, dir);

      this.b0 = 0;
      this.b1 = limitLB - 1;
      this.si = 0;

      init();
    }

    // targetへの探索の初期
    // Step(Step s0) {
    //   this.aid = s0.aid;
    //   this.dir = s0.dir;
    //   this.aid0 = s0.aid0;
    //   this.aid1 = s0.aid1;
    //   this.C = s0.C;
    //   this.parent = s0;
    //   this.prevC_step = s0.prevC_step;

    //   init();
    // }
    // 拡張
    Step(Step s0, int n_aid, int n_dir) {
      assert s0.canExtend(n_aid);
      this.aid = n_aid;
      this.dir = n_dir;
      this.aid0 = Math.min(s0.aid0, n_aid);
      this.aid1 = Math.max(s0.aid1, n_aid);
      this.C = s0.C;
      this.parent = s0;
      // this.prevC_step = s0.prevC_step;
      this.step0_C_si = s0.step0_C_si;

      this.b0 = s0.b0;
      this.b1 = s0.b1;
      this.si = s0.si;

      init();
    }
    // Fixedへの移動
    Step(Step s0, int fixed_aid) {
      this.aid = s0.aid;
      this.dir = s0.dir;
      this.aid0 = s0.aid0;
      this.aid1 = s0.aid1;
      this.C = s0.C;
      this.parent = s0;
      // this.prevC_step = s0.prevC_step;
      this.step0_C_si = s0.step0_C_si;

      this.b0 = s0.b0;
      this.b1 = s0.b1;
      this.si = s0.si;

      this.fixed_aid = fixed_aid;
      this.fixed_dpid = dpid(fixed_aid, s0.dir);

      init();
    }

    // 前の信号を再利用
    Step(Step s0, int aid, int dir, int aid0, int aid1, int b0) {
      this.aid = aid;
      this.dir = dir;
      this.aid0 = aid0;
      this.aid1 = aid1;
      this.C = s0.C;
      this.parent = s0;
      // this.prevC_step = s0;
      this.step0_C_si = this;

      this.b0 = b0;
      this.b1 = b0 + (aid1 - aid0);
      this.si = s0.prev_si_Step().si;

      init();
    }

    // 信号切り替え
    Step(Step s0, int aid, int dir, int C) {
      this.aid = aid;
      this.dir = dir;
      this.aid0 = aid;
      this.aid1 = aid;
      this.C = C;
      this.parent = s0;
      // this.prevC_step = s0;
      this.step0_C_si = this;

      this.b0 = 0;
      this.b1 = limitLB - 1;
      this.si = C;

      init();
    }
    void init() {
      this.dpid = dpid(aid, dir);
      this.priority = priority(aid, dir, C);
      this.eval = eval(aid, dir);
    }

    int eval(int n_aid, int n_dir) {
      if ( n_dir == 0 ) {
        // int n_i0 = Math.min(ci0, n_ci);
        // int max_i1 = n_i0 + in.LB - 1;
        // return Math.min(aid0, n_aid) + limitLB - 1 - n_aid;
        return Math.min(aid0, n_aid) + (b1 - b0) - n_aid;
      } else {
        // int n_i1 = Math.max(ci1, n_ci);
        // int min_i1 = n_i1 - in.LB + 1;
        // return n_aid - (Math.max(aid1, n_aid) - limitLB + 1);
        return n_aid - (Math.max(aid1, n_aid) - (b1 - b0));
      }
    }
    boolean canExtend(int n_aid) {
      if ( C == 0 ) return false;
      int n_aid0 = Math.min(aid0, n_aid);
      int n_aid1 = Math.max(aid1, n_aid);
      return n_aid1 - n_aid0 <= b1 - b0;
    }
    int min_aid0() {
      return Math.max(aid1 - (b1 - b0), 0);
    }
    int max_aid1() {
      return Math.min(aid0 + b1 - b0, in.LA - 1);
    }
    Town town(Controller cont) {
      if ( isFixed() ) {
        return cont.fixedTownByVaid(fixed_aid);
      } else {
        return cont.town(aid);
      }
    }
    Town townAdj(Controller cont) { // fixedの場合は元々の位置のTown。そうでない場合は現在Town
      if ( isFixed() ) {
        return cont.fixedAdjTownByVaid(fixed_aid);
      } else {
        return cont.town(aid);
      }
    }
    int vaid() {
      return isFixed() ? fixed_aid : aid;
    }
    int vdpid() {
      return isFixed() ? fixed_dpid : dpid;
    }

    boolean isFixed() {
      return fixed_aid != -1;
    }

    public int compareTo(int o_priority, int o_eval) {
      if ( priority != o_priority ) {
        return priority < o_priority ? -1 : 1;
      } else {
        return -eval + o_eval;
      }
    }

    Step prev_si_Step() {
      return step0_C_si.parent;
    }

    @Override
    public int compareTo(Step o) {
      if ( priority != o.priority ) {
        return priority < o.priority ? -1 : 1;
      } else {
        return -eval + o.eval;
      }
    }
    @Override
    public String toString() {
      return String.format("a[%d,%d],b[%d,%d],C=%d,si=%d,aid=%d,dir=%d",
        aid0, aid1, b0, b1, C, si, dir, aid
      );
    }
  }

  class DPMap {
    int curGen = 0;
    Step[] best = null;
    int[] bestGen = null;
    int[] minC = null;
    int[] minCGen = null;
    DPMap() {
      best = new Step[K];
      bestGen = new int[K];
      minC = new int[in.N];
      minCGen = new int[in.N];
    }
    void clear() {
      curGen++;
    }
    Step getBest(int dpid) {
      return bestGen[dpid] == curGen ? best[dpid] : null;
    }
    void setBest(int dpid, Step s) {
      best[dpid] = s;
      bestGen[dpid] = curGen;
    }

    int getMinC(int tid) {
      return minCGen[tid] == curGen ? minC[tid] : Input.MAX_C;
    }
    void setMinC(int tid, int C) {
      minC[tid] = C;
      minCGen[tid] = curGen;
    }
  }
}

abstract class Solver {
  final Input in;
  final Main main;
  static final long INF_SCORE = 100L * Input.MAX_C;
  Result best = null;

  Solver(Input in, Main main) {
    this.in = in;
    this.main = main;
  }
  Result solve(Result best) {
    this.best = best;
    solve();
    return this.best;
  }
  long bestScore() {
    return best == null ? INF_SCORE : best.stats().score;
  }
  void updateBest(Result result) {
    if ( result.stats().score < bestScore() ) {
      this.best = result;
    }
    main.debug(String.format("sol=%s, score=%d", result.stats().solverName, result.stats().score));
  }
  boolean isTimeOver() {
    boolean to = main.isTimeOver();
    if ( bestScore() < INF_SCORE && to ) {
      return true;
    } else {
      return false;
    }
    // return bestScore() < INF_SCORE && main.isTimeOver();
  }

  abstract void solve();

  protected void search(Controller cont, GraphStructure strc, String solverName) {
    SignalSearch ss = new SignalSearch(in, main);

    ArrayList<SignalSearch.Step> curSteps = ss.createInitStep(cont);
    for ( int ti : in.t ) {
      if ( isTimeOver() ) return;
      ArrayList<SignalSearch.Step> nextSteps = null;
      nextSteps = ss.search(curSteps, in.g.town[ti], cont, strc);
      assert nextSteps.size() > 0;
      curSteps = nextSteps;
    }
    Result result = new Result(in);
    result.A = cont.A;
    ArrayList<Command> com = ss.createBestCommand(curSteps, cont);
    for ( Command c : com ) result.addCommand(c);
    result.stats().setGraphInfo(in.g);
    result.stats().solverName = solverName;
    result.stats().memo = strc.memo();
    updateBest(result);
  }
}
class GridSolver extends Solver {
  GridSolver(Input in, Main main) {
    super(in, main);
  }
  @Override
  void solve() {
    Graph g = in.g;
    int DIVx = 7;
    int DIVy = 7;
    if ( (in.LA > 900 && in.LB >= 10) || (in.LA <= 900 && in.LB >= 12) ) {
      DIVx = 1;
      DIVy = 1;
    } else {
      DIVx = 7;
      DIVy = 7;
    }
    GridStructure strc = new GridStructure(g, main, DIVx, DIVy);
    // strc.createHub1();
    int[] A = strc.createGrid(in.LA);

    Controller cont = new Controller(in, A);
    search(cont, strc, String.format("N%d%d", DIVx, DIVy));
    // SignalSearch ss = new SignalSearch(in);

    // ArrayList<SignalSearch.Step> curSteps = ss.createInitStep(cont);
    // for ( int ti : in.t ) {
    //   ArrayList<SignalSearch.Step> nextSteps = null;
    //   nextSteps = ss.search(curSteps, g.town[ti], cont, strc);
    //   assert nextSteps.size() > 0;
    //   curSteps = nextSteps;
    // }
    // Result result = new Result(in);
    // result.A = A;
    // ArrayList<Command> com = ss.createBestCommand(curSteps, cont);
    // for ( Command c : com ) result.addCommand(c);
    // result.stats().setGraphInfo(g);
    // result.stats().solverName = String.format("N%d%d", DIVx, DIVy);
    // return result;
  }
}
class HubSolver extends Solver {
  HubStructure strc = null;
  HubSolver(Input in, Main main, HubStructure strc) {
    super(in, main);
    this.strc = strc;
  }
  @Override
  void solve() {
    if ( in.LB < 4 ) return;
    // Graph g = in.g;
    // HubStructure strc = null;
    // strc = new HubFixed1Structure(g, main);
    // strc = new HubTriangleStructure(g, main);
    // strc = new HubLine1Structure(g, main);
    // strc = new HubSquareStructure(g, main);
    // strc = new HubLongLineStructure(g, main);
    strc.createHub();
    int[] A = null;
    if ( strc.estimate_score() >= 0 ) {
      A = strc.createA();
    } else {
      return;
    }

    Controller cont = new Controller(in, A);
    for ( Town t : strc.fixedTown ) {
      cont.setFixed(t);
    }
    cont.fixedCommit();
    search(cont, strc, strc.name());
  }
}
abstract class AbstractResult {
  abstract long eval();
}

class Result extends AbstractResult {
  final Input in;
  ResultStats stats = null;

  int[] A = null;
  ArrayList<Command> commands = null;

  Result(Input in) {
    this.in = in;
    this.commands = new ArrayList<>();
  }

  void addCommand(Command c) {
    if ( c instanceof Signal ) stats().score++;
    commands.add(c);
  }

  ResultStats stats() {
    createStatsIfNotExists();
    return stats;
  }
  private void createStatsIfNotExists() {
    if ( stats == null ) stats = new ResultStats();
  }

  @Override
  long eval() {
    return stats == null ? 0 : stats.score;
  }

  @Override
  public String toString() {
    StringBuilder out = new StringBuilder();
    for (int i = 0; i < in.LA; i++) {
      out.append(A[i]);
      out.append(" ");
    }
    out.deleteCharAt(out.length() - 1);
    out.append("\n");
    for ( Command c : commands ) {
      out.append(c.toString());
      out.append("\n");
    }
    out.deleteCharAt(out.length() - 1);
    return out.toString();
  }
}
class ResultStats {
  int caseNum = 0;
  long elapsed = 0;
  int iterations = 0;
  long score = 0;
  int min_MaxDepth = 0;
  int connectedCnt1 = 0;
  int maxConnectedCnt = 0;
  String solverName = null;
  String memo = "";
  void setGraphInfo(Graph g) {
    this.min_MaxDepth = g.min_MaxDepth;
    this.maxConnectedCnt = g.maxConnectedCnt;
    this.connectedCnt1 = g.connectedCnt1;
  }
}
class UnionFind {
	int[] parent = null;
	int[] size = null;
	
	UnionFind(int N) {
		parent = new int[N];
		size = new int[N];
		for ( int i = 0 ; i < N ; i++ ) {
			parent[i] = i;
			size[i] = 1;
		}
	}

	int root(int i) {
		return parent[i] == i ? i : (parent[i] = root(parent[i]));
	}
	int size(int i) {
		return size[root(i)];
	}

	void unite(int i, int j) {
		int ri = root(i);
		int rj = root(j);
		if ( ri == rj ) {
			return;
		} else {
			if ( size[ri] < size[rj] ) {
				parent[ri] = rj;
				size[rj] += size[ri];
			} else {
				parent[rj] = ri;
				size[ri] += size[rj];
			}
		}
	}
	
	boolean isSame(int i, int j) {
		return root(i) == root(j);
	}
}

class FastBitSet {
  private final long[] words;
  private static final int ADDRESS_BITS_PER_WORD = 6;
  private static final long WORD_MASK = 0xffffffffffffffffL;
  private static final int BITS_PER_WORD = 1 << ADDRESS_BITS_PER_WORD;

  public FastBitSet(int n) {
    words = new long[wordIndex(n - 1) + 1];
  }
  private FastBitSet(FastBitSet from) {
    words = from.words.clone();
  }

  @Override
  public FastBitSet clone() {
    return new FastBitSet(this);
  }

  private static int wordIndex(int bitIndex) {
    return bitIndex >> ADDRESS_BITS_PER_WORD;
  }

  public boolean get(int bitIndex) {
    int wordIndex = wordIndex(bitIndex);
    return (words[wordIndex] & (1L << bitIndex)) != 0;
  }

  public void set(int bitIndex) {
    int wordIndex = wordIndex(bitIndex);
    words[wordIndex] |= (1L << bitIndex);
  }

  public void clear(int bitIndex) {
    int wordIndex = wordIndex(bitIndex);
    words[wordIndex] &= ~(1L << bitIndex);
  }

  public int cardinality() {
    int ret = 0;
    for (int i = 0; i < words.length; i++)
      ret += Long.bitCount(words[i]);
    return ret;
  }
  public void or(FastBitSet o) {
    for (int i = 0 ; i < words.length ; i++) {
      words[i] |= o.words[i];
    }
  }
  public void and(FastBitSet o) {
    for (int i = 0 ; i < words.length ; i++) {
      words[i] &= o.words[i];
    }
  }
  public int excludesCount(FastBitSet excludes) {
    int ret = 0;
    for (int i = 0; i < words.length; i++)
      ret += Long.bitCount(words[i] & ~excludes.words[i]);
    return ret;
  }

  public int nextSetBit(int fromIndex) {
    int u = wordIndex(fromIndex);

    long word = words[u] & (WORD_MASK << fromIndex);
    while (true) {
      if (word != 0) return (u * BITS_PER_WORD) + Long.numberOfTrailingZeros(word);
      if ( ++u == words.length ) return -1;
      word = words[u];
    }
  }

  public int previousSetBit(int fromIndex) {
    int u = wordIndex(fromIndex);
    long word = words[u] & (WORD_MASK >>> -(fromIndex+1));

    while (true) {
      if (word != 0) return (u + 1) * BITS_PER_WORD - 1 - Long.numberOfLeadingZeros(word);
      if (u-- == 0) return -1;
      word = words[u];
    }
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
