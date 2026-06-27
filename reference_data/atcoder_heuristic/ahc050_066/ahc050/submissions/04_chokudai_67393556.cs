using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Diagnostics;
using System.ComponentModel;
using static System.Net.Mime.MediaTypeNames;
using System.Security.Cryptography.X509Certificates;
using System.Runtime.InteropServices;

public class Myon
{
    public Myon() { }
    public static int Main()
    {
        new Myon().calc();
        return 0;
    }

    Scanner cin;
    XRand rnd;
    Stopwatch sw;
    int LIMIT = 1880;

    void calc()
    {
        cin = new Scanner();
        rnd = new XRand((uint)DateTime.Now.Millisecond);
        sw = new Stopwatch();
        input();
        sw.Start();
        init();
        solve();
    }

    class State : IComparable<State>
    {
        static public int N, M;
        public bool[,] nowBlock;
        public double[,] p;

        public double nokori;
        public double Score;
        int turn;



        static int[] vy = new int[] { 1, 0, -1, 0 };
        static int[] vx = new int[] { 0, 1, 0, -1 };

        public State(int N, int M, string[] board)
        {
            State.N = N;
            State.M = M;
            Score = 0;
            nokori = 1;
            turn = 0;
            nowBlock = new bool[N, N];
            double tmp = 1.0 / (N * N - M);
            p = new double[N, N];
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    if (board[i][j] == '.')
                    {
                        nowBlock[i, j] = false;
                        p[i, j] = tmp;
                    }
                    else
                    {
                        nowBlock[i, j] = true;
                    }
                }
            }

        }

        public bool isBlock(int y, int x)
        {
            return !(y >= 0 && x >= 0 && x < N && y < N && !nowBlock[y, x]);
        }

        public int CompareTo(State? other)
        {
            return (Score).CompareTo(other.Score);
        }

        public void update()
        {
            double[,] nextP = new double[N, N];
            for(int y = 0; y < N; y++)
            {
                double sum = 0;
                for(int x = 0;x < N; x++)
                {
                    if (nowBlock[y, x]) continue;
                    sum += p[y, x];
                    if (isBlock(y, x + 1))
                    {
                        nextP[y, x] += sum / 4;
                        sum = 0;
                    }
                }
            }
            for (int y = 0; y < N; y++)
            {
                double sum = 0;
                for (int x = N - 1; x >= 0; x--)
                {
                    if (nowBlock[y, x]) continue;
                    sum += p[y, x];
                    if (isBlock(y, x - 1))
                    {
                        nextP[y, x] += sum / 4;
                        sum = 0;
                    }
                }
            }

            for (int x = 0; x < N; x++)
            {
                double sum = 0;
                for (int y = 0; y < N; y++)
                {
                    if (nowBlock[y, x]) continue;
                    sum += p[y, x];
                    if (isBlock(y + 1, x))
                    {
                        nextP[y, x] += sum / 4;
                        sum = 0;
                    }
                }
            }

            for (int x = 0; x < N; x++)
            {
                double sum = 0;
                for (int y = N - 1; y >= 0; y--)
                {
                    if (nowBlock[y, x]) continue;
                    sum += p[y, x];
                    if (isBlock(y - 1, x))
                    {
                        nextP[y, x] += sum / 4;
                        sum = 0;
                    }
                }
            }

            p = nextP;
        }

        public void set(Point pos)
        {
            nokori -= p[pos.y, pos.x];
            Score += nokori;
            turn += 1;
            nowBlock[pos.y, pos.x] = true;
        }
    }

    int N, M;
    string[] board;


    void input()
    {
        N = cin.nextInt(); M = cin.nextInt();
        board = new string[N];
        for (int i = 0; i < N; i++)
        {
            board[i] = cin.next();
        }
    }

    void init()
    {
        //Windowsだったらローカルなので実行時間を短くする
        if (System.Runtime.InteropServices.RuntimeInformation.IsOSPlatform(System.Runtime.InteropServices.OSPlatform.Windows))
        {
            LIMIT = 800;
        }
        rnd = new XRand();
        sw.Start();
    }

    int[,] BlockPoint;
    int[,] RootPoint;
    bool[,] okPos;

    int[] vy = new int[] { 1, 0, -1, 0 };
    int[] vx = new int[] { 0, 1, 0, -1 };


    void solve()
    {
        double bestScore = 0;
        string bestAns = "";
        while(sw.ElapsedMilliseconds < LIMIT)
        {
            var ret = solve2();
            if(bestScore < ret.score)
            {
                bestScore = ret.score;
                bestAns = ret.ans;
            }
        }
        Console.WriteLine(bestAns);
        Console.Error.WriteLine($"score: {bestScore / (N * N - M)}");
    }

    (double score, string ans) solve2()
    {
        BlockPoint = new int[N, N];
        RootPoint = new int[N, N];
        okPos = new bool[N, N];
        StringBuilder sb = new StringBuilder();
        for (int i = 0;i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if(board[i][j] == '#')
                {
                    BlockPoint[i, j] = 1;
                }
                else
                {
                    okPos[i, j] = true;
                }
                
            }
        }

        State s = new State(N, M, board);

        int bestSum = 0;
        Point bestStart = new Point(-1, -1);
        int bestD = 0;

        while(bestSum <= 2)
        {
            int y = rnd.nextInt(N);
            int x = rnd.nextInt(N);
            int d = rnd.nextInt(4);
            if (s.nowBlock[y, x]) continue;
            int sy = y;
            int sx = x;
            while (inside(sy - vy[d], sx - vx[d]) && !s.nowBlock[sy - vy[d], sx - vx[d]])
            {
                sy = sy - vy[d];
                sx = sx - vx[d];
            }

            int gy = y;
            int gx = x;
            while (inside(gy + vy[d], gx + vx[d]) && !s.nowBlock[gy + vy[d], gx + vx[d]])
            {
                gy = gy + vy[d];
                gx = gx + vx[d];
            }
            int sum = Math.Abs(sy - gy) + Math.Abs(sx - gx) + 1;
            if(bestSum < sum)
            {
                bestSum = sum;
                bestStart = new Point(sy, sx);
                bestD = d;
            }
        }
        int[] kabePoint = new int[] { 1, 2, 8, 100, 1000 };

        int[] array = new int[N * N];
        for (int i = 0; i < array.Length; i++)
        {
            array[i] = i;
        }

        List<PP> L = new List<PP>();
        List<int> pre = new List<int>();
        var FirstPP = new PP(bestStart, new Point(bestStart.y + vy[bestD] * (bestSum - 1), bestStart.x + vx[bestD] * (bestSum - 1)), 1);
        L.Add(FirstPP);
        pre.Add(-1);
        FirstPP.FillRoot(BlockPoint, RootPoint, okPos);
        bool[] check = new bool[2000];
        bool[] check2 = new bool[2000];

        while (true)
        {
            int[,] kabeNum = new int[N, N];
            for (int y = 0; y < N; y++)
            {
                for (int x = 0; x < N; x++)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        int ny = y + vy[k];
                        int nx = x + vx[k];
                        if(!inside(ny,nx) || BlockPoint[ny,nx] >= 1)
                        {
                            kabeNum[y, x]++;
                        }
                    }
                }
            }

            bool ok = false;
            int bestL = 0;
            int bestP = 0;// -99999999;
            int bestPre = 0;
            PP bestPP = null;
            for (int i = L.Count - 1; i >= 0; i--)
            {
                if (check[i]) continue;
                var target = L[i];

                bool find = false;

                for (int k = 2; k < target.len; k++)
                {
                    var StartP = new Point(target.A.y + vy[target.vec] * k, target.A.x + vx[target.vec] * k);
                    for (int v = 0; v < 2; v++)
                    {

                        int tv = (target.vec + 1 + v * 2) % 4;
                        int cy = StartP.y - vy[tv];
                        int cx = StartP.x - vx[tv];
                        if (inside(cy, cx) && RootPoint[cy, cx] >= 1) continue;
                        int nowL = 0;
                        int nowP = 0;

                        if (inside(cy, cx) && BlockPoint[cy,cx] >= 1)
                        {
                            nowP -= kabePoint[kabeNum[cy, cx]];
                        }

                        for (int l = 1;; l++)
                        {
                            int dy = StartP.y + vy[tv] * l;
                            int dx = StartP.x + vx[tv] * l;
                            if (!inside(dy,dx) || !okPos[dy, dx]) break;
                            int dy2 = StartP.y + vy[tv] * (l + 1);
                            int dx2 = StartP.x + vx[tv] * (l + 1);
                            if (inside(dy2, dx2) && RootPoint[dy2, dx2] >= 1) break;
                            nowL = l;
                            nowP += kabePoint[kabeNum[dy, dx]] * 3;
                        }
                        var EndP = new Point(StartP.y + vy[tv] * nowL, StartP.x + vx[tv] * nowL);
                        nowP += kabePoint[kabeNum[EndP.y, EndP.x]] * 1;

                        for (int dd = 0; dd < 4; dd++)
                        {
                            int ey = EndP.y + vy[dd];
                            int ex = EndP.x + vx[dd];
                            if(inside(ey, ex) && BlockPoint[ey, ex] >= 1)
                            {
                                nowP -= kabePoint[1];
                            }
                        }


                        find = true;
                        if(nowP > bestP && nowL >= 1)
                        {
                            bestP = nowP;
                            bestL = nowL;
                            //var EndP = new Point(StartP.y + vy[tv] * nowL, StartP.x + vx[tv] * nowL);
                            var nowPP = new PP(StartP, EndP, target.rank + 1);
                            bestPP = nowPP;
                            bestPre = -1;
                        }
                    }
                } 

                if(!find)
                {
                    check[i] = false;
                }
            }
            if (bestPP != null)
            {
                L.Add(bestPP);
                pre.Add(bestPre);
                bestPP.FillRoot(BlockPoint, RootPoint, okPos);
                //Console.Error.WriteLine($"len: {bestPP.len} rank : {bestPP.rank} P:({bestPP.A.y},{bestPP.A.x})-({bestPP.B.y},{bestPP.B.x})");
                ok = true;
            }
            if (!ok)
            {
                randomshuffle(array);

                for (int t = 0; t < N * N; t++)
                {
                    var y = array[t] / N;
                    var x = array[t] % N;
                    for (int d = 0; d < 2; d++)
                    {
                        if (!okPos[y, x]) continue;
                        int sy = y;
                        int sx = x;
                        while (inside(sy - vy[d], sx - vx[d]) && okPos[sy - vy[d], sx - vx[d]])
                        {
                            sy = sy - vy[d];
                            sx = sx - vx[d];
                        }

                        int gy = y;
                        int gx = x;
                        while (inside(gy + vy[d], gx + vx[d]) && okPos[gy + vy[d], gx + vx[d]])
                        {
                            gy = gy + vy[d];
                            gx = gx + vx[d];
                        }
                        int sum = Math.Abs(sy - gy) + Math.Abs(sx - gx) + 1;
                        if (sum >= 7)
                        {
                            var start = new Point(sy, sx);
                            var goal = new Point(start.y + vy[d] * (sum - 1), start.x + vx[d] * (sum - 1));
                            var AddPP = new PP(start, goal, 1);
                            L.Add(AddPP);
                            pre.Add(-1);
                            AddPP.FillRoot(BlockPoint, RootPoint, okPos);

                            //Console.Error.WriteLine($"add turn{L.Count}, start:({start.y},{start.x}) goal:({goal.y},{goal.x})");

                            ok = true;
                            break;
                        }
                    }
                    if (ok) break;
                }

                if (!ok) break;
            };
        }

        /*
        for (int y = 0; y < N; y++)
        {
            for (int x = 0; x < N; x++)
            {
                char c = '?';
                if (RootPoint[y, x] >= 1) c = '.';
                else if (BlockPoint[y, x] >= 1) c = '#';
                Console.Error.Write(c);
            }
            Console.Error.WriteLine();
        }
        Console.Error.WriteLine();
        */

        /*
        int[] whitePower = new int[pre.Count];
        for (int i = pre.Count - 1; i >= 0; i--)
        {
            whitePower[i] += L[i].len;
            if (pre[i] != -1)
            {
                whitePower[pre[i]] += whitePower[i];
            }

            for (int y = Math.Min(L[i].A.y, L[i].B.y); y <= Math.Max(L[i].A.y, L[i].B.y); y++)
            {
                for (int x = Math.Min(L[i].A.x, L[i].B.x); x <= Math.Max(L[i].A.x, L[i].B.x); x++)
                {
                    RootPoint[y, x] = whitePower[i];
                }
            }
        }
        */


        for (int t = 0; t < N * N - M; t++)
        {
            s.update();
            double best = 1e300;
            Point BestPos = new Point(-1, -1);

            for (int i = 0; i < array.Length; i++)
            {
                int y = array[i] / N;
                int x = array[i] % N;
                if (s.isBlock(y, x)) continue;

                double point = s.p[y, x] * 1e17;
                if (BlockPoint[y,x] >= 1)
                {
                    point -= 1e9 * (100 - BlockPoint[y, x]);
                }
                else if (RootPoint[y,x] >= 1)
                {
                    point += 1e9 * (100 - RootPoint[y, x]);
                }

                if (best > point)
                {
                    best = point;
                    BestPos = new Point(y, x);
                }
                if (best == 0) break;
            }
            s.set(BestPos);
            sb.AppendLine($"{BestPos.y} {BestPos.x}");
            //Console.WriteLine($"{BestPos.y} {BestPos.x}");
        }
        Console.Error.WriteLine($"score: {s.Score / (N * N - M)}");
        return (s.Score,  sb.ToString());
    }

    bool inside(int y, int x)
    {
        return y >= 0 && x >= 0 && y < N && x < N;
    }

    public void randomshuffle(int[] a)
    {
        int n = a.Length;
        for (int i = 0; i < n - 1; i++)
        {
            int s = i + rnd.nextInt(n - i);
            if (s != i)
            {
                var tmp = a[i];
                a[i] = a[s];
                a[s] = tmp;
            }
        }
    }

    
}

