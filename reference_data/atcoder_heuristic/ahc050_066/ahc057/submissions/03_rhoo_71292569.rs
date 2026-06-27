#![allow(non_snake_case)]



// todo: 出てきた解を山登りで微調整


fn main(){
    get_time();

    let ps=get_input();
    let rat=[0.3,0.6,0.85,0.97,1.]; // todo
    
    let solve=|fix:&[bool]|->(i64,Vec<(usize,usize,usize)>){
        let mut gs=(0..ps.len()).map(|i|vec![ps[i]]).collect_vec();
        let mut ans=vec![];
        let mut sum=0;
        
        for phase in 0..5{
            let end=((T-1) as f64*rat[phase]).round() as usize;
            gs.sort_unstable_by_key(|g|g.len());
            let small=gs[0].len();

            #[derive(Clone,Copy,Default)]
            struct Item{
                cost:i64,
                time:usize,
                u:usize,
                v:usize,
            }

            let mut cost=vec![vec![Item{cost:i64::MAX,time:!0,u:!0,v:!0};gs.len()];gs.len()];
            for i in 0..gs.len(){
                for j in 0..i{
                    for &u in &gs[i]{
                        for &v in &gs[j]{
                            let (time,nc)=u.optimal_time(v,u.now.max(v.now),end);
                            if cost[i][j].cost>nc{
                                cost[i][j]=Item{cost:nc,time,u:u.id,v:v.id};
                                cost[j][i]=cost[i][j];
                            }
                        }
                    }
                }
            }

            let mut es=vec![];
            let mut ws=vec![];
            let mut max=i64::MIN;

            let mut add_edge=|u:usize,v:usize,w:i64|{
                es.push(u);
                es.push(v);
                ws.push(w);
                max=max.max(w);
            };

            for i in 0..gs.len(){
                for j in 0..i{
                    if phase==0 && (fix[i] || fix[j]){
                        continue;
                    }
                    if 1<=phase && phase<=3 && gs[i].len()==small{
                        assert!(gs[j].len()==small);
                        continue;
                    }
                    add_edge(i,j,cost[i][j].cost);
                }
            }

            for i in 0..ws.len(){
                ws[i]=max-ws[i]+1e7 as i64;
            }

            let mut g=WMMatching::new(es,ws,false);
            let (res,_)=g.matching();

            let mut mi=vec![!0;gs.len()];
            for &(u,v) in &res{
                mi[u]=v;
                mi[v]=u;
            }
            
            let mut ngs=vec![];
            for i in 0..gs.len(){
                if mi[i]!=!0 && i<mi[i]{
                    let Item{cost,time,u,v}=cost[i][mi[i]];
                    sum+=cost;
                    ans.push((time,u,v));

                    let g0=&gs[i];
                    let g1=&gs[mi[i]];

                    let nv=merge(g0[0].v,g0.len(),g1[0].v,g1.len());
                    let mut g=vec![];
                    for &(mut p) in chain!(g0,g1){
                        p.mov(time);
                        p.v=nv;
                        g.push(p);
                    }
                    ngs.push(g);
                } else if mi[i]==!0{
                    ngs.push(gs[i].clone());
                }
            }

            gs=ngs;
        }
        for i in 0..gs.len(){
            assert!(gs[i].len()==K);
        }

        (sum,ans)
    };

    let mut set=IndexSet::new(N);

    let mut fix=vec![false;N];
    // for i in 0..M*2{
    //     fix[i]=true;
    //     set.add(i);
    // }

    let mids = (0..N).map(|i| ps[i].p + ps[i].v * (T as f64 / 2.0)).collect_vec();
    
    for i in 0..4{
        for j in 0..5{
            let lx=L as f64*i as f64/4.;
            let rx=L as f64*(i+1) as f64/4.;
            let ly=L as f64*j as f64/5.;
            let ry=L as f64*(j+1) as f64/5.;
            let is=(0..N).filter(|&i|lx<=mids[i].x && mids[i].x<rx && ly<=mids[i].y && mids[i].y<ry).collect_vec();
            let c=V::new((lx+rx)*0.5,(ly+ry)*0.5);

            let res=*is.iter().min_by_key(|&i|mids[*i].costf(c).ord()).unwrap();
            fix[res]=true;
            set.add(res);
        }
    }

    let mut best=solve(&fix);
    let mut it=0;

    let nei=get_nei();
    while get_time()<1.83{
        let u=set.to_slice().choice();
        let v=nei[u].choice();
        if set.contains(v){
            continue;
        }
        it+=1;
        // let v=set.to_slice_co().choice();
        fix[u]=false;
        fix[v]=true;
        let new=solve(&fix);
        if best.0>new.0{
            best=new;
            set.rem(u);
            set.add(v);
        } else{
            fix[u]=true;
            fix[v]=false;
        }
    }

    eprintln!("iter = {it}");

    let score=((L*(N-M)) as f64/(best.0 as f64+1.)).log2();
    eprintln!("score = {:.1}",score*1e3);
    for &(t,u,v) in &best.1{
        println!("{} {} {}",t,u,v);
    }
}



