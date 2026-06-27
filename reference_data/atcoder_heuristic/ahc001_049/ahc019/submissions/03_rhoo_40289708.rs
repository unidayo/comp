#![allow(non_snake_case)]
#[cfg(not(local))]macro_rules! eprint{($($_:tt)*)=>{}}
#[cfg(not(local))]macro_rules! eprintln{($($_:tt)*)=>{}}
#[cfg(not(local))]macro_rules! assert{($($_:tt)*)=>{}}
#[cfg(not(local))]macro_rules! assert_eq{($($_:tt)*)=>{}}
#[cfg(not(local))]macro_rules! assert_ne{($($_:tt)*)=>{}}


use std::cmp::*;
use std::mem;
use itertools::*;
use std::f64::INFINITY;

const N:usize=16;
const N2:usize=N*N;
const N3:usize=N*N*N;

const ROTS:u8=24;
const MAX_PIECE:usize=32;

const TL:f64=5.95;


#[derive(Clone,Default)]
struct Piece{
    blocks:Vec<(usize,usize)>,
    t:T
}


#[derive(Clone)]
struct Map{
    map:[u8;N3],
    front:[u8;N2],
    right:[u8;N2],
    empty:IndexSet
}
impl Map{
    fn new(input:&InMap)->Map{
        let mut map=[!0;N3];
        for i in 0..N3{
            map[i]=!((!input.map[i]) as u8);
        }
        let mut empty=IndexSet::new(N2*2);
        for i in 0..N2*2{
            if input.sil[(i>=N2) as usize][i%N2]{
                empty.add(i);
            }
        }

        Map{
            map,empty,
            right:[0;N2],
            front:[0;N2]
        }
    }
    
    fn set(&mut self,p:usize,id:usize){
        assert_eq!(self.map[p],!0);
        self.map[p]=id as u8;

        let t=p.ft();
        if self.front[t]==0{
            self.empty.del(t);
        }
        self.front[t]+=1;
        let t=p.rt();
        if self.right[t]==0{
            self.empty.del(t+N2);
        }
        self.right[t]+=1;
    }

    fn del(&mut self,p:usize){
        assert_ne!(self.map[p],!0);
        self.map[p]=!0;

        let t=p.ft();
        assert_ne!(self.front[t],0);
        self.front[t]-=1;
        if self.front[t]==0{
            self.empty.add(t);
        }
        let t=p.rt();
        assert_ne!(self.right[t],0);
        self.right[t]-=1;
        if self.right[t]==0{
            self.empty.add(t+N2);
        }
    }

    fn random_point(&self,input:&InMap)->P{
        // todo:
        if self.empty.is_empty() || rnd::nextf()<=0.03{
            loop{
                let ret=*input.blocks.choice();
                if self.map[ret.t()]==!0{
                    return ret;
                }
            }
        }
        else{
            let ret=self.empty.rand();
            *input.cand[ret].choice()
        }
    }
}


#[derive(Clone)]
struct State{
    map:(Map,Map),
    piece:Vec<Piece>
}
impl State{
    fn new_random(input:&In,piece:usize)->State{
        let mut map=(Map::new(&input.map0),Map::new(&input.map1));
        let mut piece=vec![Piece::default();piece];
        for i in 0..piece.len(){
            let p0=map.0.random_point(&input.map0);
            let p1=map.1.random_point(&input.map1);

            piece[i]=Piece{
                blocks:vec![(p0.t(),p1.t())],
                t:T::new(0,p0,p1)
            };
            map.0.set(p0.t(),i);
            map.1.set(p1.t(),i);
        }

        State{map,piece}
    }

    fn score(&self)->f64{
        self.map.0.empty.len as f64+self.map.1.empty.len as f64
    }

    fn actual_score(&self)->f64{
        self.piece.iter().map(|piece|1./piece.blocks.len() as f64).sum()
    }

    fn reset(&mut self,id:usize){
        for &(p0,p1) in &self.piece[id].blocks{
            self.map.0.del(p0);
            self.map.1.del(p1);
        }
    }

    fn apply(&mut self,id:usize){
        for &(p0,p1) in &self.piece[id].blocks{
            self.map.0.set(p0,id);
            self.map.1.set(p1,id);
        }
    }

    fn kick(&mut self,BF:&mut BUF){
        BF.next.clear();

        let mut ord=(0..self.piece.len()).collect_vec();
        // todo
        for i in 0..(ord.len() as f64*0.8) as usize{
            ord.swap(i,rnd::range(i,self.piece.len()));
            let i=ord[i];
            self.reset(i);
            let (p0,p1)=self.piece[i].t.p;
            self.piece[i].blocks.clear();
            self.piece[i].blocks.push((p0.t(),p1.t()));
            self.map.0.set(p0.t(),i);
            self.map.1.set(p1.t(),i);
            BF.next.push(i);
        }
        BF.rebuild_greedy(self);
        BF.apply(self);
    }
}


static mut SHUF:[u8;ROTS as usize]=[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23];

// todo
const MAX_DIST:usize=10;
const SHUFN:usize=24;
const SIZETH:usize=25;


struct BUF{
    que0:Vec<Vec<P>>,
    stack0:Vec<usize>,
    que1:Vec<Vec<(usize,usize)>>,
    stack1:Vec<(usize,usize)>,
    que2:Vec<Vec<Vec<(usize,usize)>>>,
    next:Vec<usize>,
    at:usize,
    piece:Vec<Vec<(usize,usize)>>,
    seen:([usize;N3],[usize;N3]),
    temp:Piece,
    hist:Vec<(bool,usize,u8)>,
    is_zero:([usize;N2*2],[usize;N2*2]),
    backup:Vec<T>,
}
impl BUF{
    fn new()->BUF{
        BUF{
            que0:vec![vec![];MAX_PIECE],
            stack0:vec![],
            que1:vec![vec![];MAX_PIECE],
            stack1:vec![],
            que2:vec![vec![vec![];MAX_DIST];MAX_PIECE],
            next:vec![],
            at:0,
            piece:vec![vec![];MAX_PIECE+1],
            seen:([!0;N3],[!0;N3]),
            temp:Piece::default(),
            hist:vec![],
            is_zero:([0;N2*2],[0;N2*2]),
            backup:vec![T::default();MAX_PIECE]
        }
    }

