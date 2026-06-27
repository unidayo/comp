#![allow(non_snake_case)]


const CASES:usize=40; // todo: nで変える？


// seed=221,496,899


fn main(){
    get_time();
    get_perm();
    let input=get_input();

    let mut state=State::new();

    // state.make_tree();
    // let n=input.n;
    // let mut grid=vec![vec![false;n];n];
    // for p in iterp(n){
    //     grid[p]=state.tree[p.id(n)]!=!0;
    // }
    // render(&grid);
    
    let ans=anneal(&mut state,1.95);
    
    let mut ps=vec![];
    for p in iterp(input.n){
        if input.is_wall[p]^(!ans[p]){
            assert!(p!=input.start);
            assert!(input.is_wall[p]==false && ans[p]==false);
            ps.push(p);
        }
    }

    // eprintln!("#{:.3}",timer_ratio());
    
    println!("{} {}",ps.len(),ps.iter().map(|p|[p.i,p.j]).flatten().join(" "));
    println!("-1");
}



fn anneal(state:&mut State,tl:f64)->Vec<Vec<bool>>{
    let input=get_input();
    let n=input.n;
    
    let log=logs();
    let start=get_time();
    let tl=tl-start;
    assert!(0.<tl);

    let mut valid=0;
    let mut iter=0;
    let mut heat;
    let mut score=state.eval();
    eprintln!("inital = {:.0}",score);

    let mut tree=vec![!0;n*n];
    std::mem::swap(&mut state.tree,&mut tree);
    let mut bf=vec![];
    let mut best_score=score;
    let mut best=tree.clone();

    let mut cand=iterp(n).map(|p|state.pri[p.id(n)].iter().map(move|&di|(p,di))).flatten().collect_vec();
    let mut idx=cand.len();
    let mut genp=||->(P,usize){
        idx-=1;
        if idx==!0{
            idx=cand.len()-1;
        }
        cand.swap(idx,rnd::get(idx+1));
        cand[idx]
    };

    loop{
        let time=(get_time()-start)/tl;

        const T0:f64=100.; // todo
        const T1:f64=1.;
        heat=T0*(T1/T0).powf(time);
        // heat=T0+(T1-T0)*time;

        if time>=1.{
            break;
        }
        
        iter+=1;
        let add=heat*log[iter&65535]; // 最大化
        // let add=0.;

        let ok=|id:u16,p:P|->bool{
            let mut cnt=0;
            for &di in &state.pri[p.id(n)]{
                if tree[(p+DD[di]).id(n)]<=id{
                    cnt+=1;
                }
            }

            cnt<=1
        };
        
        let mut pi;
        loop{
            let (p,di)=genp();
            pi=p.id(n);

            if tree[pi]!=!0{
                bf.clear();
                bf.extend(&state.pri[pi]);
                let len=bf.len();
                if len<=1{
                    continue;
                }

                let idx=(0..len).find(|&i|bf[i]==di).unwrap();
                bf.rotate_left(idx);
                assert!(bf[0]==di);

                if let Some(nd)=bf.iter().copied().find(|&di|{
                    let np=p+DD[di];
                    ok(tree[pi],np)
                }){
                    if tree[(p+DD[nd]).id(n)]==!0
                    //  && nd==di
                    {
                        break;
                    }
                }
            }
        }

        let p=P::from(pi,n);
        std::mem::swap(&mut state.pri[pi],&mut bf);

        const R:usize=2; // todo
        let mut rot=[[0;R*2+1];R*2+1];

        if rnd::get(30)==0{ // todo
            if (p+P::new(R,R)).in_range(n) && (p-P::new(R,R)).in_range(n){
                for i in R as i64*-1..=R as i64{
                    for j in R as i64*-1..=R as i64{
                        let (i,j)=(i as usize,j as usize);
                        if (i,j)!=(0,0){
                            let pri=&mut state.pri[(p+P::new(i,j)).id(n)];
                            if pri.len()!=0{
                                rot[i+R][j+R]=rnd::get(pri.len());
                                pri.rotate_left(rot[i+R][j+R]);
                            }
                        }
                    }
                }
            }
        }

        let new=state.eval();
        
        if new-score>=add{ // 最大化
            valid+=1;
            score=new;
            std::mem::swap(&mut state.tree,&mut tree);
            if best_score<score{
                best_score=score;
                best=tree.clone();
            }
        } else{
            std::mem::swap(&mut state.pri[pi],&mut bf);

            if (p+P::new(R,R)).in_range(n) && (p-P::new(R,R)).in_range(n){
                for i in R as i64*-1..=R as i64{
                    for j in R as i64*-1..=R as i64{
                        let (i,j)=(i as usize,j as usize);
                        if (i,j)!=(0,0){
                            state.pri[(p+P::new(i,j)).id(n)].rotate_right(rot[i+R][j+R]);
                        }
                    }
                }
            }
        }
    }

    eprintln!("iter = {}",iter);
    eprintln!("ratio = {:.3}",valid as f64/iter as f64);
    eprintln!("final = {:.0}",best_score);

    let mut grid=vec![vec![false;n];n];
    for p in iterp(n){
        grid[p]=best[p.id(n)]!=!0;
    }

    grid
}



