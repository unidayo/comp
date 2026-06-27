START_TIME = Time.utc.to_unix_ms
TL         = (ENV["TL"]? || 1980).to_i
PART       = (ENV["PART"]? || 3).to_i
IC         = (ENV["IC"]? || 0.02).to_f
FC         = (ENV["FC"]? || 2.0).to_f
INIT_RANGE = (ENV["IR"]? || 50).to_i
INIT_GAP   = (ENV["IG"]? || 2).to_i
RATIO_SWAP = (ENV["RS"]? || 8).to_i
N          =    100
L          = 500000
INF        = 1u64 << 62
RND        = Random.new(2)

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

class Result
  property :score_raw
  getter :a, :b, :score
  @score : Int32

  def initialize(@a : Array(Int32), @b : Array(Int32), @score_raw : Int32)
    @score = 1_000_000 - @score_raw
  end

  def to_s(io)
    N.times do |i|
      io << "#{@a[i]} #{@b[i]}\n"
    end
  end
end

RES_EMPTY = Result.new([] of Int32, [] of Int32, 1000000)

class Solver
  def initialize(@t : Array(Int32))
  end

  def greedy(person_ord, side_ord)
    left = @t.dup
    a = Array.new(N, 0)
    b = Array.new(N, 0)
    ab = [a, b]
    person_ord.each do |i|
      side_ord[i].each do |side|
        mv = (@t[i] + 1 - side) // 2
        to = N.times.min_by { |j| left[j] > mv + INIT_GAP ? (left[j] - mv).abs : L }
        if (left[to] - mv).abs > INIT_RANGE
          to = N.times.max_by { |j| left[j] }
        end
        ab[side][i] = to
        left[to] -= mv
      end
    end
    return ab
  end

  def solve(timelimit)
    turn = 0
    person_ord = N.times.to_a.sort_by { |i| -@t[i] }
    side_ord = Array.new(N) { {0, 1} }
    ab = greedy(person_ord, side_ord)
    best_ab = ab

    cur_sum = Array.new(N, 0)
    N.times do |i|
      cur_sum[ab[0][i]] += (@t[i] + 1) // 2
      cur_sum[ab[1][i]] += @t[i] // 2
    end
    cur_val = N.times.sum { |i| (@t[i] - cur_sum[i]).abs }
    best_val = cur_val
    best_score = L
    ic = IC
    fc = FC
    cooler = ic
    init_time = Time.utc.to_unix_ms
    sim = false
    while true
      turn += 1
      if (turn & 0xFF) == 0
        cur_time = Time.utc.to_unix_ms
        if cur_time > timelimit - 50
          sim = true
        end
        if cur_time > timelimit
          debug("turn:#{turn}\n")
          break
        end
        ratio = (cur_time - init_time) / (timelimit - init_time)
        cooler = Math.exp((1.0 - ratio) * Math.log(ic) + ratio * Math.log(fc))
      end
      type = RND.rand(RATIO_SWAP)
      if type == 0
        ch_pos = RND.rand(N * 2)
        ch_side = ch_pos & 1
        ch_pos >>= 1
        ch_to = RND.rand(N - 1)
        ch_from = ab[ch_side][ch_pos]
        if ch_to >= ch_from
          ch_to += 1
        end
        if ab[1 - ch_side][ch_pos] == ch_pos && ch_to == ch_pos
          next
        end
        val_diff = -(@t[ch_to] - cur_sum[ch_to]).abs - (@t[ch_from] - cur_sum[ch_from]).abs
        mv = (@t[ch_pos] + 1 - ch_side) // 2
        val_diff += (@t[ch_to] - (cur_sum[ch_to] + mv)).abs
        val_diff += (@t[ch_from] - (cur_sum[ch_from] - mv)).abs
        if accept(val_diff, cooler)
          ab[ch_side][ch_pos] = ch_to
          cur_sum[ch_to] += mv
          cur_sum[ch_from] -= mv
          cur_val += val_diff
          if cur_val < best_val && sim
            best_val = cur_val
            score = simulate(ab)
            if score < best_score
              best_ab = ab.map { |v| v.dup }
              best_score = score
              debug("best_score:#{best_score}")
            end
          end
          # debug("0 cur_val:#{cur_val} val_diff:#{val_diff} turn:#{turn}")
        end
      else
        ch_pos0 = RND.rand(N * 2)
        ch_side0 = ch_pos0 & 1
        ch_pos0 >>= 1
        ch_pos1 = RND.rand(N * 2)
        ch_side1 = ch_pos1 & 1
        ch_pos1 >>= 1
        if ch_pos0 == ch_pos1
          next
        end
        to0 = ab[ch_side0][ch_pos0]
        to1 = ab[ch_side1][ch_pos1]
        if to0 == to1
          next
        end
        if ab[1 - ch_side0][ch_pos0] == ch_pos0 && to1 == ch_pos0
          next
        end
        if ab[1 - ch_side1][ch_pos1] == ch_pos1 && to0 == ch_pos1
          next
        end
        val_diff = -(@t[to0] - cur_sum[to0]).abs - (@t[to1] - cur_sum[to1]).abs
        mv0 = (@t[ch_pos0] + 1 - ch_side0) // 2
        mv1 = (@t[ch_pos1] + 1 - ch_side1) // 2
        val_diff += (@t[to0] - (cur_sum[to0] - mv0 + mv1)).abs
        val_diff += (@t[to1] - (cur_sum[to1] + mv0 - mv1)).abs
        if accept(val_diff, cooler)
          ab[ch_side0][ch_pos0] = to1
          ab[ch_side1][ch_pos1] = to0
          cur_sum[to0] += mv1 - mv0
          cur_sum[to1] += mv0 - mv1
          cur_val += val_diff
          if cur_val < best_val && sim
            best_val = cur_val
            score = simulate(ab)
            if score < best_score
              best_ab = ab.map { |v| v.dup }
              best_score = score
              debug("best_score:#{best_score}")
            end
          end
          # debug("1 cur_val:#{cur_val} #{N.times.sum { |i| (@t[i] - cur_sum[i]).abs }} val_diff:#{val_diff} turn:#{turn}")
        end
      end
    end
    if best_score == L
      best_ab = ab
    end
    debug("score:#{best_score} val:#{cur_val}")
    return Result.new(best_ab[0], best_ab[1], best_score)
  end

  def simulate(ab)
    cur = 0
    cnt = Array.new(N, 0)
    cnt[cur] += 1
    (L - 1).times do
      cur = ab[cnt[cur] & 1][cur]
      cnt[cur] += 1
    end
    return N.times.sum { |i| (@t[i] - cnt[i]).abs }
  end

  def accept(diff, cooler)
    return true if diff <= 0
    v = -diff * cooler
    return false if v < -8
    return RND.rand(Int32) < (1 << (31 + v).round.to_i)
    # return RND.rand < Math.exp(v)
  end
end

def main
  gets
  t = read_line.split.map(&.to_i)
  solver = Solver.new(t)
  # best_res = solver.solve(START_TIME + TL)
  best_res = RES_EMPTY
  part = PART
  part.times do |i|
    res = solver.solve(START_TIME + TL * (i + 1) // part)
    if res.score_raw < best_res.score_raw
      best_res = res
    end
  end
  puts best_res
  {% if flag?(:local) %}
    STDERR.printf("raw:%6d final_score:%6d\n", best_res.score_raw, best_res.score)
  {% end %}
end

main
