START_TIME      = Time.utc.to_unix_ms
TL              =     4800
L               =    10000
INITIAL_COOLER  =     25.0
FINAL_COOLER    =    800.0
INITIAL_COOLER2 =   3000.0
FINAL_COOLER2   = 300000.0
INF             =      1e9
COUNTER         = Counter.new

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
  n = read_line.to_i
  rs = Array.new(n) do |i|
    x, y, r = read_line.split.map(&.to_i)
    Requirement.new(x, y, r, i)
  end
  solver = Solver.new(rs, START_TIME + TL)
  result = solver.solve

  # search invalid pos
  xs = [0, 10000]
  ys = [0, 10000]
  result.rects.each do |r|
    next if r.left == -1
    xs << r.left << r.right
    ys << r.top << r.bottom
  end
  xs = xs.uniq.sort
  ys = ys.uniq.sort
  used = Array.new(xs.size) { Array.new(ys.size, false) }
  result.rects.each do |r|
    next if r.left == -1
    li = xs.bsearch_index { |p| p >= r.left }.not_nil!
    ri = xs.bsearch_index { |p| p >= r.right }.not_nil!
    ti = ys.bsearch_index { |p| p >= r.top }.not_nil!
    bi = ys.bsearch_index { |p| p >= r.bottom }.not_nil!
    li.upto(ri - 1) do |xi|
      ti.upto(bi - 1) do |yi|
        used[xi][yi] = true
      end
    end
  end
  invalid_pos = [] of Tuple(Int32, Int32)
  (xs.size - 1).times do |i|
    (ys.size - 1).times do |j|
      next if used[i][j]
      xs[i].upto(xs[i + 1] - 1) do |x|
        ys[j].upto(ys[j + 1] - 1) do |y|
          invalid_pos << {x, y}
        end
        break if invalid_pos.size >= n
      end
      break if invalid_pos.size >= n
    end
    break if invalid_pos.size >= n
  end

  result.rects.each_with_index do |r, i|
    if r.left == -1
      p = invalid_pos.pop
      puts "#{p[0]} #{p[1]} #{p[0] + 1} #{p[1] + 1}"
    else
      puts "#{r.left} #{r.top} #{r.right} #{r.bottom}"
    end
    mi, ma = {rs[i].r, (r.right - r.left) * (r.bottom - r.top)}.minmax
    debugf("%3d expect: %7d actual: %7d score: %.4f\n", i, rs[i].r, (r.right - r.left) * (r.bottom - r.top), 1.0 - (1.0 - mi / ma) ** 2)
  end
  debug(COUNTER)
end

def intersect(mi1, ma1, mi2, ma2)
  return !(ma1 <= mi2 || ma2 <= mi1)
end

def calc_score_rough(actual, expect)
  return 1.0 if actual >= expect
  return 1.0 - (1.0 - actual / expect) ** 2
end

def calc_score(actual, expect)
  mi, ma = {expect, actual}.minmax
  return 1.0 - (1.0 - mi / ma) ** 2
end

struct Requirement
  getter :x, :y, :r, :id

  def initialize(@x : Int32, @y : Int32, @r : Int32, @id : Int32)
  end
end

struct Rect
  property :left, :right, :top, :bottom

  def initialize(@left : Int32, @right : Int32, @top : Int32, @bottom : Int32)
  end

  def self.of_invalid
    return self.new(-1, -1, -1, -1)
  end

  def area
    return (@right - @left) * (@bottom - @top)
  end

  def shrink_fit(x, y, r)
    w = @right - @left
    h = @bottom - @top
    best = {w, h}
    best_ratio = {r / (best[0] * best[1]), (best[0] * best[1]) / r}.min
    j = h
    1.upto(w) do |i|
      ratio = {r / (i * j), (i * j) / r}.min
      if best_ratio < ratio
        best_ratio = ratio
        best = {i, j}
      end
      while i * j >= r
        j -= 1
        if j > 0
          ratio = {r / (i * j), (i * j) / r}.min
          if best_ratio < ratio
            best_ratio = ratio
            best = {i, j}
          end
        end
      end
    end
    debugf("%d %d (%d %d) -> (%d %d) %f\n", x, y, w, h, best[0], best[1], best_ratio)
    if @right - x <= best[0]
      @left = @right - best[0]
    else
      @left = x
      @right = x + best[0]
    end
    if @bottom - y <= best[1]
      @top = @bottom - best[1]
    else
      @top = y
      @bottom = y + best[1]
    end
    return self
  end
