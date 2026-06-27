using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Diagnostics;
using System.ComponentModel.DataAnnotations;
using System.Text.Json.Serialization;
using System.Runtime.CompilerServices;
using System.Reflection.Metadata.Ecma335;
using System.Net.Http.Headers;
using System.Xml.Schema;
using System.Globalization;
using System.ComponentModel;

public class Myon
{

    bool DEBUG = true;

    public Myon() { }
    public static int Main()
    {
        new Myon().calc();
        return 0;
    }

    Scanner cin;
    XRand rnd;
    Stopwatch sw;
    int LIMIT = 1900;
    int LIMIT2 = 1900;



    void calc()
    {
        cin = new Scanner();
        rnd = new XRand((uint)DateTime.Now.Millisecond);
        sw = new Stopwatch();
        input();
        sw.Start();
        init();
        solve2();
    }


    int N, M;
    List<(int y, int x)>[] Pos;
    int[][] A;
    int ALPHA = 26;
    int[,] board;

    int sy, sx;


    void input()
    {
        N = cin.nextInt();
        M = cin.nextInt();
        sy = cin.nextInt();
        sx = cin.nextInt();

        Pos = new List<(int y, int x)>[ALPHA];
        for (int i = 0; i < ALPHA; i++)
        {
            Pos[i] = new List<(int y, int x)>();
        }

        board = new int[N, N];
        for (int y = 0; y < N; y++)
        {
            string S = cin.next();
            for (int x = 0; x < N; x++)
            {
                board[y, x] = (int)(S[x] - 'A');
                Pos[board[y, x]].Add((y, x));
            }
        }
        A = new int[M][];
        for (int i = 0; i < M; i++)
        {
            string S = cin.next();
            A[i] = new int[5];
            for (int j = 0; j < 5; j++)
            {
                A[i][j] = (int)(S[j] - 'A');
            }
        }    }

    (int cost, (int y, int x)[] Route) calcByArray(int[] nums, int target = 1)
    {
        int[][] B = new int[M][];
        for (int i = 0; i < M; i++)
        {
            B[i] = A[nums[i]];
        }

        for (int c = 4; c >= 1; c--)
        {
            for (int i = 0; i < M; i++)
            {
                if (B[i].Length == 0) continue;
                for (int jj = 0; jj < M; jj++)
                {
                    int j = M - 1 - jj; 
                    if (i == j) continue;
                    if (B[j].Length == 0) continue;
                    if (CheckConnect(B[i], B[j], c))
                    {
                        B[i] = Connect(B[i], B[j], c);
                        B[j] = new int[0];
                    }
                }
            }
        }
        int[] ar = new int[0];
        int cnt = 0;

        List<int[]> ars = new List<int[]>();

        for (int i = 0; i < M; i++)
        {
            if (B[i].Length == 0) continue;
            cnt++;
            ars.Add(B[i]);
            ar = Connect(ar, B[i], 0);
        }

        var ret = CalcByDP((sy, sx), ar);

        if(target != 1)
        {
            int[][] ars2 = ars.ToArray();
            while(sw.ElapsedMilliseconds < LIMIT2)
            {
                randomshuffle(ars2);
                ar = new int[0];
                foreach (var item in ars2)
                {
                    ar = Connect(ar, item, 0);
                }

                var r2 = CalcByDP((sy, sx), ar);
                if (r2.cost < ret.cost)
                {
                    ret = r2;
                }
            }
        }

        return ret;
    }


