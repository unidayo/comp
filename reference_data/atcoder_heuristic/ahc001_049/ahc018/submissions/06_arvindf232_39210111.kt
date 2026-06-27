import java.io.BufferedInputStream
import java.io.File
import java.io.PrintWriter
import java.util.*
import kotlin.math.absoluteValue
import kotlin.math.exp
import kotlin.math.ln
import kotlin.math.roundToInt
import kotlin.random.Random
import kotlin.random.nextInt
import kotlin.system.measureTimeMillis

// 1. Modded
const val p = 1000000007L
const val pI = p.toInt()
fun Int.adjust():Int{ if(this >= pI){ return this  - pI }else if (this < 0){ return this + pI };return this }
fun Int.snap():Int{ if(this >= pI){return this - pI} else return this}
infix fun Int.mm(b:Int):Int{ return ((this.toLong() * b) % pI).toInt() }
infix fun Int.mp(b:Int):Int{ val ans = this + b;return if(ans >= pI) ans - pI else ans }
infix fun Int.ms(b:Int):Int{ val ans = this - b;return if(ans < 0) ans + pI else ans }
fun Int.inverse():Int = intPow(this,pI-2,pI)
infix fun Int.modDivide(b:Int):Int{ return this mm (b.inverse()) }
fun intPow(x:Int,e:Int,m:Int):Int{
    var X = x ; var E =e ; var Y = 1
    while(E > 0){
        if(E and 1 == 0){
            X = ((1L * X * X) % m).toInt()
            E = E shr 1
        }else{
            Y = ((1L * X * Y) % m).toInt()
            E -= 1
        }
    }
    return Y
}
// 2. DP initial values
const val plarge = 1_000_000_727
const val nlarge = -plarge
const val phuge = 2_727_000_000_000_000_000L
const val nhuge = -phuge
// 3. convenience conversions
val Boolean.chi:Int get() = if(this) 1 else 0 //characteristic function
val BooleanArray.chiarray:IntArray get() = IntArray(this.size){this[it].chi}
val Char.code :Int get() = this.toInt() -  'a'.toInt()
//3. hard to write stuff
fun IntArray.put(i:Int,v:Int){ this[i] = (this[i] + v).adjust() }
val mint:MutableList<Int> get() = mutableListOf<Int>()
val mong:MutableList<Long> get() = mutableListOf<Long>()
val mchar:MutableList<Char> get() = mutableListOf()
//4. more outputs
fun List<Char>.conca():String = this.joinToString("")
val CharArray.conca :String get() = this.joinToString("")
val IntArray.conca :String get() = this.joinToString(" ")
@JvmName("concaInt")
fun List<Int>.conca():String = this.joinToString(" ")
val LongArray.conca:String get() = this.joinToString(" ")
@JvmName("concaLong")
fun List<Long>.conca():String = this.joinToString(" ")
//5. Pair of ints
const val longmask = (1L shl 32) - 1
fun makepair(a:Int, b:Int):Long = (a.toLong() shl 32) xor (longmask and b.toLong())
val Long.first get() = (this ushr 32).toInt()
val Long.second get() = this.toInt()
//6. strings
val String.size get() = this.length
const val randCount = 100
//7. bits
fun Int.has(i:Int):Boolean = (this and (1 shl i) != 0)
fun Long.has(i:Int):Boolean = (this and (1L shl i) != 0L)
//8 TIME
inline fun TIME(f:()->Unit){
    val t = measureTimeMillis(){
        f()
    }
    println("$t ms")
}
//9.ordered pair
fun order(a:Int, b:Int):Pair<Int,Int>{
    return Pair(minOf(a,b), maxOf(a,b))
}
//10 rand
fun rand(x:Int) = Random.nextInt(x)
fun rand(x:IntRange) = Random.nextInt(x)
val coin:Boolean get() = Random.nextBoolean()
class DisjointUnionSets(val n: Int) {
    var size: IntArray = IntArray(n){1}
    var parent: IntArray = IntArray(n){it}
    var components:Int = n

    val successfulUnions:Int get() = n - components

    fun find(x: Int): Int {
        if (parent[x] != x) {
            parent[x] = find(parent[x])
        }
        return parent[x]
    }

    fun union(x: Int, y: Int):Boolean {
        var xRoot = find(x)
        var yRoot = find(y)
        if (xRoot == yRoot){
            return false
        }
        components--
        if(size[xRoot] < size[yRoot]){
            xRoot = yRoot.also { yRoot = xRoot }
        }
        parent[yRoot] = xRoot
        size[xRoot] += size[yRoot]
        return true
    }
    fun getsize(a:Int):Int{
        return size[find(a)]
    }

    var modified = mutableListOf<Int>()
    inline fun eachComponent(act:(Int)->Unit){
        for(i in 0 until n){
            if(parent[i] == i){
                act(i)
            }
        }
    }

    fun printAll(){
        val comps = Array(n){ mutableListOf<Int>()}
        for(i in 0 until n){
            comps[find(i)].add(i)
        }
        println(comps.filter { it.isNotEmpty() }.joinToString("     "))
    }
}
class testcase(val n:Int, val w:Int, val k:Int, val cost:Int, val arr:Array<IntArray>, val live:Boolean, val id:Int) {
    val water = mutableListOf<Pair<Int,Int>>()
    val houses = mutableListOf<Pair<Int,Int>>()

    companion object{
        fun formtc(tc:String, id:Int):testcase {
            Reader.takeFile(tc)
            val (n,w,k,c) = getline(4)
            val arr = Array(n){getline(n)}
            val tc = testcase(n,w,k,c,arr,false, id)
            repeat(w){
                tc.water.add(Pair(getint, getint))
            }
            repeat(k){
                tc.houses.add(Pair(getint, getint))
            }
            return tc
        }
        fun livetc():testcase {
            interactive = true
            val (n,w,k,c) = getline(4)
            val arr = Array(n){IntArray(n){plarge}}
            val tc = testcase(n,w,k,c,arr,true,-1)
            repeat(w){
                tc.water.add(Pair(getint, getint))
            }
            repeat(k){
                tc.houses.add(Pair(getint, getint))
            }
            return tc
        }
    }
}
class interactor(val tc:testcase) {
    var total_energy = 0L
    val cost = tc.cost
    val n = tc.n
    val arr = Array(tc.n){tc.arr[it].copyOf()}
    val already = Array(tc.n){IntArray(n)}
    val breakcost = Array(n){IntArray(n){plarge} }

    val live = tc.live

