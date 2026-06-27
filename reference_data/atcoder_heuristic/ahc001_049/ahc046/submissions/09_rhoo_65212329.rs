#![allow(non_snake_case)]



// ableのzobrist-hash
// eval_scoreをもう少し高級にする，現在のaddの個数とかを考慮
// ビーム幅の調整

// eval_scoreの部分を平均取っていい感じにやるようにする



fn main(){
    let input=get_input();
    let ans=solve();

    let get_dir=|p:P,np:P|->char{
        assert!(p!=np);

        if p.i==np.i{
            if p.j<np.j{
                'R'
            } else{
                assert!(np.j<p.j);
                'L'
            }
        } else{
            assert!(p.j==np.j);
            if p.i<np.i{
                'D'
            } else{
                assert!(np.i<p.i);
                'U'
            }
        }
    };

    let mut last=[[!0;N];N];
    let mut insert=vec![vec![];ans.len()+1];

    let set_around=|p:P,i:usize,last:&mut [[usize;N];N]|{
        for dd in DD{
            let np=p+dd;
            if np.in_range(){
                last[np]=i;
            }
        }
    };

    set_around(input[0],0,&mut last);
    
    for (i,&Act{p,add,rem,..}) in ans.iter().enumerate(){
        if add!=NULL{
            insert[last[add]].push((Add,add));
        }

        for &rem in &rem{
            if rem==NULL{
                break;
            }
            insert[last[rem]].push((Rem,rem));
        }

        set_around(p,i+1,&mut last);
    }
    
    for i in 0..=ans.len(){
        let p=if i==0{
            input[0]
        } else{
            ans[i-1].p
        };
        
        // insert[i]を処理
        for &(_,np) in &insert[i]{
            assert!(p!=np);
            println!("A {}",get_dir(p,np));
        }

        if i==ans.len(){
            break;
        }

        // ans[i]を処理
        let Act{ty,p:np,..}=ans[i];
        if ty==Move{
            println!("M {}",get_dir(p,np));
        } else{
            assert!(ty==Slide);
            println!("S {}",get_dir(p,np));
        }
    }
}



// 大きい方が良い
fn get_eval_score(turn:i64,next:usize,blocks:i64)->f64{
    4.6*(next as f64)-turn as f64+(blocks as f64)*0.01 // todo
}



fn solve()->Vec<Act>{
    // let width=100;
    let input=get_input();
    
    let mut nodes={
        let mut able=BitBord::new();
        for dd in DD{
            let np=input[0]+dd;
            if np.in_range(){
                able.to_true(np);
            }
        }
        
        let block=BitBord::new();
        
        let map=State{
            able,block,
        };

        let node=Node{
            prev:Rc::new(Nil),
            blocks:0,
            mov_dir:!0,
            slide_dir:!0,
            p:input[0],
            map,
            turn:0,
            next:1,
            eval_score:get_eval_score(0,1,0),
            hash:0,
        };

        vec![node]
    };

    let mut new=vec![];
    let mut best=nodes[0].clone();
    let mut best_score=INF;
    let mut set=rustc_hash::FxHashSet::default();

    'a: loop{
        let mut cnt=[[0;N];N];
        set.clear();

        nodes.sort_unstable_by_key(|node|O(-node.eval_score));
        nodes=nodes.into_iter().filter(|node|{
            node.next<M && set.insert((node.p,node.hash)) && {
                cnt[node.p]+=1;
                cnt[node.p]<=8 // todo
            }
        }).collect_vec();

        new.clear();
        for node in &nodes{
            append_nodes(node,&mut new);
        }
        assert!(!new.is_empty());

        std::mem::swap(&mut new,&mut nodes);

        for node in &nodes{
            if node.next==M{
                if best_score>node.turn{
                    best_score=node.turn;
                    best=node.clone();
                }
                if best_score+20<node.turn{ // todo
                    break 'a;
                }
            }
        }
    }

    eprintln!("score = {}",best.turn);
    // eprintln!("blocks = {}",best.blocks);

    best.prev.restore()
}