    int MAXNUM = 7;
    int[] getArrayByBitDP(int[][] ar)
    {
        Queue<int[]> q = new Queue<int[]>();
        foreach (var item in ar)
        {
            q.Enqueue(item);
        }

        while(q.Count > 1)
        {
            List<int[]> l = new List<int[]>();
            while(l.Count < MAXNUM && q.Count > 0)
            {
                l.Add(q.Dequeue());
            }
            q.Enqueue(getArrayByBitDP2(l.ToArray()));
        }
        return q.Dequeue();

    }
    int[] getArrayByBitDP2(int[][] ar)
    {
        int N = ar.Length;
        int[][,] table = new int[N][,];
        for (int i = 0; i < N; i++)
        {
            table[i] = getTable(ar[i]);
        }

        int[] startPos = new int[N + 1];
        int startCnt = 0;
        int[] endPos = new int[N + 1];
        int endCnt = 0;

        List<(int y, int x)> spos = new List<(int y, int x)>();
        List<(int y, int x)> epos = new List<(int y, int x)>();
        List<int> sid = new List<int>();
        List<int> eid = new List<int>();

        for (int i = 0; i < N; i++)
        {
            startPos[i] = startCnt;
            for (int j = 0; j < Pos[ar[i][0]].Count; j++)
            {
                spos.Add(Pos[ar[i][0]][j]);
                sid.Add(i);
                startCnt++;
            }

            endPos[i] = endCnt;
            for (int j = 0; j < Pos[ar[i][ar[i].Length - 1]].Count; j++)
            {
                epos.Add(Pos[ar[i][ar[i].Length - 1]][j]);
                eid.Add(i);
                endCnt++;
            }
        }
        startPos[N] = startCnt;
        endPos[N] = endCnt;




        int[,] dps = new int[1 << N, startCnt];
        int[,] pres = new int[1 << N, startCnt];
        int[,] dpe = new int[1 << N, endCnt];
        int[,] pree = new int[1 << N, endCnt];


        for (int i = 0; i < (1 << N); i++)
        {
            for (int j = 0; j < startCnt; j++)
            {
                dps[i, j] = MAX;
                pres[i, j] = -1;
            }
            for (int j = 0; j < endCnt; j++)
            {
                dpe[i, j] = MAX;
                pree[i, j] = -1;
            }
        }


        for (int t = 1; t < (1 << N); t++)
        {
            for (int i = 0; i < startCnt; i++)
            {
                int startId = sid[i];
                if ((t & (1 << startId)) == 0) continue;
                int preId = (t - (1 << startId));
                if (preId == 0)
                {
                    dps[t, i] = 0;
                    pres[t, i] = -1;
                }
                else
                {
                    for (int j = 0; j < endCnt; j++)
                    {
                        int endId = eid[j];
                        if ((preId & (1 << endId)) == 0) continue;
                        int endP = j - endPos[endId];
                        int tmpCost = dpe[preId, j] + getDist(epos[j], spos[i]);
                        if(tmpCost < dps[t, i])
                        {
                            dps[t, i] = tmpCost;
                            pres[t, i] = j;
                        }

                    }
                    if (dps[t, i] == MAX) continue;
                }
                

                for (int j = 0; j < table[startId].GetLength(1); j++)
                {
                    int si = i - startPos[startId];
                    int cost = dps[t, i] + table[startId][si, j];
                    if (dpe[t, endPos[startId] + j] > cost)
                    {
                        dpe[t, endPos[startId] + j] = cost;
                        pree[t, endPos[startId] + j] = i;
                    }
                }
            }
        }

        int bestEnd = 0;
        for (int i = 1; i < endCnt; i++)
        {
            if(dpe[(1<<N)-1, bestEnd] > dpe[(1<<N)-1, i])
            {
                bestEnd = i;
            }
        }

        List<int> route = new List<int>();
        int nowBit = (1 << N) - 1;
        int nowE = bestEnd;
        while (true)
        {
            int now = eid[nowE];
            route.Add(now);
            int nowS = pree[nowBit, nowE];
            
            int nextBit = nowBit - (1 << now);
            if (nextBit == 0) break;

            int nextE = pres[nowBit, nowS];

            nowBit = nextBit;
            nowE = nextE;
        }
        route.Reverse();

        int[] ans = new int[0];
        for (int i = 0; i < route.Count; i++)
        {
            ans = Connect(ans, ar[i], 0);
        }
        return ans;
    }