static_data!{
    get_nei:Vec<Vec<usize>>|{
        let ps=get_input();
        let mid=(0..N).map(|i|ps[i].p+ps[i].v*(T/2) as f64).collect_vec();
        let mut is=(0..N).collect_vec();
        let mut nei=vec![];

        for i in 0..N{
            is.sort_unstable_by_key(|&j|{
                if i==j{
                    f64::INFINITY.ord()
                } else{
                    mid[i].costf(mid[j]).ord()
                }
            });
            nei.push(is[..20].to_vec()); // todo
        }

        nei
    }
}



use itertools::*;


const N:usize=300;
const T:usize=1000;
const M:usize=10;
const K:usize=30;
const L:usize=1e5 as usize;



#[derive(Clone,Copy)]
struct P{
    id:usize,
    now:usize,
    p:V,
    v:V,
}
impl P{
    fn mov(&mut self,time:usize){
        assert!(self.now<=time);
        self.p+=self.v*(time-self.now) as f64;
        self.now=time;
    }

    // [since,until]の間のいつつなげるのが一番いいか
    // 厳密ではない
    fn optimal_time(&self,np:P,since:usize,until:usize)->(usize,i64){
        assert!(self.now<=since);
        let (t,neg)=fibonacci_search(since as i64,until as i64+1,|t|{
            let p=self.p+self.v*(t-self.now as i64) as f64;
            let np=np.p+np.v*(t-np.now as i64) as f64;
            -p.costf(np)
        });
        (t as usize,(-neg).round() as i64)
    }
}


static_data!{
    get_input:Vec<P>|{
        proconio::input!{
            _n:usize,
            _t:usize,
            _m:usize,
            _k:usize,
            _l:usize,
            d:[[f64;4];N],
        }

        let mut ps=vec![];
        for i in 0..N{
            let p=P{
                id:i,
                now:0,
                p:V::new(d[i][0],d[i][1]),
                v:V::new(d[i][2],d[i][3]),
            };
            ps.push(p);
        }
        ps
    }
}



#[macro_export]
macro_rules! static_data{
    ($a:ident:$t:ty|$e:expr)=>{
        fn $a()->&'static $t{
            use std::sync::OnceLock;
            static D:OnceLock<$t>=OnceLock::new();
            D.get_or_init(||$e)
        }
    }
}



use std::ops::{Add, AddAssign, Div, DivAssign, Mul, MulAssign, Sub, SubAssign};

#[derive(Clone, Copy, Debug, PartialEq)]
struct V {
    x: f64,
    y: f64,
}

impl V {
    pub fn new(x: f64, y: f64) -> Self {
        Self { x, y }
    }

    pub fn costf(self, rhs: V) -> f64 {
        let lf = L as f64;
        
        let dx = (self.x - rhs.x).abs();
        let dx = dx.min(lf - dx);
        
        let dy = (self.y - rhs.y).abs();
        let dy = dy.min(lf - dy);

        (dx*dx+dy*dy).sqrt()
    }

    /// 問題文の定義に従い、2点間のトーラス距離のコストを計算
    /// D = round(sqrt(min(L - dx, dx)^2 + min(L - dy, dy)^2))
    pub fn cost(self, rhs: V) -> i64 {
        let lf = L as f64;
        
        let dx = (self.x - rhs.x).abs();
        let dx = dx.min(lf - dx);
        
        let dy = (self.y - rhs.y).abs();
        let dy = dy.min(lf - dy);

        (dx*dx+dy*dy).sqrt().round() as i64
    }
}

// --- 演算子のオーバーロード ---

// 加算: (a + b) % L
impl Add for V {
    type Output = Self;

    fn add(self, rhs: Self) -> Self {
        let lf = L as f64;
        Self {
            x: (self.x + rhs.x).rem_euclid(lf),
            y: (self.y + rhs.y).rem_euclid(lf),
        }
    }
}

impl AddAssign for V {
    fn add_assign(&mut self, rhs: Self) {
        *self = *self + rhs;
    }
}

// 減算: (a - b) % L
impl Sub for V {
    type Output = Self;

    fn sub(self, rhs: Self) -> Self {
        let lf = L as f64;
        Self {
            x: (self.x - rhs.x).rem_euclid(lf),
            y: (self.y - rhs.y).rem_euclid(lf),
        }
    }
}

impl SubAssign for V {
    fn sub_assign(&mut self, rhs: Self) {
        *self = *self - rhs;
    }
}

// スカラー乗算 (倍精度): mod L は適用しない（速度計算等で使うため）
impl Mul<f64> for V {
    type Output = Self;

    fn mul(self, rhs: f64) -> Self {
        Self {
            x: self.x * rhs,
            y: self.y * rhs,
        }
    }
}

impl MulAssign<f64> for V {
    fn mul_assign(&mut self, rhs: f64) {
        *self = *self * rhs;
    }
}

// スカラー除算 (倍精度)
impl Div<f64> for V {
    type Output = Self;

    fn div(self, rhs: f64) -> Self {
        Self {
            x: self.x / rhs,
            y: self.y / rhs,
        }
    }
}

impl DivAssign<f64> for V {
    fn div_assign(&mut self, rhs: f64) {
        *self = *self / rhs;
    }
}

