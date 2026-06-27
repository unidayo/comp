using System.Collections.Generic;
using inalib;
using System.Diagnostics;
using System;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace inalib
{
    public class Grid
    {
        public int Height;
        public int Width;
        public int AreaSize;
        public Cell[] Cells;

        public Grid(int height, int width)
        {
            Height = height;
            Width = width;
            AreaSize = height * width;

            InitCells();
        }

        void InitCells()
        {
            Cells = new Cell[AreaSize];
            for (var y = 0; y < Height; y++)
            {
                for (var x = 0; x < Width; x++)
                {
                    var pos = y * Width + x;
                    Cells[pos] = new Cell() { X = x, Y = y, Pos = pos, RelationCells = new() };
                }
            }
            Add4Connect();
        }

        /// <summary>関係セルに4近傍の連結セルを追加する</summary>
        /// <param name="mask">立っているビットの位置のみ生成する　1:up 2:left 4:down 8:right</param>
        public void Add4Connect(int mask = 15)
        {
            for (var y = 0; y < Height; y++)
            {
                for (var x = 0; x < Width; x++)
                {
                    var pos = y * Width + x;
                    var baseCell = Cells[pos];
                    var relation = new List<Cell>();
                    baseCell.RelationCells = relation;
                    // 上下左右でループしないケース用
                    if ((mask & 1) != 0 && y > 0) relation.Add(Cells[pos - Width]);            // up
                    if ((mask & 2) != 0 && x > 0) relation.Add(Cells[pos - 1]);                // left
                    if ((mask & 4) != 0 && y < Height - 1) relation.Add(Cells[pos + Width]);   // down
                    if ((mask & 8) != 0 && x < Width - 1) relation.Add(Cells[pos + 1]);        // right
                }
            }
        }

    }

    public class Cell
    {
        public int X;
        public int Y;
        public int Pos;
        public List<Cell> RelationCells;
    }
}

namespace AHC035
{
    public class Inputs
    {
        public const int N = 6;
        public const int NN = N * N;
        public const int M = 15;
        public const int T = 10;
        public const int SeedMax = 2 * N * (N - 1); 

        public Grid Grid { get; } = new Grid(N, N);

        public Stopwatch Sw { get; } = Stopwatch.StartNew();
    }
}

namespace AHC035
{
    internal class Program
    {
        static void Main(string[] args)
        {

            Console.ReadLine(); // N M T
            var inputs = new Inputs();
            var solver = new Solver();
            for (int turn = 0; turn <= 10; turn++)
            {
                var seeds = new Seed[Inputs.SeedMax];
                for (int i = 0; i < Inputs.SeedMax; i++)
                {
                    var nums = Console.ReadLine().Split().Select(int.Parse).ToArray();
                    seeds[i] = new Seed(i, nums);
                }
                if (turn == 10) break;
                var cmds = solver.GetCommand(inputs, turn, seeds);
                for (int i = 0; i < Inputs.N; i++)
                {
                    Console.WriteLine(string.Join(' ', cmds.Skip(i * Inputs.N).Take(Inputs.N)));
                    //Console.WriteLine(string.Join(' ', cmds, i * Inputs.N, Inputs.N));
                }
            }

        }
    }
}
namespace AHC035
{
    internal class Seed
    {
        public int Id;

        public int[] Qualities { get; }

        public Seed(int id, int[] qualities)
        {
            Id = id;
            Qualities = qualities;
        }
    }
}

namespace AHC035
{
    internal class Solver
    {

        public Solver() 
        {
            Init();
        } 

        public int[] GetCommand(Inputs inputs, int turn, Seed[] seeds)
        {
            var ranks = new int[Inputs.M][];
            for (int i = 0; i < Inputs.M; i++)
            {
                ranks[i] = GetSeedRank(seeds, i);
            }
            var placements = GetGreedyCommand(inputs, seeds, ranks, turn);
            return placements;
        }

        private int[] GetSearchCommand(Inputs inputs, Seed[] seeds, int[][] ranks, int turn)
        {
            var bestScore = long.MinValue;
            var bestPlacements = new int[Inputs.NN];
            for (int firstSeedId = 0; firstSeedId < seeds.Length; firstSeedId++)
            {
                var ok = false;
                for (int type = 0; type < Inputs.M; type++)
                {
                    if (ranks[type][firstSeedId] >= 3) continue;
                    ok = true;
                    break;
                }
                if (!ok) continue;

                var score = 0L;
                var placements = Enumerable.Repeat(-1, Inputs.NN).ToArray();
                var used = new bool[seeds.Length];
                {
                    var pos = orderToPos[0];
                    var seedId = firstSeedId;
                    used[seedId] = true;
                    placements[pos] = seedId;
                }
                for (int i = 1; i < Inputs.NN; i++)
                {
                    var pos = orderToPos[i];
                    var (seedId, addScore) = GetGreedySeed(inputs, seeds, ranks, placements, used, turn, pos);
                    used[seedId] = true;
                    placements[pos] = seedId;
                    score += addScore;
                }
                if (score > bestScore)
                {
                    Array.Copy(placements, bestPlacements, Inputs.NN);
                    bestScore = score;
                }
            }

            return bestPlacements;
        }

