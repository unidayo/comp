using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Diagnostics.Contracts;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

using static Marathon.Common;

namespace Marathon {
    public static class Common {
        public static readonly int[] dr = new[] { 0, 1, 0, -1 };
        public static readonly int[] dc = new[] { 1, 0, -1, 0 };
        public const string ds = "RBLF";
        public const int N = 10;
        public const int N2 = N * N;

        public static bool Logging { get; set; } = true;

        [Conditional("LOCAL")]
        public static void Debug(object s) {
            if (Logging) {
                Console.Error.WriteLine(s);
            }
        }

        [Conditional("LOCAL")]
        public static void Info(object s) {
            Console.Error.WriteLine(s);
        }

        public static long Seed = -1;
    }

    public static class MetaParameter {
    }

    // C# 7.2
    public static class UpdateExtension {
        public static bool UpdateMin<T>(this ref T x, T newValue) where T : struct, IComparable<T> {
            if (x.CompareTo(newValue) > 0) { x = newValue; return true; }
            return false;
        }
        public static bool UpdateMax<T>(this ref T x, T newValue) where T : struct, IComparable<T> {
            if (x.CompareTo(newValue) < 0) { x = newValue; return true; }
            return false;
        }
    }

    internal static class MarathonUtil {
        public static void Swap<T>(ref T lhs, ref T rhs) { T temp; temp = lhs; lhs = rhs; rhs = temp; }

        public static void SwapArray<T>(IList<T> list, int p1, int p2) {
            var tmp = list[p1];
            list[p1] = list[p2];
            list[p2] = tmp;
        }

        private static readonly Random rand = new Random(12);

        public static void ArrayShuffle<T>(IList<T> array, int size) {
            for (int i = 1; i < size; i++) {
                int r = rand.Next() % (i + 1);
                var t = array[i];
                array[i] = array[r];
                array[r] = t;
            }
        }

        /// <summary>
        /// when difference > 0 is better
        /// </summary>
        public static bool Anneal(double temperature, double difference) {
            if (difference > 0) {
                return true;
            }
            var d = difference / temperature;
            if (d < -16) return false;
            return rand.NextDouble() < Math.Exp(d);
        }

        public static void AdjustDelta(ref double delta, double temperature, double lastTemperature, int iter, double ellasedTime, double remainTime) {
            // temprature * delta^cnt = targetT
            // delta = (targetT / temprature)^(1/cnt)
            var estimateCnt = iter / ellasedTime * remainTime;
            delta = Math.Pow(lastTemperature / temperature, 1.0 / estimateCnt);
        }
    }


    public class Constant {
        public static int TIME_LIMIT = 3000;
    }
    public class TimeCheck {
        private DateTime startTime;

        public void InitTime() {
            startTime = DateTime.UtcNow;
        }
        public long ElapsedMilliseconds() {
            var t = DateTime.UtcNow.Subtract(startTime);
            return (t.Minutes * 60000 + t.Seconds * 1000 + t.Milliseconds);
        }
        public bool IsTimeOut() {
            return IsTimeOut(Constant.TIME_LIMIT);
        }
        public bool IsTimeOut(long LIMIT) {
            return ElapsedMilliseconds() > LIMIT;
        }
    }

    public static class ImmutableStack {
        [Pure]
        public static ImmutableStack<T> Create<T>() {
            return ImmutableStack<T>.Empty;
        }
    }

    public class ImmutableStack<T> {
        public static readonly ImmutableStack<T> Empty = new ImmutableStack<T>();

        private readonly T _value;
        private readonly int _count;
        private ImmutableStack<T> _reverseStack;
        private ImmutableStack<T> _previous;

        private ImmutableStack() : this(null, default(T), 0) {
        }

        private ImmutableStack(ImmutableStack<T> prev, T value, int count) {
            _value = value;
            _count = count;
            _previous = prev;
        }

        public T Value => _value;

        [Pure]
        public ImmutableStack<T> Clear() {
            return Empty;
        }