class PP
{
    public Point A, B;
    public int rank;

    public int len;
    public int vec;

    public PP(Point a, Point b, int rank)
    {
        A = a;
        B = b;
        this.rank = rank;

        len = Math.Abs(a.y - b.y) + Math.Abs(b.x - a.x) + 1;
        if (a.y < b.y) vec = 0;
        else if (a.x < b.x) vec = 1;
        else if (a.y > b.y) vec = 2;
        else vec = 3;
    }

    int[] vy = new int[] { 1, 0, -1, 0 };
    int[] vx = new int[] { 0, 1, 0, -1 };

    public void FillRoot(int[,] BlockPoint, int[,] RootPoint, bool[,] okPos)
    {
        for (int y = Math.Min(A.y, B.y); y <= Math.Max(A.y, B.y); y++)
        {
            for (int x = Math.Min(A.x, B.x); x <= Math.Max(A.x, B.x); x++)
            {
                okPos[y, x] = false;
                if (RootPoint[y, x] == 0)
                {
                    RootPoint[y, x] = rank;
                }
            }
        }

        for (int t = 0; t < 2; t++)
        {
            for (int k = 0; k < vy.Length; k++)
            {
                for (int l = 1; l < 2; l++)
                {
                    var P = A;
                    if (t == 1) P = B;
                    int ny = P.y + vy[k] * l;
                    int nx = P.x + vx[k] * l;
                    if (ny < 0 || nx < 0 || ny >= 40 || nx >= 40) break;
                    okPos[ny, nx] = false;
                    if (RootPoint[ny, nx] >= 1) continue;
                    if (BlockPoint[ny, nx] == 0 || BlockPoint[ny, nx] > rank)
                    {
                        BlockPoint[ny, nx] = rank;
                    }
                    break;
                }
            }
        }



    }

}

class Point
{
    public int y, x;
    public Point(int y, int x)
    {
        this.y = y;
        this.x = x;
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