        private int[] GetGreedyCommand(Inputs inputs, Seed[] seeds, int[][] ranks, int turn)
        {
            var placements = Enumerable.Repeat(-1, Inputs.NN).ToArray();
            var used = new bool[seeds.Length];
            {
                var pos = orderToPos[0];
                var seedId = GetGreedyFirstSeed(inputs, seeds, ranks);
                used[seedId] = true;
                placements[pos] = seedId;
            }
            for (int i = 1; i < Inputs.NN; i++)
            {
                var pos = orderToPos[i];
                var (seedId, _) = GetGreedySeed(inputs, seeds, ranks, placements, used, turn, pos);
                used[seedId] = true;
                placements[pos] = seedId;
            }
            return placements;
        }

        private int GetGreedyFirstSeed(Inputs inputs, Seed[] seeds, int[][] ranks)
        {
            var bestSeedId = -1;
            var bestScore = long.MinValue;
            for (int seedId = 0; seedId < seeds.Length; seedId++)
            {
                var sum = 0L;
                for (int type = 0; type < Inputs.M; type++)
                {
                    sum += GetSingleScore(seeds, ranks, seedId, type);
                }
                if (sum > bestScore)
                {
                    bestSeedId = seedId;
                    bestScore = sum;
                }
            }
            return bestSeedId;
        }

        private (int, long) GetGreedySeed(Inputs inputs, Seed[] seeds, int[][] ranks, int[] placements, bool[] used, int turn, int pos)
        {
            var bestSeedId = -1;
            var bestScore = long.MinValue;
            var cell = inputs.Grid.Cells[pos];
            for (int seedId = 0; seedId < seeds.Length; seedId++)
            {
                if (used[seedId]) continue;
                var sum = 0L;
                foreach (var next in cell.RelationCells)
                {
                    var pos2 = next.Pos;
                    if (placements[pos2] == -1) continue;
                    for (int type = 0; type < Inputs.M; type++)
                    {
                        sum += GetPairScore(seeds, ranks, seedId, placements[pos2], type, turn);
                    }
                }
                if (sum > bestScore)
                {
                    bestSeedId = seedId;
                    bestScore = sum;
                }
            }
            return (bestSeedId, bestScore);
        }

        private int GetSingleScore(Seed[] seeds, int[][] ranks, int idx1, int type)
        {
            var idx1Score = seeds[idx1].Qualities[type] + rankScores[ranks[type][idx1]];
            return idx1Score;
        }

        int[] turnRankRate = new int[] { 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
        int[] turnValueRate = new int[] { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
        int[] turnCorrections = new int[] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 10 };
        private int GetPairScore(Seed[] seeds, int[][] ranks, int idx1, int idx2, int type, int turn)
        {
            var idx1Score = seeds[idx1].Qualities[type] * turnValueRate[turn] + rankScores[ranks[type][idx1]] * turnRankRate[turn];
            var idx2Score = seeds[idx2].Qualities[type] * turnValueRate[turn] + rankScores[ranks[type][idx2]] * turnRankRate[turn];
            if (seeds[idx1].Qualities[type] >= seeds[idx2].Qualities[type])
            {
                return idx1Score * (80 - turnCorrections[turn]) + idx2Score * (20 + turnCorrections[turn]);
            }
            else
            {
                return idx1Score * (20 + turnCorrections[turn]) + idx2Score * (80 - turnCorrections[turn]);
            }
        }

        private int[] GetSeedRank(Seed[] seeds, int idx)
        {
            var counts = new int[101];
            foreach (var seed in seeds)
            {
                counts[seed.Qualities[idx]]++;
            }
            var sumCounts = new int[101];
            for (int i = 99; i >= 0; i--)
            {
                sumCounts[i] = sumCounts[i + 1] + counts[i + 1];
            }
            var res = new int[seeds.Length];
            for (int i = 0; i < seeds.Length; i++)
            {
                res[i] = sumCounts[seeds[i].Qualities[idx]];
            }
            return res;
        }

        private void Init()
        {
            for (int pos = 0; pos < Inputs.NN; pos++)
            {
                orderToPos[posToOrder[pos]] = pos;
            }

            var score = 10000;
            for (int i = 0; i < Inputs.SeedMax; i++)
            {
                rankScores[i] = score;
                score = score * 5 / 10;
            }
        }

        int[] posToOrder = new int[]
        {
            20, 21, 22, 23, 24, 25,
            19, 6, 7, 8, 9, 26,
            18, 5, 0, 1, 10, 27,
            17, 4, 3, 2, 11, 28,
            16, 15, 14, 13, 12, 29,
            35, 34, 33, 32, 31, 30,
        };

        int[] orderToPos = new int[Inputs.NN];

        int[] rankScores = new int[60];

    }
}

namespace AHC035
{
    internal class State
    {
        public int Seeds { get; set; }
    }
}