    fn best_rotate(&mut self,state:&State,p0:usize,p1:usize,th:usize)->Option<u8>{
        let th0=th.max(SIZETH);
        let mut ret=None;
        let mut max=th;
        
        for i in 0..SHUFN{
            let i=unsafe{
                SHUF.swap(i,rnd::range(i,ROTS as usize));
                SHUF[i]
            };
            self.stack1.clear();
            self.stack1.push((p0,p1));
            self.at+=1;
            self.seen.0[p0]=self.at;
            let mut size=0;

            while let Some((p0,p1))=self.stack1.pop(){
                size+=1;
                if th0<size{
                    return Some(i);
                }
                for &(dd0,dd1) in &DDR[i as usize]{
                    let np0=p0+dd0;
                    let np1=p1+dd1;

                    if state.map.0.map[np0]&state.map.1.map[np1]==!0
                        && self.seen.0[np0]!=self.at
                    {
                        self.seen.0[np0]=self.at;
                        self.stack1.push((np0,np1));
                    }
                }
            }

            if max<size{
                max=size;
                ret=Some(i);
            }
        }

        ret
    }

    fn reset(&mut self,state:&mut State){
        for &i in &self.next{
            state.reset(i);
            let (p0,p1)=state.piece[i].t.p;
            state.map.0.set(p0.t(),i);
            state.map.1.set(p1.t(),i);
        }
    }

    fn init_iszero(&mut self,state:&State){
        for &p0 in state.map.0.empty.to_slice(){
            self.is_zero.0[p0]=self.at;
        }
        for &p1 in state.map.1.empty.to_slice(){
            self.is_zero.1[p1]=self.at;
        }
    }

    fn get(&mut self,p0:usize,p1:usize)->usize{
        let mut t=p0.ft();
        let f0=(self.is_zero.0[t]==self.at) as usize;
        self.is_zero.0[t]-=f0;
        t=p0.rt()+N2;
        let f1=(self.is_zero.0[t]==self.at) as usize;
        self.is_zero.0[t]-=f1;
        t=p1.ft();
        let f2=(self.is_zero.1[t]==self.at) as usize;
        self.is_zero.1[t]-=f2;
        t=p1.rt()+N2;
        let f3=(self.is_zero.1[t]==self.at) as usize;
        self.is_zero.1[t]-=f3;
        f0+f1+f2+f3
    }

    fn allinone(&mut self,state:&mut State,id:usize,p0:P,p1:P,th:usize,add:f64,score:f64)->Option<f64>{
        let th0=th.max(SIZETH);
        let mut arg=!0;
        self.piece[id].clear();

        let p0t=p0.t();
        let p1t=p1.t();

        for i in 0..SHUFN{
            let i=unsafe{
                SHUF.swap(i,rnd::range(i,ROTS as usize));
                SHUF[i]
            };
            self.stack1.clear();
            self.stack1.push((p0t,p1t));
            self.at+=1;
            self.seen.0[p0t]=self.at;
            self.piece[MAX_PIECE].clear();

            while let Some((p0,p1))=self.stack1.pop(){
                self.piece[MAX_PIECE].push((p0,p1));
                for &(dd0,dd1) in &DDR[i as usize]{
                    let np0=p0+dd0;
                    let np1=p1+dd1;

                    if state.map.0.map[np0]&state.map.1.map[np1]==!0
                        && self.seen.0[np0]!=self.at
                    {
                        self.seen.0[np0]=self.at;
                        self.stack1.push((np0,np1));
                    }
                }
            }

            if self.piece[id].len()<self.piece[MAX_PIECE].len(){
                self.piece.swap(id,MAX_PIECE);
                arg=i;
                if th0<self.piece[id].len(){
                    break;
                }
            }
        }

        if th>=self.piece[id].len(){
            state.apply(id);
            return None;
        }

        self.init_iszero(state);
        let mut next_score=state.score();

        for i in 0..self.piece[id].len(){
            let (p0,p1)=self.piece[id][i];
            next_score-=self.get(p0,p1) as f64;
        }

        if next_score-score<=add{
            mem::swap(&mut state.piece[id].blocks,&mut self.piece[id]);
            state.piece[id].t=T::new(arg as u8,p0,p1);
            state.apply(id);
            assert_eq!(state.score(),next_score);
            Some(next_score)
        }
        else{
            state.apply(id);
            None
        }
    }

    fn rebuild_random(&mut self,state:&State)->f64{
        self.at+=1;
        for &i in &self.next{
            let (p0,p1)=state.piece[i].t.p;
            self.que1[i].clear();
            self.que1[i].push((p0.t(),p1.t()));
            self.piece[i].clear();
            self.piece[i].push((p0.t(),p1.t()));
            self.seen.0[p0.t()]=self.at;
            self.seen.1[p1.t()]=self.at;
        }
        
        self.init_iszero(state);
        let mut score=state.score();

        loop{
            let mut update=false;

            for i in 0..self.next.len(){
                let i=self.next[i];
                if self.que1[i].is_empty(){
                    continue;
                }
                update=true;

                let (p0,p1)={
                    let idx=rnd::next()%self.que1[i].len();
                    self.que1[i].swap_remove(idx)
                };

                for &(dd0,dd1) in &DDR[state.piece[i].t.rot as usize]{
                    let np0=p0+dd0;
                    let np1=p1+dd1;

                    if state.map.0.map[np0]&state.map.1.map[np1]==!0
                        && self.seen.0[np0]!=self.at && self.seen.1[np1]!=self.at
                    {
                        self.seen.0[np0]=self.at;
                        self.seen.1[np1]=self.at;
                        self.piece[i].push((np0,np1));

                        score-=self.get(np0,np1) as f64;
                        self.que1[i].push((np0,np1));
                    }
                }
            }

            if !update{
                break;
            }
        }

        score
    }