    val ret = mutableListOf<String>()
    fun dig(x:Int,y:Int, p:Int, confidence:Int = 0,anger:Int = 0):Boolean{
        if(arr[x][y] == 0) return true
        total_energy += p + tc.cost
        var broken:Boolean
        if(live){
            put("$x $y $p")
            val r = getint
            if(r == -1 || r== 2){
                System.exit(0)
            }
            if(r == 1){
                arr[x][y] = 0
            }
            broken = arr[x][y] == 0
        }else{
            ret.add("$x $y $p")
            val new = maxOf(0, arr[x][y] - p)
            arr[x][y] = new
            broken = new == 0
        }
        if(broken){
            breakcost[x][y] = already[x][y] + p
            already[x][y] += confidence
        }else{
            already[x][y] += p
        }
        return broken
    }
    fun iscrushed(x:Int, y:Int):Boolean{
        return arr[x][y] ==0
    }
    fun code(x:Int,y:Int):Int{
        return x * n + y
    }
    fun validate():Boolean{
        val dsu = DisjointUnionSets(n* n + 1 )
        val water = n * n
        val house = water + 1
        for(i in 0 until n){
            for(j in 0 until n){
                if(i > 0){
                    if(iscrushed(i,j) && iscrushed(i-1,j)){
                        dsu.union(code(i,j),code(i-1,j))
                    }
                }
                if(j > 0){
                    if(iscrushed(i,j) && iscrushed(i,j-1)){
                        dsu.union(code(i,j), code(i,j-1))
                    }
                }
            }
        }
        for((a,b) in tc.water){
            if(iscrushed(a,b)){
                dsu.union(code(a,b), water)
            }
        }
        for((i,v) in tc.houses.withIndex()){
            val (a,b) = v
            if(!iscrushed(a,b)){
//				println("Some house isn't crushed")
                return false
            }
            if(dsu.find(code(a,b)) != dsu.find(water)){
//				println("no water to house")
                return false
            }
        }
        return true
    }
    fun output(){
        val x = File("log.txt").printWriter()
        x.use {
            for(c in ret){
                x.println(c)
            }
        }
    }
}
typealias pii = Pair<Int,Int>
open class strategy(val box:interactor){

    val n = box.n
    val water = box.tc.water
    val house = box.tc.houses
    val cost = box.cost

    open val base = (box.cost * 1.4).toInt() + 15

    fun Int.nextlevel():Int{
        return minOf(5000,(this + base).toInt())
    }
    fun Int.nextcap():Int{
        return maxOf(this.nextlevel(),minOf(1e7.toInt(), (this * 1.9).toInt()))
    }


    fun justdig(i:Int, j:Int, p:Int = base, guess:Int = -10000){
        if(box.iscrushed(i,j)) return
        val already = box.already[i][j]
        if(already + p <= guess){
            val extra = ((guess) - already )
            box.dig(i,j,extra)
        }
        while(!box.iscrushed(i,j)){
            box.dig(i,j,p)
        }
    }
    fun justmultidig(i:Int, j:Int, p:Int = base, multiguess:IntArray, confidence:Double, anger:Double){
        if(box.iscrushed(i,j)) return


        while(!box.iscrushed(i,j)){
            val already = box.already[i][j]
            val nextpower = multiguess.firstOrNull{it >= already + p} ?: (already + p )
            val pp = nextpower - already
            if(pp == p){

                // 2.4
                box.dig(i,j,pp,(anger * pp).roundToInt(), 0)
            }else{
                box.dig(i,j,pp,(pp * confidence).roundToInt(),0 )
//				box.dig(i,j,pp, (pp * 0.3).roundToInt())
            }
//			box.dig(i,j,pp)
//			box.dig(i,j, nextpower - already)
        }
    }
    fun infodig(i:Int, j:Int, p:Int):Pair<Int, Int> {
        while(!box.iscrushed(i,j)){
            box.dig(i,j,p)
        }
        return Pair(box.already[i][j], box.breakcost[i][j])
    }
    fun infodig2(fa:Int, fb:Int, a:Int, b:Int,p:Int):Int {
        infodig(fa,fb,p)
        var aa = fa
        var bb = fb
        var total = 0
        var count = 0
        while(aa != a || bb != b){
            if(aa > a){
                aa--
            }else if(aa < a){
                aa++
            }else if( bb> b) {
                bb--
            }else if(bb < b){
                bb ++
            }
            val (l,u) = infodig(aa,bb,p)
            count += 2
            total += l + u
        }
        return (total/count)
    }


    fun digtotalcap(i:Int, j:Int, limit:Int):Boolean{
        var p = base
        var total = 0
        while(!box.iscrushed(i,j)){
            if(total > limit) return false
            p = p.nextlevel()
            box.dig(i,j,p)
            total += p
        }
        return true
    }
    fun justconnecttotal(a:Int, b:Int, x:Int, y:Int, cap:Int):Boolean{
        var aa = x
        var bb = y
        val attempt = digtotalcap(aa,bb,cap)
        if(!attempt) return false
        while(aa != a || bb != b){
            if(aa > a){
                aa--
            }else if(aa < a){
                aa++
            }else if( bb> b) {
                bb--
            }else if(bb < b){
                bb ++
            }
            val attempt = digtotalcap(aa,bb,cap)
            if(!attempt){
                return false
            }
        }
        return true
    }
    fun justdigcap(i:Int, j:Int, limit:Int = (box.cost * 1.1).toInt() + 30):Boolean{
        var p = base
        while(!box.iscrushed(i,j)){
            p = p.nextlevel()
            if(p > limit) return false
            box.dig(i,j,p)
        }
        return true
    }
    fun just_connect_balanced(a:Int, b:Int, x:Int, y:Int, guess:Int = -10000){
        var aa = x
        var bb = y
        justdig(aa,bb,guess=guess)
        while(aa != a || bb != b){
            if(aa > a){
                aa--
                justdig(aa,bb,guess=guess)
            }else if(aa < a){
                aa++
                justdig(aa,bb,guess=guess)
            }
            if( bb> b) {
                bb--
                justdig(aa,bb,guess=guess)
            }else if(bb < b){
                bb ++
                justdig(aa,bb,guess=guess)
            }
        }
    }

    fun rating(i:Int, j:Int):Int {
        kotlin.assert(box.iscrushed(i, j))
        return (box.already[i][j] + box.breakcost[i][j])/2
    }

    fun just_connect_seeking(a:Int, b:Int, x:Int, y:Int, guess:Int = -10000){
        var aa = x
        var bb = y
        justdig(aa,bb,guess=guess)
        var last = rating(aa,bb)
        var gox = true
        while(aa != a && bb != b){
            if(gox){
                if(aa > a){
                    aa --
                }else{
                    aa++
                }
                justdig(aa,bb,guess=guess)
                val new = rating(aa,bb)
                if(new > last){
                    gox = !gox
                }
                last = new
            }else{
                if(bb > b){
                    bb--
                }else if(bb < b){
                    bb++
                }
                justdig(aa,bb,guess=guess)
                val new = rating(aa,bb)
                if(new > last){
                    gox = !gox
                }
                last = new
            }
        }
        while(aa != a || bb != b){
            if(aa > a){
                aa--
                justdig(aa,bb,guess=guess)
            }else if(aa < a){
                aa++
                justdig(aa,bb,guess=guess)
            }
            if( bb> b) {
                bb--
                justdig(aa,bb,guess=guess)
            }else if(bb < b){
                bb ++
                justdig(aa,bb,guess=guess)
            }
        }
    }
    fun justconnectonce(a:Int, b:Int, x:Int, y:Int, cap:Int):Boolean{
        var aa = x
        var bb = y
        val attempt = justdigcap(aa,bb,cap)
        if(!attempt) return false
        while(aa != a || bb != b){
            if(aa > a){
                aa--
            }else if(aa < a){
                aa++
            }else if( bb> b) {
                bb--
            }else if(bb < b){
                bb ++
            }
            val attempt = justdigcap(aa,bb,cap)
            if(!attempt){
                return false
            }
        }
        return true
    }

