START_TIME     = Time.utc.to_unix_ms
TL             = (ENV["TL"]? || 1980).to_i
INITIAL_COOLER = (ENV["IC"]? || 0.7).to_f
FINAL_COOLER   = (ENV["FC"]? || 1.5).to_f
N              = 200
M              =  10
INF            = 1 << 29
RND            = Random.new(3)

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
  def to_s(io)
    io << "(#{@y},#{@x})"
  end
end

class Result
  property :ops, :consume

  def initialize
    @ops = Array(Tuple(Int32, Int32)).new
    @consume = 0
  end

  def initialize(res : Result)
    @ops = res.ops.dup
    @consume = res.consume
  end

  def add_move(v, i, count)
    @ops << {v, i + 1}
    @consume += count + 1
  end

  def add_use(v)
    @ops << {v, 0}
  end

  def score
    return @ops.size <= 5000 ? {1, 10000 - @consume}.max : 0
  end

  def to_s(io)
    @ops.each do |op|
      io << "#{op[0]} #{op[1]}\n"
    end
    io
  end
end

class Solver
  @b : Array(Array(Int32))

  def initialize(@orig_b : Array(Array(Int32)))
    @b = @orig_b.map { |s| s.dup }
    @res = Result.new
    @next_v = 1
    @done = Array(Bool).new(M, false)
  end

  def move(v, from, to, pos = nil)
    if pos.nil?
      pos = @b[from].index(v).not_nil!
    end
    @res.add_move(v, to, @b[from].size - pos)
    @b[to] += @b[from][pos..]
    @b[from].pop(@b[from].size - pos)
  end

  def use(v, from = nil)
    assert(@next_v == v)
    if from.nil?
      from = M.times.select { |i| !@b[i].empty? && @b[i][-1] == v }.first
    else
      assert(@b[from][-1] == v)
    end
    @b[from].pop
    @res.add_use(v)
    @next_v = v + 1
  end

  def stacksort(bi)
    # TODO: ソート失敗妥協ケースでは、減少列は全部一気に使ってるのを途中で切るのも試す
    bucket = @b[bi]
    # debug("bucket:#{bucket}")
    s = bucket.sort.reverse
    bottom = 0
    while bottom < bucket.size && bucket[bottom] == s[bottom]
      bottom += 1
    end
    return if bottom == bucket.size
    # "未ソートの列から持ってこれるなら持ってくる" ができやすくなるようにバッファの使う順をいじる
    order_stack = M.times.to_a.reject(bi).sort_by { |i| @done[i] ? 0 : 1 }
    cur = bucket.size - 1
    stack = Array.new(M) { [] of Int32 }
    max_done = 0
    M.times { |i| max_done = {max_done, @b[i][-1]}.max if @done[i] && !@b[i].empty? }
    while cur >= bottom
      if bucket[-1] == @next_v
        # 途中で取り出しをやる
        use(@next_v, bi)
        s.pop
        cur -= 1
        next
      end
      if bucket[-1] < max_done
        # 他のソート済み列の上に移動する
        M.times do |i|
          if @done[i] && stack[i].empty? && !@b[i].empty? && @b[i][-1] == max_done
            s.delete(bucket[-1])
            move(bucket[-1], bi, i, bucket.size - 1)
            cur -= 1
            break
          end
        end
        max_done = 0
        M.times { |i| max_done = {max_done, @b[i][-1]}.max if @done[i] && !@b[i].empty? && stack[i].empty? }
        next
      end
      ni = cur
      while ni > bottom && bucket[ni - 1] > max_done && bucket[ni - 1] > @next_v
        if bucket[ni - 1] < bucket[ni]
          ni -= 1
        else
          si = s.index(bucket[ni]).not_nil!
          if s[si - 1] == bucket[ni - 1]
            # 複数をまとめて一緒に動かす
            debug("dual move pre #{bucket[ni]} #{bucket[ni - 1]}")
            ni -= 1
          else
            break
          end
        end
      end
      ok = false
      order_stack.each do |to|
        if !stack[to].empty? && stack[to][-1] > bucket[ni]
          si = s.index(bucket[ni]).not_nil!
          if s[si - 1] == stack[to][-1]
            debug("planned dual move")
            stack[to] += bucket[ni..cur]
            move(bucket[ni], bi, to, ni)
            ok = true
            break
          end
        end
      end
      if !ok
        order_stack.each do |to|
          if stack[to].empty? || stack[to][-1] < bucket[ni]
            stack[to] += bucket[ni..cur]
            move(bucket[ni], bi, to, ni)
            ok = true
            break
          end
        end
      end
      if !ok
        debug("fail stack #{bi} #{ni}")
        to = M.times.min_by { |i| i == bi ? INF : stack[i][-1] }
        stack[to] += bucket[ni..cur]
        move(bucket[ni], bi, to, ni)
      end
      max_done = 0
      M.times { |i| max_done = {max_done, @b[i][-1]}.max if @done[i] && !@b[i].empty? && stack[i].empty? }
      cur = ni - 1
    end
    # debug("stack:#{stack}")
    while bottom < s.size
      nv = s[bottom]
      from = -1
      pos = -1
      M.times do |i|
        p = stack[i].index(nv)
        if !p.nil?
          from = i
          pos = p
          break
        end
      end
      assert(from != -1)
      assert(pos != -1)
      if stack[from].size == pos + 1
        stack[from].pop
        move(nv, from, bi, @b[from].size - 1)
        bottom += 1
      else
        if pos == stack[from].size - 2 && stack[from][pos + 1] == s[bottom + 1]
          # 2つ一緒に動かす
          debug("dual move #{nv} #{from} #{bi} #{@b[from][-1]}")
          move(nv, from, bi, @b[from].size - 2)
          stack[from].pop(2)
          bottom += 2
        elsif pos == stack[from].size - 3 && stack[from][pos + 1] == s[bottom + 1] && stack[from][pos + 2] == s[bottom + 2]
          # 3つ一緒に動かす
          debug("triple move #{nv} #{from} #{bi} #{@b[from][-1]}")
          move(nv, from, bi, @b[from].size - 3)
          stack[from].pop(3)
          bottom += 3
        elsif pos == stack[from].size - 4 && stack[from][pos + 1] == s[bottom + 1] && stack[from][pos + 2] == s[bottom + 2] && stack[from][pos + 3] == s[bottom + 3]
          # 4つ一緒に動かす
          debug("4 move #{nv} #{from} #{bi} #{@b[from][-1]}")
          move(nv, from, bi, @b[from].size - 4)
          stack[from].pop(4)
          bottom += 4
        else
          debug("fail sort #{bi} #{nv} #{bottom}")
          to = M.times.min_by { |i| i == bi || i == from ? INF : stack[i].empty? ? 0 : stack[i][-1] }
          move(stack[from][pos + 1], from, to)
          stack[to] += stack[from][(pos + 1)..]
          stack[from].pop(stack[from].size - pos)
          move(nv, from, bi, @b[from].size - 1)
          bottom += 1
        end
      end
      if !stack[from].empty? && stack[from][-1] == @next_v
        # 途中で取り出しをやる
        use(@next_v)
        stack[from].pop
        s.pop
      end
      M.times do |i|
        # 未ソートの列から持ってこれるなら持ってくる
        if bottom < s.size && !@done[i] && stack[i].empty? && !@b[i].empty? && s[bottom] < @b[i][-1] < @b[bi][-1]
          move(@b[i][-1], i, bi, @b[i].size - 1)
        end
      end
    end
  end

  def solve_one(initial_moves)
    @b = @orig_b.map { |s| s.dup }
    @res = Result.new
    @next_v = 1
    @done.fill(false)

    initial_moves.each do |from, to, count|
      if @b[from].size <= count
        # fail
        @res.consume = 10000
        return @res
      end
      move(@b[from][-count], from, to, @b[from].size - count)
    end

    # ソート途中で取り出しできるよう小さいのがある所からやる
    order = M.times.to_a.sort_by { |i| @b[i].min }
    order.each do |i|
      stacksort(i)
      @done[i] = true
      # debug(@b.join("\n"))
    end
    @next_v.upto(N) do |i|
      # debug("use #{i}")
      use(i)
    end
    debug("score:#{@res.score}")
    return @res
  end

  def solve(timelimit)
    initial_moves = [] of Tuple(Int32, Int32, Int32) # from, to, count
    best_result = solve_one(initial_moves)
    turn = 0
    # 最初にランダムmoveして山登り
    while true
      turn += 1
      elapsed = Time.utc.to_unix_ms - START_TIME
      if elapsed > timelimit
        debug("total_turn:#{turn}")
        break
      end
      cooler = INITIAL_COOLER + (FINAL_COOLER - INITIAL_COOLER) * elapsed / timelimit
      if initial_moves.size <= 1 || RND.rand(3) != 0
        pos = RND.rand(2) == 0 ? RND.rand(initial_moves.size + 1) : initial_moves.size
        from = RND.rand(M)
        to = RND.rand(M - 1)
        to += 1 if to >= from
        count = RND.rand(2) + 1
        initial_moves.insert(pos, {from, to, count})
        res = solve_one(initial_moves)
        if accept(res.score - best_result.score, cooler)
          if res.score >= best_result.score
            debug("best_score:#{res.score} at turn #{turn} I")
            best_result = res
          end
        else
          initial_moves.delete_at(pos)
        end
      else
        pos = RND.rand(initial_moves.size)
        removed = initial_moves[pos]
        initial_moves.delete_at(pos)
        res = solve_one(initial_moves)
        if accept(res.score - best_result.score, cooler)
          if res.score >= best_result.score
            debug("best_score:#{res.score} at turn #{turn} D")
            best_result = res
          end
        else
          initial_moves.insert(pos, removed)
        end
      end
    end
    return best_result
  end

  def accept(diff, cooler)
    return true if diff >= 0
    v = diff * cooler
    return false if v < -8
    return RND.rand < Math.exp(v)
  end
end

def main
  gets
  b = Array.new(M) { read_line.split.map(&.to_i) }
  solver = Solver.new(b)
  best_res = solver.solve(TL)
  puts best_res
  {% if flag?(:local) %}
    STDERR.puts("raw_score:#{best_res.consume} score:#{best_res.score}")
  {% end %}
end

main