    // BF.nextはシャッフルしておく
    fn rebuild_greedy(&mut self,state:&State)->f64{
        self.at+=1;
        for &i in &self.next{
            let (p0,p1)=state.piece[i].t.p;
            for j in 0..MAX_DIST{
                self.que2[i][j].clear();
            }
            self.que2[i][0].push((p0.t(),p1.t()));
            self.piece[i].clear();
            self.piece[i].push((p0.t(),p1.t()));
            self.seen.0[p0.t()]=self.at;
            self.seen.1[p1.t()]=self.at;
        }

        self.init_iszero(state);
        let mut score=state.score();
    
        loop{
            let mut update=false;

            for i in 0..self.next.len(){
                let i=self.next[i];
                for dist in 0..MAX_DIST{
                    if self.que2[i][dist].is_empty(){
                        continue;
                    }
                    update=true;
    
                    let (p0,p1)={
                        let idx=rnd::next()%self.que2[i][dist].len();
                        self.que2[i][dist].swap_remove(idx)
                    };
    
                    for &(dd0,dd1) in &DDR[state.piece[i].t.rot as usize]{
                        let np0=p0+dd0;
                        let np1=p1+dd1;
    
                        if state.map.0.map[np0]&state.map.1.map[np1]==!0
                            && self.seen.0[np0]!=self.at && self.seen.1[np1]!=self.at
                        {
                            self.seen.0[np0]=self.at;
                            self.seen.1[np1]=self.at;
                            self.piece[i].push((np0,np1));

                            let add=self.get(np0,np1);
                            score-=add as f64;
                            let ndist=(dist+1)&((add==0) as usize-1);
                            if ndist<MAX_DIST{
                                self.que2[i][ndist].push((np0,np1));
                            }
                        }
                    }
                    break;
                }
            }
    
            if !update{
                break;
            }
        }

        score
    }

    fn revert(&mut self,state:&mut State){
        for &i in &self.next{
            let (p0,p1)=self.piece[i][0];
            state.map.0.del(p0);
            state.map.1.del(p1);
        }
        
        for &i in &self.next{
            for &(p0,p1) in &state.piece[i].blocks{
                state.map.0.set(p0,i);
                state.map.1.set(p1,i);
            }
        }
    }

    fn apply(&mut self,state:&mut State){
        for &i in &self.next{
            assert!(!self.piece[i].is_empty());
            mem::swap(&mut state.piece[i].blocks,&mut self.piece[i]);
            for &(p0,p1) in &state.piece[i].blocks[1..]{
                state.map.0.set(p0,i);
                state.map.1.set(p1,i);
            }
        }
    }
}


// todo
fn get_th(input:&In,piece:usize)->usize{
    let ret=input.map0.blocks.len().min(input.map1.blocks.len()) as f64/piece as f64;
    ((ret/6.).round() as usize).max(1)
}


static mut TOTAL:usize=0;
static mut UPDATE:usize=0;


#[derive(Clone)]
struct SA{
    it:usize,
    add:[f64;64],
    start:f64,
    score:f64,
    tl:f64
}
impl SA{
    fn new(score:f64,tl:f64)->SA{
        let start=get_time();
        let mut ret=SA{it:0,add:[0.;64],start,score:score,tl:tl.min(TL-start)};
        ret.recalc(start);
        ret
    }
    
    fn trans0(&self,piece:usize,state:&mut State,BF:&mut BUF)->Option<f64>{
        let id=self.it%piece;

        // ピースに触れているやつを何個かkick
        let (p0,p1)=*state.piece[id].blocks.choice();
        let (p0,p1)=(P::from(p0),P::from(p1));
        BF.next.clear();
        BF.next.push(id);
        BF.at+=1;
        BF.seen.0[id]=BF.at;
        let len=state.piece[id].blocks.len();

        // todo
        'outer: for i in 0..len.min(3){
            state.piece[id].blocks.swap(i,rnd::range(i,len));
            let (p0,p1)=state.piece[id].blocks[i];
            for &(dd0,dd1) in &DDR[0]{
                const T:usize=3;
                let np0=dd0+p0;
                let id0=state.map.0.map[np0] as usize;
                if id0<piece && BF.seen.0[id0]!=BF.at{
                    BF.seen.0[id0]=BF.at;
                    BF.next.push(id0);
                    if BF.next.len()>=T{
                        break 'outer;
                    }
                }
                let np1=dd1+p1;
                let id1=state.map.1.map[np1] as usize;
                if id1<piece && BF.seen.0[id1]!=BF.at{
                    BF.seen.0[id1]=BF.at;
                    BF.next.push(id1);
                    if BF.next.len()>=T{
                        break 'outer;
                    }
                }
            }
        }
        
        if BF.next.len()<=1{
            return None;
        }

        BF.temp.blocks.clear();
        // todo
        let tid=if rnd::next()&1==0{!0}else{*BF.next[1..].choice()};
        rnd::shuffle(&mut BF.next);

