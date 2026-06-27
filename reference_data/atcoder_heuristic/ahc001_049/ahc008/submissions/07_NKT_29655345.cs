using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Numerics;

// このコードでは、縦軸の座標(row)をy、横軸の座標(col)をx、座標の値を0～29としています。

class Program
{
    static void Main(string[] args)
    {
        StreamReader reader = null;
        StreamWriter writer = null;
        for (int i = 0; i < args.Length; i++)
        {
            var arg = args[i];
            if (arg == "si")
            {
                var path = args[i + 1];
                reader = new StreamReader(path);
            }
            if (arg == "so")
            {
                var path = args[i + 1];
                writer = new StreamWriter(path);
            }
        }

        var state = new State();
        var N = int.Parse((reader != null) ? reader.ReadLine() : Console.ReadLine());
        if (writer != null) writer.WriteLine(N);
        state.N = N;
        for (int i = 0; i < N; i++)
        {
            var line = (reader != null) ? reader.ReadLine() : Console.ReadLine();
            if (writer != null) writer.WriteLine(line);
            var nums = line.Split(' ').Select(int.Parse).ToArray();
            state.PetType.Add(nums[2]);
            state.PetPos.Add((nums[0] - 1) * 30 + nums[1] - 1);
            state.LeftPetCount[nums[2]]++;
            state.LeftPetId.Add(i);
        }
        var M = int.Parse((reader != null) ? reader.ReadLine() : Console.ReadLine());
        if (writer != null) writer.WriteLine(M);
        state.M = M;
        for (int i = 0; i < M; i++)
        {
            var line = (reader != null) ? reader.ReadLine() : Console.ReadLine();
            if (writer != null) writer.WriteLine(line);
            var nums = line.Split(' ').Select(int.Parse).ToArray();
            state.HumanPos.Add((nums[0] - 1) * 30 + nums[1] - 1);
        }

        var solver = new Solver(state);
        for (int i = 0; i < 300; i++)
        {
            var cmd = solver.GetCommand();
            Console.WriteLine(cmd);
            var line = (reader != null) ? reader.ReadLine() : Console.ReadLine();
            if (writer != null) writer.WriteLine(line);
            if (writer != null) writer.Flush();
            solver.UpdatePet(line);
        }
        if (writer != null) writer.Flush();
        if (writer != null) writer.Close();
        if (reader != null) reader.Close();
    }
}

class Solver
{
    public readonly State State;
    readonly Grid Grid;

    /// <summary>個人の行動決めルーチン</summary>
    ISoloTask[] SoloTasks;

    /// <summary>チームの行動決めルーチン</summary>
    TeamTask[] TeamTasks;

    CloseDoorTask CloseDoorTask;

    /// <summary>各チームが目標にしているCage列番号(デバッグ表示のみに使用)</summary>
    int[] TeamTargetCols;

    /// <summary>犬ゾーンの行動決めルーチン</summary>
    DogZoneTask DogZoneTask;

    /// <summary>終盤無理そうな時のダメコンの行動決めルーチン</summary>
    DamageControlTask DamageControlTask;

    /// <summary>距離に応じて掛けるScore係数</summary>
    double[] DistanceWeight;

    /// <summary>部屋への距離のキャッシュ</summary>
    int[][] cageDistanceCache;

    /// <summary>部屋への距離のキャッシュを作成した時の壁の数</summary>
    int[] cageDistanceChachWallCount;

    /// <summary>タイプ毎に、どれだけ閉じ込めを重視するか</summary>
    readonly double[] TypeWeight = new double[] { 0.0, 1.0, 0.8, 0.7, 0.6, 1.1 };

    public Solver(State state)
    {
        var template = GetTemplate(state);

        State = state;
        State.Init(template);
        Grid = state.Grid;
        Grid.Add4Connect();
        DistanceWeight = Enumerable.Range(0, 900).Select(i => Math.Pow(0.8, i)).ToArray();
        cageDistanceCache = new int[template.CageNum][];
        cageDistanceChachWallCount = new int[template.CageNum];
        // 戦略の設定
        InitTask(template);
    }

    AbstractTemplate GetTemplate(State state)
    {
        if (state.M <= 5) return new Row5Template();
        if (state.M == 6 && state.N >= 12) return new Row5Template();
        if (state.M == 7 && state.N >= 17) return new Row5Template();


        if (state.M <= 6) return new Row6Template();
        if (state.M == 7 && state.N >= 11) return new Row6Template();
        if (state.M == 8 && state.N >= 15) return new Row6Template();
        if (state.M == 9 && state.N >= 18) return new Row6Template();
        if (state.M == 10 && state.N >= 18) return new Row6Template();

        return new Row7Template();
    }

    public string GetCommand()
    {
        State.PreCalc();
        var commands = Enumerable.Repeat('.', State.M).ToArray();

        // 全捕獲済なら全員待機（時間節約のため）
        if (!State.LeftPetId.Any()) return string.Join("", commands);

        // チームを組む前の人(必ず未行動)
        for (int i = State.SoloHumanIds.Count - 1; i >= 0; i--)
        {
            var humanId = State.SoloHumanIds[i];
            var cmd = SoloTasks[humanId].GetCommand(State);
            if (cmd == 'X')
            {
                DogZoneTask.Team.MemberIds.Add(humanId);
                AddToTeam(humanId);
                continue;
            }
            commands[humanId] = cmd;
            State.HumanMove(humanId, cmd);
        }

        // 犬ゾーン
        if (State.Template.HasDogZone)
        {
            var cmds = DogZoneTask.GetWallCommand(State);
            var isSetWall = false;
            for (int i = 0; i < cmds.Length; i++)
            {
                var cmd = cmds[i];
                if (cmd == 'X') continue;
                var humanId = DogZoneTask.Team.MemberIds[i];
                commands[humanId] = cmd;
                State.HumanMove(humanId, cmd);
                isSetWall = true;
            }
            if (isSetWall)
            {
                for (int i = 0; i < State.M; i++)
                {
                    AddToTeam(i);
                }
                DogZoneTask.Team.MemberIds.Clear();
            }
            else
            {
                cmds = DogZoneTask.GetMoveCommand(State);
                for (int i = 0; i < cmds.Length; i++)
                {
                    var cmd = cmds[i];
                    if (cmd == 'X') continue;
                    var humanId = DogZoneTask.Team.MemberIds[i];
                    commands[humanId] = cmd;
                    State.HumanMove(humanId, cmd);
                }
            }
        }

        // 部屋を閉じる扉を設置
        {
            var cmds = CloseDoorTask.GetWallCommand(State);
            for (int i = 0; i < cmds.Length; i++)
            {
                var cmd = cmds[i];
                if (cmd == 'X') continue;
                var humanId = i;
                commands[humanId] = cmd;
                State.HumanMove(humanId, cmd);
            }
        }

        // 終盤ダメコン
        {
            var cmds = DamageControlTask.GetWallCommand(State);
            for (int i = 0; i < cmds.Length; i++)
            {
                var cmd = cmds[i];
                if (cmd == 'X') continue;
                var humanId = i;
                commands[humanId] = cmd;
                State.HumanMove(humanId, cmd);
            }
            // 特殊処理なので全ペットを撤去して終わらせる
            if (cmds.Any(c => c >= 'a' && c <= 'z'))
            {
                State.LeftPetId.Clear();
                for (int i = 0; i < 6; i++)
                {
                    State.LeftPetCount[i] = 0;
                }
                return string.Join("", commands);
            }
        }
        {
            var cmds = DamageControlTask.GetMoveCommand(State);
            for (int i = 0; i < cmds.Length; i++)
            {
                var cmd = cmds[i];
                if (cmd == 'X') continue;
                var humanId = i;
                commands[humanId] = cmd;
                State.HumanMove(humanId, cmd);
            }
        }

        // チームを組んだ後の人 壁設置以外
        State.Teams.ForEach(team => team.AdjustmentHeight(State));
        TeamTargetCols = GetTeamTargetCols();
        for (int ti = 0; ti < TeamTasks.Length; ti++)
        {
            var teamTask = TeamTasks[ti];
            var cmds = teamTask.GetMoveCommand(State, TeamTargetCols[ti]);
            for (int i = 0; i < cmds.Length; i++)
            {
                var cmd = cmds[i];
                if (cmd == 'X') continue;
                var humanId = teamTask.Team.MemberIds[i];
                commands[humanId] = cmd;
                State.HumanMove(humanId, cmd);
            }
        }

        //PrintDebugMessage();
        var res = string.Join("",commands);
        return res;
    }

    /// <summary>メンバーを何処かのチームに所属させる</summary>
    void AddToTeam(int humanId)
    {
        // ぼっち解消
        State.SoloHumanIds.Remove(humanId);
        // 最初に見つけた空いているチームに入れる
        foreach (var team in State.Teams)
        {
            if (team.MemberIds.Count >= team.Capacity) continue;
            team.MemberIds.Add(humanId);
            break;
        }
    }

    void PrintDebugMessage()
    {
        for (int i = 0; i < State.Teams.Count; i++)
        {
            var team = State.Teams[i];
            Console.Write($"# Team[{i}] : TargetCol = {TeamTargetCols[i]}, Member = ");
            foreach (var id in team.MemberIds)
            {
                Console.Write(id + ", ");
            }
            Console.WriteLine();
        }
        Console.Write($"# ChanceCage = ");
        for (int i = 0; i < State.Cages.Length; i++)
        {
            var cage = State.Cages[i];
            if (cage.CanClose && cage.IsEnable) Console.Write(i + ", ");
        }
        Console.WriteLine();
    }

    /// <summary>各チームが何列目のcageに向かうべきかを取得する</summary>
    int[] GetTeamTargetCols()
    {
        var cageScores = GetCageScores();
        var moveCosts = GetMoveCost();
 
        // チーム3つ用に3重ループにして、チーム2つの場合は無理矢理抜けて対応(きたない)
        var bestCols = new int[] { 0, 0, 0 };
        var bestScore = double.MinValue;
        for (int c1 = 0; c1 < State.CageColNum; c1++)
        {
            for (int c2 = 0; c2 < State.CageColNum; c2++)
            {
                for (int c3 = 0; c3 < State.CageColNum; c3++)
                {
                    var cols = new int[] { c1, c2, c3 };
                    var score = GetTeamTargetScore(cols, moveCosts, cageScores);
                    if (score > bestScore)
                    {
                        bestScore = score;
                        bestCols = cols;
                    }
                    if (State.Teams.Count == 2) break;
                }
            }
        }
        return bestCols.Take(State.Teams.Count).ToArray();
    }

    /// <summary>部屋ごとに、ペットの近さに応じたスコアを付ける</summary>
    double[] GetCageScores()
    {
        var wallCount = State.IsWall.Count(v => v);
        var res = new double[State.Cages.Length];
        for (int i = 0; i < State.Cages.Length; i++)
        {
            if (State.EnableDogZone && (i % State.CageColNum <= 6 || i % State.CageColNum >= 10)) continue; // 犬ゾーンを作る＆有効なうちは右寄りにいる
            var cage = State.Cages[i];
            if (!cage.IsEnable) continue;   // 未完成の部屋はスコア無し
            var distances = cageDistanceChachWallCount[i] == wallCount ? cageDistanceCache[i] : State.GetDistance(cage.InnerPos);
            cageDistanceChachWallCount[i] = wallCount;
            cageDistanceCache[i] = distances;
            for (int j = 0;j < State.N; j++)
            {
                var type = State.PetType[j];
                if (cage.CloseWallPos.Count == 1 && type == 4) continue;    // 犬が行き止まり部屋に入ることは考えない
                if (State.EnableDogZone && type == 4) continue;             // 犬ゾーンを作る場合は犬を考慮しない
                var rate = 1.0;
                if (type == 5 && !cage.InnerPos.Any(pos => State.IsCatTarget[j][pos])) rate = 0.001;    // 猫が目的地でない部屋に入ることはほぼ考慮しない
                var dis = distances[State.PetPos[j]];
                res[i] += TypeWeight[type] * Math.Pow(2.0, -dis) * rate;
            }
        }
        return res;
    }