fn append_nodes(node:&Node,new_nodes:&mut Vec<Node>){
    let input=get_input();
    let zob=get_zob();
    let map=&node.map;

    let get_next=|np:P|{
        node.next+(np==input[node.next]) as usize
    };

    let adjust_to_true=|np:P,map:&mut State|{
        for dd in DD{
            let nnp=np+dd;
            if nnp.in_range(){
                map.able.to_true(nnp);
            }
        }
    };

    // Move
    for (dir,dd) in DD.into_iter().enumerate(){
        // if dir^2==node.mov_dir{
        //     continue;
        // }
        
        let np=node.p+dd;
        if !np.in_range(){
            continue;
        }

        let next=get_next(np);
        let mut new_map=map.clone();

        new_map.able.to_false(np);
        adjust_to_true(np,&mut new_map);

        if !map.block.get(np){
            // 普通に移動
            let act=Act{
                ty:Move,
                p:np,
                add:NULL,
                rem:[NULL;N-1],
            };
            let turn=node.turn+1;
            let blocks=node.blocks;
            
            let new=Node{
                prev:push_list(&node.prev,act),
                blocks,
                mov_dir:dir,
                slide_dir:!0,
                p:np,
                map:new_map,
                turn,next,
                eval_score:get_eval_score(turn,next,blocks),
                hash:node.hash,
            };

            new_nodes.push(new);
        } else{
            // 消して移動
            let mut rem=[NULL;N-1];
            rem[0]=np;

            new_map.block.to_false(np);
            
            let act=Act{
                ty:Move,
                p:np,
                add:NULL,
                rem,
            };
            let turn=node.turn+2;
            let blocks=node.blocks-1;

            let new=Node{
                prev:push_list(&node.prev,act),
                blocks,
                mov_dir:dir,
                slide_dir:!0,
                p:np,
                map:new_map,
                turn,next,
                eval_score:get_eval_score(turn,next,blocks),
                hash:node.hash^zob[np],
            };

            new_nodes.push(new);
        }
    }
    
    // Slide
    for (dir,dd) in DD.into_iter().enumerate(){
        // if dir==node.mov_dir || dir^2==node.mov_dir || dir==node.slide_dir || dir^2==node.slide_dir{
        //     continue;
        // }
        
        let mut np=node.p+dd;
        let mut rem=[NULL;N-1];
        let mut rems=0;
        let mut new_map=map.clone();
        let mut hash=node.hash;
        
        while np.in_range(){
            // npにブロックがあったら破壊する
            if map.block.get(np){
                new_map.block.to_false(np);
                rem[rems]=np;
                rems+=1;
                hash^=zob[np];
            }
            new_map.able.to_false(np);

            // 1マスの移動はMoveにやらせる
            if np==node.p+dd{
                np+=dd;
                continue;
            }
            
            let next=get_next(np);
            let dir=if node.next!=next{
                !0
            } else{
                dir
            };

            // npの次がブロックor範囲外であればそこで止まってnodeに追加
            let nnp=np+dd;
            if !nnp.in_range() || map.block.get(nnp){
                let act=Act{
                    ty:Slide,
                    p:np,
                    add:NULL,
                    rem,
                };

                let mut new_map=new_map.clone();
                adjust_to_true(np,&mut new_map);
                let turn=node.turn+1+rems as i64;
                let blocks=node.blocks-rems as i64;

                let new=Node{
                    prev:push_list(&node.prev,act),
                    blocks,
                    mov_dir:!0,
                    slide_dir:dir,
                    p:np,
                    map:new_map,
                    turn,next,
                    eval_score:get_eval_score(turn,next,blocks),
                    hash,
                };

                new_nodes.push(new);

            } else if map.able.get(nnp){
                // 過去改変できるなら，改変して止まる
                let act=Act{
                    ty:Slide,
                    p:np,
                    add:nnp,
                    rem,
                };

                let mut new_map=new_map.clone();
                new_map.block.to_true(nnp);
                adjust_to_true(np,&mut new_map);
                let turn=node.turn+2+rems as i64;
                let blocks=node.blocks-rems as i64+1;

                let new=Node{
                    prev:push_list(&node.prev,act),
                    blocks,
                    mov_dir:!0,
                    slide_dir:dir,
                    p:np,
                    map:new_map,
                    turn,next,
                    eval_score:get_eval_score(turn,next,blocks),
                    hash:hash^zob[np],
                };

                new_nodes.push(new);
            }

            np+=dd;
        }
    }
}



#[derive(Clone,Copy,PartialEq,Eq)]
enum ActType{
    Move,
    Slide,
}
use ActType::*;



#[derive(Clone,Copy,PartialEq,Eq)]
enum BuildType{
    Add,
    Rem,
}
use BuildType::*;



#[derive(Clone,Copy)]
struct Act{
    ty:ActType,
    p:P,
    add:P,
    rem:[P;N-1],
}



#[derive(Clone)]
struct State{
    able:BitBord,
    block:BitBord,
}



use std::rc::Rc;

#[derive(Clone)]
enum List{
    Cons(Rc<List>,Act),
    Nil,
}
impl List{
    fn restore(&self)->Vec<Act>{
        match self{
            Cons(prev,act)=>{
                let mut res=prev.restore();
                res.push(act.clone());
                res
            },
            Nil=>{
                vec![]
            }
        }
    }
}
use List::*;



fn push_list(prev:&Rc<List>,act:Act)->Rc<List>{
    Rc::new(Cons(prev.clone(),act))
}



#[derive(Clone)]
struct Node{
    prev:Rc<List>,
    blocks:i64,
    mov_dir:usize,
    slide_dir:usize,
    p:P,
    map:State,
    turn:i64,
    next:usize,
    eval_score:f64, // ビームサーチ用のスコア：大きい方が良い todo: 直す
    hash:u64,
}



use ordered_float::OrderedFloat as O;
use itertools::*;


const INF:i64=i64::MAX/64;