struct State{
    pri:Vec<Vec<usize>>,
    tree:Vec<u16>,
    cnt:Vec<u16>,
    stack:Vec<P>,
    
    memo:Vec<(usize,usize)>, // rank,par
    visible:Vec<P>,
    rank:Vec<usize>,
    par:Vec<P>,
    dsu:Dsu,
    at:u64,
    mask:Vec<[u64;2]>,
    seeni:Vec<u64>,
    seenj:Vec<u64>,
    vis:Vec<u64>,
    que:VecDeque<P>,
}
impl State{
    fn new()->State{
        let input=get_input();
        let n=input.n;
        
        let mut pri=get_connection().clone();
        let is=[0,1,2,3]; // todo
        for p in iterp(n){
            let pi=p.id(n);
            let pri=&mut pri[pi];
            pri.sort_by_key(|&di|is[di]);
            
            let len=pri.len();
            if len>0{
                pri.rotate_left(rnd::get(len));
            }
        }

        State{
            pri,
            tree:vec![!0;n*n],
            cnt:vec![0;n*n],
            stack:vec![],
            memo:vec![(!0,!0);n*n],
            visible:vec![NULL;n*n],
            par:vec![NULL;n*n],
            rank:vec![!0;n*n],
            dsu:Dsu::new(n*n),
            at:0,
            mask:vec![[0;2];n*n],
            seeni:vec![0;n],
            seenj:vec![0;n],
            vis:vec![0;n*n],
            que:VecDeque::new(),
        }
    }

    fn make_tree(&mut self)->u8{
        let input=get_input();
        let n=input.n;

        let pri=&self.pri;
        let tree=&mut self.tree;
        let cnt=&mut self.cnt;
        let rank=&mut self.rank;
        let level=*get_level();
        
        tree.fill(!0);
        cnt.fill(0);
        let mut id=0;

        cap!{
            #![n:usize,pri:&Vec<Vec<usize>>,tree:&mut [u16],
                cnt:&mut [u16],&mut id:u16,rank:&mut [usize]]
            
            fn rec(p:P,depth:usize){
                let pi=p.id(n);
                assert!(tree[pi]==!0);

                tree[pi]=*id;
                *id+=1;
                rank[pi]=depth;
                for &di in &pri[pi]{
                    cnt[(p+DD[di]).id(n)]+=1;
                }

                for &di in &pri[pi]{
                    let np=p+DD[di];
                    let npi=np.id(n);
                    if tree[npi]==!0 && cnt[npi]==1{
                        rec!(np,depth+1);
                    }
                }
            }
        }

        rec!(input.start,1);

        for p in iterp(n){
            let pi=p.id(n);
            if tree[pi]!=!0 && cnt[pi]==1 && p!=input.start && p!=input.goal
                && pri[pi].iter().all(|&di|cnt[(p+DD[di]).id(n)]!=1)
            {
                tree[pi]=!0;
            }
        }

        let is_wall=&input.is_wall;
        let g=input.goal;

        let table=get_table();
        let check_around=|p:P,tree:&[u16]|->bool{
            let mut bit=0;
            for di in 0..8{
                let np=p+DX[di];
                bit|=((np.in_range(n) && tree[np.id(n)]!=!0) as usize)<<di;
            }
            table[bit]
        };
        
