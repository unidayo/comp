#![allow(non_snake_case)]



static mut HALF:f64=-1.;


fn main(){
    get_time();
    let input=get_input();
    let N=input.N;
    let M=input.M;
    let C=input.C;
    let rat=M as f64/(N*N) as f64;
    
    // 0:simple
    // 1:bidir_750
    // 2:bidir_625
    // 3:bidir_500
    
    let typ=if rat<=0.45{
        0
    } else if rat<=0.50{
       [[0,0,0,0,2],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0]][16-N][C-3]
    } else if rat<=0.55{
       [[0,0,2,2,2],
        [0,0,0,1,2],
        [0,0,0,0,2],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0]][16-N][C-3]
    } else if rat<=0.60{
       [[0,2,2,3,3],
        [0,0,2,2,2],
        [0,0,0,2,2],
        [0,0,0,0,2],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0]][16-N][C-3]
    } else if rat<=0.65{
       [[0,2,3,3,3],
        [0,2,2,2,2],
        [0,0,1,2,2],
        [0,0,0,2,2],
        [0,0,0,0,2],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0]][16-N][C-3]
    } else if rat<=0.70{
       [[2,3,3,3,3],
        [0,2,3,3,3],
        [0,0,2,2,3],
        [0,0,2,2,2],
        [0,0,0,2,2],
        [0,0,0,0,1],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0]][16-N][C-3]
    } else{
       [[3,3,3,3,3],
        [1,2,3,3,3],
        [0,2,3,3,3],
        [0,2,2,2,2],
        [0,0,0,2,2],
        [0,0,0,0,2],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0]][16-N][C-3]
    };
    
    let solve=match typ{
        0=>solve_simple,
        1=>{
            unsafe{HALF=0.75};
            solve_bidir
        },
        2=>{
            unsafe{HALF=0.625};
            solve_bidir
        },
        3=>{
            unsafe{HALF=0.5};
            solve_bidir
        },
        _=>panic!(),
    };
    let ans=solve_until_tl(solve);
    // let ans=solve(2).unwrap();
    
    eprintln!("score = {}",ans.len());
    println!("{}",ans.windows(2).map(|w|DC[dir(w[0],w[1])]).join("\n"));
}




fn solve_simple(WIDTH:usize)->Option<Vec<P>>{
    let input=get_input();
    let state=State::new(&input.d,&[]);
    beam_search(state,!0,WIDTH,false).map(|mut f|{f.insert(0,toP(4,0));f})
}



fn solve_bidir(WIDTH:usize)->Option<Vec<P>>{
    let input=get_input();
    
    let part=((input.M as f64-5.)*unsafe{HALF}).round() as usize+5;
    let d=chain!(repeat(0).take(5),input.d[part..].iter().copied()).collect_vec();
    let mut state=State::new(&d,&[]);
    let first=beam_search(state.clone(),!0,WIDTH,true)?;
    
    for &p in &first{
        state.mov(p);
    }
    
    let (mov,path)=relocate(&state)?;
    for &p in &mov{
        state.mov(p);
    }
    
    state.set(&input.d[..part],&path);
    let second=beam_search(state,path[0],WIDTH,false)?;
    
    Some(chain!(once(toP(4,0)),first,mov,second,path).collect_vec())
}



fn solve_until_tl(mut f:impl FnMut(usize)->Option<Vec<P>>)->Vec<P>{
    let next_width=|width:usize,TL:f64,start:f64,end:f64|->usize{
        ((width as f64*(TL-end)/(end-start)).round() as usize).max(1)
    };
    
    let mut vis=rustc_hash::FxHashSet::default();
    
    let mut WIDTH=5; // todo
    vis.insert(WIDTH);
    let mut start=get_time();
    let mut res=f(WIDTH).unwrap();
    let mut end=get_time();
    
    let mut lis=vec![];
    let T=2; // todo
    for i in 1..=T{
        lis.push(1.8*i as f64/T as f64);
    }
    lis.push(1.9);
    lis.push(1.95);
    lis.push(1.95);
    
    // todo
    for tl in lis{
        if tl<1.9 && tl-end<=0.2{
            continue;
        }
    
        if end<=tl{
            WIDTH=next_width(WIDTH,tl,start,end);
            while vis.contains(&WIDTH){
                WIDTH-=1;
            }
            if WIDTH==0{
                continue;
            }
            vis.insert(WIDTH);
            eprintln!("#{WIDTH}");
            
            start=get_time();
            if let Some(new)=f(WIDTH){
                if res.len()>new.len(){
                    res=new;
                }
            }
            end=get_time();
        }
    }
    
    res
}



