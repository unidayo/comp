START_TIME     = Time.utc.to_unix_ms
TL             =  2950
N              =    20
INITIAL_COOLER =  10.1
FINAL_COOLER   = 100.0
INF            =   1e9
EMPTY          =    -1
SCORE_EMPTY    =   0.5
COUNTER        = Counter.new

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
  _, m = read_line.split.map(&.to_i)
  ss = Array.new(m) do
    read_line.chars.map { |c| c - 'A' }
  end
  solver = Solver.new(m, ss, START_TIME + TL)
  result = solver.solve
  debug("score:#{100.0 * result.score / m}")
  puts(
    result.rows.map do |r|
      r.map { |c| c == EMPTY ? '.' : 'A' + c }.join
    end.join("\n")
  )
end

class Result
  property :score, :rows

  def initialize(@score : Int32, @rows : Array(Array(Int32)))
  end
end

class Pos
  property :r, :c, :dir

  def initialize(@r : Int32, @c : Int32, @dir : Int32)
  end
end

class State
  property :used, :f, :score, :pi

  @used : Array(Bool)
  @f : Array(Array(Int32))

  def initialize(m)
    @used = Array.new(m, false)
    @f = [] of Array(Int32)
    @score = 0
    @pi = -1
  end

  def initialize(used, score, @pi : Int32)
    @used = used
    @f = [] of Array(Int32)
    @score = score
  end
end

