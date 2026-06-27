START_TIME = Time.utc.to_unix_ms
TL         = 1980
N          =   30
INF        = 1 << 29
RND        = Random.new(2)
DIR_TILE   = [
  [1, 0, -1, -1],
  [3, -1, -1, 0],
  [-1, -1, 3, 2],
  [-1, 2, 1, -1],
  [1, 0, 3, 2],
  [3, 2, 1, 0],
  [2, -1, 0, -1],
  [-1, 3, -1, 1],
]
CANDS = [
  [[0, 1], [0, 3], [2, 3], [1, 2]],
  [] of Array(Int32),
  [] of Array(Int32),
  [] of Array(Int32),
  [[0, 1], [0, 1], [0, 1], [0, 1]],
  [] of Array(Int32),
  [[0], [1], [0], [1]],
  [] of Array(Int32),
]
DR = [0, -1, 0, 1]
DC = [-1, 0, 1, 0]

alias Pos = Tuple(Int32, Int32)

macro debug(msg)
  {% if flag?(:trace) %}
    STDERR.puts({{msg}})
  {% end %}
end

macro debugf(format_string, *args)
  {% if flag?(:trace) %}
    STDERR.printf({{format_string}}, {{*args}})
  {% end %}
end

class Result
  property :cmd, :score

  def initialize(@cmd : Array(Array(Int32)), @score : Int32)
  end

  def to_s(io)
    io << @cmd.map { |row| row.join }.join
  end
end

RES_EMPTY = Result.new(Array.new(N) { Array.new(N, 0) }, 0)

