import java.io.InputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.BitSet;
import java.util.Collections;
import java.util.function.BiPredicate;
import java.util.NoSuchElementException;
import java.util.Random;

class Solver1 {
	final Main main;
	final Input in;
	final Grid grid;
	int pattern = 0;
	Solver1(Input in, Main main, int pattern) {
		this.in = in;
		this.main = main;
		this.grid = in.grid;
		this.pattern = pattern;
	}

	Conveyor main_cv = null;
	SwapConveyor[] swap_cv = null;
	Result solve() {
		ArrayList<Conveyor> conveyors = createConvayor();
		ArrayList<Move> best_moves = null;
		int min_t = -main_cv.size();
		int max_t = 0;
		while (max_t - min_t > 1) {
			int mid = (min_t + max_t) / 2;
			ArrayList<Move> moves = solve0(mid);
			if (moves == null) {
				max_t = mid;
			} else {
				if (best_moves == null || moves.size() < best_moves.size()) {
					best_moves = moves;
				}
				min_t = mid;
			}
		}
		Result res = new Result(in);
		res.conveyors = conveyors;
		res.moves = best_moves;
		res.score = 1000000 + Math.round(1000000 * Math.log(100000.0 / best_moves.size()) / Math.log(2));
		return res;
	}
	ArrayList<Move> solve0(int t0) {
		ArrayList<Move> moves = new ArrayList<>();
		Board bd = new Board(in);
		for (int t = t0; t < in.N * in.N; t++) {
			for (int i = 0; i < main_cv.size(); i++) {
				int sb = t + i;
				Pos p = main_cv.p(i);
				if (swap_cv[p.id] == null) continue;
				SwapConveyor sc = swap_cv[p.id];
				if (sb < 0) {
					int b = bd.box(p);
					if (0 <= b && b < sb + main_cv.size()) {
						int b1 = bd.box(sc.p1());
						if (b1 != -1 && b1 >= main_cv.size()) {
							Move m = new Move(sc, 1);
							moves.add(m);
							bd.apply(m);
						}
					}
				} else {
					int b1 = bd.box(sc.p1());
					if (sb == b1) {
						Move m = new Move(sc, 1);
						moves.add(m);
						bd.apply(m);
					}
				}
			}
			Move m = new Move(main_cv, -1);
			moves.add(m);
			bd.apply(m);
		}
		return bd.nb == in.N * in.N ? moves : null;
	}

	ArrayList<Conveyor> createConvayor() {
		if (pattern < 4) {
			return createConvayor0(pattern % 2, (pattern % 4) / 2);
		} else {
			return createConvayor1(pattern % 2, (pattern % 4) / 2);
		}
	}

	ArrayList<Conveyor> createConvayor0(int dir, int ref) {
		ArrayList<Conveyor> conveyors = new ArrayList<>();
		ArrayList<Pos> main_loop = new ArrayList<>();
		for (int j = in.N - 2; j >= 0; j--) main_loop.add(pos(0, j, ref));
		main_loop.add(pos(1, 0, ref));
		for (int j = 0; j < in.N; j += 4) {
			for (int i = 2; i < in.N; i++) main_loop.add(pos(i, j, ref));
			main_loop.add(pos(in.N - 1, j + 1, ref));
			for (int i = in.N - 1; i >= 2; i--) main_loop.add(pos(i, j + 2, ref));
			if (j + 3 != in.N - 1) main_loop.add(pos(2, j + 3, ref));
		}
		main_loop.add(pos(1, in.N - 2, ref));
		if (dir == 0) {
			Collections.reverse(main_loop);
		}
		main_loop = shift(main_loop);
		main_cv = new Conveyor(conveyors.size());
		main_cv.addAll(main_loop);
		conveyors.add(main_cv);

		swap_cv = new SwapConveyor[in.N * in.N];
		for (int j = 1; j < in.N - 2; j++) {
			Pos p0 = pos(0, j, ref);
			Pos p1 = pos(1, j, ref);
			swap_cv[p0.id] = new SwapConveyor(conveyors.size(), p0, p1);
			conveyors.add(swap_cv[p0.id]);
		}
		for (int j = 0; j < in.N; j += 2) {
			for (int i = 0; i < in.N; i++) {
				if (i == 0 && j != in.N - 2) continue;
				if (i == 1 && j != 0 && j != in.N - 2) continue;
				if (i == 2 && j % 4 != 0 && j != in.N - 2) continue;
				if (i == in.N - 1 && j % 4 == 0) continue;
				Pos p0 = pos(i, j, ref);
				Pos p1 = pos(i, j + 1, ref);
				swap_cv[p0.id] = new SwapConveyor(conveyors.size(), p0, p1);
				conveyors.add(swap_cv[p0.id]);
			}
		}
		return conveyors;
	}
	Pos pos(int i, int j, int ref) {
		return ref == 0 ? grid.pos(i, j) : grid.pos(i, in.N - j - 1);
	}

