START_TIME     = Time.utc.to_unix_ms
TL             = 1950
INF            = 1 << 29
N              = 1000
M              =   50
MULTISTART     =    1
INITIAL_COOLER = 0.01
FINAL_COOLER   = 0.20

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

class Pos
  getter :x, :y
  property :d
  @d : Delivery?

  def initialize(@y : Int32, @x : Int32)
    @d = nil
  end

  def dist(p)
    return (p.x - @x).abs + (p.y - @y).abs
  end

  def dist(y, x)
    return (x - @x).abs + (y - @y).abs
  end

  def to_s(io)
    io << @x << " " << @y
  end
end

class Delivery
  getter :s, :t, :id

  def initialize(@s : Pos, @t : Pos, @id : Int32)
  end
end

main

def main
  ds = Array.new(1000) do |i|
    a, b, c, d = read_line.split.map(&.to_i)
    deliv = Delivery.new(Pos.new(b, a), Pos.new(d, c), i)
    deliv.s.d = deliv
    deliv.t.d = deliv
    deliv
  end
  solver = Solver.new(ds, START_TIME + TL)
  result = solver.solve
  debug("score:#{(100_000_000 / (1000 + result.score)).round.to_i} raw=#{result.score}")
  puts "50 " + result.dis.map { |i| i + 1 }.join(" ")
  puts "102 " + result.path.join(" ")
end

class Result
  property :dis, :path, :score

  def initialize(@dis : Array(Int32), @path : Array(Pos), @score : Int32)
  end
end