    int[,] ConnectPower;
    int[,] ConnectNum;



    void init()
    {
        sw.Start();

        ConnectPower = new int[M, M];
        ConnectNum= new int[M, M];

        int[] preCost = new int[M];
        for (int i = 0; i < M; i++)
        {
            preCost[i] = CalcByDP((-1, -1), A[i]).cost;
        }

        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < M; j++)
            {
                if(i == j)
                {
                    ConnectPower[i, j] = -99999999;
                    continue;
                }

                for (int k = 4; k >= 0; k--)
                {
                    if (CheckConnect(A[i], A[j], k))
                    {
                        ConnectPower[i, j] = k * 10000;
                        ConnectNum[i, j] = k;
                        int a = CalcByDP((-1, -1), Connect(A[i], A[j], k)).cost;
                        ConnectPower[i, j] = (preCost[i] + preCost[j] - a) * 10000;

                        break;
                    }
                }
            }
        }

    }

    void solve2()
    {
        int[,] bestConnect = ConnectPower;
        var FirstAns = calcByConnectPower(bestConnect);
        (int y, int x)[] bestRoute = FirstAns.Route;
        int bestCost = FirstAns.cost;


        Console.Error.WriteLine("First: " + bestCost);


        Console.Error.WriteLine("Phase 1");
        while (sw.ElapsedMilliseconds < LIMIT)
        {
            int MOVE = (int)(1000 * (LIMIT - sw.ElapsedMilliseconds) / LIMIT + 5);

            int rate = (int)(LIMIT - sw.ElapsedMilliseconds) + 1;

            int[,] Connect = (int[,])bestConnect.Clone();
            for (int i = 0; i < M; i++)
            {
                for (int j = 0; j < M; j++)
                {
                    if (rnd.nextInt(LIMIT) < rate)
                    {
                        Connect[i, j] += (rnd.nextInt(MOVE * 2 + 1) - MOVE);
                    }
                }
            }


            var ret = calcByConnectPower(Connect);
            if (ret.cost < bestCost)
            {
                bestCost = ret.cost;
                bestRoute = ret.Route;
                bestConnect = Connect;
                Console.Error.WriteLine($"now:{bestCost} time:{sw.ElapsedMilliseconds}");
            }
        }

        int nextCONNECT = 100000000;


        Console.Error.WriteLine("Phase 2");
        while (sw.ElapsedMilliseconds < LIMIT)
        {
            int MOVE = 1000;// (int)(1000 * (LIMIT - sw.ElapsedMilliseconds) / LIMIT + 5);

            int rate = (int)(LIMIT - sw.ElapsedMilliseconds) + 1;

            int[,] Connect = (int[,])bestConnect.Clone();
            int a = rnd.nextInt(M);
            int b = (a + rnd.nextInt(M - 1)) % M;
            Connect[a, b] = nextCONNECT++;

            var ret = calcByConnectPower(Connect);
            if (ret.cost < bestCost)
            {
                bestCost = ret.cost;
                bestRoute = ret.Route;
                bestConnect = Connect;
                Console.Error.WriteLine($"now:{bestCost} time:{sw.ElapsedMilliseconds}");
            }
        }

        foreach (var item in bestRoute)
        {
            Console.WriteLine($"{item.y} {item.x}");
        }

        Console.Error.WriteLine($"Score: {bestCost}");

    }





    (int cost, (int y, int x)[] Route) calcByConnectPower(int[,] connect)
    {
        List<(int d, int a, int b)> l = new List<(int d, int a, int b)>();
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < M; j++)
            {
                if (i == j) continue;
                l.Add((connect[i, j], i, j));
            }
        }
        l.Sort((a, b) => -a.d.CompareTo(b.d));

        int[] pos = new int[M];
        int[] top = new int[M];
        int[] tail = new int[M];
        bool[] useTop = new bool[M];
        bool[] useTail = new bool[M];

        for (int i = 0; i < M; i++)
        {
            pos[i] = top[i] = tail[i] = i;
        }

        int[][] B = new int[M][];
        for (int i = 0; i < M; i++)
        {
            B[i] = (int[])A[i].Clone();
        }

        foreach (var item in l)
        {
            int a = item.a;
            int b = item.b;
            if (useTail[a]) continue;
            if (useTop[b]) continue;

            int pa = pos[a];
            int pb = pos[b];
            if (pa == pb) continue;

            useTail[a] = true;
            useTop[b] = true;

            tail[pa] = tail[pb];
            pos[tail[pa]] = pa;
            B[pa] = Connect(B[pa], B[pb], ConnectNum[a, b]);
            B[pb] = new int[0];
        }

        int[] ar = new int[0];
        for (int i = 0; i < M; i++)
        {
            if (B[i].Length == 0) continue;
            ar = Connect(ar, B[i], 0);
            //Console.WriteLine(B[i].Length);
        }
        //Console.WriteLine(cnt);

        return CalcByDP((sy, sx), ar);

    }

    
    
    void solve()
    {

        int[] num = new int[M];
        for (int i = 0; i < M; i++)
        {
            num[i] = i;
        }

        var FirstAns = calcByArray(num);
        int bestCost = FirstAns.cost;
        (int y, int x)[] bestRoute = FirstAns.Route;
        int[] bestTarget = (int[])num.Clone();

        Console.Error.WriteLine("First: " + bestCost);

        Console.Error.WriteLine("Phase 1");
        while (sw.ElapsedMilliseconds < LIMIT / 8)
        {
            int[] target = (int[])num.Clone();
            randomshuffle(target);
            var ret = calcByArray(target);
            //Console.WriteLine($"score: {ret.cost}");
            if (ret.cost < bestCost)
            {
                bestCost = ret.cost;
                bestRoute = ret.Route;
                bestTarget = (int[])target.Clone();
                Console.Error.WriteLine($"now:{bestCost} time:{sw.ElapsedMilliseconds}");
            }
        }

        Console.Error.WriteLine("Phase 2");
        while (sw.ElapsedMilliseconds < LIMIT / 2)
        {
            int[] target = (int[])bestTarget.Clone();
            for (int i = 0; i < 3; i++)
            {
                int a = rnd.nextInt(M);
                int b = (a + rnd.nextInt(M - 1)) % M;
                swap(ref target[a], ref target[b]);
            }

            var ret = calcByArray(target);
            if (ret.cost < bestCost)
            {
                bestCost = ret.cost;
                bestRoute = ret.Route;
                bestTarget = target;
                Console.Error.WriteLine($"now:{bestCost} time:{sw.ElapsedMilliseconds}");
            }
        }

        Console.Error.WriteLine("Phase 3");
        while (sw.ElapsedMilliseconds < LIMIT)
        {
            int[] target = (int[])bestTarget.Clone();
            for (int i = 0; i < 1; i++)
            {
                int a = rnd.nextInt(M);
                int b = (a + rnd.nextInt(M - 1)) % M;
                swap(ref target[a], ref target[b]);
            }

            var ret = calcByArray(target);
            if (ret.cost < bestCost)
            {
                bestCost = ret.cost;
                bestRoute = ret.Route;
                bestTarget = target;
                Console.Error.WriteLine($"now:{bestCost} time:{sw.ElapsedMilliseconds}");
            }
        }


        /*
        Console.Error.WriteLine("Phase 4");
        {
            int[] target = (int[])bestTarget.Clone();


            var ret = calcByArray(target, 100);
            if (ret.cost < bestCost)
            {
                bestCost = ret.cost;
                bestRoute = ret.Route;
                bestTarget = target;
                Console.Error.WriteLine($"now:{bestCost} time:{sw.ElapsedMilliseconds}");
            }
        }
        */


        foreach (var item in bestRoute)
        {
            Console.WriteLine($"{item.y} {item.x}");
        }

        Console.Error.WriteLine($"Score: {bestCost}");
    }


    bool CheckConnect(int[] A, int[] B, int c)
    {
        for (int i = 0; i < c; i++)
        {
            if (A[A.Length - c + i] != B[i]) return false;
        }
        return true;
    }

    int[] Connect(int[] A, int[] B, int c)
    {
        int[] ret = new int[A.Length + B.Length - c];
        A.CopyTo(ret, 0);
        B.CopyTo(ret, A.Length - c);

        
        return ret;
    }

    int MAX = int.MaxValue / 4;

    (int cost, (int y, int x)[] Route) CalcByDP((int y, int x) start, int[] A)
    {
        int L = A.Length + 1;
        int[][] dp, pre;

        var re = subDP(start, A);
        dp = re.dp;
        pre = re.pre;        

        int best = 0;
        for (int j = 1; j < dp[L - 1].Length; j++)
        {
            if (dp[L - 1][best] > dp[L - 1][j])
            {
                best = j;
            }
        }

        int nowj = best;
        int nowi = L - 1;
        int ret = dp[L - 1][best];
        List<(int y, int x)> ans = new List<(int y, int x)>(); 
        while(nowi > 0)
        {
            ans.Add(Pos[A[nowi - 1]][nowj]);
            nowj = pre[nowi][nowj];
            nowi--;
        }
        ans.Reverse();
        return (ret, ans.ToArray());
    }

    (int[][] dp, int[][] pre) subDP((int y, int x) start, int[] A)
    {
        int L = A.Length + 1;
        int[][] dp = new int[L][];
        int[][] pre = new int[L][];
        dp[0] = new int[1];
        dp[0][0] = 0;
        pre[0] = new int[1];
        pre[0][0] = 0;
        for (int i = 0; i < A.Length; i++)
        {
            dp[i + 1] = new int[Pos[A[i]].Count];
            pre[i + 1] = new int[Pos[A[i]].Count];
            for (int j = 0; j < dp[i + 1].Length; j++)
            {
                dp[i + 1][j] = MAX;
                pre[i + 1][j] = -1;
            }
        }

        for (int i = 0; i < A.Length; i++)
        {
            for (int j = 0; j < dp[i].Length; j++)
            {
                for (int k = 0; k < dp[i + 1].Length; k++)
                {
                    int next;
                    if (i == 0)
                    {
                        if (start.y != -1)
                        {
                            next = getDist(start, Pos[A[i]][k]) + 1;
                        }
                        else
                        {
                            next = 1;
                        }
                    }
                    else
                    {
                        next = dp[i][j] + getDist(Pos[A[i - 1]][j], Pos[A[i]][k]) + 1;
                    }
                    if (next < dp[i + 1][k])
                    {
                        dp[i + 1][k] = next;
                        pre[i + 1][k] = j;
                    }
                }
            }
        }
        return (dp, pre);
    }

    int[,] getTable(int[] ar)
    {
        int[,] ret = new int[Pos[ar[0]].Count, Pos[ar[ar.Length - 1]].Count];
        int[] br = new int[ar.Length - 1];
        for (int i = 0; i < ar.Length - 1; i++)
        {
            br[i] = ar[i + 1];
        }

        for (int i = 0; i < ret.GetLength(0); i++)
        {
            var re = subDP(Pos[ar[0]][i], br);
            for (int j = 0; j < ret.GetLength(1); j++)
            {
                ret[i, j] = re.dp[br.Length][j];
            }
        }
        return ret;
    }




    int getDist((int y, int x) a , (int y, int x) b)
    {
        return Math.Abs(a.y - b.y) + Math.Abs(a.x - b.x);
    }

    //配列のランダム並び替え
    void randomshuffle<T>(T[] a)
    {
        int len = a.Length;
        for (int i = 0; i < len - 1; i++)
        {
            swap(ref a[i], ref a[i + rnd.nextInt(len - i)]);
        }
    }

    //swap
    void swap<T>(ref T a, ref T b)
    {
        T c = a;
        a = b;
        b = c;
    } 

}