        let isok=|p:P,tree:&[u16]|->bool{
            !(p.manh(g)==level && (p.i==g.i || p.j==g.j))
            && !is_wall[p] && check_around(p,tree)
        };

        let prev=&mut self.par;
        let que=&mut self.que;
        self.at+=1;
        let at=self.at;
        let seen=&mut self.vis;

        que.clear();
        que.push_back(g);
        prev[g.id(n)]=NULL;
        seen[g.id(n)]=at;

        let mut max=(0,NULL);

        while let Some(v)=que.pop_front(){
            let all=DD.iter().all(|&dd|{
                let nv=v+dd;
                !nv.in_range(n) || tree[nv.id(n)]==!0
            });

            let mut min=(!0,NULL);
            
            for dd in DD{
                let nv=v+dd;
                let nvi=nv.id(n);
                if nv.in_range(n) && seen[nvi]!=at{
                    prev[nvi]=v;
                    seen[nvi]=at;
                    if tree[nvi]!=!0{
                        if min.0>rank[nvi]{
                            min=(rank[nvi],nv);
                        }
                    } else if all && isok(nv,tree){
                        que.push_back(nv);
                    }
                }
            }

            if min.1!=NULL && max.0<min.0{
                max=min;
            }
        }

        if max.1==NULL{
            return !0;
        }

        let stack=&mut self.stack;
        stack.clear();

        let mut v=prev[max.1.id(n)];
        let mut depth=max.0+1;
        while v!=NULL{
            stack.push(v);
            let vi=v.id(n);
            assert!(tree[vi]==!0);
            tree[vi]=id;
            rank[vi]=depth;
            depth+=1;
            id+=1;
            v=prev[vi];
        }

