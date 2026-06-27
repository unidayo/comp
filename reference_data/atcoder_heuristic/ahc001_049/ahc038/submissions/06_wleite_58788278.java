import java.io.*;
import java.util.*;

public class Main {
    private static final long timeout = System.currentTimeMillis() + 2880;
    private static int n, nn, en, v, pending, bestScore, rnd = 99999999;
    private static byte[] grid;
    private static int[] value, targets;
    private static Integer[][] auxOrd;
    private static final int[] rx = {1,0,-1,0}, ry = {0,1,0,-1}, matchIdx1 = new int[16], matchPos1 = new int[16], matchRot1 = new int[16], matchIdx2 = new int[16], matchPos2 = new int[16], matchRot2 = new int[16];
    private static final int[][][] memoRots = new int[16][][], memoOrd = new int[2][][];
    private static final char[] moves = ".RDLU".toCharArray();
    private static final State[] auxItems = new State[32768];
    private static State best;

    public static void main(String[] args) throws Exception {
        init();
        bestScore = 500;
        int[] a0 = config();
        bs(a0, 16);
        while (System.currentTimeMillis() < timeout - 100) {
            bs(a0, 0);
        }
        output();
    }

    private static void bs(int[] a0, int fixed) {
        int[][] rots = getRots(a0.length);
        int width = fixed > 0 ? fixed : Math.max(16, Math.min(32768, (int) (1000 * (timeout - System.currentTimeMillis()) / (1.807 * Math.pow(pending, 0.225) * Math.pow(a0.length, 2.845) * Math.pow(v, 0.352) * Math.pow(bestScore, 0.65)))));
        int alfa = Math.min((n + 2) / 3, v - 1 - a0.length);
        Beam currStates = new Beam(width);
        Beam nextStates = new Beam(width);
        for (int y = n / 2 - 7; y <= n / 2 + 7; y += 2) {
            for (int x = n / 2 - 7; x <= n / 2 + 7; x += 2) {
                State s = new State();
                s.grid = grid;
                s.cx = x;
                s.cy = y;
                s.arms0 = a0.clone();
                s.arms1 = new int[v - 1 - s.arms0.length];
                int add = 1;
                if (s.arms1.length <= 2) add = 1 + nextInt(4);
                for (int i = 0; i < s.arms1.length; i++) {
                    s.arms1[i] = i + add;
                }
                shuffle(s.arms0);
                s.dir = new int[v - 1];
                s.pick = new boolean[s.arms1.length];
                Arrays.fill(s.pick, true);
                currStates.add(s);
            }
        }
        State ns = new State();
        OUT: while (currStates.size() > 0) {
            long t0 = System.currentTimeMillis();
            if (t0 >= timeout) break;
            for (int si = 0; si < currStates.size(); si++) {
                if ((si & 31) == 31 && System.currentTimeMillis() >= timeout) break;
                State cs = currStates.get(si);
                for (int[] r : rots) {
                    int ax = cs.cx;
                    int ay = cs.cy;
                    int d = 0;
                    for (int i = 0; i < r.length; i++) {
                        int a = cs.arms0[i];
                        d += cs.dir[i] + r[i] + 4;
                        d &= 3;
                        ax += a * rx[d];
                        ay += a * ry[d];
                    }
                    for (int m = -1; m < rx.length; m++) {
                        ns.cx = cs.cx;
                        ns.cy = cs.cy;
                        if (m >= 0) {
                            ns.cx += rx[m];
                            ns.cy += ry[m];
                            if (ns.cx < alfa || ns.cx >= n - alfa || ns.cy < alfa || ns.cy >= n - alfa) continue;
                        }
                        int mx = ax;
                        int my = ay;
                        if (m >= 0) {
                            mx += rx[m];
                            my += ry[m];
                        }
                        ns.turns = cs.turns + 1;
                        int cnt1 = 0;
                        int sum1 = 0;
                        int cnt2 = 0;
                        for (int i = 0; i < cs.arms1.length; i++) {
                            int a = cs.arms1[i];
                            byte look = cs.pick[i] ? (byte) 1 : (byte) 2;
                            int di = d + cs.dir[i + cs.arms0.length];
                            int add1 = 0;
                            int pp1 = -1;
                            int jj1 = -1;
                            int add2 = 0;
                            int pp2 = -1;
                            int jj2 = -1;
                            int end = cs.turns > 0 && cs.cmd[1 + i + v + cs.arms0.length] == 0 ? 2 : 1;
                            for (int j = -1; j <= end; j++) {
                                int ad = (di + j + 4) & 3;
                                int nx = mx + a * rx[ad];
                                if (nx < 0 || nx >= n) continue;
                                int ny = my + a * ry[ad];
                                if (ny < 0 || ny >= n) continue;
                                int np = ny * n + nx;
                                if (cs.grid[np] == look) {
                                    int nv = value[np];
                                    if (nv > add1) {
                                        add2 = add1;
                                        pp2 = pp1;
                                        jj2 = jj1;
                                        add1 = nv;
                                        pp1 = np;
                                        jj1 = j;
                                    } else if (nv > add2) {
                                        add2 = nv;
                                        pp2 = np;
                                        jj2 = j;
                                    }
                                }
                            }
                            if (pp1 >= 0) {
                                matchRot1[cnt1] = jj1;
                                matchIdx1[cnt1] = i;
                                matchPos1[cnt1++] = pp1;
                                sum1 += add1;
                            }
                            if (pp2 >= 0) {
                                matchRot2[cnt2] = jj2;
                                matchIdx2[cnt2] = i;
                                matchPos2[cnt2++] = pp2;
                            }
                        }
                        ns.bonus = 0;
                        if (cnt1 == 0) {
                            if (cs.turns + 2 >= bestScore) continue;
                            State last = nextStates.last();
                            if (last != null && last.score > cs.score) continue;
                            State first = nextStates.first();
                            updateBonus(ns, cs, r, mx, my, d, (first == null || first.bonus == 1000));
                        }
                        for (int rep = 0; rep < 2; rep++) {
                            if (rep == 1) {
                                if (cnt2 == 0) break;
                                boolean ok = false;
                                for (int i = 0; i < cnt2; i++) {
                                    if (nextInt(2) != 0) continue;
                                    int idx2 = matchIdx2[i];
                                    for (int j = 0; j < cnt1; j++) {
                                        if (idx2 == matchIdx1[j]) {
                                            int p1 = matchPos1[j];
                                            int p2 = matchPos1[j] = matchPos2[i];
                                            matchRot1[j] = matchRot2[i];
                                            sum1 += value[p2] - value[p1];
                                            ok = true;
                                            break;
                                        }
                                    }
                                }
                                if (!ok) break;
                            }
                            ns.turns = cs.turns + 1;
                            ns.score = cs.score + sum1;
                            ns.solved = cs.solved + cnt1;
                            if (nextStates.add(ns)) {
                                ns.grid = cs.grid.clone();
                                ns.pick = cs.pick.clone();
                                ns.dir = cs.dir.clone();
                                ns.cmd = new byte[v << 1];
                                ns.parent = cs;
                                ns.arms0 = cs.arms0;
                                ns.arms1 = cs.arms1;
                                for (int i = 0; i < cnt1; i++) {
                                    int idx = matchIdx1[i];
                                    int pos = matchPos1[i];
                                    int cr = matchRot1[i];
                                    ns.cmd[v + 1 + ns.arms0.length + idx] = (byte) 'P';
                                    if (cr != 0) ns.cmd[ns.arms0.length + 1 + idx] = cr == -1 ? (byte) 'L' : cr == 1 ? (byte) 'R' : (byte) 'W';
                                    ns.pick[idx] = !ns.pick[idx];
                                    ns.grid[pos] = 0;
                                    ns.dir[idx + ns.arms0.length] = (ns.dir[idx + ns.arms0.length] + cr + 4) & 3;
                                }
                                for (int j = 0; j < r.length; j++) {
                                    ns.dir[j] = (ns.dir[j] + r[j] + 4) & 3;
                                }
                                ns.cmd[0] = (byte) moves[m + 1];
                                for (int i = 0; i < r.length; i++) {
                                    int ri = r[i];
                                    if (ri != 0) ns.cmd[i + 1] = ri == -1 ? (byte) 'L' : (byte) 'R';
                                }
                                if (ns.solved == pending << 1) {
                                    best = ns;
                                    bestScore = best.turns;
                                    break OUT;
                                }
                                State o = ns;
                                ns = new State();
                                ns.cx = o.cx;
                                ns.cy = o.cy;
                            }
                        }
                    }
                }
            }
            if (nextStates.size() == 0) break;
            int turn = nextStates.get(0).turns;
            if (turn + 1 >= bestScore) break;
            Beam auxStates = currStates;
            currStates = nextStates;
            nextStates = auxStates;
            nextStates.clear();
            if (fixed == 0 && turn > 1) {
                long t1 = System.currentTimeMillis();
                if (t1 >= timeout) break;
                double rem = (t1 - t0) * (bestScore - turn);
                double f = Math.sqrt(Math.max(timeout - t1 - 10, 0) / Math.max(1, rem));
                if (f < 0.5) f = 0.5;
                else if (f > 1.5) f = 1.5;
                width = Math.min(32768, Math.max(16, (int) (width * f)));
                currStates.setWidth(width);
                nextStates.setWidth(width);
            }

        }
    }