    /// <summary>[teamId][col] teamがcol列目のcageに移動するための移動距離を取得</summary>
    int[][] GetMoveCost()
    {
        // TODO 行動済のメンバーは移動に1手多くかかる
        var res = new int[State.Teams.Count][];
        for (int i = 0; i < State.Teams.Count; i++)
        {
            var team = State.Teams[i];
            var distances = team.MemberIds.Select(id => State.GetDistance(State.HumanPos[id])).ToArray();
            res[i] = new int[State.CageColNum];
            for (int col = 0; col < State.CageColNum; col++)
            {
                var targets = Enumerable.Range(0, team.MemberIds.Count).Select(idx => team.TargetY[idx] * 30 + State.Cages[col].BaseX).ToArray();
                res[i][col] = State.GetBestCost(distances, targets).cost;   // memo 最後のメンバーを配置する時間なので、先に一部の配置が完了するケースは計算が狂う
            }
        }
        return res;
    }

    double GetTeamTargetScore(int[] cols, int[][] moveCosts, double[] cageCosts)
    {
        // いずれかのチームが各部屋の入口に到達するまでの時間を求める
        var cageMinDistance = Enumerable.Repeat(int.MaxValue, State.Cages.Length).ToArray();
        for (int i = 0; i < State.Teams.Count; i++)
        {
            foreach (var cage in State.Teams[i].TargetCages[State.Teams[i].MemberIds.Count][cols[i]])
            {
                cageMinDistance[cage.Id] = Math.Min(cageMinDistance[cage.Id], moveCosts[i][cage.Id % State.CageColNum]);
            }
        }
        // 部屋の動物スコアと到達時間からスコアを求める
        var score = 0.0;
        for (int i = 0; i < State.Cages.Length; i++)
        {
            var dis = cageMinDistance[i];
            if (dis >= 900) continue;
            score += cageCosts[i] * DistanceWeight[dis];
        }
        return score;
    }

    public void UpdatePet(string petMove)
    {
        State.Turn++;
        var i = 0;
        foreach (var move in petMove.Split(' '))
        {
            var j = 0;
            foreach (var c in move)
            {
                // 猫の1移動目から、目的地を推測
                if (State.PetType[i] == 5 && j == 0)
                {
                    var isValid = GetValidCatTarget(State.PetPos[i], c);
                    State.UpdateCatTarget(i, isValid);
                }
                switch (c)
                {
                    case 'L':
                        State.PetPos[i] -= 1;
                        break;
                    case 'R':
                        State.PetPos[i] += 1;
                        break;
                    case 'U':
                        State.PetPos[i] -= 30;
                        break;
                    case 'D':
                        State.PetPos[i] += 30;
                        break;
                }
                j++;
            }
            i++;
        }
    }

    bool[] GetValidCatTarget(int srcPos, char dir)
    {
        var dirIdx = dir == 'U' ? 0 : (dir == 'D' ? 1 : (dir == 'L' ? 2 : 3));
        var y = srcPos / 30;
        var x = srcPos % 30;
        var distances = new int[4][];
        // UP
        distances[0] = (y <= 0 || State.IsWall[srcPos - 30]) ? Enumerable.Repeat(int.MaxValue, 900).ToArray() : State.GetDistance(srcPos - 30);
        // DOWN
        distances[1] = (y >= 29 || State.IsWall[srcPos + 30]) ? Enumerable.Repeat(int.MaxValue, 900).ToArray() : State.GetDistance(srcPos + 30);
        // LEFT
        distances[2] = (x <= 0 || State.IsWall[srcPos - 1]) ? Enumerable.Repeat(int.MaxValue, 900).ToArray() : State.GetDistance(srcPos - 1);
        // RIGHT
        distances[3] = (x >= 29 || State.IsWall[srcPos + 1]) ? Enumerable.Repeat(int.MaxValue, 900).ToArray() : State.GetDistance(srcPos + 1);

        var isValid = new bool[900];
        for (int i = 0; i < 900; i++)
        {
            if (i == srcPos ||
                distances[dirIdx][i] == int.MaxValue ||
                distances[dirIdx][i] > distances[0][i] ||
                distances[dirIdx][i] > distances[1][i] ||
                distances[dirIdx][i] > distances[2][i] ||
                distances[dirIdx][i] > distances[3][i]) continue;
            isValid[i] = true;
        }
        return isValid;
    }

    void InitTask(AbstractTemplate template)
    {
        var colOrders = new int[10][];
        colOrders[0] = new int[] { 1, 4, 7, 10, 13, 16, 19, 22, 25, 28 };
        colOrders[1] = colOrders[0].Reverse().ToArray();
        colOrders[2] = new int[] { 16, 13, 10, 7, 4, 1, 19, 22, 25, 28 };
        colOrders[3] = new int[] { 19, 22, 25, 28, 16, 13, 10, 7, 4, 1 };
        colOrders[4] = new int[] { 7, 10, 13, 16, 19, 22, 25, 28, 1, 4 };
        colOrders[5] = new int[] { 25, 28, 22, 19, 16, 13, 10, 7, 4, 1 };
        colOrders[6] = new int[] { 4, 7, 10, 13, 16, 19, 22, 25, 28, 1 };
        colOrders[7] = new int[] { 22, 19, 25, 28, 16, 13, 10, 7, 4, 1 };
        colOrders[8] = new int[] { 13, 10, 7, 4, 1, 16, 19, 22, 25, 28 };
        colOrders[9] = new int[] { 10, 13, 16, 19, 22, 25, 28, 1, 4, 7 };

        var taskXIds = Enumerable.Range(0, State.M).ToList();
        taskXIds.Sort((a, b) => colOrders[a][0] - colOrders[b][0]);
        var humanXIds = Enumerable.Range(0, State.M).ToList();
        humanXIds.Sort((a, b) => (State.HumanPos[a] % 30) - (State.HumanPos[b] % 30));

        SoloTasks = new ISoloTask[State.M];
        for (int i = 0; i < State.M; i++)
        {
            var humanId = humanXIds[i];
            var order = colOrders[taskXIds[i]];
            var task = new SequenceTask();
            foreach (var col in order)
            {
                Debug.Assert(colOrders[0].Contains(col));   // 変な列番号を指定していないかチェック
                task.Tasks.Add(new SetPreWallTask(humanId, col - (template.HasDogZone ? 1 : 0), Grid, template));
            }
            SoloTasks[humanId] = task;
        }

        CloseDoorTask = new CloseDoorTask();
        TeamTasks = new TeamTask[State.Teams.Count];
        for (int i = 0; i < State.Teams.Count; i++)
        {
            TeamTasks[i] = new TeamTask(State.Teams[i]);
        }

        DogZoneTask = new DogZoneTask() { Team = new Team() { Capacity = 999 } };
        DamageControlTask = new DamageControlTask();
    }

}

class State
{
    public int CageRowNum => Template.CageRowNum;
    public int CageColNum => Template.CageColNum;

    public Grid Grid = new Grid(30, 30);
    public int N;
    public int M;
    /// <summary>人間・ペットがそれぞれ移動した回数(0～299)</summary>
    public int Turn;
    public bool[] IsWall = new bool[900];
    public List<int> PetType = new List<int>();
    public List<int> PetPos = new List<int>();
    public List<int> HumanPos = new List<int>();
    public List<Team> Teams = new List<Team>();

    public AbstractTemplate Template;

    /// <summary>各メンバーがこのターンに既に行動済みか</summary>
    public bool[] IsMoved = new bool[10];

    /// <summary>セルの状態 0.通常 1.移動禁止 2.壁作成禁止</summary>
    public int[] CellState = new int[900];

    /// <summary>事前配置すべき壁が、設置者のX位置ごとに何個残っているか</summary>
    public readonly int[] PreWallLeft = new int[30];

    /// <summary>事前配置すべき壁を現在担当中の人が、設置者のX位置ごとに何人いるか</summary>
    public readonly int[] PreWallMemberCount = new int[30];

    /// <summary>各メンバが、事前配置すべき壁のどの列を担当しているか</summary>
    public readonly int[] PreWallMemberCol = new int[10];

    /// <summary>単独行動中(=事前配置壁を設置中)の人間ID</summary>
    public List<int> SoloHumanIds;

    /// <summary>動物を入れる部屋</summary>
    public Cage[] Cages;

    /// <summary>Cageの行単位での残り数(これが0になると上下が分断される)</summary>
    public int[] LeftRowCageCount;

    /// <summary>ペットの種類ごとの残り数(まだ部屋に入れてない数)</summary>
    public readonly int[] LeftPetCount = new int[6];

    /// <summary>まだ部屋に入れていないペットのID</summary>
    public readonly List<int> LeftPetId = new List<int>();

    public bool EnableDogZone;

    /// <summary>[petId][pos] 猫の移動目標として妥当か</summary>
    public bool[][] IsCatTarget;

    /// <summary>方向(文字)ごとの、現在座標を基準とした座標の差異</summary>
    private readonly int[] posDiff = new int[26];

    public void Init(AbstractTemplate template)
    {
        // 各方向の座標情報を設定
        posDiff['L' - 'A'] = -1;
        posDiff['R' - 'A'] = +1;
        posDiff['U' - 'A'] = -30;
        posDiff['D' - 'A'] = +30;
        // テンプレート適用
        SetTemplate(template);
        IsCatTarget = Enumerable.Range(0, N).Select(i => Enumerable.Repeat(true, 900).ToArray()).ToArray();
    }

    void SetTemplate(AbstractTemplate template)
    {
        Template = template;
        EnableDogZone = template.HasDogZone;
        Cages = template.GetCages();
        Teams = template.GetTeams(M, Cages);
        // 事前配置壁の情報を初期化
        var cols = new int[] { 1, 4, 7, 10, 13, 16, 19, 22, 25, 28 };
        foreach (var col in cols)
        {
            var c = col - (template.HasDogZone ? 1 : 0);
            PreWallLeft[c] = template.IsPreWall
                .Select((v, pos) => (v, pos))
                .Count(vpos => vpos.v && (vpos.pos % 30 >= c - 1) && (vpos.pos % 30 <= c + 1));
        }
        // チーム編成の初期化(ソロ)
        SoloHumanIds = Enumerable.Range(0, M).Reverse().ToList();
        SoloHumanIds.ForEach(id => PreWallMemberCol[id] = -1);

        LeftRowCageCount = Enumerable.Repeat(CageColNum, CageRowNum).ToArray();
    }

    /// <summary>毎ターンの入力受け取り直後に行うべき前処理</summary>
    public void PreCalc()
    {
        Array.Clear(IsMoved, 0, 10);
        Array.Clear(CellState, 0, 900);
        // 人がいる(いた)場所に建築禁止
        foreach (var pos in HumanPos)
        {
            CellState[pos] = 2;
        }
        // 動物とその隣の場所に建築禁止
        foreach (var pos in PetPos)
        {
            CellState[pos] = 2;
            foreach (var next in Grid.Cells[pos].RelationCells)
            {
                CellState[next.Pos] = 2;
            }
        }
        // 壁は通行禁止(建築禁止より優先で上書き)
        for (int i = 0; i < 900; i++)
        {
            if (IsWall[i]) CellState[i] = 1;
        }
        // 部屋の状態更新
        UpdateCages();
    }