        stack.pop().unwrap();
        stack.reverse();
        let mut v=max.1;
        'a: loop{
            stack.push(v);
            if v==input.start{
                break;
            }
            for dd in DD{
                let nv=v+dd;
                let nvi=nv.id(n);
                if nv.in_range(n) && tree[nvi]!=!0 && rank[v.id(n)]>rank[nvi]{
                    v=nv;
                    continue 'a;
                }
            }
            panic!();
        }

        let on_path=&mut self.vis;
        self.at+=1;
        let at=self.at;

        for &p in stack.iter(){
            on_path[p.id(n)]=at;
        }

        //   # b
        // z g a x
        //   y #

        //   w # b
        // z g a . x
        //   y v #

        let (a,b,v,w,x,y,z,l,u,r,d);
        if level==2{
            (a,b)=(stack[0],stack[1]);
            (l,u)=(g-a,b-a);
            (r,d)=(-l,-u);
            (x,y,z)=(a+r,g+d,g+l);
            (v,w)=(NULL,NULL);
        } else{
            assert!(level==3);
            (a,b)=(stack[0],stack[2]);
            (l,u)=(g-a,b-(a*2-g));
            (r,d)=(-l,-u);
            (x,y,z,v,w)=(g+r*2,g+d,g+l,a+d,g+u);
        }

        let x_dir=[u,r,d];
        let y_dir=[d,l];
        let z_dir=[d,l,u];
        let v_dir=[d];
        let w_dir=[l,u];

        let level2_cand=[(x,&x_dir[..]),(y,&y_dir[..]),(z,&z_dir[..])];
        let level3_cand=[(x,&x_dir[..]),(v,&v_dir[..]),(y,&y_dir[..]),(z,&z_dir[..]),(w,&w_dir[..])];

        let cand=if level==2{
            &level2_cand[..]
        } else{
            assert!(level==3);
            &level3_cand[..]
        };

        let get_next=|mut p:P,dir:P,tree:&[u16]|->Option<P>{
            while p.in_range(n) && !is_wall[p]{
                if tree[p.id(n)]!=!0 || DD.iter().any(|&dd|{let np=p+dd; np.in_range(n) && tree[np.id(n)]!=!0}){
                    return Some(p);
                }
                p+=dir;
            }
            None
        };

        let mut done=[false;5];

        // candのマスがon_pathのマスから見えるように
        for i in 0..cand.len(){
            let (p,dirs)=cand[i];

            let mut min=(!0,NULL,NULL);
            'a: for &dir in dirs{
                if let Some(mut np)=get_next(p+dir,dir,tree){
                    if tree[np.id(n)]!=!0 || check_around(np,tree){
                        let mut cnt=np.manh(p)-(tree[np.id(n)]!=!0) as usize;
                        while on_path[np.id(n)]!=at{
                            let npi=np.id(n);
                            let old=tree[npi];
                            cnt+=(old==!0) as usize;
                            tree[npi]=!1;

                            let nnp=np+dir;
                            if !nnp.in_range(n) || is_wall[nnp] || tree[nnp.id(n)]==!0 && !check_around(nnp,tree){
                                tree[npi]=old;
                                continue 'a;
                            }
                            tree[npi]=old;
                            
                            np=nnp;
                        }

                        if min.0>cnt{
                            min=(cnt,np,dir);
                        }
                    }
                }
            }

            if min.1!=NULL{
                done[i]=true;

                let mut t=min.1;
                while t!=p{
                    if tree[t.id(n)]==!0{
                        tree[t.id(n)]=id;
                        id+=1;
                    }
                    t-=min.2;
                }
            }
        }

        // できなそうだったら適当にたくさん穴を開けておく
        for i in 0..cand.len(){
            if done[i]{
                continue;
            }
            let (p,dirs)=cand[i];

            for &dir in dirs{
                if let Some(np)=get_next(p+dir,dir,tree){
                    if tree[np.id(n)]!=!0 || check_around(np,tree){
                        let mut t=np;
                        while t!=p{
                            if tree[t.id(n)]==!0{
                                tree[t.id(n)]=id;
                                id+=1;
                            }
                            t-=dir;
                        }
                    }
                }
            }
        }

        if done[..cand.len()].iter().all(|&f|f){
            1
        } else{
            0
        }
    }

    fn eval(&mut self)->f64{
        let res=self.make_tree();
        if res==!0{
            return -1e6;
        }

        let input=get_input();
        let n=input.n;
        let memo=&mut self.memo;
        let dsu=&mut self.dsu;
        let rank=&mut self.rank;
        dsu.clear();
        self.at+=1;
        let at=self.at;
        let tree=&self.tree;
        let visible=&mut self.visible;
        let par=&mut self.par;
        let vis=&mut self.vis;
        
        cap!{
            #![tree:&[u16],memo:&mut [(usize,usize)],rank:&mut [usize],
                dsu:&mut Dsu,vis:&mut [u64],at:u64,n:usize,
                visible:&mut [P],par:&mut [P]]
            
            fn rec(p:P,v:P,last:P,dir:usize){
                let vi=v.id(n);
                assert!(vis[vi]!=at);
                vis[vi]=at;
                visible[vi]=last;
                par[vi]=p;

                for di in 0..4{
                    let nv=v+DD[di];
                    let nvi=nv.id(n);
                    if nv.in_range(n) && tree[nvi]!=!0 && vis[nvi]!=at{
                        rank[nvi]=rank[vi]+1;
                        if di==dir{
                            rec!(v,nv,last,dir);
                        } else{
                            rec!(v,nv,v,di);
                        }
                    }
                }

                if p!=NULL{
                    let pi=p.id(n);
                    memo[dsu.merge(pi,vi).unwrap().1]=(rank[vi],pi);
                }
            }
        }

        let mask=&mut self.mask;

        for t in 0..n{
            let mut sj=0;
            while sj!=n-1{
                let tj=(sj+1..n).find(|&j|tree[t*n+j]==!0).unwrap_or(n-1);
                let bit=get_mask(sj..=tj);
                for j in sj..=tj{
                    mask[t*n+j][0]=bit;
                }
                sj=tj;
            }

            let mut si=0;
            while si!=n-1{
                let ti=(si+1..n).find(|&i|tree[i*n+t]==!0).unwrap_or(n-1);
                let bit=get_mask(si..=ti);
                for i in si..=ti{
                    mask[i*n+t][1]=bit;
                }
                si=ti;
            }
        }

        let seeni=&mut self.seeni;
        let seenj=&mut self.seenj;
        let goal=input.goal;

        let root=input.start.id(n);
        rank[root]=1;
        rec!(NULL,input.start,NULL,!0);
        memo[dsu.leader(root)]=(0,!0);

        let lca=|u:P,v:P|->P{
            let(mut u,mut v)=(u.id(n),v.id(n));
            assert!(tree[u]!=!0 && tree[v]!=!0);
            let mut ret=u;
            while u!=v{
                if memo[u].0>memo[v].0{
                    (u,v)=(v,u);
                }
                ret=memo[v].1;
                v=dsu.a[v] as usize;
            }
            P::from(ret,n)
        };

        let mut score=0;
        for perm in get_perm(){
            let mut v=input.start;

            self.at+=1;
            let at=self.at;
            seeni.fill(0);
            seenj.fill(0);

            cap!{
                #![n:usize,at:u64,mask:&[[u64;2]],tree:&[u16],
                vis:&mut [u64],seeni:&mut [u64],seenj:&mut [u64]]

                fn fill(p:P){
                    let pi=p.id(n);
                    assert!(tree[pi]!=!0 && vis[pi]!=at);
                    vis[pi]=at;
                    let [bi,bj]=mask[pi];
                    seeni[p.i]|=bi;
                    seenj[p.j]|=bj;
                }
            }
            
            fill!(v);

            for &np in perm{
                if seeni[goal.i]>>goal.j&1==1 || seenj[goal.j]>>goal.i&1==1{
                    break;
                }

                let npi=np.id(n);
                if seeni[np.i]>>np.j&1==1 || seenj[np.j]>>np.i&1==1{
                    continue;
                }

                let add;
                let nv=if tree[np.id(n)]!=!0{
                    let nv=visible[npi];
                    let mi=lca(v,nv).id(n);
                    assert!(vis[mi]==at);
                    add=rank[v.id(n)]+rank[nv.id(n)]-rank[mi]*2;
                    nv
                } else{
                    let mut min=(!0,NULL);
                    for dd in DD{
                        let nnp=np+dd;
                        let nnpi=nnp.id(n);
                        if nnp.in_range(n) && tree[nnpi]!=!0{
                            let mut nv=visible[nnpi];
                            if nv.i!=np.i && nv.j!=np.j{
                                nv=nnp;
                            }
                            let dist=rank[v.id(n)]+rank[nv.id(n)]-rank[lca(v,nv).id(n)]*2;
                            if min.0>dist{
                                min=(dist,nv);
                            }
                        }
                    }

                    if min.1==NULL{
                        continue;
                    }

                    add=min.0;
                    min.1
                };
                
                assert!(vis[nv.id(n)]!=at);

                let mut t=nv;
                while vis[t.id(n)]!=at{
                    fill!(t);
                    t=par[t.id(n)];
                }
                score+=add;

                assert!(seeni[np.i]>>np.j&1==1 || seenj[np.j]>>np.i&1==1);
                v=nv;
            }
        }

        let mut score=score as f64/get_perm().len() as f64;

        if res==1{
            score+=500.; // todo
        }

        score
    }
}