        let old=state.piece[id].t;
        state.piece[id].t=T::new(old.rot,p0,p1);
        if tid!=!0{
            let (p0,p1)=*state.piece[tid].blocks.choice();
            let (p0,p1)=(P::from(p0,),P::from(p1));
            BF.backup[tid]=state.piece[tid].t;
            state.piece[tid].t=T::new(state.piece[tid].t.rot,p0,p1);
        }
        BF.reset(state);
        let next_score=BF.rebuild_random(state);

        if next_score-self.score<=self.add[self.it&63]{
            BF.apply(state);
            Some(next_score)
        }
        else{
            BF.revert(state);
            state.piece[id].t=old;
            if tid!=!0{
                state.piece[tid].t=BF.backup[tid];
            }
            None
        }
    }

    fn trans1(&self,input:&In,piece:usize,state:&mut State,BF:&mut BUF,th:usize)->Option<f64>{
        let id=self.it%piece;
        state.reset(id);

        let p0=state.map.0.random_point(&input.map0);
        let p1=state.map.1.random_point(&input.map1);
        BF.next.clear();
        BF.next.push(id);
        BF.at+=1;
        BF.seen.0[id]=BF.at;
        if rnd::nextf()<=0.75{ // todo
            const T:usize=4; // todo
            for &(dd0,dd1) in &DDR[0]{
                let np0=p0.t()+dd0;
                let id0=state.map.0.map[np0] as usize;
                if id0<piece && BF.seen.0[id0]!=BF.at{
                    BF.seen.0[id0]=BF.at;
                    BF.next.push(id0);
                    if BF.next.len()>=T{
                        break;
                    }
                }
                
                let np1=p1.t()+dd1;
                let id1=state.map.1.map[np1] as usize;
                if id1<piece && BF.seen.0[id1]!=BF.at{
                    BF.seen.0[id1]=BF.at;
                    BF.next.push(id1);
                    if BF.next.len()>=T{
                        break;
                    }
                }
            }
        }

        if BF.next.len()==1{
            let res=BF.allinone(state,id,p0,p1,th,self.add[self.it&63],self.score);
            unsafe{
                UPDATE+=res.is_some() as usize;
            }

            res
        }
        else if let Some(rot)=BF.best_rotate(&state,p0.t(),p1.t(),th){
            BF.temp.blocks.clear();
            mem::swap(&mut BF.temp,&mut state.piece[id]);
            rnd::shuffle(&mut BF.next);
    
            state.piece[id].t=T::new(rot,p0,p1);
            BF.reset(state);
            let next_score=BF.rebuild_random(state);
    
            if next_score-self.score<=self.add[self.it&63]{
                BF.apply(state);
                Some(next_score)
            }
            else{
                BF.revert(state);
                mem::swap(&mut BF.temp,&mut state.piece[id]);
                state.apply(id);
                None
            }
        }
        else{
            state.apply(id);
            None
        }
    }

    fn trans(&self,input:&In,piece:usize,state:&mut State,BF:&mut BUF,th:usize)->Option<f64>{
        // todo
        let res=if rnd::nextf()<=0.3{
            self.trans0(piece,state,BF)
        }
        else{
            self.trans1(input,piece,state,BF,th)
        };

        unsafe{
            TOTAL+=1;
            UPDATE+=res.is_some() as usize;
        }
        res
    }

    fn recalc(&mut self,time:f64){
        let time=(time-self.start)/self.tl;
        // todo:
        const T0:f64=1.;
        const T1:f64=0.05;

        let heat=T0*(T1/T0).powf(time);
        for n in self.add.iter_mut(){
            *n=-rnd::nextf().ln()*heat;
        }
    }

    fn to_valid(&mut self,input:&In,piece:usize,state:&mut State,BF:&mut BUF,th:usize)->bool{
        if let Some(next_score)=self.trans(input,piece,state,BF,th){
            if next_score==0.{
                return true;
            }
            self.score=next_score;
        }

        self.it+=1;
        false
    }
}


#[derive(Clone)]
struct G{
    state:State,
    SA:SA,
    piece:usize,
    th:usize
}
impl G{
    fn new(input:&In,piece:usize,BF:&mut BUF,th:usize,tl:f64)->G{
        let mut state=State::new_random(input,piece);
        BF.next.clear();
        for i in 0..piece{
            BF.next.push(i);
        }
        let score=BF.rebuild_greedy(&mut state);
        BF.apply(&mut state);
        assert!(score==state.score());
        
        G{
            state,piece,th,
            SA:SA::new(score,tl)
        }
    }

    fn to_valid(&mut self,input:&In,BF:&mut BUF)->bool{
        self.SA.to_valid(input,self.piece,&mut self.state,BF,self.th)
    }

    fn reduce_piece(&mut self,input:&In,tl:f64){
        let id=(0..self.piece).min_by_key(|&i|self.state.piece[i].blocks.len()).unwrap();
        self.piece-=1;
        self.state.reset(id);
        if id!=self.piece{
            self.state.reset(self.piece);
        }
        self.state.piece.swap_remove(id);
        if id!=self.piece{
            self.state.apply(id);
        }
        self.th=get_th(input,self.piece);

        let score=self.state.score();
        self.SA=SA::new(score,tl);
    }
    
    fn restart(&mut self,th:usize,tl:f64,kick:bool,BF:&mut BUF){
        self.th=th;
        if kick{
            self.state.kick(BF);
        }
        let score=self.state.score();
        self.SA=SA::new(score,tl);
    }
}


struct MapLS{
    map:[u8;N3],
    front:[u8;N2],
    right:[u8;N2],
    empty:usize
}
impl MapLS{
    fn new(input:&InMap,map:&[u8;N3])->MapLS{
        let mut front=[0;N2];
        let mut right=[0;N2];
        for &p in &input.blocks{
            if map[p.t()]!=!0{
                front[p.ft()]+=1;
                right[p.rt()]+=1;
            }
        }
        
        MapLS{
            map:*map,
            front,
            right,
            empty:0
        }
    }