class Solver
  def initialize(@ds : Array(Delivery), @timelimit : Int64)
    @rnd = XorShift.new(2u64)
    @home = Pos.new(400, 400)
    debug("init time:#{Time.utc.to_unix_ms - START_TIME}")
  end

  def solve
    cur_time = Time.utc.to_unix_ms
    time_budget = @timelimit - cur_time
    # best_result = solve_one(START_TIME + TL)
    best_result = solve_one(cur_time + time_budget // MULTISTART)
    2.upto(MULTISTART) do |i|
      result = solve_one(cur_time + time_budget * i // MULTISTART)
      if result.score > best_result.score
        debug("score:#{result.score} at turn #{i - 1}")
        best_result = result
      end
    end
    return best_result
  end

  def greedy(dis)
    path = [@home, @home]
    time = 0
    M.times do |i|
      di = dis[i]
      cur_d = @ds[di]
      min_diff = INF
      min_pi_s = 0
      (path.size - 1).times do |i|
        diff = path[i].dist(cur_d.s) + cur_d.s.dist(path[i + 1]) - path[i].dist(path[i + 1])
        if diff < min_diff
          min_diff = diff
          min_pi_s = i
        end
      end
      path.insert(min_pi_s + 1, cur_d.s)
      time += min_diff

      min_diff = INF
      min_pi_t = 0
      (min_pi_s + 1).upto(path.size - 2) do |i|
        diff = path[i].dist(cur_d.t) + cur_d.t.dist(path[i + 1]) - path[i].dist(path[i + 1])
        if diff < min_diff
          min_diff = diff
          min_pi_t = i
        end
      end
      path.insert(min_pi_t + 1, cur_d.t)
      time += min_diff
    end
    return path, time
  end

  def solve_one(tl)
    cp = @home
    ds_s_pi = Array.new(N, -1)
    ds_t_pi = Array.new(N, -1)
    ndis = N.times.to_a.sort_by { |i| {@ds[i].s.dist(@home), @ds[i].t.dist(@home)}.max }
    active_dps = ndis[0, M]
    inactive_dps = ndis[M, N - M]
    path, time = greedy(ndis)
    best_time = time
    best_path = path.dup
    best_active_dps = active_dps.dup
    1.upto(path.size - 2) do |i|
      d = path[i].d.not_nil!
      if d.s == path[i]
        ds_s_pi[d.id] = i
      else
        ds_t_pi[d.id] = i
      end
    end

    path_dist = Array.new(path.size - 1) { |i| path[i].dist(path[i + 1]) }
    cooler = INITIAL_COOLER
    turn = 0
    begin_time = Time.utc.to_unix_ms
    while true
      turn += 1
      if (turn & 0x7FF) == 0
        cur_time = Time.utc.to_unix_ms
        if cur_time > tl
          debug("improve turn:#{turn}")
          break
        end
        ratio = (cur_time - begin_time) / (tl - begin_time)
        cooler = Math.exp(Math.log(INITIAL_COOLER) * (1.0 - ratio) + Math.log(FINAL_COOLER) * ratio)
        debug("cooler:#{cooler}")
      end
      rm_pos = @rnd.next_int(M)
      rm_di = active_dps[rm_pos]
      rm_s_pi = ds_s_pi[rm_di]
      rm_t_pi = ds_t_pi[rm_di]
      new_path = path.dup
      new_path.delete_at(rm_t_pi)
      new_path.delete_at(rm_s_pi)
      ins_pos = @rnd.next_int(inactive_dps.size)
      ins_di = inactive_dps[ins_pos]
      ins_d = @ds[ins_di]
      new_time = time
      if rm_s_pi == rm_t_pi - 1
        new_time -= path_dist[rm_s_pi - 1] + path_dist[rm_s_pi] + path_dist[rm_t_pi] - path[rm_s_pi - 1].dist(path[rm_t_pi + 1])
      else
        new_time -= path_dist[rm_s_pi - 1] + path_dist[rm_s_pi] - path[rm_s_pi - 1].dist(path[rm_s_pi + 1])
        new_time -= path_dist[rm_t_pi - 1] + path_dist[rm_t_pi] - path[rm_t_pi - 1].dist(path[rm_t_pi + 1])
      end

      min_d_s = INF
      min_pi_s = -1
      (new_path.size - 1).times do |pi|
        dist = new_path[pi].dist(ins_d.s) + ins_d.s.dist(new_path[pi + 1]) - new_path[pi].dist(new_path[pi + 1])
        if dist < min_d_s
          min_d_s = dist
          min_pi_s = pi
        end
      end
      min_d_t = ins_d.s.dist(ins_d.t) + ins_d.t.dist(new_path[min_pi_s + 1]) - ins_d.s.dist(new_path[min_pi_s + 1])
      min_pi_t = min_pi_s + 1
      (min_pi_s + 1).upto(new_path.size - 2) do |pi|
        dist = new_path[pi].dist(ins_d.t) + ins_d.t.dist(new_path[pi + 1]) - new_path[pi].dist(new_path[pi + 1])
        if dist < min_d_t
          min_d_t = dist
          min_pi_t = pi + 1
        end
      end
      new_time += min_d_s + min_d_t
      diff = new_time - time
      if accept(diff, cooler)
        new_path.insert(min_pi_s + 1, ins_d.s)
        new_path.insert(min_pi_t + 1, ins_d.t)
        time = new_time
        active_dps[rm_pos] = ins_di
        inactive_dps[ins_pos] = rm_di
        path = new_path
        debug("time:#{time} turn #{turn}")
        if time < best_time
          best_time = time
          best_path = new_path
          best_active_dps = active_dps.dup
        end
        path_dist[0] = path[0].dist(path[1])
        1.upto(path.size - 2) do |i|
          path_dist[i] = path[i].dist(path[i + 1])
          d = path[i].d.not_nil!
          if d.s == path[i]
            ds_s_pi[d.id] = i
          else
            ds_t_pi[d.id] = i
          end
        end
      end
    end

    return Result.new(best_active_dps, best_path, best_time)
  end

  # def solve_one2(tl)
  #   range = 200
  #   uds = [] of Delivery
  #   center = Pos.new(@rnd.next_int(800 - range * 2 + 1) + range, @rnd.next_int(800 - range * 2 + 1) + range)
  #   while true
  #     uds = @ds.select { |d| d.s.dist(center) <= 400 && d.t.dist(center) <= 400 }
  #     if uds.size >= M
  #       break
  #     end
  #     center = Pos.new(@rnd.next_int(401) + 200, @rnd.next_int(401) + 200)
  #   end
  #   did2udi = Array.new(N, -1)
  #   uds.size.times do |i|
  #     did2udi[uds[i].id] = i
  #   end
  #   debug("center:#{center} uds:#{uds.size}")
  #   cp = @home
  #   used_s = Array.new(uds.size, false)
  #   used_t = Array.new(uds.size, false)
  #   ds_s_pi = Array.new(uds.size, -1)
  #   ds_t_pi = Array.new(uds.size, -1)
  #   s_cnt = 0
  #   path = [@home]
  #   time = 0
  #   (M * 2).times do |turn|
  #     min_t = INF
  #     min_di = -1
  #     min_s = false
  #     uds.size.times do |i|
  #       if s_cnt < M && !used_s[i]
  #         t = cp.dist(uds[i].s)
  #         if t < min_t
  #           min_t = t
  #           min_di = i
  #           min_s = true
  #         end
  #       end
  #       if used_s[i] && !used_t[i]
  #         t = cp.dist(uds[i].t)
  #         if t < min_t
  #           min_t = t
  #           min_di = i
  #           min_s = false
  #         end
  #       end
  #     end
  #     d = uds[min_di]
  #     if min_s
  #       path << d.s
  #       used_s[min_di] = true
  #       ds_s_pi[min_di] = path.size - 1
  #       s_cnt += 1
  #     else
  #       path << d.t
  #       used_t[min_di] = true
  #       ds_t_pi[min_di] = path.size - 1
  #     end
  #     cp = path[-1]
  #     time += min_t
  #   end
  #   time += cp.dist(@home)
  #   path << @home
  #   active_dps = [] of Int32
  #   inactive_dps = [] of Int32
  #   uds.size.times do |i|
  #     if used_s[i]
  #       active_dps << i
  #     else
  #       inactive_dps << i
  #     end
  #   end
  #   sum = 0
  #   (path.size - 1).times do |i|
  #     sum += path[i].dist(path[i + 1])
  #   end
  #   if sum != time
  #     raise "fail: #{sum} #{time}"
  #   end

  #   move_pi = Array.new(2 * M + 2, 0)
  #   10000.times do |i|
  #     rm_pos = @rnd.next_int(M)
  #     rm_di = active_dps[rm_pos]
  #     rm_s_pi = ds_s_pi[rm_di]
  #     rm_t_pi = ds_t_pi[rm_di]
  #     diff = 0
  #     if rm_s_pi + 1 == rm_t_pi
  #       diff += path[rm_s_pi - 1].dist(path[rm_t_pi + 1])
  #       diff -= path[rm_s_pi - 1].dist(path[rm_s_pi]) + path[rm_s_pi].dist(path[rm_s_pi + 1]) + path[rm_t_pi].dist(path[rm_t_pi + 1])
  #       next
  #     else
  #       diff += path[rm_s_pi - 1].dist(path[rm_s_pi + 1])
  #       diff -= path[rm_s_pi - 1].dist(path[rm_s_pi]) + path[rm_s_pi].dist(path[rm_s_pi + 1])
  #       diff += path[rm_t_pi - 1].dist(path[rm_t_pi + 1])
  #       diff -= path[rm_t_pi - 1].dist(path[rm_t_pi]) + path[rm_t_pi].dist(path[rm_t_pi + 1])
  #     end
  #     ins_pos = @rnd.next_int(inactive_dps.size + 1)
  #     ins_di = (ins_pos == inactive_dps.size ? rm_di : inactive_dps[ins_pos])
  #     ins_d = uds[ins_di]
  #     pps = 0
  #     if (@rnd.next_int & 0) == 0
  #       # new_path = path.dup
  #       # new_path.delete_at(rm_t_pi)
  #       # new_path.delete_at(rm_s_pi)
  #       # sum = 0
  #       # (new_path.size - 1).times do |i|
  #       #   sum += new_path[i].dist(new_path[i + 1])
  #       # end
  #       # if sum != time + diff
  #       #   debug(new_path.join(" "))
  #       #   raise "fail: #{sum} #{time} #{diff} #{new_path.size}"
  #       # end

  #       min_d_s = INF
  #       min_pi_s = -1
  #       (path.size - 1).times do |pi|
  #         next if pi == rm_s_pi - 1 || pi == rm_s_pi || pi == rm_t_pi - 1 || pi == rm_t_pi
  #         dist = path[pi].dist(ins_d.s) + ins_d.s.dist(path[pi + 1]) - path[pi].dist(path[pi + 1])
  #         if dist < min_d_s
  #           min_d_s = dist
  #           min_pi_s = pi
  #           break
  #         end
  #       end

  #       min_d_t = ins_d.s.dist(ins_d.t) + ins_d.t.dist(path[min_pi_s + 1]) - ins_d.s.dist(path[min_pi_s + 1])
  #       min_pi_t = min_pi_s
  #       (min_pi_s + 1).upto(path.size - 2) do |pi|
  #         next if pi == rm_s_pi - 1 || pi == rm_s_pi || pi == rm_t_pi - 1 || pi == rm_t_pi
  #         dist = path[pi].dist(ins_d.t) + ins_d.t.dist(path[pi + 1]) - path[pi].dist(path[pi + 1])
  #         if dist < min_d_t
  #           min_d_t = dist
  #           min_pi_t = pi
  #         end
  #       end
  #       diff += min_d_s + min_d_t
  #       debug("diff:#{diff}")
  #       if accept(diff, 3.0)
  #         time += diff
  #         if rm_di != ins_di
  #           active_dps[rm_pos] = ins_di
  #           inactive_dps[ins_pos] = rm_di
  #         end
  #         # new_path.insert(min_pi_t + 1, ins_d.t)
  #         # new_path.insert(min_pi_s + 1, ins_d.s)
  #         new_path = [] of Pos
  #         path.size.times do |pi|
  #           if rm_s_pi != pi && rm_t_pi != pi
  #             new_path << path[pi]
  #           end
  #           if min_pi_s == pi
  #             new_path << ins_d.s
  #           end
  #           if min_pi_t == pi
  #             new_path << ins_d.t
  #           end
  #         end
  #         path = new_path
  #         1.upto(path.size - 2) do |i|
  #           d = path[i].d.not_nil!
  #           if d.s == path[i]
  #             ds_s_pi[did2udi[d.id]] = i
  #           else
  #             ds_t_pi[did2udi[d.id]] = i
  #           end
  #         end
  #         sum = 0
  #         (path.size - 1).times do |i|
  #           sum += path[i].dist(path[i + 1])
  #         end
  #         if sum != time
  #           debug(path.join(" "))
  #           raise "fail: #{sum} #{time} #{diff} #{path.size} #{min_pi_s} #{min_pi_t}"
  #         end
  #       end
  #     else
  #     end
  #   end

  #   dis = active_dps.map { |p| uds[p].id }
  #   return Result.new(dis, path, time)
  # end

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
        ratio = (cur_time - begin_time) / (tl - begin_time)
        cooler = Math.exp(Math.log(INITIAL_COOLER) * (1.0 - ratio) + Math.log(FINAL_COOLER) * ratio)
      end
      p1 = @rnd.next_int(path.size - 1)
      p2 = @rnd.next_int(path.size - 1)
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
end