        [Pure]
        public ImmutableStack<T> Push(T value) {
            return new ImmutableStack<T>(
                this,
                value, Count + 1
            );
        }

        [Pure]
        public ImmutableStack<T> Pop() {
            if (_count == 0) {
                throw new Exception("stack is empty");
            }

            return _previous;
        }

        [Pure]
        public T Peek() {
            if (_count == 0) {
                throw new Exception("stack is empty");
            }

            return _value;
        }

        [Pure]
        public int Count {
            get {
                return _count;
            }
        }

        [Pure]
        public T[] ToArray() {
            var array = new T[Count];
            var current = this;
            int index = array.Length - 1;
            while (current.Count > 0) {
                array[index--] = current._value;
                current = current._previous;
            }
            return array;
        }

        public ImmutableStack<T> ReverseStack {
            get {
                if (_reverseStack == null) {
                    _reverseStack = new ImmutableStack<T>();
                    var current = this;
                    while (current.Count > 0) {
                        _reverseStack = _reverseStack.Push(current.Peek());
                        current = current.Pop();
                    }
                }
                return _reverseStack;
            }
        }
    }
    public partial class Deque<T> {
        private T[] _array;
        private int _head;
        private int _tail;
        public int Count { get; private set; }

        public Deque(int capacity = 16) {
            _array = new T[capacity];
            _head = 0;
            _tail = 0;
        }

        public void Clear() {
            _head = _tail = Count = 0;
        }

        public void PushBack(T value) {
            if (Count + 1 == _array.Length) {
                Expand();
            }
            Count++;

            _array[_tail++] = value;
            if (_tail == _array.Length) {
                _tail = 0;
            }
        }

        public void PushFront(T value) {
            if (Count + 1 == _array.Length) {
                Expand();
            }
            Count++;

            if (--_head == -1) {
                _head += _array.Length;
            }
            _array[_head] = value;
        }

        public T PopBack() {
            if (Count == 0) {
                throw new Exception("empty");
            }

            Count--;
            if (--_tail == -1) {
                _tail += _array.Length;
            }
            return _array[_tail];
        }

        public T PeekBack() {
            if (Count == 0) {
                throw new Exception("empty");
            }

            return this[Count - 1];
        }

        public T RandomPopBack(Random rand) {
            var index = rand.Next(Count);
            (this[Count - 1], this[index]) = (this[index], this[Count - 1]);
            return PopBack();
        }

        public T PopFront() {
            if (Count == 0) {
                throw new Exception("empty");
            }

            var ret = _array[_head];
            Count--;
            if (++_head == _array.Length) {
                _head = 0;
            }
            return ret;
        }

        public T PeekFront() {
            if (Count == 0) {
                throw new Exception("empty");
            }

            return _array[_head];
        }

        public T this[int index] {
            get {
                if (index < 0 || index >= Count) {
                    throw new Exception("Invalid index");
                }
                index += _head;
                if (index >= _array.Length) {
                    index -= _array.Length;
                }

                return _array[index];
            }
            set {
                if (index < 0 || index >= Count) {
                    throw new Exception("Invalid index");
                }
                index += _head;
                if (index >= _array.Length) {
                    index -= _array.Length;
                }

                _array[index] = value;
            }
        }

        private void Expand() {
            var newarray = new T[_array.Length * 2];

            if (_head < _tail) {
                Array.Copy(_array, _head, newarray, 0, Count);
            }
            else {
                Array.Copy(_array, _head, newarray, 0, _array.Length - _head);
                Array.Copy(_array, 0, newarray, _array.Length - _head, _tail);
            }

            _array = newarray;
            _head = 0;
            _tail = Count == newarray.Length ? 0 : Count;
        }

    }
    public class BooleanArray {
        private readonly int[] array;
        private int trueValue = 1;

        public BooleanArray(int size) {
            array = new int[size];
        }

        public void Clear() {
            trueValue++;
        }