/// 運動量保存則に従って新しい速度ベクトルを計算する関数
/// va: 成分Aの速度, a: 成分Aのサイズ
/// vb: 成分Bの速度, b: 成分Bのサイズ
fn merge(va: V, a: usize, vb: V, b: usize) -> V {
    // 注: 速度の計算にはトーラスの % L を適用してはいけないため、
    // オーバーロードした演算子を使わず、内部で直接計算します。
    let size_a = a as f64;
    let size_b = b as f64;
    let total = size_a + size_b;

    V {
        x: (va.x * size_a + vb.x * size_b) / total,
        y: (va.y * size_a + vb.y * size_b) / total,
    }
}



// a<=x<b のf(x)が最大となるものをnとしたとき (n,f(n))を返す
fn fibonacci_search<T:PartialOrd>(a:i64,b:i64,mut f:impl FnMut(i64)->T)->(i64,T){
    assert!(a<b);

    const FIB:[u64;92]=[0x1,0x2,0x3,0x5,0x8,0xd,0x15,0x22,0x37,0x59,0x90,0xe9,0x179,0x262,0x3db,0x63d,0xa18,0x1055,0x1a6d,0x2ac2,0x452f,0x6ff1,0xb520,0x12511,0x1da31,0x2ff42,0x4d973,0x7d8b5,0xcb228,0x148add,0x213d05,0x35c7e2,0x5704e7,0x8cccc9,0xe3d1b0,0x1709e79,0x2547029,0x3c50ea2,0x6197ecb,0x9de8d6d,0xff80c38,0x19d699a5,0x29cea5dd,0x43a53f82,0x6d73e55f,0xb11924e1,0x11e8d0a40,0x1cfa62f21,0x2ee333961,0x4bdd96882,0x7ac0ca1e3,0xc69e60a65,0x1415f2ac48,0x207fd8b6ad,0x3495cb62f5,0x5515a419a2,0x89ab6f7c97,0xdec1139639,0x1686c8312d0,0x2472d96a909,0x3af9a19bbd9,0x5f6c7b064e2,0x9a661ca20bb,0xf9d297a859d,0x19438b44a658,0x28e0b4bf2bf5,0x42244003d24d,0x6b04f4c2fe42,0xad2934c6d08f,0x1182e2989ced1,0x1c5575e509f60,0x2dd8587da6e31,0x4a2dce62b0d91,0x780626e057bc2,0xc233f54308953,0x13a3a1c2360515,0x1fc6e116668e68,0x336a82d89c937d,0x533163ef0321e5,0x869be6c79fb562,0xd9cd4ab6a2d747,0x16069317e428ca9,0x23a367c34e563f0,0x39a9fadb327f099,0x5d4d629e80d5489,0x96f75d79b354522,0xf444c01834299ab,0x18b3c1d91e77decd,0x27f80ddaa1ba7878,0x40abcfb3c0325745,0x68a3dd8e61eccfbd,0xa94fad42221f2702];
    let mut g=|n|if a<=n && n<b{Some(f(n))}else{None};
    
    let mut s=a-1;
    let k=FIB.partition_point(|&n|n<(b-a) as u64+2);

    let mut f0=g(s+FIB[k-2] as i64);
    let mut f1=g(s+FIB[k-1] as i64);

    for i in (3..=k).rev(){
        if f0<f1{
            s+=FIB[i-2] as i64;
            (f0,f1)=(f1,g(s+FIB[i-2] as i64));
        } else{
            (f0,f1)=(g(s+FIB[i-3] as i64),f0);
        }
    }

    if f0<f1{
        (s+2,f1.unwrap())
    } else{
        (s+1,f0.unwrap())
    }
}



#[allow(unused)]
mod rnd {
    static mut X2:u32=12345;
    static mut X3:u32=0xcafef00d;
    static mut C_X1:u64=0xd15ea5e5<<32|23456;

    pub fn next()->u32{
        unsafe{
            let x=X3 as u64*3487286589;
            let ret=(X3^X2)+(C_X1 as u32^(x>>32) as u32);
            X3=X2;
            X2=C_X1 as u32;
            C_X1=x+(C_X1>>32);
            ret
        }
    }

    pub fn next64()->u64{
        (next() as u64)<<32|next() as u64
    }

    pub fn nextf()->f64{
        f64::from_bits(0x3ff0000000000000|(next() as u64)<<20)-1.
    }

    pub fn get(n:usize)->usize{
        assert!(0<n && n<=u32::MAX as usize);
        next() as usize*n>>32
    }

    pub fn range(a:usize,b:usize)->usize{
        assert!(a<b);
        get(b-a)+a
    }

    pub fn range_skip(a:usize,b:usize,skip:usize)->usize{
        assert!(a<=skip && skip<b);
        let n=range(a,b-1);
        n+(skip<=n) as usize
    }

    pub fn rangei(a:i64,b:i64)->i64{
        assert!(a<b);
        get((b-a) as usize) as i64+a
    }

    pub fn shuffle<T>(a:&mut [T]){
        for i in (1..a.len()).rev(){
            a.swap(i,get(i+1));
        }
    }

    pub fn shuffle_iter<T:Copy>(a:&mut [T])->impl Iterator<Item=T>+'_{
        (0..a.len()).rev().map(|i|{
            a.swap(i,get(i+1));
            a[i]
        })
    }
}


trait RandomChoice{
    type Output;
    fn choice(&self)->Self::Output;
}
impl<T:Copy> RandomChoice for [T]{
    type Output=T;
    fn choice(&self)->T{
        self[rnd::get(self.len())]
    }
}



