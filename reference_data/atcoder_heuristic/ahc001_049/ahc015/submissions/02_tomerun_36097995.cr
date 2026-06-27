START_TIME = Time.utc.to_unix_ms
TL         = 1900
N          =  100
REP        = (ENV["REP"]? || 400).to_i
INF        = 1 << 29
RND        = Random.new(2)
DIR_CHARS  = "FLBR"
DR         = [-1, 0, 1, 0]
DC         = [0, -1, 0, 1]
EMPTY      = 0

macro debug(msg)
  {% if flag?(:local) %}
    STDERR.puts({{msg}})
  {% end %}
end

macro debugf(format_string, *args)
  {% if flag?(:local) %}
    STDERR.printf({{format_string}}, {{*args}})
  {% end %}
end

def crash(msg, caller_line = __LINE__)
  STDERR.puts "[ERROR] line #{caller_line}: #{msg}"
  exit
end

macro assert(cond, msg = "", caller_line = __LINE__)
  {% if flag?(:local) %}
    if !({{cond}})
      crash({{msg}}, {{caller_line}})
    end
  {% end %}
end

class UnionFind
  def initialize(size : Int32)
    @root = Array(Int32).new(size, -1)
  end

  def union(i, j)
    a = root(i)
    b = root(j)
    if a != b
      @root[a] += @root[b]
      @root[b] = a
    end
  end

  def find(i, j)
    root(i) == root(j)
  end

  def root(i)
    return i if @root[i] < 0
    @root[i] = root(@root[i])
  end

  def size(i)
    -@root[root(i)]
  end
end

