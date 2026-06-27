import java.io.*;
import java.util.*;

public class Main {
    private static BufferedReader in;
    private static int numPets, numHumans, turn;
    private static int[] petsPos, humansPos;
    private static int[] petsType;
    private static int[][] petsDist;
    private static int[][] humansDist;
    private static int[] dd;
    private static boolean finished;
    private static boolean[] petsCaptured;
    private static boolean[] busy;
    private static boolean[] blocked;
    private static boolean[] fill;
    private static char[] moves;
    private static int[][] targetsVal, trapsVal;
    private static int[][] petsHistory;
    private static int[] petsHistoryCnt;
    private static Target[] targets;
    private static Trap[] traps;
    private static final int mem = 1024;
    private static final int inf = 999999;
    private static final int[] queue = new int[mem];
    private static final SplittableRandom rnd = new SplittableRandom(inf);
    private static int[][] initialPaths;
    private static int[] initialPathsSize;
    private static int[] initialPathsPos;
    private static final double[] log = new double[1 << 16];
    private static final int[] dirs = new int[] {-32,32,-1,1};
    private static final int lookAhead = 25;
    private static final int[][] petsProb = new int[lookAhead * 3 + 1][mem];
    private static final int[][] petsProbTot = new int[lookAhead + 1][mem];
    private static final int[][] optPos = new int[mem][128];
    private static final int[] optCnt = new int[mem];
    private static final boolean[] connected = new boolean[mem];
    private static final int mBlocked = -1;
    private static final int mReach = 1;
    private static final int mNeib = 2;
    private static final int mPet = 3;
    private static final int mDest = -2;

    public static void main(String[] args) {
        init();
        run();
        try {
            in.close();
        } catch (Exception e) {
        }
    }