static_data!{
    get_table:[bool;256]|{
        let mut a=[false;256];
        for bit in 0u8..=255{
            let mask=bit.rotate_left(1)|bit.rotate_right(1)|0b01010101;
            let new=bit&mask;
            a[bit as usize]=(new^new.rotate_left(1)).count_ones()<=2;
        }
        a
    }
}



static_data!{
    get_blocked_wall:Vec<bool>|{
        let input=get_input();
        let n=input.n;
        let level=*get_level();

        let mut is_wall=vec![false;n*n];
        for p in iterp(input.n){
            let pi=p.id(n);
            is_wall[pi]=input.is_wall[p];
            if p.manh(input.goal)<=level{
                is_wall[pi]=true;
            }
        }
        is_wall
    }
}



static_data!{
    get_connection:Vec<Vec<usize>>|{
        let is_wall=get_blocked_wall();
        let n=get_input().n;

        let mut a=vec![vec![];n*n];
        for p in iterp(n){
            for di in 0..4{
                let np=p+DD[di];
                if np.in_range(n) && !is_wall[np.id(n)]{
                    a[p.id(n)].push(di);
                }
            }
        }

        a
    }
}



static_data!{
    get_perm:Vec<Vec<P>>|{
        let n=get_input().n;
        let mut ps=vec![iterp(n).collect_vec();CASES];
        for i in 0..CASES{
            rnd::shuffle(&mut ps[i]);
        }
        ps
    }
}