class Solver
  def initialize
    @base_rot = Array(Array(Int32)).new(N) { Array.new(N, 0) }
    @tile = Array(Array(Int32)).new(N) { Array.new(N, 0) }
    N.times do |i|
      row = read_line.chars
      N.times do |j|
        t = row[j].to_i
        case t
        when 0..3
          @base_rot[i][j] = (4 - t) % 4
          @tile[i][j] = 0
        when 4..5
          @base_rot[i][j] = (6 - t) % 2
          @tile[i][j] = 4
        when 6..7
          @base_rot[i][j] = (8 - t) % 2
          @tile[i][j] = 6
        end
      end
    end
    @dfs_cnt = 0
  end

  def solve(timelimit)
    start_time = Time.utc.to_unix_ms
    before_time = start_time
    best_res, best_loops = solve_single(0)
    after_time = Time.utc.to_unix_ms
    worst_time = after_time - before_time
    turn = 0
    while true
      before_time = after_time
      if before_time + worst_time > (timelimit + start_time) // 2
        debug("turn:#{turn} worst_time:#{worst_time}")
        break
      end
      res, loops = solve_single(best_res.score)
      if res.score > best_res.score
        best_res = res
        best_loops = loops
        debug("score:#{best_res.score} at turn #{turn}")
      end
      after_time = Time.utc.to_unix_ms
      worst_time = {worst_time, after_time - before_time}.max
      turn += 1
    end
    debug(convert_res(best_res))
    best_res = improve(best_res, best_loops, timelimit)
    return convert_res(best_res)
  end

  def convert_res(res)
    cmd = Array.new(N) do |i|
      Array.new(N) do |j|
        (res.cmd[i][j] + @base_rot[i][j]) & 3
      end
    end
    return Result.new(cmd, res.score)
  end

  def solve_single(best_score)
    rot = Array.new(N) { Array.new(N, -1) }
    loops = [] of Array(Pos)
    while loops.size < 2
      if loops.size == 0
        sr = RND.rand(N - 20) + 3
        sc = RND.rand(N - 20) + 3
        while rot[sr][sc] != -1
          sr = RND.rand(N - 20) + 3
          sc = RND.rand(N - 20) + 3
        end
      else
        sr = RND.rand(N - 20) + 17
        sc = RND.rand(N - 20) + 17
        while rot[sr][sc] != -1
          sr = RND.rand(N - 20) + 17
          sc = RND.rand(N - 20) + 17
        end
      end
      path = try_create_loop(rot, sr, sc)
      if !path.empty?
        if path.size * 600 < best_score
          return RES_EMPTY, loops
        end
        loops << path
      end
    end
    return Result.new(rot, loops[0].size * loops[1].size), loops
  end

  def try_create_loop(rot, sr, sc)
    sd = 0
    4.times do |i|
      if DIR_TILE[@tile[sr][sc]][i] != -1
        sd = i
        break
      end
    end
    rot[sr][sc] = 0
    path = [{sr, sc}]
    cd = DIR_TILE[@tile[sr][sc]][sd]
    cr = sr + DR[cd]
    cc = sc + DC[cd]
    # debug("start:" + [sr, sc, sd].to_s)
    @dfs_cnt = 0
    if dfs(rot, sr, sc, sd, cr, cc, (cd + 2) & 3, path)
      return path
    end
    rot[sr][sc] = -1
    path.clear
    return path
  end

  def dfs(rot, sr, sc, sd, cr, cc, cd, path)
    if sr == cr && sc == cc && sd == cd
      return true
    end
    if cr == -1 || N == cr || cc == -1 || N == cc
      return false
    end
    if @dfs_cnt >= 80
      return false
    end
    if path.size > 500
      return false
    end
    if rot[cr][cc] != -1
      nd = DIR_TILE[@tile[cr][cc] + rot[cr][cc]][cd]
      if nd == -1
        return false
      end
      nr = cr + DR[nd]
      nc = cc + DC[nd]
      path << {cr, cc}
      ret = dfs(rot, sr, sc, sd, nr, nc, (nd + 2) & 3, path)
      if !ret
        path.pop
      end
      return ret
    end
    path << {cr, cc}
    cands = CANDS[@tile[cr][cc]][cd]
    shuffle(cands)
    @dfs_cnt += 1 if cands.size > 1
    cands.each do |r|
      nd = DIR_TILE[@tile[cr][cc] + r][cd]
      rot[cr][cc] = r
      nr = cr + DR[nd]
      nc = cc + DC[nd]
      if dfs(rot, sr, sc, sd, nr, nc, (nd + 2) & 3, path)
        return true
      end
    end
    rot[cr][cc] = -1
    path.pop
    return false
  end

  def evaluate(s1, s2)
    mi, ma = {s1, s2}.minmax
    return mi
  end

  def improve(res, loops, timelimit)
    used = Array.new(N) { Array.new(N, 0) }
    loops.each do |path|
      path.each do |pos|
        used[pos[0]][pos[1]] += 1
      end
    end
    best_res = Result.new(res.cmd, res.score)
    cur_val = evaluate(loops[0].size, loops[1].size)
    rot = Array.new(N) { |i| res.cmd[i].dup }
    turn = 0
    initial_cooer = 0.001
    final_cooer = 0.01
    cooler = initial_cooer
    begin_time = Time.utc.to_unix_ms
    while true
      turn += 1
      if (turn & 0xFF) == 0
        cur_time = Time.utc.to_unix_ms
        if cur_time > timelimit
          debug("turn:#{turn}")
          break
        end
        ratio = (cur_time - begin_time) / (timelimit - begin_time)
        cooler = Math.exp((1.0 - ratio) * Math.log(initial_cooer) + ratio * Math.log(final_cooer))
        # debug("cooler:#{cooler} at turn #{turn}")
      end
      pi = RND.rand(2)
      path = loops[pi]
      chi = RND.rand(path.size)
      start_pos = path[chi]
      next if @tile[start_pos[0]][start_pos[1]] == 6
      next if used[start_pos[0]][start_pos[1]] >= 2
      rm_path = [] of Pos
      rm_size = {RND.rand(20) + 2, path.size // 2}.min
      rm_size.times do |i|
        rm_pos = path[(chi + i) % path.size]
        if used[rm_pos[0]][rm_pos[1]] >= 2
          rm_size = i
          break
        end
        rm_path << rm_pos
        rot[rm_pos[0]][rm_pos[1]] = -1
      end
      end_pos = path[(chi + rm_size) % path.size]
      end_prev_pos = path[(chi + rm_size - 1) % path.size]
      sd = path_dir(end_prev_pos, end_pos)
      cd = path_dir(path[chi - 1], start_pos)
      add_path = [] of Pos
      # debug([*end_pos, sd, *start_pos, cd])
      @dfs_cnt = 0
      if dfs(rot, *end_pos, sd, *start_pos, cd, add_path)
        next if add_path == rm_path
        next_score = (loops[pi].size - rm_size + add_path.size) * loops[1 - pi].size
        next_val = evaluate(loops[pi].size - rm_size + add_path.size, loops[1 - pi].size)
        debug("diff:#{next_val - cur_val}")
        if accept(next_val - cur_val, cooler)
          if chi + rm_size <= path.size
            loops[pi] = path[0, chi]
            loops[pi].concat(add_path)
            loops[pi].concat(path[(chi + rm_size)..])
          else
            loops[pi] = path[((chi + rm_size) % path.size)...chi]
            loops[pi].concat(add_path)
          end
          N.times do |i|
            N.times do |j|
              res.cmd[i][j] = rot[i][j]
              used[i][j] = 0
            end
          end
          loops.each do |path|
            path.each do |pos|
              used[pos[0]][pos[1]] += 1
            end
          end
          cur_val = next_val
          res.score = next_score
          debug("improve #{res.score} at turn #{turn}")
          if next_score > best_res.score
            N.times do |i|
              N.times do |j|
                best_res.cmd[i][j] = rot[i][j]
              end
            end
            best_res.score = next_score
            debug(convert_res(best_res))
            debug("")
          end
        else
          add_path.each do |p|
            rot[p[0]][p[1]] = res.cmd[p[0]][p[1]]
          end
          rm_path.each do |p|
            rot[p[0]][p[1]] = res.cmd[p[0]][p[1]]
          end
        end
      else
        rm_path.each do |p|
          rot[p[0]][p[1]] = res.cmd[p[0]][p[1]]
        end
      end
    end
    return best_res
  end

  def accept(diff, cooler)
    return true if diff >= 0
    v = diff * cooler
    return false if v < -10
    return RND.rand < Math.exp(v)
  end

  def path_dir(prev_pos, cur_pos)
    if prev_pos[0] == cur_pos[0] - 1
      1
    elsif prev_pos[0] == cur_pos[0] + 1
      3
    elsif prev_pos[1] == cur_pos[1] - 1
      0
    else
      2
    end
  end

  def shuffle(a)
    (a.size - 1).times do |i|
      pos = RND.rand(a.size - i) + i
      a[i], a[pos] = a[pos], a[i]
    end
  end
end

def main
  solver = Solver.new
  part = 120
  best_res = RES_EMPTY
  part.times do |i|
    res = solver.solve(START_TIME + TL * (i + 1) // part)
    if res.score > best_res.score
      best_res = res
    end
  end
  puts best_res
  {% if flag?(:local) %}
    STDERR.puts("score:#{best_res.score}")
  {% end %}
end

main