        public bool this[int index] {
            get {
                return array[index] == trueValue;
            }
            set {
                array[index] = value ? trueValue : trueValue - 1;
            }
        }

        public int Length {
            get {
                return array.Length;
            }
        }
    }

    public class State {
        public int[] _board;
        public int CandyNum { get; private set; }
        public State() {
            _board = new int[N2];
        }

        public State Clone() {
            var clone = new State {
                CandyNum = CandyNum
            };
            Array.Copy(_board, clone._board, _board.Length);
            return clone;
        }

        public int this[int index] {
            get { return _board[index]; }
        }

        public void Place(int posIndex, int kind) {
            for (int i = 0; i < _board.Length; i++) {
                if (_board[i] == 0) {
                    posIndex--;
                    if (posIndex == 0) {
                        _board[i] = kind;
                        CandyNum++;
                        return;
                    }
                }
            }
            throw new InvalidOperationException();
        }

        public unsafe void Apply(int dirIndex) {
            var N = 10;
            var dir = ds[dirIndex];

            fixed (int* p = _board) {
                if (dir == 'L') {
                    for (int i = 0; i < N; i++) {
                        int k = 0;
                        for (int j = 0; j < N; j++) {
                            if (p[i * N + j] != 0) {
                                p[i * N + k] = p[i * N + j];
                                if (k != j) p[i * N + j] = 0;
                                k++;
                            }
                        }
                    }
                }
                else if (dir == 'R') {
                    for (int i = 0; i < N; i++) {
                        int k = N - 1;
                        for (int j = N - 1; j >= 0; j--) {
                            if (p[i * N + j] != 0) {
                                p[i * N + k] = p[i * N + j];
                                if (k != j) p[i * N + j] = 0;
                                k--;
                            }
                        }
                    }
                }
                else if (dir == 'F') {
                    for (int j = 0; j < N; j++) {
                        int k = 0;
                        for (int i = 0; i < N; i++) {
                            if (p[i * N + j] != 0) {
                                p[k * N + j] = p[i * N + j];
                                if (k != i) p[i * N + j] = 0;
                                k++;
                            }
                        }
                    }
                }
                else {
                    for (int j = 0; j < N; j++) {
                        int k = N - 1;
                        for (int i = N - 1; i >= 0; i--) {
                            if (p[i * N + j] != 0) {
                                p[k * N + j] = p[i * N + j];
                                if (k != i) p[i * N + j] = 0;
                                k--;
                            }
                        }
                    }
                }
            }
        }
    }

    public class Solver {
        private readonly TextReader _reader;
        private readonly TextWriter _writer;
        private readonly TimeCheck _timeCheck;

        private readonly int[] F;

        private readonly Random rand = new Random();

        private State _current = new State();

        private int[] kindHist = new int[4];
        private int many = 0;

        public Solver(TextReader reader, TextWriter writer, TimeCheck timeCheck, int[] f) {
            F = f;
            _reader = reader;
            _writer = writer;
            _timeCheck = timeCheck;

            foreach (var x in f) kindHist[x]++;
            many = 1;
            for (int i = 1; i < 4; i++) {
                if (kindHist[many] < kindHist[i]) many = i;
            }
        }

        public int Next(int pos) {
            var turn = _current.CandyNum;
            _current.Place(pos, F[turn]);

            if (turn == 99) return 0;

            int loop = 330;

            if (F[turn + 1] == many) {
                if (F[turn] == many) {
                    var score = new double[4];
                    for (int dir = 0; dir < 4; dir++) {
                        if (dir == 2) continue;
                        var state = _current.Clone();
                        state.Apply(dir);

                        for (int t = 0; t < loop; t++) {
                            score[dir] += Playout(state.Clone());
                        }
                    }
                    int target = 0;
                    for (int dir = 0; dir < 4; dir++) {
                        if (dir == 2) continue;
                        if (score[target] < score[dir]) target = dir;
                    }
                    _current.Apply(target);
                    return target;
                }
                else {
                    _current.Apply(0);
                    return 0;
                }
            }
            else {
                if (F[turn] == many) {
                    _current.Apply(2);
                    return 2;
                }
                else {
                    var score = new double[4];
                    for (int dir = 1; dir < 4; dir++) {
                        var state = _current.Clone();
                        state.Apply(dir);
                        for (int t = 0; t < loop; t++) {
                            score[dir] += Playout(state.Clone());
                        }
                    }
                    int target = 2;
                    for (int dir = 1; dir < 4; dir++) {
                        if (score[target] < score[dir]) target = dir;
                    }
                    _current.Apply(target);
                    return target;
                }
            }
        }

