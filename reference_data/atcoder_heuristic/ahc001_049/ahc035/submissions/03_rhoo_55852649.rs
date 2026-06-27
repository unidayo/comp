#![allow(non_snake_case)]



fn main(){
    proconio::input_interactive!{
        _n:usize,
        _m:usize,
        _t:usize,
    }

    let mut is=(0..N2).collect_vec();
    let deg=get_ps();

    for (_turn,tl) in multi_start_iter(T,1.98).enumerate(){
        let a=read_input();
        let mut max=[0;M];
        for i in 0..N2{
            for j in 0..M{
                max[j]=max[j].max(a[i][j]);
            }
        }

        let mut score=[1.;N2];
        for i in 0..N2{
            for j in 0..M{
                score[i]*=(max[j]-a[i][j]) as f64+0.5; // todo
            }
        }
        is.sort_unstable_by_key(|&i|O(score[i]));
        
        // T-1-_turnT-1 -> 0
        
        let mut cnt=[0;M];
        let mut protect=[0;N2];
        for &i in &is{
            for j in 0..M{
                if max[j]==a[i][j] && cnt[j]<(T-1-_turn).min(3){ // todo
                    protect[i]=protect[i].max(100-cnt[j]);
                    cnt[j]+=1;
                }
            }
        }
        
        if _turn!=T-1{
            is.sort_by_key(|&i|!protect[i]);
        } else{
            // eprintln!("SKIP");
        }

        let ps0=is[..16].iter().copied().collect_vec();
        let ps1=is[16..32].iter().copied().collect_vec();
        let ps2=is[32..36].iter().copied().collect_vec();
        let mut ps=[ps0,ps1,ps2];

        anneal(&a,&mut ps,tl);

        let mut ans=[[!0;N];N];
        for i in 0..3{
            for (&i,&p) in izip!(&ps[i],&deg[i]){
                ans[p.0][p.1]=i;
            }
        }
        
        for i in 0..N{
            for j in 0..N{
                print!("{} ",ans[i][j]);
            }
            println!();
        }
        println!();
    }

    // unsafe{
    //     eprintln!("iter = {}",ITER);
    //     eprintln!("valid = {}",VALID);
    // }
}


static mut ITER:usize=0;
static mut VALID:usize=0;


fn anneal(a:&[[i64;M];N2],is:&mut [Vec<usize>;3],tl:f64){
    let pos=get_ps();
    let get=|i:usize|(pos[i].len()*pos[i].len()) as f64;
    let sum=get(0)+get(1)+get(2);
    let prob=[get(0)/sum,(get(0)+get(1))/sum,1.];

    let mut ans=[[0;N];N];
    for i in 0..3{
        for j in 0..pos[i].len(){
            ans[pos[i][j].0][pos[i][j].1]=is[i][j];
        }
    }
    
    let log=logs();
    let start=get_time();
    let tl=tl-start;
    assert!(0.<tl);

    let mut valid=0;
    let mut iter=0;
    let mut heat=0.;

    let dot=|i:usize,j:usize|->i64{
        (0..M).map(|t|a[i][t]*a[j][t]).sum::<i64>()
    };

    let mut dots=[[0;N2];N2];
    for i in 0..N2{
        for j in 0..N2{
            dots[i][j]=dot(i,j);
        }
    }
    
    let dot_around=|ans:&[[usize;N];N],(i,j):(usize,usize),skip:(usize,usize)|->i64{
        let mut sum=0;
        for (di,dj) in DIJ{
            let ni=i+di;
            let nj=j+dj;
            if in_range(ni,nj) && (ni,nj)!=skip{
                sum+=dots[ans[i][j]][ans[ni][nj]];
            }
        }
        sum
    };

    while iter&255!=0 || {
        let time=(get_time()-start)/tl;
        heat=1e3*(1.-time); // todo
        time<=1.
    }{
        iter+=1;
        let add=-heat*log[iter&65535]; // 最小化
        // let add=0.;

        let r=rnd::nextf();
        let t=if r<=prob[0]{
            0
        } else if r<=prob[1]{
            1
        } else{
            2
        };

        let i0=rnd::range(0,pos[t].len());
        let i1=rnd::range_skip(0,pos[t].len(),i0);
        let p0=pos[t][i0];
        let p1=pos[t][i1];

        let old=dot_around(&ans,p0,(!0,!0))+dot_around(&ans,p1,p0);

        swap!(ans[p0.0][p0.1],ans[p1.0][p1.1]);
        let new=dot_around(&ans,p0,(!0,!0))+dot_around(&ans,p1,p0);

        if (new-old) as f64<=add{
            valid+=1;
        } else{
            swap!(ans[p0.0][p0.1],ans[p1.0][p1.1]);
        }
    }

    for i in 0..3{
        for j in 0..pos[i].len(){
            is[i][j]=ans[pos[i][j].0][pos[i][j].1];
        }
    }

    unsafe{
        ITER+=iter;
        VALID+=valid;
    }
}



fn in_range(i:usize,j:usize)->bool{
    i<N && j<N
}


fn get_ps()->Vec<Vec<(usize,usize)>>{
    let mut deg=vec![vec![];3];
    for i in 0..N{
        for j in 0..N{
            let mut d=0;
            for (di,dj) in DIJ{
                d+=in_range(i+di,j+dj) as usize;
            }
            deg[4-d].push((i,j));
        }
    }
    deg
}



use ordered_float::OrderedFloat as O;
use itertools::*;

const N:usize=6;
const M:usize=15;
const T:usize=10;
const N2:usize=2*N*(N-1);



fn read_input()->[[i64;M];N2]{
    proconio::input_interactive!{
        _a:[[i64;M];N2],
    }
    let mut a=[[0;M];N2];
    for i in 0..N2{
        for j in 0..M{
            a[i][j]=_a[i][j];
        }
    }
    a
}




const DIJ:[(usize,usize);4]=[(0,!0),(!0,0),(0,1),(1,0)];





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
        unsafe{
            std::mem::transmute::<u64,f64>(0x3ff0000000000000|(next() as u64)<<20)-1.
        }
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

    pub fn shuffle<T>(list:&mut [T]){
        for i in (0..list.len()).rev(){
            list.swap(i,get(i+1));
        }
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





fn logs()->&'static [f64;65536]{
    use std::sync::OnceLock;
    static LOGS:OnceLock<[f64;65536]>=OnceLock::new();
    LOGS.get_or_init(||{
        let mut log=[0.;65536];
        for i in 0..65536{
            log[i]=((i as f64+0.5)/65536.).ln();
        }
        rnd::shuffle(&mut log);
        log
    })
}





fn get_time()->f64{
    static mut START:f64=0.;
    let time=std::time::SystemTime::now().duration_since(std::time::UNIX_EPOCH).unwrap().as_secs_f64();
    unsafe{
        if START==0.{
            START=time;
        }

        #[cfg(local)]{
            return (time-START)*1.;
        }
        time-START
    }
}



#[macro_export]
macro_rules! swap{
    ($a:expr,$b:expr)=>{{
        let t=$a;
        $a=$b;
        $b=t;
    }}
}





fn multi_start_iter(mut n:usize,tl:f64)->impl Iterator<Item=f64>{
    std::iter::from_fn(move||{
        if n==0{
            None
        } else{
            let time=get_time();
            let ret=Some((tl-time)/n as f64+time);
            n-=1;
            ret
        }
    })
}



fn abs_diff(a:usize,b:usize)->usize{
    (a as i64-b as i64).abs() as usize
}