    fn get_size(&self,p:usize,id:u8,BF:&mut BUF)->usize{
        BF.stack0.clear();
        BF.at+=1;
        
        for &dd in &DD{
            let np=p+dd;
            if self.map[np]==id{
                BF.stack0.push(np);
                BF.seen.0[np]=BF.at;
                let mut size=0;

                while let Some(p)=BF.stack0.pop(){
                    size+=1;
                    for &dd in &DD{
                        let np=p+dd;
                        if self.map[np]==id && BF.seen.0[np]==BF.at{
                            BF.seen.0[np]=BF.at;
                            BF.stack0.push(np);
                        }
                    }
                }

                return size;
            }
        }

        0
    }

    fn set(&mut self,p:usize,id:u8){
        let f=p.ft();
        let r=p.rt();
        if self.map[p]!=!0{
            assert_ne!(self.front[f],0);
            self.front[f]-=1;
            self.empty+=(self.front[f]==0) as usize;
            assert_ne!(self.right[r],0);
            self.right[r]-=1;
            self.empty+=(self.right[r]==0) as usize;
            self.map[p]=!0;
        }

        if id!=!0{
            self.empty-=(self.front[f]==0) as usize;
            self.front[f]+=1;
            self.empty-=(self.right[r]==0) as usize;
            self.right[r]+=1;
            self.map[p]=id;
        }
    }

    fn is_valid(&self)->bool{
        self.empty==0
    }
}


fn local_search(input:&In,piece:&[T],m0:&mut [u8;N3],m1:&mut [u8;N3],BF:&mut BUF)->(f64,usize){
    let compute_score=|size:&[_]|size.iter().map(|&x|1./x as f64).sum::<f64>();
    
    let mut size=vec![0;piece.len()];
    for &p in &input.map0.blocks{
        let id=m0[p.t()];
        if id!=!0{
            size[id as usize]+=1;
        }
    }
    let mut score=compute_score(&size);
    let mut ord=(0..piece.len()).collect_vec();
    ord.sort_unstable_by_key(|&i|size[i]);

    let mut map0=MapLS::new(&input.map0,&m0);
    let mut map1=MapLS::new(&input.map1,&m1);

    for i in 0..piece.len(){
        BF.que0[i].clear();
        for &p in &input.map0.blocks{
            BF.que0[i].push(p);
        }
    }

    loop{
        let mut update=false;
        'outer: for &n in &ord{
            if BF.que0[n].is_empty(){
                continue;
            }
            update=true;
            let p0={
                let idx=rnd::next()%BF.que0[n].len();
                BF.que0[n].swap_remove(idx)
            };
            let p1=p0*piece[n];

            if map0.map[p0.t()]!=n as u8
                && DDP.iter().any(|&dd|map0.map[(p0+dd).t()]==n as u8)
                && map1.map[p1.t()]!=!1
            {
                let id0=map0.map[p0.t()];
                let id1=map1.map[p1.t()];
                assert_ne!(id0,!1);
                assert_ne!(id1,!1);

                let get=|id,add|if id==!0{0.}else{1./(size[id as usize] as f64+add)-1./size[id as usize] as f64};

                let mut f=false;
                let diff=if id0==id1 && id0!=!0 && {
                    let np1=p0*piece[id0 as usize];
                    np1==p1
                }{
                    f=true;
                    get(id0,-1.)
                } else if id0==id1{
                    get(id0,-2.)
                } else{
                    get(id0,-1.)+get(id1,-1.)
                }+get(n as u8,1.);


                if diff<0.{
                    BF.hist.clear();
                    BF.hist.push((false,p0.t(),map0.map[p0.t()]));
                    map0.set(p0.t(),n as u8);
                    BF.hist.push((true,p1.t(),map1.map[p1.t()]));
                    map1.set(p1.t(),n as u8);
                    if !f{
                        if id0!=!0{
                            let np1=p0*piece[id0 as usize];
                            BF.hist.push((true,np1.t(),map1.map[np1.t()]));
                            map1.set(np1.t(),!0);
                        }
                        if id1!=!0{
                            let np0=p1*-piece[id1 as usize];
                            BF.hist.push((false,np0.t(),map0.map[np0.t()]));
                            map0.set(np0.t(),!0);
                        }
                    }

                    if map0.is_valid() && map1.is_valid(){
                        if f{
                            size[id0 as usize]-=1;
                        }
                        else{
                            if id0!=!0{
                                size[id0 as usize]-=1;
                            }
                            if id1!=!0{
                                size[id1 as usize]-=1;
                            }
                        }
                        size[n]+=1;

                        if (id0==!0 || map0.get_size(p0.t(),id0,BF)==size[id0 as usize])
                            && (id1==!0 || map1.get_size(p1.t(),id1,BF)==size[id1 as usize])
                        {
                            score+=diff;
                            for &dd in &DDP{
                                let np=p0+dd;
                                if input.map0.map[np.t()]{
                                    for i in 0..piece.len(){
                                        BF.que0[i].push(np);
                                    }
                                }
                            }
                            break 'outer;
                        }
                        else{
                            if f{
                                size[id0 as usize]+=1;
                            }
                            else{
                                if id0!=!0{
                                    size[id0 as usize]+=1;
                                }
                                if id1!=!0{
                                    size[id1 as usize]+=1;
                                }
                            }
                            size[n]-=1;
                            for &(f,p,id) in BF.hist.iter().rev(){
                                let map=&mut [&mut map0,&mut map1][f as usize];
                                map.set(p,id);
                            }
                        }
                    }
                    else{
                        for &(f,p,id) in BF.hist.iter().rev(){
                            let map=&mut [&mut map0,&mut map1][f as usize];
                            map.set(p,id);
                        }
                    }
                }
            }
        }

        if !update{
            break;
        }
    }

    let true_score:f64=size.iter().map(|&x|1./x as f64).sum();
    assert!((true_score-score).abs()<=1e-5);

    *m0=map0.map;
    *m1=map1.map;

    (score,size.into_iter().min().unwrap())
}