fn get_time()->f64{
    let time;

    #[cfg(feature="local")]
    unsafe{
        let mut ts=libc::timespec{tv_sec:0,tv_nsec:0};
        libc::clock_gettime(libc::CLOCK_PROCESS_CPUTIME_ID,&mut ts);
        time=ts.tv_sec as f64+ts.tv_nsec as f64*1e-9;
    }

    #[cfg(not(feature="local"))]{
        time=std::time::SystemTime::now().duration_since(std::time::UNIX_EPOCH).unwrap().as_secs_f64();
    }

    unsafe{
        static mut START:f64=-1.;
        if START==-1.{
            START=time;
        }

        #[cfg(feature="local")]{
            (time-START)*1.
        }
        #[cfg(not(feature="local"))]{
            time-START
        }
    }
}



#[derive(Clone,Copy,PartialOrd,Debug)]
struct OrderedFloat(f64);
impl PartialEq for OrderedFloat{
    fn eq(&self,a:&Self)->bool{
        assert!(!self.0.is_nan() && !a.0.is_nan());
        self.0==a.0
    }
}
impl Eq for OrderedFloat{}
impl Ord for OrderedFloat{
    fn cmp(&self,a:&Self)->std::cmp::Ordering{
        self.partial_cmp(a).unwrap()
    }
}
trait ToOrderedFloat{
    fn ord(self)->OrderedFloat;
}
impl ToOrderedFloat for f64{
    fn ord(self)->OrderedFloat{
        OrderedFloat(self)
    }
}



// 最大化: new-old>=add
fn anneal_iter(skip:usize,t0:f64,t1:f64,tl:f64)->impl Iterator<Item=f64>{
    assert!(0.<t1 && t1<t0);
    let t1=t1/t0;

    let start=get_time();
    let tl=tl-start;
    assert!(0.<tl);

    use std::sync::OnceLock;
    static LOGS:OnceLock<[f64;65536]>=OnceLock::new();
    let log=LOGS.get_or_init(||{
        let mut log=[0.;65536];
        for i in 0..65536{
            log[i]=((i as f64+0.5)/65536.).ln();
        }
        rnd::shuffle(&mut log);
        log
    });
    let mut iter=0;
    let mut heat=0.;

    assert!(skip<usize::BITS as usize);
    let mask=(1<<skip)-1;

    std::iter::from_fn(move||{
        if iter&mask==0{
            let time=(get_time()-start)/tl;
            if time>=1.{
                return None;
            }
            heat=t0*t1.powf(time);
        }
        iter+=1;
        Some(heat*log[iter&65535])
    })
}




// 重み付き一般マッチングは `https://judge.yosupo.jp/submission/53472` ここからパクってます．ありがと～

use std::iter::*;


#[derive(Debug)]
struct DuelDelta {
    tp: u8,
    delta: i64,
    extra: i64,
}

impl DuelDelta {
    fn new(tp: u8, delta: i64) -> Self {
        Self {
            tp,
            delta,
            extra: -1,
        }
    }
    #[inline]
    fn update(&mut self, d: i64, e: i64) {
        if self.delta > d {
            self.delta = d;
            self.extra = e;
        }
    }
}

struct BlossomChildsIterator<'a> {
    nvertex: usize,
    blossomchilds: &'a Vec<Vec<usize>>,
    s: Vec<(usize, usize)>,
}

impl<'a> BlossomChildsIterator<'a> {
    fn new(n: usize, blossomchilds: &'a Vec<Vec<usize>>, b: usize) -> Self {
        Self {
            nvertex: n,
            blossomchilds,
            s: vec![(b, 0)],
        }
    }
}

impl<'a> Iterator for BlossomChildsIterator<'a> {
    type Item = usize;
    fn next(&mut self) -> Option<Self::Item> {
        loop {
            if let Some((k, pos)) = self.s.pop() {
                if k < self.nvertex {
                    return Some(k);
                }
                let p = &self.blossomchilds[k];
                if pos >= p.len() {
                    continue;
                }
                self.s.push((k, pos + 1));
                self.s.push((p[pos], 0));
            } else {
                break None;
            }
        }
    }
}

struct WMMatching {
    endpoint: Vec<usize>,
    maxcardinality: bool,
    nvertex: usize,
    allocatedvertex: usize,
    neighbend: Vec<Vec<usize>>,
    mate: Vec<i64>,
    label: Vec<i8>,
    labelend: Vec<i64>,
    inblossom: Vec<usize>,
    blossomparent: Vec<isize>,
    blossomchilds: Vec<Vec<usize>>,
    blossombase: Vec<isize>,
    blossomendps: Vec<Vec<i64>>,
    bestedge: Vec<i64>,
    blossombestedges: Vec<Option<Vec<i64>>>,
    dualvar: Vec<i64>,
    allowedge: Vec<bool>,
    dw: Vec<i64>,
    queue: Vec<usize>,
    unusedblossoms: Vec<usize>,
}

fn forward(j: usize, l: usize) -> usize {
    if j == l {
        0
    } else {
        j + 1
    }
}

fn backward(j: usize, l: usize) -> usize {
    if j == 0 {
        l
    } else {
        j - 1
    }
}

