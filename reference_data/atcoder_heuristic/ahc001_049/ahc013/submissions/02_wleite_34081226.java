import java.io.*;
import java.util.*;

public class Main {
    private static final long timeout = System.currentTimeMillis() + 2700;
    private static int n, k, currConn, currScore, currMoves, numFreeGroups, maxScore, badColor1 = -1, badColor2 = -1, badColor3 = -1;
    private static int[] groupLen, freeGroups;
    private static Node[] nodes;
    private static Node saveConn0, saveConn1, saveConn2, saveConn3;
    private static final Node[] grid = new Node[4096], queue = new Node[4096], cr = new Node[4096], queue1 = new Node[4096], largest = new Node[1 << 18];
    private static final int[] used = new int[4096], qp = new int[] {1,64,-1,-64}, ord = new int[] {0,1,2,3}, ord2 = new int[] {0,1};
    private static final SplittableRandom rnd = new SplittableRandom(9999);
    private static final double[] log = new double[1 << 16];
    private static final List<Integer> bestConn = new ArrayList<Integer>();

    public static void main(String[] args) throws Exception {
        init();
        if (k >= 3) {
            solve(System.currentTimeMillis() + 200);
            int[] max = new int[k];
            for (Node node : nodes) {
                max[node.color] = Math.max(max[node.color], groupLen[node.group]);
            }
            int min1 = 1000;
            int min2 = 1000;
            int min3 = 1000;
            for (int i = 0; i < k; i++) {
                if (max[i] < min1) {
                    min3 = min2;
                    badColor3 = badColor2;
                    min2 = min1;
                    badColor2 = badColor1;
                    min1 = max[i];
                    badColor1 = i;
                } else if (max[i] < min2) {
                    min3 = min2;
                    badColor3 = badColor2;
                    min2 = max[i];
                    badColor2 = i;
                } else if (max[i] < min3) {
                    min3 = max[i];
                    badColor3 = i;
                }
            }
            if (k == 3) badColor2 = badColor3 = -1;
            else if (k == 4) badColor3 = -1;
            reset();
        }
        solve(timeout);
        List<Integer> mv = improve();
        System.out.print(mount(mv));
        System.out.flush();
    }