    fun cheatdig(i:Int, j:Int){
        box.dig(i,j,box.arr[i][j])
    }
    open fun go(){
        for(i in 0 until n){
            for(j in 0 until n){
                justdig(i,j)
            }
        }
    }
}
const val graphWeighed = true
class Graph(val n:Int, val m:Int, val directed:Boolean) {
    val maxedge = if (directed) m else m * 2

    var cnt = -1

    val edgecount:Int get() = cnt + 1

    val next = IntArray(maxedge)
    val head = IntArray(n) { -1 }
    val to = IntArray(maxedge)
    val from = IntArray(maxedge)
    val weights = IntArray(if (graphWeighed) m else 0)
    val Q = fastDeque(0,n)

    private fun primitive_add(u: Int, v: Int): Int {
        next[++cnt] = head[u]
        head[u] = cnt
        to[cnt] = v
        from[cnt] = u
        return cnt
    }

    fun add(u: Int, v: Int): Int {
        val e = primitive_add(u, v)
        if (!directed) {
            primitive_add(v, u)
        }
        return if (directed) e else e shr 1
    }

    fun addWeighted(u: Int, v: Int, w: Int):Int{
        val e = add(u, v)
        weights[e] = w
        return e
    }

    //Basic Transversals
    inline fun NS(a:Int, act:(Int)->Unit){
        var i= head[a]
        while(i != -1){
            act(to[i])
            i = next[i]
        }
    }
    inline fun NS_E(a:Int, act:(e:Int,v:Int)->Unit){
        var i= head[a]
        while(i != -1){
            act(i,to[i])
            i = next[i]
        }
    }


    inline fun everyEdge(act:(a:Int, b:Int)->Unit){
        val s = if(directed) 1 else 2
        for(e in 0 until edgecount step s ){
            act(from[e], to[e])
        }
    }

    // Store a spanning tree,
    var root = 0
    var preorder:IntArray = IntArray(0)
    var parent:IntArray = IntArray(0)
    val hasDFSorder:Boolean get() = preorder.size == n
    var parentEdge:IntArray = IntArray(0)



    //stores the order
    fun treeOrderDFS(withEdges:Boolean = false){
        parent = IntArray(n){-1}
        var pt = -1
        preorder = IntArray(n){-1}
        if(withEdges) parentEdge = IntArray(n){-1}

        Q.reset()
//        val Q = fastDeque(0,n)
        parent[root] = root
        Q.addLast(root)
        while(Q.isNotEmpty()){
            val a = Q.removeLast()
            val p = parent[a]
            preorder[++pt] = a
            NS_E(a){e,v ->
                if(v == p) return@NS_E
                if(withEdges) parentEdge[v] = if(directed) e else (e shr 1)
                parent[v] = a
                Q.addLast(v)
            }
        }
    }
    inline fun BFS(distRoot:Int, reached:(Int, Int)->Unit = {_,_ ->}): IntArray {
        Q.reset()
        val explored = IntArray(n+1){-1} // also store parents
        Q.addLast(distRoot)
        explored[distRoot] = -2
        val dist = IntArray(n){-1}
        dist[distRoot] = 0

        while(Q.size > 0){
            val x = Q.removeFirst()
            reached(x,explored[x])
            NS(x){ a->
                if(explored[a] == -1){
                    explored[a] = x
                    dist[a] = dist[x] + 1
                    Q.addLast(a)
                }
            }
        }
        return dist
    }
    inline fun trueOrderDFS(root:Int?,newroot:(r:Int) ->Unit, preexplore:(v:Int) ->Unit, postExplore:(v:Int)->Unit, move:(p:Int,v:Int,e:Int)->Unit, cross:(from:Int, to:Int,e:Int)->Unit ){
        Q.reset()
        val explored = BooleanArray(n){false}
        val headHere = head.copyOf()
        for(i in 0 until n) {
            if (explored[i] || (root != null && root != i)) continue
            newroot(i)
            explored[i] = true
            Q.addLast(i)
            while(Q.isNotEmpty()){
                val v = Q.last()
                val e = headHere[v]
                if(e == head[v]){
                    preexplore(v)
                }
                if(e == -1){
                    postExplore(v)
                    Q.removeLast()
                    continue
                }
                headHere[v] = next[e]
                val t = to[e]
                if(!explored[t]){
                    explored[t] = true
                    move(v,t,e)
                    Q.addLast(t)
                }else{
                    cross(v,t,e)
                }
            }
        }
    }

    fun EulerDoubleOrder(): Pair<IntArray, IntArray> {
        var pointer = -1
        val euler = IntArray(2 * n - 1)
        val entry = IntArray(n)
        fun dfs(v:Int, p:Int) {
            euler[++pointer] = v
            entry[v] = pointer
            NS(v){ w->
                if(w == p) return@NS
                dfs(w,v)
                euler[++pointer] = v
            }
        }
        dfs(root,-1)
        return Pair(euler, entry)
    }



    //Tree Transversals
    inline fun leafFirst(act:(Int)->Unit){
        if(!hasDFSorder) treeOrderDFS()
        for(i in preorder.lastIndex downTo 0){
            act(preorder[i])
        }
    }
    inline fun rootFirst(act:(Int)->Unit){
        if(!hasDFSorder) treeOrderDFS()
        for(a in preorder){
            act(a)
        }
    }
    inline fun rootFirstEdge(act:(from:Int, to:Int, e:Int)->Unit){
        if(!hasDFSorder) treeOrderDFS(true)
        for(i in 1 until preorder.size){
            val v = preorder[i]
            act(parent[v],v,parentEdge[v])
        }
    }

    // Basic invariants maintaining
    fun calculateParents():IntArray{
        if(!hasDFSorder) treeOrderDFS()
        return parent
    }
    fun calculateSizes():IntArray{
        val ret = IntArray(n){1}
        leafFirst { v -> if(v != root) ret[parent[v]] += ret[v] }
        return ret
    }
    fun calculateSubtreeSum(weights:IntArray){
        leafFirst { v -> if(v != root) weights[parent[v]] += weights[v] }
    }
    fun calculateDepth(): IntArray {
        val ret = IntArray(n)
        rootFirst { v -> if(v != root) ret[v] = ret[parent[v]] + 1  }
        return ret
    }
    inline fun subs(v:Int, act:(Int)->Unit){
        NS(v){w ->
            if(w != parent[v]) act(w)
        }
    }
    fun calculateDepthWeighted(): LongArray {
        val ret = LongArray(n)
        rootFirstEdge{from,to,e -> ret[to] = ret[from] + weights[e]}
        return ret
    }
    fun outdegree():IntArray{
        val ret = IntArray(n)
        everyEdge { a, b -> ret[a] ++  }
        return ret
    }
    fun indegree():IntArray{
        val ret = IntArray(n)
        everyEdge {a, b -> ret[b] ++}
        return ret
    }
    fun degree():IntArray = outdegree()
    fun reversed():Graph{
        assert(directed)
        val new = Graph(n,m,true)
        everyEdge { a, b -> new.add(b,a) }
        return new
    }
    fun intime():IntArray{
        val tin = IntArray(n)
        if(!hasDFSorder) treeOrderDFS()
        for(i in 0 until n) tin[preorder[i]] = i
        return tin
    }
    fun outtime():IntArray{
        val tout = intime()
        leafFirst { v ->
            val p = parent[v]
            if(p == v) return@leafFirst
            tout[p] = maxOf(tout[p], tout[v])
        }
        return tout
    }
    fun markcomponent():IntArray{
        val ret = IntArray(n)
        rootFirst { v ->
            if(v == root) return@rootFirst
            if(parent[v] == root){ ret[v] = v; return@rootFirst}
            ret[v] = ret[parent[v]]
        }
        return ret
    }


}