const TD:f64=0.6; // todo

fn minimize_piece(input:&In,BF:&mut BUF,inital_piece:usize,tl:f64,d:f64)->Option<G>{
    let mut state=G::new(input,inital_piece,BF,get_th(input,inital_piece),TD);
    
    let mut it=0;
    let mut valid=0;
    let mut best=None;
    let mut last=0.;
    let mut time=0.;
    
    loop{
        if it&255==0{
            time=get_time();
            if time>=tl{
                break;
            }
            state.SA.recalc(time);
            if time-last>=d && best.is_some(){
                break;
            }
            if time-state.SA.start>=TD{
                // todo
                state.restart(state.th,TD,false,BF);
            }
        }
        it+=1;

        if state.to_valid(input,BF){
            // eprintln!("# Found! {} {:.3}",state.piece,get_time());
            last=time;
            best=Some(state.clone());
            valid+=1;
            if state.piece==2{
                break;
            }
            state.reduce_piece(input,TD);
        }
    }

    let best=best?;
    eprintln!("it0 = {}",it);
    eprintln!("piece = {}",best.piece);
    eprintln!("phase1_score = {:.4}",best.state.actual_score());
    eprintln!("time = {:.3}",get_time());

    Some(best)
}


fn minimize_score(input:&In,state:&mut G,BF:&mut BUF)->([u8;N3],[u8;N3]){
    let mut best_score=INFINITY;
    let mut best=([!0;N3],[!0;N3]);
    let mut it=0;
    let mut valid=0;

    let mut trans=vec![];
    macro_rules! update{
        ($state:expr)=>{
            {
                valid+=1;
                trans.clear();
                for p in &$state.state.piece{
                    trans.push(p.t);
                }
                let mut map0=$state.state.map.0.map;
                let mut map1=$state.state.map.1.map;
                let (score,size)=local_search(input,&trans,&mut map0,&mut map1,BF);

                if best_score>score{
                    best_score=score;
                    best=(map0,map1);
                }

                size
            }
        };
    }

    let th=update!(state);
    state.restart(th-1,TD,true,BF);

    loop{
        if it&255==0{
            let time=get_time();
            if time>=TL{
                break;
            }
            state.SA.recalc(time);
            if time-state.SA.start>=TD{
                state.restart(state.th,TD,false,BF);
            }
        }
        it+=1;

        if state.to_valid(input,BF){
            let th=update!(state);
            state.restart(th-1,TD,true,BF);
        }
    }
    
    eprintln!("it1 = {}",it);
    eprintln!("valid = {}",valid);
    eprintln!("th = {}",state.th);
    eprintln!("score = {:.4}",best_score);

    best
}


fn solve(input:&In)->(usize,[u8;N3],[u8;N3]){
    let get_inital=|base:usize|base.min(input.map0.blocks.len()/2).min(input.map1.blocks.len()/2);
    
    let mut BF=BUF::new();
    // todo
    let mut state=minimize_piece(input,&mut BF,get_inital(16),3.5,2.2).unwrap_or_else(
        ||minimize_piece(input,&mut BF,get_inital(25),TL,100.).unwrap()
    );
    assert_eq!(state.state.score(),0.);
    let piece=state.piece;
    let (map0,map1)=minimize_score(input,&mut state,&mut BF);
    (piece,map0,map1)
}


fn main(){
    let input=In::input();
    let ans=solve(&input);
    unsafe{
        eprintln!("total = {}",TOTAL);
        eprintln!("update_ratio = {:.5}",UPDATE as f64/TOTAL as f64);
    }
    output(input.d,ans.0,&ans.1,&ans.2);
}


///////////////////////////////////////////////////////

#[cfg(めも)]
後でやる!{
    // - ピースの再構築を乱択 & ユーティリティ
    // - 有効解かどうかは特に考えず体積最大化の焼きなまし
    // - ピース数固定で最低限のvalid解をランダム多点Primで焼きなましで作成
    // - 提出可能な状態にする
    - 増加道みつけて局所改善
    - thとかpieceとかの設定で天才をしたい
    // - 高速化
    - 近傍とかkickの種類数
    // - 嘘Primの精度を頑張って上げる(多様性も上げたほうが良さそう)
}

///////////////////////////////////////////////////////



struct InMap{
    blocks:Vec<P>,
    // front,right
    sil:[[bool;N2];2],
    map:[bool;N3],
    cand:Vec<Vec<P>>
}
impl InMap{
    fn new(d:usize,sil:[[bool;N2];2])->InMap{
        let mut map=[false;N3];
        let mut cand=vec![vec![];N2*2];
        let mut blocks=vec![];
        
        for (i,j,k) in iproduct!(1..=d,1..=d,1..=d){
            let idx=P::new(i,j,k);
            let f=sil[0][idx.ft()]&sil[1][idx.rt()];
            map[idx.t()]=f;
            if f{
                blocks.push(idx);
                cand[idx.ft()].push(idx);
                cand[idx.rt()+N2].push(idx);
            }
        }

        InMap{blocks,sil,map,cand}
    }
}