end

class Part
  property :left, :right, :top, :bottom, :rid, :score

  def initialize(@left : Int32, @right : Int32, @top : Int32, @bottom : Int32, @rid : Int32)
    @score = 0.0
  end

  def initialize(part : Part)
    @left = part.left
    @right = part.right
    @top = part.top
    @bottom = part.bottom
    @rid = part.rid
    @score = part.score
  end

  def area
    return (@right - @left) * (@bottom - @top)
  end

  def to_s(io)
    io << "[(#{@left}, #{@top})-(#{@right}, #{bottom})] rid=#{@rid}"
  end
end

class Result
  property :score, :rects

  def initialize
    @score = 0.0
    @rects = [] of Rect
  end

  def normalized_score
    (1e9 * @score / @rects.size).round.to_i64
  end
end

class Solver
  @n : Int32

  def initialize(@reqs : Array(Requirement), @timelimit : Int64)
    @n = @reqs.size
    @rnd = XorShift.new(2u64)
    @cooler = INITIAL_COOLER
  end

  def solve
    init_parts = @reqs.map do |r|
      p = Part.new(r.x, r.x + 1, r.y, r.y + 1, r.id)
      p.score = calc_score(p.area, r.r)
      p
    end
    multistart = @n < 110 ? 50 : 32
    halving_step = 5
    states = Array.new(multistart) do
      init_parts.map { |p| Part.new(p) }
    end
    before_time = Time.utc.to_unix_ms
    init_time_budget = (@timelimit - before_time) * 15 // 16
    halving_step.times do |i|
      results = [] of Tuple(Float64, Array(Part))
      step_start_time = Time.utc.to_unix_ms
      states.each_with_index do |state, i|
        timelimit = init_time_budget * (i + 1) // (halving_step * states.size) + step_start_time
        simulated_annealing(state, timelimit, Time.utc.to_unix_ms)
        score = state.map(&.score).sum
        results << {score, state}
      end
      results = results.sort_by { |s, _| -s }
      states = results[0, results.size // 2].map { |_, parts| parts }
    end
    best_parts = states[0]
    before_time = Time.utc.to_unix_ms
    simulated_annealing2(best_parts, @timelimit, before_time)
    # best_parts.each do |p|
    #   STDERR.puts("#{p.left} #{p.right} #{p.top} #{p.bottom} #{@reqs[p.rid].r}")
    # end

    res = Result.new
    res.rects = Array.new(@n) { Rect.of_invalid }
    best_parts.each do |part|
      req = @reqs[part.rid]
      res.rects[part.rid] = Rect.new(part.left, part.right, part.top, part.bottom).shrink_fit(req.x, req.y, req.r)
      part.score = calc_score(res.rects[part.rid].area, req.r)
      res.score += part.score
    end
    return res
  end

  def simulated_annealing(parts, tl, init_time)
    turn = 0
    @cooler = INITIAL_COOLER
    cutoff = -7.0 / @cooler
    score = parts.map(&.score).sum
    best_score = score
    best_parts = parts.map { |p| Part.new(p) }
    old_l = parts.map { |p| p.left }
    old_r = parts.map { |p| p.right }
    old_t = parts.map { |p| p.top }
    old_b = parts.map { |p| p.bottom }
    old_s = parts.map { |p| p.score }
    neighbors = [] of Int32
    # STDERR.puts(parts.size)
    update_cnt = 0
    ratio = 0.0
    fit_mask = 0xF
    while true
      turn += 1
      if (turn & 0x3FF) == 0
        cur_time = Time.utc.to_unix_ms
        if cur_time > tl
          debug("turn:#{turn} score:#{score} best_score:#{best_score} update_cnt:#{update_cnt}")
          break
        else
          ratio = (cur_time - init_time) / (tl - init_time)
          if @n < 80
            @cooler = (1.0 - ratio) * INITIAL_COOLER + ratio * FINAL_COOLER
          else
            @cooler = Math.exp((1.0 - ratio) * Math.log(INITIAL_COOLER) + ratio * Math.log(FINAL_COOLER))
          end
          cutoff = -7.0 / @cooler
        end
        debug("turn:#{turn} cur_score:#{score} cooler:#{@cooler}")
      end
      pi = @rnd.next_int(parts.size).to_i32
      part_ch = parts[pi]
      req = @reqs[part_ch.rid]
      neighbors.clear
      neighbors << pi
      diff = -part_ch.score

      area_bits = @rnd.next_int(15).to_i + 1
      area_left = part_ch.left - (area_bits & 1)
      area_right = part_ch.right + ((area_bits >> 1) & 1)
      area_top = part_ch.top - ((area_bits >> 2) & 1)
      area_bottom = part_ch.bottom + ((area_bits >> 3) & 1)
      parts.each_with_index do |p, i|
        next if p == part_ch
        if intersect(area_left, area_right, p.left, p.right) && intersect(area_top, area_bottom, p.top, p.bottom)
          diff -= p.score
          neighbors << i
        end
      end
      if @n < 100 && part_ch.score >= 0.9999 && neighbors.size == 1
        next
      end
      neighbors.size.times do |i|
        ri = @rnd.next_int(neighbors.size - i) + i
        neighbors[i], neighbors[ri] = neighbors[ri], neighbors[i]
        part = parts[neighbors[i]]
        req = @reqs[part.rid]
        part.left = req.x
        part.right = req.x + 1
        part.top = req.y
        part.bottom = req.y + 1
      end
      neighbors.size.times do |i|
        part = parts[neighbors[i]]
        if (@rnd.next_int & 1) == 0
          left = 0
          right = L
          parts.each do |np|
            next if np == part
            if intersect(part.top, part.bottom, np.top, np.bottom)
              if np.right <= part.left
                left = {left, np.right}.max
              else
                right = {right, np.left}.min
              end
            end
          end
          part.left = (@rnd.next_int & fit_mask) != 0 ? left : part.left - @rnd.next_int(part.left - left + 1)
          part.right = (@rnd.next_int & fit_mask) != 0 ? right : part.right + @rnd.next_int(right - part.right + 1)
          top = 0
          bottom = L
          parts.each do |np|
            next if np == part
            if intersect(part.left, part.right, np.left, np.right)
              if np.bottom <= part.top
                top = {top, np.bottom}.max
              else
                bottom = {bottom, np.top}.min
              end
            end
          end
          r = @reqs[part.rid].r
          w = part.right - part.left
          if w * (bottom - top) <= r
            part.top = top
            part.bottom = bottom
          else
            h = {1, r // w}.max
            if (@rnd.next_int & 1) == 0
              part.top = {part.top - (h - 1), top}.max
              part.bottom = part.top + h
            else
              part.bottom = {part.bottom + (h - 1), bottom}.min
              part.top = part.bottom - h
            end
          end
        else
          top = 0
          bottom = L
          parts.each do |np|
            next if np == part
            if intersect(part.left, part.right, np.left, np.right)
              if np.bottom <= part.top
                top = {top, np.bottom}.max
              else
                bottom = {bottom, np.top}.min
              end
            end
          end
          part.top = (@rnd.next_int & fit_mask) != 0 ? top : part.top - @rnd.next_int(part.top - top + 1)
          part.bottom = (@rnd.next_int & fit_mask) != 0 ? bottom : part.bottom + @rnd.next_int(bottom - part.bottom + 1)
          left = 0
          right = L
          parts.each do |np|
            next if np == part
            if intersect(part.top, part.bottom, np.top, np.bottom)
              if np.right <= part.left
                left = {left, np.right}.max
              else
                right = {right, np.left}.min
              end
            end
          end
          r = @reqs[part.rid].r
          h = part.bottom - part.top
          if h * (right - left) <= r
            part.left = left
            part.right = right
          else
            w = {1, r // h}.max
            if (@rnd.next_int & 1) == 0
              part.left = {part.left - (w - 1), left}.max
              part.right = part.left + w
            else
              part.right = {part.right + (w - 1), right}.min
              part.left = part.right - w
            end
          end
        end
        part.score = calc_score(part.area, r)
        diff += part.score
        if diff + neighbors.size - 1 - i <= cutoff
          break # pruning
        end
      end
      if diff >= 0 || (diff > cutoff && @rnd.next_double < Math.exp(diff * @cooler))
        score += diff
        if score > best_score
          best_score = score
          best_parts = parts.map { |p| Part.new(p) }
        end
        # if update_cnt < 5000
        #   parts.each do |p|
        #     STDERR.puts("#{p.left} #{p.right} #{p.top} #{p.bottom} #{@reqs[p.rid].r}")
        #   end
        # end
        update_cnt += 1
        neighbors.each do |i|
          old_l[i] = parts[i].left
          old_r[i] = parts[i].right
          old_t[i] = parts[i].top
          old_b[i] = parts[i].bottom
          old_s[i] = parts[i].score
        end
      else
        neighbors.each do |i|
          parts[i].left = old_l[i]
          parts[i].right = old_r[i]
          parts[i].top = old_t[i]
          parts[i].bottom = old_b[i]
          parts[i].score = old_s[i]
        end
      end
    end
    parts.size.times do |i|
      parts[i] = best_parts[i]
    end
  end

  def simulated_annealing2(parts, tl, init_time)
    turn = 0
    @cooler = INITIAL_COOLER2
    cutoff = -10.0 / @cooler
    t_pos = Array.new(L + 1) { [] of Int32 }
    b_pos = Array.new(L + 1) { [] of Int32 }
    l_pos = Array.new(L + 1) { [] of Int32 }
    r_pos = Array.new(L + 1) { [] of Int32 }
    score = 0.0
    parts.each_with_index do |p, i|
      p.score = calc_score(p.area, @reqs[p.rid].r)
      t_pos[p.top] << i
      b_pos[p.bottom] << i
      l_pos[p.left] << i
      r_pos[p.right] << i
      score += p.score
    end
    best_score = score
    best_parts = parts.map { |p| Part.new(p) }
    neighbors = [] of Tuple(Int32, Float64)
    align = [] of Part
    ratio = 0.0

    while true
      turn += 1
      if (turn & 0x7FFF) == 0
        cur_time = Time.utc.to_unix_ms
        if cur_time > tl
          debug("turn:#{turn} score:#{score} best_score:#{best_score}")
          break
        else
          ratio = (cur_time - init_time) / (tl - init_time)
          # @cooler = (1.0 - ratio) * INITIAL_COOLER + ratio * FINAL_COOLER
          @cooler = Math.exp((1.0 - ratio) * Math.log(INITIAL_COOLER2) + ratio * Math.log(FINAL_COOLER2))
          cutoff = -10.0 / @cooler
        end
        debug("turn:#{turn} cur_score:#{score} cooler:#{@cooler}")
      end
      pi = @rnd.next_int(parts.size).to_i
      part = parts[pi]
      req = @reqs[part.rid]
      kind = @rnd.next_int
      extend_or_shrink = turn & 1
      # extend_or_shrink = kind & 1
      kind >>= 1
      dir = kind & 3
      neighbors.clear
      diff = -part.score
      if extend_or_shrink == 1
        # extend
        case dir
        when 0
          next if part.left == 0
          diff += calc_score((part.area + part.bottom - part.top), req.r)
          r_pos[part.left].each do |ni|
            np = parts[ni]
            if intersect(part.top, part.bottom, np.top, np.bottom)
              if @reqs[np.rid].x == np.right - 1
                diff = -INF
                break
              end
              ns = calc_score((np.area - (np.bottom - np.top)), @reqs[np.rid].r)
              diff += ns - np.score
              neighbors << {ni, ns}
            end
          end
        when 1
          next if part.right == L
          diff += calc_score((part.area + part.bottom - part.top), req.r)
          l_pos[part.right].each do |ni|
            np = parts[ni]
            if intersect(part.top, part.bottom, np.top, np.bottom)
              if @reqs[np.rid].x == np.left
                diff = -INF
                break
              end
              ns = calc_score((np.area - (np.bottom - np.top)), @reqs[np.rid].r)
              diff += ns - np.score
              neighbors << {ni, ns}
            end
          end
        when 2
          next if part.top == 0
          diff += calc_score((part.area + part.right - part.left), req.r)
          b_pos[part.top].each do |ni|
            np = parts[ni]
            if intersect(part.left, part.right, np.left, np.right)
              if @reqs[np.rid].y == np.bottom - 1
                diff = -INF
                break
              end
              ns = calc_score((np.area - (np.right - np.left)), @reqs[np.rid].r)
              diff += ns - np.score
              neighbors << {ni, ns}
            end
          end
        when 3
          next if part.bottom == L
          diff += calc_score((part.area + part.right - part.left), req.r)
          t_pos[part.bottom].each do |ni|
            np = parts[ni]
            if intersect(part.left, part.right, np.left, np.right)
              if @reqs[np.rid].y == np.top
                diff = -INF
                break
              end
              ns = calc_score((np.area - (np.right - np.left)), @reqs[np.rid].r)
              diff += ns - np.score
              neighbors << {ni, ns}
            end
          end
        end
        if diff == -INF
          next
        end
        if diff >= 0 || (diff > cutoff && @rnd.next_double < Math.exp(diff * @cooler))
          score += diff
          case dir
          when 0
            l_pos[part.left].delete(pi)
            l_pos[part.left - 1] << pi
            part.left -= 1
            neighbors.each do |ni, ns|
              np = parts[ni]
              r_pos[np.right].delete(ni)
              r_pos[np.right - 1] << ni
              np.right -= 1
              np.score = ns
            end
          when 1
            r_pos[part.right].delete(pi)
            r_pos[part.right + 1] << pi
            part.right += 1
            neighbors.each do |ni, ns|
              np = parts[ni]
              l_pos[np.left].delete(ni)
              l_pos[np.left + 1] << ni
              np.left += 1
              np.score = ns
            end
          when 2
            t_pos[part.top].delete(pi)
            t_pos[part.top - 1] << pi
            part.top -= 1
            neighbors.each do |ni, ns|
              np = parts[ni]
              b_pos[np.bottom].delete(ni)
              b_pos[np.bottom - 1] << ni
              np.bottom -= 1
              np.score = ns
            end
          when 3
            b_pos[part.bottom].delete(pi)
            b_pos[part.bottom + 1] << pi
            part.bottom += 1
            neighbors.each do |ni, ns|
              np = parts[ni]
              t_pos[np.top].delete(ni)
              t_pos[np.top + 1] << ni
              np.top += 1
              np.score = ns
            end
          end
          part.score = calc_score(part.area, req.r)
        end
      else
        # shrink
        align.clear
        case dir
        when 0
          next if req.x == part.left
          diff += calc_score((part.area - (part.bottom - part.top)), req.r)
          l_pos[part.left].each do |ai|
            align << parts[ai] if ai != pi
          end
          r_pos[part.left].each do |ni|
            np = parts[ni]
            if np.right == part.left && align.all? { |ap| !intersect(ap.top, ap.bottom, np.top, np.bottom) }
              ns = calc_score((np.area + (np.bottom - np.top)), @reqs[np.rid].r)
              if ns >= np.score
                diff += ns - np.score
                neighbors << {ni, ns}
              end
            end
          end
        when 1
          next if req.x == part.right - 1
          diff += calc_score((part.area - (part.bottom - part.top)), req.r)
          r_pos[part.right].each do |ai|
            align << parts[ai] if ai != pi
          end
          l_pos[part.right].each do |ni|
            np = parts[ni]
            if np.left == part.right && align.all? { |ap| !intersect(ap.top, ap.bottom, np.top, np.bottom) }
              ns = calc_score((np.area + (np.bottom - np.top)), @reqs[np.rid].r)
              if ns >= np.score
                diff += ns - np.score
                neighbors << {ni, ns}
              end
            end
          end
        when 2
          next if req.y == part.top
          diff += calc_score((part.area - (part.right - part.left)), req.r)
          t_pos[part.top].each do |ai|
            align << parts[ai] if ai != pi
          end
          b_pos[part.top].each do |ni|
            np = parts[ni]
            if np.bottom == part.top && align.all? { |ap| !intersect(ap.left, ap.right, np.left, np.right) }
              ns = calc_score((np.area + (np.right - np.left)), @reqs[np.rid].r)
              if ns >= np.score
                diff += ns - np.score
                neighbors << {ni, ns}
              end
            end
          end
        when 3
          next if req.y == part.bottom - 1
          diff += calc_score((part.area - (part.right - part.left)), req.r)
          b_pos[part.bottom].each do |ai|
            align << parts[ai] if ai != pi
          end
          t_pos[part.bottom].each do |ni|
            np = parts[ni]
            if np.top == part.bottom && align.all? { |ap| !intersect(ap.left, ap.right, np.left, np.right) }
              ns = calc_score((np.area + (np.right - np.left)), @reqs[np.rid].r)
              if ns >= np.score
                diff += ns - np.score
                neighbors << {ni, ns}
              end
            end
          end
        end
        if diff >= 0 # || (diff > cutoff && @rnd.next_double < Math.exp(diff * @cooler))
          score += diff
          if ratio > 0.9 && score > best_score
            best_score = score
            best_parts = parts.map { |p| Part.new(p) }
          end
          case dir
          when 0
            l_pos[part.left].delete(pi)
            l_pos[part.left + 1] << pi
            part.left += 1
            neighbors.each do |ni, ns|
              np = parts[ni]
              r_pos[np.right].delete(ni)
              r_pos[np.right + 1] << ni
              np.right += 1
              np.score = ns
            end
          when 1
            r_pos[part.right].delete(pi)
            r_pos[part.right - 1] << pi
            part.right -= 1
            neighbors.each do |ni, ns|
              np = parts[ni]
              l_pos[np.left].delete(ni)
              l_pos[np.left - 1] << ni
              np.left -= 1
              np.score = ns
            end
          when 2
            t_pos[part.top].delete(pi)
            t_pos[part.top + 1] << pi
            part.top += 1
            neighbors.each do |ni, ns|
              np = parts[ni]
              b_pos[np.bottom].delete(ni)
              b_pos[np.bottom + 1] << ni
              np.bottom += 1
              np.score = ns
            end
          when 3
            b_pos[part.bottom].delete(pi)
            b_pos[part.bottom - 1] << pi
            part.bottom -= 1
            neighbors.each do |ni, ns|
              np = parts[ni]
              t_pos[np.top].delete(ni)
              t_pos[np.top - 1] << ni
              np.top -= 1
              np.score = ns
            end
          end
          part.score = calc_score(part.area, req.r)
        end
      end
    end
    parts.size.times do |i|
      parts[i] = best_parts[i]
    end
  end
end