private typealias ei = Int
private typealias earr = IntArray
//NOT batle tested
class fastDeque(val l:Int, val r:Int) {
    val total = r - l + 1
    val st = earr(total)
    var startpointer = 0 - l
    var endpointer = startpointer - 1
    val size: Int get() = endpointer - startpointer + 1
    val lastIndex:Int get() = endpointer

    fun reset(){
        startpointer = -l
        endpointer = startpointer -1
    }

    fun isEmpty(): Boolean {
        return endpointer < startpointer
    }
    fun isNotEmpty():Boolean{
        return endpointer >= startpointer
    }
    fun first():ei{
        assert(isNotEmpty())
        return st[startpointer]
    }
    fun last():ei{
        assert(isNotEmpty())
        return st[endpointer]
    }
    fun addFirst(v:ei ){
        startpointer --
        st[startpointer]= v
    }
    fun addLast(v:ei){
        endpointer ++
        st[endpointer] = v
    }
    fun removeFirst(): ei {
        assert(isNotEmpty())
        val ret = st[startpointer]
        startpointer ++
        return ret
    }
    fun removeLast(): ei {
        assert(isNotEmpty())
        val ret = st[endpointer]
        endpointer --
        return ret
    }
    fun toArray():earr{
        return st.sliceArray(startpointer..endpointer)
    }
    fun toList():List<ei>{
        return st.slice(startpointer..endpointer)
    }

    inline fun forEach(act:(v:ei)->Unit){
        for(i in startpointer..endpointer){
            act(st[i])
        }
    }
    inline fun withIndex(act:(i:Int,v:ei)->Unit){
        for(i in startpointer..endpointer){
            act(i-startpointer,st[i])
        }
    }
    operator fun get(v:Int): ei {
        return st[startpointer + v]
    }
}
fun fastEmptyList(max:Int):fastDeque{
    return fastDeque(0,max-1)
}
var interactive = false
object Reader{
    private const val BS = 1 shl 16
    private const val NC = 0.toChar()
    private val buf = ByteArray(BS)
    private var bId = 0
    private var size = 0
    private var c = NC

    var warningActive = true
    var fakein = StringBuilder()

    private var IN:BufferedInputStream = BufferedInputStream(System.`in`, BS)
    val OUT:PrintWriter = PrintWriter(System.out)

    private val char: Char
        get() {
            if(interactive){
                return System.`in`.read().toChar()
            }
            while (bId == size) {
                size = IN.read(buf) // no need for checked exceptions
                if (size == -1) return NC
                bId = 0
            }
            return buf[bId++].toChar()
        }

    fun nextInt(): Int {
        var neg = false
        if (c == NC) c = char
        while (c < '0' || c > '9') {
            if (c == '-') neg = true
            c = char
        }
        var res = 0
        while (c in '0'..'9') {
            res = (res shl 3) + (res shl 1) + (c - '0')
            c = char
        }
        return if (neg) -res else res
    }
    fun nextLong(): Long {
        var neg = false
        if (c == NC) c = char
        while (c < '0' || c > '9') {
            if (c == '-') neg = true
            c = char
        }
        var res = 0L
        while (c in '0'..'9') {
            res = (res shl 3) + (res shl 1) + (c - '0')
            c = char
        }
        return if (neg) -res else res
    }
    fun nextString():String{
        val ret = StringBuilder()
        while (true){
            c = char
            if(!isWhitespace(c)){ break}
        }
        ret.append(c)
        while (true){
            c = char
            if(isWhitespace(c)){ break}
            ret.append(c)
        }
        return ret.toString()
    }
    fun isWhitespace(c:Char):Boolean{
        return c == ' ' || c == '\n' || c == '\r' || c == '\t'
    }
    fun rerouteInput(){
        if(warningActive){
            put("Custom test enabled")
            println("Custom test enabled")
            warningActive = false
        }
        val S = fakein.toString()
        println("New Case ")
        println(S.take(80))
        println("...")
        fakein.clear()
        IN = BufferedInputStream(S.byteInputStream(),BS)
    }
    fun flush(){
        OUT.flush()
    }
    fun takeFile(name:String){
        IN = BufferedInputStream(File(name).inputStream(),BS)
    }
}
fun eat(){ val st1 = TreeSet<Int>(); val st2 = TreeMap<Int,Int>()
}
fun put(aa:Any){
    Reader.OUT.println(aa)
    if(interactive){ Reader.flush()}
}
fun done(){ Reader.OUT.close() }
fun share(aa:Any){
    if(aa is IntArray){Reader.fakein.append(aa.joinToString(" "))}
    else if(aa is LongArray){Reader.fakein.append(aa.joinToString(" "))}
    else if(aa is List<*>){Reader.fakein.append(aa.joinToString(" "))}
    else{Reader.fakein.append(aa.toString())}
    Reader.fakein.append("\n")
}

val getintfast:Int get() = Reader.nextInt()
val getint:Int get(){ val ans = getlong ; if(ans > Int.MAX_VALUE) IntArray(1000000000); return ans.toInt() }
val getlong:Long get() = Reader.nextLong()
val getstr:String get() = Reader.nextString()
fun getline(n:Int):IntArray{
    return IntArray(n){getint}
}
fun getlineL(n:Int):LongArray{
    return LongArray(n){getlong}
}
var dmark = -1
infix fun Any.dei(a:Any){
    dmark++
    var str = "<${dmark}>   "
    debug()
    if(this is String){ str += this
    }else if(this is Int){ str += this.toString()
    }else if(this is Long){ str += this.toString()
    }else{ str += this.toString()}
    if(a is List<*>){ println("$str : ${a.joinToString(" ")}")
    }else if(a is IntArray){ println("$str : ${a.joinToString(" ")}")
    }else if(a is LongArray){ println("$str : ${a.joinToString(" ")}")
    }else if(a is BooleanArray){ println("$str :${a.map{if(it)'1' else '0'}.joinToString(" ")}")
    }else if(a is Array<*>){
        println("$str : ")
        for(c in a){if(c is IntArray){println(c.joinToString(" "))}
        else if(c is LongArray){println(c.joinToString(" "))}
        else if(c is BooleanArray){println(c.map { if(it) '1' else '0' }.joinToString(""))
        }
        }
        println()
    }else{ println("$str : $a")
    }
}
const val just = " "
fun crash(){
    throw Exception("Bad programme")}
