import java.io.*;
import java.util.*;

public class Main {
    private static final long timeLimit = 2828;
    private static final long timeStart = System.currentTimeMillis();
    private static final SplittableRandom rnd = new SplittableRandom(999);
    private static final double[] log = new double[1 << 16];
    private static final int[] ndx = {-2,-1,-1,-1,0,0,0,0,1,1,1,2}, ndy = {0,-1,0,1,-2,-1,1,2,-1,0,1,0};
    private static int n, numPeople, initialMoney, numTurns, lastTurn;
    private static byte[] connected, otw;
    private static int[] peopleDist, from, locations, path, ll;
    private static int[][] peoplePerLocation;
    private static Sol bestSol;

    public static void main(String[] args) throws Exception {
        init();
        solve(0, timeStart + timeLimit / 6);
        solve(1, timeStart + timeLimit / 3);
        solve(2, timeStart + timeLimit / 2);
        solve(3, timeStart + timeLimit);
        improve();
        output();
    }

    private static void solve(int pass, long timeout) {
        numTurns = new int[] {200,350,450,800}[pass];
        Sol currSol = new Sol();
        Sol nextSol = new Sol();
        currSol.copyFrom(bestSol);
        boolean[] used = new boolean[n << 6];
        for (int i = 0; i < currSol.len; i++) {
            used[currSol.stations[i]] = true;
        }
        currSol.eval();
        byte[] currConn = connected.clone();
        double t0 = pass == 0 ? 100 : 3000;
        double temp = t0;
        int step = 0;
        double duration = timeout - System.currentTimeMillis();
        while (true) {
            if ((++step & 127) == 0) {
                long t = System.currentTimeMillis();
                if (t >= timeout) break;
                temp = t0 * (timeout - t) / duration;
            }
            int add = -1;
            int rem = -1;
            int mode = rnd.nextInt(7);
            if (mode == 0) {
                // Insert station
                if (currSol.len == currSol.stations.length) continue;
                int add1 = locations[rnd.nextInt(locations.length)];
                int add2 = locations[rnd.nextInt(locations.length)];
                int add3 = locations[rnd.nextInt(locations.length)];
                int v1 = used[add1] ? -1 : evalConn(add1, currConn);
                int v2 = used[add2] ? -1 : evalConn(add2, currConn);
                int v3 = used[add3] ? -1 : evalConn(add3, currConn);
                if (v1 < 0 && v2 < 0 && v3 < 0) continue;
                add = v1 >= v2 && v1 >= v3 ? add1 : v2 >= v3 ? add2 : add3;
                int pos = rnd.nextInt(currSol.len + 1);
                System.arraycopy(currSol.stations, 0, nextSol.stations, 0, pos);
                System.arraycopy(currSol.stations, pos, nextSol.stations, pos + 1, currSol.len - pos);
                nextSol.stations[pos] = add;
                nextSol.len = currSol.len + 1;
            } else if (mode == 1) {
                // Move station to somewhere else
                if (currSol.len == 0) continue;
                int add1 = locations[rnd.nextInt(locations.length)];
                int add2 = locations[rnd.nextInt(locations.length)];
                int v1 = used[add1] ? -1 : evalConn(add1, currConn);
                int v2 = used[add2] ? -1 : evalConn(add2, currConn);
                if (v1 < 0 && v2 < 0) continue;
                add = v1 >= v2 ? add1 : add2;
                int pos = rnd.nextInt(currSol.len);
                rem = currSol.stations[pos];
                System.arraycopy(currSol.stations, 0, nextSol.stations, 0, currSol.len);
                nextSol.len = currSol.len;
                nextSol.stations[pos] = add;
            } else if (mode >= 5) {
                // Move station to neighbor (x,y +1,-1) 
                if (currSol.len == 0) continue;
                int pos = rnd.nextInt(currSol.len);
                rem = currSol.stations[pos];
                int add1 = neighbor(rem);
                int add2 = neighbor(rem);
                int v1 = add1 == -1 || used[add1] ? -1 : evalConn(add1, currConn);
                int v2 = add2 == -1 || used[add2] ? -1 : evalConn(add2, currConn);
                if (v1 < 0 && v2 < 0) continue;
                add = v1 >= v2 ? add1 : add2;
                System.arraycopy(currSol.stations, 0, nextSol.stations, 0, currSol.len);
                nextSol.len = currSol.len;
                nextSol.stations[pos] = add;
            } else if (mode == 2) {
                // Swap order
                if (currSol.len < 2) continue;
                int p1 = rnd.nextInt(currSol.len);
                int p2 = rnd.nextInt(currSol.len);
                if (p1 == p2) continue;
                System.arraycopy(currSol.stations, 0, nextSol.stations, 0, currSol.len);
                nextSol.len = currSol.len;
                nextSol.stations[p1] = currSol.stations[p2];
                nextSol.stations[p2] = currSol.stations[p1];
            } else if (mode == 3) {
                // Rotate order
                if (currSol.len < 2) continue;
                int p1 = rnd.nextInt(currSol.len);
                int p2 = rnd.nextInt(currSol.len);
                if (p1 == p2) continue;
                System.arraycopy(currSol.stations, 0, nextSol.stations, 0, currSol.len);
                nextSol.len = currSol.len;
                if (p1 < p2) System.arraycopy(currSol.stations, p1 + 1, nextSol.stations, p1, p2 - p1);
                else System.arraycopy(currSol.stations, p2, nextSol.stations, p2 + 1, p1 - p2);
                nextSol.stations[p2] = currSol.stations[p1];
            } else if (mode == 4) {
                // Remove station
                if (currSol.len <= 2) continue;
                int pos = rnd.nextInt(currSol.len);
                rem = currSol.stations[pos];
                System.arraycopy(currSol.stations, 0, nextSol.stations, 0, pos);
                System.arraycopy(currSol.stations, pos + 1, nextSol.stations, pos, currSol.len - pos - 1);
                nextSol.len = currSol.len - 1;
            }
            nextSol.eval();
            if (nextSol.value == Integer.MIN_VALUE) continue;
            int gain = pass == 0 ? nextSol.income - currSol.income : nextSol.value - currSol.value;
            if (gain >= 0 || gain > log[rnd.nextInt(log.length)] * temp) {
                if (add != -1) used[add] = true;
                if (rem != -1) used[rem] = false;
                currSol.copyFrom(nextSol);
                System.arraycopy(connected, 0, currConn, 0, connected.length);
                if ((pass == 0 && currSol.income > bestSol.income) || (pass > 0 && currSol.value > bestSol.value)) {
                    bestSol.copyFrom(currSol);
                }
            }
        }
    }