fn relocate(state:&State)->Option<(Vec<P>,Vec<P>)>{
    let W=3; // todo
    
    let input=get_input();
    let N=input.N;
    let N2=input.N2;
    let DD=input.DD;
    
    let mut ok=vec![0;N2];
    for (i,&p) in state.route[state.tail..].iter().enumerate(){
        ok[p]=ok[p].max(i);
    }
    
    static_data!{
        get_outer:(Vec<P>,Vec<usize>)|{
            let input=get_input();
            let N=input.N;
            let N2=input.N2;
            
            let outer=chain!(
                (0..N-1).map(|i|toP(0,i)),
                (0..N-1).map(|i|toP(i,N-1)),
                (1..N).rev().map(|i|toP(N-1,i)),
                (1..N).rev().map(|i|toP(i,0)),
            ).collect_vec();
            
            let mut idx=vec![!0;N2];
            for i in 0..outer.len(){
                idx[outer[i]]=i;
            }
            (outer,idx)
        }
    }
    
    let is_corner=|p:P|->bool{
        p==toP(0,0) || p==toP(0,N-1) || p==toP(N-1,0) || p==toP(N-1,N-1)
    };
    
    let get_dir=|mut p:P,q:P|->usize{
        if is_corner(p){
            p=q;
        }
        
        let (i,j)=fromP(p);
        if j==N-1{
            0
        } else if i==N-1{
            1
        } else if j==0{
            2
        } else if i==0{
            3
        } else{
            panic!();
        }
    };
    
    let (ord,id)=get_outer();
    
    let get_diff=|i:usize,j:usize|->usize{
        let abs=i.abs_diff(j);
        abs.min(ord.len()-abs)
    };
    
    let p=state.get_head();
    let mut que=VecDeque::from([(p,0)]);
    let mut prev=vec![!0;N2];
    prev[p]=!1;
    
    let mut turn=vec![];
    let mut cand=BinaryHeap::new();
    let mut next=vec![!0;N2];
    let mut at=0;
    let mut vis=vec![0;N2];
    let mut walk=vec![];
    let mut nok=vec![0;N2];
    
    let board=get_board();
    
    // 一番近い場所にいく
    while let Some((p,d))=que.pop_front(){
        if id[p]!=!0 && !is_corner(p){
            // ここから外周ぐるぐる
            'a: for di in [!0,1]{
                let mut idx=id[p];
                let get_prev=|idx:usize|(idx+ord.len()-di)%ord.len();
                let get_next=|idx:usize|(idx+ord.len()+di)%ord.len();
                
                if is_corner(ord[get_prev(idx)]){
                    continue;
                }
                
                // okに head → p の経路も含める..
                walk.clear();
                {
                    let head=state.get_head();
                    let mut cur=p;
                    while cur!=head{
                        walk.push(cur);
                        cur=prev[cur];
                    }
                    walk.reverse();
                }
                
                nok.copy_from_slice(&ok);
                for (i,&p) in izip!(state.route.len()-state.tail..,&walk){
                    nok[p]=nok[p].max(i);
                }
                
                cand.clear();
                turn.clear();
                turn.push(d+1);
                let mut f=true;
                
                at+=1;
                vis[p]=at;
                
                // state.d.len()-5 回動かないとだめ
                // pですでに1回
                // 2回ずつ
                for _ in 0..(state.d.len()-5)/2{
                    let ni0=get_next(idx);
                    let ni1=get_next(ni0);
                    
                    if get_diff(id[p],ni0)>get_diff(id[p],ni1)
                    && get_diff(id[p],ni1)<=4{
                        f=false;
                    }
                    
                    let p=ord[idx];
                    let p0=ord[ni0];
                    let p1=ord[ni1];
                    
                    let ti=turn.len()-1;
                    
                    if f && nok[p0]<=turn[ti] && nok[p1]<=turn[ti]+1{
                        next[p]=p0;
                        next[p0]=p1;
                        vis[p0]=at;
                        vis[p1]=at;
                        
                        if !is_corner(p0) && !is_corner(p1)
                        && !is_corner(ord[get_next(get_next(ni1))])
                        && !is_corner(ord[get_prev(get_prev(ni0))])
                        {
                            cand.push(Rev((0,ti,p0,p1)));
                        }
                        
                        turn.push(turn[ti]+2);
                        turn[ti]+=1;
                        idx=ni1;
                    } else{
                        let mut fail=true;
                        while let Some(Rev((k,ti,p0,p1)))=cand.pop(){
                            let dir=get_dir(p0,p1);
                            let np0=p0+DD[dir];
                            let np1=p1+DD[dir];
                            
                            if vis[np0]!=at && vis[np1]!=at
                            && nok[np0]<=turn[ti] && nok[np1]<=turn[ti]+1{
                                vis[np0]=at;
                                vis[np1]=at;
                                
                                next[p0]=np0;
                                next[np0]=np1;
                                next[np1]=p1;
                                
                                let nk=k+1;
                                if nk<W{
                                    cand.push(Rev((nk,ti,p0,p1)));
                                }
                                
                                turn[ti]+=1;
                                for i in ti..turn.len(){
                                    turn[i]+=2;
                                }
                                
                                fail=false;
                                break;
                            }
                        }
                        
                        if fail{
                            continue 'a;
                        }
                    }
                }
                
                let i0=get_next(idx);
                let i1=get_next(i0);
                
                let p0=ord[i0];
                let p1=ord[i1];
                next[ord[idx]]=p0;
                
                let dir=get_dir(p0,p1);
                
                let q0=p0+DD[dir];
                let q1=p1+DD[dir];
                
                // p0 → q0 → q1 → p1
                let ti=turn.len()-1;
                
                if vis[p0]!=at && vis[p1]!=at
                && vis[q0]!=at && vis[q1]!=at
                && nok[p0]<=turn[ti] && nok[q0]<=turn[ti]+1
                && nok[q1]<=turn[ti]+2 && nok[p1]<=turn[ti]+3
                {
                    // head → p → ぐるぐる → 噛みちぎり
                    let mut path=vec![p];
                    let mut cur=p;
                    while cur!=p0{
                        cur=next[cur];
                        walk.push(cur);
                        path.push(cur);
                    }
                    walk.extend([q0,q1,p1,p0]);
                    path.pop();
                    path.reverse();
                    path.truncate(state.d.len()-5);
                    return Some((walk,path));
                } else{
                    continue 'a;
                }
            }
        }
        
        for dd in DD{
            let np=p+dd;
            if board[np] && ok[np]<=d && prev[np]==!0{
                prev[np]=p;
                que.push_back((np,d+1));
            }
        }
    }
    
    None
}