    public void HumanMove(int id, char cmd)
    {
        IsMoved[id] = true;
        if (cmd >= 'A' && cmd <= 'Z')
        {
            var diff = posDiff[cmd - 'A'];
            HumanPos[id] += diff;
            CellState[HumanPos[id]] = 2;
        }
        if (cmd >= 'a' && cmd <= 'z')
        {
            var diff = posDiff[cmd - 'a'];
            IsWall[HumanPos[id] + diff] = true;
            CellState[HumanPos[id] + diff] = 1;
            // 檻情報を更新
            UpdateCages(HumanPos[id] + diff);
            // 事前設置壁情報を更新
            var x = Grid.Cells[HumanPos[id]].X;
            PreWallLeft[x]--;
        }
    }

    public void UpdateCatTarget(int petId, bool[] isValid)
    {
        for (int i = 0; i < 900; i++)
        {
            IsCatTarget[petId][i] &= isValid[i];
        }
        // 破綻(候補無し)になったら目的地が変わっているので、新しく追跡を始める
        if (!IsCatTarget[petId].Any(v => v)) IsCatTarget[petId] = isValid;
    }

    void UpdateCages(int newWallPos)
    {
        foreach (var cage in Cages)
        {
            var isOpen = cage.CloseWallPos.Any();
            if (cage.PreWallPos.Contains(newWallPos)) cage.LeftPreWallCount--;
            cage.CloseWallPos.RemoveAll(pos => pos == newWallPos);
            if (!cage.CloseWallPos.Any()) 
            {
                cage.IsEnable = false;
                if (isOpen)
                {
                    LeftRowCageCount[cage.Id / CageColNum]--;
                    cage.InnerPets.ForEach(id => LeftPetId.Remove(id));
                    cage.InnerPets.ForEach(id => LeftPetCount[PetType[id]]--);
                }
            }
        }
        foreach (var cage in Cages)
        {
            var row = cage.Id / CageColNum;
            if (!(row == 0 || row == CageRowNum - 1 || LeftRowCageCount[row] >= 2)) cage.IsEnable = false;
        }
    }

    public void UpdateCages()
    {
        foreach (var cage in Cages)
        {
            var row = cage.Id / CageColNum;
            cage.InnerPets.Clear();
            cage.InnerHumans.Clear();
            foreach (var pos in cage.InnerPos)
            {
                for (int i = 0; i < N; i++)
                {
                    if (PetPos[i] == pos) cage.InnerPets.Add(i);
                }
                for (int i = 0; i < M; i++)
                {
                    if (HumanPos[i] == pos) cage.InnerHumans.Add(i);
                }
            }
            cage.IsEnable = cage.LeftPreWallCount == 0 && cage.CloseWallPos.Any() && PreWallLeft[cage.BaseX] <= 0 && (row == 0 || row == CageRowNum - 1 || LeftRowCageCount[row] >= 2);
            cage.CanClose = cage.InnerPets.Any() && !cage.InnerHumans.Any() && !cage.CloseWallPos.Any(pos => CellState[pos] == 2);
        }
    }

    public int[] GetDistance(int srcPos)
    {
        return GetDistance(new int[] { srcPos });
    }

    public int[] GetDistance(IEnumerable<int> srcPos)
    {
        var queue = new Queue<int>();
        var distance = Enumerable.Repeat(int.MaxValue, 900).ToArray();
        foreach (var pos in srcPos)
        {
            queue.Enqueue(pos);
            distance[pos] = 0;
        }
        while (queue.Count > 0)
        {
            var pos = queue.Dequeue();
            foreach (var next in Grid.Cells[pos].RelationCells)
            {
                if (distance[next.Pos] > distance[pos] + 1 && CellState[next.Pos] != 1)
                {
                    distance[next.Pos] = distance[pos] + 1;
                    queue.Enqueue(next.Pos);
                }
            }
        }
        return distance;
    }

    public char GetDirection(int[] distance, int srcPos, int targetPos)
    {
        var cmd = '.';
        while (targetPos != srcPos)
        {
            foreach (var next in Grid.Cells[targetPos].RelationCells)
            {
                if (distance[next.Pos] == distance[targetPos] - 1)
                {
                    if (next.Pos + 1 == targetPos) cmd = 'R';
                    if (next.Pos - 1 == targetPos) cmd = 'L';
                    if (next.Pos + 30 == targetPos) cmd = 'D';
                    if (next.Pos - 30 == targetPos) cmd = 'U';
                    targetPos = next.Pos;
                    break;
                }
            }
        }
        return cmd;
    }

    /// <summary>多対多の移動コストで、最大コストの最小を取得する</summary>
    public (int cost, List<int> targets) GetBestCost(int[][]distances, int[] targets)
    {
        // 4人用に4重ループにして、1～3人の場合は無理矢理抜けて対応(きたなすぎる)
        var n = targets.Length;
        var t = new List<int>();
        var minDistance = int.MaxValue;
        var minCombination = new List<int>();
        for (int i1 = 0; i1 < n; i1++)
        {
            t.Add(targets[i1]);
            if (n == 1)
            {
                var dis = GetMaxDistance(distances, t);
                if (dis < minDistance)
                {
                    minDistance = dis;
                    minCombination.Clear();
                    minCombination.AddRange(t);
                }
                t.RemoveAt(0);
                continue;
            }
            for (int i2 = 0; i2 < Math.Max(1, n); i2++)
            {
                if (i2 == i1) continue;
                t.Add(targets[i2]);
                if (n == 2)
                {
                    var dis = GetMaxDistance(distances, t);
                    if (dis < minDistance)
                    {
                        minDistance = dis;
                        minCombination.Clear();
                        minCombination.AddRange(t);
                    }
                    t.RemoveAt(1);
                    continue;
                }
                for (int i3 = 0; i3 < Math.Max(1, n); i3++)
                {
                    if (i3 == i1 || i3 == i2) continue;
                    t.Add(targets[i3]);
                    if (n == 3)
                    {
                        var dis = GetMaxDistance(distances, t);
                        if (dis < minDistance)
                        {
                            minDistance = dis;
                            minCombination.Clear();
                            minCombination.AddRange(t);
                        }
                        t.RemoveAt(2);
                        continue;
                    }
                    for (int i4 = 0; i4 < Math.Max(1, n); i4++)
                    {
                        if (i4 == i1 || i4 == i2 || i4 == i3) continue;
                        t.Add(targets[i4]);

                        var dis = GetMaxDistance(distances, t);
                        if (dis < minDistance)
                        {
                            minDistance = dis;
                            minCombination.Clear();
                            minCombination.AddRange(t);
                        }

                        t.RemoveAt(3);
                    }
                    t.RemoveAt(2);
                }
                t.RemoveAt(1);
            }
            t.RemoveAt(0);
        }
        return (minDistance, minCombination);
    }

    int GetMaxDistance(int[][] distances, List<int> targets)
    {
        return targets.Select((pos, i) => distances[i][pos]).Max();
    }
}

class Cage
{
    public int Id;
    /// <summary>X基準位置</summary>
    public int BaseX;

    /// <summary>何行目の部屋か</summary>
    public int CageRow;

    /// <summary>何列目の部屋か</summary>
    public int CageCol;

    /// <summary>事前に設置が必要な壁の位置</summary>
    public List<int> PreWallPos = new List<int>();
    /// <summary>閉じ込め可能な位置</summary>
    public List<int> InnerPos = new List<int>();
    /// <summary>閉じるときに壁を置く位置</summary>
    public List<int> CloseWallPos = new List<int>();
    /// <summary>閉じるために人が立つ位置</summary>
    public List<int> StandPos = new List<int>();

    /// <summary>中にいるペットのID</summary>
    public List<int> InnerPets = new List<int>();
    /// <summary>中にいる人のID</summary>
    public List<int> InnerHumans = new List<int>();

    public int LeftPreWallCount;

    /// <summary>有効か(事前壁を全て設置済、かつ使用前、かつX基準位置から置く事前設置壁を全て配置済)</summary>
    /// <remarks>事前設置壁の完成を同時に検出したりは難しいので対応しない(ターン開始時点で完成してないと駄目)</remarks>
    public bool IsEnable; 

    /// <summary>閉じてもいい状態か(壁が設置不可でない、かつ中に人がいない、かつ中に動物がいる、かつ行単位のCageを使い切らない)</summary>
    /// <remarks>設置→移動の順で解決すればターン中の再計算は不要(人が壁の設置予定地に新しく侵入した場合のみ再計算が必要)</remarks>
    public bool CanClose;
}

class Team
{
    /// <summary>最大人数</summary>
    public int Capacity;
    public List<int> TargetY = new List<int>();
    public List<int> MemberIds = new List<int>();

    /// <summary>[num][col] num人のとき、左からcol番目(0-Based)で封鎖可能なCageの一覧</summary>
    public List<Cage>[][] TargetCages;

    public int BaseTargetYMin;
    public int BaseTargetYMax;

    public void AdjustmentHeight(State state)
    {
        if (MemberIds.Count < 2) return;        // 最低2人を割り当てるまで変更はしない(別にしても良さそう？)
        if (!state.LeftPetId.Any()) return;     // 全捕獲完了
        if (state.LeftPetCount[5] > 0) return;  // 猫残り

        var petYMin = state.LeftPetId.Min(id => state.PetPos[id] / 30);
        var petYMax = state.LeftPetId.Max(id => state.PetPos[id] / 30);
        var areaYMin = TargetY.Min();
        var areaYMax = TargetY.Max();
        var yDiff = Math.Abs(TargetY[0] - TargetY[1]);

        // 上にずらす
        if (petYMax + 2 < areaYMin || (petYMin <= BaseTargetYMin && areaYMin > BaseTargetYMin))
        {
            if (areaYMin <= yDiff) return;      // もうずらせない
            for (int i = 0; i < TargetY.Count; i++)
            {
                TargetY[i] -= yDiff;
            }
            foreach (var cages1 in TargetCages)
            {
                foreach (var cages2 in cages1)
                {
                    for (int i = 0; i < cages2.Count; i++)
                    {
                        cages2[i] = state.Cages[cages2[i].Id - state.CageColNum];
                    }
                }
            }
        }
        // 下にずらす
        if (petYMin  > areaYMax + 2 || (petYMax >= BaseTargetYMax && areaYMax < BaseTargetYMax))
        {
            if (areaYMax >= 30 - yDiff) return; // もうずらせない
            for (int i = 0; i < TargetY.Count; i++)
            {
                TargetY[i] += yDiff;
            }
            foreach (var cages1 in TargetCages)
            {
                foreach (var cages2 in cages1)
                {
                    for (int i = 0; i < cages2.Count; i++)
                    {
                        cages2[i] = state.Cages[cages2[i].Id + state.CageColNum];
                    }
                }
            }
        }
    }
}

class Grid
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
                Cells[pos] = new Cell() { X = x, Y = y, Pos = pos, RelationCells = new List<Cell>() };
            }
        }
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
                var relation = baseCell.RelationCells;
                // 上下左右でループしないケース用
                if ((mask & 1) != 0 && y > 0) relation.Add(Cells[pos - Width]);            // up
                if ((mask & 2) != 0 && x > 0) relation.Add(Cells[pos - 1]);                // left
                if ((mask & 4) != 0 && y < Height - 1) relation.Add(Cells[pos + Width]);   // down
                if ((mask & 8) != 0 && x < Width - 1) relation.Add(Cells[pos + 1]);        // right
            }
        }
    }

}