class Solver
  @f : Array(Int32)
  @max_f : Int32
  @min_f : Int32
  @score_den : Float64

  def initialize
    @f = read_line.split.map(&.to_i)
    @history = [] of Int32
    @cs = Array(Array(Int32)).new(10) { Array.new(10, EMPTY) }
    @cs_bk = Array(Array(Int32)).new(10) { Array.new(10, EMPTY) }
    @score_den = 1.0 / (1..3).sum { |i| @f.count(i) ** 2 }
    @max_f = 0
    @min_f = 0
    best_max_f = 0
    best_min_f = 0
    best_sum = 0.0
    1.upto(3) do |max_f|
      @max_f = max_f
      1.upto(3) do |min_f|
        next if max_f == min_f
        @min_f = min_f
        sum = 0.0
        100.times do
          sum += sim_all(0, @f[1] == @max_f ? 0 : 2)
        end
        if sum > best_sum
          best_sum = sum
          best_max_f = max_f
          best_min_f = min_f
        end
      end
    end
    @max_f = best_max_f
    @min_f = best_min_f
  end

  def solve(timelimit)
    N.times do |i|
      p = read_line.to_i
      set(@f[i], p)
      dir = step(i)
      @history << dir
      tilt(dir)
      puts DIR_CHARS[dir]
      STDOUT.flush
    end
    print()
  end

  def set(kind, pos)
    i = 0
    10.times do |r|
      10.times do |c|
        if @cs[r][c] == EMPTY
          i += 1
          if i == pos
            @cs[r][c] = kind
            return
          end
        end
      end
    end
    assert(false)
  end

  def tilt(dir)
    case dir
    when 0
      10.times do |c|
        from = 0
        while from < 10 && @cs[from][c] != EMPTY
          from += 1
        end
        to = from
        while from < 10
          v = @cs[from][c]
          if v != EMPTY
            @cs[from][c] = EMPTY
            @cs[to][c] = v
            to += 1
          end
          from += 1
        end
      end
    when 1
      10.times do |r|
        from = 0
        while from < 10 && @cs[r][from] != EMPTY
          from += 1
        end
        to = from
        while from < 10
          v = @cs[r][from]
          if v != EMPTY
            @cs[r][from] = EMPTY
            @cs[r][to] = v
            to += 1
          end
          from += 1
        end
      end
    when 2
      10.times do |c|
        from = 9
        while from >= 0 && @cs[from][c] != EMPTY
          from -= 1
        end
        to = from
        while from >= 0
          v = @cs[from][c]
          if v != EMPTY
            @cs[from][c] = EMPTY
            @cs[to][c] = v
            to -= 1
          end
          from -= 1
        end
      end
    when 3
      10.times do |r|
        from = 9
        while from >= 0 && @cs[r][from] != EMPTY
          from -= 1
        end
        to = from
        while from >= 0
          v = @cs[r][from]
          if v != EMPTY
            @cs[r][from] = EMPTY
            @cs[r][to] = v
            to -= 1
          end
          from -= 1
        end
      end
    end
  end

  def print
    10.times do |r|
      debug(@cs[r].join)
    end
  end

  def score
    uf = UnionFind.new(N)
    10.times do |r|
      10.times do |c|
        if r < 9 && @cs[r][c] == @cs[r + 1][c]
          uf.union(r * 10 + c, (r + 1) * 10 + c)
        end
        if c < 9 && @cs[r][c] == @cs[r][c + 1]
          uf.union(r * 10 + c, r * 10 + c + 1)
        end
      end
    end
    num = 0
    N.times do |i|
      if uf.root(i) == i # && @cs[i // 10][i % 10] != EMPTY
        num += uf.size(i) ** 2
      end
    end
    return num * @score_den
  end

  def step(turn, in_sim = false)
    return 0 if turn == N - 1
    if !in_sim && turn > 4
      debug(turn)
      10.times do |i|
        @cs_bk[i][0...10] = @cs[i]
      end
      scores = Array.new(4, 0.0)
      near_tl = Time.utc.to_unix_ms - START_TIME > TL
      if near_tl
        4.times do |dir|
          10.times { scores[dir] += sim_all(turn, dir) }
        end
      elsif turn < N - 8
        warn_tl = Time.utc.to_unix_ms - START_TIME > TL - 200
        rep = warn_tl ? 50 : (REP * (turn > 50 ? 2 : 1))
        rep.times do |t|
          mv = scores.max
          4.times do |dir|
            if t > 10 && scores[dir] < mv * (1.0 - 3.0 / t)
              # skip
            else
              scores[dir] += sim_all(turn, dir)
            end
          end
          max_i = 4.times.max_by { |i| scores[i] }
          if t > 30 && 4.times.all? { |i| i == max_i || scores[i] < scores[max_i] * (1.0 - 3.0 / t) }
            break
          end
        end
      else
        ps = (1..(N - turn - 1)).to_a
        4.times do |dir|
          ps.each_permutation.each do |perm|
            scores[dir] += sim_all(turn, dir, perm)
          end
        end
      end
      debug(scores)
      debug([@f[turn + 1], 4.times.max_by { |i| scores[i] }])
      return 4.times.max_by { |i| scores[i] }
    end
    if @f[turn + 1] == @max_f
      # if turn > 0 && @history[-1] == 0 && @f[turn + 2] != @max_f
      #   return @f[turn + 2] == @min_f ? 1 : 3
      # end
      return 0
    else
      if turn > 0 && @f[turn] != @max_f
        return @f[turn + 1] == @min_f ? 1 : 3
      else
        return 2
      end
    end
  end

  def sim_all(turn, dir, perm = nil)
    # hist_bk = @history.dup
    tilt(dir)
    if perm
      (turn + 1).upto(N - 1) do |i|
        pos = perm[i - turn - 1]
        (i - turn).upto(perm.size - 1) do |j|
          if perm[j] > pos
            perm[j] -= 1
          end
        end
        set(@f[i], pos)
        cdir = step(i, true)
        # @history << cdir
        tilt(cdir)
      end
    else
      (turn + 1).upto(85) do |i|
        cpos = RND.rand(N)
        while @cs[cpos // 10][cpos % 10] != EMPTY
          cpos = RND.rand(N)
        end
        @cs[cpos // 10][cpos % 10] = @f[i]
        cdir = step(i, true)
        @history << cdir
        tilt(cdir)
      end
      {86, (turn + 1)}.max.upto(N - 1) do |i|
        pos = RND.rand(N - i) + 1
        set(@f[i], pos)
        cdir = step(i, true)
        # @history << cdir
        tilt(cdir)
      end
    end
    ret = score()
    10.times do |i|
      @cs[i][0...10] = @cs_bk[i]
    end
    # @history = hist_bk
    # debug([turn, dir, ret])
    return ret
  end
end

def main
  solver = Solver.new
  solver.solve(START_TIME + TL)
  debug("final_score:#{(1000000i64 * solver.score).round.to_i}")
end

main