fn beam_search(state:State,goal:P,WIDTH:usize,re:bool)->Option<Vec<P>>{
    let input=get_input();
    let N2=input.N2;
    
    assert!(goal==!0 || state.ban[goal]);
    
    let first=state.get_first_cand();
    let mut beam=BeamSearch::new(state.clone(),first);
    let mut is=vec![vec![vec![];4];N2];
    let mut set=rustc_hash::FxHashSet::default();
    let L=2; // todo
    let mut prev=vec![vec![rustc_hash::FxHashSet::default();N2];L*2];
    
    let len=beam.state.d.len();
    let is_end=|cand:&Cand|->bool{
        (goal==!0 || is_neighbor(cand.p,goal))
        && cand.recover==!0
        && cand.fits==len
    };
    
    let mut best=vec![];
    let mut min=usize::MAX;
    let mut tr=0;
    
    for turn in 0..{
        if get_time()>=1.95{
            return None;
        }
        
        if turn!=0{
            for p in 0..N2{
                for dir in 0..4{
                    is[p][dir].clear();
                }
            }
            
            for (i,cand) in beam.cand.iter().enumerate(){
                if is_end(cand){
                    let mut ret=beam.restore(cand.par);
                    ret.push(cand.p);
                    
                    if re{
                        let mut state=state.clone();
                        for &p in &ret{
                            state.mov(p);
                        }
                        
                        if let Some((mov,_))=relocate(&state){
                            tr+=1;
                            let new=mov.len()+ret.len();
                            if min>new{
                                min=new;
                                best=ret;
                            }
                            
                            // todo
                            if tr>=1{
                                return Some(best);
                            }
                        }
                        
                        continue;
                    }
                    
                    return Some(ret);
                }
                
                if (0..L).any(|j|prev[j*2+turn%2][cand.p].contains(&cand.hash)){
                    continue;
                }
                is[cand.p][cand.dir].push(i);
            }
            
            for p in 0..N2{
                prev[turn%(L*2)][p].clear();
            }
            for p in 0..N2{
                for dir in 0..4{
                    set.clear();
                    is[p][dir].sort_unstable_by_key(|&i|O(-beam.cand[i].eval_score));
                    
                    let mut cnt=0;
                    for &i in &is[p][dir]{
                        let cand=&beam.cand[i];
                        if set.insert(cand.hash){
                            prev[turn%(L*2)][cand.p].insert(cand.hash);
                            cnt+=1;
                            beam.select(i);
                            if cnt>=WIDTH{
                                break;
                            }
                        }
                    }
                }
            }
        }
        
        beam.enum_cands();
        assert!(!beam.cand.is_empty(),"次の候補が存在しません");
    }
    
    panic!();
}