    private static void run() {
        try {
            for (turn = 0; turn < 300; turn++) {
                Arrays.fill(busy, false);
                for (int i = 0; i < numHumans; i++) {
                    busy[humansPos[i]] = true;
                }
                for (int i = 0; i < numPets; i++) {
                    int p = petsPos[i];
                    busy[p] = busy[p - 32] = busy[p + 32] = busy[p - 1] = busy[p + 1] = true;
                }
                Arrays.fill(moves, '.');
                move();
                System.out.println(new String(moves));
                System.out.flush();
                String[] petsMoves = in.readLine().split(" ");
                for (int i = 0; i < numPets; i++) {
                    String m = petsMoves[i];
                    int t = petsType[i];
                    for (int j = 0; j < m.length(); j++) {
                        char c = m.charAt(j);
                        int mv = 0;
                        if (c == 'U') mv = -32;
                        else if (c == 'D') mv = 32;
                        else if (c == 'L') mv = -1;
                        else if (c == 'R') mv = 1;
                        else System.err.println("ERR>>>" + c);
                        petsPos[i] += mv;
                        if (!petsCaptured[i] && (t == 4 || t == 5)) petsHistory[i][petsHistoryCnt[i]++] = mv;
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static void initTraps() {
        traps = new Trap[128];
        int nt = 0;
        boolean[] used = new boolean[mem];
        int xAvg = 0;
        int tot = 0;
        for (int i = 0; i < numPets; i++) {
            int t = petsType[i];
            if (t <= 3) {
                int w = 4 - t;
                tot += w;
                xAvg += x(petsPos[i]) * w;
            }
        }
        int[] xx = new int[] {2,5,9,12,16,19,23,26};
        if (numPets == 20 && numHumans == 5) xx = new int[] {2,13,17,27};
        else if (numPets >= 17 && numHumans == 5) xx = new int[] {3,8,12,17,21,26};
        else if (numPets >= 15 && numHumans == 5) xx = new int[] {2,5,9,13,17,20,24,27};
        else if (numPets >= 18 && numHumans == 6) xx = new int[] {2,5,9,13,17,20,24,27};
        if (xAvg > 15 * tot) {
            for (int i = 0; i < xx.length; i++) {
                xx[i] = 29 - xx[i];
            }
            Arrays.sort(xx);
        }
        for (int y = 1; y <= 29; y += 2) {
            for (int k = 0; k < xx.length; k += 2) {
                int x0 = xx[k] + 1;
                int x1 = xx[k + 1] + 1;
                Trap t = traps[nt++] = new Trap();
                t.inside = new int[64];
                t.capture = new int[64];
                t.walls = new int[64];
                int i = 0;
                int w = 0;
                int c = 0;
                for (int x = x0; x <= x1; x++) {
                    used[t.inside[i++] = pos(x, y)] = true;
                    if (x > x0 && x < x1) t.capture[c++] = pos(x, y);
                    if (x > x0 && x < x1 && y == 29) {
                        used[t.inside[i++] = pos(x, y + 1)] = true;
                        t.capture[c++] = pos(x, y + 1);
                    } else {
                        if (!blocked[pos(x, y - 1)]) used[t.walls[w++] = pos(x, y - 1)] = true;
                        if (!blocked[pos(x, y + 1)]) used[t.walls[w++] = pos(x, y + 1)] = true;
                    }
                }
                t.walls = Arrays.copyOf(t.walls, w);
                t.capture = Arrays.copyOf(t.capture, c);
                t.inside = Arrays.copyOf(t.inside, i);
                t.doors = new int[2];
                used[t.doors[0] = pos(x0 - 1, y)] = true;
                used[t.doors[1] = pos(x1 + 1, y)] = true;
            }
        }
        traps = Arrays.copyOf(traps, nt);
        targets = new Target[16 * traps.length];
        nt = 0;
        for (int i = 0; i < traps.length; i++) {
            Trap t = traps[i];
            int p0 = t.doors[0];
            for (int j0 : dirs) {
                int np0 = p0 + j0;
                if (!used[np0]) {
                    int p1 = t.doors[1];
                    for (int j1 : dirs) {
                        int np1 = p1 + j1;
                        if (!used[np1]) {
                            int[] pos = new int[] {np0,np1};
                            Arrays.sort(pos);
                            boolean found = false;
                            for (int j = 0; j < nt; j++) {
                                Target o = targets[j];
                                if (Arrays.equals(pos, o.pos)) {
                                    o.traps = Arrays.copyOf(o.traps, o.traps.length + 1);
                                    o.traps[o.traps.length - 1] = t;
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                Target g = targets[nt++] = new Target();
                                g.pos = pos;
                                g.traps = new Trap[] {t};
                            }
                        }
                    }
                }
            }
        }
        targets = Arrays.copyOf(targets, nt);
        for (int i = 0; i < targets.length; i++) {
            targets[i].idx = i;
        }
        for (int i = 0; i < traps.length; i++) {
            traps[i].idx = i;
        }
        targetsVal = new int[targets.length][lookAhead + 1];
        trapsVal = new int[traps.length][lookAhead + 1];
    }

    private static void initialPaths() {
        initTraps();
        for (int i = 0; i < numHumans; i++) {
            initialPaths[i][initialPathsSize[i]++] = humansPos[i];
        }
        Set<Integer> walls = new HashSet<Integer>();
        fill = new boolean[mem];
        for (Trap t : traps) {
            for (int a : t.walls) {
                walls.add(a);
                fill[a] = true;
            }
        }
        List<Integer> pending = new ArrayList<Integer>();
        for (int pass = 2; pass >= 1; pass--) {
            for (int x = 1; x <= 30; x++) {
                for (int y = 1; y <= 30; y++) {
                    int p = pos(x, y);
                    if (fill[p]) continue;
                    int c = 0;
                    if (!blocked[p + 1] && walls.contains(p + 1)) c++;
                    if (!blocked[p - 1] && walls.contains(p - 1)) c++;
                    if (c >= pass) {
                        pending.add(p);
                        walls.remove(p + 1);
                        walls.remove(p - 1);
                    }
                }
            }
        }
        int idx = 0;
        while (!pending.isEmpty()) {
            int[] l = initialPaths[idx];
            int min = inf;
            int p = -1;
            int last = l[initialPathsSize[idx] - 1];
            for (int i = 0; i < pending.size(); i++) {
                int d = dist(last, pending.get(i));
                if (d < min) {
                    min = d;
                    p = i;
                }
            }
            l[initialPathsSize[idx]++] = pending.get(p);
            if (p == pending.size() - 1) pending.remove(pending.size() - 1);
            else pending.set(p, pending.remove(pending.size() - 1));
            if (++idx == numHumans) idx = 0;
        }
        int[] len = new int[numHumans];
        for (int i = 0; i < numHumans; i++) {
            len[i] = len(initialPaths[i], initialPathsSize[i]);
        }
        int[] aux = new int[512];
        double t0 = 1;
        double temp = t0;
        int numSteps = 4000000;
        for (int step = 0; step < numSteps; step++) {
            if ((step & 63) == 0) temp = t0 * (1 - step / (double) numSteps);
            int idx1 = rnd.nextInt(numHumans);
            int[] p1 = initialPaths[idx1];
            int size1 = initialPathsSize[idx1];
            if (size1 <= 1) continue;
            int a1 = rnd.nextInt(1, size1);
            if ((step & 1) == 1 && dist(p1[a1], p1[a1 - 1]) > 1) {
                int idx2 = rnd.nextInt(numHumans);
                if (idx1 == idx2) continue;
                int[] p2 = initialPaths[idx2];
                int size2 = initialPathsSize[idx2];
                int a2 = rnd.nextInt(1, size2 + 1);
                int len1 = len[idx1];
                int len2 = len[idx2];
                int prev1 = p1[a1 - 1];
                int next1 = p1[a1];
                int prev2 = p2[a2 - 1];
                int next2 = a2 == size2 ? -1 : p2[a2];
                int delta = ((size2 - a2) - (size1 - a1)) * 3;
                int n = next1;
                for (int i = a1 + 1; i < size1; i++) {
                    int v1 = p1[i];
                    delta -= dist(n, v1);
                    n = v1;
                }
                n = next2;
                for (int i = a2 + 1; i < size2; i++) {
                    int v2 = p2[i];
                    delta += dist(n, v2);
                    n = v2;
                }
                int n1 = len1 - dist(prev1, next1) + dist(prev1, next2) + delta;
                int n2 = len2 - dist(prev2, next2) + dist(prev2, next1) - delta;
                int gain = 9 * (len1 + len2 - (n1 + n2)) + (Math.max(len1, len2) - Math.max(n1, n2));
                if (gain >= 0 || gain > log[rnd.nextInt(log.length)] * temp * 10) {
                    System.arraycopy(p1, a1, aux, 0, size1 - a1);
                    System.arraycopy(p2, a2, p1, a1, size2 - a2);
                    System.arraycopy(aux, 0, p2, a2, size1 - a1);
                    initialPathsSize[idx1] = a1 + size2 - a2;
                    initialPathsSize[idx2] = a2 + size1 - a1;
                    len[idx1] = n1;
                    len[idx2] = n2;
                }
            } else {
                if (size1 <= 2) continue;
                int a2 = rnd.nextInt(1, size1);
                if (a1 == a2) continue;
                if (a1 > a2) {
                    int aa = a1;
                    a1 = a2;
                    a2 = aa;
                }
                int prev = p1[a1 - 1];
                int next = a2 == size1 - 1 ? -1 : p1[a2 + 1];
                int n1 = p1[a1];
                int n2 = p1[a2];
                int gain = dist(prev, n1) + dist(n2, next) - (dist(prev, n2) + dist(n1, next));
                if (gain >= 0 || gain > log[rnd.nextInt(log.length)] * temp) {
                    for (; a1 < a2; a1++, a2--) {
                        int aa = p1[a2];
                        p1[a2] = p1[a1];
                        p1[a1] = aa;
                    }
                    len[idx1] -= gain;
                }
            }
        }
        Arrays.fill(initialPathsPos, 1);
    }

    private static int len(int[] l, int size) {
        int d = 0;
        int pa = l[0];
        for (int j = 1; j < size; j++) {
            int pb = l[j];
            d += dist(pa, pb);
            pa = pb;
        }
        return d + 3 * (size - 1);
    }

    private static int dist(int a, int b) {
        if (a == -1 || b == -1) return 0;
        return Math.abs(x(a) - x(b)) + Math.abs(y(a) - y(b));
    }

    private static int updateMap(int[] map, int[] dest, int start) {
        map[start] = mReach;
        for (int d : dirs) {
            int n = d + start;
            if (map[n] == 0) map[n] = mNeib;
        }
        int nd = 0;
        for (int i = 0; i < mem; i++) {
            int m = map[i];
            if (m == mReach || m == mNeib || m == mPet) {
                for (int d : dirs) {
                    int n = d + i;
                    if (map[n] == 0) {
                        map[n] = mDest;
                        dest[nd++] = n;
                    }
                }
            }
        }
        int limit = 32;
        int fill = fill(start, map, limit);
        for (int i = 0; i < nd; i++) {
            int d = dest[i];
            map[d] = 0;
            fill = fill(start, map, limit);
            if (fill < limit) dest[i--] = dest[--nd];
            else map[d] = mDest;
        }
        return nd;
    }

    private static void assign(int[] map, int[] assign, int[] dest, int nd) {
        for (int i = 0; i < numHumans; i++) {
            assign[i] = i % nd;
        }
        double t0 = 1;
        double temp = t0;
        int numSteps = 10000;
        for (int step = 0; step < numSteps; step++) {
            if ((step & 63) == 0) temp = t0 * (1 - step / (double) numSteps);
            int a = rnd.nextInt(numHumans);
            int b = rnd.nextInt(numHumans);
            if (a == b) continue;
            int da = dest[assign[a]];
            int db = dest[assign[b]];
            int cda = inf;
            int cdb = inf;
            int nda = inf;
            int ndb = inf;
            for (int d : dirs) {
                int p = da + d;
                if (map[p] == 0) {
                    cda = Math.min(cda, humansDist[a][p]);
                    ndb = Math.min(ndb, humansDist[b][p]);
                }
                p = db + d;
                if (map[p] == 0) {
                    cdb = Math.min(cdb, humansDist[b][p]);
                    nda = Math.min(nda, humansDist[a][p]);
                }
            }
            int gain = cda + cdb - nda - ndb;
            if (gain >= 0 || gain > log[rnd.nextInt(log.length)] * temp) {
                int aux = assign[a];
                assign[a] = assign[b];
                assign[b] = aux;
            }
        }
    }

    private static boolean lock() {
        int pet = -1;
        for (int k = 0; k < numPets; k++) {
            if (petsCaptured[k]) continue;
            pet = petsPos[k];
            break;
        }
        int xp = x(pet);
        int yp = y(pet);
        int[][] opt = new int[numHumans][];
        for (int i = 0; i < numHumans; i++) {
            int h = humansPos[i];
            int[] a = new int[2];
            int n = 0;
            int xh = x(h);
            int yh = y(h);
            for (int d : dirs) {
                int p = h + d;
                if (blocked[p] || busy[p]) continue;
                if (Math.abs(x(p) - xp) < Math.abs(xh - xp) || Math.abs(y(p) - yp) < Math.abs(yh - yp)) {
                    a[n++] = p;
                }
            }
            if (n != 2) a = Arrays.copyOf(a, n);
            opt[i] = a;
        }
        int all = 1 << numHumans;
        int[] undo = new int[numHumans];
        int min = 16;
        int mask = -1;
        for (int i = 0; i < all; i++) {
            int n = 0;
            for (int j = 0; j < numHumans; j++) {
                int[] a = opt[j];
                boolean up = (i & (1 << j)) != 0;
                if (up && a.length != 2) continue;
                if (a.length == 0) continue;
                int p = a[up ? 1 : 0];
                if (!blocked[p]) {
                    blocked[p] = true;
                    undo[n++] = p;
                }
            }
            int ff = ff(pet, min);
            for (int j = 0; j < numHumans; j++) {
                int c = ff(humansPos[j], 32);
                if (c < 32) {
                    ff = inf;
                    break;
                }
            }
            if (ff < min) {
                min = ff;
                mask = i;
                System.err.println("FOUND=>" + min);
            }
            for (int j = 0; j < n; j++) {
                blocked[undo[j]] = false;
            }
        }
        if (mask != -1) {
            for (int j = 0; j < numHumans; j++) {
                int[] a = opt[j];
                boolean up = (mask & (1 << j)) != 0;
                if (up && a.length != 2) continue;
                if (a.length == 0) continue;
                int p = a[up ? 1 : 0];
                if (!blocked[p]) {
                    moves[j] = moveChar(humansPos[j], p, true);
                    blocked[p] = true;
                }
            }
            return true;
        }
        return false;
    }

    private static void hunt() {
        if (lock()) return;
        int[] dest = new int[128];
        int[] assign = new int[numHumans];
        int[] map = new int[mem];
        for (int k = 0; k < numPets; k++) {
            if (petsCaptured[k]) continue;
            int pp = petsPos[k];
            for (int i = 0; i < numHumans; i++) {
                bfs(humansPos[i], humansDist[i], -1);
            }
            Arrays.fill(map, 0);
            for (int i = 0; i < mem; i++) {
                if (blocked[i]) map[i] = mBlocked;
            }
            int nd = updateMap(map, dest, pp);
            int limit = 16;
            boolean bl = false;
            if (fill(pp, map, limit) < limit) {
                boolean ok = true;
                for (int i = 0; i < numHumans; i++) {
                    if (connected[humansPos[i]]) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
                    for (int i = 0; i < numHumans; i++) {
                        if (fill(humansPos[i], map, limit) < limit) {
                            ok = false;
                            break;
                        }
                    }
                }
                if (ok) {
                    int[] mv = new int[numHumans];
                    Arrays.fill(mv, -1);
                    assign(map, assign, dest, nd);
                    int nf = 0;
                    for (int i = 0; i < nd; i++) {
                        int d = dest[i];
                        if (busy[d]) continue;
                        boolean found = false;
                        for (int j = 0; j < numHumans; j++) {
                            if (assign[j] == i) {
                                if (humansDist[j][d] == 1) {
                                    mv[j] = d;
                                    found = true;
                                    break;
                                }
                            }
                        }
                        if (found) nf++;
                    }
                    if (nf == nd || nf >= 1) {
                        for (int i = 0; i < numHumans; i++) {
                            int np = mv[i];
                            if (np == -1) continue;
                            if (!blocked[np]) {
                                moves[i] = moveChar(humansPos[i], np, true);
                                blocked[np] = true;
                                bl = true;
                            }
                        }
                        return;
                    }
                }
            }
            if (bl) break;
        }
        updatePetsProb(3);
        Arrays.fill(map, 0);
        int[] pt1 = petsProbTot[1];
        int[] pt2 = petsProbTot[2];
        int[] pt3 = petsProbTot[3];
        int[] opt = new int[100];
        int no = 0;
        int max = 1;
        for (int i = 0; i < mem; i++) {
            if (blocked[i]) {
                map[i] = mBlocked;
                continue;
            }
            int v = pt1[i] + pt2[i] / 4 + pt3[i] / 16;
            if (v == 0) continue;
            int[] d = new int[numHumans];
            for (int j = 0; j < numHumans; j++) {
                d[j] = humansDist[j][i];
            }
            Arrays.sort(d);
            v /= d[d.length / 2] + 1;
            if (v > max) {
                max = v;
                no = 0;
            }
            if (v == max) {
                opt[no++] = i;
            }
        }
        int guess = opt[rnd.nextInt(no)];
        int nd = updateMap(map, dest, guess);
        assign(map, assign, dest, nd);
        for (int i = 0; i < numHumans; i++) {
            if (moves[i] != '.') continue;
            int di = dest[assign[i]];
            int np = -1;
            int min = inf;
            for (int d : dirs) {
                int p = di + d;
                if (map[p] == 0) {
                    int curr = humansDist[i][p];
                    if (curr < min) {
                        min = curr;
                        np = p;
                    }
                }
            }
            if (np == -1) continue;
            int hp = humansPos[i];
            if (hp != np) {
                bfs(np, dd, hp);
                int best = -1;
                int dx = Math.abs(x(hp) - x(np));
                int dy = Math.abs(y(hp) - y(np));
                for (int d : dirs) {
                    int to = hp + d;
                    if (dd[to] < dd[hp]) {
                        if (best == -1 || (dx > dy && Math.abs(d) == 32) || (dx <= dy && Math.abs(d) == 1)) best = to;
                    }
                }
                if (best != -1) {
                    moves[i] = moveChar(hp, best, false);
                    humansPos[i] = best;
                    busy[best] = true;
                }
            } else if (!busy[di]) {
                blocked[di] = true;
                int size = ff(humansPos[i], 100);
                if (size < 100) blocked[di] = false;
                else moves[i] = moveChar(humansPos[i], di, true);
            }
        }
    }

    private static void move() {
        if (turn == 0) initialPaths();
        if (finished) return;
        boolean[] usedHumans = new boolean[numHumans];
        boolean done = true;
        for (int i = 0; i < numHumans; i++) {
            int pos = initialPathsPos[i];
            int size = initialPathsSize[i];
            if (pos >= size) {
                done = false;
                continue;
            }
            int[] path = initialPaths[i];
            int p0 = humansPos[i];
            int p1 = path[pos];
            if (blocked[p1]) {
                initialPathsPos[i]++;
                i--;
                continue;
            }
            int p2 = pos == size - 1 ? -1 : path[pos + 1];
            usedHumans[i] = true;
            int x = x(p0);
            int y = y(p0);
            if (p0 == p1) {
                boolean bl = false;
                for (int j = 0; j < 2; j++) {
                    int np = j == 0 ? p0 - 1 : p0 + 1;
                    if (fill[np]) {
                        int x2 = x(p2);
                        if (x == x2) {
                            if (Integer.compare(y(p2), y) == Integer.compare(y(np), y)) {
                                int p3 = pos(x, np < p0 ? y - 2 : y + 2);
                                if (p3 != p2) {
                                    for (int k = size; k > pos + 1; k--) {
                                        path[k] = path[k - 1];
                                    }
                                    path[pos + 1] = p3;
                                    size = ++initialPathsSize[i];
                                }
                                continue;
                            }
                        }
                        bl = true;
                        if (!busy[np]) {
                            moves[i] = moveChar(p0, np, true);
                            blocked[np] = true;
                            fill[np] = false;
                            updateOptNeighbors(np);
                            break;
                        }
                    }
                }
                if (!bl) {
                    initialPathsPos[i]++;
                    i--;
                }
            } else {
                bfs(p1, dd, p0);
                for (int j : dirs) {
                    int np = p0 + j;
                    if (dd[np] < dd[p0]) {
                        moves[i] = moveChar(p0, np, false);
                        humansPos[i] = np;
                        busy[np] = true;
                        break;
                    }
                }
            }
        }
        if (done) return;
        int pendingPets = 0;
        for (int i = 0; i < numPets; i++) {
            if (petsCaptured[i]) continue;
            if (bfs(petsPos[i], petsDist[i], -1)) petsCaptured[i] = true;
            else pendingPets++;
        }
        finished = pendingPets == 0;
        if (finished) return;
        if (pendingPets == 1 && turn > 270) {
            hunt();
            return;
        }
        updatePetsProb();
        int maxTurns = Math.min(lookAhead, 300 - turn);
        NEXT: for (int i = 0; i < traps.length; i++) {
            Trap t = traps[i];
            int[] tv = trapsVal[i];
            Arrays.fill(tv, 0);
            for (int a : t.doors) {
                if (blocked[a]) continue NEXT;
            }
            for (int a : t.inside) {
                if (blocked[a]) continue NEXT;
            }
            for (int a : t.walls) {
                if (!blocked[a]) continue NEXT;
            }
            if (numHumans > 5) {
                int h0a = -1;
                int h1a = -1;
                int h0b = -1;
                int h1b = -1;
                for (int j = 0; j < numHumans; j++) {
                    if (usedHumans[j]) continue;
                    int p = humansPos[j];
                    int d0 = dist(p, t.doors[0]);
                    int d1 = dist(p, t.doors[1]);
                    if (h0a == -1 && d0 == 0) h0a = j;
                    else if (h1a == -1 && d1 == 1) h1a = j;
                    if (h0b == -1 && d0 == 1) h0b = j;
                    else if (h1b == -1 && d1 == 0) h1b = j;
                }
                if ((h0a != -1 && h1a != -1) || (h0b != -1 && h1b != -1)) {
                    int idx = (h0a != -1 && h1a != -1) ? 0 : 1;
                    int h0 = idx == 0 ? h0a : h0b;
                    int h1 = idx == 0 ? h1a : h1b;
                    int t0 = idx == 0 ? t.doors[0] + 32 : t.doors[0];
                    int t1 = idx == 0 ? t.doors[1] : t.doors[1] - 32;
                    if (!busy[t0] && !busy[t1]) {
                        for (int a : t.inside) {
                            for (int j = 0; j < numHumans; j++) {
                                if (a == humansPos[j]) {
                                    h0 = -1;
                                    break;
                                }
                            }
                        }
                        if (h0 != -1) {
                            int trapPets = 0;
                            for (int a : t.capture) {
                                for (int j = 0; j < numPets; j++) {
                                    if (a == petsPos[j] && !petsCaptured[j]) trapPets++;
                                }
                            }
                            if (trapPets > 0) {
                                moves[h0] = moveChar(humansPos[h0], t0, true);
                                moves[h1] = moveChar(humansPos[h1], t1, true);
                                blocked[t0] = blocked[t1] = true;
                                updateOptNeighbors(t0);
                                updateOptNeighbors(t1);
                                usedHumans[h0] = usedHumans[h1] = true;
                                updatePetsProb();
                                i = -1;
                                continue;
                            }
                        }
                    }
                }
            }
            double m = 1 + 0.1 / (0.5 + Math.abs(15.5 - y(t.doors[0])));
            for (int p : t.capture) {
                double prev = 0;
                for (int j = maxTurns - 1; j >= 0; j--) {
                    tv[j] += prev += petsProbTot[j][p] * m / (j + 1) / t.inside.length;
                }
            }
        }
        boolean[] usedTargets = new boolean[targets.length];
        for (int i = 0; i < targets.length; i++) {
            Target g = targets[i];
            int[] gv = targetsVal[i];
            Arrays.fill(gv, 0);
            for (Trap t : g.traps) {
                int[] tv = trapsVal[t.idx];
                for (int k = maxTurns - 1; k >= 0; k--) {
                    gv[k] += tv[k];
                }
            }
            if (gv[0] == 0) usedTargets[i] = true;
        }
        for (int i = 0; i < numHumans; i++) {
            bfs(humansPos[i], humansDist[i], -1);
        }
        int pendingHumans = numHumans;
        for (int pass = 0; pass < 2; pass++) {
            while (pendingHumans > 0) {
                double max = 0;
                int target = -1;
                int h0 = -1;
                int h1 = -1;
                for (int i = 0; i < targets.length; i++) {
                    if (usedTargets[i]) continue;
                    int[] gv = targetsVal[i];
                    if (gv[0] <= max) continue;
                    Target g = targets[i];
                    double penalty = 1;
                    for (int p : g.pos) {
                        int min = inf;
                        for (int j = 0; j < numHumans; j++) {
                            if (!usedHumans[j]) continue;
                            int d0 = humansDist[j][p];
                            if (d0 < min) min = d0;
                        }
                        if (min < inf) {
                            if (min == 0) min = 1;
                            penalty *= 1 + 1.0 / min;
                        }
                    }
                    int p0 = g.pos[0];
                    int p1 = g.pos[1];
                    for (int j0 = 0; j0 < numHumans; j0++) {
                        if (usedHumans[j0] && pass == 0) continue;
                        int d0 = humansDist[j0][p0];
                        if (d0 >= maxTurns) continue;
                        if (gv[d0] <= max * penalty) continue;
                        for (int j1 = 0; j1 < numHumans; j1++) {
                            if (j1 == j0 || (pass == 0 && usedHumans[j1])) continue;
                            if (pass == 1 && !usedHumans[j0] && !usedHumans[j1]) continue;
                            int d1 = humansDist[j1][p1];
                            if (d1 >= maxTurns) continue;
                            int d = Math.max(d0, d1);
                            double curr = gv[d] / penalty;
                            if (curr > max) {
                                max = curr;
                                target = i;
                                h0 = j0;
                                h1 = j1;
                            }
                        }
                    }
                }
                if (target == -1) break;
                boolean bl = false;
                Target g = targets[target];
                usedTargets[target] = true;
                for (int i = 0; i < targets.length; i++) {
                    if (i == g.idx) continue;
                    Target o = targets[i];
                    int[] gv = targetsVal[i];
                    for (Trap t : g.traps) {
                        for (Trap to : o.traps) {
                            if (t.equals(to)) {
                                int[] tv = trapsVal[to.idx];
                                for (int k = maxTurns - 1; k >= 0; k--) {
                                    gv[k] -= tv[k];
                                }
                            }
                        }
                    }
                }
                int p0 = g.pos[0];
                int p1 = g.pos[1];
                int d0 = humansDist[h0][p0];
                int d1 = humansDist[h1][p1];
                if (d0 == 0 && d1 == 0 && !usedHumans[h0] && !usedHumans[h1]) {
                    int t0 = -1;
                    int t1 = -1;
                    int maxPets = 0;
                    NEXT: for (Trap t : g.traps) {
                        for (int a : t.doors) {
                            if (busy[a]) continue NEXT;
                        }
                        for (int a : t.walls) {
                            if (!blocked[a]) continue NEXT;
                        }
                        for (int a : t.inside) {
                            for (int i = 0; i < numHumans; i++) {
                                if (a == humansPos[i]) continue NEXT;
                            }
                        }
                        int trapPets = 0;
                        for (int a : t.capture) {
                            for (int i = 0; i < numPets; i++) {
                                if (a == petsPos[i] && !petsCaptured[i]) trapPets++;
                            }
                        }
                        if (trapPets > maxPets) {
                            maxPets = trapPets;
                            t0 = t.doors[0];
                            t1 = t.doors[1];
                        }
                    }
                    if (t0 != -1) {
                        if (!blocked[t0]) {
                            moves[h0] = moveChar(humansPos[h0], t0, true);
                            blocked[t0] = true;
                            updateOptNeighbors(t0);
                            bl = true;
                        }
                        if (!blocked[t1]) {
                            moves[h1] = moveChar(humansPos[h1], t1, true);
                            blocked[t1] = true;
                            updateOptNeighbors(t1);
                            bl = true;
                        }
                    }
                } else {
                    if (d0 != 0 && !usedHumans[h0]) {
                        int p = humansPos[h0];
                        bfs(p0, dd, p);
                        for (int i : dirs) {
                            int np = p + i;
                            if (!blocked[np] && dd[np] < dd[p]) {
                                moves[h0] = moveChar(p, np, false);
                                humansPos[h0] = np;
                                busy[np] = true;
                                break;

                            }
                        }
                    }
                    if (d1 != 0 && !usedHumans[h1]) {
                        int p = humansPos[h1];
                        bfs(p1, dd, p);
                        for (int i : dirs) {
                            int np = p + i;
                            if (!blocked[np] && dd[np] < dd[p]) {
                                moves[h1] = moveChar(p, np, false);
                                humansPos[h1] = np;
                                busy[np] = true;
                                break;
                            }
                        }
                    }
                }
                if (!usedHumans[h0]) {
                    usedHumans[h0] = true;
                    pendingHumans--;
                }
                if (!usedHumans[h1]) {
                    usedHumans[h1] = true;
                    pendingHumans--;
                }
                if (bl) {
                    for (int i = 0; i < numHumans; i++) {
                        bfs(humansPos[i], humansDist[i], -1);
                    }
                    updatePetsProb();
                }
            }
        }
        if (numHumans > 5) {
            Arrays.fill(usedHumans, false);
            NEXT: for (int i = 0; i < traps.length; i++) {
                Trap t = traps[i];
                int[] tv = trapsVal[i];
                Arrays.fill(tv, 0);
                for (int a : t.doors) {
                    if (blocked[a]) continue NEXT;
                }
                for (int a : t.inside) {
                    if (blocked[a]) continue NEXT;
                }
                for (int a : t.walls) {
                    if (!blocked[a]) continue NEXT;
                }
                int h0 = -1;
                int h1 = -1;
                for (int j = 0; j < numHumans; j++) {
                    if (moves[j] != '.' || usedHumans[j]) continue;
                    int p = humansPos[j];
                    if (h0 == -1 && dist(p, t.doors[0]) == 1) h0 = j;
                    else if (h1 == -1 && dist(p, t.doors[1]) == 1) h1 = j;
                }
                if (h0 != -1 && h1 != -1) {
                    for (int a : t.inside) {
                        for (int j = 0; j < numHumans; j++) {
                            if (a == humansPos[j]) {
                                h0 = -1;
                                break;
                            }
                        }
                    }
                    if (h0 != -1) {
                        for (int a : t.capture) {
                            for (int j = 0; j < numPets; j++) {
                                if (a == petsPos[j] && !petsCaptured[j]) {
                                    h0 = -1;
                                    break;
                                }
                            }
                        }
                        if (h0 != -1) {
                            int pet = -1;
                            for (int a : t.inside) {
                                for (int j = 0; j < numPets; j++) {
                                    if (a == petsPos[j] && !petsCaptured[j]) {
                                        if (pet == -1) pet = j;
                                        else {
                                            pet = -1;
                                            break;
                                        }
                                    }
                                }
                            }
                            if (pet != -1) {
                                int type = petsType[pet];
                                int pos = petsPos[pet];
                                int idx = dist(t.doors[0], pos) == 1 ? 0 : 1;
                                if (type == 1 || type == 3) idx = 1 - idx;
                                int hh = idx == 0 ? h0 : h1;
                                int hp = humansPos[hh];
                                usedHumans[h0] = usedHumans[h1] = true;
                                int np = t.doors[idx];
                                moves[hh] = moveChar(hp, np, false);
                                humansPos[hh] = np;
                                busy[np] = true;
                                continue NEXT;
                            }
                        }
                    }
                }
            }
        }
    }

    private static void updateOptNeighbors(int p) {
        updateOpt(p);
        updateOpt(p + 32);
        updateOpt(p - 32);
        updateOpt(p + 1);
        updateOpt(p - 1);
    }

    private static void updateOpt(int p) {
        if (blocked[p]) {
            optCnt[p] = 0;
            return;
        }
        int[] opt = optPos[p];
        int cnt = 0;
        int np = p - 32;
        if (!blocked[np]) opt[cnt++] = np;
        if (!blocked[np += 64]) opt[cnt++] = np;
        if (!blocked[np -= 33]) opt[cnt++] = np;
        if (!blocked[np += 2]) opt[cnt++] = np;
        optCnt[p] = cnt;
    }

    private static void updatePetsProb() {
        int maxTurns = Math.min(lookAhead, 300 - turn);
        updatePetsProb(maxTurns);
    }

    private static void updatePetsProb(int maxTurns) {
        for (int i = 0; i < petsProbTot.length; i++) {
            Arrays.fill(petsProbTot[i], 0);
        }
        for (int t = 1; t <= 5; t++) {
            int step = t;
            if (t >= 4) step = 2;
            int numTurns = maxTurns * step;
            for (int j = 0; j <= numTurns; j++) {
                Arrays.fill(petsProb[j], 0);
            }
            boolean done = true;
            for (int j = 0; j < numPets; j++) {
                if (petsType[j] != t || petsCaptured[j]) continue;
                int pos = petsPos[j];
                int m = t == 1 ? 120_000_000 : 100_000_000;
                petsProb[0][pos] += m;
                petsProbTot[0][pos] += m;
                done = false;
            }
            if (done) continue;
            int[] prev = petsProb[0];
            for (int j = 1; j <= numTurns; j++) {
                int[] next = petsProb[j];
                for (int p = 0; p < prev.length; p++) {
                    int prob = prev[p];
                    if (prob < 10000) continue;
                    int cnt = optCnt[p];
                    int[] opt = optPos[p];
                    if (cnt > 0) {
                        if ((t == 5 || t == 4) && j % step == 1 && j <= 3 && turn > 30) {
                            for (int k = 0; k < numPets; k++) {
                                if (petsType[k] != t || petsCaptured[k]) continue;
                                if (dist(petsPos[k], p) >= j) continue;
                                int c = petsHistoryCnt[k];
                                int[] h = petsHistory[k];
                                for (int l = c - 2, q = 0; l >= 0 && q < 8; l -= 2, q++) {
                                    int np = p + h[l];
                                    if (!blocked[np]) opt[cnt++] = np;
                                }
                            }
                        }
                        int pc = prob / cnt;
                        for (int l = 0; l < cnt; l++) {
                            next[opt[l]] += pc;
                        }
                    }
                }
                if (j % step == 0) {
                    int[] pt = petsProbTot[j / step];
                    for (int p = 0; p < next.length; p++) {
                        int prob = next[p];
                        if (prob != 0) pt[p] += prob;
                    }
                }
                prev = next;
            }
        }
    }

    private static boolean bfs(int start, int[] dist, int target) {
        int tot = 0;
        int curr = 0;
        Arrays.fill(dist, inf);
        queue[tot++] = start;
        dist[start] = 0;
        while (curr < tot) {
            int p = queue[curr++];
            if (p == target) break;
            int nd = dist[p] + 1;
            if (nd >= inf) continue;
            if (dist[p -= 32] > nd && !blocked[p]) dist[queue[tot++] = p] = nd;
            if (dist[p += 64] > nd && !blocked[p]) dist[queue[tot++] = p] = nd;
            if (dist[p -= 33] > nd && !blocked[p]) dist[queue[tot++] = p] = nd;
            if (dist[p += 2] > nd && !blocked[p]) dist[queue[tot++] = p] = nd;
        }
        return tot <= 32;
    }

    private static int ff(int start, int limit) {
        int tot = 0;
        int curr = 0;
        Arrays.fill(connected, false);
        queue[tot++] = start;
        connected[start] = true;
        while (curr < tot) {
            int p = queue[curr++];
            if (!connected[p -= 32] && !blocked[p]) connected[queue[tot++] = p] = true;
            if (!connected[p += 64] && !blocked[p]) connected[queue[tot++] = p] = true;
            if (!connected[p -= 33] && !blocked[p]) connected[queue[tot++] = p] = true;
            if (!connected[p += 2] && !blocked[p]) connected[queue[tot++] = p] = true;
            if (tot >= limit) break;
        }
        return tot;
    }

    private static int fill(int start, int[] map, int limit) {
        int tot = 0;
        int curr = 0;
        Arrays.fill(connected, false);
        queue[tot++] = start;
        connected[start] = true;
        while (curr < tot && tot < limit) {
            int p = queue[curr++];
            if (!connected[p -= 32] && map[p] >= 0) connected[queue[tot++] = p] = true;
            if (!connected[p += 64] && map[p] >= 0) connected[queue[tot++] = p] = true;
            if (!connected[p -= 33] && map[p] >= 0) connected[queue[tot++] = p] = true;
            if (!connected[p += 2] && map[p] >= 0) connected[queue[tot++] = p] = true;
        }
        return tot;
    }

    private static char moveChar(int p1, int p2, boolean block) {
        char m = '.';
        if (p2 == p1 + 1) m = 'R';
        else if (p2 == p1 - 1) m = 'L';
        else if (p2 == p1 - 32) m = 'U';
        else if (p2 == p1 + 32) m = 'D';
        else if (p2 != p1 || block) System.err.println("ERR MoveChar >>>>> " + p1 + " :" + p2 + " : " + block);
        if (block) m = Character.toLowerCase(m);
        return m;
    }

    private static int pos(int x, int y) {
        return (x << 5) | y;
    }

    private static int x(int p) {
        return p >>> 5;
    }

    private static int y(int p) {
        return p & 31;
    }

    private static void init() {
        try {
            in = new BufferedReader(new InputStreamReader(System.in));
            numPets = Integer.parseInt(in.readLine());
            petsPos = new int[numPets];
            petsType = new int[numPets];
            petsHistory = new int[numPets][];
            petsHistoryCnt = new int[numPets];
            for (int i = 0; i < numPets; i++) {
                String[] s = in.readLine().split(" ");
                petsPos[i] = pos(Integer.parseInt(s[0]), Integer.parseInt(s[1]));
                int t = petsType[i] = Integer.parseInt(s[2]);
                if (t == 4 || t == 5) petsHistory[i] = new int[600];
            }
            numHumans = Integer.parseInt(in.readLine());
            humansPos = new int[numHumans];
            moves = new char[numHumans];
            for (int i = 0; i < numHumans; i++) {
                String[] s = in.readLine().split(" ");
                humansPos[i] = pos(Integer.parseInt(s[0]), Integer.parseInt(s[1]));
            }
            busy = new boolean[mem];
            blocked = new boolean[mem];
            for (int i = 0; i < 32; i++) {
                blocked[pos(0, i)] = blocked[pos(31, i)] = blocked[pos(i, 0)] = blocked[pos(i, 31)] = true;
            }
            for (int p = 0; p < mem; p++) {
                updateOpt(p);
            }
            dd = new int[mem];
            petsDist = new int[numPets][mem];
            humansDist = new int[numHumans][mem];
            for (int i = 0; i < log.length; i++) {
                log[i] = Math.log((i + 0.5) / log.length);
            }
            petsCaptured = new boolean[numPets];
            initialPaths = new int[numHumans][256];
            initialPathsSize = new int[numHumans];
            initialPathsPos = new int[numHumans];
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

class Trap {
    int[] inside;
    int[] capture;
    int[] walls;
    int[] doors;
    int idx;
}

class Target {
    int[] pos;
    Trap[] traps;
    int idx;
}