const N:usize=20;
const M:usize=40;


static_data!{
    get_input:Vec<P>|{
        proconio::input!{
            n:usize,
            m:usize,
            ij:[(usize,usize);m],
        }
        assert!((n,m)==(N,M));

        ij.into_iter().map(|(i,j)|P::new(i,j)).collect()
    }
}



static_data!{
    get_zob:[[u64;N];N]|{
        use rand::prelude::*;
        let mut rng=StdRng::seed_from_u64(0);

        let mut zob=[[0;N];N];
        for i in 0..N{
            for j in 0..N{
                zob[i][j]=rng.gen_range(0..=u64::MAX);
            }
        }

        zob
    }
}



#[macro_export]
macro_rules! static_data{
    ($a:ident:$t:ty|$e:expr)=>{
        fn $a()->&'static $t{
            use std::sync::OnceLock;
            #[allow(non_upper_case_globals)]
            static $a:OnceLock<$t>=OnceLock::new();
            $a.get_or_init(||$e)
        }
    }
}



// const DC:[char;4]=['L','U','R','D'];
const DD:[P;4]=[P{i:0,j:!0},P{i:!0,j:0},P{i:0,j:1},P{i:1,j:0}];
// const DX:[P;8]=[P{i:0,j:!0},P{i:!0,j:!0},P{i:!0,j:0},P{i:!0,j:1},P{i:0,j:1},P{i:1,j:1},P{i:1,j:0},P{i:1,j:!0}];

const NULL:P=P{i:!0,j:!0};

#[derive(Clone,Copy,PartialEq,Eq,PartialOrd,Ord,Hash,Debug)]
struct P{
    i:usize,
    j:usize,
}
impl P{
    fn new(i:usize,j:usize)->P{
        P{i,j}
    }

    fn in_range(self)->bool{
        self.i<N && self.j<N
    }
}
impl std::ops::Add for P{
    type Output=P;
    fn add(self,rhs:P)->P{
        P{
            i:self.i+rhs.i,
            j:self.j+rhs.j,
        }
    }
}
impl std::ops::Sub for P{
    type Output=P;
    fn sub(self,rhs:P)->P{
        P{
            i:self.i-rhs.i,
            j:self.j-rhs.j,
        }
    }
}
impl std::ops::Mul<usize> for P{
    type Output=P;
    fn mul(self,rhs:usize)->P{
        P{
            i:self.i*rhs,
            j:self.j*rhs,
        }
    }
}
impl std::ops::Div<usize> for P{
    type Output=P;
    fn div(self,rhs:usize)->P{
        P{
            i:self.i/rhs,
            j:self.j/rhs,
        }
    }
}
impl std::ops::Neg for P{
    type Output=P;
    fn neg(self)->P{
        P{
            i:self.i.wrapping_neg(),
            j:self.j.wrapping_neg(),
        }
    }
}

impl std::ops::AddAssign for P{
    fn add_assign(&mut self,rhs:P){
        *self=*self+rhs;
    }
}
impl std::ops::SubAssign for P{
    fn sub_assign(&mut self,rhs:P){
        *self=*self-rhs;
    }
}
impl std::ops::MulAssign<usize> for P{
    fn mul_assign(&mut self,rhs:usize){
        *self=*self*rhs;
    }
}
impl std::ops::DivAssign<usize> for P{
    fn div_assign(&mut self,rhs:usize){
        *self=*self/rhs;
    }
}

impl<T:std::ops::Index<usize>> std::ops::Index<P> for [T]{
    type Output=T::Output;
    fn index(&self,idx:P)->&T::Output{
        &self[idx.i][idx.j]
    }
}
impl<T:std::ops::IndexMut<usize>> std::ops::IndexMut<P> for [T]{
    fn index_mut(&mut self,idx:P)->&mut T::Output{
        &mut self[idx.i][idx.j]
    }
}

impl<T:std::ops::Index<usize>> std::ops::Index<P> for Vec<T>{
    type Output=T::Output;
    fn index(&self,idx:P)->&T::Output{
        &self[idx.i][idx.j]
    }
}
impl<T:std::ops::IndexMut<usize>> std::ops::IndexMut<P> for Vec<T>{
    fn index_mut(&mut self,idx:P)->&mut T::Output{
        &mut self[idx.i][idx.j]
    }
}



#[derive(Clone)]
struct BitBord{
    a:[u64;7],
}
impl BitBord{
    fn new()->BitBord{
        BitBord{
            a:[0;7],
        }
    }

    fn to_false(&mut self,p:P){
        let i=p.i*N+p.j;
        self.a[i>>6]&=!(1<<(i&63));
    }

    fn to_true(&mut self,p:P){
        let i=p.i*N+p.j;
        self.a[i>>6]|=1<<(i&63);
    }

    fn get(&self,p:P)->bool{
        let i=p.i*N+p.j;
        (self.a[i>>6]>>(i&63))&1==1
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

    pub fn shuffle<T>(a:&mut [T]){
        for i in (0..a.len()).rev(){
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