    private static void output() {
        StringBuilder sb = new StringBuilder();
        sb.append(best.arms0.length + best.arms1.length + 1).append('\n');
        for (int i = 0; i < best.arms0.length; i++) {
            sb.append(i).append(' ').append(best.arms0[i]).append('\n');
        }
        for (int i = 0; i < best.arms1.length; i++) {
            sb.append(best.arms0.length).append(' ').append(best.arms1[i]).append('\n');
        }
        List<State> l = new ArrayList<State>();
        while (best.cmd != null) {
            l.add(best);
            best = best.parent;
        }
        sb.append(best.cy).append(' ').append(best.cx).append('\n');
        Collections.reverse(l);
        for (State s : l) {
            for (byte b : s.cmd) {
                if (b == 'W') {
                    int p = sb.length() - v * 2 - 1;
                    char c = sb.charAt(p);
                    if (c == '.') {
                        sb.setCharAt(p, 'L');
                        b = (byte) 'L';
                    } else if (c == 'L') {
                        sb.setCharAt(p, 'R');
                        b = (byte) 0;
                    } else if (c == 'R') {
                        sb.setCharAt(p, 'L');
                        b = (byte) 0;
                    }
                }
                sb.append(b == 0 ? '.' : (char) b);
            }
            sb.append('\n');
        }
        System.out.print(sb.toString());
        System.out.flush();
    }

