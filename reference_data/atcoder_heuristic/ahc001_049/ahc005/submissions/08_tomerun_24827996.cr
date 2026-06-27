START_TIME     = Time.utc.to_unix_ms
TL             = 2960
N              =   20
MULTISTART     =   10
INITIAL_COOLER = 0.01
FINAL_COOLER   = 1.00
DR             = [1, 0, -1, 0]
DC             = [0, 1, 0, -1]
DIRS           = ['D', 'R', 'U', 'L']

class XorShift
  TO_DOUBLE = 0.5 / (1u64 << 63)

  def initialize(@x = 123456789u64)
  end

  def next_int
    @x ^= @x << 13
    @x ^= @x >> 17
    @x ^= @x << 5
    return @x
  end

  def next_int(m)
    return (next_int % m).to_i
  end

  def next_double
    return TO_DOUBLE * next_int
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

main

def main
  n, si, sj = read_line.split.map(&.to_i)

  f = [[-1] * (n + 2)]
  n.times do
    f << [-1] + read_line.chars.map { |ch| ch == '#' ? -1 : ch.to_i } + [-1]
  end
  f << [-1] * (n + 2)
  solver = Solver.new(si + 1, sj + 1, f, START_TIME + TL)
  result = solver.solve
  debug("score:#{10_000 + 10_000_000 * n / result.score} t=#{result.score}")
  puts result.moves.map { |d| DIRS[d] }.join
end

class Result
  property :score, :moves

  def initialize(@score : Int32, @moves : Array(Int32))
  end
end

class Cross
  @cost : Int32
  getter :r, :c, :idx, :cost

  def initialize(@r : Int32, @c : Int32, @idx : Int32, f)
    @cost = f[@r][@c]
    c = 1
    4.times do |i|
      nr = @r + DR[i]
      nc = @c + DC[i]
      if f[nr][nc] != -1
        @cost += f[nr][nc]
        c += 1
      end
    end
    @cost *= 60
    @cost //= c
  end
end