    private static void improve() {
        boolean[] used = new boolean[n << 6];
        for (int i = 0; i < bestSol.len; i++) {
            used[bestSol.stations[i]] = true;
        }
        int rem = numTurns - lastTurn;
        bestSol.eval();
        byte[] sc = connected.clone();
        for (int i : ll) {
            if (used[i]) continue;
            if (System.currentTimeMillis() > timeStart + timeLimit + 50) break;
            if (bestSol.len == bestSol.stations.length) break;
            int incomeGain = 0;
            int[] p = peoplePerLocation[i];
            for (int v : p) {
                if (v < numPeople) {
                    if (connected[v] == 2) incomeGain += peopleDist[v];
                } else {
                    if (connected[v -= numPeople] == 1) incomeGain += peopleDist[v];
                }
            }
            if (incomeGain * rem > 5000) {
                Sol s = new Sol();
                s.copyFrom(bestSol);
                s.stations[s.len++] = i;
                s.eval();
                if (s.value > bestSol.value) {
                    bestSol.copyFrom(s);
                    used[i] = true;
                    continue;
                }
            }
            System.arraycopy(sc, 0, connected, 0, sc.length);
        }
    }

    private static void output() {
        bestSol.eval();
        List<Cmd> cmds = new ArrayList<Cmd>();
        for (int y = 0; y < n; y++) {
            for (int x = 0; x < n; x++) {
                int pos = pos(x, y);
                int p = path[pos];
                if (p != 0) {
                    Cmd cmd = new Cmd();
                    if (p > 256) {
                        cmd.isStation = true;
                        cmd.order = ((p >>> 8) - 1) * 2 - 1;
                        int prev = p & 255;
                        if (prev != 0) {
                            Cmd cmd2 = new Cmd();
                            cmd2.pos = pos;
                            cmd2.order = prev * 2;
                            cmds.add(cmd2);
                        }
                    } else {
                        cmd.order = p * 2;
                    }
                    cmd.pos = pos;
                    cmds.add(cmd);
                }
            }
        }
        Collections.sort(cmds);
        for (Cmd cmd : cmds) {
            if (!cmd.isStation) {
                int p0 = -1;
                int p1 = cmd.pos;
                int p2 = -1;
                int cv = path[p1] & 255;
                for (int rep = 0; rep < 2; rep++) {
                    for (int i = 0; i < 4; i++) {
                        int x = x(p1) + (i == 0 ? 1 : i == 1 ? -1 : 0);
                        if (x < 0 || x >= n) continue;
                        int y = y(p1) + (i == 2 ? 1 : i == 3 ? -1 : 0);
                        if (y < 0 || y >= n) continue;
                        int np = pos(x, y);
                        int nv = path[np];
                        int ps = (nv >>> 8) - 1;
                        if ((rep == 1 && ps > 0 && ps - 1 <= cv) || (rep == 0 && (nv & 255) == cv)) {
                            if (p0 == -1) p0 = np;
                            else if (p2 == -1) p2 = np;
                        }
                    }
                }
                int m = -1;
                int xp0 = x(p0);
                int xp1 = x(p1);
                int xp2 = x(p2);
                int yp0 = y(p0);
                int yp1 = y(p1);
                int yp2 = y(p2);
                if (yp0 == yp1 && yp1 == yp2) m = 1;
                else if (xp0 == xp1 && xp1 == xp2) m = 2;
                else if (xp0 == xp1 - 1 && yp1 == yp2 - 1) m = 3;
                else if (xp2 == xp1 - 1 && yp1 == yp0 - 1) m = 3;
                else if (xp0 == xp1 + 1 && yp1 == yp2 - 1) m = 6;
                else if (xp2 == xp1 + 1 && yp1 == yp0 - 1) m = 6;
                else if (xp0 == xp1 - 1 && yp1 == yp2 + 1) m = 4;
                else if (xp2 == xp1 - 1 && yp1 == yp0 + 1) m = 4;
                else if (xp0 == xp1 + 1 && yp1 == yp2 + 1) m = 5;
                else if (xp2 == xp1 + 1 && yp1 == yp0 + 1) m = 5;
                cmd.move = m;
            }
        }
        int money = initialMoney;
        Arrays.fill(connected, (byte) 0);
        int income = 0;
        StringBuilder sb = new StringBuilder();
        int turn = 0;
        OUT: for (int i = 0; i < cmds.size(); i++) {
            Cmd cmd = cmds.get(i);
            if (cmd.pos == -1) continue;
            if (cmd.isStation) {
                while (money < 5000 && turn < numTurns) {
                    if (income == 0) break OUT;
                    int wait = (5000 - money + income - 1) / income;
                    boolean ahead = false;
                    if (money + income * wait >= 5100) {
                        for (int j = i + 1; j < cmds.size(); j++) {
                            Cmd cj = cmds.get(j);
                            if (cj.pos != -1 && !cj.isStation) {
                                ahead = true;
                                money -= 100;
                                sb.append(cj.move).append(' ').append(y(cj.pos)).append(' ').append(x(cj.pos)).append('\n');
                                cj.pos = -1;
                                break;
                            }
                        }
                    }
                    turn++;
                    money += income;
                    if (!ahead) sb.append("-1").append('\n');
                }
                if (turn == numTurns) break OUT;
                sb.append('0').append(' ').append(y(cmd.pos)).append(' ').append(x(cmd.pos)).append('\n');
                int[] p = peoplePerLocation[cmd.pos];
                for (int v : p) {
                    if (v < numPeople) {
                        if (connected[v] != 3 && (connected[v] |= 1) == 3) income += peopleDist[v];
                    } else {
                        v -= numPeople;
                        if (connected[v] != 3 && (connected[v] |= 2) == 3) income += peopleDist[v];
                    }
                }
                turn++;
                money -= 5000;
                money += income;
            } else {
                while (money < 100 && turn < numTurns) {
                    if (income == 0) break OUT;
                    turn++;
                    money += income;
                    sb.append("-1").append('\n');
                }
                if (turn == numTurns) break OUT;
                sb.append(cmd.move).append(' ').append(y(cmd.pos)).append(' ').append(x(cmd.pos)).append('\n');
                turn++;
                money -= 100;
                money += income;
            }
            if (turn == numTurns) break;
        }
        for (int i = turn; i < numTurns; i++) {
            money += income;
            sb.append("-1").append('\n');
        }
        System.out.print(sb);
        System.out.flush();
    }