    private static void updateBonus(State ns, State cs, int[] r0, int ox, int oy, int od, boolean deep) {
        int max = 0;
        if (cs.solved >>> 1 >= pending - cs.arms1.length && deep) {
            int[][] rots = getRots(cs.arms0.length);
            for (int[] r1 : rots) {
                int ax = ns.cx;
                int ay = ns.cy;
                int d = 0;
                for (int i = 0; i < r1.length; i++) {
                    int a = cs.arms0[i];
                    d += cs.dir[i] + r0[i] + r1[i] + 8;
                    d &= 3;
                    ax += a * rx[d];
                    ay += a * ry[d];
                }
                for (int m = -1; m < rx.length; m++) {
                    int fx = ns.cx;
                    int fy = ns.cy;
                    if (m >= 0) {
                        fx += rx[m];
                        fy += ry[m];
                        if (fx < 0 || fx >= n || fy < 0 || fy >= n) continue;
                    }
                    int mx = ax;
                    int my = ay;
                    if (m >= 0) {
                        mx += rx[m];
                        my += ry[m];
                    }
                    int curr = 0;
                    for (int i = 0; i < cs.arms1.length; i++) {
                        int a = cs.arms1[i];
                        byte look = cs.pick[i] ? (byte) 1 : (byte) 2;
                        int di = d + cs.dir[i + cs.arms0.length];
                        int add = 0;
                        for (int j = -1; j <= 2; j++) {
                            int ad = (di + j + 4) & 3;
                            int nx = mx + a * rx[ad];
                            if (nx < 0 || nx >= n) continue;
                            int ny = my + a * ry[ad];
                            if (ny < 0 || ny >= n) continue;
                            int np = ny * n + nx;
                            if (cs.grid[np] == look) {
                                int nv = value[np];
                                if (nv > add) add = nv;
                            }
                        }
                        curr += add;
                    }
                    if (curr > max) max = curr;
                }
            }
            ns.bonus = max;
            if (max > 0) return;
        }
        int target = 1;
        for (int i = 0; i < cs.arms1.length; i++) {
            if (!cs.pick[i]) {
                target = 2;
                break;
            }
        }
        int[] ord = getOrd(target, ns.cx, ns.cy);
        int nt = 0;
        for (int p : ord) {
            if (cs.grid[p] != 0) targets[nt++] = p;
        }
        ns.bonus = 1000;
        for (int i = 0; i < cs.arms1.length; i++) {
            int min = 100;
            int a = cs.arms1[i];
            if (cs.pick[i] ^ target == 1) continue;
            int di = od + cs.dir[i + cs.arms0.length];
            for (int j = -1; j <= 2; j++) {
                int ad = (di + j + 4) & 3;
                int nx = ox + a * rx[ad];
                int ny = oy + a * ry[ad];
                for (int k = 0; k < nt; k++) {
                    int t = targets[k];
                    int kd = Math.abs(ny - t / n) + Math.abs(nx - t % n);
                    if (kd < min) min = kd;
                }
            }
            ns.bonus -= min;
        }
    }