use proconio::{*,marker::*};
struct In{
    d:usize,
    map0:InMap,
    map1:InMap
}
impl In{
    fn input()->In{
        input!{
            d:usize,
            fi:[[Chars;d];4]
        }

        let mut t=[[[false;N2];2];2];
        for (i,j,k) in iproduct!(0..4,1..=d,1..=d){
            t[i>>1][i&1^1][j*N+k]=fi[i][j-1][k-1]=='1';
        }

        In{
            d,
            map0:InMap::new(d,t[0]),
            map1:InMap::new(d,t[1])
        }
    }
}


#[fastout]
fn output(d:usize,n:usize,res0:&[u8;N3],res1:&[u8;N3]){
    let res=[res0,res1];
    
    println!("{}",n);
    for i in 0..2{
        for (j,k,l) in iproduct!(1..=d,1..=d,1..=d){
            let idx=P::new(j,k,l);
            let x=res[i][idx.t()]+1;
            print!("{} ",if x==!0{0}else{x});
        }
        println!();
    }
}


#[allow(unused)]
mod rnd {
    static mut S:usize=88172645463325252;

    pub fn next()->usize{
        unsafe{
            S^=S<<7;
            S^=S>>9;
            S
        }
    }

    pub fn nextf()->f64{
        f64::from_bits((0x3ff0000000000000|next()&0xfffffffffffff) as u64)-1.
    }

    pub fn range(a:usize,b:usize)->usize{
        next()%(b-a)+a
    }

    pub fn exu(a:usize,b:usize,skip:usize)->usize{
        assert!(a<=skip && skip<b);
        let ret=range(a,b-1);
        
        ret+(skip<=ret) as usize
    }

    pub fn rangei(a:i64,b:i64)->i64{
        (next()%((b-a) as usize)) as i64+a
    }

    pub fn shuffle<T>(list:&mut [T]){
        for i in (0..list.len()).rev(){
            list.swap(i,next()%(i+1));
        }
    }
}


fn get_time()->f64{
    static mut START:f64=-1.;
    let time=std::time::SystemTime::now().duration_since(std::time::UNIX_EPOCH).unwrap().as_secs_f64();
    unsafe{
        if START<0.{
            START=time;
        }

        // todo
        #[cfg(local)]{
            (time-START)*1.25
        }
        #[cfg(not(local))]{
            time-START
        }
    }
}


#[derive(Clone)]
struct IndexSet{
    pos:Vec<usize>,
    que:Vec<usize>,
    len:usize
}
impl IndexSet{
    fn new(n:usize)->IndexSet{
        IndexSet{
            pos:(0..n).collect(),
            que:(0..n).collect(),
            len:0
        }
    }

    fn add(&mut self,a:usize){
        let p=self.pos[a];
        assert!(self.len<=p);

        self.que.swap(p,self.len);
        self.pos.swap(a,self.que[p]);
        self.len+=1;
    }

    fn del(&mut self,a:usize){
        let p=self.pos[a];
        assert!(p<self.len);

        self.que.swap(p,self.len-1);
        self.pos.swap(a,self.que[p]);
        self.len-=1;
    }

    fn rand(&self)->usize{
        self.que[rnd::next()%self.len]
    }

    fn nrand(&self)->usize{
        self.que[rnd::range(self.len,self.que.len())]
    }

    fn to_slice(&self)->&[usize]{
        &self.que[..self.len]
    }

    fn nto_slice(&self)->&[usize]{
        &self.que[self.len..]
    }

    fn contains(&self,v:usize)->bool{
        self.pos[v]<self.len
    }

    fn is_empty(&self)->bool{
        self.len==0
    }

    fn clear(&mut self){
        self.len=0;
    }

    fn fill(&mut self){
        self.len=self.pos.len();
    }
}


#[derive(Clone,Copy,PartialEq,Eq,Debug,Default,Hash)]
struct T{
    rot:u8,
    add:P,
    p:(P,P),
}
impl T{
    fn new(rot:u8,p0:P,p1:P)->T{
        let add=p1-p0.rot(rot);
        T{rot,add,p:(p0,p1)}
    }
}
impl Neg for T{
    type Output=T;
    fn neg(self)->T{
        const NEG_ROT:[u8;24]=[0,1,2,3,4,6,5,7,9,8,10,11,16,19,17,18,12,14,15,13,22,21,20,23];
        T::new(NEG_ROT[self.rot as usize],self.p.1,self.p.0)
    }
}


#[derive(Clone,Copy,PartialEq,Eq,Debug,Default,Hash)]
struct P(i64,i64,i64);
impl P{
    fn from(t:usize)->P{
        P((t&15) as i64,(t>>4&15) as i64,(t>>8) as i64)
    }
    
    fn new(i:usize,j:usize,k:usize)->P{
        P(i as i64,j as i64,k as i64)
    }
    
    // fn in_range(&self,n:usize)->bool{
    //     n>self.0 as usize-1 && n>self.1 as usize-1 && n>self.2 as usize-1
    // }

    fn t(&self)->usize{
        ((self.2 as usize)<<8)|((self.1 as usize)<<4)|self.0 as usize
    }
    
    fn ft(&self)->usize{
        ((self.2 as usize)<<4)|self.1 as usize
    }

    fn rt(&self)->usize{
        ((self.2 as usize)<<4)|self.0 as usize
    }