	ArrayList<Conveyor> createConvayor1(int dir, int ref) {
		ArrayList<Conveyor> conveyors = new ArrayList<>();
		ArrayList<Pos> main_loop = new ArrayList<>();
		for (int i = 0; i < in.N; i += 4) {
			int j0 = i == 0 ? 0 : 2;
			for (int j = j0; j < in.N; j++) main_loop.add(pos(i, j, ref));
			main_loop.add(pos(i + 1, in.N - 1, ref));
			int j1 = i == in.N - 4 ? 0 : 2;
			for (int j = in.N - 1; j >= j1; j--) main_loop.add(pos(i + 2, j, ref));
			if (i != in.N - 4) main_loop.add(pos(i + 3, 2, ref));
		}
		for (int i = in.N - 3; i >= 1; i--) main_loop.add(pos(i, 0, ref));
		if (dir == 0) {
			Collections.reverse(main_loop);
		}
		main_loop = shift(main_loop);
		main_cv = new Conveyor(conveyors.size());
		main_cv.addAll(main_loop);
		conveyors.add(main_cv);

		swap_cv = new SwapConveyor[in.N * in.N];
		for (int i = 1; i < in.N - 2; i++) {
			Pos p0 = pos(i, 0, ref);
			Pos p1 = pos(i, 1, ref);
			swap_cv[p0.id] = new SwapConveyor(conveyors.size(), p0, p1);
			conveyors.add(swap_cv[p0.id]);
		}
		for (int i = 0; i < in.N; i += 2) {
			for (int j = 0; j < in.N; j++) {
				if (j == 0 && i != in.N - 2) continue;
				if (j == 1 && i != 0 && i != in.N - 2) continue;
				if (j == 2 && i % 4 != 0 && i != in.N - 2) continue;
				if (j == in.N - 1 && i % 4 == 0) continue;
				Pos p0 = pos(i, j, ref);
				Pos p1 = pos(i + 1, j, ref);
				swap_cv[p0.id] = new SwapConveyor(conveyors.size(), p0, p1);
				conveyors.add(swap_cv[p0.id]);
			}
		}
		return conveyors;
	}

	ArrayList<Pos> shift(ArrayList<Pos> org) {
		int ii = org.indexOf(in.pe);
		ArrayList<Pos> ret = new ArrayList<>();
		for (int i = 0; i < org.size(); i++) {
			ret.add(org.get((i + ii) % org.size()));
		}
		return ret;
	}
}

class Board {
	final Input in;
	int[] box;
	Pos[] pos;
	int nb = 0;
	Board(Input in) {
		this.in = in;
		this.box = Arrays.copyOf(in.a0, in.N * in.N);
		this.pos = Arrays.copyOf(in.p0, in.N * in.N);
	}
	void apply(Move m) {
		apply(m.cv, m.d);
	}
	void apply(Conveyor cv, int d) {
		if (d == 1) {
			int b0 = box(cv.p(0));
			for (int i = cv.size() - 1; i >= 1; i--) {
				Pos p1 = cv.p(i);
				Pos p2 = cv.p(i + 1);
				int b = box(p1);
				set(p2, b);
			}
			set(cv.p(1), b0);
		} else {
			int b0 = box(cv.p(0));
			for (int i = 1; i < cv.size(); i++) {
				Pos p1 = cv.p(i);
				Pos p2 = cv.p(i - 1);
				int b = box(p1);
				set(p2, b);
			}
			set(cv.p(cv.size() - 1), b0);
		}
		checkExit();
	}
	void checkExit() {
		if (box(in.pe) == nb) {
			set(in.pe, -1);
			nb++;
		}
	}
	int box(Pos p) {
		return box[p.id];
	}
	Pos pos(int b) {
		return pos[b];
	}
	void set(Pos p, int b) {
		box[p.id] = b;
		if (b >= 0) pos[b] = p;
	}
}

class Conveyor {
	ArrayList<Pos> mem;
	int id;
	Conveyor(int id) {
		this.id = id;
		this.mem = new ArrayList<>();
	}
	int size() {
		return mem.size();
	}
	Pos p(int i) {
		int ii = i % mem.size();
		if (ii < 0) ii += mem.size();
		return mem.get(ii);
	}
	void add(Pos p) {
		mem.add(p);
	}
	void addAll(ArrayList<Pos> arr) {
		mem.addAll(arr);
	}
	@Override
	public String toString() {
		StringBuilder out = new StringBuilder();
		out.append(mem.size());
		for (Pos p: mem) {
			out.append(" ").append(p.i).append(" ").append(p.j);
		}
		return out.toString();
	}
}