class Solver
  @n : Int32

  def initialize(@si : Int32, @sj : Int32, @f : Array(Array(Int32)), @timelimit : Int64)
    @n = @f.size - 2
    @rnd = XorShift.new(2u64)
    @crs = [] of Cross
    @cr_idx = Array(Array(Int32)).new(@n + 2) { [-1] * (@n + 2) }
    @cr_idx[@si][@sj] = 0
    @crs << Cross.new(@si, @sj, 0, f)
    1.step(to: @n + 1, by: 2) do |i|
      1.step(to: @n + 1, by: 2) do |j|
        next if f[i][j] == -1 || (i == @si && j == @sj)
        if (f[i - 1][j] != -1 || f[i + 1][j] != -1) && (f[i][j - 1] != -1 || f[i][j + 1] != -1)
          @cr_idx[i][j] = @crs.size
          @crs << Cross.new(i, j, @crs.size, @f)
        end
      end
    end
    debug("crs size:#{@crs.size}")
    @cr_dist = Array(Array(Int32)).new(@crs.size) { [1 << 30] * @crs.size }
    qs = Array.new(1000) { [] of Tuple(Int32, Int32) }
    dist = Array.new(@n + 2) { [qs.size] * (@n + 2) }
    visited = Array.new(@n + 2) { [false] * (@n + 2) }
    @crs.size.times do |i|
      1.upto(@n) do |i|
        visited[i].fill(false)
        dist[i].fill(qs.size)
      end
      qs.each { |q| q.clear }
      @cr_dist[i][i] = 0
      cr = @crs[i]
      qs[0] << {cr.r, cr.c}
      visited[cr.r][cr.c] = true
      qs.size.times do |cost|
        q = qs[cost]
        q.each do |cur|
          cr, cc = cur
          4.times do |dir|
            nr = cr + DR[dir]
            nc = cc + DC[dir]
            ncost = cost + @f[nr][nc]
            if @f[nr][nc] != -1 && !visited[nr][nc] && ncost < dist[nr][nc]
              visited[nr][nc] = true
              dist[nr][nc] = ncost
              qs[ncost] << {nr, nc}
              ni = @cr_idx[nr][nc]
              if ni != -1
                @cr_dist[i][ni] = ncost
              end
            end
          end
        end
      end
    end
    debug("init time:#{Time.utc.to_unix_ms - START_TIME}")
  end

  def solve
    cur_time = Time.utc.to_unix_ms
    time_budget = @timelimit - cur_time
    best_path, best_score = solve_one(cur_time + time_budget // MULTISTART)
    2.upto(MULTISTART) do |i|
      path, score = solve_one(cur_time + time_budget * i // MULTISTART)
      if score < best_score
        best_path = path
        best_score = score
      end
    end
    return Result.new(best_score, recover(best_path))
  end

  def solve_one(tl)
    use_idx, view_count = select_crs
    best_score, best_path = solve_single(use_idx)
    debug("score:#{best_score}")
    turn = 0
    while true
      turn += 1
      if (turn & 0xF) == 0 && Time.utc.to_unix_ms > tl - 50
        debug("total turn:#{turn}")
        break
      end
      removed_cr, added_cris = update_crs(use_idx, view_count)
      if removed_cr == -1
        while true
          cand = @rnd.next_int(@crs.size - 1) + 1
          if !use_idx.index(cand)
            added_cris << cand
            update_view_count(@crs[cand], view_count, 1)
            break
          end
        end
      end
      use_idx += added_cris
      score, path = solve_single(use_idx)
      if score <= best_score
        debug("score:#{score} turn:#{turn}") if score < best_score
        best_score = score
        best_path = path
      else
        added_cris.size.times do |ri|
          update_view_count(@crs[use_idx[-1]], view_count, -1)
          use_idx.pop
        end
        if removed_cr != -1
          update_view_count(@crs[removed_cr], view_count, 1)
          use_idx << removed_cr
        end
      end
    end
    return improve(best_path, best_score, tl)
  end

  def select_crs
    all_idx = (1...@crs.size).to_a
    all_idx.size.times do |i|
      pos = @rnd.next_int(all_idx.size - i) + i
      all_idx[i], all_idx[pos] = all_idx[pos], all_idx[i]
    end
    view_count = Array.new(@n + 2) { [0] * (@n + 2) }
    @crs.each do |cr|
      update_view_count(cr, view_count, 1)
    end
    use_idx = [] of Int32
    all_idx.size.times do |i|
      cr = @crs[all_idx[i]]
      any_zero = false
      view_count[cr.r][cr.c] -= 1
      if view_count[cr.r][cr.c] == 0
        any_zero = true
      end
      4.times do |dir|
        r = cr.r + DR[dir]
        c = cr.c + DC[dir]
        while @f[r][c] != -1
          view_count[r][c] -= 1
          if view_count[r][c] == 0
            any_zero = true
          end
          r += DR[dir]
          c += DC[dir]
        end
      end
      if any_zero
        use_idx << all_idx[i]
        view_count[cr.r][cr.c] += 1
        4.times do |dir|
          r = cr.r + DR[dir]
          c = cr.c + DC[dir]
          while @f[r][c] != -1
            view_count[r][c] += 1
            r += DR[dir]
            c += DC[dir]
          end
        end
      end
    end
    return use_idx, view_count
  end

  def update_crs(use_idx, view_count)
    ui = @rnd.next_int(use_idx.size)
    remove = use_idx[ui]
    rm_cr = @crs[remove]
    vert = false
    horz = false
    view_count[rm_cr.r][rm_cr.c] -= 1
    4.times do |dir|
      r = rm_cr.r + DR[dir]
      c = rm_cr.c + DC[dir]
      while @f[r][c] != -1
        view_count[r][c] -= 1
        if view_count[r][c] == 0
          if (dir & 1) == 0
            vert = true
          else
            horz = true
          end
        end
        r += DR[dir]
        c += DC[dir]
      end
    end
    cands_horz = [] of Int32
    cands_vert = [] of Int32
    if horz
      c = rm_cr.c - 1
      while @f[rm_cr.r][c] != -1
        if @cr_idx[rm_cr.r][c] != -1
          cands_horz << @cr_idx[rm_cr.r][c]
        end
        c -= 1
      end
      c = rm_cr.c + 1
      while @f[rm_cr.r][c] != -1
        if @cr_idx[rm_cr.r][c] != -1
          cands_horz << @cr_idx[rm_cr.r][c]
        end
        c += 1
      end
      if cands_horz.empty?
        update_view_count(rm_cr, view_count, 1)
        return -1, [] of Int32
      end
    end
    if vert
      r = rm_cr.r - 1
      while @f[r][rm_cr.c] != -1
        if @cr_idx[r][rm_cr.c] != -1
          cands_vert << @cr_idx[r][rm_cr.c]
        end
        r -= 1
      end
      r = rm_cr.r + 1
      while @f[r][rm_cr.c] != -1
        if @cr_idx[r][rm_cr.c] != -1
          cands_vert << @cr_idx[r][rm_cr.c]
        end
        r += 1
      end
      if cands_vert.empty?
        update_view_count(rm_cr, view_count, 1)
        return -1, [] of Int32
      end
    end
    use_idx[ui] = use_idx[-1]
    use_idx.pop
    added_cris = [] of Int32
    if horz
      added_cris << cands_horz[@rnd.next_int(cands_horz.size)]
      update_view_count(@crs[added_cris[-1]], view_count, 1)
    end
    if vert
      added_cris << cands_vert[@rnd.next_int(cands_vert.size)]
      update_view_count(@crs[added_cris[-1]], view_count, 1)
    end
    return remove, added_cris
  end

  def update_view_count(cr, view_count, diff)
    view_count[cr.r][cr.c] += diff
    4.times do |dir|
      r = cr.r + DR[dir]
      c = cr.c + DC[dir]
      while @f[r][c] != -1
        view_count[r][c] += diff
        r += DR[dir]
        c += DC[dir]
      end
    end
  end

  def solve_single(use_idx)
    # nearest neighbor
    score = 0
    path = [0]
    # debug(use_idx)
    # debug("use size:#{use_idx.size}")
    used = [false] * use_idx.size
    use_idx.size.times do |i|
      prev_idx = path[-1]
      min_dist = 1 << 30
      min_i = -1
      use_idx.size.times do |j|
        next if used[j] || i == j
        idx = use_idx[j]
        dist = @cr_dist[prev_idx][idx]
        if dist < min_dist
          min_dist = dist
          min_i = j
        end
      end
      path << use_idx[min_i]
      used[min_i] = true
      score += min_dist
    end
    score += @cr_dist[path[-1]][0]
    path << 0
    return score, path
  end

  def improve(path, score, tl)
    # 2-opt
    turn = 0
    best_score = score
    best_path = path.dup
    cooler = INITIAL_COOLER
    begin_time = Time.utc.to_unix_ms
    while true
      turn += 1
      if (turn & 0x7FF) == 0
        # costs are not symmetric
        cur_time = Time.utc.to_unix_ms
        if cur_time > tl
          debug("improve turn:#{turn}")
          break
        end
        score = 0
        (path.size - 1).times do |i|
          score += @cr_dist[path[i]][path[i + 1]]
        end
        if score < best_score
          best_score = score
          best_path = path.dup
        end
        ratio = (cur_time - begin_time) / (tl - begin_time)
        cooler = Math.exp(Math.log(INITIAL_COOLER) * (1.0 - ratio) + Math.log(FINAL_COOLER) * ratio)
      end
      p1 = @rnd.next_int(path.size - 1)
      p2 = @rnd.next_int(path.size - 1)
      next if (p1 - p2).abs <= 1
      p1, p2 = p2, p1 if p2 < p1
      ri1 = path[p1]
      nri1 = path[p1 + 1]
      ri2 = path[p2]
      nri2 = path[p2 + 1]
      diff = @cr_dist[ri1][ri2] + @cr_dist[nri1][nri2] - @cr_dist[ri1][nri1] - @cr_dist[ri2][nri2]
      if accept(diff, cooler)
        op1, op2 = p1, p2
        score += diff
        p1 += 1
        while p1 < p2
          path[p1], path[p2] = path[p2], path[p1]
          p1 += 1
          p2 -= 1
        end
      end
    end
    score = 0
    (path.size - 1).times do |i|
      score += @cr_dist[path[i]][path[i + 1]]
    end
    if score < best_score
      best_score = score
      best_path = path.dup
    end
    debug("after improve:#{best_score}")
    return best_path, best_score
  end

  def accept(diff, cooler)
    return true if diff <= 0
    v = -diff * cooler
    return false if v < -10
    return @rnd.next_double < Math.exp(v)
  end

  def recover(cr_idx)
    ret = [] of Int32
    back = Array.new(@n + 2) { [-1] * (@n * 2) }
    qs = Array.new(10000) { [] of Tuple(Int32, Int32) }
    visited = Array.new(@n + 2) { [false] * (@n + 2) }
    dist = Array.new(@n + 2) { [qs.size] * (@n + 2) }
    (cr_idx.size - 1).times do |i|
      cr1 = @crs[cr_idx[i]]
      cr2 = @crs[cr_idx[i + 1]]
      1.upto(@n) do |i|
        visited[i].fill(false)
        dist[i].fill(qs.size)
      end
      qs[0] << {cr1.r, cr1.c}
      visited[cr1.r][cr1.c] = true
      found = false
      max_cost = 0
      qs.size.times do |cost|
        q = qs[cost]
        q.each do |cur|
          cr, cc = cur
          if cr == cr2.r && cc == cr2.c
            tmp = [] of Int32
            while cr != cr1.r || cc != cr1.c
              d = back[cr][cc]
              tmp << d
              cr, cc = cr - DR[d], cc - DC[d]
            end
            ret += tmp.reverse
            debug("cost:#{cost} #{[cr1.r, cr1.c]} -> #{[cr2.r, cr2.c]}")
            0.upto(max_cost) do |ci|
              qs[ci].clear
            end
            found = true
            break
          end
          4.times do |dir|
            nr = cr + DR[dir]
            nc = cc + DC[dir]
            ncost = cost + @f[nr][nc]
            if @f[nr][nc] != -1 && !visited[nr][nc] && ncost < dist[nr][nc]
              visited[nr][nc] = true
              dist[nr][nc] = ncost
              qs[ncost] << {nr, nc}
              back[nr][nc] = dir
              max_cost = {max_cost, ncost}.max
            end
          end
        end
        break if found
      end
    end
    return ret
  end
end