fun assert(a:Boolean){
    if(!a){
        throw Exception("Failed Assertion")
    }}
enum class solveMode {
    real, rand, tc
}
object solve{
    var mode:solveMode = solveMode.real
    var tcNum:Int = 0
    var rand:()->Unit = {}
    var TC:MutableMap<Int,()->Unit> = mutableMapOf()
    var tn:Long = 0
    fun cases(onecase:()->Unit){
        val t = if(mode == solveMode.real){if(singleCase) 1 else getint} else if(mode == solveMode.tc){1 } else randCount
        if(pI != 998_244_353 && pI != 1_000_000_007){
            throw Exception("Not usual primes!")
        }
        if(t == 1 && mode != solveMode.real){
            tn = System.currentTimeMillis()
        }
        repeat(t){
            if(mode == solveMode.tc){
                TC[tcNum]?.let { it() }
                Reader.rerouteInput()
            }else if(mode == solveMode.rand){
                rand()
                Reader.rerouteInput()
            }
            onecase()
        }
        if(t == 1 && mode != solveMode.real){
            val dt = System.currentTimeMillis() - tn
            println("Time $dt ms ")
        }
    }
    fun rand(a:()->Unit){
        this.rand = a
    }
    fun tc(id:Int = 0,a:()->Unit){
        TC[id] = a
    }
    fun usetc(a:Int = 0 ){
        this.tcNum = a
        this.mode = solveMode.tc
    }
    fun userand(){
        this.mode = solveMode.rand
    }
}
fun debug(){}

const val singleCase = true


fun Graph.spfa(sources:List<Int>):Pair<IntArray, IntArray> {
    val dist = IntArray(n){plarge}
    val parent = IntArray(n){-1}
    val visited = BooleanArray(n)
    val Q = ArrayDeque<Int>()
    for(source in sources) {
        visited[source] = true
        parent[source] = -2
        dist[source] = 0
        Q.add(source)
    }
    while(Q.isNotEmpty()){
        val v = Q.removeFirst()
        visited[v] = false
        NS_E(v){
                e,u ->
            val cost = weights[e shr 1]
            if(dist[u] > dist[v]  + cost){
                dist[u] = dist[v] + cost
                parent[u] = v
                if(!visited[u]){
                    visited[u] = true
                    Q.addLast(u)
                }
            }
        }
    }
    return Pair(dist,parent)
}
fun Graph.genericspfa(dist:IntArray, density:IntArray):IntArray {
    val parent = IntArray(n){-1}
    val visited = BooleanArray(n)
    val Q = IntArray(n+1)
    var head = 0
    var last = 0 //exclusive
    for(i in 0 until n) {
        if(dist[i] == plarge) continue
        val source = i
        visited[source] = true
        parent[source] = -2
        Q[last] = source
        last ++
        if(last == n+1) last = 0

    }
    while(head != last){
        val v = Q[head]
        head ++
        if(head == n+1) head = 0
        visited[v] = false
        NS(v){
                u ->
            val cost = density[u]
            if(dist[u] > dist[v]  + cost){
                dist[u] = dist[v] + cost
                parent[u] = v
                if(!visited[u]){
                    visited[u] = true
                    if(head != last && dist[u] < dist[Q[head]]){
                        head --
                        if(head == -1) head = n
                        Q[head] = u
                    }else{
                        Q[last] = u
                        last ++
                        if(last == n+1) last = 0
                    }
                }
            }
        }
    }
    return parent
}

val basehardness = 780
val optimisticfacotr = 2.0
val untilfixated = 20
class regressor(){
    var sxx = 0.0
    var sxy = 0.0
    var syy = 0.0
    var sx1 = 0.0
    var sy1 = 0.0
    var s11 = 0.0

    var count = 0

    var uniquex = 0
    var thatx = 0.0
    var uniquey = 0
    var thaty = 0.0


    val bruh = mint


    val baseestimate = basehardness.toDouble() / optimisticfacotr
//	val baseestimate = Math.pow(basehardness.toDouble(), 1/3.0)

    var sxz = 0.0
    var syz = 0.0
    var sz = 0.0

    val pts = mutableListOf<DoubleArray>()
    fun addpoint(x:Double, y:Double, z:Double,mul:Double){
        pts.add(doubleArrayOf(x,y,z,mul))
        if(uniquex == 0){
            uniquex = 1
            thatx = x
        }else if(uniquex == 1 && x != thatx){
            uniquex = -1
        }
        if(uniquey == 0){
            uniquey = 1
            thaty = y
        }else if(uniquey == 1 && y != thaty){
            uniquey = -1
        }


//		val ff = (21 - x.absoluteValue - y.absoluteValue)

//		val mul = Math.pow(ff,2.5)

        sxx += x * x * mul
        sxy += x * y * mul
        syy += y * y * mul
        sx1 += x * mul
        sy1 += y * mul
        s11 += 1.0 * mul
        sxz += x * z * mul
        syz += y * z * mul
        sz += z * mul

        count ++
    }

    fun estimate():Double{
        return sz/s11

    }
    fun adjustedestimate():Double{
        val est = estimate()
        if(count >= untilfixated){
            return est
        }else{
            val r = count.toDouble()/ untilfixated
            return (r) * est + (1 - r ) * baseestimate
        }



    }
}


public inline fun Int.countTrailingZeroBits(): Int = Integer.numberOfTrailingZeros(this)
class linearregressor(){
    var total = 0.0
    var totalexpo = 0.0
    var count = 0.0

    fun addpoint(x:Double, y:Double, z:Double, mul:Double){
        total += z * mul
        totalexpo += ln(z) * mul
        count += mul
    }
    fun estimate(eratio:Double):Double{
        val e1 = exp(totalexpo /count)
        val e2 = (total/count)
        return eratio * e1 + (1 - eratio) * e2
    }
    fun setone(){
        totalexpo /= count
        total /= count
        count = 1.0
    }
}


val parmChoice = listOf<DoubleArray>(
    (40..99).map { it.toDouble() / 100.0 }.toDoubleArray(), //bound
    (10..80).map { it.toDouble() / 10.0 }.toDoubleArray(), //newinfo
    ((10..20) +(20..100 step 5)).map { it.toDouble() / 10.0 }.toDoubleArray(), //second ratio
//	doubleArrayOf(18.0),
    (8..24 step 2).map { it.toDouble()}.toDoubleArray(), //red
    ((5 until 20) + (20..200)).map { it.toDouble() /1.0 }.toDoubleArray(), // basepower
    ((5 until 20) + (20..200)).map { it.toDouble() /1.0 }.toDoubleArray(), //fail power
    (40..170).map { it.toDouble() / 100.0 }.toDoubleArray(), //digpower
    (40..120).map { it.toDouble() / 100.0 }.toDoubleArray(), //confirm ratio
    (0..60).map { it.toDouble() / 100.0 }.toDoubleArray(), //sunk ratio
    (0..20).map { it.toDouble() / 100.0 }.toDoubleArray(), // second dig
    (0..20).map { it.toDouble() / 100.0 }.toDoubleArray(), // third dig
    (0..10).map { it.toDouble() / 10.0 }.toDoubleArray(),//expo

    (-5..10).map { it.toDouble() / 10.0 }.toDoubleArray(), //confidence
    (-10..60).map { it.toDouble() / 10.0 }.toDoubleArray(),//anger

    (2..50 step 2).map { it.toDouble() / 100.0 }.toDoubleArray(), // unconfirmed ratio
    (0..50 step 2).map { it.toDouble() / 100.0 }.toDoubleArray(), // commit ratio

    (0..100 step 5).map { it.toDouble() }.toDoubleArray(),
    (80..100).map{it.toDouble() / 100.0}.toDoubleArray()
)
//val parm0 = doubleArrayOf(0.66,4.5,3.5,18.0,15.0,15.0,0.6,0.56,0.22)


