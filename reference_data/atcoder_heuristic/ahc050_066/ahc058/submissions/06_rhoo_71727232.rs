#![allow(non_snake_case)]



fn main(){
    get_time();
    let input=get_input();
    let A=&input.A;
    let C=&input.C;

    let mut dp=vec![vec![];T];
    dp[0].push(Node::new());
    let mut track=Track::new();

    let mut best=(0,!0); // score,id
    let mut act=vec![];
    for i in 0..N{
        for j in 0..L{
            act.push((i,j));
        }
    }

    let zob=get_zob();
    let mut enumerate=|mut node:Node,dp:&mut Vec<Vec<Node>>|{
        let get_cost=|i:usize,j:usize|C[i][j]*(node.P[i][j] as usize+1);
        act.sort_unstable_by_key(|&(i,j)|get_cost(i,j));
        let mut i=0;
        let mut cur=node.tot;
        
        for turn in node.turn..T{
            while turn+1<T && i<act.len() && get_cost(act[i].0,act[i].1)<=cur{
                let (ni,nl)=act[i];
                i+=1;
                if 0<nl && node.P[ni][nl-1]==0{
                    continue;
                }

                let mut cur=cur-get_cost(ni,nl);
                let mut P=node.P;
                P[ni][nl]+=1;
                let mut B=node.B;
                
                for l in 0..L{
                    for i in 0..N{
                        if l==0{
                            cur+=A[i]*B[i][l]*P[i][l] as usize;
                        } else{
                            B[i][l-1]+=B[i][l]*P[i][l] as usize;
                        }
                    }
                }
                
                let id=track.push(node.id,(turn,(ni,nl)));
                let turn=turn+1;
                let hash=node.hash+zob[ni][nl];

                let mut F=[[0;L];N];
                let mut E=[0;L];
                let mut exp=cur;
                let rem=T-turn;

                for i in 0..N{
                    let mut gain=A[i];
                    let mut pow=rem;
                    
                    for l in 0..L{
                        gain*=P[i][l] as usize;
                        F[i][l]=gain*B[i][l];
                        E[l]+=F[i][l];
                        exp+=F[i][l]*pow/[1,2,6,24][l];
                        pow*=rem;
                    }
                }

                dp[turn].push(Node{id,turn,hash,ok:false,tot:cur,exp,F,B,P,E});
            }

            for l in 0..L{
                for i in 0..N{
                    if l==0{
                        cur+=A[i]*node.B[i][l]*node.P[i][l] as usize;
                    } else{
                        node.B[i][l-1]+=node.B[i][l]*node.P[i][l] as usize;
                    }
                }
            }
        }

        if best.0<cur{
            best=(cur,node.id);
        }
    };

    let mut is=vec![];
    let mut seen=rustc_hash::FxHashMap::default();
    let mut cur=vec![];

    for i in 0..T{
        is.clear();
        is.extend(0..dp[i].len());
        is.sort_unstable_by_key(|&j|!dp[i][j].exp);
        is.retain(|&j|{
            let hash=dp[i][j].hash;
            let score=dp[i][j].exp;
            let e=seen.entry(hash).or_insert(!0);
            if (*e as i64)<score as i64{
                *e=score;
                true
            } else{
                false
            }
        });

        let W=5;
        let mut cnt=0;
        for &j in &is{
            if dp[i][j].ok{
                continue;
            }
            dp[i][j].ok=true;
            cnt+=1;
            if cnt>=W{
                break;
            }
        }

        let W=3;
        for a in 0..N{
            for b in 0..L{
                is.sort_unstable_by_key(|&j|(!dp[i][j].F[a][b]/*,!dp[i][j].exp*/));
                let mut cnt=0;
                for &j in &is{
                    if dp[i][j].ok{
                        continue;
                    }
                    if dp[i][j].F[a][b]==0{
                        break;
                    }
                    dp[i][j].ok=true;
                    cnt+=1;
                    if cnt>=W{
                        break;
                    }
                }
            }
        }

        for b in 0..L{
            is.sort_unstable_by_key(|&j|(!dp[i][j].E[b]/*,!dp[i][j].exp*/));
            let mut cnt=0;
            for &j in &is{
                if dp[i][j].ok{
                    continue;
                }
                if dp[i][j].E[b]==0{
                    break;
                }
                dp[i][j].ok=true;
                cnt+=1;
                if cnt>=W{
                    break;
                }
            }
        }

        std::mem::swap(&mut cur,&mut dp[i]);
        for node in cur.drain(..){
            if node.ok{
                enumerate(node,&mut dp);
            }
        }
    }
    eprintln!("raw = {}",best.0);
    eprintln!("score = {:.1}",(best.0 as f64).log2());
    let res=track.restore(best.1);

    let mut ans=vec![(!0,!0);T];
    let mut all_actions = vec![];
    let last=res.last().unwrap().1;
    for &(t,(i,j)) in &res{
        all_actions.push((i, j));
        ans[t]=(i,j);
    }

    all_actions.sort();
    all_actions.dedup();

    let calc = |ops: &[(usize, usize)]| -> usize {
        let mut cur = 1;
        let mut P = [[0; L]; N];
        let mut B = [[1; L]; N];

        for &(i, j) in ops {
            if i != !0 {
                let cost = C[i][j] * (P[i][j] + 1);
                if cur < cost {
                    return 0;
                }
                cur -= cost;
                P[i][j] += 1;
            }

            for l in 0..L {
                for k in 0..N {
                    if l == 0 {
                        cur += A[k] * B[k][l] * P[k][l];
                    } else {
                        B[k][l - 1] += B[k][l] * P[k][l];
                    }
                }
            }
        }
        cur
    };

    // 3. Main Loop
    let mut current_score = calc(&ans);
    let mut iter = 0;

    // for add in anneal_iter(0,1e-3,1e-6,1.95){
    while get_time()<1.95{
        iter+=1;
        // Backup current state to revert if mutation fails
        let old_ans = ans.clone();
        
        // Select mutation type: 0=Change, 1=Insert, 2=Delete
        let type_ = rnd::get(3);
        let t = rnd::get(T);

        match type_ {
            0 => { 
                if rnd::get(2) == 0 {
                    let (ni, nl) = *all_actions.choice();
                    ans[t] = (ni, nl);
                } else {
                    ans[t] = (!0, !0);
                }
            },
            1 => { 
                let new_act = if rnd::get(2) == 0 { *all_actions.choice() } else { (!0, !0) };
                ans.insert(t, new_act);
                ans.pop();
            },
            2 => { 
                ans.remove(t);
                ans.push((!0, !0));
            },
            _ => unreachable!(),
        }

        // Evaluate
        let new_score = calc(&ans);

        // Greedy Step: Only accept if strict improvement
        // (Note: To escape local optima, Simulated Annealing could be used, 
        // but prompt asks for simple Hill Climbing/Greedy)
        if (new_score as f64).log2()-(current_score as f64).log2() >=0. {
            current_score = new_score;
        } else {
            // Revert
            ans = old_ans;
        }
    }

    eprintln!("HC Iterations: {}", iter);
    eprintln!("Final Improved Score: {:.1}", (current_score as f64).log2());

    for i in 0..T{
        if ans[i]==(!0,!0){
            println!("-1");
        } else{
            println!("{} {}",ans[i].1,ans[i].0);
        }
    }
}