class Scanner
{
    string[] s;
    int i;

    char[] cs = new char[] { ' ' };

    public Scanner()
    {
        s = new string[0];
        i = 0;
    }

    public string next()
    {
        if (i < s.Length) return s[i++];
        string st = Console.ReadLine();
        while (st == "") st = Console.ReadLine();
        s = st.Split(cs, StringSplitOptions.RemoveEmptyEntries);
        if (s.Length == 0) return next();
        i = 0;
        return s[i++];
    }

    public int nextInt()
    {
        return int.Parse(next());
    }
    public int[] ArrayInt(int N, int add = 0)
    {
        int[] Array = new int[N];
        for (int i = 0; i < N; i++)
        {
            Array[i] = nextInt() + add;
        }
        return Array;
    }

    public long nextLong()
    {
        return long.Parse(next());
    }

    public long[] ArrayLong(int N, long add = 0)
    {
        long[] Array = new long[N];
        for (int i = 0; i < N; i++)
        {
            Array[i] = nextLong() + add;
        }
        return Array;
    }

    public double nextDouble()
    {
        return double.Parse(next());
    }


    //swap
    void swap<T>(ref T a, ref T b)
    {
        T c = a;
        a = b;
        b = c;
    }


    public double[] ArrayDouble(int N, double add = 0)
    {
        double[] Array = new double[N];
        for (int i = 0; i < N; i++)
        {
            Array[i] = nextDouble() + add;
        }
        return Array;
    }
}