class Cell
{
    public int X;
    public int Y;
    public int Pos;
    /// <summary>隣接セル [index]</summary>
    public List<Cell> RelationCells;
}

class DamageControlArea
{
    public List<int> WallPos;
    public List<int> StandPos;
    public List<int> InnerPos;

    public List<int> StandHumanIds;

    /// <summary>使用するセル数（概算）</summary>
    public int Cost;
}

interface ISoloTask
{
    /// <summary>行動内容。終了の場合は'X'、待機の場合は'.'。</summary>
    char GetCommand(State state);
}

/// <summary>登録されたタスクを登録順に優先して実行する</summary>
class SequenceTask : ISoloTask
{
    public List<ISoloTask> Tasks = new List<ISoloTask>();

    public char GetCommand(State state)
    {
        foreach (var task in Tasks)
        {
            var cmd = task.GetCommand(state);
            if (cmd != 'X') return cmd;
        }
        // 全てのタスクで作業なしの場合
        return 'X';
    }
}

class MoveToXTask : ISoloTask
{
    readonly int HumanId;
    readonly int TargetX;
    readonly Grid Grid;

    public MoveToXTask(int humanId, int targetX, Grid grid)
    {
        HumanId = humanId;
        TargetX = targetX;
        Grid = grid;
    }

    public char GetCommand(State state)
    {
        var pos = state.HumanPos[HumanId];
        var x = Grid.Cells[pos].X;
        if (x == TargetX) return 'X';
        return Dfs(state, pos, TargetX);
    }

    char Dfs(State state, int srcPos, int targetX)
    {
        // 経路探索
        var queue = new Queue<int>();
        var distance = Enumerable.Repeat(int.MaxValue, 900).ToArray();
        queue.Enqueue(srcPos);
        distance[srcPos] = 0;
        var targetPos = -1;
        while (queue.Count > 0)
        {
            var pos = queue.Dequeue();
            foreach (var next in Grid.Cells[pos].RelationCells)
            {
                if (distance[next.Pos] > distance[pos] + 1 && state.CellState[next.Pos] != 1)
                {
                    distance[next.Pos] = distance[pos] + 1;
                    if (next.X == targetX)  // 目標に到達
                    {
                        targetPos = next.Pos;
                        queue.Clear();
                        break; 
                    }
                    queue.Enqueue(next.Pos);
                }
            }
        }
        // 経路復元
        var cmd = '?';
        while (targetPos != srcPos)
        {
            foreach (var next in Grid.Cells[targetPos].RelationCells)
            {
                if (distance[next.Pos] == distance[targetPos] - 1)
                {
                    if (next.Pos + 1 == targetPos) cmd = 'R';
                    if (next.Pos - 1 == targetPos) cmd = 'L';
                    if (next.Pos + 30 == targetPos) cmd = 'D';
                    if (next.Pos - 30 == targetPos) cmd = 'U';
                    targetPos = next.Pos;
                    break;
                }
            }
        }
        return cmd;
    }

}

/// <summary>幅3、高さ無制限の領域に事前設置壁を置く</summary>
class SetPreWallTask : ISoloTask
{
    readonly int HumanId;
    readonly Grid Grid;
    readonly int X;
    readonly MoveToXTask MoveToXTask;
    readonly AbstractTemplate Template;

    public SetPreWallTask(int humanId, int x, Grid grid, AbstractTemplate template)
    {
        HumanId = humanId;
        X = x;
        Grid = grid;
        MoveToXTask = new MoveToXTask(humanId, X, grid);
        Template = template;
    }

    public char GetCommand(State state)
    {
        // 終了済、もしくは新規加入お断りか
        if (IsComplete(state)) return 'X';

        var pos = state.HumanPos[HumanId];
        var x = Grid.Cells[pos].X;
        var y = Grid.Cells[pos].Y;

        // X位置が合っていなければ合わせる
        if (x != X) return MoveToXTask.GetCommand(state);

        // 今いる場所の横に壁を置けるなら置く
        var cmd = GetWallCommand(state);
        if (cmd != 'X') return cmd;   // 現設定では、"置く必要があって置けない場合"には待機する

        // 予定地の一番近い場所に向かう
        var upMinDistance = int.MaxValue;
        var upMaxDistance = int.MinValue;
        for (int i = y; i >= 0; i--)
        {
            if (ExistWallPlan(i, X - 1))
            {
                var targetPos = i * 30 + X - 1;
                if (!state.IsWall[targetPos])
                {
                    if(upMinDistance == int.MaxValue) upMinDistance = y - i;
                    upMaxDistance = y - i;
                }
            }
            if (ExistWallPlan(i, X + 1))
            {
                var targetPos = i * 30 + X + 1;
                if (!state.IsWall[targetPos])
                {
                    if (upMinDistance == int.MaxValue) upMinDistance = y - i;
                    upMaxDistance = y - i;
                }
            }
        }
        var downMinDistance = int.MaxValue;
        var downMaxDistance = int.MinValue;
        for (int i = y; i < 30; i++)
        {
            if (ExistWallPlan(i, X - 1))
            {
                var targetPos = i * 30 + X - 1;
                if (!state.IsWall[targetPos])
                {
                    if (downMinDistance == int.MaxValue) downMinDistance = i - y;
                    downMaxDistance = i - y;
                }
            }
            if (ExistWallPlan(i, X + 1))
            {
                var targetPos = i * 30 + X + 1;
                if (!state.IsWall[targetPos])
                {
                    if (downMinDistance == int.MaxValue) downMinDistance = i - y;
                    downMaxDistance = i - y;
                }
            }
        }
        if (upMinDistance == int.MaxValue)
        {
            return 'D';
        }
        else if (downMinDistance == int.MaxValue)
        {
            return 'U';
        }
        else
        {
            return (upMaxDistance < downMaxDistance)  ? 'U' : 'D';
        }
    }

    /// <summary>指定位置に壁を置く予定があるか(既に置いていてもtrue)</summary>
    bool ExistWallPlan(int row, int col)
    {
        if (col < 0 || col >= 30) return false;
        if (row < 0 || row >= 30) return false;
        return Template.IsPreWall[row * 30 + col];
    }

    bool IsComplete(State state)
    {
        if (state.PreWallMemberCol[HumanId] == X)
        {
            if (state.PreWallLeft[X] <= 0)
            { 
                // 担当から外れる
                state.PreWallMemberCount[X]--;
                state.PreWallMemberCol[HumanId] = -1;
                return true;
            }
        }
        else
        {
            if (state.PreWallLeft[X] <= state.PreWallMemberCount[X] * 30) return true;  // 担当が他にいたら入らないのつもりだけど、こういう書き方じゃないと動いてくれない（バグってそう）
            // 担当に入る
            state.PreWallMemberCount[X]++;
            state.PreWallMemberCol[HumanId] = X;
        }
        return false;
    }

    /// <summary>隣に壁を置けるなら置く。置く必要が無いなら'X'、置く必要があって今は置けないなら'.'。</summary>
    char GetWallCommand(State state)
    {
        var pos = state.HumanPos[HumanId];
        var y = Grid.Cells[pos].Y;
        var existWallPlan = false;
        if (ExistWallPlan(y, X - 1))
        {
            var targetPos = y * 30 + X - 1;
            if (!state.IsWall[targetPos]) existWallPlan = true;
            if (!state.IsWall[targetPos] && state.CellState[targetPos] != 2) return 'l';
        }
        if (ExistWallPlan(y, X + 1))
        {
            var targetPos = y * 30 + X + 1;
            if (!state.IsWall[targetPos]) existWallPlan = true;
            if (!state.IsWall[targetPos] && state.CellState[targetPos] != 2) return 'r';
        }
        return existWallPlan ? '.' : 'X';
    }
}

class TeamTask
{
    public Team Team;

    public TeamTask(Team team)
    {
        Team = team;
    }

    public char[] GetMoveCommand(State state, int col)
    {
        // メンバーがいない場合は処理を省略
        if (!Team.MemberIds.Any()) return new char[0];

        var commands = Enumerable.Repeat('X', Team.MemberIds.Count).ToArray();

        // 各メンバの移動先を決定
        var targets = new int[Team.MemberIds.Count];
        var distances = new int[Team.MemberIds.Count][];
        for (int i = 0; i < Team.MemberIds.Count; i++)
        {
            var targetPos = Team.TargetY[i] * 30 + state.Cages[col].BaseX;
            targets[i] = targetPos;

            var humanId = Team.MemberIds[i];
            var currentPos = state.HumanPos[humanId];
            distances[i] = state.GetDistance(currentPos);
        }

        var cTargets = state.GetBestCost(distances, targets).targets;
        // 移動先への経路を取得して移動
        for (int i = 0; i < Team.MemberIds.Count; i++)
        {
            var humanId = Team.MemberIds[i];
            if (state.IsMoved[humanId]) continue;
            var currentPos = state.HumanPos[humanId];
            var currentX = currentPos % 30;
            // X座標の同じ人が行動済なら自分も動かない(なるべく全員のX座標を合わせる)
            if (Team.MemberIds.Any(id => state.IsMoved[id] && state.HumanPos[id] % 30 == currentX)) continue;
            var targetPos = cTargets[i];
            var distance = distances[i];
            var direction = state.GetDirection(distance, currentPos, targetPos);
            commands[i] = direction;
        }

        return commands;
    }
}

class CloseDoorTask
{
    public char[] GetWallCommand(State state)
    {
        // 未行動メンバーの現在地を記録する
        var existHumanIdx = Enumerable.Repeat(-1, 900).ToArray();
        for (int i = 0; i < state.M; i++)
        {
            var humanId = i;
            if (state.IsMoved[humanId]) continue;
            existHumanIdx[state.HumanPos[humanId]] = i;
        }
        // 各部屋を閉じる条件が揃っていたら閉じる(良い部屋から貪欲に)
        var commands = Enumerable.Repeat('X', state.M).ToArray();
        while (true)
        {
            Cage bestCage = null;
            var bestScore = int.MinValue;
            foreach (var cage in state.Cages)
            {
                if (!cage.IsEnable || !cage.CanClose) continue;
                if (!cage.StandPos.All(pos => existHumanIdx[pos] != -1)) continue;
                if (state.EnableDogZone && cage.InnerPets.All(id => state.PetType[id] == 4)) continue;  // 犬ゾーンありの犬
                var score = 0;
                score += cage.InnerPets.Count(id => state.PetType[id] == 1) * 150;  // 牛
                score += cage.InnerPets.Count(id => state.PetType[id] == 2) * 100;  // 豚
                score += cage.InnerPets.Count(id => state.PetType[id] == 3) * 100;  // 兎
                score += cage.InnerPets.Count(id => state.PetType[id] == 4) * 80;   // 犬
                score += cage.InnerPets.Count(id => state.PetType[id] == 5) * 120;  // 猫
                if (cage.StandPos.Count == 1) score += 50;  // 端は高評価(端以外を使いすぎると良くないので)
                if (score > bestScore)
                {
                    bestCage = cage;
                    bestScore = score;
                }
            }
            if (bestCage == null) break;
            foreach (var standPos in bestCage.StandPos)
            {
                var humanIdx = existHumanIdx[standPos];
                commands[humanIdx] = bestCage.CloseWallPos.Contains(standPos + 30) ? 'd' : 'u';
                // 行動するので、他のcageを閉じる担当にはなれない
                existHumanIdx[standPos] = -1;
            }
        }

        return commands;
    }
}

/// <summary>完遂できなさそうな時のダメコン</summary>
class DamageControlTask
{
    DamageControlArea Area = null;