// old seed 1 0.500,3.700,3.000,12.000,14.000,14.0,0.670,0.570,0.330
val cost_1_seed = doubleArrayOf(0.630,3.900,2.500,16.000,18.000,11.000,0.690,0.590,0.330,0.000,0.090,0.000,-0.300,2.100,0.100,0.160,0.000,0.9)
val cost_2_seed = doubleArrayOf(0.650,2.100,6.000,18.000,26.000,11.000,0.820,0.930,0.270,0.000,0.130,0.000,-0.500,4.000,0.080,0.440,0.000,0.920)
val cost_4_seed = doubleArrayOf(0.630,3.200,2.500,18.000,25.000,12.000,0.980,0.550,0.210,0.050,0.100,0.000,-0.500,2.000,0.160,0.380,0.000,0.810)
val cost_8_seed = doubleArrayOf(0.600,8.000,4.000,18.000,40.000,10.000,1.150,0.880,0.120,0.130,0.120,0.300,-0.500,2.900,0.260,0.320,5.000,0.840)
val cost_16_seed = doubleArrayOf(0.630,7.300,4.500,16.000,20.000,14.000,1.180,0.730,0.320,0.190,0.160,0.200,-0.500,3.100,0.100,0.500,10.000,0.860)
val cost_32_seed = doubleArrayOf(0.610,3.700,1.800,16.000,79.000,16.000,1.170,1.060,0.210,0.150,0.200,0.100,-0.400,1.300,0.500,0.320,0.000,0.980)
//val cost_32_seed = doubleArrayOf(0.630,3.900,1.800,16.000,94.000,16.000,1.090,1.130,0.060,0.200,0.170,0.000,-0.500,1.900,0.440,0.380,0.000,0.940)
//val cost_32_seed = doubleArrayOf(0.630,3.100,1.600,18.000,106.000,29.000,0.900,1.000,0.130,0.020,0.050,0.000,-0.500,2.300,0.220,0.400,0.000,0.940)

val cost_64_seed = doubleArrayOf(0.640,4.800,2.000,18.000,81.000,20.000,1.460,1.170,0.200,0.110,0.180,0.100,-0.300,2.200,0.260,0.320,0.000,0.910)
val cost_128_seed = doubleArrayOf(0.520,7.100,1.600,18.000,77.000,24.000,1.370,0.780,0.220,0.070,0.200,0.100,-0.100,3.600,0.320,0.500,0.000,0.86)


val red16_c1 = doubleArrayOf(0.660,4.600,3.500,16.000,18.000,18.0,0.620,0.600,0.260,0.0,0.0)


val params = intArrayOf(15,20,24,32,44,58,93,168)



fun makestrategy(x:interactor):surveyseek{
    val c = x.tc.cost
    return when(c){
        1 -> surveyseek(x, cost_1_seed)
        2 -> surveyseek(x, cost_2_seed)
        4 -> surveyseek(x, cost_4_seed)
        8 -> surveyseek(x, cost_8_seed)
        16 -> surveyseek(x, cost_16_seed)
        32 -> surveyseek(x, cost_32_seed)
        64 -> surveyseek(x, cost_64_seed)
        128 -> surveyseek(x, cost_128_seed)
        else -> surveyseek(x, cost_128_seed)
    }
}