// todo
fn eval(len:usize,fits:usize,no_waste:bool,recover:bool)->f64{
    let mut score=fits as f64*1.;
    
    if no_waste{
        if recover{
            assert!(len<fits);
            score+=0.3;
        } else{
            score+=2.;
        }
    }
    
    score-=len.saturating_sub(fits) as f64*0.003;
    
    score
}



impl BeamSearch{
    fn append_cands(&mut self,idx:usize,par:Index){
        let state=&mut self.state;
        let cand=&self.cand[idx];
        assert!(cand.active);
        // assert!((cand.pot-state.get_pot()).abs()<=1e-5);
        // assert!(state.get_hash()==cand.hash);
        
        let mut add_cand=|new:Cand|{
            assert!(new.par==par);
            assert!(!new.active);
            self.next_cand.push(new);
        };
        
        let input=get_input();
        let DD=&input.DD;
        
        let p=cand.p;
        let tail=state.tail;
        let cur_len=state.route.len()-state.tail;
        
        let (zob0,zob1)=get_zob();
        let mut d=!0;
        if cand.recover!=!0{
            let np=state.route[cand.recover];
            assert!(!state.ban[np]);
            
            let recover=if cur_len+1==cand.fits{
                !0
            } else{
                cand.recover-1
            };
            
            d=dir(p,np);
            assert!(d!=cand.dir^2);
            let pot=cand.pot-state.food_pot[np];
            
            let new=Cand{
                p:np,dir:d,
                typ:!1,tail,
                
                fits:cand.fits,
                no_waste:true,
                recover,pot,
                eval_score:pot+eval(cur_len+1,cand.fits,true,recover!=!0),
                hash:cand.hash+(zob1[state.col.len()]-zob0[np])*(state.a[np] as u64+1),
                
                par,active:false,
            };
            add_cand(new);
            
            if (cand.fits-cur_len) as i64>=10{ // todo
                return;
            }
        }
        
        for dir in 0..4{
            if dir==d || dir==cand.dir^2{
                continue;
            }
            let np=p+DD[dir];
            if state.ban[np]{
                continue;
            }
            
            let mut fits=if cand.recover!=!0{
                state.route.len()-state.tail
            } else{
                cand.fits
            };
            let mut no_waste=cand.no_waste;
            
            if state.cnt[np]-(np==state.route[tail]) as u8!=0 && np!=state.route[tail+1]{
                // 噛みちぎり
                let idx=(tail+1..).find(|&i|state.route[i]==np).unwrap();
                let new_len=state.route.len()-idx+1;
                let mut pot=cand.pot;
                
                if new_len>fits || fits-new_len>=10{ // todo
                    continue;
                }
                
                let mut hash=cand.hash;
                let mut it=state.col.len()-1;
                
                for i in tail+1..idx{
                    let p=state.route[i];
                    hash+=(zob0[p]-zob1[it])*(state.col[it] as u64+1);
                    it-=1;
                    pot+=state.food_pot[p];
                }
                
                // if new_len<=fits{
                    let recover=if new_len<fits{
                        idx-1
                    } else{
                        !0
                    };
                    
                    let new=Cand{
                        p:np,dir,typ:idx,tail,
                        
                        fits,no_waste:true,
                        recover,pot,
                        eval_score:pot+eval(new_len,fits,true,recover!=!0),
                        hash,
                        
                        par,active:false,
                    };
                    add_cand(new);
                // } else{
                //     let new=Cand{
                //         p:np,dir,typ:idx,tail,
                        
                //         fits,no_waste:false,
                //         recover:!0,pot,
                //         eval_score:pot+eval(new_len,fits,false,false),
                //         hash,
                        
                //         par,active:false,
                //     };
                //     add_cand(new);
                // }
            } else if state.a[np]==!0{
                let pot=cand.pot;
                
                // 進む
                let new=Cand{
                    p:np,dir,typ:!0,tail,
                    
                    fits,no_waste,
                    recover:!0,pot,
                    eval_score:pot+eval(cur_len,fits,no_waste,false),
                    hash:cand.hash,
                    
                    par,active:false,
                };
                add_cand(new);
            } else{
                // 食べる
                let pot=cand.pot-state.food_pot[np];
                
                if no_waste && fits<state.d.len() && state.d[fits]==state.a[np]{
                    fits+=1;
                } else{
                    no_waste=false;
                }
                
                let new=Cand{
                    p:np,dir,typ:!1,tail,
                    
                    fits,no_waste,
                    recover:!0,pot,
                    eval_score:pot+eval(cur_len+1,fits,no_waste,false),
                    hash:cand.hash+(zob1[state.col.len()]-zob0[np])*(state.a[np] as u64+1),
                    
                    par,active:false,
                };
                add_cand(new);
            }
        }
    }
}



