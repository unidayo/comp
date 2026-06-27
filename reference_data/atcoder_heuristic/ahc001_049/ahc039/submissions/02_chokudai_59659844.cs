using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Diagnostics;
using System.ComponentModel;
using static System.Net.Mime.MediaTypeNames;

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
        public int SabaScore;
        public int IwashiScore;
        public static int N;
        public static List<Point> Saba;
        public static List<Point> Iwashi;

        public static List<int>[,] DivSaba;
        public static List<int>[,] DivIwashi;
        public const int divN = 32;
        public const int divD = 100000 / divN;

        public static void SetDiv()
        {
            DivSaba = new List<int>[divN, divN];
            DivIwashi = new List<int>[divN, divN];
            for (int i = 0; i < divN; i++)
            {
                for (int j = 0; j < divN; j++)
                {
                    DivSaba[i, j] = new List<int>();
                    DivIwashi[i, j] = new List<int>();
                }
            }

            for (int i = 0; i < N; i++)
            {
                int px = Saba[i].x / divD;
                int py = Saba[i].y / divD;
                DivSaba[px, py].Add(i);
            }
            for (int i = 0; i < N; i++)
            {
                int px = Iwashi[i].x / divD;
                int py = Iwashi[i].y / divD;
                DivIwashi[px, py].Add(i);
            }
        }


        public bool[] ActiveSaba, ActiveIwashi;

        public int score = 0;
        public List<Rectangle> rects;

        const int LimitP = 1000;
        const int LimitLen = 400000;

        public State()
        {
            SabaScore = 0;
            IwashiScore = 0;
            ActiveSaba = new bool[N];
            ActiveIwashi = new bool[N];
            rects = new List<Rectangle>();
        }

        public State(State state, Rectangle AddRec)
        {
            SabaScore = state.SabaScore;
            IwashiScore = state.IwashiScore;
            ActiveSaba = (bool[])state.ActiveSaba.Clone();
            ActiveIwashi = (bool[])state.ActiveIwashi.Clone();
            rects = new List<Rectangle>(state.rects);
            rects.Add(AddRec);

            int LX = AddRec.lu.x / divD;
            int RX = AddRec.rd.x / divD;
            int LY = AddRec.lu.y / divD;
            int RY = AddRec.rd.y / divD;


            for (int xp = LX; xp <= RX; xp++)
            {
                for (int yp = LY; yp <= RY; yp++)
                {
                    foreach (var i in DivSaba[xp, yp])
                    {
                        if (ActiveSaba[i]) continue;
                        if (AddRec.IsInside(Saba[i]))
                        {
                            ActiveSaba[i] = true;
                            SabaScore++;
                        }
                    }
                    foreach (var i in DivIwashi[xp, yp])
                    {
                        if (ActiveIwashi[i]) continue;
                        if (AddRec.IsInside(Iwashi[i]))
                        {
                            ActiveIwashi[i] = true;
                            IwashiScore++;
                        }
                    }
                }
            }
            score = SabaScore - IwashiScore;
        }

        public (int Saba, int Iwashi) GetAddScore(Rectangle AddRec)
        {
            int SabaAdd = 0;
            int IwashiAdd = 0;

            int LX = AddRec.lu.x / divD;
            int RX = AddRec.rd.x / divD;
            int LY = AddRec.lu.y / divD;
            int RY = AddRec.rd.y / divD;

            for (int xp = LX; xp <= RX; xp++)
            {
                for (int yp = LY; yp <= RY; yp++)
                {
                    foreach (var i in DivSaba[xp, yp])
                    {
                        if (ActiveSaba[i]) continue;
                        if (AddRec.IsInside(Saba[i]))
                        {
                            SabaAdd++;
                        }
                    }
                    foreach (var i in DivIwashi[xp, yp])
                    {
                        if (ActiveIwashi[i]) continue;
                        if (AddRec.IsInside(Iwashi[i]))
                        {
                            IwashiAdd++;
                        }
                    }
                }
            }
            return (SabaAdd, IwashiAdd);
        }

        public List<Rectangle> Connector;

        public void ConnectRects()
        {
            bool[] UsedRec = new bool[rects.Count];
            Connector = new List<Rectangle>();
            UsedRec[0] = true;
            for (int i = 0; i < rects.Count - 1; i++)
            {
                int bestFrom = -1;
                int bestType = -1;
                int bestTo = -1;
                int bestDist = 999999999;
                for (int f = 0; f < rects.Count; f++)
                {
                    if (UsedRec[f]) continue;
                    for (int t = 0; t < rects.Count; t++)
                    {
                        if (!UsedRec[t]) continue;
                        int dist = rects[f].GetDist(rects[t]);
                        if (dist < bestDist)
                        {
                            bestDist = dist;
                            bestFrom = f;
                            bestTo = t;
                            bestType = 0;
                        }
                    }
                    for (int t = 0; t < Connector.Count; t++)
                    {
                        int dist = rects[f].GetDist(Connector[t]);
                        if (dist < bestDist)
                        {
                            bestDist = dist;
                            bestFrom = f;
                            bestTo = t;
                            bestType = 1;
                        }
                    }
                }

                UsedRec[bestFrom] = true;
                Rectangle a = rects[bestFrom];
                Rectangle b;
                if (bestType == 0)
                {
                    b = rects[bestTo];
                }
                else
                {
                    b = Connector[bestTo];
                }
                Point NearA;
                int NearAX, NearAY;
                if (a.rd.x <= b.lu.x) { NearAX = a.rd.x - 1; }
                else { NearAX = Math.Max(a.lu.x, b.lu.x); }
                if (a.rd.y <= b.lu.y) { NearAY = a.rd.y - 1; }
                else { NearAY = Math.Max(a.lu.y, b.lu.y); }
                NearA = new Point(NearAX, NearAY);

                Point NearB;
                int NearBX, NearBY;
                if (b.rd.x <= a.lu.x) { NearBX = b.rd.x - 1; }
                else { NearBX = Math.Max(b.lu.x, a.lu.x); }
                if (b.rd.y <= a.lu.y) { NearBY = b.rd.y - 1; }
                else { NearBY = Math.Max(b.lu.y, a.lu.y); }
                NearB = new Point(NearBX, NearBY);

                if (NearAX != NearBX)
                {
                    if (NearAX < NearBX)
                    {
                        Connector.Add(new Rectangle(new Point(NearAX, NearAY), new Point(NearBX + 1, NearAY + 1)));
                    }
                    else
                    {
                        Connector.Add(new Rectangle(new Point(NearBX, NearAY), new Point(NearAX + 1, NearAY + 1)));
                    }
                }

                if (NearAY != NearBY)
                {
                    if (NearAY < NearBY)
                    {
                        Connector.Add(new Rectangle(new Point(NearBX, NearAY), new Point(NearBX + 1, NearBY + 1)));
                    }
                    else
                    {
                        Connector.Add(new Rectangle(new Point(NearBX, NearBY), new Point(NearBX + 1, NearAY + 1)));
                    }
                }
            }
        }

        public List<Point> points;
        public int len;
        public bool error = false;

        public void GetPointsAndLen()
        {
            Dictionary<int, int> dx = new Dictionary<int, int>();
            Dictionary<int, int> dy = new Dictionary<int, int>();
            foreach (Rectangle r in rects)
            {
                dx[r.lu.x] = 0;
                dx[r.rd.x] = 0;
                dy[r.lu.y] = 0;
                dy[r.rd.y] = 0;
            }
            foreach (Rectangle r in Connector)
            {
                dx[r.lu.x] = 0;
                dx[r.rd.x] = 0;
                dy[r.lu.y] = 0;
                dy[r.rd.y] = 0;
            }
            int[] revX = dx.Keys.ToArray();
            Array.Sort(revX);
            int[] revY = dy.Keys.ToArray();
            Array.Sort(revY);

            for (int i = 0; i < revX.Length; i++)
            {
                dx[revX[i]] = i;
            }
            for (int i = 0; i < revY.Length; i++)
            {
                dy[revY[i]] = i;
            }


            int NX = dx.Count + 2;
            int NY = dy.Count + 2;
            int[,] board = new int[NX, NY];

            foreach (Rectangle rec in rects)
            {
                int l = dx[rec.lu.x];
                int r = dx[rec.rd.x];
                int u = dy[rec.lu.y];
                int d = dy[rec.rd.y];


                for (int x = l; x < r; x++)
                {
                    for (int y = u; y < d; y++)
                    {
                        board[x + 1, y + 1] = 1;
                    }
                }
            }
            foreach (Rectangle rec in Connector)
            {
                int l = dx[rec.lu.x];
                int r = dx[rec.rd.x];
                int u = dy[rec.lu.y];
                int d = dy[rec.rd.y];

                for (int x = l; x < r; x++)
                {
                    for (int y = u; y < d; y++)
                    {
                        board[x + 1, y + 1] = 1;
                    }
                }
            }

            int[] vx = new int[] { 1, 0, -1, 0 };
            int[] vy = new int[] { 0, 1, 0, -1 };
            int[] vx2 = new int[] { 0, 0, -1, -1 };
            int[] vy2 = new int[] { -1, 0, 0, -1 };

            int fx = -1;
            int fy = -1;
            for (int x = 0; x < NX; x++)
            {
                for (int y = 0; y < NY; y++)
                {
                    if (board[x, y] == 1)
                    {
                        fx = x; fy = y;
                        break;
                    }
                }
                if (fx != -1) break;
            }

            int k = 0;

            int nowX = fx;
            int nowY = fy;







            points = new List<Point>();
            points.Add(new Point(revX[nowX - 1], revY[nowY - 1]));

            //error check
            for (int i = 0; i < NX - 1; i++)
            {
                for (int j = 0; j < NY - 1; j++)
                {
                    if (board[i, j] == 1 && board[i + 1, j + 1] == 1 && board[i, j + 1] == 0 && board[i + 1, j] == 0)
                    {
                        error = true;
                        return;
                    }

                    if (board[i, j] == 0 && board[i + 1, j + 1] == 0 && board[i, j + 1] == 1 && board[i + 1, j] == 1)
                    {
                        error = true;
                        return;
                    }
                }
            }

            //bfs error check
            if (true)
            {
                Queue<Tuple<int, int>> q = new Queue<Tuple<int, int>>();
                board[0, 0] = -1;
                q.Enqueue(Tuple.Create(0, 0));
                while (q.Count > 0)
                {
                    var now = q.Dequeue();
                    int x = now.Item1;
                    int y = now.Item2;
                    for (int k2 = 0; k2 < vy.Length; k2++)
                    {
                        int nx = x + vx[k2];
                        int ny = y + vy[k2];
                        if (ny < 0 || ny >= NY || nx < 0 || nx >= NX) continue;
                        if (board[nx, ny] != 0) continue;
                        board[nx, ny] = -1;

                        //ここに更新処理を挿入

                        q.Enqueue(Tuple.Create(nx, ny));
                    }
                }

                for (int i = 0; i < NX; i++)
                {
                    for (int j = 0; j < NY; j++)
                    {
                        if (board[i, j] == 0)
                        {
                            error = true;
                            return;
                        }
                        if (board[i, j] == -1)
                        {
                            board[i, j] = 0;
                        }
                    }
                }
            }




            while (true)
            {
                nowX += vx[k];
                nowY += vy[k];
                if (nowX == fx && nowY == fy) break;

                if (board[nowX + vx2[k], nowY + vy2[k]] == 1)
                {
                    k = (k + 3) % 4;
                    points.Add(new Point(revX[nowX - 1], revY[nowY - 1]));
                }
                else if (board[nowX + vx2[(k + 1) % 4], nowY + vy2[(k + 1) % 4]] == 0)
                {
                    k = (k + 1) % 4;
                    points.Add(new Point(revX[nowX - 1], revY[nowY - 1]));
                }
            }

            len = 0;
            for (int i = 0; i < points.Count; i++)
            {
                len += points[i].GetDiff(points[(i + 1) % points.Count]);
            }

        }

        public void outputPoint()
        {
            Console.WriteLine(points.Count);
            foreach (var item in points)
            {
                Console.WriteLine($"{item.x} {item.y}");
            }
        }


        public int CompareTo(State? other)
        {
            return (score).CompareTo(other.score);
        }
    }

    class Rectangle
    {
        public Point lu, rd;
        public Rectangle(Point lu, Point rd)
        {
            this.lu = lu;
            this.rd = rd;
        }

        public int GetDist(Rectangle r)
        {
            int DiffX = Math.Max(Math.Max(lu.x - r.rd.x, r.lu.x - rd.x), 0);
            int DiffY = Math.Max(Math.Max(lu.y - r.rd.y, r.lu.y - rd.y), 0);
            return DiffX + DiffY;
        }

        public bool IsInside(Point p)
        {
            return p.x >= lu.x && p.x <= rd.x && p.y >= lu.y && p.y <= rd.y;
        }
    }


    class Point
    {
        public int x; public int y;
        public Point(int x, int y)
        {
            this.x = x;
            this.y = y;
        }

        public int GetDiff(Point p)
        {
            return Math.Abs(x - p.x) + Math.Abs(y - p.y);
        }
    }


    int N;
    List<Point> SabaPoint;
    List<Point> IwashiPoint;


    void input()
    {
        N = cin.nextInt();
        SabaPoint = new List<Point>();
        IwashiPoint = new List<Point>();
        for (int i = 0; i < N; i++)
        {
            int x = cin.nextInt();
            int y = cin.nextInt();
            SabaPoint.Add(new Point(x, y));
        }
        for (int i = 0; i < N; i++)
        {
            int x = cin.nextInt();
            int y = cin.nextInt();
            IwashiPoint.Add(new Point(x, y));
        }
        State.N = N;
        State.Saba = SabaPoint;
        State.Iwashi = IwashiPoint;
    }

    void init()
    {
        //Windowsだったらローカルなので実行時間を短くする
        if (System.Runtime.InteropServices.RuntimeInformation.IsOSPlatform(System.Runtime.InteropServices.OSPlatform.Windows))
        {
            LIMIT = 1000;
        }
        sw.Start();
        rnd = new XRand();
        State.SetDiv();
    }

    int mod = 998244353;

    void solve()
    {
        State FirstState = new State();

        int L = 250;

        int[] IwashiMul = new int[] { 10, 7, 5, 3, 2, 1 };
        int[] AddLimit = new int[] { 100, 50, 30, 20, 10, 0 };


        State Allbest = FirstState;
        int AllBestScore = 0;

        while (sw.ElapsedMilliseconds < LIMIT / 2)
        {
            State Now = FirstState;
            for (int phase = 0; phase < 5; phase++)
            {
                while (true)
                {
                    State Best = Now;
                    int BestAdd = AddLimit[phase];
                    for (int t = 0; t < L; t++)
                    {
                        int lx, rx, ly, ry;
                        int wx, wy;
                        do
                        {
                            int t1, t2;
                            do
                            {
                                t1 = rnd.nextInt(N);
                            } while (Now.ActiveSaba[t1]);
                            do
                            {
                                t2 = rnd.nextInt(N);
                            } while (t1 == t2);

                            lx = Math.Min(SabaPoint[t1].x, SabaPoint[t2].x);
                            rx = Math.Max(SabaPoint[t1].x, SabaPoint[t2].x);

                            if (lx == rx)
                            {
                                if (rx != 100000) rx++;
                                else lx--;
                            }

                            ly = Math.Min(SabaPoint[t1].y, SabaPoint[t2].y);
                            ry = Math.Max(SabaPoint[t1].y, SabaPoint[t2].y);

                            if (ly == ry)
                            {
                                if (ry != 100000) ry++;
                                else ly--;
                            }

                            wx = rx - lx;
                            wy = ry - ly;
                        } while ((long)wx * wy > 1e9);


                        Rectangle rec = new Rectangle(new Point(lx, ly), new Point(rx, ry));
                        var score = Now.GetAddScore(rec);
                        var cscore = score.Saba - score.Iwashi * IwashiMul[phase];

                        //Console.WriteLine(score);

                        if (cscore > BestAdd)
                        {
                            State next = new State(Now, rec);
                            next.ConnectRects();
                            next.GetPointsAndLen();
                            if (next.len <= 400000 && next.points.Count <= 1000 && !next.error)
                            {
                                Best = next;
                                BestAdd = cscore;

                                if (next.score >= AllBestScore)
                                {
                                    AllBestScore = Best.score;
                                    Allbest = next;
                                }
                            }
                        }
                    }
                    if (BestAdd == AddLimit[phase]) break;

                    Now = Best;
                    //Console.Error.WriteLine($"Phase: {phase} Add: {BestAdd} RecNum: {Best.rects.Count} ConnectNum: {Best.Connector.Count} Len: {Best.len} PointNum: {Best.points.Count}");
                }

            }
            Console.Error.WriteLine($"Score: " + Now.score);
        }

        while (sw.ElapsedMilliseconds < LIMIT)
        {
            State Now = Allbest;
            for (int phase = 5; phase < 6; phase++)
            {
                while (true)
                {
                    State Best = Now;
                    int BestAdd = AddLimit[phase];
                    for (int t = 0; t < L; t++)
                    {
                        if (sw.ElapsedMilliseconds >= LIMIT)
                        {
                            break;
                        }
                        int lx, rx, ly, ry;
                        int wx, wy;
                        do
                        {
                            int t1, t2;
                            do
                            {
                                t1 = rnd.nextInt(N);
                            } while (Now.ActiveSaba[t1]);
                            do
                            {
                                t2 = rnd.nextInt(N);
                            } while (t1 == t2);

                            lx = Math.Min(SabaPoint[t1].x, SabaPoint[t2].x);
                            rx = Math.Max(SabaPoint[t1].x, SabaPoint[t2].x);

                            if (lx == rx)
                            {
                                if (rx != 100000) rx++;
                                else lx--;
                            }

                            ly = Math.Min(SabaPoint[t1].y, SabaPoint[t2].y);
                            ry = Math.Max(SabaPoint[t1].y, SabaPoint[t2].y);

                            if (ly == ry)
                            {
                                if (ry != 100000) ry++;
                                else ly--;
                            }

                            wx = rx - lx;
                            wy = ry - ly;
                        } while ((long)wx * wy > 2e8);


                        Rectangle rec = new Rectangle(new Point(lx, ly), new Point(rx, ry));
                        var score = Now.GetAddScore(rec);
                        var cscore = score.Saba - score.Iwashi * IwashiMul[phase];


                        //Console.WriteLine(score);

                        if (cscore > BestAdd)
                        {
                            State next = new State(Now, rec);
                            next.ConnectRects();
                            next.GetPointsAndLen();
                            if (next.len <= 400000 && next.points.Count <= 1000 && !next.error)
                            {
                                //if (sw.ElapsedMilliseconds + 250 < LIMIT && next.len - Now.len > 300) continue;

                                Best = next;
                                BestAdd = cscore;

                                if (next.score >= AllBestScore)
                                {
                                    AllBestScore = Best.score;
                                    Allbest = next;
                                }
                            }
                        }
                    }
                    if (BestAdd == AddLimit[phase]) break;

                    Now = Best;
                    //Console.Error.WriteLine($"Phase: {phase} Add: {BestAdd} RecNum: {Best.rects.Count} ConnectNum: {Best.Connector.Count} Len: {Best.len} PointNum: {Best.points.Count}");
                }
            }
        }

        Allbest.outputPoint();
        Console.Error.WriteLine($"Score: {AllBestScore}, Time: {sw.ElapsedMilliseconds} Points: {Allbest.points.Count}");
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