class surveyseek(x:interactor,
                 val bound:Double,
                 val newinfo:Double,
                 val secondconfirmratio:Double,
                 val red:Int,
                 val surveypower:Int,
                 val digfail:Int,
                 val digpower:Double,
                 val confirmratio:Double, // opt 3: confirm ratio
                 val sunkratio:Double, // opt 4: better explorations

                 val seconddig:Double,
                 val thirddig:Double,
                 val takeexpo:Double,

                 val confidence:Double,
                 val anger:Double,

                 val unconfirmedimpact:Double,
                 val commitratio:Double,

                 val fixedGuess:Int,
                 val diagonalworth:Double
):strategy(x) {

    constructor(x:interactor,nums:DoubleArray):this(x,
        bound = nums[0],
        newinfo = nums[1],
        secondconfirmratio = nums[2],
        red = nums[3].roundToInt(),
        surveypower = nums[4].roundToInt(),
        digfail = nums[5].roundToInt(),
        digpower = nums[6],
        confirmratio = nums[7],
        sunkratio = nums[8],
        seconddig = nums[9],
        thirddig = nums[10],
        takeexpo = nums[11],

        confidence = nums[12],
        anger = nums[13],

        unconfirmedimpact = nums[14],
        commitratio = nums[15],

        fixedGuess = nums[16].roundToInt(),
        diagonalworth = nums[17]
    )
    {

    }

    val d2 = digpower + seconddig
    val d3 = d2 + thirddig



    // final ratio in affecting


    override val base:Int
        get() = -1


    fun digall(will:Array<BooleanArray>){
        for(i in 0 until n){
            for(j in 0 until n){
                if(will[i][j]){
                    box.dig(i,j,5000)
                }
            }
        }
    }

    override fun go() {
        val iskeypoint = Array(n){BooleanArray(n)}
        val offset = red / 4
        val a = 0
        val b = a + (red/2)
        for(i in 0 until n){
            for(j in 0 until n){
                if(i % red == a && j % red == a){
                    iskeypoint[i][j] = true
                }else if(i % red == b && j % red == b) {
                    iskeypoint[i][j] = true
                }
            }
        }
        val forward = mutableListOf<Pair<Int,Int>>()
        val virtualnode = mutableListOf<Boolean>()
        val reverse = mutableMapOf<Pair<Int,Int>,Int>()
        for(i in 0 until n){
            for(j in 0 until n){
                if(iskeypoint[i][j]){

                    forward.add(Pair(i,j))
                    reverse[Pair(i,j)] = forward.lastIndex
                    virtualnode.add(true)
                }
            }
        }
        val waterx = mutableListOf<pii>()
        val housex = mutableListOf<pii>()


        val delaydig = Array(n){BooleanArray(n)}

        for((a,b) in house){
            forward.add(Pair(a,b))
            reverse[Pair(a,b)] = forward.lastIndex
            virtualnode.add(false)
            iskeypoint[a][b] = true
//			val closest = forward.minBy { (it.first -a).absoluteValue + (it.second -b).absoluteValue }!!
//			housex.add(closest)
//			delaydig[closest.first][closest.second] = true
            delaydig[a][b] = true
//			justdig(closest.first, closest.second, surveypower, -10000)
//			justdig(a,b,surveypower,-10000)
//			just_connect_seeking(closest.first, closest.second, a,b)
        }
        for((a,b) in water){
            forward.add(Pair(a,b))
            iskeypoint[a][b] = true
            reverse[Pair(a,b)] = forward.lastIndex
            virtualnode.add(false)
//			val closest = forward.minBy { (it.first -a).absoluteValue + (it.second -b).absoluteValue }!!
//			waterx.add(closest)
//			delaydig[closest.first][closest.second] = true
            delaydig[a][b] = true
//			justdig(a,b,surveypower,-10000)
//			justdig(closest.first, closest.second, surveypower, -10000)
//			just_connect_seeking(closest.first, closest.second, a,b)
        }




        val rn = forward.size
        val info = IntArray(rn){surveypower}
        val sunkcost = IntArray(rn){0}
        val confirmed = BooleanArray(rn)

        fun mean(a:Int, b:Int):Int{
            return (a +b)/2
//			val x = (Math.pow(a.toDouble(),r) + Math.pow(b.toDouble(),r))
//			return Math.pow(x/2.0, 1/r)
        }

        fun estimate(v:Int):Int{
            if(confirmed[v]){
                return (info[v] * confirmratio).roundToInt()
            }else {
                return info[v]
            }
        }
        val hred = red /2
        fun cost(v:Int, w:Int):Int{
            val x1 = estimate(v)
            val x2 = estimate(w)
            val (a,b) = forward[v]
            val (c,d) = forward[w]
            val dx = (a-c).absoluteValue
            val dy = (b-d).absoluteValue
            val dist = dx + dy
            val diagonal = dx <= 2 * dy && dy <=2 * dx && virtualnode[v] && virtualnode[w]
            val dratio = if(diagonal) diagonalworth else 1.0
            val rmean = ((x1+x2) * dratio).roundToInt()
            return (rmean * dist /( 2*red))
        }
        val proposewater = BooleanArray(rn)
//		val havewater = BooleanArray(rn)
        for(w in water){
            proposewater[reverse[w]!!] = true
//			havewater[reverse[w]!!] = true
        }
        val G = Graph(forward.size, forward.size * 8, false)


        val dx = intArrayOf(red, -red, 0 ,0, hred, hred, -hred, -hred)
        val dy = intArrayOf(0,0,red ,-red, hred, -hred, hred,  -hred)
        for(i in 0 until n){
            for(j in 0 until n){
                if(!iskeypoint[i][j]) continue
                val here = reverse[Pair(i,j)]!!
                if(!virtualnode[here]) continue
                for(d in 0 until 8){
                    val ii = i + dx[d]
                    val jj = j + dy[d]
                    if(ii !in 0 until n || jj !in 0 until n) continue

                    val other = reverse[Pair(ii,jj)]!!
                    if(!virtualnode[other]) continue
                    val cost = cost(here,other)
//					val cost = (estimate(here) + estimate(other))/2
                    if(here > other){
                        G.addWeighted(here, other, cost)
                    }
                }
            }
        }
        for(i in 0 until n) {
            for(j in 0 until n) {
                if(!iskeypoint[i][j]) continue
                val v1 = reverse[Pair(i, j)]!!
                if(virtualnode[v1]) continue
                for(other in forward){
                    val (a,b) = other
                    val dist = (a- i).absoluteValue + (b - j).absoluteValue
                    if(dist > red) continue
                    val v2 = reverse[other]!!
                    val cost = cost(v1,v2)
                    G.addWeighted(v1,v2,cost)
                }
            }
        }
        fun updatenode(v:Int){
            G.NS_E(v) { e,u ->
                val sank = ((sunkcost[v] + sunkcost[u]) * sunkratio ).roundToInt()
                val costnow = cost(v,u)
                G.weights[e shr 1] = maxOf(0,costnow - sank)
            }
        }
        explore@while(house.any { !proposewater[reverse[it]!!] }){
            val sources = mutableListOf<Int>()
            for(x in house){
                if(proposewater[reverse[x]!!]) continue
                sources.add(reverse[x]!!)
            }
            val (dist,parent) = G.spfa(sources)
            var best = plarge
            var how = -727
            for(i in 0 until rn){
                if(proposewater[i]){
                    if(dist[i] < best){
                        best = dist[i]
                        how = i
                    }
                }
            }
            var now = how
            while(true){
                val next = parent[now]
                if(next == -2) break

                val power = maxOf(surveypower, (info[next] * commitratio).roundToInt())
                val (na,nb) = forward[next]
                val doit = box.dig(na,nb,power)
                if(doit){
                    confirmed[next] = true
                    proposewater[next] = true
                    now = next
                    updatenode(next)
                }else{
                    info[next] += power
                    sunkcost[next] += power
                    updatenode(next)
                    continue@explore
                }
            }
        }

        fun finalestimate(v:Int):Int {
            if(!confirmed[v]){
                return 50000
            }else{
                return info[v]
            }
        }

        fun finalisenode(v:Int){
            G.NS_E(v) { e,u ->
                val costnow = mean(finalestimate(v), finalestimate(u) )
                G.weights[e shr 1] = costnow
            }
        }
        for(i in 0 until rn){
            finalisenode(i)
        }
        val criticalpath = BooleanArray(rn)
        run{
            val critwater = BooleanArray(rn)
            for(p in water){
                critwater[reverse[p]!!] = true
                criticalpath[reverse[p]!!] = true
            }
            while(house.any{!critwater[reverse[it]!!]}){
                val sources = mutableListOf<Int>()
                for(x in house){
                    if(critwater[reverse[x]!!]) continue
                    sources.add(reverse[x]!!)
                }
                val (dist,parent) = G.spfa(sources)
                var best = plarge
                var how = -1
                for(i in 0 until rn){
                    if(critwater[i]){
                        if(dist[i] < best){
                            best = dist[i]
                            how = i
                        }
                    }
                }
                var now = how
                critwater[now] = true
                while(true){
                    val next = parent[now]
                    if(next == -2) break

                    critwater[next] = true
                    criticalpath[next] = true
                    now = next
                }
            }
        }



        val relevantpoint = Array(n){BooleanArray(n)}
        val estimator = Array(n){Array(n){ linearregressor() } }
        val havewater = Array(n){BooleanArray(n)}
        val crushed = Array(n){BooleanArray(n)}



        //add relevant points
        for(i in 0 until rn){
            if(!criticalpath[i]) continue
            val (a,b) = forward[i]
            for(dx in -hred..hred){
                for(dy in -hred..hred){
                    val aa = a + dx
                    val bb = b + dy
                    if(aa !in 0 until n || bb !in 0 until n) continue
                    relevantpoint[aa][bb] = true
                }
            }
        }
        for((a,b) in water){
            havewater[a][b] = true
            for(dx in -hred..hred){
                for(dy in -hred..hred){
                    val aa = a + dx
                    val bb = b + dy
                    if(aa !in 0 until n || bb !in 0 until n) continue
                    relevantpoint[aa][bb] = true
                }
            }
        }
        for((a,b) in house){
            for(dx in -hred..hred){
                for(dy in -hred..hred){
                    val aa = a + dx
                    val bb = b + dy
                    if(aa !in 0 until n || bb !in 0 until n) continue
                    relevantpoint[aa][bb] = true
                }
            }
        }
        kotlin.assert(water.any { relevantpoint[it.first][it.second] })

        val relevantbound = hred
        val influrence = relevantbound
        for(i in 0 until rn){
            val (a,b) = forward[i]

            val here = if(confirmed[i]) info[i] else (info[i] * secondconfirmratio).roundToInt()
            for(dx in -influrence..influrence) {
                for(dy in -influrence..influrence) {
                    val aa = a + dx
                    val bb = b + dy
                    if(aa !in 0 until n || bb !in 0 until n) continue
                    if(!relevantpoint[aa][bb]) continue

//					val fraction = maxOf((aa - a).absoluteValue, (bb - b).absoluteValue).toDouble() / (hred+1).toDouble()
                    val fraction = Math.pow(bound, ((aa-a).absoluteValue +  (bb-b).absoluteValue).toDouble())
                    val ratio = (fraction) * (if(confirmed[i]) 1.0 else unconfirmedimpact)
                    estimator[aa][bb].addpoint((aa - a).toDouble(), (bb - b).toDouble(), here.toDouble(),ratio)
                }
            }
        }

        for(i in 0 until n){
            for(j in 0 until n){
                if(relevantpoint[i][j]){
                    estimator[i][j].setone()
                }
            }
        }

        run{
            val forward = mutableListOf<Pair<Int,Int>>()
            val reverse = mutableMapOf<Pair<Int,Int>,Int>()

            val reverse2 = Array(n){IntArray(n)}

            for(i in 0 until n){
                for(j in 0 until n){
                    if(relevantpoint[i][j]){
                        forward.add(Pair(i,j))
                        reverse[Pair(i,j)] = forward.lastIndex
                        reverse2[i][j] = forward.lastIndex
                    }
                }
            }
            val rn = forward.size
            val G = Graph(forward.size, forward.size * 8, false)
            val dx = intArrayOf(0,0,1,-1)
            val dy = intArrayOf(1,-1,0,0)
            for(i in 0 until n){
                for(j in 0 until n){
                    if(!relevantpoint[i][j]) continue
                    for(d in 0 until 4){
                        val ii = i + dx[d]
                        val jj = j + dy[d]
                        if(ii !in 0 until n || jj !in 0 until n) continue
                        if(!relevantpoint[ii][jj]) continue
                        val here = reverse[Pair(i,j)]!!
                        val other = reverse[Pair(ii,jj)]!!
                        if(here > other)
                            G.add(here,other)
                    }
                }
            }
            val hardness = IntArray(rn)

            for(i in 0 until n){
                for(j in 0 until n){
                    if(!relevantpoint[i][j]) continue
                    if(box.iscrushed(i,j) || delaydig[i][j]){
                        crushed[i][j] = true
                        hardness[reverse[Pair(i,j)]!!] = 0
                    }else{
                        hardness[reverse[Pair(i,j)]!!] = maxOf(0.0,estimator[i][j].estimate(takeexpo) -box.already[i][j]).roundToInt()
                    }
                }
            }



            val watersources = mutableSetOf<Pair<Int,Int>>()
            val powseries = DoubleArray(100){Math.pow(bound,it.toDouble())}
            mine@while(house.any { !havewater[it.first][it.second]}){
                val dist = IntArray(rn){plarge}
                for(x in house){
                    if(havewater[x.first][x.second]) continue
                    dist[reverse[x]!!] = 0
                }
                val par = G.genericspfa(dist,hardness)

                var best = plarge
                var how = -728
                for(i in 0 until rn){
                    if(dist[i] < best && havewater[forward[i].first][forward[i].second]){
                        how = i
                        best = dist[i]
                    }
                }

                var now = how
                if(forward[now] in water){
                    watersources.add(forward[now])
                }
                while(true){
                    val next = par[now]
                    if(next == -2) break
                    val (nx,ny) = forward[next]

                    if(!box.iscrushed(nx,ny)){
                        val guess = estimator[nx][ny].estimate(takeexpo)
                        val attempts = intArrayOf(fixedGuess + (guess * digpower).roundToInt(), fixedGuess + (guess * d2).roundToInt(),fixedGuess + (guess * d3).roundToInt())
                        justmultidig(nx,ny,digfail,attempts,confidence,anger)
                        val new = rating(nx,ny)
                        crushed[nx][ny] = true
                        hardness[next] = 0
                        for(dx in -influrence..influrence){
                            for(dy in -influrence..influrence){
                                val ii = nx + dx
                                val jj = ny + dy
                                if(ii !in 0 until n || jj !in 0 until n || !relevantpoint[ii][jj]) continue
                                val bound = powseries[dx.absoluteValue + dy.absoluteValue]
//								val bound = Math.pow(bound,(dx.absoluteValue + dy.absoluteValue).toDouble())
                                estimator[ii][jj].addpoint((ii-nx).toDouble(), (jj-ny).toDouble(), new.toDouble(), bound * newinfo)
                                if(!crushed[ii][jj]){
                                    hardness[reverse2[ii][jj]] = maxOf(0, (estimator[ii][jj].estimate(takeexpo) - box.already[ii][jj]).roundToInt())
                                }
                            }
                        }
                        havewater[nx][ny] = true

                        continue@mine
                    }
                    havewater[nx][ny] = true
                    now = next
                }
            }
//			for(i in 0 until n){
//				for(j in 0 until n){
//					if(delaydig[i][j]){

            for((i,j) in watersources) {
                val guess = estimator[i][j].estimate(takeexpo)
                val attempts =
                    intArrayOf(fixedGuess + (guess * digpower).roundToInt(), fixedGuess + (guess * d2).roundToInt())
                justmultidig(i, j, digfail, attempts, confidence, anger)
            }
//					}
//				}
//			}

        }
    }
}