fn center(a:&[u8],ban:&[bool])->(f64,f64){
    let N2=get_input().N2;
    
    // todo
    let (cnt,i,j)=(0..N2)
        .filter(|&p|{
            !ban[p]
            // && a[p]!=!0
        }).fold((0,0,0),|a:(usize,usize,usize),b:P|{
            let (i,j)=fromP(b);
            (a.0+1,a.1+i,a.2+j)
        });
    
    (i as f64/cnt as f64,j as f64/cnt as f64)
}



fn make_food_pot(a:&[u8],ban:&[bool])->Vec<f64>{
    let input=get_input();
    let N=input.N;
    let N2=input.N2;
    let c=center(a,ban);
    
    let mut pot=vec![0.;N2];
    for i in 0..N{
        for j in 0..N{
            let di=(i as f64-c.0)/N as f64;
            let dj=(j as f64-c.1)/N as f64;
            // todo
            let d=di.abs().powf(2.)+dj.abs().powf(2.);
            pot[toP(i,j)]=-d*0.3;
        }
    }
    
    pot
}



#[derive(Clone)]
struct State{
    d:Vec<u8>,
    ban:Vec<bool>,
    food_pot:Vec<f64>,
    
    route:Vec<P>,
    col:Vec<u8>,
    tail:usize,
    a:Vec<u8>,
    cnt:Vec<u8>,
}
impl State{
    fn new(d:&[u8],path:&[P])->State{
        let input=get_input();
        let N2=input.N2;
        
        let mut route=vec![];
        let mut col=vec![];
        let mut cnt=vec![0;N2];
        for i in 0..5{
            let p=toP(i,0);
            route.push(p);
            col.push(0);
            cnt[p]+=1;
        }
        
        let mut state=State{
            d:vec![],
            ban:vec![false;N2],
            food_pot:vec![],
            
            route,col,cnt,tail:0,
            a:input.a.clone(),
        };
        
        state.set(d,path);
        state
    }
    