    private static int neighbor(int p) {
        int k = rnd.nextInt(ndx.length);
        int x = x(p) + ndx[k];
        if (x < 0 || x >= n) return -1;
        int y = y(p) + ndy[k];
        if (y < 0 || y >= n) return -1;
        return pos(x, y);
    }

    private static int evalConn(int s, byte[] conn) {
        int[] p = peoplePerLocation[s];
        int ret = 0;
        int c = 0;
        for (int v : p) {
            if (v < numPeople) {
                if ((c = conn[v]) == 2) ret += peopleDist[v] << 3;
            } else {
                if ((c = conn[v -= numPeople]) == 1) ret += peopleDist[v] << 3;
            }
            if (c == 0) ret += peopleDist[v];
        }
        return ret;
    }

    private static boolean place(int[] path, int a, int b, int idx) {
        int xa = x(a);
        int ya = y(a);
        int xb = x(b);
        int yb = y(b);
        int dy = ya < yb ? 1 : ya > yb ? -1 : 0;
        int dx = xa < xb ? 1 : xa > xb ? -1 : 0;
        if (dx == 0) {
            ya += dy;
            while (ya != yb) {
                int pos = pos(xa, ya);
                if ((path[pos] & 255) != 0) return false;
                path[pos] |= idx;
                ya += dy;
            }
        } else if (dy == 0) {
            xa += dx;
            while (xa != xb) {
                int pos = pos(xa, ya);
                if ((path[pos] & 255) != 0) return false;
                path[pos] |= idx;
                xa += dx;
            }
        } else {
            Arrays.fill(otw, Math.min(ya, yb) << 6, (Math.max(ya, yb) + 1) << 6, (byte) -1);
            otw[a] = 0;
            for (int y = ya;; y += dy) {
                int p = pos(xa, y);
                for (int x = xa;; x += dx, p += dx) {
                    int pp = path[p];
                    if ((pp & 255) == 0) {
                        int prev = -1;
                        byte max = -1;
                        if (x != xa) {
                            int ph = p - dx;
                            byte v = otw[ph];
                            if (v > max) {
                                max = v;
                                prev = ph;
                            }
                        }
                        if (y != ya) {
                            int pv = p - dy * 64;
                            byte v = otw[pv];
                            if (v > max) {
                                max = v;
                                prev = pv;
                            }
                        }
                        if (prev != -1) {
                            if (pp >= 256) max++;
                            otw[p] = max;
                            from[p] = prev;
                        }
                    }
                    if (x == xb) break;
                }
                if (y == yb) break;
            }
            if (otw[b] == -1) return false;
            int p = from[b];
            while (p != a) {
                path[p] |= idx;
                p = from[p];
            }
        }
        return true;
    }