    private static int[][] getRots(int len) {
        int[][] ret = memoRots[len];
        if (ret == null) {
            List<int[]> l = new ArrayList<int[]>();
            int[] r = new int[len];
            go(0, r, l);
            ret = memoRots[len] = new int[l.size()][];
            for (int i = 0; i < l.size(); i++) {
                ret[i] = l.get(i);
            }
        }
        return ret;
    }

    private static void go(int pos, int[] r, List<int[]> l) {
        if (pos == r.length) {
            l.add(r.clone());
        } else {
            for (int i = -1; i <= 1; i++) {
                r[pos] = i;
                go(pos + 1, r, l);
            }
        }
    }

    private static int[] getOrd(int v, int x, int y) {
        int p = y * n + x;
        v--;
        int[] ret = memoOrd[v][p];
        if (ret[0] == -1) {
            Integer[] a = auxOrd[v];
            Arrays.sort(a, new Comparator<Integer>() {
                public int compare(Integer a, Integer b) {
                    return Integer.compare(d(a), d(b));
                }

                int d(int p) {
                    return Math.abs(x - p % n) + Math.abs(y - p / n);
                }
            });
            for (int i = 0; i < pending; i++) {
                ret[i] = a[i];
            }
        }
        return ret;
    }

    private static int nextInt(int v) {
        rnd ^= rnd << 13;
        rnd ^= rnd >>> 17;
        rnd ^= rnd << 15;
        return ((rnd >>> 15) * v) >>> 17;
    }