    public char[] GetMoveCommand(State state)
    {
        var commands = Enumerable.Repeat('X', state.M).ToArray();

        // 終盤で残り1匹の場合のみ実行する
        if (state.LeftPetId.Count != 1) return commands;
        if (state.Turn < 280) return commands;
        if (state.IsMoved.Any(v => v)) return commands;

        var bestArea = GetBestArea(state);
        Area = bestArea;

        if (bestArea == null) return commands;

        var humanDistance = Enumerable.Range(0, state.M).Select(id => state.GetDistance(state.HumanPos[id])).ToArray();

        // 余計な動きをしないため、全員待機を初期状態にする
        commands = Enumerable.Repeat('.', state.M).ToArray();

        // 壁設置のための目的地を目指す
        for (int i = 0; i < bestArea.StandPos.Count; i++)
        {
            var humanId = bestArea.StandHumanIds[i];
            var dir = state.GetDirection(humanDistance[humanId], state.HumanPos[humanId], bestArea.StandPos[i]);
            commands[humanId] = dir;
        }

        // 壁予定地を踏んでいたら、隣に目的地があるので出る
        for (int i = 0; i < state.M; i++)
        {
            if (commands[i] != '.') continue;
            if (!bestArea.WallPos.Contains(state.HumanPos[i])) continue;
            // memo 目的地が左右端にならなければ座標オーバーは無い
            if (bestArea.StandPos.Contains(state.HumanPos[i] - 1)) commands[i] = 'L';
            if (bestArea.StandPos.Contains(state.HumanPos[i] + 1)) commands[i] = 'R';
            if (bestArea.StandPos.Contains(state.HumanPos[i] - 30)) commands[i] = 'U';
            if (bestArea.StandPos.Contains(state.HumanPos[i] + 30)) commands[i] = 'D';
        }

        // 領域の外に出るための移動（299ターン目以降に壁予定地を踏むのは禁止）
        // TODO 2ターン後に動物が外に出そうな場合、移動を諦める
        if (state.Turn < 298)
        {
            for (int i = 0; i < state.M; i++)
            {
                if (commands[i] != '.') continue;
                var target = bestArea.StandPos.Select(pos => (pos, humanDistance[i][pos])).OrderBy(ipos => ipos.Item2).First().pos;
                var dir = state.GetDirection(humanDistance[i], state.HumanPos[i], target);
                commands[i] = dir;
            }
        }

        return commands;
    }

    public char[] GetWallCommand(State state)
    {
        var commands = Enumerable.Repeat('X', state.M).ToArray();

        // 目途が立っている場合のみ実行する
        if (Area == null) return commands;

        // 壁が設置可能か
        if (Area.WallPos.Any(pos => state.CellState[pos] == 2)) return commands;
        // 中に動物がいるか
        if (!state.LeftPetId.All(id => Area.InnerPos.Contains(state.PetPos[id]))) return commands;

        // 未行動メンバーの現在地を記録する
        var existHumanIdx = Enumerable.Repeat(-1, 900).ToArray();
        for (int i = 0; i < state.M; i++)
        {
            var humanId = i;
            if (state.IsMoved[humanId]) continue;
            existHumanIdx[state.HumanPos[humanId]] = i;
        }
        // 前提位置に人が立っているか
        if (!Area.StandPos.All(pos => existHumanIdx[pos] != -1)) return commands;

        // 中に人がいないかを任意でチェックしてもいい
        // ターンが残っていて動物が出てくるリスクも無ければ待つべき

        // memo 同じ座標から複数箇所に壁を置く配置にはならない
        foreach (var standPos in Area.StandPos)
        {
            var humanIdx = existHumanIdx[standPos];
            if (Area.WallPos.Contains(standPos - 1)) commands[humanIdx] = 'l';
            if (Area.WallPos.Contains(standPos + 1)) commands[humanIdx] = 'r';
            if (Area.WallPos.Contains(standPos - 30)) commands[humanIdx] = 'u';
            if (Area.WallPos.Contains(standPos + 30)) commands[humanIdx] = 'd';
            // 行動するので、他のcageを閉じる担当にはなれない
            existHumanIdx[standPos] = -1;
        }

        return commands;
    }

    DamageControlArea GetBestArea(State state)
    {
        // 幅10 * 高さ7 * 範囲4 * 上下通路有無4 = 1080パターン、詳しい計算は範囲内にペットがいるケースのみで、実行は終盤のみなので耐えられるはず
        var petDistance = state.GetDistance(state.PetPos[state.LeftPetId[0]]);
        var humanDistance = Enumerable.Range(0, state.M).Select(id => state.GetDistance(state.HumanPos[id])).ToArray();
        DamageControlArea bestArea = null;
        var bestCost = int.MaxValue;
        for (int left = 0; left < state.CageColNum; left++)
        {
            for (int right = left; right < state.CageColNum; right++)
            {
                if (right >= left + 4) continue; // 幅4部屋まで
                for (int top = 0; top < state.CageRowNum; top++)
                {
                    for (int bottom = top; bottom < state.CageRowNum; bottom++)
                    {
                        if (bottom >= top + 4) continue; // 高さ4部屋+その上下の通路まで
                        if ((right - left + 1) * (bottom - top + 1) > 6) continue;  // 幅*高さで6部屋まで
                        var areas = GetAreas(state, left, right, top, bottom);
                        foreach (var area in areas)
                        {
                            if (!IsValidArea(state, area, petDistance, humanDistance)) continue;
                            var cost = area.Cost; 
                            if (cost < bestCost)
                            {
                                bestArea = area;
                                bestCost = cost;
                            }
                        }
                    }
                }
            }
        }
        return bestArea;
    }

    List<DamageControlArea> GetAreas(State state, int cageLeft, int cageRight, int cageTop, int cageBottom)
    {
        var res = new List<DamageControlArea>();

        // ペットが範囲内かチェック(各部屋の内部座標のMin,Maxを境界にする)
        var left = state.Cages[cageLeft].InnerPos.Min(pos => pos % 30);
        var right = state.Cages[cageRight].InnerPos.Max(pos => pos % 30);
        var top = state.Cages[cageTop * state.CageColNum].InnerPos.Min(pos => pos / 30);
        var bottom = state.Cages[cageBottom * state.CageColNum].InnerPos.Max(pos => pos / 30);
        var petX = state.PetPos[state.LeftPetId[0]] % 30;
        var petY = state.PetPos[state.LeftPetId[0]] / 30;
        if (left > petX || petX > right) return res;
        if (top > petY || petY > bottom) return res;

        // 上下端通路の使用可否、可なら使用するべきかをチェックする(さらに1つ上か下の部屋が全て閉じているか)
        var enableTopPassway = true;
        if (cageTop != 0)
        {
            enableTopPassway = !state.Cages
                .Any(cage => cage.CageRow == cageTop - 1 && cage.CageCol >= cageLeft && cage.CageCol <= cageRight && cage.CloseWallPos.Any());
        }
        var enableBottomPassway = true;
        if (cageBottom != state.CageRowNum - 1)
        {
            enableBottomPassway = !state.Cages
                .Any(cage => cage.CageRow == cageBottom + 1 && cage.CageCol >= cageLeft && cage.CageCol <= cageRight && cage.CloseWallPos.Any());
        }

        // 必ず閉じるのは、間の段の左右両端。
        var closePos_Common = new List<int>();
        var standPos_Common = new List<int>();
        for (int i = cageTop; i < cageBottom; i++)
        {
            closePos_Common.AddRange(GetLeftRightWallPos(state, i, cageLeft, cageRight));
            standPos_Common.AddRange(GetLeftRightStandPos(state, i, cageLeft, cageRight));
        }

        // 上端の通路を使う場合に閉じるのは、上端の通路の左右両端。
        var closePos_Top1 = (cageTop == 0) ? new List<int>() : GetLeftRightWallPos(state, cageTop - 1, cageLeft, cageRight);
        var standPos_Top1 = (cageTop == 0) ? new List<int>() : GetLeftRightStandPos(state, cageTop - 1, cageLeft, cageRight);
        // 上端の通路を使わない場合に閉じるのは、上端の部屋の各閉じる位置。（上側を閉じることにする）
        var topCages = state.Cages
            .Where(c => c.CloseWallPos.Any() && c.CageRow == cageTop && c.CageCol >= cageLeft && c.CageCol <= cageRight)
            .ToList();
        var closePos_Top2 = (cageTop == 0) ? new List<int>() : topCages.Select(c => c.CloseWallPos.Min()).ToList();
        var standPos_Top2 = (cageTop == 0) ? new List<int>() : topCages.Select(c => c.StandPos.Min()).ToList();

        // 下端の通路を使う場合に閉じるのは、下端の通路の左右両端。
        var closePos_Bottom1 = (cageBottom == state.CageRowNum - 1) ? new List<int>() : GetLeftRightWallPos(state, cageBottom, cageLeft, cageRight);
        var standPos_Bottom1 = (cageBottom == state.CageRowNum - 1) ? new List<int>() : GetLeftRightStandPos(state, cageBottom, cageLeft, cageRight);
        // 下端の通路を使わない場合に閉じるのは、下端の部屋の各閉じる位置。（下側を閉じることにする）
        var bottomCages = state.Cages
            .Where(c => c.CloseWallPos.Any() && c.CageRow == cageBottom && c.CageCol >= cageLeft && c.CageCol <= cageRight)
            .ToList();
        var closePos_Bottom2 = (cageBottom == state.CageRowNum - 1) ? new List<int>() : bottomCages.Select(c => c.CloseWallPos.Max()).ToList();
        var standPos_Bottom2 = (cageBottom == state.CageRowNum - 1) ? new List<int>() : bottomCages.Select(c => c.StandPos.Max()).ToList();

        // memo 上下端の通路を使わないケースで範囲内に動物がいることをチェックしているので、領域を拡張しても範囲内にいることは保証される

        // コスト 概算で、未使用部屋の広さ + 間の通路の広さ(実装時間が足りないため、厳密な計算ではない)
        var commonCost = state.Cages
            .Where(c => c.CageCol >= cageLeft && c.CageCol <= cageRight && c.CageRow >= cageTop && c.CageRow <= cageBottom && c.CloseWallPos.Any())
            .Sum(c => c.InnerPos.Count + c.CloseWallPos.Count);
        commonCost += (cageBottom - cageTop) * (right - left);

        if (cageTop != cageBottom) res.Add(new DamageControlArea()
        {
            WallPos = closePos_Common.Concat(closePos_Top2).Concat(closePos_Bottom2).ToList(),
            StandPos = standPos_Common.Concat(standPos_Top2).Concat(standPos_Bottom2).ToList(),
            Cost = commonCost,
        });

        if (enableTopPassway) res.Add(new DamageControlArea()
        {
            WallPos = closePos_Common.Concat(closePos_Top1).Concat(closePos_Bottom2).ToList(),
            StandPos = standPos_Common.Concat(standPos_Top1).Concat(standPos_Bottom2).ToList(),
            Cost = commonCost + (right - left),
        });

        if (enableBottomPassway) res.Add(new DamageControlArea()
        {
            WallPos = closePos_Common.Concat(closePos_Top2).Concat(closePos_Bottom1).ToList(),
            StandPos = standPos_Common.Concat(standPos_Top2).Concat(standPos_Bottom1).ToList(),
            Cost = commonCost + (right - left),
        });

        if (enableTopPassway && enableBottomPassway) res.Add(new DamageControlArea() { 
            WallPos = closePos_Common.Concat(closePos_Top1).Concat(closePos_Bottom1).ToList(),
            StandPos = standPos_Common.Concat(standPos_Top1).Concat(standPos_Bottom1).ToList(),
            Cost = commonCost + (right - left) * 2,
        });

        return res;
    }