    fn rot(&self,rot:u8)->P{
        let ret=match rot>>2{
            0=>P(self.0,self.1,self.2),
            1=>P(-self.0,self.2,self.1),
            2=>P(-self.1,self.0,self.2),
            3=>P(self.1,self.2,self.0),
            4=>P(self.2,self.0,self.1),
            5=>P(-self.2,self.1,self.0),
            _=>panic!()
        };

        let f=[
            (1,1,1),
            (-1,-1,1),
            (-1,1,-1),
            (1,-1,-1)
        ][(rot&3) as usize];
        
        P(ret.0*f.0,ret.1*f.1,ret.2*f.2)
    }
}
use std::ops::*;
impl Add for P{
    type Output=P;
    fn add(self,a:P)->P{
        P(self.0+a.0,self.1+a.1,self.2+a.2)
    }
}
impl Sub for P{
    type Output=P;
    fn sub(self,a:P)->P{
        P(self.0-a.0,self.1-a.1,self.2-a.2)
    }
}
impl Mul<i64> for P{
    type Output=P;
    fn mul(self,a:i64)->P{
        P(self.0*a,self.1*a,self.2*a)
    }
}
impl Div<i64> for P{
    type Output=P;
    fn div(self,a:i64)->P{
        P(self.0/a,self.1/a,self.2/a)
    }
}
impl Neg for P{
    type Output=P;
    fn neg(self)->P{
        P(-self.0,-self.1,-self.2)
    }
}
impl AddAssign for P{
    fn add_assign(&mut self,a:P){
        *self=*self+a;
    }
}
impl SubAssign for P{
    fn sub_assign(&mut self,a:P){
        *self=*self-a;
    }
}
impl MulAssign<i64> for P{
    fn mul_assign(&mut self,a:i64){
        *self=*self*a;
    }
}
impl DivAssign<i64> for P{
    fn div_assign(&mut self,a:i64){
        *self=*self/a;
    }
}

impl Mul<T> for P{
    type Output=P;
    fn mul(self,t:T)->P{
        self.rot(t.rot)+t.add
    }
}
impl MulAssign<T> for P{
    fn mul_assign(&mut self,t:T){
        *self=*self*t
    }
}


const DDP:[P;6]=[P(1,0,0),P(-1,0,0),P(0,1,0),P(0,-1,0),P(0,0,1),P(0,0,-1)];
const DD:[usize;6]=[1,!0,16,!15,256,!255,];
const DDR:[[(usize,usize);6];24]=[[(1,1),(!0,!0),(16,16),(!15,!15),(256,256),(!255,!255),],[(1,!0),(!0,1),(16,!15),(!15,16),(256,256),(!255,!255),],[(1,!0),(!0,1),(16,16),(!15,!15),(256,!255),(!255,256),],[(1,1),(!0,!0),(16,!15),(!15,16),(256,!255),(!255,256),],[(1,!0),(!0,1),(16,256),(!15,!255),(256,16),(!255,!15),],[(1,1),(!0,!0),(16,256),(!15,!255),(256,!15),(!255,16),],[(1,1),(!0,!0),(16,!255),(!15,256),(256,16),(!255,!15),],[(1,!0),(!0,1),(16,!255),(!15,256),(256,!15),(!255,16),],[(1,16),(!0,!15),(16,!0),(!15,1),(256,256),(!255,!255),],[(1,!15),(!0,16),(16,1),(!15,!0),(256,256),(!255,!255),],[(1,16),(!0,!15),(16,1),(!15,!0),(256,!255),(!255,256),],[(1,!15),(!0,16),(16,!0),(!15,1),(256,!255),(!255,256),],[(1,256),(!0,!255),(16,1),(!15,!0),(256,16),(!255,!15),],[(1,256),(!0,!255),(16,!0),(!15,1),(256,!15),(!255,16),],[(1,!255),(!0,256),(16,!0),(!15,1),(256,16),(!255,!15),],[(1,!255),(!0,256),(16,1),(!15,!0),(256,!15),(!255,16),],[(1,16),(!0,!15),(16,256),(!15,!255),(256,1),(!255,!0),],[(1,!15),(!0,16),(16,256),(!15,!255),(256,!0),(!255,1),],[(1,16),(!0,!15),(16,!255),(!15,256),(256,!0),(!255,1),],[(1,!15),(!0,16),(16,!255),(!15,256),(256,1),(!255,!0),],[(1,256),(!0,!255),(16,16),(!15,!15),(256,!0),(!255,1),],[(1,256),(!0,!255),(16,!15),(!15,16),(256,1),(!255,!0),],[(1,!255),(!0,256),(16,16),(!15,!15),(256,1),(!255,!0),],[(1,!255),(!0,256),(16,!15),(!15,16),(256,!0),(!255,1),],];


trait RandomChoice{
    type Output;
    fn choice(&self)->&Self::Output;
}
impl<T> RandomChoice for Vec<T>{
    type Output=T;
    fn choice(&self)->&T{
        &self[rnd::next()%self.len()]
    }
}
impl<T> RandomChoice for [T]{
    type Output=T;
    fn choice(&self)->&T{
        &self[rnd::next()%self.len()]
    }
}


#[derive(Clone)]
struct S<T:Clone,U:Clone>(T,U);
impl<T:PartialEq+Clone,U:Clone> PartialEq for S<T,U>{
    fn eq(&self,a:&S<T,U>)->bool{
        self.0==a.0
    }
}
impl<T:PartialEq+Clone,U:Clone> Eq for S<T,U>{}
impl<T:PartialOrd+Clone,U:Clone> PartialOrd for S<T,U>{
    fn partial_cmp(&self,a:&S<T,U>)->Option<std::cmp::Ordering>{
        self.0.partial_cmp(&a.0)
    }
}
impl<T:PartialOrd+Clone,U:Clone> Ord for S<T,U>{
    fn cmp(&self,a:&S<T,U>)->std::cmp::Ordering{
        self.partial_cmp(a).unwrap()
    }
}


trait Hoge{
    fn ft(self)->usize;
    fn rt(self)->usize;
}

impl Hoge for usize{
    fn ft(self)->usize{
        self>>4
    }
    
    fn rt(self)->usize{
        use std::arch::x86_64::_pext_u64;
        unsafe{
            _pext_u64(self as u64,0xf0f) as usize
        }
    }
}