    private static void init() throws Exception {
        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
        String[] s = in.readLine().split(" ");
        n = Integer.parseInt(s[0]);
        v = Integer.parseInt(s[2]);
        nn = n * n;
        grid = new byte[nn];
        for (byte rep = 1; rep <= 2; rep++) {
            for (int y = 0; y < n; y++) {
                int yn = y * n;
                String a = in.readLine();
                for (int x = 0; x < n; x++) {
                    int p = yn + x;
                    if (a.charAt(x) == '1') {
                        byte g = grid[p] |= rep;
                        if (rep == 2) {
                            if (g == 3) grid[p] = 0;
                            else pending++;
                        }
                    }
                }
            }
        }
        in.close();
        value = new int[nn];
        for (int y = 0; y < n; y++) {
            int yn = y * n;
            int yy = (y << 1) - n;
            yy *= yy;
            for (int x = 0; x < n; x++) {
                int p = yn + x;
                int xx = (x << 1) - n;
                xx *= xx;
                value[p] = xx + yy + nn + y;
            }
        }
        memoOrd[0] = new int[nn][pending];
        memoOrd[1] = new int[nn][pending];
        auxOrd = new Integer[2][pending];
        int[] c = new int[2];
        for (int i = 0; i < nn; i++) {
            int v = grid[i] - 1;
            if (v >= 0) auxOrd[v][c[v]++] = i;
            memoOrd[0][i][0] = memoOrd[1][i][0] = -1;
        }
        int yMin = 0;
        OUT: for (int y = 0; y < n; y++) {
            int yn = y * n;
            for (int x = 0; x < n; x++) {
                if (grid[yn + x] != 0) break OUT;
            }
            yMin++;
        }
        int yMax = n - 1;
        OUT: for (int y = n - 1; y >= yMin; y--) {
            int yn = y * n;
            for (int x = 0; x < n; x++) {
                if (grid[yn + x] != 0) break OUT;
            }
            yMax--;
        }
        int xMin = 0;
        OUT: for (int x = 0; x < n; x++) {
            for (int y = 0; y < n; y++) {
                if (grid[y * n + x] != 0) break OUT;
            }
            xMin++;
        }
        int xMax = n - 1;
        OUT: for (int x = n - 1; x >= xMin; x--) {
            for (int y = 0; y < n; y++) {
                if (grid[y * n + x] != 0) break OUT;
            }
            xMax--;
        }
        en = n;
        if (yMax - yMin < n - 1 && xMax - xMin < n - 1) {
            en = Math.max(15, Math.max(xMax - xMin, yMax - yMin) + 1);
        }
        targets = new int[pending];
    }

    private static void shuffle(int[] a) {
        for (int i = a.length - 1; i > 0; i--) {
            int index = nextInt(i + 1);
            int temp = a[index];
            a[index] = a[i];
            a[i] = temp;
        }
    }

    private static class Beam {
        private int beamWidth;
        private State[] items;
        private int size;

        Beam(int beamWidth) {
            this.beamWidth = beamWidth;
            items = new State[32768];
        }

        boolean add(State item) {
            if (size >= beamWidth && (items[beamWidth - 1]).compareTo(item) <= 0) return false;
            int pos = Arrays.binarySearch(items, 0, size, item);
            if (pos < 0) pos = -pos - 1;
            else return false;
            if (pos >= beamWidth) return false;
            if (size < beamWidth) size++;
            int len = size - 1 - pos;
            if (len > 4) {
                System.arraycopy(items, pos, auxItems, 0, len);
                System.arraycopy(auxItems, 0, items, pos + 1, len);
            } else {
                for (int i = size - 1; i > pos; i--) {
                    items[i] = items[i - 1];
                }
            }
            items[pos] = item;
            return true;
        }

        State last() {
            return size == 0 ? null : items[size - 1];
        }

        State first() {
            return size == 0 ? null : items[0];
        }

        State get(int idx) {
            return items[idx];
        }

        int size() {
            return size;
        }

        void clear() {
            size = 0;
        }

        void setWidth(int width) {
            beamWidth = width;
            if (size > width) size = width;
            if (items.length < width) items = Arrays.copyOf(items, width);
        }
    }

    private static class State implements Comparable<State> {
        byte[] grid;
        boolean[] pick;
        int[] dir, arms0, arms1;
        int solved, turns, score, cx, cy, bonus;
        State parent;
        byte[] cmd;

        public int compareTo(State o) {
            int cmp = 0;
            if ((cmp = Integer.compare(o.score, score)) != 0) return cmp;
            if ((cmp = Integer.compare(o.solved, solved)) != 0) return cmp;
            if ((cmp = Integer.compare(o.bonus, bonus)) != 0) return cmp;
            if ((cmp = Integer.compare(value[cx + cy * n], value[o.cx + o.cy * n])) != 0) return cmp;
            if ((cmp = Integer.compare(o.cx, cx)) != 0) return cmp;
            if ((cmp = Arrays.compare(o.arms1, arms1)) != 0) return cmp;
            return 0;
        }