class SwapConveyor extends Conveyor {
	SwapConveyor(int id, Pos p0, Pos p1) {
		super(id);
		add(p0);
		add(p1);
	}
	Pos p0() {
		return mem.get(0);
	}
	Pos p1() {
		return mem.get(1);
	}
}
class Move {
	Conveyor cv = null;
	int d = 0;
	Move(Conveyor cv, int d) {
		this.cv = cv;
		this.d = d;
	}
	@Override
	public String toString() {
		return String.format("%d %d", cv.id, d);
	}
}

class Pos {
	private final Grid grid;
	public final int i;
	public final int j;
	public final int id;

	public ArrayList<Pos> adj = null;
	public Pos[] adj4 = null;
	public Pos[] adj8 = null;

	public Pos(int i, int j, int id, Grid grid) {
		this.i = i;
		this.j = j;
		this.id = id;
		this.grid = grid;
	}
	public void initAdj(BiPredicate<Pos, Pos> existsWall) {
		adj = new ArrayList<>();
		adj4 = new Pos[Direction.DIR.length];
		for (Direction d: Direction.DIR) {
			int ai = i + d.di, aj = j + d.dj;
			if (!grid.isValid(ai, aj)) continue;
			Pos a = grid.pos(ai, aj);
			if (a == null || existsWall.test(this, a)) continue;
			adj4[d.id] = a;
			adj.add(a);
		}
	}
	public void initAdj8(BiPredicate<Pos, Pos> existsWall) {
		adj = new ArrayList<>();
		adj8 = new Pos[Direction.DIR8.length];
		for (Direction d: Direction.DIR8) {
			int ai = i + d.di, aj = j + d.dj;
			if (!grid.isValid(ai, aj)) continue;
			Pos a = grid.pos(ai, aj);
			if (a == null || existsWall.test(this, a)) continue;
			adj8[d.id8] = a;
			adj.add(a);
		}
	}
	public Pos next(Direction d) {
		return adj4[d.id];
	}
	public Pos next(Direction d, int cnt) {
		int ni = i + d.di * cnt;
		int nj = j + d.dj * cnt;
		return grid.isValid(ni, nj) ? grid.pos(ni, nj) : null;
	}
	public Pos next8(Direction d) {
		return adj8[d.id8];
	}
	public Pos next8(Direction d, int cnt) {
		return next(d, cnt);
	}

	@Override
	public int hashCode() {
		return id;
	}
	@Override
	public boolean equals(Object o) {
		if (o instanceof Pos) {
			return id == ((Pos)o).id;
		} else {
			return false;
		}
	}
	@Override
	public String toString() {
		return String.format("(%d,%d)", i, j);
	}
}
enum Direction {
	R(0, 1, 0, 0, "R"),
	D(1, 0, 1, 2, "D"),
	L(0, -1, 2, 4, "L"),
	U(-1, 0, 3, 6, "U"),
	RD(1, 1, -1, 1, "RD"),
	LD(1, -1, -1, 3, "LD"),
	LU(-1, -1, -1, 5, "LU"),
	RU(-1, 1, -1, 7, "RU")
	;
	public static final Direction[] DIR = {R, D, L, U};
	public static final Direction[] DIR8 = {R, RD, D, LD, L, LU, U, RU};
	public final int di;
	public final int dj;
	public final int id;
	public final int id8;
	public final String name;

	private Direction(int di, int dj, int id, int id8, String name) {
		this.di = di;
		this.dj = dj;
		this.id = id;
		this.id8 = id8;
		this.name = name;
	}
	public Direction rev() {
		return DIR8[(this.id8 + 4) % DIR8.length];
	}
	public Direction rot() {
		return DIR8[(this.id8 + 2) % DIR8.length];
	}
	public Direction rot(int i) {
		int ri = (this.id8 + i * 2) % DIR8.length;
		if (ri < 0) ri += DIR8.length;
		return DIR8[ri];
	}
	public Direction rot45() {
		return DIR8[(this.id8 + 1) % DIR8.length];
	}
	public Direction rot45(int i) {
		int ri = (this.id8 + i) % DIR8.length;
		if (ri < 0) ri += DIR8.length;
		return DIR8[ri];
	}
	public Direction irot() {
		return DIR8[(this.id8 + 6) % DIR8.length];
	}
	public Direction irot(int i) {
		int ri = (this.id8 - i * 2) % DIR8.length;
		if (ri < 0) ri += DIR8.length;
		return DIR8[ri];
	}
	public Direction irot45() {
		return DIR8[(this.id8 + 7) % DIR8.length];
	}
	public Direction irot45(int i) {
		int ri = (this.id8 - i) % DIR8.length;
		if (ri < 0) ri += DIR8.length;
		return DIR8[ri];
	}
	public boolean isHorizontal() {
		return di == 0;
	}
	public boolean isVertical() {
		return dj == 0;
	}
	@Override
	public String toString() {
		return name;
	}
}