    fn set(&mut self,d:&[u8],path:&[P]){
        let input=get_input();
        let N=input.N;
        let N2=input.N2;
        let DD=input.DD;
        
        self.d=d.to_vec();
        self.ban.fill(true);
        for i in 0..N{
            for j in 0..N{
                self.ban[toP(i,j)]=false;
            }
        }
        for &p in path{
            self.ban[p]=true;
        }
        
        // banされてないマスは次数2以上、連結
        let mut dsu=ac_library::Dsu::new(N2);
        for p in 0..N2{
            if self.ban[p]{
                continue;
            }
            
            let mut deg=0;
            for dd in DD{
                let np=p+dd;
                if !self.ban[np]{
                    deg+=1;
                    dsu.merge(p,np);
                }
            }
            
            assert!(deg>=2);
        }
        assert!((0..N2).filter(|&p|!self.ban[p]).map(|p|dsu.leader(p)).all_equal());
        assert!(self.route[self.tail..].iter().all(|&p|!self.ban[p]));
        
        self.food_pot=make_food_pot(&self.a,&self.ban);
    }
    
    fn get_head(&self)->P{
        self.route[self.route.len()-1]
    }
    
    fn get_pot(&self)->f64{
        let input=get_input();
        let mut score=0.;
        
        for p in 0..input.N2{
            if self.a[p]!=!0{
                score+=self.food_pot[p];
            }
        }
        
        score
    }
    
    fn get_hash(&self)->u64{
        let input=get_input();
        let (zob0,zob1)=get_zob();
        let mut hash=0;
        
        for p in 0..input.N2{
            if self.a[p]!=!0{
                hash+=zob0[p]*(self.a[p] as u64+1);
            }
        }
        for i in 0..self.col.len(){
            hash+=zob1[i]*(self.col[i] as u64+1);
        }
        
        hash
    }
    
    fn get_first_cand(&self)->Cand{
        let head=self.route.len()-1;
        let pp=self.route[head-1];
        let p=self.route[head];
        let pot=self.get_pot();
        
        let mut fits=0;
        for (a,b) in izip!(&self.d,&self.col){
            if a!=b{
                break;
            }
            fits+=1;
        }
        
        let no_waste=fits==self.col.len();
        
        Cand{
            dir:dir(pp,p),
            p,typ:!0,
            tail:self.tail,
            
            fits,no_waste,
            recover:!0,
            pot,
            eval_score:pot+eval(self.route.len()-self.tail,fits,no_waste,false),
            hash:self.get_hash(),
            
            par:!0,
            active:true,
        }
    }
    
    fn apply(&mut self,node:&Node){
        let np=node.p;
        
        if node.typ==!1{
            self.col.push(self.a[np]);
            self.a[np]=!0;
        } else{
            let p=self.route[self.tail];
            self.cnt[p]-=1;
            
            if node.typ!=!0{
                for i in node.tail+1..node.typ{
                    let p=self.route[i];
                    self.cnt[p]-=1;
                    self.a[p]=self.col.pop().unwrap();
                }
                self.tail=node.typ;
            } else{
                self.tail+=1;
            }
        }
        
        self.route.push(np);
        self.cnt[np]+=1;
    }
    
    fn revert(&mut self,node:&Node){
        let np=node.p;
        self.route.pop().unwrap();
        self.cnt[np]-=1;
        
        if node.typ==!1{
            self.a[np]=self.col.pop().unwrap();
        } else{
            if node.typ!=!0{
                self.tail=node.tail;
                for i in (node.tail+1..node.typ).rev(){
                    let p=self.route[i];
                    self.cnt[p]+=1;
                    self.col.push(self.a[p]);
                    self.a[p]=!0;
                }
            } else{
                self.tail-=1;
            }
            
            let p=self.route[self.tail];
            self.cnt[p]+=1;
        }
    }
    
    fn mov(&mut self,np:P){
        let head=self.route.len()-1;
        assert!(is_neighbor(np,self.route[head]));
        assert!(np!=self.route[head-1]);
        
        if self.a[np]!=!0{
            self.col.push(self.a[np]);
            self.a[np]=!0;
        } else{
            let p=self.route[self.tail];
            self.cnt[p]-=1;
            
            if self.cnt[np]!=0{
                let idx=(self.tail+1..).find(|&i|self.route[i]==np).unwrap();
                
                for i in self.tail+1..idx{
                    let p=self.route[i];
                    self.cnt[p]-=1;
                    self.a[p]=self.col.pop().unwrap();
                }
                self.tail=idx;
            } else{
                self.tail+=1;
            }
        }
        
        self.route.push(np);
        self.cnt[np]+=1;
    }
}



