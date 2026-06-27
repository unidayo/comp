START_TIME = Time.utc.to_unix_ms
N          = 20
EMPTY      =  0
ONI        =  1
FUKU       =  2
TL         = (ENV["TL"]? || 1990).to_i
INF        = 1i64 << 60
RND        = Random.new(2)
DR         = [1, 0, -1, 0]
DC         = [0, 1, 0, -1]
DOWN       = 0
RIGHT      = 1
UP         = 2
LEFT       = 3

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

record Pos, y : Int32, x : Int32 do
  def <=>(other)
    if y != other.y
      return y <=> other.y
    end
    x <=> other.x
  end
end

record Action, dir : Int32, i : Int32 do
  def to_s(io)
    io << "DRUL"[dir] << " " << i
  end
end

class Result
  getter :acts

  def initialize(@acts : Array(Action))
  end

  def score
    @acts.size
  end

  def to_s(io)
    io << @acts.join("\n")
  end
end

def len_to_out(r, c)
  return {r + 1, N - r, c + 1, N - c}.min
end

def shuffle(a)
  (a.size - 1).times do |i|
    pos = RND.rand(a.size - i) + i
    a.swap(i, pos)
  end
end

RES_EMPTY = Result.new(Array.new(10000, Action.new(0, 0)))

class Solver
  @st : Array(Array(Int32))

  def initialize(@init_st : Array(Array(Int32)))
    @st = @init_st.map { |row| row.dup }
    @acts = [] of Action
  end

  def solve(timelimit)
    before_time = Time.utc.to_unix_ms
    best_res = solve_one(false, 9999999)
    debug("score:#{best_res.score} turn:#{0}")
    after_time = Time.utc.to_unix_ms
    worst_time = after_time - before_time
    turn = 0
    before_time = after_time
    while true
      if Time.utc.to_unix_ms + worst_time >= timelimit
        debug("total_turn:#{turn}")
        break
      end
      turn += 1
      res = solve_one(true, best_res.score)
      if res.score < best_res.score
        debug("score:#{res.score} turn:#{turn}")
        best_res = res
      end
      after_time = Time.utc.to_unix_ms
      worst_time = {worst_time, after_time - before_time}.max
      before_time = after_time
    end
    return best_res
  end

  def manip_vert(len, dir, i, escape)
    row = dir == UP ? len : N - 1 - len
    if @st[row][i] == ONI
      while @st[row - DR[dir]][i] == ONI
        row -= DR[dir]
        len += 1
      end
      skip = false
      1.upto(len) do |j|
        cur_row = row + DR[dir] * j
        if @st[cur_row][i] == FUKU
          oni_cols = N.times.select { |j| @st[cur_row][j] == ONI }.to_a
          if oni_cols.empty?
            rm_dir = RND.rand(2) * 2 + 1
          else
            sum_left = oni_cols.sum { |c| len_to_out(cur_row, c - 1) }
            sum_right = oni_cols.sum { |c| len_to_out(cur_row, c + 1) }
            rm_dir = (sum_left < sum_right || sum_left == sum_right && RND.rand(2) == 0) ? LEFT : RIGHT
          end
          rm_len = can_remove_fuku(cur_row, i, rm_dir)
          if rm_len
            rm_len.times do
              shift(rm_dir, cur_row)
            end
          else
            rm_len = can_remove_fuku(cur_row, i, rm_dir ^ 2)
            if rm_len
              rm_len.times do
                shift(rm_dir ^ 2, cur_row)
              end
            else
              skip = true
              if escape
                if can_shift(rm_dir, row)
                  shift(rm_dir, row)
                  break
                elsif can_shift(rm_dir ^ 2, row)
                  shift(rm_dir ^ 2, row)
                  break
                end
              end
              return false
            end
          end
        end
        break if skip
      end
      if !skip
        len.times do |c|
          adj_row = row + DR[dir] * (len - 1)
          1.upto(N - 2) do
            if @st[adj_row][i] != ONI
              if i > 0 && @st[adj_row][i - 1] == ONI && can_shift(RIGHT, adj_row) && len_to_out(adj_row + DR[dir] * (len + 1 - c), i) + 1 < len_to_out(adj_row, i - 1)
                shift(RIGHT, adj_row)
              elsif i < N - 1 && @st[adj_row][i + 1] == ONI && can_shift(LEFT, adj_row) && len_to_out(adj_row + DR[dir] * (len + 1 - c), i) + 1 < len_to_out(adj_row, i + 1)
                shift(LEFT, adj_row)
              elsif i > 1 && @st[adj_row][i - 2] == ONI && can_shift(RIGHT, adj_row) && @st[adj_row][N - 2] != FUKU && len_to_out(adj_row + DR[dir] * (len + 1 - c), i) + 2 < len_to_out(adj_row, i - 2)
                shift(RIGHT, adj_row)
                shift(RIGHT, adj_row)
              elsif i < N - 2 && @st[adj_row][i + 2] == ONI && can_shift(LEFT, adj_row) && @st[adj_row][1] != FUKU && len_to_out(adj_row + DR[dir] * (len + 1 - c), i) + 2 < len_to_out(adj_row, i + 2)
                shift(LEFT, adj_row)
                shift(LEFT, adj_row)
              end
            end
            adj_row -= DR[dir]
          end
          shift(dir, i)
        end
        while @st[row + DR[dir] * len][i] == ONI # || @st[row + DR[dir] * len][i] != FUKU && @st[row + DR[dir] * (len - 1)][i] == ONI
          shift(dir, i)
        end
      end
    end
    return true
  end

  def manip_horz(len, dir, i, escape)
    col = dir == LEFT ? len : N - 1 - len
    if @st[i][col] == ONI
      while @st[i][col - DC[dir]] == ONI
        col -= DC[dir]
        len += 1
      end
      skip = false
      1.upto(len) do |j|
        cur_col = col + DC[dir] * j
        if @st[i][cur_col] == FUKU
          oni_rows = N.times.select { |j| @st[j][cur_col] == ONI }.to_a
          if oni_rows.empty?
            rm_dir = RND.rand(2) * 2
          else
            sum_up = oni_rows.sum { |r| len_to_out(r, cur_col) }
            sum_down = oni_rows.sum { |r| len_to_out(r, cur_col) }
            rm_dir = (sum_up < sum_down || sum_up == sum_down && RND.rand(2) == 0) ? UP : DOWN
          end
          rm_len = can_remove_fuku(i, cur_col, rm_dir)
          if rm_len
            rm_len.times do
              shift(rm_dir, cur_col)
            end
          else
            rm_len = can_remove_fuku(i, cur_col, rm_dir ^ 2)
            if rm_len
              rm_len.times do
                shift(rm_dir ^ 2, cur_col)
              end
            else
              skip = true
              if escape
                if can_shift(rm_dir, len)
                  shift(rm_dir, len)
                  break
                elsif can_shift(rm_dir ^ 2, len)
                  shift(rm_dir ^ 2, len)
                  break
                end
              end
              return false
            end
          end
        end
        break if skip
      end
      if !skip
        len.times do |c|
          adj_col = col + DC[dir] * (len - 1)
          1.upto(N - 2) do
            if @st[i][adj_col] != ONI
              if i > 0 && @st[i - 1][adj_col] == ONI && can_shift(DOWN, adj_col) && len_to_out(adj_col + DC[dir] * (len + 1 - c), i) + 1 < len_to_out(adj_col, i - 1)
                shift(DOWN, adj_col)
              elsif i < N - 1 && @st[i + 1][adj_col] == ONI && can_shift(UP, adj_col) && len_to_out(adj_col + DC[dir] * (len + 1 - c), i) + 1 < len_to_out(adj_col, i + 1)
                shift(UP, adj_col)
              elsif i > 1 && @st[i - 2][adj_col] == ONI && can_shift(DOWN, adj_col) && @st[N - 2][adj_col] != FUKU && len_to_out(adj_col + DC[dir] * (len + 1 - c), i) + 2 < len_to_out(adj_col, i - 2)
                shift(DOWN, adj_col)
                shift(DOWN, adj_col)
              elsif i < N - 2 && @st[i + 2][adj_col] == ONI && can_shift(UP, adj_col) && @st[1][adj_col] != FUKU && len_to_out(adj_col + DC[dir] * (len + 1 - c), i) + 2 < len_to_out(adj_col, i + 2)
                shift(UP, adj_col)
                shift(UP, adj_col)
              end
            end
            adj_col -= DC[dir]
          end
          shift(dir, i)
        end
        while @st[i][col + DC[dir] * len] == ONI # || @st[i][col + DC[dir] * len] != FUKU && @st[i][col + DC[dir] * (len - 1)] == ONI
          shift(dir, i)
        end
      end
    end
    return true
  end

  def solve_one(escape, best_score)
    N.times { |i| @st[i][..] = @init_st[i] }
    @acts.clear
    prev_cnt = @st.sum { |row| row.count(ONI) }
    dirs = [UP, RIGHT, DOWN, LEFT]
    trial = 0
    while true
      dir_pos = [] of Tuple(Int32, Int32, Int32)
      (N // 2).times do |len|
        len.upto(N - 1 - len) do |i|
          dir_pos << {UP, i, len} << {RIGHT, i, len} << {DOWN, i, len} << {LEFT, i, len}
        end
      end
      shuffle(dir_pos)
      dir_pos.each do |dir, pos, len|
        if dir == UP || dir == DOWN
          if !manip_vert(len, dir, pos, escape)
            return RES_EMPTY
          end
        else
          if !manip_horz(len, dir, pos, escape)
            return RES_EMPTY
          end
        end
      end
      if @acts.size >= best_score
        return RES_EMPTY
      end
      cur_cnt = @st.sum { |row| row.count(ONI) }
      break if cur_cnt == 0
      if cur_cnt == prev_cnt
        return RES_EMPTY
      end
      prev_cnt = cur_cnt
    end
    return Result.new(@acts.dup)
  end

  def can_remove_fuku(sr, sc, dir)
    r = sr
    c = sc
    len = 0
    while 0 <= r < N && 0 <= c < N && @st[r][c] == FUKU
      len += 1
      r -= DR[dir]
      c -= DC[dir]
    end
    r = sr
    c = sc
    while 0 <= r < N && 0 <= c < N
      r += DR[dir]
      c += DC[dir]
    end
    len.times do |i|
      r -= DR[dir]
      c -= DC[dir]
      if @st[r][c] == FUKU
        return nil
      end
    end
    return len
  end

  def can_shift(dir, pos)
    case dir
    when DOWN
      return @st[N - 1][pos] != FUKU
    when RIGHT
      return @st[pos][N - 1] != FUKU
    when UP
      return @st[0][pos] != FUKU
    else
      return @st[pos][0] != FUKU
    end
  end

  def shift(dir, pos)
    @acts << Action.new(dir, pos)
    case dir
    when DOWN
      (N - 1).downto(1) do |i|
        @st[i][pos] = @st[i - 1][pos]
      end
      @st[0][pos] = EMPTY
    when RIGHT
      (N - 1).downto(1) do |i|
        @st[pos][i] = @st[pos][i - 1]
      end
      @st[pos][0] = EMPTY
    when UP
      (N - 1).times do |i|
        @st[i][pos] = @st[i + 1][pos]
      end
      @st[N - 1][pos] = EMPTY
    else
      (N - 1).times do |i|
        @st[pos][i] = @st[pos][i + 1]
      end
      @st[pos][N - 1] = EMPTY
    end
  end

  def print
    N.times do |i|
      debug(@st[i].map { |s| s == EMPTY ? '.' : s == FUKU ? 'o' : 'x' }.join + "\n")
    end
  end
end

def main
  gets
  st = Array.new(N) do
    row = read_line.chars.map { |ch| ch == 'o' ? FUKU : ch == 'x' ? ONI : EMPTY }
  end
  solver = Solver.new(st)
  best_res = solver.solve(START_TIME + TL)
  puts best_res
  {% if flag?(:local) %}
    STDERR.printf("final_score:%4d raw_score:%d\n", 8 * N * N - best_res.score, best_res.score)
  {% end %}
end

main
