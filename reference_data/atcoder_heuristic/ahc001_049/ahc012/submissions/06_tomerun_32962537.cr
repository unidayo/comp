START_TIME = Time.utc.to_unix_ms
TL         =  2980
K          =   100
L          = 10000
M          = (ENV["M"]? || 28).to_i
INF        = 1 << 29
RND        = Random.new(2)
MV         = 140

struct Pos
  property :r, :c

  def initialize(@r : Int32, @c : Int32)
  end
end

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
  puts "[ERROR] line #{caller_line}: #{msg}"
  exit
end

macro assert(cond, msg = "", caller_line = __LINE__)
  {% if flag?(:local) %}
    if !({{cond}})
      crash({{msg}}, {{caller_line}})
    end
  {% end %}
end

class Line
  property :r0, :c0, :r1, :c1

  def initialize(@r0 : Int32, @c0 : Int32, @r1 : Int32, @c1 : Int32)
  end

  def clone
    return Line.new(@r0, @c0, @r1, @c1)
  end

  def ccw(p)
    dr0 = @r1 - @r0
    dc0 = @c1 - @c0
    dr1 = p.r - @r0
    dc1 = p.c - @c0
    return dr1 * dc0 - dr0 * dc1
  end

  def to_s(io)
    io << @c0 << " " << @r0 << " " << @c1 << " " << @r1
  end
end

class Result
  property :lines, :score

  def initialize(@lines : Array(Line), @score : Int32)
  end

  def to_s(io)
    io << @lines.size << "\n" << @lines.map(&.to_s).join("\n") << "\n"
  end
end

RES_EMPTY = Result.new([] of Line, -INF)