        public boolean equals(Object o) {
            return compareTo((State) o) == 0;
        }
    }

    private static int[] config() {
        if (v == 5 && en == 15) return new int[] {6};
        if (v == 5 && en == 16) return new int[] {3,5};
        if (v == 5 && en == 17) return new int[] {3,6};
        if (v == 5 && en == 18) return new int[] {4,6};
        if (v == 5 && en == 19) return new int[] {4,6};
        if (v == 5 && en == 20) return new int[] {4,6};
        if (v == 5 && en == 21) return new int[] {5,8};
        if (v == 5 && en == 22) return new int[] {5,8};
        if (v == 5 && en == 23) return new int[] {5,8};
        if (v == 5 && en == 24) return new int[] {5,8};
        if (v == 5 && en == 25) return new int[] {5,8};
        if (v == 5 && en == 26) return new int[] {6,9};
        if (v == 5 && en == 27) return new int[] {6,9};
        if (v == 5 && en == 28) return new int[] {6,9};
        if (v == 5 && en == 29) return new int[] {7,11};
        if (v == 5 && en == 30) return new int[] {7,11};
        if (v == 6 && en == 15) return new int[] {3,5};
        if (v == 6 && en == 16) return new int[] {3,6};
        if (v == 6 && en == 17) return new int[] {4,6};
        if (v == 6 && en == 18) return new int[] {4,6};
        if (v == 6 && en == 19) return new int[] {4,7};
        if (v == 6 && en == 20) return new int[] {4,8};
        if (v == 6 && en == 21) return new int[] {4,8};
        if (v == 6 && en == 22) return new int[] {5,9};
        if (v == 6 && en == 23) return new int[] {5,9};
        if (v == 6 && en == 24) return new int[] {5,10};
        if (v == 6 && en == 25) return new int[] {5,10};
        if (v == 6 && en == 26) return new int[] {6,11};
        if (v == 6 && en == 27) return new int[] {6,11};
        if (v == 6 && en == 28) return new int[] {6,11};
        if (v == 6 && en == 29) return new int[] {6,11};
        if (v == 6 && en == 30) return new int[] {6,11};
        if (v == 7 && en == 15) return new int[] {3,5};
        if (v == 7 && en == 16) return new int[] {3,5};
        if (v == 7 && en == 17) return new int[] {4,6};
        if (v == 7 && en == 18) return new int[] {4,6};
        if (v == 7 && en == 19) return new int[] {4,7};
        if (v == 7 && en == 20) return new int[] {4,7};
        if (v == 7 && en == 21) return new int[] {5,8};
        if (v == 7 && en == 22) return new int[] {5,8};
        if (v == 7 && en == 23) return new int[] {5,10};
        if (v == 7 && en == 24) return new int[] {5,10};
        if (v == 7 && en == 25) return new int[] {5,10};
        if (v == 7 && en == 26) return new int[] {5,11};
        if (v == 7 && en == 27) return new int[] {6,11};
        if (v == 7 && en == 28) return new int[] {6,11};
        if (v == 7 && en == 29) return new int[] {6,12};
        if (v == 7 && en == 30) return new int[] {6,12};
        if (v == 8 && en == 15) return new int[] {2,5};
        if (v == 8 && en == 16) return new int[] {2,6};
        if (v == 8 && en == 17) return new int[] {3,6};
        if (v == 8 && en == 18) return new int[] {3,6};
        if (v == 8 && en == 19) return new int[] {4,7};
        if (v == 8 && en == 20) return new int[] {4,7};
        if (v == 8 && en == 21) return new int[] {4,8};
        if (v == 8 && en == 22) return new int[] {5,7};
        if (v == 8 && en == 23) return new int[] {5,9};
        if (v == 8 && en == 24) return new int[] {6,10};
        if (v == 8 && en == 25) return new int[] {6,10};
        if (v == 8 && en == 26) return new int[] {6,9};
        if (v == 8 && en == 27) return new int[] {6,11};
        if (v == 8 && en == 28) return new int[] {6,12};
        if (v == 8 && en == 29) return new int[] {6,12};
        if (v == 8 && en == 30) return new int[] {7,12};
        if (v == 9 && en == 15) return new int[] {2,5};
        if (v == 9 && en == 16) return new int[] {2,5};
        if (v == 9 && en == 17) return new int[] {2,6};
        if (v == 9 && en == 18) return new int[] {2,3,7};
        if (v == 9 && en == 19) return new int[] {3,7};
        if (v == 9 && en == 20) return new int[] {2,4,7};
        if (v == 9 && en == 21) return new int[] {2,4,7};
        if (v == 9 && en == 22) return new int[] {3,5,7};
        if (v == 9 && en == 23) return new int[] {3,5,7};
        if (v == 9 && en == 24) return new int[] {3,5,7};
        if (v == 9 && en == 25) return new int[] {4,6,8};
        if (v == 9 && en == 26) return new int[] {4,6,8};
        if (v == 9 && en == 27) return new int[] {4,6,8};
        if (v == 9 && en == 28) return new int[] {4,7,9};
        if (v == 9 && en == 29) return new int[] {5,7,9};
        if (v == 9 && en == 30) return new int[] {4,7,9};
        if (v == 10 && en == 15) return new int[] {2,3,4};
        if (v == 10 && en == 16) return new int[] {2,3,5};
        if (v == 10 && en == 17) return new int[] {2,3,5};
        if (v == 10 && en == 18) return new int[] {2,3,6};
        if (v == 10 && en == 19) return new int[] {2,4,5};
        if (v == 10 && en == 20) return new int[] {2,3,7};
        if (v == 10 && en == 21) return new int[] {2,4,7};
        if (v == 10 && en == 22) return new int[] {2,4,7};
        if (v == 10 && en == 23) return new int[] {3,5,7};
        if (v == 10 && en == 24) return new int[] {3,5,7};
        if (v == 10 && en == 25) return new int[] {3,5,7};
        if (v == 10 && en == 26) return new int[] {3,6,8};
        if (v == 10 && en == 27) return new int[] {4,6,8};
        if (v == 10 && en == 28) return new int[] {4,7,9};
        if (v == 10 && en == 29) return new int[] {4,7,9};
        if (v == 10 && en == 30) return new int[] {5,7,9};
        if (v == 11 && en == 15) return new int[] {2,3,4};
        if (v == 11 && en == 16) return new int[] {2,3,4};
        if (v == 11 && en == 17) return new int[] {2,3,6};
        if (v == 11 && en == 18) return new int[] {2,3,6};
        if (v == 11 && en == 19) return new int[] {2,3,6};
        if (v == 11 && en == 20) return new int[] {2,3,7};
        if (v == 11 && en == 21) return new int[] {2,3,7};
        if (v == 11 && en == 22) return new int[] {2,3,7};
        if (v == 11 && en == 23) return new int[] {2,4,8};
        if (v == 11 && en == 24) return new int[] {3,5,7};
        if (v == 11 && en == 25) return new int[] {3,5,7};
        if (v == 11 && en == 26) return new int[] {3,6,8};
        if (v == 11 && en == 27) return new int[] {3,5,10};
        if (v == 11 && en == 28) return new int[] {4,6,8};
        if (v == 11 && en == 29) return new int[] {3,6,10};
        if (v == 11 && en == 30) return new int[] {4,7,9};
        if (v == 12 && en == 15) return new int[] {2,3,4};
        if (v == 12 && en == 16) return new int[] {2,3,4};
        if (v == 12 && en == 17) return new int[] {2,3,6};
        if (v == 12 && en == 18) return new int[] {2,3,6};
        if (v == 12 && en == 19) return new int[] {2,3,7};
        if (v == 12 && en == 20) return new int[] {2,3,7};
        if (v == 12 && en == 21) return new int[] {2,3,7};
        if (v == 12 && en == 22) return new int[] {2,4,7};
        if (v == 12 && en == 23) return new int[] {2,4,8};
        if (v == 12 && en == 24) return new int[] {2,4,9};
        if (v == 12 && en == 25) return new int[] {2,4,9};
        if (v == 12 && en == 26) return new int[] {2,4,9};
        if (v == 12 && en == 27) return new int[] {4,6,8};
        if (v == 12 && en == 28) return new int[] {3,5,9};
        if (v == 12 && en == 29) return new int[] {3,5,10};
        if (v == 12 && en == 30) return new int[] {4,7,9};
        if (v == 13 && en == 15) return new int[] {2,3,4};
        if (v == 13 && en == 16) return new int[] {2,4,5};
        if (v == 13 && en == 17) return new int[] {2,4,5};
        if (v == 13 && en == 18) return new int[] {2,3,6};
        if (v == 13 && en == 19) return new int[] {2,3,6};
        if (v == 13 && en == 20) return new int[] {2,3,7};
        if (v == 13 && en == 21) return new int[] {2,3,7};
        if (v == 13 && en == 22) return new int[] {2,3,7};
        if (v == 13 && en == 23) return new int[] {2,4,7};
        if (v == 13 && en == 24) return new int[] {2,3,8};
        if (v == 13 && en == 25) return new int[] {2,4,8};
        if (v == 13 && en == 26) return new int[] {2,4,9};
        if (v == 13 && en == 27) return new int[] {3,4,9};
        if (v == 13 && en == 28) return new int[] {2,5,10};
        if (v == 13 && en == 29) return new int[] {3,5,10};
        if (v == 13 && en == 30) return new int[] {3,5,10};
        if (v == 14 && en == 15) return new int[] {2,3,4};
        if (v == 14 && en == 16) return new int[] {2,4,5};
        if (v == 14 && en == 17) return new int[] {2,4,5};
        if (v == 14 && en == 18) return new int[] {2,3,4,7};
        if (v == 14 && en == 19) return new int[] {2,3,6};
        if (v == 14 && en == 20) return new int[] {2,3,4,8};
        if (v == 14 && en == 21) return new int[] {2,4,5,8};
        if (v == 14 && en == 22) return new int[] {2,3,4,8};
        if (v == 14 && en == 23) return new int[] {2,3,7};
        if (v == 14 && en == 24) return new int[] {2,4,8};
        if (v == 14 && en == 25) return new int[] {2,3,4,8};
        if (v == 14 && en == 26) return new int[] {2,4,5,8};
        if (v == 14 && en == 27) return new int[] {2,4,9};
        if (v == 14 && en == 28) return new int[] {2,4,5,8};
        if (v == 14 && en == 29) return new int[] {3,5,10};
        if (v == 14 && en == 30) return new int[] {2,4,5,8};
        if (v == 15 && en == 15) return new int[] {2,3,4,8};
        if (v == 15 && en == 16) return new int[] {2,3,4,6};
        if (v == 15 && en == 17) return new int[] {2,3,4,5};
        if (v == 15 && en == 18) return new int[] {2,4,5,8};
        if (v == 15 && en == 19) return new int[] {2,3,4,6};
        if (v == 15 && en == 20) return new int[] {2,4,5,8};
        if (v == 15 && en == 21) return new int[] {2,3,4,6};
        if (v == 15 && en == 22) return new int[] {2,3,4,8};
        if (v == 15 && en == 23) return new int[] {2,3,4,8};
        if (v == 15 && en == 24) return new int[] {2,4,5,8};
        if (v == 15 && en == 25) return new int[] {2,3,4,8};
        if (v == 15 && en == 26) return new int[] {2,4,5,8};
        if (v == 15 && en == 27) return new int[] {2,4,5,8};
        if (v == 15 && en == 28) return new int[] {2,4,5,8};
        if (v == 15 && en == 29) return new int[] {2,4,5,10};
        if (v == 15 && en == 30) return new int[] {2,4,5,8};
        return null;
    }
}