        private bool CheckValid(State state) {
            bool check = true;
            for (int i = 0; i < N; i++) {
                bool foundMany = false;
                bool foundOther = false;
                for (int j = 0; j < N; j++) {
                    if (state[i * N + j] == many) {
                        if (foundOther) {
                            check = false;
                            break;
                        }
                        foundMany = true;
                    }
                    else if (!foundMany && state[i * N + j] > 0) {
                        check = false;
                        break;
                    }
                    else if (state[i * N + j] > 0) {
                        foundOther = true;
                    }
                }
                if (!check) break;
            }

            return check;
        }

        public double Playout(State state) {
            while (state.CandyNum < F.Length) {
                var turn = state.CandyNum;
                var pos = rand.Next(1, 101 - turn);
                state.Place(pos, F[turn]);

                if (turn + 1 < F.Length && F[turn + 1] == many) {
                    state.Apply(0);
                }
                else {
                    state.Apply(2);
                }
            }
            return CalcScore(state);
        }

        public void Debug() {
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    Console.Error.Write(_current[i * N + j]);
                }
                Console.Error.WriteLine();
            }
        }

        public double CalcScore() {
            return CalcScore(_current);
        }

        BooleanArray visited = new BooleanArray(N2);

        public double CalcScore(State state) {
            var queue = new Queue<(int Row, int Col)>();
            visited.Clear();
            var hist = new int[4];
            double totalScore = 0;
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    hist[state[i * N + j]]++;
                    if (visited[i * N + j]) continue;
                    queue.Enqueue((i, j));
                    int size = 0;
                    var kind = state[i * N + j];
                    if (kind == 0) continue;
                    while (queue.Count > 0) {
                        var p = queue.Dequeue();
                        if (visited[p.Row * N + p.Col]) continue;
                        visited[p.Row * N + p.Col] = true;
                        size++;
                        for (int k = 0; k < 4; k++) {
                            var nr = p.Row + dr[k];
                            var nc = p.Col + dc[k];
                            if (0 <= nr && nr < N && 0 <= nc && nc < N && !visited[nr * N + nc] && state[nr * N + nc] == kind) {
                                queue.Enqueue((nr, nc));
                            }
                        }
                    }
                    totalScore += size * size;
                }
            }
            totalScore /= hist[1] * hist[1] + hist[2] * hist[2] + hist[3] * hist[3];
            return (int)Math.Round(1000000 * totalScore);
        }
    }
    public class Program {
        private static void Main(string[] args) {
            Execute(Console.In, Console.Out);
        }

        public static double Execute(TextReader reader, TextWriter writer) {
            var timeCheck = new TimeCheck();
            timeCheck.InitTime();
            var line = reader.ReadLine();
            var f = line.Split(' ').Select(a => int.Parse(a)).ToArray();
            var solver = new Solver(reader, writer, timeCheck, f);

            for (int i = 0; i < 100; i++) {
                var pos = int.Parse(reader.ReadLine());

                var dir = solver.Next(pos);
                writer.WriteLine(ds[dir]);
                writer.Flush();
            }

            solver.Debug();
            Console.Error.WriteLine("{0} ms", timeCheck.ElapsedMilliseconds());

            return solver.CalcScore();
        }

    }
}