type Index=u32;



#[derive(Clone)]
struct Cand{
    dir:usize,
    p:P,
    typ:usize, // [0,):=噛みちぎりでtailがtypになる、!0:=進んだだけ、!1:=餌を食べた
    tail:usize,
    
    fits:usize,
    no_waste:bool,
    recover:usize, // recoverしたいindex
    pot:f64,
    eval_score:f64,
    hash:u64,
    
    par:Index,
    active:bool,
}
impl Cand{
    fn to_node(&self,rank:Index)->Node{
        assert!(self.active);
        Node{
            p:self.p,
            typ:self.typ,
            tail:self.tail,
            
            rank,
        }
    }
}



#[derive(Clone)]
struct Node{
    p:P,
    typ:usize,
    tail:usize,
    
    rank:Index,
}



struct BeamSearch{
    state:State,
    tour:Vec<Node>,
    next:Vec<Node>,
    cand:Vec<Cand>,
    next_cand:Vec<Cand>,
    stack:Vec<(Index,usize)>,
    track:Vec<P>,
    dir:bool,
}
impl BeamSearch{
    // state: 初期状態
    // first: 初期状態に対応する Cand
    fn new(state:State,first:Cand)->Self{
        assert!(first.active);
        assert!(first.par==!0);
        
        Self{
            state,
            tour:vec![],
            next:vec![],
            cand:vec![first],
            next_cand:vec![],
            stack:vec![],
            track:vec![],
            dir:true,
        }
    }
    
    fn enum_cands(&mut self){
        self.next_cand.clear();

        if self.tour.is_empty(){
            assert!(self.cand.len()==1);
            let node=self.cand[0].to_node(0);
            self.tour.push(node);
            self.append_cands(0,0);
            std::mem::swap(&mut self.cand,&mut self.next_cand);
            return;
        }

        let move_while=|tour:&[Node],i:&mut usize,stack:&mut Vec<_>,leaf,until|{
            stack.clear();
            stack.push((tour[*i].rank,*i));
            while {*i-=1; *i as Index!=until}{
                if tour[*i].rank==leaf{
                    continue;
                }
                let rank=tour[*i].rank;
                let last=stack.len()-1;
                if stack[last].0==rank && stack[last-1].0<=rank{
                    stack.pop();
                } else{
                    stack.push((rank,*i));
                }
            }
            stack.push((tour[*i].rank,*i));
        };

        fn update_track(tour:&[Node],track:&mut Vec<P>,iter:impl Iterator<Item=(usize,usize)>){
            for (ui,vi) in iter{
                let (u,v)=(&tour[ui],&tour[vi]);
                if u.rank>=v.rank{
                    track.pop();
                }
                if u.rank<=v.rank{
                    track.push(v.p);
                }
            }
        }
        
        self.next.clear();
        self.dir=!self.dir;

        let leaf=self.tour[0].rank;

        let mut i=self.tour.len()-1;
        for j in (0..self.cand.len()).rev(){
            let cand=&self.cand[j];
            if !cand.active{
                continue;
            }

            if i as Index!=cand.par{
                move_while(&self.tour,&mut i,&mut self.stack,leaf,cand.par);

                for w in self.stack.windows(2){
                    let (ui,vi)=(w[0].1,w[1].1);
                    let (u,v)=(&self.tour[ui],&self.tour[vi]);
                    if u.rank>=v.rank{
                        self.state.revert(u);
                    }
                    if u.rank<=v.rank{
                        self.state.apply(v);
                    }
                }

                if !self.next_cand.is_empty(){
                    self.next.extend(self.stack.iter().map(|&i|self.tour[i.1].clone()));
                } else if !self.dir{
                    update_track(&self.tour,&mut self.track,self.stack.windows(2).map(|t|(t[0].1,t[1].1)));
                }
            }

            let node=cand.to_node(self.tour[i].rank+1);
            self.state.apply(&node);
            self.append_cands(j,self.next.len() as Index);
            self.state.revert(&node);
            self.next.push(node);
        }

        if self.dir{
            if i!=0{
                move_while(&self.tour,&mut i,&mut self.stack,leaf,0);
                update_track(&self.tour,&mut self.track,self.stack.windows(2).rev().map(|t|(t[1].1,t[0].1)));
            }
            self.track.push(self.next.last().unwrap().p);
        } else{
            self.track.push(self.next[0].p);
        }
        self.state.apply(&self.next[self.next.len()-1]);

        std::mem::swap(&mut self.cand,&mut self.next_cand);
        std::mem::swap(&mut self.tour,&mut self.next);
    }