impl WMMatching {
    fn new(endpoint: Vec<usize>, weights: Vec<i64>, maxcardinality: bool) -> Self {
        let n = 1 + *endpoint.iter().max().unwrap();
        let maxweight = *weights.iter().max().unwrap();
        let dw: Vec<_> = weights.iter().map(|v| *v * 2).collect();
        WMMatching {
            endpoint,
            maxcardinality,
            nvertex: n,
            allocatedvertex: n,
            neighbend: vec![Vec::new(); n],
            mate: vec![-1; n],
            label: vec![0i8; 2 * n],
            labelend: vec![-1; 2 * n],
            inblossom: (0..n).collect(),
            blossomparent: vec![-1; 2 * n],
            blossomchilds: vec![Vec::new(); 2 * n],
            blossombase: (0..n as isize).chain(repeat(-1).take(n)).collect(),
            blossomendps: vec![Vec::new(); 2 * n],
            bestedge: vec![-1; 2 * n],
            blossombestedges: vec![None; 2 * n],
            dualvar: repeat(maxweight).take(n).chain(repeat(0).take(n)).collect(),
            allowedge: vec![false; dw.len()],
            dw,
            queue: Vec::new(),
            unusedblossoms: Vec::new(),
        }
    }
    #[inline]
    fn slack(&self, k: usize) -> i64 {
        let kk = 2 * k;
        self.dualvar[self.endpoint[kk]] + self.dualvar[self.endpoint[kk + 1]] - self.dw[k]
    }

    fn blossom_leaves(&self, b: usize) -> BlossomChildsIterator {
        BlossomChildsIterator::new(self.nvertex, &self.blossomchilds, b)
    }

    #[inline]
    fn queue_push(&mut self, x: usize) {
        self.queue.push(x);
    }