class XRand
{
    uint x, y, z, w;


    public XRand()
    {
        init();
    }

    public XRand(uint s)
    {
        init();
        init_xor128(s);
    }

    void init()
    {
        x = 314159265; y = 358979323; z = 846264338; w = 327950288;

    }

    public void init_xor128(uint s)
    {
        z ^= s;
        z ^= z >> 21; z ^= z << 35; z ^= z >> 4;
        z *= 736338717;
    }

    uint next()
    {
        uint t = x ^ x << 11; x = y; y = z; z = w; return w = w ^ w >> 19 ^ t ^ t >> 8;
    }

    public long nextLong(long m)
    {
        return (long)((((ulong)next() << 32) + next()) % (ulong)m);
    }

    public int nextInt(int m)
    {
        return (int)(next() % m);
    }

    public int nextIntP(int a)
    {
        return (int)Math.Pow(a, nextDouble());
    }

    public int nextInt(int min, int max)
    {
        return min + nextInt(max - min + 1);
    }


    public double nextDouble()
    {
        return (double)next() / uint.MaxValue;
    }

    public double nextDoubleP(double a)
    {
        return Math.Pow(a, nextDouble());
    }
}



class Heap<T> where T : IComparable<T>
{
    public HeapNode<T> top;

    public Heap() { }

    public void push(T val)
    {
        top = HeapNode<T>.meld(top, new HeapNode<T>(val));
    }

    public T pop()
    {
        T ret = top.val;
        top = HeapNode<T>.meld(top.r, top.l);
        return ret;
    }

    public void merge(Heap<T> h2)
    {
        top = HeapNode<T>.meld(top, h2.top);
    }

    public class HeapNode<NT> where NT : IComparable<NT>
    {
        public HeapNode<NT> l, r;
        public NT val;

        public HeapNode(NT _val)
        {
            val = _val;
        }

        public static HeapNode<NT> meld(HeapNode<NT> a, HeapNode<NT> b)
        {
            if (a == null) return b;
            if (b == null) return a;
            if (a.val.CompareTo(b.val) > 0)
            {
                HeapNode<NT> temp = a;
                a = b;
                b = temp;
            }
            a.r = meld(a.r, b);
            HeapNode<NT> temph = a.l;
            a.l = a.r;
            a.r = temph;
            return a;
        }
    }
}