class Solver
  @f : Array(Array(Int32))
  @cooler : Float64
  @cutoff : Float64
  @l : Int32
  @pfs : Array(Array(Array(Int32)))
  @sv : Array(UInt64)

  def initialize(@m : Int32, @ss : Array(Array(Int32)), @timelimit : Int64)
    @f = Array.new(N) { Array.new(N, EMPTY) }
    @rnd = XorShift.new(2u64)
    @cooler = INITIAL_COOLER
    @cutoff = -10.0 / @cooler
    @l = @ss.max_of { |s| s.size } - 2
    @pfs = Array.new(8) { Array.new(8) { [] of Int32 } }
    @ss.each.with_index do |s, i|
      @pfs[s[0]][s[1]] << i
    end
    @pfs.each do |aa|
      aa.each do |a|
        a.sort_by! { |i| -@ss[i].size }
      end
    end
    @sv = @ss.map do |s|
      v = 0u64
      s.each do |ch|
        v <<= 3
        v |= ch
      end
      v
    end
  end

  def connect_str(row, x, si)
    s = @ss[si]
    s.size.times do |i|
      if row[x + i] != EMPTY && row[x + i] != s[i]
        return false
      end
    end
    s.size.times do |i|
      if row[x + i] == EMPTY
        row[x + i] = s[i]
        if x + i < N
          row[x + i + N] = s[i]
        end
      end
    end
    return true
  end

  def solve
    total_chars = @ss.map { |s| s.size }.sum
    beam = Array.new(total_chars + 1) { [] of State }
    beam[0] << State.new(@m)
    beam_width = 1500
    beam_trial = 200
    max_spawn = 30
    pc = 1
    (N + 2).times do |y|
      debug("y:#{y}")
      if @timelimit - Time.utc.to_unix_ms < 400
        beam_width = 100
      end
      if @timelimit - Time.utc.to_unix_ms < 100
        beam_width = 10
      end
      turn = 0
      parent_count = Array.new(pc, 0)
      pc = 0
      next_beam = Array.new(total_chars + 1) { [] of State }
      (beam.size - 1).downto(0) do |sc|
        beam[sc].each do |prev_state|
          if parent_count[prev_state.pi] == max_spawn
            next
          end
          parent_count[prev_state.pi] += 1
          cands = [] of Int32
          if y == 2
            nf = Array.new(N) { [EMPTY] * N }
            N.times do |i|
              nf[i][0] = prev_state.f[0][i]
              nf[i][1] = prev_state.f[1][i]
            end
            prev_state.f = nf
          end
          if y <= 1
            @m.times do |i|
              cands << i if !prev_state.used[i]
            end
          else
            @m.times do |i|
              cands << i if !prev_state.used[i] && @ss[i][0] == prev_state.f[y - 2][0] && @ss[i][1] == prev_state.f[y - 2][1]
            end
            cands << -1
          end
          {cands.size, beam_trial}.min.times do
            8.times do |i|
              8.times do |j|
                pf = @pfs[i][j]
                next if pf.size <= 1
                pos = @rnd.next_int(pf.size)
                pf[pos], pf[0] = pf[0], pf[pos]
              end
            end
            ci = @rnd.next_int(cands.size)
            si = cands[ci]
            cands[ci] = cands[-1]
            cands.pop
            used = prev_state.used.dup
            row = [EMPTY] * (2 * N)
            if y >= 2
              row[0] = row[N] = prev_state.f[y - 2][0]
              row[1] = row[N + 1] = prev_state.f[y - 2][1]
            end
            if si != -1
              @ss[si].each.with_index do |ch, i|
                row[i] = row[i + N] = ch
              end
              used[si] = true
              tail = @ss[si].size
            else
              tail = 2
            end
            score = prev_state.score
            N.times do |x|
              if tail - x > 1
                pf = @pfs[row[x]][row[x + 1]]
                pf.each do |pfi|
                  next if used[pfi]
                  if connect_str(row, x, pfi)
                    used[pfi] = true
                    score += @ss[pfi].size
                  end
                end
              elsif tail - x == 1
                @pfs[row[x]].each do |pfss|
                  pfss.each do |pfi|
                    next if used[pfi]
                    if connect_str(row, x, pfi)
                      used[pfi] = true
                      score += @ss[pfi].size
                    end
                  end
                end
              else
                @m.times do |si|
                  next if used[si]
                  if connect_str(row, x, si)
                    used[si] = true
                    score += @ss[si].size
                  end
                end
              end
              while tail < row.size && row[tail] != EMPTY
                tail += 1
              end
            end
            next_state = State.new(used, score, pc)
            if y == 0
              next_state.f << row[0, N]
            elsif y == 1
              next_state.f = [prev_state.f[0].dup, row[0, N]]
            else
              next_state.f = prev_state.f.map { |row| row.dup }
              N.times do |i|
                next_state.f[y - 2][i] = row[i]
              end
            end
            next_beam[score] << next_state
            # debug("#{prev_state.score} -> #{score}")
          end
          pc += 1
          turn += 1
          break if turn == beam_width
        end
        break if turn == beam_width
      end
      beam = next_beam
    end
    score = beam.size - 1
    best_score = 0
    best_result = Result.new(0, @f)
    field = Array.new(N) { [EMPTY] * N }
    while score > 0
      break if Time.utc.to_unix_ms > @timelimit
      beam[score].each do |st|
        break if Time.utc.to_unix_ms > @timelimit
        unused = Array.new(@l + 3) { [] of UInt64 }
        score_raw = st.used.count(true)
        @m.times do |i|
          next if st.used[i]
          unused[@ss[i].size] << @sv[i]
        end

        vvs = Array.new(@l + 3) { Set(UInt64).new }
        (@l - 2).upto(@l + 2) do |len|
          mask = (1u64 << (3 * len)) - 1
          N.times do |x|
            v = 0u64
            len.times do |i|
              v <<= 3
              v |= st.f[-len + i][x]
            end
            N.times do |y|
              v <<= 3
              v |= st.f[y][x]
              v &= mask
              vvs[len] << v
            end
          end
        end
        v_score = 0
        (@l - 2).upto(@l + 2) do |len|
          v_score += unused[len].count { |v| vvs[len].includes?(v) }
        end
        if score_raw + v_score > best_score
          best_score = score_raw + v_score
          debug("best_score = #{score_raw} + #{v_score} = #{best_score}")
          best_result = Result.new(best_score, st.f)
        end

        # 100.times do
        #   N.times do |x|
        #     field[x][0] = st.f[0][x]
        #   end
        # 1.upto(N - 1) do |y|
        #   x = @rnd.next_int(N)
        #   N.times do |i|
        #     field[x][y] = st.f[y][i]
        #     x += 1
        #     x = 0 if x == N
        #   end
        # end
        # vvs = Array.new(@l + 3) { Set(UInt64).new }
        # (@l - 2).upto(@l + 2) do |len|
        #   mask = (1u64 << (3 * len)) - 1
        #   N.times do |x|
        #     v = 0u64
        #     len.times do |i|
        #       v <<= 3
        #       v |= field[x][-len + i]
        #     end
        #     N.times do |y|
        #       v <<= 3
        #       v |= field[x][y]
        #       v &= mask
        #       vvs[len] << v
        #     end
        #   end
        # end
        # v_score = 0
        # (@l - 2).upto(@l + 2) do |len|
        #   v_score += unused[len].count { |v| vvs[len].includes?(v) }
        # end
        # if score_raw + v_score > best_score
        #   best_score = score_raw + v_score
        #   debug("best_score = #{score_raw} + #{v_score} = #{best_score}")
        #   best_result = Result.new(best_score, field.map { |row| row.dup })
        # end
        # end
      end
      score -= 1
    end
    return best_result
  end

  def solve2
    cnts = Array.new(N) { Array.new(N) { [0] * 8 } }
    total = Array.new(N) { Array.new(N, 0) }
    pos = [] of Pos
    sis = Array.new(N) { Array.new(N) { [] of Int32 } }
    @ss.each.with_index do |s, i|
      r = @rnd.next_int(N)
      c = @rnd.next_int(N)
      dir = (@rnd.next_int & 1).to_i
      pos << Pos.new(r, c, dir)
      if dir == 0
        s.each do |ch|
          cnts[r][c][ch] += 1
          total[r][c] += 1
          r += 1
          r = 0 if r == N
        end
      else
        s.each do |ch|
          cnts[r][c][ch] += 1
          total[r][c] += 1
          c += 1
          c = 0 if c == N
        end
      end
    end
    score = 0.0
    # N.times do |i|
    #   N.times do |j|
    #     score += (cnts[i][j].max / total[i][j]) ** 2
    #   end
    # end

    turn = 0
    @cooler = INITIAL_COOLER
    @cutoff = -10.0 / @cooler
    init_time = Time.utc.to_unix_ms
    sc_max = @ss.map { |s| s.size }.sum // (N * N) + 1
    debug("sc_max:#{sc_max}")
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
          @cutoff = -10.0 / @cooler
          debug("score:#{score} turn:#{turn} cooler:#{@cooler}")
        end
      end
      si = @rnd.next_int(@m)
      nr = @rnd.next_int(N)
      nc = @rnd.next_int(N)
      nd = (@rnd.next_int & 1).to_i
      diff = 0.0
      op = pos[si]
      pr = op.r
      pc = op.c
      if op.dir == 0
        @ss[si].each do |ch|
          diff -= {sc_max, cnts[pr][pc].max}.min * 1.2 - total[pr][pc]
          cnts[pr][pc][ch] -= 1
          total[pr][pc] -= 1
          diff += total[pr][pc] == 0 ? SCORE_EMPTY : {sc_max, cnts[pr][pc].max}.min * 1.2 - total[pr][pc]
          pr += 1
          pr = 0 if pr == N
        end
      else
        @ss[si].each do |ch|
          diff -= {sc_max, cnts[pr][pc].max}.min * 1.2 - total[pr][pc]
          cnts[pr][pc][ch] -= 1
          total[pr][pc] -= 1
          diff += total[pr][pc] == 0 ? SCORE_EMPTY : {sc_max, cnts[pr][pc].max}.min * 1.2 - total[pr][pc]
          pc += 1
          pc = 0 if pc == N
        end
      end
      if nd == 0
        @ss[si].each do |ch|
          diff -= total[nr][nc] == 0 ? SCORE_EMPTY : {sc_max, cnts[nr][nc].max}.min * 1.2 - total[nr][nc]
          cnts[nr][nc][ch] += 1
          total[nr][nc] += 1
          diff += {sc_max, cnts[nr][nc].max}.min * 1.2 - total[nr][nc]
          nr += 1
          nr = 0 if nr == N
        end
        nr -= @ss[si].size
        nr += N if nr < 0
      else
        @ss[si].each do |ch|
          diff -= total[nr][nc] == 0 ? SCORE_EMPTY : {sc_max, cnts[nr][nc].max}.min * 1.2 - total[nr][nc]
          cnts[nr][nc][ch] += 1
          total[nr][nc] += 1
          diff += {sc_max, cnts[nr][nc].max}.min * 1.2 - total[nr][nc]
          nc += 1
          nc = 0 if nc == N
        end
        nc -= @ss[si].size
        nc += N if nc < 0
      end
      if accept(diff)
        pos[si].r = nr
        pos[si].c = nc
        pos[si].dir = nd
        score += diff
      else
        if nd == 0
          @ss[si].each do |ch|
            cnts[nr][nc][ch] -= 1
            total[nr][nc] -= 1
            nr += 1
            nr = 0 if nr == N
          end
        else
          @ss[si].each do |ch|
            cnts[nr][nc][ch] -= 1
            total[nr][nc] -= 1
            nc += 1
            nc = 0 if nc == N
          end
        end
        pr = op.r
        pc = op.c
        if op.dir == 0
          @ss[si].each do |ch|
            cnts[pr][pc][ch] += 1
            total[pr][pc] += 1
            pr += 1
            pr = 0 if pr == N
          end
        else
          @ss[si].each do |ch|
            cnts[pr][pc][ch] += 1
            total[pr][pc] += 1
            pc += 1
            pc = 0 if pc == N
          end
        end
      end
    end
    N.times do |i|
      N.times do |j|
        if total[i][j] == 0
          @f[i][j] = EMPTY
        else
          mv = 0
          8.times do |k|
            if cnts[i][j][k] > mv
              mv = cnts[i][j][k]
              @f[i][j] = k
            end
          end
        end
      end
    end
    N.times do |i|
      N.times do |j|
        debug "#{cnts[i][j]} #{total[i][j]}"
      end
    end
    best_res = Result.new(0, @f)
    return best_res
  end

  def accept(diff)
    return true if diff >= 0
    return false if diff < @cutoff
    return @rnd.next_double < Math.exp(diff * @cooler)
  end
end