    /// <summary>通路の左右端の壁設置座標を取得する</summary>
    /// <param name="row">何番目の通路か</param>
    List<int> GetLeftRightWallPos(State state, int row, int cageLeft, int cageRight)
    {
        var res = new List<int>();
        // 左 一番下にある前提壁の1個下(cage自体が左端なら不要)
        if (cageLeft > 0) res.Add(state.Cages[row * state.CageColNum + cageLeft].PreWallPos.Max() + (cageLeft != 9 ? 30 : 28));
        // 右 一番下にある前提壁の右右下(cage自体が右端なら不要)
        if (cageLeft < state.CageColNum - 1) res.Add(state.Cages[row * state.CageColNum + cageRight].PreWallPos.Max() + 32);
        return res;
    }

    /// <summary>通路の左右端の壁設置座標を取得する</summary>
    /// <param name="row">何番目の通路か</param>
    List<int> GetLeftRightStandPos(State state, int row, int cageLeft, int cageRight)
    {
        var res = new List<int>();
        // 左 一番下にある前提壁の1個左下(cage自体が左端なら不要)
        if (cageLeft > 0) res.Add(state.Cages[row * state.CageColNum + cageLeft].PreWallPos.Max() + (cageLeft != 9 ? 29 : 27));
        // 右 一番下にある前提壁の右右右下(cage自体が右端なら不要)
        if (cageLeft < state.CageColNum - 1) res.Add(state.Cages[row * state.CageColNum + cageRight].PreWallPos.Max() + 33);
        return res;
    }

    /// <summary>1ターンにこれくらい進みそうの値(100倍値)</summary>
    /// <remarks>最大速度では判定が厳しすぎるので、少し遅めとみなす。</remarks>
    readonly int[] TypeSpeed = new int[] { 0, 70, 100, 100, 120, 120 };
    bool IsValidArea(State state, DamageControlArea area, int[] petDistance, int[][] humanDistance)
    {
        if (area.WallPos.Count > state.M) return false;

        // 動物が壁予定地の隣に来るまでの距離
        var limitDistance = area.WallPos.Min(pos => petDistance[pos]) - 1;
        // この回数だけ行動可能（移動＋壁設置に間に合えば良し）
        // 「petが壁予定地の隣に来るまでの時間」「300ターンまで」の少ない方がリミット
        var limitTurn = 300 - state.Turn;        
        limitTurn = Math.Min(limitTurn, limitDistance * 100 / TypeSpeed[state.PetType[state.LeftPetId[0]]]);

        // 外への退避が間に合わない人の数(一定数まで許容)
        // TODO 領域外の人はカウントしない
        var cannotEscapeCount = Enumerable.Range(0, state.M).Count(id => !area.StandPos.Any(pos => humanDistance[id][pos] <= limitTurn - 1));
        //if (cannotEscapeCount * 5 > state.M) return false;

        // 各立ち位置に間に合う人から担当を決める。
        area.StandHumanIds = GetStandHumanIds(state, area, humanDistance, limitTurn);
        if (area.StandHumanIds == null) return false;

        area.InnerPos = GetInnerPos(state, area);
        area.Cost = area.InnerPos.Count;

        return true;
    }

    List<int> GetStandHumanIds(State state, DamageControlArea area, int[][] humanDistance, int limitTurn)
    {
        // 0:source
        // 1:target
        // 2~:HumanId
        // 2+M~:TargetPosIdx
        List<Edge> edges = new List<Edge>();
        edges.AddRange(area.StandPos.Select((pos, i) => new Edge() { SourcePoint = 2 + state.M + i, TargetPoint = 1, Value = 1 }));
        for (int humanId = 0; humanId < state.M; humanId++)
        {
            edges.Add(new Edge() { SourcePoint = 0, TargetPoint = 2 + humanId, Value = 1 });
            for (int standPosIdx = 0; standPosIdx < area.StandPos.Count; standPosIdx++)
            {
                if (humanDistance[humanId][area.StandPos[standPosIdx]] > limitTurn - 1) continue;
                edges.Add(new Edge() { SourcePoint = 2 + humanId, TargetPoint = 2 + state.M + standPosIdx, Value = 1 });
            }
        }

        var maxflow = new MaxFlow(edges, 2 + state.M + area.StandPos.Count);
        var flow = maxflow.GetMaxFlow(0, 1);
        if (flow < area.StandPos.Count) return null;

        // 残余グラフの[目的地→人]が残っていれば担当
        var res = new List<int>();
        for (int standPosIdx = 0; standPosIdx < area.StandPos.Count; standPosIdx++)
        {
            for (int humanId = 0; humanId < state.M; humanId++)
            {
                if (!maxflow.ResidualCapacity.ContainsKey(2 + state.M + standPosIdx)) continue;
                if (!maxflow.ResidualCapacity[2 + state.M + standPosIdx].ContainsKey(2 + humanId)) continue;
                if (maxflow.ResidualCapacity[2 + state.M + standPosIdx][2 + humanId] == 0) continue;
                res.Add(humanId);
                break;
            }
        }
        return res;
    }

    List<int> GetInnerPos(State state, DamageControlArea area)
    {
        var distance = GetDistance(state, state.PetPos[state.LeftPetId[0]], area.WallPos);
        var res = new List<int>();
        for (int i = 0; i < 900; i++)
        {
            if (distance[i] < int.MaxValue) res.Add(i);
        }
        return res;
    }

    public int[] GetDistance(State state, int srcPos, List<int> stopPos)
    {
        var queue = new Queue<int>();
        var distance = Enumerable.Repeat(int.MaxValue, 900).ToArray();
        queue.Enqueue(srcPos);
        distance[srcPos] = 0;
        while (queue.Count > 0)
        {
            var pos = queue.Dequeue();
            foreach (var next in state.Grid.Cells[pos].RelationCells)
            {
                if (distance[next.Pos] > distance[pos] + 1 && state.CellState[next.Pos] != 1)
                {
                    if (stopPos.Contains(next.Pos)) continue;
                    distance[next.Pos] = distance[pos] + 1;
                    queue.Enqueue(next.Pos);
                }
            }
        }
        return distance;
    }

}

/// <summary>犬ゾーンによる犬の全頭捕獲</summary>
class DogZoneTask
{
    public Team Team;
    int HumanTop = 4;
    int HumanBottom = 25;

    public char[] GetWallCommand(State state)
    {
        // メンバーがいない場合は処理を省略
        if (!Team.MemberIds.Any()) return new char[0];

        var commands = Enumerable.Repeat('X', Team.MemberIds.Count).ToArray();
        // 設置済か
        if (!state.EnableDogZone) return commands;
        // 壁設置可能か
        if (state.CellState[(HumanTop + 1) * 30 + 29] == 2) return commands;
        if (state.CellState[(HumanBottom - 1) * 30 + 29] == 2) return commands;
        // 前提壁が全て設置済か
        for (int i = HumanTop; i <= HumanBottom; i++)
        {
            if (!state.IsWall[i * 30 + 28]) return commands;
        }
        // 必要な場所に人がいるか
        if (!Team.MemberIds.Any(id => state.HumanPos[id] == HumanTop * 30 + 29 && !state.IsMoved[id])) return commands;
        if (!Team.MemberIds.Any(id => state.HumanPos[id] == HumanBottom * 30 + 29 && !state.IsMoved[id])) return commands;
        // 不要な場所に人がいないか
        if (Team.MemberIds
            .Any(id => state.HumanPos[id] % 30 == 29 && state.HumanPos[id] / 30 > HumanTop && state.HumanPos[id] / 30 < HumanBottom)) return commands;
        // 犬が全て範囲内にいるか
        if (!state.LeftPetId
            .Where(id => state.PetType[id] == 4)
            .All(id => state.PetPos[id] % 30 == 29 && state.PetPos[id] / 30 > HumanTop && state.PetPos[id] / 30 < HumanBottom)) return commands;

        var pets = state.LeftPetId
            .Where(id => state.PetPos[id] % 30 == 29 && state.PetPos[id] / 30 > HumanTop && state.PetPos[id] / 30 < HumanBottom).ToList();
        pets.ForEach(id => state.LeftPetCount[state.PetType[id]]--);
        pets.ForEach(id => state.LeftPetId.Remove(id));
        state.EnableDogZone = false;

        var upHumanId = Team.MemberIds.First(id => state.HumanPos[id] == HumanTop * 30 + 29 && !state.IsMoved[id]);
        var downHumanId = Team.MemberIds.First(id => state.HumanPos[id] == HumanBottom * 30 + 29 && !state.IsMoved[id]);
        for (int i = 0; i < Team.MemberIds.Count; i++)
        {
            var humanId = Team.MemberIds[i];
            if (humanId == upHumanId) commands[i] = 'd';
            if (humanId == downHumanId) commands[i] = 'u';
        }
        return commands;
    }

    public char[] GetMoveCommand(State state)
    {
        // メンバーが全員揃っていない場合は動作しない
        if (Team.MemberIds.Count < state.M) return Enumerable.Repeat('X', Team.MemberIds.Count).ToArray();

        var upPos = HumanTop * 30 + 29;
        var downPos = HumanBottom * 30 + 29;

        var distances = Enumerable.Range(0, state.M).Select(id => state.GetDistance(state.HumanPos[id])).ToArray();

        // 分割パターン自体は全員を考慮する
        // 2^10 = 1024。数のバランスが悪いものを省けばもっと削れる。(10C5)
        var bestPattern = -1;
        var bestCost = int.MaxValue;
        var patternNum = 1 << state.M;
        for (var i = 0; i < patternNum; i++)
        {
            var downCount = BitOperations.PopCount((uint)i);
            if (downCount != state.M / 2 && downCount != (state.M + 1) / 2) continue;   // 人数バランスが悪ければ不採用
            var maxCost = int.MinValue;
            for (int j = 0; j < state.M; j++)
            {
                maxCost = Math.Max(maxCost, distances[j][(i & (1 << j)) == 0 ? upPos : downPos]);
            }
            if (maxCost < bestCost)
            {
                bestPattern = i;
                bestCost = maxCost;
            }
        }

        // 移動先への経路を取得して移動
        var commands = Enumerable.Repeat('X', Team.MemberIds.Count).ToArray();
        for (int i = 0; i < Team.MemberIds.Count; i++)
        {
            var humanId = Team.MemberIds[i];
            if (state.IsMoved[humanId]) continue;
            var currentPos = state.HumanPos[humanId];
            var targetPos = (bestPattern & (1 << humanId)) == 0 ? upPos : downPos;
            var distance = distances[humanId];
            var direction = state.GetDirection(distance, currentPos, targetPos);
            commands[i] = direction;
        }
        return commands;
    }
}

abstract class AbstractTemplate
{
    public int CageNum => CageColNum * CageRowNum;
    public abstract int CageRowNum { get; }
    public abstract int CageColNum { get; }
    public abstract bool[] IsPreWall { get; }
    public abstract bool HasDogZone { get; }

    public abstract Cage[] GetCages();

    public abstract List<Team> GetTeams(int m, Cage[] cages);

    protected void TryAddPos(List<int> list, int x, int y)
    {
        if (y < 0 || y >= 30) return;
        if (x < 0 || x >= 30) return;
        list.Add(y * 30 + x);
    }