    fn assign_label(&mut self, w: usize, t: i8, p: i64) {
        let b = self.inblossom[w];
        debug_assert!(self.label[w] == 0 && self.label[b] == 0);
        self.label[w] = t;
        self.label[b] = t;
        self.labelend[w] = p;
        self.labelend[b] = p;
        self.bestedge[w] = -1;
        self.bestedge[b] = -1;
        if t == 1 {
            for x in self.blossom_leaves(b).collect::<Vec<_>>() {
                self.queue_push(x);
            }
        } else {
            let mate_base = self.mate[self.blossombase[b] as usize];
            debug_assert!(mate_base >= 0);
            self.assign_label(self.endpoint[mate_base as usize], 1, mate_base ^ 1);
        }
    }
    fn scan(&mut self, v: isize, w: isize, path: &mut Vec<usize>) -> isize {
        if v == -1 && w == -1 {
            return -1;
        }
        let b = self.inblossom[v as usize] as usize;
        if self.label[b] == 3 {
            return self.blossombase[b] as isize;
        }
        debug_assert_eq!(self.label[b], 1);
        self.label[b] = 3;
        debug_assert_eq!(self.labelend[b], self.mate[self.blossombase[b] as usize]);
        let nv = if self.labelend[b] == -1 {
            -1
        } else {
            let v = self.endpoint[self.labelend[b] as usize];
            let b = self.inblossom[v as usize] as usize;
            debug_assert_eq!(self.label[b], 2);
            debug_assert!(self.labelend[b] >= 0);
            self.endpoint[self.labelend[b] as usize] as isize
        };
        path.push(b);
        if w != -1 {
            self.scan(w, nv, path)
        } else {
            self.scan(nv, w, path)
        }
    }
    fn scan_blossom(&mut self, v: isize, w: isize) -> isize {
        let mut p = Vec::new();
        let base = self.scan(v, w, &mut p);
        for x in p {
            self.label[x] = 1;
        }
        base
    }
    fn trace_back(
        &mut self,
        b: usize,
        bb: usize,
        mut v: usize,
        d: i64,
        path: &mut Vec<usize>,
        endps: &mut Vec<i64>,
    ) {
        loop {
            let bv = self.inblossom[v];
            if bv == bb {
                break;
            }
            self.blossomparent[bv] = b as isize;
            path.push(bv);
            endps.push(self.labelend[bv] ^ d);
            debug_assert!(
                self.label[bv] == 2
                    || self.label[bv] == 1 && self.labelend[bv] == self.mate[self.blossombase[bv] as usize]
            );
            v = self.endpoint[self.labelend[bv] as usize];
        }
    }
    fn add_blossom(&mut self, base: usize, l: usize) {
        let ll = 2 * l;
        let v = self.endpoint[ll];
        let w = self.endpoint[ll + 1];
        let bb = self.inblossom[base];
        let b = if let Some(ub) = self.unusedblossoms.pop() {
            ub
        } else {
            self.allocatedvertex
        };
        if self.allocatedvertex <= b {
            self.allocatedvertex = b + 1;
        }
        self.blossombase[b] = base as isize;
        self.blossomparent[b] = -1;
        self.blossomparent[bb] = b as isize;
        let mut path2 = Vec::new();
        let mut endps2 = Vec::new();
        self.trace_back(b, bb, w, 1, &mut path2, &mut endps2);
        let mut path1 = Vec::new();
        let mut endps1 = vec![ll as i64];
        self.trace_back(b, bb, v, 0, &mut path1, &mut endps1);
        path1.push(bb);
        path1.reverse();
        endps1.reverse();
        path1.append(&mut path2);
        endps1.append(&mut endps2);
        self.blossomchilds[b] = path1;
        self.blossomendps[b] = endps1;
        debug_assert_eq!(self.label[bb], 1);
        self.label[b] = 1;
        self.labelend[b] = self.labelend[bb];
        self.dualvar[b] = 0;
        for v in self.blossom_leaves(b).collect::<Vec<_>>() {
            if self.label[self.inblossom[v]] == 2 {
                self.queue.push(v);
            }
            self.inblossom[v] = b;
        }
        let mut bestedgeto = vec![-1isize; self.allocatedvertex];
        for &bv in &self.blossomchilds[b] {
            let nblist = if let Some(v) = self.blossombestedges[bv].take() {
                v
            } else {
                self
                    .blossom_leaves(bv)
                    .flat_map(|i| self.neighbend[i].iter().map(|p| (p >> 1) as i64))
                    .collect::<Vec<_>>()
            };
            for k32 in nblist {
                let k = k32 as usize;
                let kk = 2 * k;
                let j = self.endpoint[if self.inblossom[self.endpoint[kk + 1]] == b {
                    kk
                } else {
                    kk + 1
                }];
                let bj = self.inblossom[j];
                if bj != b && self.label[bj] == 1 {
                    let i = bestedgeto[bj];
                    if i == -1 || self.slack(k) < self.slack(i as usize) {
                        bestedgeto[bj] = k as isize;
                    }
                }
            }
            self.bestedge[bv] = -1;
        }
        let a = bestedgeto
            .into_iter()
            .filter_map(|i| if i != -1 { Some(i as i64) } else { None })
            .collect::<Vec<i64>>();
        let mut x = -1;
        let mut min_v = i64::MAX;
        for k in &a {
            let v = self.slack(*k as usize);
            if min_v > v {
                x = *k as i64;
                min_v = v;
            }
        }
        self.bestedge[b] = x;
        self.blossombestedges[b] = Some(a);
    }
    fn expand_blossom(&mut self, b: usize, endstage: bool) {
        for sl in 0..self.blossomchilds[b].len() {
            let s = self.blossomchilds[b][sl];
            self.blossomparent[s] = -1;
            if s < self.nvertex {
                self.inblossom[s] = s;
            } else if endstage && self.dualvar[s] == 0 {
                self.expand_blossom(s, endstage)
            } else {
                for i in self.blossom_leaves(s).collect::<Vec<_>>() {
                    self.inblossom[i] = s;
                }
            }
        }
        if !endstage && self.label[b] == 2 {
            debug_assert!(self.labelend[b] >= 0);
            let entrychild = self.inblossom[self.endpoint[self.labelend[b] as usize ^ 1]];
            let l1 = self.blossomchilds[b].len() - 1;
            let mut j = (0..=l1)
                .find(|i| self.blossomchilds[b][*i] == entrychild)
                .unwrap();
            let endptrick = (j & 1) ^ 1;
            let jstep = if endptrick == 0 { forward } else { backward };
            let mut p = self.labelend[b] as usize;
            while j != 0 {
                self.label[self.endpoint[p ^ 1]] = 0;
                let t = self.blossomendps[b][j - endptrick] as usize;
                self.label[self.endpoint[t ^ endptrick ^ 1]] = 0;
                self.assign_label(self.endpoint[p ^ 1], 2, p as i64);
                self.allowedge[t >> 1] = true;
                j = jstep(j, l1);
                p = self.blossomendps[b][j - endptrick] as usize ^ endptrick;
                self.allowedge[p >> 1] = false;
                j = jstep(j, l1);
            }
            let bv = self.blossomchilds[b][j];
            self.label[self.endpoint[p ^ 1]] = 2;
            self.label[bv] = 2;
            self.labelend[self.endpoint[p ^ 1]] = p as i64;
            self.labelend[bv] = p as i64;
            self.bestedge[bv] = -1;
            j = jstep(j, l1);
            while self.blossomchilds[b][j] != entrychild {
                let bv = self.blossomchilds[b][j];
                if self.label[bv] == 1 {
                    j = jstep(j, l1);
                    continue;
                }
                for v in self.blossom_leaves(bv) {
                    if self.label[v] != 0 {
                        debug_assert_eq!(self.label[v], 2);
                        debug_assert_eq!(self.inblossom[v], bv);
                        self.label[v] = 0;
                        self.label[self.endpoint[self.mate[self.blossombase[bv] as usize] as usize]] = 0;
                        self.assign_label(v, 2, self.labelend[v]);
                        break;
                    }
                }
                j = jstep(j, l1);
            }
        }
        self.label[b] = -1;
        self.labelend[b] = -1;
        self.blossomchilds[b] = Vec::new();
        self.blossomendps[b] = Vec::new();
        self.blossombase[b] = -1;
        self.blossombestedges[b] = None;
        self.bestedge[b] = -1;
        self.unusedblossoms.push(b);
        //TODO: allocatedvertex
    }
    fn augment_blossom(&mut self, b: usize, v: usize) {
        let mut t = v;
        while self.blossomparent[t] != b as isize {
            t = self.blossomparent[t] as usize;
        }
        if t >= self.nvertex {
            self.augment_blossom(t, v);
        }
        let l1 = self.blossomchilds[b].len() - 1;
        let i = (0..=l1).find(|j| self.blossomchilds[b][*j] == t).unwrap();
        let mut j = i;
        let endptrick = (j & 1) ^ 1;
        let jstep = if endptrick == 0 { forward } else { backward };
        while j != 0 {
            j = jstep(j, l1);
            t = self.blossomchilds[b][j];
            let p = self.blossomendps[b][j - endptrick] as usize ^ endptrick;
            if t >= self.nvertex {
                self.augment_blossom(t, self.endpoint[p]);
            }
            j = jstep(j, l1);
            t = self.blossomchilds[b][j];
            if t >= self.nvertex {
                self.augment_blossom(t, self.endpoint[p ^ 1]);
            }
            self.mate[self.endpoint[p]] = p as i64 ^ 1;
            self.mate[self.endpoint[p ^ 1]] = p as i64;
        }
        if i > 0 {
            self.blossomchilds[b].rotate_left(i);
            self.blossomendps[b].rotate_left(i);
        }
        self.blossombase[b] = self.blossombase[self.blossomchilds[b][0]];
        debug_assert_eq!(self.blossombase[b], v as isize);
    }
    fn augment_matching_f(&mut self, s: usize, p: usize) {
        let bs = self.inblossom[s];
        debug_assert_eq!(self.label[bs], 1);
        debug_assert_eq!(self.labelend[bs], self.mate[self.blossombase[bs] as usize]);
        if bs >= self.nvertex {
            self.augment_blossom(bs, s);
        }
        self.mate[s] = p as i64;
        if self.labelend[bs] != -1 {
            let t = self.endpoint[self.labelend[bs] as usize];
            let bt = self.inblossom[t];
            debug_assert_eq!(self.label[bt], 2);
            debug_assert!(self.labelend[bt] >= 0);
            let ns = self.endpoint[self.labelend[bt] as usize];
            let j = self.endpoint[self.labelend[bt] as usize ^ 1];
            debug_assert_eq!(self.blossombase[bt] as usize, t);
            if bt >= self.nvertex {
                self.augment_blossom(bt, j);
            }
            self.mate[j] = self.labelend[bt];
            self.augment_matching_f(ns, self.labelend[bt] as usize ^ 1);
        }
    }
    fn augment_matching(&mut self, k: usize) {
        let kk = 2 * k;
        let v = self.endpoint[kk];
        let w = self.endpoint[kk + 1];
        self.augment_matching_f(v, kk + 1);
        self.augment_matching_f(w, kk);
    }
    fn go(&mut self, v: usize, p: usize) -> bool {
        let k = p >> 1;
        let w = self.endpoint[p];
        if self.inblossom[v] == self.inblossom[w] {
            false
        } else {
            let mut kslack = 0;
            if !self.allowedge[k] {
                kslack = self.slack(k);
                if kslack <= 0 {
                    self.allowedge[k] = true;
                }
            }
            if self.allowedge[k] {
                if self.label[self.inblossom[w]] == 0 {
                    self.assign_label(w, 2, p as i64 ^ 1);
                    false
                } else if self.label[self.inblossom[w]] == 1 {
                    let base = self.scan_blossom(v as isize, w as isize);
                    if base >= 0 {
                        self.add_blossom(base as usize, k);
                        false
                    } else {
                        self.augment_matching(k);
                        true
                    }
                } else if self.label[w] == 0 {
                    debug_assert_eq!(self.label[self.inblossom[w]], 2);
                    self.label[w] = 2;
                    self.labelend[w] = p as i64 ^ 1;
                    false
                } else {
                    false
                }
            } else if self.label[self.inblossom[w]] == 1 {
                let b = self.inblossom[v];
                if self.bestedge[b] == -1 || kslack < self.slack(self.bestedge[b] as usize) {
                    self.bestedge[b] = k as i64;
                }
                false
            } else if self.label[w] == 0 {
                if self.bestedge[w] == -1 || kslack < self.slack(self.bestedge[w] as usize) {
                    self.bestedge[w] = k as i64;
                }
                false
            } else {
                false
            }
        }
    }
    fn substage(&mut self) -> bool {
        while let Some(v) = self.queue.pop() {
            debug_assert_eq!(self.label[self.inblossom[v]], 1);
            let l = self.neighbend[v].len();
            for i in 0..l {
                if self.go(v, self.neighbend[v][i]) {
                    return true;
                }
            }
        }
        false
    }
    fn update_dual(&mut self) -> bool {
        let mut dt0 = DuelDelta::new(0, i64::MAX);
        if !self.maxcardinality {
            dt0.update(*(&self.dualvar[0..self.nvertex]).iter().min().unwrap(), -1);
        }
        let mut dt1 = DuelDelta::new(1, dt0.delta);
        for v in 0..self.nvertex {
            if self.label[self.inblossom[v]] == 0 {
                let be = self.bestedge[v];
                if be != -1 {
                    dt1.update(self.slack(be as usize), be);
                }
            }
        }
        let mut dt2 = DuelDelta::new(2, dt1.delta);
        for b in 0..self.allocatedvertex {
            if self.blossomparent[b] == -1 && self.label[b] == 1 {
                let be = self.bestedge[b];
                if be != -1 {
                    dt2.update(self.slack(be as usize) >> 1, be);
                }
            }
        }

        let mut dt3 = DuelDelta::new(3, dt2.delta);
        for b in self.nvertex..self.allocatedvertex {
            if self.blossombase[b] >= 0 && self.blossomparent[b] == -1 && self.label[b] == 2 {
                dt3.update(self.dualvar[b], b as i64);
            }
        }
        if dt3.delta == i64::MAX {
            false
        } else {
            let delta = dt3.delta;
            let dt = vec![dt0, dt1, dt2, dt3]
                .into_iter()
                .find(|p| p.delta == delta)
                .unwrap();
            for v in 0..self.nvertex {
                match self.label[self.inblossom[v]] {
                    0 => (),
                    1 => self.dualvar[v] -= delta,
                    2 => self.dualvar[v] += delta,
                    _ => panic!("unknown label"),
                }
            }
            for b in self.nvertex..self.allocatedvertex {
                if self.blossombase[b] >= 0 && self.blossomparent[b] == -1 {
                    match self.label[b] {
                        0 => (),
                        1 => self.dualvar[b] += delta,
                        2 => self.dualvar[b] -= delta,
                        _ => panic!("unknown label"),
                    }
                }
            }
            match dt.tp {
                0 => false,
                1 => {
                    self.allowedge[dt.extra as usize] = true;
                    let kk = 2 * dt.extra as usize;
                    let ei = self.endpoint[kk];
                    let i = if self.label[self.inblossom[ei]] == 0 {
                        self.endpoint[kk + 1]
                    } else {
                        ei
                    };
                    debug_assert_eq!(self.label[self.inblossom[i]], 1);
                    self.queue_push(i);
                    true
                }
                2 => {
                    self.allowedge[dt.extra as usize] = true;
                    let i = self.endpoint[2 * dt.extra as usize];
                    debug_assert_eq!(self.label[self.inblossom[i]], 1);
                    self.queue_push(i);
                    true
                }
                3 => {
                    self.expand_blossom(dt.extra as usize, false);
                    true
                }
                _ => panic!("unknown dt.tp"),
            }
        }
    }
    fn end_of_stage(&mut self) {
        for b in self.nvertex..self.allocatedvertex {
            if self.blossomparent[b] == -1
                && self.blossombase[b] >= 0
                && self.label[b] == 1
                && self.dualvar[b] == 0
            {
                self.expand_blossom(b, true);
            }
        }
    }
    fn stage(&mut self) -> bool {
        loop {
            if self.substage() {
                break true;
            }
            if !self.update_dual() {
                break false;
            }
        }
    }
    fn main_loop(&mut self) {
        for _sid in 0..self.nvertex {
            for p in &mut self.label[0..self.allocatedvertex] {
                *p = 0;
            }
            for p in &mut self.bestedge[0..self.allocatedvertex] {
                *p = -1;
            }
            for p in &mut self.blossombestedges[self.nvertex..self.allocatedvertex] {
                *p = None;
            }
            for p in &mut self.allowedge {
                *p = false;
            }
            self.queue = Vec::new();
            for v in 0..self.nvertex {
                if self.mate[v] == -1 && self.label[self.inblossom[v]] == 0 {
                    self.assign_label(v, 1, -1);
                }
            }
            if !self.stage() {
                break;
            }
            self.end_of_stage();
        }
    }
    fn matching(&mut self) -> (Vec<(usize, usize)>, i64) {
        for (i, v) in self.endpoint.iter().enumerate() {
            self.neighbend[*v].push(i ^ 1);
        }
        self.main_loop();
        let mut l = Vec::new();
        let mut cost = 0;
        for v in (0..self.nvertex - 1).rev() {
            let k = self.mate[v];
            if k >= 0 {
                let e = self.endpoint[k as usize];
                if v < e {
                    l.push((v, e));
                    let k = self.neighbend[v]
                        .iter()
                        .find(|j| self.endpoint[**j as usize] == e)
                        .unwrap();
                    cost += self.dw[k >> 1] >> 1;
                }
            }
        }
        (l, cost)
    }
}