#[derive(Clone,Copy)]
struct Node{
    id:usize,
    turn:usize,
    hash:u64,
    ok:bool,
    tot:usize,
    exp:usize,
    F:[[usize;L];N],
    B:[[usize;L];N],
    P:[[u16;L];N],
    E:[usize;L],
}
impl Node{
    fn new()->Node{
        Node{
            id:!0,
            turn:0,
            hash:0,
            ok:false,
            tot:1,
            exp:1,
            F:[[0;L];N],
            B:[[1;L];N],
            P:[[0;L];N],
            E:[0;L],
        }
    }
}



static_data!{
    get_zob:[[u64;L];N]|{
        let mut zob=[[!0;L];N];
        for i in 0..N{
            for j in 0..L{
                zob[i][j]=rnd::next64();
            }
        }
        zob
    }
}



const N:usize=10;
const L:usize=4;
const T:usize=500;



struct Input{
    A:[usize;N],
    C:[[usize;L];N],
}


static_data!{
    get_input:Input|{
        proconio::input!{
            _n:usize,
            _l:usize,
            _t:usize,
            _k:usize,
            a:[usize;N],
            c:[[usize;N];L],
        }

        let mut A=[!0;N];
        A.copy_from_slice(&a);
        let mut C=[[!0;L];N];
        for i in 0..L{
            for j in 0..N{
                C[j][i]=c[i][j];
            }
        }

        Input{A,C}
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



struct Track<T:Clone>(Vec<(usize,T)>);
impl<T:Clone> Track<T>{
    fn new()->Self{
        Track(vec![])
    }

    fn push(&mut self,prev:usize,v:T)->usize{
        self.0.push((prev,v));
        self.0.len()-1
    }

    fn restore(&self,mut idx:usize)->Vec<T>{
        let mut ret=vec![];
        while idx!=!0{
            let (prev,v)=self.0[idx].clone();
            ret.push(v);
            idx=prev;
        }
        ret.reverse();
        ret
    }
}



#[allow(unused)]
mod rnd{
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

    pub fn shuffle_iter<T:Copy>(a:&mut [T])->impl Iterator<Item=T>{
        (0..a.len()).rev().map(|i|{
            a.swap(i,get(i+1));
            a[i]
        })
    }
}


#[allow(unused)]
trait RandomChoice{
    type Output;
    fn choice(&self)->&Self::Output;
}
impl<T> RandomChoice for [T]{
    type Output=T;
    fn choice(&self)->&T{
        &self[rnd::get(self.len())]
    }
}



#[cfg(feature="local")]
#[allow(unused)]
fn get_time()->f64{
    static START:std::sync::OnceLock<f64>=std::sync::OnceLock::new();
    let now=||{
        let mut ts=libc::timespec{tv_sec:0,tv_nsec:0};
        unsafe{libc::clock_gettime(libc::CLOCK_PROCESS_CPUTIME_ID,&mut ts);}
        ts.tv_sec as f64+ts.tv_nsec as f64*1e-9
    };
    let start=*START.get_or_init(now);
    (now()-start)*1.
}

#[cfg(not(feature="local"))]
fn get_time()->f64{
    static START:std::sync::OnceLock<std::time::Instant>=std::sync::OnceLock::new();
    let now=||std::time::Instant::now();
    let start=*START.get_or_init(now);
    now().duration_since(start).as_secs_f64()
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