static_data!{
    get_level:usize|{
        let input=get_input();
        let n=get_input().n;
        let mut is_wall=input.is_wall.clone();
        let s=input.start;
        let g=input.goal;

        for level in 2..4{
            for dd in DD{
                let np=g+dd*level;
                if np.in_range(n){
                    is_wall[np]=true;
                }
            }

            let mut dsu=Dsu::new(n*n);
            for p in iterp(n){
                for &dd in &DD[..2]{
                    let np=p+dd;
                    if !is_wall[p] && np.in_range(n) && !is_wall[np]{
                        dsu.merge(p.id(n),np.id(n));
                    }
                }
            }

            if dsu.same(s.id(n),g.id(n)){
                return level;
            }

            for dd in DD{
                let np=g+dd*level;
                if np.in_range(n){
                    is_wall[np]=input.is_wall[np];
                }
            }
        }

        panic!();
    }
}



use std::collections::*;
use itertools::*;
use proconio::{marker::*,input_interactive as input};



struct Input{
    n:usize,
    start:P,
    goal:P,
    is_wall:Vec<Vec<bool>>,
}


static_data!{
    get_input:Input|{
        input!{
            n:usize,
            goal:(usize,usize),
            b:[Chars;n],
        }

        let mut is_wall=vec![vec![false;n];n];
        for p in iterp(n){
            is_wall[p]=b[p]=='T';
        }

        let start=P::new(0,n/2);
        let goal=P::new(goal.0,goal.1);

        Input{n,start,goal,is_wall}
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



const DD:[P;4]=[P{i:0,j:!0},P{i:!0,j:0},P{i:0,j:1},P{i:1,j:0}];
const DX:[P;8]=[P{i:0,j:!0},P{i:!0,j:!0},P{i:!0,j:0},P{i:!0,j:1},P{i:0,j:1},P{i:1,j:1},P{i:1,j:0},P{i:1,j:!0}];
const NULL:P=P{i:!0,j:!0};

#[derive(Clone,Copy,PartialEq,Eq,PartialOrd,Ord,Hash)]
struct P{
    i:usize,
    j:usize,
}
impl P{
    fn new(i:usize,j:usize)->P{
        P{i,j}
    }

    fn from(x:usize,n:usize)->P{
        P{i:x/n,j:x%n}
    }

    fn id(self,n:usize)->usize{
        self.i*n+self.j
    }

    fn in_range(self,n:usize)->bool{
        self.i<n && self.j<n
    }
    
    fn manh(self,p:P)->usize{
        let abs_diff=|a,b|(a as i64-b as i64).abs() as usize;
        abs_diff(self.i,p.i)+abs_diff(self.j,p.j)
    }
}


impl std::fmt::Debug for P{
    fn fmt(&self,f:&mut std::fmt::Formatter)->std::fmt::Result{
        write!(f,"({}, {})",self.i,self.j)
    }
}


impl std::ops::Add for P{
    type Output=P;
    fn add(self,a:P)->P{
        P{
            i:self.i+a.i,
            j:self.j+a.j,
        }
    }
}
impl std::ops::Sub for P{
    type Output=P;
    fn sub(self,a:P)->P{
        P{
            i:self.i-a.i,
            j:self.j-a.j,
        }
    }
}
impl std::ops::Mul<usize> for P{
    type Output=P;
    fn mul(self,a:usize)->P{
        P{
            i:self.i*a,
            j:self.j*a,
        }
    }
}
impl std::ops::Div<usize> for P{
    type Output=P;
    fn div(self,a:usize)->P{
        P{
            i:self.i/a,
            j:self.j/a,
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


macro_rules! impl_p_ops{
    ($t:ty,$assign_trait:ident,$assign_func:ident,$op:tt)=>{
        impl std::ops::$assign_trait<$t> for P{
            fn $assign_func(&mut self,a:$t){
                *self=*self $ op a;
            }
        }
    }
}
impl_p_ops!(P,AddAssign,add_assign,+);
impl_p_ops!(P,SubAssign,sub_assign,-);
impl_p_ops!(usize,MulAssign,mul_assign,*);
impl_p_ops!(usize,DivAssign,div_assign,/);


macro_rules! impl_p_index{
    ($t:ty)=>{
        impl<T:std::ops::Index<usize>> std::ops::Index<P> for $t{
            type Output=T::Output;
            fn index(&self,idx:P)->&T::Output{
                &self[idx.i][idx.j]
            }
        }
        impl<T:std::ops::IndexMut<usize>> std::ops::IndexMut<P> for $t{
            fn index_mut(&mut self,idx:P)->&mut T::Output{
                &mut self[idx.i][idx.j]
            }
        }
    }
}
impl_p_index!([T]);
impl_p_index!(Vec<T>);


fn iterp(n:usize)->impl Iterator<Item=P>{
    iproduct!(0..n,0..n).map(|(i,j)|P::new(i,j))
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



#[macro_export]
macro_rules! cap{
    ()=>{};
    (#![] $($fn:tt)*)=>{
        cap!([],[],[],[],[$($fn)*],[],[]);
    };
    (#![$($g:tt)*] $($fn:tt)*)=>{
        cap!([$($g)*,],[],[],[],[$($fn)*],[],[]);
    };
    ([$name:ident:$t:ty,$($rem:tt)*],[$($ga:tt)*],[$($gn:tt)*],[$($ge:tt)*],[$($fn:tt)*],[],[])=>{
        cap!([$($rem)*],[$name:$t,$($ga)*],[$name,$($gn)*],[$name,$($ge)*],[$($fn)*],[],[]);
    };
    ([$($flag:tt)? $name:ident:$t:ty,$($rem:tt)*],[$($ga:tt)*],[$($gn:tt)*],[$($ge:tt)*],[$($fn:tt)*],[],[])=>{
        cap!([$($rem)*],[$name:$($flag)?$t,$($ga)*],[$name,$($gn)*],[$($flag)?$name,$($ge)*],[$($fn)*],[],[]);
    };
    ([&mut $name:ident:$t:ty,$($rem:tt)*],[$($ga:tt)*],[$($gn:tt)*],[$($ge:tt)*],[$($fn:tt)*],[],[])=>{
        cap!([$($rem)*],[$name:&mut $t,$($ga)*],[$name,$($gn)*],[&mut $name,$($ge)*],[$($fn)*],[],[]);
    };
    ([$(,)?],[$($ga:tt)*],[$($gn:tt)*],[$($ge:tt)*],[$(#[$($att:tt)*])? fn $name:ident($($arg:tt)*) $body:block $($rem:tt)*],[$($info:tt)*],[$($fn:tt)*])=>{
        cap!([],[$($ga)*],[$($gn)*],[$($ge)*],[$($rem)*],[(($(#[$($att)*])?),$name,($($arg)*),(),$body),$($info)*],[$name,$($fn)*]);
    };
    ([$(,)?],[$($ga:tt)*],[$($gn:tt)*],[$($ge:tt)*],[$(#[$($att:tt)*])? fn $name:ident($($arg:tt)*)->$ret:ty $body:block $($rem:tt)*],[$($info:tt)*],[$($fn:tt)*])=>{
        cap!([],[$($ga)*],[$($gn)*],[$($ge)*],[$($rem)*],[(($(#[$($att)*])?),$name,($($arg)*),$ret,$body),$($info)*],[$name,$($fn)*]);
    };
    ([$(,)?],[$($ga:tt)*],[$($gn:tt)*],[$($ge:tt)*],[],[$($info:tt)*],[$($fn:tt)*])=>{
        cap!(@make_fn [$($ga)*],[$($gn)*],[$($ge)*],[$($info)*],[$($fn)*]);
    };
    (@make_fn [$($ga:ident:$gt:ty,)*],[$($gn:tt)*],[$($ge:tt)*],[(($($att:tt)*),$name:ident,($($arg:tt)*),$ret:ty,$body:block),$($rem:tt)*],[$($fn:tt)*])=>{
        $($att)*
        fn $name($($ga:$gt,)*$($arg)*)->$ret{
            $(#[allow(unused_variables)] let $ga=$ga;)*
            cap!(@make_macros ($),[$($gn)*],[$($fn)*]);
            $body
        }
        cap!(@make_fn [$($ga:$gt,)*],[$($gn)*],[$($ge)*],[$($rem)*],[$($fn)*]);
    };
    (@make_fn [$($ga:tt)*],[$($gn:tt)*],[$($ge:tt)*],[],[$($fn:tt)*])=>{
        cap!(@make_global_macros ($),[$($ge)*],[$($fn)*]);
    };
    (@make_macros ($dol:tt),[$($gn:ident,)*],[$name:ident,$($rem:tt)*])=>{
        #[allow(unused_macros)]
        macro_rules! $name{
            ($dol($dol arg:expr),*)=>{$name($($gn,)* $dol($dol arg),*)}
        }
        cap!(@make_macros ($),[$($gn,)*],[$($rem)*]);
    };
    (@make_macros ($dol:tt),[$($gn:ident,)*],[])=>{};
    (@make_global_macros ($dol:tt),[$($ge:expr,)*],[$name:ident,$($rem:tt)*])=>{
        #[allow(unused_macros)]
        macro_rules! $name{
            ($dol($dol arg:expr),*)=>{$name($($ge,)* $dol($dol arg),*)}
        }
        cap!(@make_global_macros ($),[$($ge,)*],[$($rem)*]);
    };
    (@make_global_macros ($dol:tt),[$($ge:expr,)*],[])=>{};
}



#[allow(unused)]
fn render(grid:&Vec<Vec<bool>>){
    let h=grid.len();
    if h==0{
        return;
    }
    let w=grid[0].len();

    let mut out=String::new();
    for i in 0..h{
        for j in 0..w{
            if !grid[i][j]{
                out.push('#');
                continue;
            }

            let up=0<i && grid[i-1][j];
            let down=i+1<h && grid[i+1][j];
            let left=0<j && grid[i][j-1];
            let right=j+1<w && grid[i][j+1];

            let idx=(up as usize)<<3|(right as usize)<<2|(down as usize)<<1|left as usize;
            let new=['.','─','│','┐','─','─','┌','┬','│','┘','│','┤','└','┴','├','┼'][idx];
            out.push(new);
        }

        if i+1<h{
            out.push('\n');
        }
    }

    eprintln!("{out}\n");
}



struct Dsu{
    a:Vec<i64>,
}
impl Dsu{
    fn new(n:usize)->Dsu{
        Dsu{a:vec![-1;n]}
    }

    fn clear(&mut self){
        self.a.fill(-1);
    }

    fn leader(&self,mut v:usize)->usize{
        assert!(v<self.a.len());
        while 0<=self.a[v]{
            v=self.a[v] as usize;
        }
        v
    }

    fn same(&mut self,u:usize,v:usize)->bool{
        self.leader(u)==self.leader(v)
    }

    fn merge(&mut self,u:usize,v:usize)->Option<(usize,usize)>{
        let mut u=self.leader(u);
        let mut v=self.leader(v);
        if u==v{
            return None;
        }
        
        if self.a[u]>self.a[v]{
            (u,v)=(v,u);
        }

        self.a[u]+=self.a[v];
        self.a[v]=u as i64;
        Some((u,v))
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

        #[cfg(feature = "local")]{
            return (time-START)*1.;
        }
        #[allow(unused)]{
            time-START
        }
    }
}



#[macro_export]
macro_rules! timer{
    ()=>{
        #[cfg(feature = "local")] let _timer=Timer(get_time());
    }
}

static mut TIME:f64=0.;
#[allow(unused)]
struct Timer(f64);
impl Drop for Timer{
    fn drop(&mut self){
        unsafe{ TIME+=get_time()-self.0 }
    }
}

#[allow(unused)]
fn timer_ratio()->f64{
    unsafe{ TIME/get_time() }
}



fn get_mask(range:impl std::ops::RangeBounds<usize>)->u64{
    use std::ops::Bound::*;
    let a=match range.start_bound(){
        Included(&n)=>n,
        Excluded(&n)=>n+1,
        Unbounded=>0,
    };
    let b=match range.end_bound(){
        Included(&n)=>n+1,
        Excluded(&n)=>n,
        Unbounded=>u64::BITS as usize,
    };

    assert!(0<=a as i64 && a<=b && b<=u64::BITS as usize);

    if a==u64::BITS as usize{
        0
    } else if b==u64::BITS as usize{
        0-(1<<a)
    } else{
        (1<<b)-(1<<a)
    }
}