#[derive(Clone)]
struct IndexSet{
    pos:Vec<usize>,
    que:Vec<usize>,
    len:usize,
}
#[allow(unused)]
impl IndexSet{
    fn new(n:usize)->Self{
        Self{
            pos:(0..n).collect(),
            que:(0..n).collect(),
            len:0,
        }
    }

    fn len(&self)->usize{
        self.len
    }

    fn add(&mut self,a:usize){
        let p=self.pos[a];
        assert!(self.len<=p);
        self.que.swap(p,self.len);
        self.pos.swap(a,self.que[p]);
        self.len+=1;
    }

    fn rem(&mut self,a:usize){
        let p=self.pos[a];
        assert!(p<self.len);
        self.len-=1;
        self.que.swap(p,self.len);
        self.pos.swap(a,self.que[p]);
    }

    fn to_slice(&self)->&[usize]{
        &self.que[..self.len]
    }

    fn to_slice_co(&self)->&[usize]{
        &self.que[self.len..]
    }

    fn choice(&self)->usize{
        self.que[rnd::get(self.len)]
    }

    fn choice_co(&self)->usize{
        self.que[rnd::range(self.len,self.que.len())]
    }

    fn contains(&self,v:usize)->bool{
        self.pos[v]<self.len
    }

    fn clear(&mut self){
        self.len=0;
    }

    fn fill(&mut self){
        self.len=self.pos.len();
    }
}