    protected Team GetTeam(int capacity, Cage[] cages, int[] cageRows, int[] rows)
    {
        var res = new Team() { Capacity = capacity };
        res.TargetY.AddRange(rows);
        res.TargetCages = new List<Cage>[capacity + 1][];
        for (int i = 0; i <= capacity; i++)
        {
            res.TargetCages[i] = new List<Cage>[CageColNum];
            for (int col = 0; col < CageColNum; col++)
            {
                res.TargetCages[i][col] = new List<Cage>();
                for (int k = 0; k < i; k++)
                {
                    if (cageRows[k] == -1) continue;
                    var cage = cages[cageRows[k] * CageColNum + col];
                    res.TargetCages[i][col].Add(cage);
                }
            }
        }
        res.BaseTargetYMin = res.TargetY.Min();
        res.BaseTargetYMax = res.TargetY.Max();
        return res;
    }
}

class Row5Template : AbstractTemplate
{
    public override int CageRowNum => 5;

    public override int CageColNum => 10;

    public override bool[] IsPreWall => isPreWall;

    public override bool HasDogZone => false;

    readonly bool[] isPreWall = new bool[]
    {
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
    };

    public override Cage[] GetCages()
    {
        var cages = new Cage[CageNum];
        for (int row = 0; row < CageRowNum; row++)
        {
            for (int col = 0; col < CageColNum; col++)
            {
                var isRightEnd = col == CageColNum - 1;
                var left = col * 3 - 1;
                var top = row * 7 - 3;
                var cage = new Cage() { CageRow = row, CageCol = col };
                cages[row * CageColNum + col] = cage;
                cage.Id = row * CageColNum + col;
                cage.BaseX = left + 2;
                TryAddPos(cage.StandPos, left + 2, top);
                TryAddPos(cage.StandPos, left + 2, top + 7);
                TryAddPos(cage.CloseWallPos, left + 2, top + 1);
                TryAddPos(cage.CloseWallPos, left + 2, top + 6);
                // 上下
                TryAddPos(cage.PreWallPos, left + 1, top + 1);
                TryAddPos(cage.PreWallPos, left + 1, top + 6);
                // 左辺
                TryAddPos(cage.PreWallPos, left, top + 2);
                TryAddPos(cage.PreWallPos, left, top + 3);
                TryAddPos(cage.PreWallPos, left, top + 4);
                TryAddPos(cage.PreWallPos, left, top + 5);
                if (isRightEnd)
                {
                    // 上下(追加)
                    TryAddPos(cage.PreWallPos, left + 3, top + 1);
                    TryAddPos(cage.PreWallPos, left + 3, top + 6);
                }
                else
                {
                    // 右辺
                    TryAddPos(cage.PreWallPos, left + 3, top + 2);
                    TryAddPos(cage.PreWallPos, left + 3, top + 3);
                    TryAddPos(cage.PreWallPos, left + 3, top + 4);
                    TryAddPos(cage.PreWallPos, left + 3, top + 5);
                }
                for (int rowP = 2; rowP <= 5; rowP++)
                {
                    for (int colP = 1; colP <= (isRightEnd ? 3 : 2); colP++)
                    {
                        TryAddPos(cage.InnerPos, left + colP, top + rowP);
                    }
                }
                cage.LeftPreWallCount = cage.PreWallPos.Count;
            }
        }
        return cages;
    }

    public override List<Team> GetTeams(int m, Cage[] cages)
    {
        var teams = new List<Team>();
        switch (m)
        {
            case 5:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 4, 11, 18 }));
                teams.Add(GetTeam(2, cages, new int[] { 4, 3 }, new int[] { 25, 18 }));
                return teams;
            case 6:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 4, 11, 18 }));
                teams.Add(GetTeam(3, cages, new int[] { 4, 3, 2 }, new int[] { 25, 18, 11 }));
                return teams;
            case 7:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 4, 11, 18 }));
                teams.Add(GetTeam(4, cages, new int[] { 4, 3, 2, 1 }, new int[] { 25, 18, 11, 4 }));
                return teams;
            case 8:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 4, 11, 18 }));
                teams.Add(GetTeam(3, cages, new int[] { 4, 3, 2 }, new int[] { 25, 18, 11 }));
                teams.Add(GetTeam(2, cages, new int[] { 4, 3 }, new int[] { 25, 18 }));
                return teams;
            case 9:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 4, 11, 18 }));
                teams.Add(GetTeam(3, cages, new int[] { 4, 3, 2 }, new int[] { 25, 18, 11 }));
                teams.Add(GetTeam(3, cages, new int[] { 4, 3, 2 }, new int[] { 25, 18, 11 }));
                return teams;
            case 10:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 4, 11, 18 }));
                teams.Add(GetTeam(3, cages, new int[] { 4, 3, 2 }, new int[] { 25, 18, 11 }));
                teams.Add(GetTeam(4, cages, new int[] { 4, 3, 2, 1 }, new int[] { 25, 18, 11, 4 }));
                return teams;
        }
        throw new ArgumentException();
    }
}

class Row6Template : AbstractTemplate
{
    public override int CageRowNum => 6;

    public override int CageColNum => 10;

    public override bool[] IsPreWall => isPreWall;

    public override bool HasDogZone => false;

    readonly bool[] isPreWall = new bool[]
    {
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
    };

    public override Cage[] GetCages()
    {
        var cages = new Cage[CageNum];
        for (int row = 0; row < CageRowNum; row++)
        {
            for (int col = 0; col < CageColNum; col++)
            {
                var isRightEnd = col == CageColNum - 1;
                var left = col * 3 - 1;
                var top = row * 6 - 3;
                var cage = new Cage() { CageRow = row, CageCol = col };
                cages[row * CageColNum + col] = cage;
                cage.Id = row * CageColNum + col;
                cage.BaseX = left + 2;
                TryAddPos(cage.StandPos, left + 2, top);
                TryAddPos(cage.StandPos, left + 2, top + 6);
                TryAddPos(cage.CloseWallPos, left + 2, top + 1);
                TryAddPos(cage.CloseWallPos, left + 2, top + 5);
                // 上下
                TryAddPos(cage.PreWallPos, left + 1, top + 1);
                TryAddPos(cage.PreWallPos, left + 1, top + 5);
                // 左辺
                TryAddPos(cage.PreWallPos, left, top + 2);
                TryAddPos(cage.PreWallPos, left, top + 3);
                TryAddPos(cage.PreWallPos, left, top + 4);
                if (isRightEnd)
                {
                    // 上下(追加)
                    TryAddPos(cage.PreWallPos, left + 3, top + 1);
                    TryAddPos(cage.PreWallPos, left + 3, top + 5);
                }
                else
                {
                    // 右辺
                    TryAddPos(cage.PreWallPos, left + 3, top + 2);
                    TryAddPos(cage.PreWallPos, left + 3, top + 3);
                    TryAddPos(cage.PreWallPos, left + 3, top + 4);
                }
                for (int rowP = 2; rowP <= 4; rowP++)
                {
                    for (int colP = 1; colP <= (isRightEnd ? 3 : 2); colP++)
                    {
                        TryAddPos(cage.InnerPos, left + colP, top + rowP);
                    }
                }
                cage.LeftPreWallCount = cage.PreWallPos.Count;
            }
        }
        return cages;
    }

    public override List<Team> GetTeams(int m, Cage[] cages)
    {
        var teams = new List<Team>();
        switch (m)
        {
            case 5:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 3, 9, 15 }));
                teams.Add(GetTeam(2, cages, new int[] { 5, 4 }, new int[] { 27, 21 }));
                return teams;
            case 6:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 3, 9, 15 }));
                teams.Add(GetTeam(3, cages, new int[] { 5, 4, 3 }, new int[] { 27, 21, 15 }));
                return teams;
            case 7:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 3, 9, 15 }));
                teams.Add(GetTeam(4, cages, new int[] { 5, 4, 3, 2 }, new int[] { 27, 21, 15, 9 }));
                return teams;
            case 8:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 3, 9, 15 }));
                teams.Add(GetTeam(3, cages, new int[] { 5, 4, 3 }, new int[] { 27, 21, 15 }));
                teams.Add(GetTeam(2, cages, new int[] { 5, 4 }, new int[] { 27, 21 }));
                return teams;
            case 9:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 3, 9, 15 }));
                teams.Add(GetTeam(3, cages, new int[] { 5, 4, 3 }, new int[] { 27, 21, 15 }));
                teams.Add(GetTeam(3, cages, new int[] { 5, 4, 3 }, new int[] { 27, 21, 15 }));
                return teams;
            case 10:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 3, 9, 15 }));
                teams.Add(GetTeam(3, cages, new int[] { 5, 4, 3 }, new int[] { 27, 21, 15 }));
                teams.Add(GetTeam(4, cages, new int[] { 5, 4, 3, 2 }, new int[] { 27, 21, 15, 9 }));
                return teams;
        }
        throw new ArgumentException();
    }
}

class Row7Template : AbstractTemplate
{
    public override int CageRowNum => 7;

    public override int CageColNum => 10;

    public override bool[] IsPreWall => isPreWall;

    public override bool HasDogZone => false;

    readonly bool[] isPreWall = new bool[]
    {
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
        true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
    };

    public override Cage[] GetCages()
    {
        var cages = new Cage[CageNum];
        for (int row = 0; row < CageRowNum; row++)
        {
            for (int col = 0; col < CageColNum; col++)
            {
                var isRightEnd = col == CageColNum - 1;
                var left = col * 3 - 1;
                var top = row * 5 - 3;
                var cage = new Cage() { CageRow = row, CageCol = col };
                cages[row * CageColNum + col] = cage;
                cage.Id = row * CageColNum + col;
                cage.BaseX = left + 2;
                TryAddPos(cage.StandPos, left + 2, top);
                TryAddPos(cage.StandPos, left + 2, top + 5);
                TryAddPos(cage.CloseWallPos, left + 2, top + 1);
                TryAddPos(cage.CloseWallPos, left + 2, top + 4);
                // 上下
                TryAddPos(cage.PreWallPos, left + 1, top + 1);
                TryAddPos(cage.PreWallPos, left + 1, top + 4);
                // 左辺
                TryAddPos(cage.PreWallPos, left, top + 2);
                TryAddPos(cage.PreWallPos, left, top + 3);
                if (isRightEnd)
                {
                    // 上下(追加)
                    TryAddPos(cage.PreWallPos, left + 3, top + 1);
                    TryAddPos(cage.PreWallPos, left + 3, top + 4);
                }
                else
                {
                    // 右辺
                    TryAddPos(cage.PreWallPos, left + 3, top + 2);
                    TryAddPos(cage.PreWallPos, left + 3, top + 3);
                }
                for (int rowP = 2; rowP <= 3; rowP++)
                {
                    for (int colP = 1; colP <= (isRightEnd ? 3 : 2); colP++)
                    {
                        TryAddPos(cage.InnerPos, left + colP, top + rowP);
                    }
                }
                cage.LeftPreWallCount = cage.PreWallPos.Count;
            }
        }
        return cages;
    }

    public override List<Team> GetTeams(int m, Cage[] cages)
    {
        var teams = new List<Team>();
        switch (m)
        {
            case 5:
                // 人数不足
                throw new ArgumentException();
            case 6:
                // 人数不足(でもないけどチーム同士の結合を考えてないので12～17行目の対処ができない)
                throw new ArgumentException();
            case 7:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 2, 7, 12 }));
                teams.Add(GetTeam(4, cages, new int[] { 6, 5, 4, 3 }, new int[] { 27, 22, 17, 12 }));
                return teams;
            case 8:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 2, 7, 12 }));
                teams.Add(GetTeam(3, cages, new int[] { 6, 5, 4 }, new int[] { 27, 22, 17 }));
                teams.Add(GetTeam(2, cages, new int[] { -1, 3 }, new int[] { 12, 17 }));
                return teams;
            case 9:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 2, 7, 12 }));
                teams.Add(GetTeam(3, cages, new int[] { 6, 5, 4 }, new int[] { 27, 22, 17 }));
                teams.Add(GetTeam(3, cages, new int[] { -1, 3, 4 }, new int[] { 12, 17, 22 }));
                return teams;
            case 10:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 2, 7, 12 }));
                teams.Add(GetTeam(3, cages, new int[] { 6, 5, 4 }, new int[] { 27, 22, 17 }));
                teams.Add(GetTeam(4, cages, new int[] { -1, 2, 3, 4 }, new int[] { 7, 12, 17, 22 }));
                return teams;
        }
        throw new ArgumentException();
    }
}