fun run(f:String,id:Int,output:Boolean,create:(interactor)->strategy){
    val tc = testcase.formtc(f,id)
    val int = interactor(tc)
    val x = create(int)
    x.go()

    if(!int.validate()){
        println("Test case failed")
        int.output()
        System.exit(0)
    }else{
        println("C = ${int.tc.cost} Objective ${int.total_energy}")
        if(output){
            int.output()
        }
    }

}
fun main(){
    solve.cases{
//		for(m in 0..99){
//			if(m % 10 == 0) println("TC $m")
//			val tcs= "/Users/arvin/IdeaProjects/ahc_excavate/src/testcases/in/${m.toString().padStart(4,'0')}.txt"
//			val tc = testcase.formtc(tcs, m)
//			analysis.check(tc)
//		}
//
//		just dei analysis.ret.average()

//        for(m in 0..9){
//            if(m % 10 == 0) println("TC $m")
//            val tc= "/Users/arvin/IdeaProjects/ahc_excavate/src/testcases/in/${m.toString().padStart(4,'0')}.txt"
//            run(tc,m,m == 6){ dynamicsurvey(it) }
//
//        }

        val tc = testcase.livetc()
        val int = interactor(tc)
        val x = makestrategy(int)
        x.go()

//		val tc = "/Users/arvin/IdeaProjects/ahc_excavate/src/testcases/in/xx.txt"
//		run(tc,true){harden(it)}



    }
    done()
}
