START_TIME     = Time.utc.to_unix_ms
TL             =  1980
N              =    50
INITIAL_COOLER = 0.004
FINAL_COOLER   = 0.100
INF            =   1e9
COUNTER        = Counter.new
DR             = [0, 1, 0, -1]
DC             = [1, 0, -1, 0]
DIRS           = "RDLU"

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
    return next_int % m
  end

  def next_double
    return TO_DOUBLE * next_int
  end
end

class Counter
  def initialize
    @hist = [] of Int32
  end

  def add(i)
    while @hist.size <= i
      @hist << 0
    end
    @hist[i] += 1
  end

  def to_s(io)
    io << "counter:\n"
    ((@hist.size + 9) // 10).times do |i|
      io << @hist[((i * 10)...(i * 10 + 10))]
      io << "\n"
    end
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
  sr, sc = read_line.split.map(&.to_i)
  t = Array.new(N) { read_line.split.map(&.to_i) }
  p = Array.new(N) { read_line.split.map(&.to_i) }
  solver = Solver.new(sr, sc, t, p, START_TIME + TL)
  result = solver.solve
  puts result.mvs.map { |m| DIRS[m] }.join
end

class Result
  property :score, :mvs

  def initialize
    @score = 0
    @mvs = [] of Int32
  end
end

class Solver
  @otr : Array(Array(Int32))
  @otc : Array(Array(Int32))
  @visited : Array(Array(Int32))
  @cooler : Float64

  def initialize(@sr : Int32, @sc : Int32, t, @p : Array(Array(Int32)), @timelimit : Int64)
    @rnd = XorShift.new(2u64)
    @cooler = INITIAL_COOLER
    @otr = Array.new(N) { |i| Array.new(N, i) }
    @otc = Array.new(N) { Array.new(N) { |i| i } }
    @visited = Array.new(N) { Array.new(N, 0) }
    N.times do |i|
      N.times do |j|
        if i < N - 1 && t[i][j] == t[i + 1][j]
          @otr[i][j] = i + 1
          @otr[i + 1][j] = i
        end
        if j < N - 1 && t[i][j] == t[i][j + 1]
          @otc[i][j] = j + 1
          @otc[i][j + 1] = j
        end
      end
    end
  end

  def visit(r, c)
    @visited[r][c] += 1
    @visited[@otr[r][c]][@otc[r][c]] += 1
  end

  def unvisit(r, c)
    @visited[r][c] -= 1
    @visited[@otr[r][c]][@otc[r][c]] -= 1
  end

  def solve
    tl_greedy = @timelimit - TL * 4 // 5
    before_time = Time.utc.to_unix_ms
    best_res = Result.new
    turn = 0
    while true
      if Time.utc.to_unix_ms > tl_greedy
        debugf("total turn:%d\n", turn)
        break
      end
      res = random_greedy()
      if res.score > best_res.score
        debugf("score:%d turn:%d\n", res.score, turn)
        best_res = res
      end
      turn += 1
      break
    end
    improve(best_res)
    debugf("total score:%d\n", best_res.score)
    return best_res
  end

  def improve(res)
    @visited.each do |row|
      row.fill(0)
    end
    cr = @sr
    cc = @sc
    visit(cr, cc)
    ps = [] of Tuple(Int32, Int32)
    ps << {cr, cc}
    res.mvs.each do |d|
      cr += DR[d]
      cc += DC[d]
      ps << {cr, cc}
      visit(cr, cc)
    end
    turn = 0
    best_score = res.score
    best_ps = ps
    @cooler = INITIAL_COOLER
    cutoff = -10.0 / @cooler
    init_time = Time.utc.to_unix_ms
    while true
      turn += 1
      if (turn & 0xFF) == 0
        cur_time = Time.utc.to_unix_ms
        if cur_time > @timelimit
          debugf("improve turn:%d\n", turn)
          break
        else
          ratio = (cur_time - init_time) / (@timelimit - init_time)
          @cooler = Math.exp((1.0 - ratio) * Math.log(INITIAL_COOLER) + ratio * Math.log(FINAL_COOLER))
          cutoff = -10.0 / @cooler
        end
      end
      sti = @rnd.next_int(ps.size - 10).to_i
      eti = sti + 1 + @rnd.next_int(20).to_i
      if eti > ps.size
        eti = ps.size
      end
      diff = 0
      (sti + 1).upto(eti - 1) do |i|
        cr = ps[i][0]
        cc = ps[i][1]
        diff -= @p[cr][cc]
        unvisit(cr, cc)
      end
      if eti == ps.size
        new_ps, sc = connect_goal_free(ps[sti][0], ps[sti][1])
      else
        new_ps, sc = connect(ps[sti][0], ps[sti][1], ps[eti][0], ps[eti][1])
      end
      if !new_ps
        (sti + 1).upto(eti - 1) do |i|
          cr = ps[i][0]
          cc = ps[i][1]
          visit(cr, cc)
        end
        next
      end
      diff += sc
      if accept(diff, cutoff)
        debugf("diff:%4d len:%2d fin?:%s\n", diff, eti - sti, eti == ps.size ? "true" : "false")
        ps = ps[0..sti] + new_ps + ps[eti..]
        res.score += diff
        if res.score > best_score
          debugf("score:%d at turn:%d\n", res.score, turn)
          best_score = res.score
          best_ps = ps
        end
      else
        new_ps.each do |p|
          unvisit(p[0], p[1])
        end
        (sti + 1).upto(eti - 1) do |i|
          visit(ps[i][0], ps[i][1])
        end
      end
    end
    res.score = best_score
    res.mvs.clear
    (best_ps.size - 1).times do |i|
      4.times do |d|
        if best_ps[i][0] + DR[d] == best_ps[i + 1][0] && best_ps[i][1] + DC[d] == best_ps[i + 1][1]
          res.mvs << d
          break
        end
      end
    end
  end

  def accept(diff, cutoff)
    return true if diff >= 0
    return false if diff < cutoff
    return @rnd.next_double < Math.exp(diff * @cooler)
  end

  def connect(sr, sc, gr, gc)
    score = 0
    cr = sr
    cc = sc
    ps = [] of Tuple(Int32, Int32)
    cand_dirs = [] of Int32
    while ps.size < 50
      cand_dirs.clear
      4.times do |d|
        nr = cr + DR[d]
        nc = cc + DC[d]
        next if nr < 0 || N <= nr || nc < 0 || N <= nc
        if nr == gr && nc == gc
          next if ps.empty?
          cand_dirs.clear
          cand_dirs << d
          break
        else
          next if @visited[nr][nc] != 0
        end
        cand_dirs << d
      end
      break if cand_dirs.empty?
      dir = cand_dirs.size == 1 ? cand_dirs[0] : cand_dirs[@rnd.next_int(cand_dirs.size)]
      cr += DR[dir]
      cc += DC[dir]
      break if cr == gr && cc == gc
      break if (cr - gr).abs + (cc - gc).abs + ps.size > 50
      ps << {cr, cc}
      score += @p[cr][cc]
      visit(cr, cc)
      dist = (cr - gr).abs + (cc - gc).abs
      if dist == 2
        ng = 4.times.all? do |i|
          ar = gr + DR[i]
          ac = gc + DC[i]
          ar < 0 || N <= ar || ac < 0 || N <= ac || @visited[ar][ac] > 0
        end
        break if ng
      elsif dist == 1
        cr = gr
        cc = gc
        break
      end
    end
    if cr == gr && cc == gc
      return ps, score
    else
      cr = sr
      cc = sc
      ps.each do |p|
        unvisit(p[0], p[1])
      end
      return nil, 0
    end
  end

  def connect_goal_free(sr, sc)
    score = 0
    cr = sr
    cc = sc
    ps = [] of Tuple(Int32, Int32)
    cand_dirs = [] of Int32
    while true
      cand_dirs.clear
      4.times do |d|
        nr = cr + DR[d]
        nc = cc + DC[d]
        next if nr < 0 || N <= nr || nc < 0 || N <= nc
        next if @visited[nr][nc] != 0
        cand_dirs << d
      end
      break if cand_dirs.empty?
      dir = cand_dirs.size == 1 ? cand_dirs[0] : cand_dirs[@rnd.next_int(cand_dirs.size)]
      cr += DR[dir]
      cc += DC[dir]
      ps << {cr, cc}
      score += @p[cr][cc]
      visit(cr, cc)
    end
    return ps, score
  end

  def random_greedy
    @visited.each do |row|
      row.fill(0)
    end
    visit(@sr, @sc)
    dead_check_length = 100
    dead_check_count = 50
    cr = @sr
    cc = @sc
    res = Result.new
    res.score = @p[cr][cc]
    cand_dirs = [] of Int32
    while true
      # debugf("len:%d (%d %d)\n", res.mvs.size, cr, cc)
      cand_dirs.clear
      4.times do |d|
        nr = cr + DR[d]
        nc = cc + DC[d]
        next if nr < 0 || N <= nr || nc < 0 || N <= nc
        next if @visited[nr][nc] != 0
        cand_dirs << d
      end
      break if cand_dirs.empty?
      if cand_dirs.size == 1
        dir = cand_dirs[0]
      else
        best_score = -INF
        dir = 0
        cand_dirs.each do |d|
          nr = cr + DR[d]
          nc = cc + DC[d]
          visit(nr, nc)
          dead_check_count.times do
            ns = random_walk(nr, nc, dead_check_length) + @p[nr][nc]
            # debugf("ns:%d\n", ns)
            if ns > best_score
              best_score = ns
              dir = d
            end
          end
          unvisit(nr, nc)
        end
      end
      cr += DR[dir]
      cc += DC[dir]
      visit(cr, cc)
      res.score += @p[cr][cc]
      res.mvs << dir
    end
    return res
  end

  def random_walk(cr, cc, max_len)
    score = 0
    cand_dirs = [] of Int32
    mv = [] of Tuple(Int32, Int32)
    max_len.times do
      cand_dirs.clear
      4.times do |d|
        nr = cr + DR[d]
        nc = cc + DC[d]
        next if nr < 0 || N <= nr || nc < 0 || N <= nc
        next if @visited[nr][nc] != 0
        cand_dirs << d
      end
      break if cand_dirs.empty?
      dir = cand_dirs.size == 1 ? cand_dirs[0] : cand_dirs[@rnd.next_int(cand_dirs.size)]
      cr += DR[dir]
      cc += DC[dir]
      visit(cr, cc)
      score += @p[cr][cc]
      mv << {cr, cc}
    end
    mv.each do |r, c|
      unvisit(r, c)
    end
    return score
  end
end