// memo 6人 かつ Pet - 犬 <= 7 くらいじゃないと使えない？
class Row5DogTemplate : AbstractTemplate
{
    public override int CageRowNum => 5;

    public override int CageColNum => 10;

    public override bool[] IsPreWall => isPreWall;

    public override bool HasDogZone => true;

    readonly bool[] isPreWall = new bool[]
    {
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true, false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true, false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true, false,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true, false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true, false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true, false,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true, false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true, false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,true, false,
        false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true, false,
        false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,false,
        false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,true, false,false,false,false,
    };

    public override Cage[] GetCages()
    {
        var cages = new Cage[CageNum];
        for (int row = 0; row < CageRowNum; row++)
        {
            for (int col = 0; col < CageColNum; col++)
            {
                var isRightEnd = col == CageColNum - 1;
                var left = col * 3 - 2;
                var top = row * 7 - 3;
                var cage = new Cage() { CageRow = row, CageCol = col };
                cages[row * CageColNum + col] = cage;
                cage.Id = row * CageColNum + col;
                cage.BaseX = left + 2;
                TryAddPos(cage.StandPos, left + 2, top);
                TryAddPos(cage.StandPos, left + 2, top + 7);
                TryAddPos(cage.CloseWallPos, left + 2, top + 1);
                TryAddPos(cage.CloseWallPos, left + 2, top + 6);
                // 上下
                TryAddPos(cage.PreWallPos, left + 1, top + 1);
                TryAddPos(cage.PreWallPos, left + 1, top + 6);
                // 左辺
                TryAddPos(cage.PreWallPos, left, top + 2);
                TryAddPos(cage.PreWallPos, left, top + 3);
                TryAddPos(cage.PreWallPos, left, top + 4);
                TryAddPos(cage.PreWallPos, left, top + 5);
                if (isRightEnd)
                {
                    // 上下(追加) は無し(必要な分は犬ゾーンで既に設置済の前提)
                    if (row == 0)
                    {
                        TryAddPos(cage.InnerPos, left + 3, top + 2);
                        TryAddPos(cage.InnerPos, left + 3, top + 3);
                        TryAddPos(cage.InnerPos, left + 3, top + 4);
                        TryAddPos(cage.InnerPos, left + 3, top + 5);
                        TryAddPos(cage.InnerPos, left + 3, top + 6);
                        TryAddPos(cage.InnerPos, left + 4, top + 2);
                        TryAddPos(cage.InnerPos, left + 4, top + 3);
                        TryAddPos(cage.InnerPos, left + 4, top + 4);
                        TryAddPos(cage.InnerPos, left + 4, top + 5);
                        TryAddPos(cage.InnerPos, left + 4, top + 6);
                        TryAddPos(cage.InnerPos, left + 4, top + 7);
                        TryAddPos(cage.PreWallPos, left + 4, top + 8);
                    }
                    if (row == CageRowNum - 1)
                    {
                        TryAddPos(cage.InnerPos, left + 3, top + 1);
                        TryAddPos(cage.InnerPos, left + 3, top + 2);
                        TryAddPos(cage.InnerPos, left + 3, top + 3);
                        TryAddPos(cage.InnerPos, left + 3, top + 4);
                        TryAddPos(cage.InnerPos, left + 3, top + 5);
                        TryAddPos(cage.InnerPos, left + 3, top + 6);
                        TryAddPos(cage.InnerPos, left + 4, top + 0);
                        TryAddPos(cage.InnerPos, left + 4, top + 1);
                        TryAddPos(cage.InnerPos, left + 4, top + 2);
                        TryAddPos(cage.InnerPos, left + 4, top + 3);
                        TryAddPos(cage.InnerPos, left + 4, top + 4);
                        TryAddPos(cage.InnerPos, left + 4, top + 5);
                        TryAddPos(cage.PreWallPos, left + 4, top - 1);
                    }
                }
                else
                {
                    // 右辺
                    TryAddPos(cage.PreWallPos, left + 3, top + 2);
                    TryAddPos(cage.PreWallPos, left + 3, top + 3);
                    TryAddPos(cage.PreWallPos, left + 3, top + 4);
                    TryAddPos(cage.PreWallPos, left + 3, top + 5);
                }
                for (int rowP = 2; rowP <= 5; rowP++)
                {
                    for (int colP = 1; colP <= 2; colP++)
                    {
                        TryAddPos(cage.InnerPos, left + colP, top + rowP);
                    }
                }
                cage.LeftPreWallCount = cage.PreWallPos.Count;
            }
        }
        return cages;
    }

    public override List<Team> GetTeams(int m, Cage[] cages)
    {
        var teams = new List<Team>();
        switch (m)
        {
            case 5:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 4, 11, 18 }));
                teams.Add(GetTeam(2, cages, new int[] { 4, 3 }, new int[] { 25, 18 }));
                return teams;
            case 6:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 4, 11, 18 }));
                teams.Add(GetTeam(3, cages, new int[] { 4, 3, 2 }, new int[] { 25, 18, 11 }));
                return teams;
            case 7:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 4, 11, 18 }));
                teams.Add(GetTeam(4, cages, new int[] { 4, 3, 2, 1 }, new int[] { 25, 18, 11, 4 }));
                return teams;
            case 8:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 4, 11, 18 }));
                teams.Add(GetTeam(3, cages, new int[] { 4, 3, 2 }, new int[] { 25, 18, 11 }));
                teams.Add(GetTeam(2, cages, new int[] { 4, 3 }, new int[] { 25, 18 }));
                return teams;
            case 9:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 4, 11, 18 }));
                teams.Add(GetTeam(3, cages, new int[] { 4, 3, 2 }, new int[] { 25, 18, 11 }));
                teams.Add(GetTeam(3, cages, new int[] { 4, 3, 2 }, new int[] { 25, 18, 11 }));
                return teams;
            case 10:
                teams.Add(GetTeam(3, cages, new int[] { 0, 1, 2 }, new int[] { 4, 11, 18 }));
                teams.Add(GetTeam(3, cages, new int[] { 4, 3, 2 }, new int[] { 25, 18, 11 }));
                teams.Add(GetTeam(4, cages, new int[] { 4, 3, 2, 1 }, new int[] { 25, 18, 11, 4 }));
                return teams;
        }
        throw new ArgumentException();
    }
}

/// <summary>
/// グラフの辺
/// 頂点は別のコレクションで管理する
/// </summary>
public class Edge
{
    /// <summary>(有向の場合、始点となる)頂点の添字</summary>
    public int SourcePoint;
    /// <summary>(有向の場合、終点となる)頂点の添字</summary>
    public int TargetPoint;
    /// <summary>辺に関連する値</summary>
    /// <remarks>重み、容量など。用途は限定しない。</remarks>
    public int Value;
}

/// <summary>
/// 最大フローを求めるためのクラス
/// </summary>
public class MaxFlow
{
    List<Edge> edges;
    int vertexCount;
    bool[] seen;
    public Dictionary<int, Dictionary<int, int>> ResidualCapacity;
    public List<int> MinCutSourceVertexes;
    public List<int> MinCutTargetVertexes;

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="edges">辺の一覧(書き換えはしないが参照をコピーする)</param>
    /// <param name="vertexCount">頂点の数</param>
    public MaxFlow(List<Edge> edges, int vertexCount)
    {
        this.edges = edges;
        this.vertexCount = vertexCount;
        seen = new bool[vertexCount];
    }

    /// <summary>最大流量を取得する。</summary>
    public int GetMaxFlow(int sourceVertex, int targetVertex)
    {
        // 残余グラフを作成
        ResidualCapacity = new Dictionary<int, Dictionary<int, int>>();
        for (var i = 0; i < vertexCount; i++)
        {
            ResidualCapacity.Add(i, new Dictionary<int, int>());
        }
        for (var i = 0; i < edges.Count; i++)
        {
            var edge = edges[i];
            if (!ResidualCapacity[edge.SourcePoint].ContainsKey(edge.TargetPoint))
            {
                ResidualCapacity[edge.SourcePoint].Add(edge.TargetPoint, edge.Value);
            }
            else
            {
                ResidualCapacity[edge.SourcePoint][edge.TargetPoint] += edge.Value;
            }
            if (!ResidualCapacity[edge.TargetPoint].ContainsKey(edge.SourcePoint))
            {
                ResidualCapacity[edge.TargetPoint][edge.SourcePoint] = 0;
            }
        }

        var res = 0;
        while (true)
        {
            for (var i = 0; i < vertexCount; i++) seen[i] = false;
            // s-tルートを1つ探して容量を取得する
            int flow = FoDfs(sourceVertex, targetVertex, int.MaxValue);
            // 容量が残っているs-tルートが見つからなくなったら終了
            if (flow == 0) break;
            // 容量を加算
            res += flow;
        }

        // 最小カット時のSource,Targetをそれぞれ求める
        for (var i = 0; i < vertexCount; i++) seen[i] = false;
        seen[sourceVertex] = true;
        var queue = new Queue<int>();
        queue.Enqueue(sourceVertex);
        while (queue.Any())
        {
            var source = queue.Dequeue();
            foreach (var target in ResidualCapacity[source])
            {
                if (target.Value <= 0 || seen[target.Key]) continue;
                seen[target.Key] = true;
                queue.Enqueue(target.Key);
            }
        }
        seen[targetVertex] = false;
        MinCutSourceVertexes = new List<int>();
        MinCutTargetVertexes = new List<int>();
        for (var i = 0; i < vertexCount; i++)
        {
            (seen[i] ? MinCutSourceVertexes : MinCutTargetVertexes).Add(i);
        }

        return res;
    }

    /// <summary>
    /// 残余グラフ上のs-tパスを探索する
    /// </summary>
    /// <param name="v">(vertex)中継点となる頂点番号</param>
    /// <param name="t">(target)終点となる頂点番号</param>
    /// <param name="f">(flow)始点～中継点に流せる容量</param>
    /// <returns>始点s-vまでの容量の最小値(パスが見つからない場合は0)</returns>
    int FoDfs(int v, int t, int f)
    {
        // 終点tに到達したら探索終了
        if (v == t) return f;

        seen[v] = true;
        var caps = ResidualCapacity[v];
        foreach (var kv in caps)
        {
            var to = kv.Key;
            var restCap = kv.Value;
            // 探索済の頂点への辺、残容量0の辺は探索しない
            if (seen[to] || restCap == 0) continue;

            // 1つ先の頂点から終点への容量を調べる
            int flow = FoDfs(to, t, Math.Min(f, restCap));
            if (flow == 0) continue;

            // 辺にパスに流せる容量分のフローを流す
            ResidualCapacity[v][to] -= flow;
            ResidualCapacity[to][v] += flow;

            // 見つけたv-tパスの容量を返す(最終的にs-tパスの最小容量を返す)
            return flow;
        }

        // v-tパスが見つからなかった場合
        return 0;
    }
}