    private static List<Integer> improve() {
        rebuild();
        List<Integer> bestMoves = new ArrayList<Integer>();
        Arrays.fill(cr, null);
        int pending = 0;
        for (Node node : nodes) {
            cr[node.orgPos] = node;
            if (node.orgPos != node.pos) queue[pending++] = node;
        }
        while (pending > 0) {
            for (int i = 0; i < pending; i++) {
                Node node = queue[i];
                if (cr[node.pos] == null) {
                    cr[node.pos] = node;
                    cr[node.orgPos] = null;
                    bestMoves.add(node.orgPos);
                    bestMoves.add(node.pos);
                    queue[i--] = queue[--pending];
                }
            }
        }
        List<Integer> remMoves = new ArrayList<Integer>();
        int prevMoves = bestMoves.size();
        int prevScore = currScore;
        for (Node node : nodes) {
            int len = groupLen[node.group];
            if (len > 1 && len <= 6) disconnect(node);
        }
        for (Node node : nodes) {
            if (groupLen[node.group] == 1 && node.pos != node.orgPos && grid[node.orgPos] == null && used[node.orgPos] == -1) {
                remMoves.add(node.orgPos);
                remMoves.add(node.pos);
                grid[node.pos] = null;
                grid[node.orgPos] = node;
                used[node.pos] = -1;
                used[node.orgPos] = node.color;
                node.pos = node.orgPos;
                currMoves--;
            }
        }
        int[] dist = new int[n << 6];
        int[] queue = new int[4096];
        List<Integer> topGroups = new ArrayList<Integer>();
        boolean head = true;
        REP: for (int rep = 0; rep < 100; rep++) {
            topGroups.clear();
            for (int i = 0; i < groupLen.length; i++) {
                int len = groupLen[i];
                if (len > 20 && len < 100) topGroups.add(i);
            }
            if (topGroups.isEmpty()) break;
            Collections.sort(topGroups, new Comparator<Integer>() {
                public int compare(Integer a, Integer b) {
                    return groupLen[b] - groupLen[a];
                }
            });
            if (currConn + currMoves + 1 <= nodes.length) {
                for (int g : topGroups) {
                    for (Node node : nodes) {
                        if (node.group != g) continue;
                        NEXT: for (int i = 0; i < 4; i++) {
                            Node first = null;
                            Node second = null;
                            int p = node.pos;
                            int dp = qp[i];
                            while (true) {
                                p += dp;
                                Node n2 = grid[p];
                                if (n2 == null) {
                                    if (used[p] >= 0) break;
                                } else {
                                    if (n2.group == g) continue NEXT;
                                    if (first == null) {
                                        if (n2.color == node.color) {
                                            connect(node, n2);
                                            continue REP;
                                        }
                                        first = n2;
                                    } else {
                                        if (n2.color == node.color) second = n2;
                                        break;
                                    }
                                }
                            }
                            if (second != null && currConn + currMoves + 2 <= nodes.length) {
                                int[] dirs = new int[] {ortog(i),reverse(ortog(i))};
                                int to = -1;
                                for (int dir : dirs) {
                                    int np = first.pos + qp[dir];
                                    if (grid[np] == null && used[np] < k) {
                                        if ((groupLen[first.group] == 1 && used[np] == -1) || (first.connCount() == 1 && first.conn(dir) != null)) {
                                            to = np;
                                            break;
                                        }
                                    }
                                }
                                if (to >= 0) {
                                    int from = first.pos;
                                    used[from] = -1;
                                    grid[from] = null;
                                    used[to] = first.color;
                                    grid[to] = first;
                                    first.pos = to;
                                    bestMoves.add(from);
                                    bestMoves.add(to);
                                    currMoves++;
                                    connect(node, second);
                                    continue REP;
                                }
                                if (second != null && currConn + currMoves + 3 <= nodes.length) {
                                    int to1 = -1;
                                    int to2 = -1;
                                    for (int dir : dirs) {
                                        int np1 = first.pos + qp[dir];
                                        if (grid[np1] == null && used[np1] < k) {
                                            int np2 = np1 + qp[dir];
                                            if (grid[np2] == null && groupLen[first.group] == 1 && used[np2] == -1) {
                                                to1 = np1;
                                                to2 = np2;
                                                break;
                                            }
                                        }
                                    }
                                    if (to1 >= 0) {
                                        int from = first.pos;
                                        used[from] = -1;
                                        grid[from] = null;
                                        used[to2] = first.color;
                                        grid[to2] = first;
                                        first.pos = to2;
                                        bestMoves.add(from);
                                        bestMoves.add(to1);
                                        bestMoves.add(to1);
                                        bestMoves.add(to2);
                                        currMoves++;
                                        currMoves++;
                                        connect(node, second);
                                        continue REP;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            int bestDist = 1000;
            int bestFrom = -1;
            int bestTo = -1;
            int bestGroupLen = 0;
            int bestMovable = -1;
            int bestSideMove = -1;
            Node bestSide = null;
            for (int g : topGroups) {
                int c = -1;
                Arrays.fill(dist, 1000);
                for (Node node : nodes) {
                    dist[node.pos] = -2;
                }
                for (int i = 0; i < n; i++) {
                    dist[pos(i, 0)] = dist[pos(i, n - 1)] = dist[pos(0, i)] = dist[pos(n - 1, i)] = -2;
                }
                for (Node node : nodes) {
                    if (node.group != g) continue;
                    c = node.color;
                    if (head && movable(node) >= 0) dist[node.pos] = -9;
                    for (int i = 0; i < 4; i++) {
                        int p = node.pos;
                        int dp = qp[i];
                        boolean keep = node.conn(i) != null;
                        while (true) {
                            p += dp;
                            if (dist[p] <= -2) break;
                            int u = used[p];
                            if (u == c && keep) dist[p] = 999;
                            else if (u >= 0) break;
                            dist[p] = 999;
                        }
                    }
                }
                int curr = 0;
                int tot = 0;
                for (Node node : nodes) {
                    if (node.color != c || node.group == g) continue;
                    int len = groupLen[node.group];
                    if (len == 1 || (len <= 20 && node.connCount() == 1)) {
                        dist[node.pos] = 0;
                        queue[tot++] = node.pos;
                    }
                }
                int found = -1;
                int minDist = 40;
                OUT: while (curr < tot) {
                    int p = queue[curr++];
                    int d = dist[p] + 1;
                    if (d > minDist) break;
                    for (int i = 0; i < 4; i++) {
                        int np = p + qp[i];
                        int nd = dist[np];
                        if (nd == -9) {
                            if (d + 1 < minDist) {
                                found = np;
                                minDist = d + 1;
                            }
                            continue;
                        }
                        if (nd > d) {
                            if (nd == 999) {
                                found = np;
                                minDist = d;
                                break OUT;
                            }
                            dist[np] = d;
                            queue[tot++] = np;
                        }
                    }
                }
                if (found >= 0) {
                    if (groupLen[g] * (bestDist + 1) > bestGroupLen * (minDist + 1)) {
                        bestGroupLen = groupLen[g];
                        bestDist = minDist;
                        int d = minDist;
                        if (grid[found] != null) {
                            d--;
                            bestMovable = movable(grid[found]);
                            dist[bestMovable] = 1001;
                        } else bestMovable = -1;
                        bestTo = found;
                        int p = found;
                        while (d > 0) {
                            for (int i = 0; i < 4; i++) {
                                int np = p + qp[i];
                                if (dist[np] == d - 1) {
                                    d--;
                                    p = np;
                                    break;
                                }
                            }
                        }
                        bestFrom = p;
                        bestSide = null;
                        bestSideMove = -1;
                    }
                }
                for (Node node : nodes) {
                    int p = node.pos;
                    if (dist[p] != 0) continue;
                    for (int dir = 0; dir < 4; dir++) {
                        Node n2 = findNode2(p, dir);
                        if (n2 != null && groupLen[n2.group] == 1) {
                            int d1 = ortog(dir);
                            int p1 = n2.pos + qp[d1];
                            int d2 = reverse(d1);
                            int p2 = n2.pos + qp[d2];
                            if (used[p1] == -1 || used[p2] == -1) {
                                int np = n2.pos;
                                int dp = qp[dir];
                                int currDist = dist(p, n2.pos) + 1;
                                while (true) {
                                    currDist++;
                                    np += dp;
                                    if (dist[np] == 999) {
                                        if (groupLen[g] * (bestDist + 1) > bestGroupLen * (currDist + 1)) {
                                            bestGroupLen = groupLen[g];
                                            bestDist = currDist;
                                            bestTo = np;
                                            bestFrom = p;
                                            bestSide = n2;
                                            bestSideMove = used[p1] == -1 ? p1 : p2;
                                        }
                                        break;
                                    }
                                    if (grid[np] != null || used[np] >= k) break;
                                }
                            }
                        }
                    }
                }
            }
            if (bestFrom == -1) break;
            while (currConn + currMoves + bestDist + 1 > nodes.length) {
                int minConn = 20;
                int g = -1;
                for (int i = 0; i < groupLen.length; i++) {
                    int v = groupLen[i];
                    if (v > 1 && v < minConn) {
                        minConn = v;
                        g = i;
                    }
                }
                if (g == -1) break;
                if (minConn * (minConn - 1) > bestGroupLen * 2) break;
                int cnt = 0;
                for (Node node : nodes) {
                    if (node.group == g) queue1[cnt++] = node;
                }
                for (int i = 0; i < cnt; i++) {
                    disconnect(queue1[i]);
                }
            }
            if (currConn + currMoves + bestDist + 1 > nodes.length) break;
            Node before = grid[bestTo];
            if (before != null) {
                int to = bestMovable;
                disconnect(before);
                int from = before.pos;
                used[from] = -1;
                grid[from] = null;
                used[to] = before.color;
                grid[to] = before;
                before.pos = to;
                bestMoves.add(from);
                bestMoves.add(to);
                currMoves++;
                bestDist--;
            } else {
                int c = used[bestTo];
                if (c >= 0) {
                    for (int i = 0; i < 4; i++) {
                        Node n1 = findNode2(bestTo, i);
                        if (n1 != null) {
                            Node n2 = n1.conn(reverse(i));
                            if (n2 != null) {
                                disconnect(n1, n2);
                                break;
                            }
                        }
                    }
                }
            }
            if (bestSide != null) {
                bestMoves.add(bestSide.pos);
                bestMoves.add(bestSideMove);
                used[bestSide.pos] = -1;
                grid[bestSide.pos] = null;
                bestSide.pos = bestSideMove;
                used[bestSide.pos] = bestSide.color;
                grid[bestSide.pos] = bestSide;
                currMoves++;
            }
            Node node = grid[bestFrom];
            disconnect(node);
            used[bestFrom] = -1;
            grid[bestFrom] = null;
            used[bestTo] = node.color;
            grid[bestTo] = node;
            node.pos = bestTo;
            Arrays.fill(dist, 1000);
            int curr = 0;
            int tot = 1;
            queue[0] = bestTo;
            dist[bestTo] = 0;
            OUT: while (curr < tot) {
                int p = queue[curr++];
                int d = dist[p] + 1;
                for (int i = 0; i < 4; i++) {
                    int np = p + qp[i];
                    if (grid[np] != null || used[np] >= k) continue;
                    if (dist[np] <= d) continue;
                    dist[np] = d;
                    queue[tot++] = np;
                    if (np == bestFrom) break OUT;
                }
            }
            int p = bestFrom;
            int d = dist[p];
            while (p != bestTo) {
                for (int i = 0; i < 4; i++) {
                    int np = p + qp[i];
                    if (dist[np] == d - 1) {
                        bestMoves.add(p);
                        bestMoves.add(np);
                        d--;
                        p = np;
                        currMoves++;
                        break;
                    }
                }
            }
            for (int loop = 0; loop < 2; loop++) {
                Node n1 = loop == 0 ? before : node;
                if (n1 == null) continue;
                while (currConn + currMoves < nodes.length) {
                    Node cc = null;
                    int max = -1;
                    for (int i = 0; i < 4; i++) {
                        Node n2 = findNode(n1, i);
                        if (n2 != null) {
                            if (groupLen[n2.group] > max) {
                                max = groupLen[n2.group];
                                cc = n2;
                            }
                        }
                    }
                    if (cc == null) break;
                    connect(n1, cc);
                }
            }
        }
        badColor1 = badColor2 = badColor3 = -1;
        OUT: while (currConn + currMoves < nodes.length) {
            boolean imp = false;
            for (Node node : nodes) {
                for (int i = 0; i < 4; i++) {
                    if (node.conn(i) != null) continue;
                    Node n2 = findNode(node, i);
                    if (n2 != null) {
                        if (connect(node, n2)) {
                            imp = true;
                            if (currConn + currMoves == nodes.length) break OUT;
                        }
                    }
                }
            }
            if (!imp) break;
        }
        if (currScore <= prevScore) bestMoves.subList(prevMoves, bestMoves.size()).clear();
        else {
            for (int i = 0; i < remMoves.size(); i += 2) {
                int i1 = remMoves.get(i);
                int i2 = remMoves.get(i + 1);
                for (int j = 0; j < bestMoves.size(); j += 2) {
                    if (i1 == bestMoves.get(j) && i2 == bestMoves.get(j + 1)) {
                        bestMoves.subList(j, j + 2).clear();
                        break;
                    }
                }
            }
            store();
        }
        return bestMoves;
    }

    private static void reset() {
        Arrays.fill(grid, null);
        for (int y = 1; y < n - 1; y++) {
            for (int x = 1; x < n - 1; x++) {
                used[pos(x, y)] = -1;
            }
        }
        Arrays.fill(groupLen, 1);
        numFreeGroups = 0;
        maxScore = currMoves = currConn = currScore = 0;
        for (int i = 0; i < nodes.length; i++) {
            Node node = nodes[i];
            node.pos = node.orgPos;
            node.group = i;
            used[node.pos] = node.color;
            grid[node.pos] = node;
            node.clearConn();
        }
    }

    private static int movable(Node node) {
        int p = node.pos;
        for (int i = 0; i < 4; i++) {
            int np = p + qp[i];
            if (grid[np] == null && (used[np] == -1 || node.conn(i) != null)) return np;
        }
        return -1;
    }

    private static void rebuild() {
        Arrays.fill(grid, null);
        for (int y = 1; y < n - 1; y++) {
            for (int x = 1; x < n - 1; x++) {
                used[pos(x, y)] = -1;
            }
        }
        Arrays.fill(groupLen, 1);
        numFreeGroups = 0;
        currMoves = currConn = currScore = 0;
        for (int i = 0; i < nodes.length; i++) {
            Node node = nodes[i];
            node.pos = node.bestPos;
            currMoves += node.pos == node.orgPos ? 0 : 1;
            node.group = i;
            used[node.pos] = node.color;
            grid[node.pos] = node;
            node.clearConn();
        }
        for (int i = 0; i < bestConn.size(); i += 2) {
            int a = bestConn.get(i);
            int b = bestConn.get(i + 1);
            connect(grid[a], grid[b]);
        }
    }

    private static void solve(long timeout) {
        int alfa = 10;
        double t0 = 540 - 70 * k;
        double temp = t0;
        int cycles = 0;
        double duration = timeout - System.currentTimeMillis();
        while (true) {
            if ((++cycles & 255) == 0) {
                long time = System.currentTimeMillis();
                if (time >= timeout) break;
                double rem = (timeout - time) / duration;
                if (badColor1 != -1 && rem < 0.2) badColor1 = -1;
                temp = t0 * rem;
                ord2[0] = rnd.nextInt(2);
                ord2[1] = 1 - ord2[0];
            }
            Node node1 = nodes[cycles % nodes.length];
            int prevScore = currScore;
            int prevMoves = currMoves;
            int prevConn = currConn;
            int mode = rnd.nextInt(3);
            if (mode != 0) {
                int p1 = node1.pos;
                int p2 = node1.orgPos;
                Node n3 = null;
                int n3p = -1;
                if (p1 == p2) {
                    if (currMoves + currConn == nodes.length) mode = 0;
                    else p2 = p1 + qp[rnd.nextInt(4)];
                } else if (mode == 2) {
                    p2 = node1.orgPos + qp[rnd.nextInt(4)];
                    if (p2 == p1) p2 = node1.orgPos;
                    n3 = grid[node1.orgPos];
                    if (n3 != null && (groupLen[n3.group] != 1 || grid[n3.orgPos] != null || used[n3.orgPos] != -1)) continue;
                }
                if (mode != 0) {
                    if (n3 != null) {
                        used[n3p = n3.pos] = -1;
                        grid[n3.pos] = null;
                        n3.pos = n3.orgPos;
                        used[n3.pos] = n3.color;
                        grid[n3.pos] = n3;
                        currMoves--;
                    }
                    if (grid[p2] != null) continue;
                    int u2 = used[p2];
                    if (u2 >= 0 && u2 != node1.color) continue;
                    int a1 = rnd.nextInt(16);
                    int a2 = a1 & 3;
                    a1 >>>= 2;
                    int oo = ord[a1];
                    ord[a1] = ord[a2];
                    ord[a2] = oo;
                    saveConn0 = node1.conn0;
                    saveConn1 = node1.conn1;
                    saveConn2 = node1.conn2;
                    saveConn3 = node1.conn3;
                    Node[] changed = move(node1, p2, false, null);
                    if (changed == null) continue;
                    double gain = (currScore - prevScore) * alfa + prevMoves - currMoves + prevConn - currConn;
                    if (gain < 0 && gain < log[rnd.nextInt(log.length)] * temp) {
                        move(node1, p1, true, changed);
                        if (n3 != null) {
                            used[n3.orgPos] = -1;
                            grid[n3.orgPos] = null;
                            n3.pos = n3p;
                            used[n3.pos] = n3.color;
                            grid[n3.pos] = n3;
                            currMoves++;
                        }
                    }
                }
            }
            if (mode == 0) {
                if (node1.color == badColor1 || node1.color == badColor2 || node1.color == badColor3) continue;
                int dir = rnd.nextInt(4);
                Node node2 = node1.conn(dir);
                if (node2 == null) {
                    if (currMoves + currConn < nodes.length) {
                        node2 = findNode(node1, dir);
                        if (node2 != null) connect(node1, node2, dir);
                    }
                } else {
                    disconnect(node1, node2, dir);
                    int numConn = connectRange(node1.pos, node2.pos, dir);
                    int gain = (currScore - prevScore) * alfa + prevConn - currConn;
                    if (gain < 0 && gain < log[rnd.nextInt(log.length)] * temp) {
                        for (int i = 0; i < numConn; i += 2) {
                            disconnect(cr[i], cr[i + 1]);
                        }
                        connect(node1, node2, dir);
                    }
                }
            }

            if (currScore > maxScore) {
                maxScore = currScore;
                store();
            }
        }
    }

    private static Node[] move(Node node, int to, boolean reconn, Node[] changed) {
        int from = node.pos;
        int add = (to == node.orgPos ? 0 : 1) - (from == node.orgPos ? 0 : 1);
        if (currMoves + currConn + add - node.connCount() > nodes.length) return null;
        currMoves += add;
        Node[] ret = new Node[4];
        if (changed != null && changed[2] != null) disconnect(changed[2], changed[3]);
        disconnect(node);
        if (changed == null && used[to] >= 0) {
            for (int dir : ord2) {
                Node na = findNode2(to, dir);
                if (na != null) {
                    Node nb = na.conn(reverse(dir));
                    if (nb != null) {
                        ret[0] = na;
                        ret[1] = nb;
                        disconnect(na, nb);
                        break;
                    }
                }
            }
        }
        used[node.pos] = -1;
        grid[node.pos] = null;
        node.pos = to;
        used[node.pos] = node.color;
        grid[node.pos] = node;
        Node conn0 = null;
        Node conn1 = null;
        Node conn2 = null;
        Node conn3 = null;
        if (node.color != badColor1 && node.color != badColor2 && node.color != badColor3) {
            if (!reconn) {
                conn0 = findNode(node, 0);
                conn1 = findNode(node, 1);
                conn2 = findNode(node, 2);
                conn3 = findNode(node, 3);
            } else {
                conn0 = saveConn0;
                conn1 = saveConn1;
                conn2 = saveConn2;
                conn3 = saveConn3;
            }
        }
        if (changed != null && changed[0] != null) connect(changed[0], changed[1]);
        if (node.color != badColor1 && node.color != badColor2 && node.color != badColor3) {
            for (int dir : ord) {
                Node o = dir == 0 ? conn0 : dir == 1 ? conn1 : dir == 2 ? conn2 : conn3;
                if (o != null && o.group != node.group && currMoves + currConn < nodes.length) connect(node, o, dir);
            }
        }
        if (changed == null && currMoves + currConn < nodes.length && used[from] < 0) {
            for (int dir : ord2) {
                Node na = findNode(from, dir);
                if (na != null && na.color != badColor1 && na.color != badColor2 && na.color != badColor3) {
                    Node nb = findNode(from, reverse(dir));
                    if (nb != null && na.color == nb.color && na.group != nb.group) {
                        ret[2] = na;
                        ret[3] = nb;
                        connect(na, nb);
                        break;
                    }
                }
            }
        }
        return ret;
    }

    private static int connectRange(int p1, int p2, int dir0) {
        int dp = qp[dir0];
        int dir1 = ortog(dir0);
        int dir2 = reverse(dir1);
        int cnt = 0;
        while (true) {
            if ((p1 += dp) == p2) break;
            Node na = findNode(p1, dir1);
            if (na == null) continue;
            if (na.color == badColor1 || na.color == badColor2 || na.color == badColor3) continue;
            Node nb = findNode(p1, dir2);
            if (nb == null) continue;
            if (na.color == nb.color && na.group != nb.group && rnd.nextInt(5) != 0) {
                connect(cr[cnt++] = na, cr[cnt++] = nb, dir2);
                if (currMoves + currConn == nodes.length) break;
            }
        }
        return cnt;
    }

    private static void disconnect(Node node) {
        if (node.conn0 != null) disconnect(node, node.conn0, 0);
        if (node.conn1 != null) disconnect(node, node.conn1, 1);
        if (node.conn2 != null) disconnect(node, node.conn2, 2);
        if (node.conn3 != null) disconnect(node, node.conn3, 3);
    }

    private static void disconnect(Node node1, Node node2) {
        disconnect(node1, node2, dir(node1, node2));
    }

    private static void disconnect(Node node1, Node node2, int dir) {
        if (node1.color == badColor1 || node1.color == badColor2 || node1.color == badColor3) return;
        currConn--;
        switch (dir) {
        case 0:
            node1.conn0 = node2.conn2 = null;
            break;
        case 1:
            node1.conn1 = node2.conn3 = null;
            break;
        case 2:
            node1.conn2 = node2.conn0 = null;
            break;
        default:
            node1.conn3 = node2.conn1 = null;
        }
        int p = node1.pos;
        int dp = qp[dir];
        while ((p += dp) != node2.pos) {
            used[p] = -1;
        }
        int ng = freeGroups[--numFreeGroups];
        Node lg = largest[key(node1, node2)];
        if (lg == node2) {
            int l2 = groupLen[node2.group];
            int l1 = groupLen[ng] = fill(node1, ng);
            currScore += score(l2 - l1) + score(l1) - score(l2);
            groupLen[node2.group] -= l1;
        } else {
            int l1 = groupLen[node1.group];
            int l2 = groupLen[ng] = fill(node2, ng);
            currScore += score(l1 - l2) + score(l2) - score(l1);
            groupLen[node1.group] -= l2;
        }
    }

    private static int fill(Node node, int g) {
        int curr = 0;
        int tot = 1;
        (queue[0] = node).group = g;
        while (curr < tot) {
            if ((node = queue[curr++]).conn0 != null && node.conn0.group != g) (queue[tot++] = node.conn0).group = g;
            if (node.conn1 != null && node.conn1.group != g) (queue[tot++] = node.conn1).group = g;
            if (node.conn2 != null && node.conn2.group != g) (queue[tot++] = node.conn2).group = g;
            if (node.conn3 != null && node.conn3.group != g) (queue[tot++] = node.conn3).group = g;
        }
        return groupLen[g] = tot;
    }

    private static boolean connect(Node node1, Node node2) {
        return connect(node1, node2, dir(node1, node2));
    }

    private static boolean connect(Node node1, Node node2, int dir) {
        if (node1.color == badColor1 || node1.color == badColor2 || node1.color == badColor3) return false;
        int l1 = groupLen[node1.group];
        int l2 = groupLen[node2.group];
        currConn++;
        currScore += score(l1 + l2) - score(l1) - score(l2);
        switch (dir) {
        case 0:
            node1.conn0 = node2;
            node2.conn2 = node1;
            break;
        case 1:
            node1.conn1 = node2;
            node2.conn3 = node1;
            break;
        case 2:
            node1.conn2 = node2;
            node2.conn0 = node1;
            break;
        default:
            node1.conn3 = node2;
            node2.conn1 = node1;
        }
        int p = node1.pos;
        int dp = qp[dir];
        while ((p += dp) != node2.pos) {
            used[p] = node1.color;
        }
        int g1 = node1.group;
        int g2 = node2.group;
        if (l1 >= l2) {
            largest[key(node1, node2)] = node1;
            fill(node2, g1);
            groupLen[g1] = l1 + l2;
            groupLen[freeGroups[numFreeGroups++] = g2] = 0;
        } else {
            largest[key(node1, node2)] = node2;
            fill(node1, g2);
            groupLen[g2] = l1 + l2;
            groupLen[freeGroups[numFreeGroups++] = g1] = 0;
        }
        return true;
    }

    private static Node findNode(Node node1, int dir) {
        if (node1.conn(dir) != null) return null;
        int dp = qp[dir];
        int p = node1.pos;
        while (true) {
            Node node2 = grid[p += dp];
            if (node2 != null) return node2.color != node1.color || node2.group == node1.group ? null : node2;
            if (used[p] >= 0) return null;
        }
    }

    private static Node findNode(int p, int dir) {
        int dp = qp[dir];
        while (true) {
            Node node2 = grid[p += dp];
            if (node2 != null) return node2;
            if (used[p] >= 0) return null;
        }
    }

    private static Node findNode2(int p, int dir) {
        int dp = qp[dir];
        while (true) {
            Node node2 = grid[p += dp];
            if (node2 != null) return node2;
            if (used[p] >= k) return null;
        }
    }

    private static void store() {
        for (Node node : nodes) {
            node.bestPos = node.pos;
        }
        bestConn.clear();
        for (Node node1 : nodes) {
            for (int i = 0; i < 4; i++) {
                Node node2 = node1.conn(i);
                if (node2 != null && node1.pos < node2.pos) {
                    bestConn.add(node1.pos);
                    bestConn.add(node2.pos);
                }
            }
        }
    }

    private static String mount(List<Integer> bestMoves) {
        StringBuilder sb = new StringBuilder();
        for (int step = 0; step < 2; step++) {
            List<Integer> l = step == 0 ? bestMoves : bestConn;
            sb.append(l.size() / 2).append(System.lineSeparator());
            for (int i = 0; i < l.size(); i += 2) {
                int a = l.get(i);
                int b = l.get(i + 1);
                sb.append(y(a) - 1).append(' ');
                sb.append(x(a) - 1).append(' ');
                sb.append(y(b) - 1).append(' ');
                sb.append(x(b) - 1).append(System.lineSeparator());
            }
        }
        return sb.toString();
    }

    private static void init() throws Exception {
        for (int i = 0; i < log.length; i++) {
            log[i] = Math.log((i + 0.5) / log.length);
        }
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        String[] s = br.readLine().split(" ");
        n = Integer.parseInt(s[0]);
        k = Integer.parseInt(s[1]);
        nodes = new Node[k * 100];
        groupLen = new int[k * 100];
        freeGroups = new int[k * 100];
        numFreeGroups = 0;
        Arrays.fill(groupLen, 1);
        Arrays.fill(used, -1);
        int numNodes = 0;
        for (int y = 0; y < n; y++) {
            String line = br.readLine();
            for (int x = 0; x < n; x++) {
                int color = line.charAt(x) - '0' - 1;
                if (color >= 0) {
                    int p = pos(x + 1, y + 1);
                    Node node = nodes[numNodes] = grid[p] = new Node(color, p);
                    used[p] = node.color;
                    node.group = numNodes;
                    numNodes++;
                }
            }
        }
        br.close();
        n += 2;
        for (int i = 0; i < n; i++) {
            used[pos(i, 0)] = used[pos(i, n - 1)] = used[pos(0, i)] = used[pos(n - 1, i)] = k;
        }
    }

    private static int pos(int x, int y) {
        return (y << 6) | x;
    }

    private static int x(int p) {
        return p & 63;
    }

    private static int y(int p) {
        return p >>> 6;
    }

    private static int score(int len) {
        return (len * (len - 1)) >>> 1;
    }

    private static int dir(Node n1, Node n2) {
        return dir(n1.pos, n2.pos);
    }

    private static int dist(int p1, int p2) {
        return Math.abs(x(p1) - x(p2)) + Math.abs(y(p1) - y(p2));
    }

    private static int key(Node a, Node b) {
        return a.id > b.id ? (b.id << 9) | a.id : (a.id << 9) | b.id;
    }

    private static int dir(int p1, int p2) {
        int d = p2 - p1;
        if (d >= 64) return 1;
        if (d >= 1) return 0;
        if (d <= -64) return 3;
        return 2;
    }

    private static int ortog(int dir) {
        return (dir + 1) & 3;
    }

    private static int reverse(int dir) {
        return (dir + 2) & 3;
    }

    static class Node {
        private static int sid = 0;
        final int color, orgPos, id;
        int pos, group, bestPos;
        Node conn0, conn1, conn2, conn3;

        Node(int color, int pos) {
            this.color = color;
            this.orgPos = this.pos = pos;
            id = sid++;
        }

        Node conn(int idx) {
            switch (idx) {
            case 0:
                return conn0;
            case 1:
                return conn1;
            case 2:
                return conn2;
            }
            return conn3;
        }

        void clearConn() {
            conn0 = conn1 = conn2 = conn3 = null;
        }

        int connCount() {
            int cnt = 0;
            if (conn0 != null) cnt++;
            if (conn1 != null) cnt++;
            if (conn2 != null) cnt++;
            if (conn3 != null) cnt++;
            return cnt;
        }
    }
}