    fn restore(&self,id:Index)->Vec<P>{
        fn restore(tour:&[Node],track:&mut Vec<P>,iter:impl Iterator<Item=(usize,usize)>){
            for (ui,vi) in iter{
                let (u,v)=(&tour[ui],&tour[vi]);
                if u.rank>=v.rank{
                    track.pop();
                }
                if u.rank<=v.rank{
                    track.push(v.p);
                }
            }
        }

        let mut ret=self.track.clone();
        let id=id as usize;
        if self.dir{
            restore(&self.tour,&mut ret,(id+1..self.tour.len()).rev().map(|i|(i,i-1)));
        } else{
            restore(&self.tour,&mut ret,(0..id).map(|i|(i,i+1)));
        }
        ret
    }

    fn select(&mut self,i:usize){
        assert!(!self.cand[i].active);
        self.cand[i].active=true;
    }
}



static_data!{
    get_zob:(Vec<u64>,Vec<u64>)|{
        let input=get_input();
        let N2=input.N2;
        let M=input.M;
        
        let mut rng=rand_pcg::Pcg64Mcg::new(0);
        use rand::prelude::*;
        let zob0=(0..N2).map(|_|rng.next_u64()).collect();
        let zob1=(0..M).map(|_|rng.next_u64()).collect();
        (zob0,zob1)
    }
}



use std::iter::*;
use std::collections::*;
use std::cmp::Reverse as Rev;
use itertools::*;
use ordered_float::OrderedFloat as O;



struct Input{
    N:usize,
    N2:usize,
    M:usize,
    C:usize,
    DD:[usize;4],
    d:Vec<u8>,
    a:Vec<u8>,
}



static_data!{
    get_input:Input|{
        proconio::input!{
            N:usize,
            M:usize,
            C:usize,
            mut d:[u8;M],
            a_:[[u8;N];N],
        }
        for i in 0..M{
            d[i]-=1;
        }
        let mut a=vec![!0;(N+2)*(N+2)];
        for i in 0..N{
            for j in 0..N{
                let p=(i+1)*(N+2)+j+1;
                a[p]=a_[i][j]-1;
            }
        }
        let DD=[!0,(N+2).wrapping_neg(),1,N+2];
        let N2=(N+2)*(N+2);
        
        Input{N,N2,M,C,DD,d,a}
    }
}




static_data!{
    get_board:Vec<bool>|{
        let input=get_input();
        let N=input.N;
        let N2=input.N2;
        
        let mut board=vec![false;N2];
        for i in 0..N{
            for j in 0..N{
                board[toP(i,j)]=true;
            }
        }
        board
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



const DC:[char;4]=['L','U','R','D'];
type P=usize;



fn toP(i:usize,j:usize)->P{
    let N=get_input().N;
    (i+1)*(N+2)+j+1
}


fn fromP(p:P)->(usize,usize){
    let N=get_input().N;
    (p/(N+2)-1,p%(N+2)-1)
}


// LURD
// pからqを見た時の方向
fn dir(p:P,q:P)->usize{
    let N=get_input().N;
    let diff=q-p;
    
    if diff==!0{
        0
    } else if diff==(N+2).wrapping_neg(){
        1
    } else if diff==1{
        2
    } else{
        assert!(diff==N+2);
        3
    }
}


// p,qが隣接してるか
fn is_neighbor(p:P,q:P)->bool{
    let N=get_input().N;
    let diff=q-p;
    diff==!0 || diff==N+2 || diff==1 || diff==(N+2).wrapping_neg()
}