    private static class Sol {
        int[] stations = new int[255];
        int len, value, income;

        void eval() {
            if (!doEval()) value = Integer.MIN_VALUE;
        }

        void copyFrom(Sol o) {
            value = o.value;
            len = o.len;
            income = o.income;
            System.arraycopy(o.stations, 0, stations, 0, len);
        }

        boolean doEval() {
            int turn = 0;
            income = 0;
            value = initialMoney;
            Arrays.fill(connected, (byte) 0);
            Arrays.fill(path, 0);
            for (int i = 0; i < len; i++) {
                int s = stations[i];
                if (path[s] == 256) return false;
                path[s] = 256;
            }
            int ahead = 0;
            for (int i = 0; i < len; i++) {
                int s = stations[i];
                int rails = 0;
                if (path[s] == 256 && i > 0) {
                    int min = 10000;
                    int target = -1;
                    for (int j = 0; j < i; j++) {
                        int sj = stations[j];
                        int d = dist(s, sj);
                        if (d < min) {
                            min = d;
                            target = sj;
                        }
                    }
                    if (!place(path, target, s, i)) return false;
                    rails = min - 1;
                }
                if (ahead > rails) {
                    ahead -= rails;
                    rails = 0;
                } else {
                    rails -= ahead;
                    ahead = 0;
                }
                int nextTurn = turn + rails;
                if (nextTurn > numTurns) return false;
                int cost = rails * 100 + 5000;
                int add = 0;
                if (cost > value) {
                    if (income == 0) return false;
                    int wait = (cost - value + income - 1) / income;
                    if (wait > rails) {
                        if (income >= 100) add = Math.min(wait - rails, (value + income * wait - cost) / 100);
                        wait -= rails;
                        nextTurn += wait;
                        if (nextTurn >= numTurns) return false;
                    }
                }
                path[s] = ((2 + i) << 8) | (path[s] & 255);
                if (add > 0) {
                    value -= add * 100;
                    ahead += add;
                }
                value += (nextTurn - turn) * income - cost;
                int[] p = peoplePerLocation[s];
                for (int v : p) {
                    if (v < numPeople) {
                        if (connected[v] != 3 && (connected[v] |= 1) == 3) income += peopleDist[v];
                    } else {
                        if (connected[v -= numPeople] != 3 && (connected[v] |= 2) == 3) income += peopleDist[v];
                    }
                }
                value += income;
                turn = nextTurn + 1;
                if (turn > numTurns) return false;
            }
            lastTurn = turn;
            value += (numTurns - turn) * income;
            return true;
        }
    }