class Grid {
	private final int H;
	private final int W;
	private final Pos[][] pos;
	public final Pos[] pos1;

	public Grid(int N) {
		this(N, N);
	}
	public Grid(int H, int W) {
		this.H = H;
		this.W = W;
		this.pos = new Pos[H][W];
		this.pos1 = new Pos[H * W];
		for (int i = 0; i < H; i++) {
			for (int j = 0; j < W; j++) {
				pos[i][j] = new Pos(i, j, i * W + j, this);
				pos1[pos[i][j].id] = pos[i][j];
			}
		}
	}
	public void initAdj() {
		initAdj((p1, p2) -> false);
	}
	public void initAdj(BitSet[] horizontalWall, BitSet[] verticalWall) {
		initAdj((p1, p2) -> {
			if (p1.i == p2.i) {
				if (Math.abs(p1.j - p2.j) == 1) {
					return verticalWall[p1.i].get(Math.min(p1.j, p2.j));
				} else {
					return false;
				}
			} else if (p1.j == p2.j) {
				if (Math.abs(p1.i - p2.i) == 1) {
					return horizontalWall[Math.min(p1.i, p2.i)].get(p1.j);
				} else {
					return false;
				}
			} else {
				return false;
			}
		});
	}
	public void initAdj(BiPredicate<Pos, Pos> existsWall) {
		for (Pos p: pos1) {
			p.initAdj(existsWall);
		}
	}
	public void initAdj8() {
		initAdj8((p1, p2) -> false);
	}
	public void initAdj8(BiPredicate<Pos, Pos> existsWall) {
		for (Pos p: pos1) {
			p.initAdj8(existsWall);
		}
	}

	public Pos pos(int i, int j) {
		return pos[i][j];
	}
	public Pos pos(int id) {
		return pos1[id];
	}
	public boolean isValid(int i, int j) {
		return 0 <= i && i < H && 0 <= j && j < W;
	}
	public int H() {
		return H;
	}
	public int W() {
		return W;
	}
	public int size() {
		return H * W;
	}
}

class Main {
  static final long TL = 1650;

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
  private int seq = 0;

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
    for (int pattern = 0; pattern < 8; pattern++) {
      Solver1 sol = new Solver1(in, this, pattern);
      Result res = sol.solve();
      if (best == null || res.score() > best.score()) {
        best = res;
      }
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
  int seq() {
    return seq++;
  }
  double rndDouble() {
    return rnd.nextDouble();
  }

  public void debug(String str) {}
}
class Input {
	int N = 0;
	int[][] a = null;
	int[] a0 = null;
	Pos[] p0 = null;
	Grid grid = null;
	Pos pe = null;
	public void readParameter(FastScanner sc) {
		N = sc.nextInt();
		a = new int[N][N];
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				a[i][j] = sc.nextInt();
			}
		}
	}

	public void preCalc() {
		grid = new Grid(N);
		a0 = new int[N * N];
		p0 = new Pos[N * N];
		for (Pos p: grid.pos1) {
			a0[p.id] = a[p.i][p.j];
			p0[a[p.i][p.j]] = p;
		}
		pe = grid.pos(0, N / 2);
	}
}

class Result {
	final Input in;

	// 統計情報
	int caseNum = 0;
	long elapsed = 0;
	int iterations = 0;
	long score = 0;

	// 結果情報
	ArrayList<Conveyor> conveyors = null;
	ArrayList<Move> moves = null;

	Result(Input in) {
		this.in = in;
	}

	long score() {
		return score;
	}

	@Override
	public String toString() {
		StringBuilder out = new StringBuilder();
		out.append(conveyors.size());
		for (Conveyor cv: conveyors) {
			out.append('\n');
			out.append(cv.toString());
		}
		out.append('\n');
		out.append(moves.size());
		for (Move m: moves) {
			out.append('\n');
			out.append(m.toString());
		}
		return out.toString();
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