class Solver
  def initialize(@a : Array(Int32), @ps : Array(Pos))
    @hist = Array(Int32).new(100, 0)
    @bucket = Array(Array(Array(Int32))).new(M + 1) do
      Array(Array(Int32)).new(M + 1) do
        [] of Int32
      end
    end
    @destroy = Array(Int32).new(ps.size, 0)
  end

  def solve(timelimit)
    @hist.fill(0)
    (M + 1).times do |i|
      (M + 1).times do |j|
        @bucket[i][j].clear
      end
    end
    lgs = [] of Array(Line)
    lgs << Array.new(M) do |i|
      x = -(L - 1000) + (2 * L - 1000) * i // (M - 1)
      Line.new(-L, x, L, x)
    end
    lgs << Array.new(M) do |i|
      y = -(L - 1000) + (2 * L - 1000) * i // (M - 1)
      Line.new(y, L, y, -L)
    end

    @ps.size.times do |i|
      idx = lgs.size.times.map do |j|
        find_idx(lgs[j], @ps[i])
      end.to_a
      @bucket[idx[0]][idx[1]] << i
    end
    (M + 1).times do |i0|
      (M + 1).times do |i1|
        @hist[@bucket[i0][i1].size] += 1
      end
    end
    cur_v = eval(@hist)
    best_v = cur_v
    best_lines = lgs
    debug("cur_v:#{cur_v} hist:#{@hist[0, 16]}")

    initial_cooler = 1.0
    final_cooler = 10.0
    cooler = initial_cooler
    turn = 0
    move_cnt = Array.new(2) { Array.new(M + 1, 0) }
    new_hist = @hist.clone
    begin_time = Time.utc.to_unix_ms
    total_time = timelimit - begin_time
    while true
      if (turn & 0xF) == 0
        cur_time = Time.utc.to_unix_ms
        if cur_time > timelimit
          debug("turn: #{turn}")
          break
        end
        ratio = (cur_time - begin_time) / total_time
        cooler = initial_cooler + (final_cooler - initial_cooler) * ratio
      end
      lgi = RND.rand(lgs.size)
      li = RND.rand(M)
      line = lgs[lgi][li].clone
      move0 = RND.rand(2 * MV + 1) - MV
      move1 = RND.rand(2 * MV + 1) - MV
      if lgi == 0
        line.c0 += move0
        if li > 0 && line.c0 < lgs[lgi][li - 1].c0
          line.c0 = lgs[lgi][li - 1].c0
        end
        if li < M - 1 && line.c0 > lgs[lgi][li + 1].c0
          line.c0 = lgs[lgi][li + 1].c0
        end
        line.c1 += move1
        if li > 0 && line.c1 < lgs[lgi][li - 1].c1
          line.c1 = lgs[lgi][li - 1].c1
        end
        if li < M - 1 && line.c1 > lgs[lgi][li + 1].c1
          line.c1 = lgs[lgi][li + 1].c1
        end
      else
        line.r0 += move0
        if li > 0 && line.r0 < lgs[lgi][li - 1].r0
          line.r0 = lgs[lgi][li - 1].r0
        end
        if li < M - 1 && line.r0 > lgs[lgi][li + 1].r0
          line.r0 = lgs[lgi][li + 1].r0
        end
        line.r1 += move1
        if li > 0 && line.r1 < lgs[lgi][li - 1].r1
          line.r1 = lgs[lgi][li - 1].r1
        end
        if li < M - 1 && line.r1 > lgs[lgi][li + 1].r1
          line.r1 = lgs[lgi][li + 1].r1
        end
      end

      new_hist[0, @hist.size] = @hist
      if lgi == 0
        (M + 1).times do |i|
          move_cnt[0][i] = 0
          move_cnt[1][i] = 0
          pos = 0
          @bucket[li][i].size.times do
            pi = @bucket[li][i][pos]
            if line.ccw(@ps[pi]) < 0
              move_cnt[0][i] += 1
              @bucket[li][i][pos], @bucket[li][i][-move_cnt[0][i]] = @bucket[li][i][-move_cnt[0][i]], pi
            else
              pos += 1
            end
          end
          pos = 0
          @bucket[li + 1][i].size.times do
            pi = @bucket[li + 1][i][pos]
            if line.ccw(@ps[pi]) >= 0
              move_cnt[1][i] += 1
              @bucket[li + 1][i][pos], @bucket[li + 1][i][-move_cnt[1][i]] = @bucket[li + 1][i][-move_cnt[1][i]], pi
            else
              pos += 1
            end
          end
          new_hist[@bucket[li][i].size] -= 1
          new_hist[@bucket[li + 1][i].size] -= 1
          new_hist[@bucket[li][i].size - move_cnt[0][i] + move_cnt[1][i]] += 1
          new_hist[@bucket[li + 1][i].size - move_cnt[1][i] + move_cnt[0][i]] += 1
        end
      else
        (M + 1).times do |i|
          move_cnt[0][i] = 0
          move_cnt[1][i] = 0
          pos = 0
          @bucket[i][li].size.times do
            pi = @bucket[i][li][pos]
            if line.ccw(@ps[pi]) < 0
              move_cnt[0][i] += 1
              @bucket[i][li][pos], @bucket[i][li][-move_cnt[0][i]] = @bucket[i][li][-move_cnt[0][i]], pi
            else
              pos += 1
            end
          end
          pos = 0
          @bucket[i][li + 1].size.times do
            pi = @bucket[i][li + 1][pos]
            if line.ccw(@ps[pi]) >= 0
              move_cnt[1][i] += 1
              @bucket[i][li + 1][pos], @bucket[i][li + 1][-move_cnt[1][i]] = @bucket[i][li + 1][-move_cnt[1][i]], pi
            else
              pos += 1
            end
          end
          new_hist[@bucket[i][li].size] -= 1
          new_hist[@bucket[i][li + 1].size] -= 1
          new_hist[@bucket[i][li].size - move_cnt[0][i] + move_cnt[1][i]] += 1
          new_hist[@bucket[i][li + 1].size - move_cnt[1][i] + move_cnt[0][i]] += 1
        end
      end
      new_v = eval(new_hist)
      if accept(new_v - cur_v, cooler)
        debug("new_v:#{new_v} hist:#{new_hist[0, 16]}")
        cur_v = new_v
        @hist[0, @hist.size] = new_hist
        lgs[lgi][li] = line
        if lgi == 0
          (M + 1).times do |i|
            ms0 = @bucket[li][i].pop(move_cnt[0][i])
            ms1 = @bucket[li + 1][i].pop(move_cnt[1][i])
            @bucket[li][i].concat(ms1)
            @bucket[li + 1][i].concat(ms0)
          end
        else
          (M + 1).times do |i|
            ms0 = @bucket[i][li].pop(move_cnt[0][i])
            ms1 = @bucket[i][li + 1].pop(move_cnt[1][i])
            @bucket[i][li].concat(ms1)
            @bucket[i][li + 1].concat(ms0)
          end
        end
        if cur_v > best_v
          best_v = cur_v
          best_lines = lgs.map { |lg| lg.map { |line| line.clone } }
        end
      else
      end
      turn += 1
    end
    return Result.new(best_lines.flatten, cur_v)
  end

  def find_idx(lg, p)
    M.times do |i|
      line = lg[i]
      ccw = line.ccw(p)
      if ccw >= 0
        # if ccw == 0
        #   @destroy[i] += 1
        # end
        return i
      end
    end
    return M
  end

  def eval(hist)
    sum = 0
    1.upto(10) do |i|
      sum += {@a[i], hist[i]}.min
    end
    return sum
  end

  def accept(diff, cooler)
    return true if diff >= 0
    v = diff * cooler
    return false if v < -8
    # return false
    return RND.rand < Math.exp(v)
  end
end

def main
  n, k = read_line.split.map(&.to_i)
  a = [0] + read_line.split.map(&.to_i)
  ps = Array(Pos).new(n) do
    x, y = read_line.split.map(&.to_i)
    Pos.new(y, x)
  end
  solver = Solver.new(a, ps)
  part = 1
  best_res = RES_EMPTY
  part.times do |i|
    res = solver.solve(START_TIME + TL * (i + 1) // part)
    if res.score > best_res.score
      best_res = res
    end
  end
  puts best_res
  {% if flag?(:local) %}
    final_score = (1000_000 * best_res.score / a.sum).round.to_i
    STDERR.puts("final_score:#{final_score}")
  {% end %}
end

main