    private static final int dist(int a, int b) {
        return Math.abs(y(a) - y(b)) + Math.abs(x(a) - x(b));
    }

    private static final int pos(int x, int y) {
        return x | (y << 6);
    }

    private static final int x(int p) {
        return p & 63;
    }

    private static final int y(int p) {
        return p >>> 6;
    }

    private static void init() throws Exception {
        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
        String[] s = in.readLine().split(" ");
        n = Integer.parseInt(s[0]);
        numPeople = Integer.parseInt(s[1]);
        initialMoney = Integer.parseInt(s[2]);
        numTurns = Integer.parseInt(s[3]);
        peoplePerLocation = new int[n << 6][512];
        int[] cnt = new int[n << 6];
        peopleDist = new int[numPeople];
        for (int i = 0; i < numPeople; i++) {
            s = in.readLine().split(" ");
            int home = pos(Integer.parseInt(s[1]), Integer.parseInt(s[0]));
            int work = pos(Integer.parseInt(s[3]), Integer.parseInt(s[2]));
            peopleDist[i] = dist(home, work);
            for (int j = 0; j < 2; j++) {
                int p = j == 0 ? home : work;
                int idx = j == 0 ? i : i + numPeople;
                int x = x(p);
                int y = y(p);
                for (int dx = -2; dx <= 2; dx++) {
                    int nx = x + dx;
                    if (nx < 0 || nx >= n) continue;
                    for (int dy = -2; dy <= 2; dy++) {
                        if (Math.abs(dx) + Math.abs(dy) > 2) continue;
                        int ny = y + dy;
                        if (ny < 0 || ny >= n) continue;
                        int np = pos(nx, ny);
                        peoplePerLocation[np][cnt[np]++] = idx;
                    }
                }
            }
        }
        in.close();
        int numNonEmptyLocations = 0;
        locations = new int[n << 10];
        int tot = 0;
        for (int i = 0; i < peoplePerLocation.length; i++) {
            tot += cnt[i];
        }
        ll = new int[n << 6];
        int cc = 0;
        for (int i = 0; i < peoplePerLocation.length; i++) {
            peoplePerLocation[i] = Arrays.copyOf(peoplePerLocation[i], cnt[i]);
            if (cnt[i] > 0) {
                ll[cc++] = i;
                int v = cnt[i] * n * n * 3 / tot + 1;
                for (int j = 0; j < v; j++) {
                    locations[numNonEmptyLocations++] = i;
                }
            }
        }
        ll = Arrays.copyOf(ll, cc);
        locations = Arrays.copyOf(locations, numNonEmptyLocations);
        connected = new byte[numPeople];
        from = new int[n << 6];
        otw = new byte[n << 6];
        path = new int[n << 6];
        bestSol = new Sol();
        for (int i = 0; i < log.length; i++) {
            log[i] = Math.log((i + 0.5) / log.length);
        }
    }

    static class Cmd implements Comparable<Cmd> {
        boolean isStation;
        int pos, order, move;

        public int compareTo(Cmd o) {
            return order - o.order;
        }
    }
}
