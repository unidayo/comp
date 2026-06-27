#![allow(non_snake_case)]



fn run(){
    get_time();
    let input=Input::read();

    let mut best=vec![(NULLP,!0);input.k];
    let mut best_score=0;
    loop{
        let (prev,mut graph)=make_tree_sa(&input);
        let Some(ans)=solve(&input,&prev,&mut graph) else {break};
        let mut score=0;
        for i in 0..input.k{
            if ans[i].0!=NULLP{
                let (a,b)=input.crop[i];
                score+=b-a+1;
            }
        }

        if best_score<score{
            best_score=score;
            best=ans;
        }
    }
    input.write(&best);
    eprintln!("time = {}",get_time());
}



fn solve(input:&Input,prev:&[usize;N*N],graph:&mut Vec<Vec<usize>>)->Option<Vec<(P,usize)>>{
    let mut rest=[[0;T];T];
    for &(a,b) in &input.crop{
        rest[a][b]+=1;
    }

    let mut size=[0;N*N];
    get_tree_size(&graph,to_id(input.root),&mut size);

    let mut leaf=(0..N*N).filter(|&i|graph[i].is_empty()).collect_vec();
    fn dfs(graph:&Vec<Vec<usize>>,p:usize,num:&mut [usize],it:&mut usize){
        for &np in &graph[p]{
            dfs(graph,np,num,it);
        }
        num[p]=*it;
        *it+=1;
    }
    for i in 0..N*N{
        // todo: ここのソートのやつを色々頑張る
        graph[i].sort_unstable_by_key(|&t|size[t]);
    }
    let mut num=[0;N*N];
    dfs(graph,to_id(input.root),&mut num,&mut 0);

    leaf.sort_unstable_by_key(|&t|!num[t]);
    let mut route=make_route(prev,&leaf);

    let mut dfs=DFS::new();
    let mut ans:Vec<Vec<Vec<(usize,usize)>>>=vec![vec![vec![];T];T];
    let mut reserve=[[0u128;N];N];

    for route in &mut route{
        if get_time()>=1.8{
            return None;
        }
        
        // ここで少し余分に探索して先読み
        let len=route.len();
        
        let mut p=*route.last().unwrap();
        for _ in 0..15{ // todo
            let np=prev[to_id(p)];
            if np==!0{
                break;
            }
            p=P::new(np/N,np%N);
            route.push(p);
        }
        
        let mut split=vec![vec![];route.len()];
        let mut pbit=0u128;
        for (i,&p) in route.iter().enumerate(){
            let mut bit=0u128;
            for &np in &graph[to_id(p)]{
                bit|=reserve[np/N][np%N];
            }
            let mut new=bit&!pbit;
            while 0<new{
                let t=new.trailing_zeros();
                new^=1<<t;
                split[i].push(t as usize);
            }
            pbit=bit;
        }
        
        let (_,res)=dfs.solve(&mut rest,0,99,route.len(),split.clone());

        for &(s,t,n) in &res{
            for &x in &split[n]{
                assert!(!(s<x && x<=t));
            }
            if n<len{
                ans[route[n]].push((s,t));
                reserve[route[n]]|=1<<s;
            } else{
                rest[s][t]+=1;
            }
        }
        for &p in route[..len].iter(){
            for &np in &graph[to_id(p)]{
                reserve[p]|=reserve[np/N][np%N];
            }
        }
    }

    let mut ids=vec![vec![vec![];T];T];
    for (i,&(a,b)) in input.crop.iter().enumerate(){
        ids[a][b].push(i);
    }

    let mut ret=vec![(NULLP,!0);input.k];
    for p in iterp(N){
        for i in 0..ans[p].len(){
            let (s,t)=ans[p][i];
            let id=ids[s][t].pop().unwrap();
            assert!(reserve[p]&get_mask(s+1,t+1)==0);
            let bit=reserve[p]&!get_mask(0,t+1);
            ret[id]=(p,bit.trailing_zeros().min(100) as usize-1);
        }
    }

    Some(ret)
}



fn make_route(prev:&[usize;N*N],leaf:&[usize])->Vec<Vec<P>>{
    let mut deg=[0;N*N];
    for &p in prev{
        if p!=!0{
            deg[p]+=1;
        }
    }

    let mut ret=vec![];
    for &p in leaf{
        let mut p=p;
        let mut t=vec![];
        while deg[p]==0{
            t.push(P::new(p/N,p%N));
            p=prev[p];
            if p!=!0{
                deg[p]-=1;
            } else{
                break;
            }
        }
        ret.push(t);
    }
    assert!(ret.iter().map(|t|t.len()).sum::<usize>()==N*N);

    ret
}



const MAX_DEPTH:usize=400;
const WASTE_COEFF:i64=100000; // todo


struct DFS{
    var:u16,
    len_score:[i64;T+1],
    split_head:Vec<Vec<usize>>, // [n][i] := n日目の必須分割でiより大きい最小の数
    dp:[[[(u16,i64);MAX_DEPTH];T];T],
    prev:[[[[(u16,u16,u16);2];MAX_DEPTH];T];T],
}
impl DFS{
    fn new()->DFS{
        let mut len_score=[0;T+1];
        for i in 0..T{
            len_score[i]=(i*i) as i64;
        }
        DFS{
            var:1,
            len_score,
            split_head:vec![],
            dp:[[[(0,0);MAX_DEPTH];T];T],
            prev:[[[[(!0,!0,!0);2];MAX_DEPTH];T];T],
        }
    }

    fn init(&mut self,mut split:Vec<Vec<usize>>){
        split.reverse();
        self.split_head=vec![vec![];split.len()];
        for i in 0..split.len(){
            split[i].sort_unstable();
            split[i].push(!0/4);
            split[i].dedup();
            for t in 0..T{
                self.split_head[i].push(split[i][split[i].partition_point(|&a|a<=t)]);
            }
        }
    }
    
    // (s,t,n,e) := (s,t)をn番目の場所で栽培しeで収穫する
    fn solve(&mut self,rest:&mut [[u8;T];T],s:usize,t:usize,n:usize,split:Vec<Vec<usize>>)->(i64,Vec<(usize,usize,usize)>){
        timer!();
        self.init(split);
        self.var+=1;
        let a=self.get(rest,s,t,n);
        let mut ret=vec![];
        self.restore_dfs(rest,s,t,n,&mut ret);
        for i in 0..ret.len(){
            ret[i].2=n-ret[i].2;
        }

        let mut dbg_a=((t-s+1)*n) as i64;
        let mut dbg_b=0;
        for &(a,b,_) in &ret{
            dbg_a-=(b-a+1) as i64;
            dbg_b-=self.len_score[b-a+1];
        }
        assert!(dbg_a*WASTE_COEFF+dbg_b==a);

        (a as i64,ret)
    }

    fn restore_dfs(&mut self,rest:&mut [[u8;T];T],s:usize,t:usize,n:usize,ret:&mut Vec<(usize,usize,usize)>){
        let mut nn=n;
        for i in 0..(rest[s][t] as usize).min(n){
            let d=self.split_head[nn-1][s];
            assert!(s<d);
            if d<=t{
                break;
            }
            ret.push((s,t,n-i as usize));
            assert!(0<rest[s][t]);
            rest[s][t]-=1;
            nn-=1;
        }

        let next=self.prev[s][t][n];
        if next[1].0!=!0{
            let (a,b,c)=next[0];
            self.restore_dfs(rest,a as usize,b as usize,c as usize,ret);
            let (a,b,c)=next[1];
            self.restore_dfs(rest,a as usize,b as usize,c as usize,ret);
        }
        else if next[0].0!=!0{
            let (a,b,c)=next[0];
            if next[1].2==0{
                let mut i=0;
                'a: for j in (s..t).rev(){
                    for _ in 0..rest[s][j]{
                        ret.push((s,j,nn-i));
                        i+=1;
                        assert!(0<rest[s][j]);
                        rest[s][j]-=1;
                        if i==next[1].1 as usize{
                            break 'a;
                        }
                    }
                }
                assert!(nn-i==c as usize);
            }
            self.restore_dfs(rest,a as usize,b as usize,c as usize,ret);
        }
    }

    fn get(&mut self,rest:&[[u8;T];T],s:usize,t:usize,n:usize)->i64{
        assert!(s<=t,"{} {}",s,t);
        let (a,b)=self.dp[s][t][n];
        if a==self.var{
            b
        } else{
            let b=self.dfs(rest,s,t,n);
            self.dp[s][t][n]=(self.var,b);
            b
        }
    }

    fn set(&mut self,s:usize,t:usize,n:usize,set:&[(usize,usize,usize);2]){
        self.prev[s][t][n]=[(set[0].0 as u16,set[0].1 as u16,set[0].2 as u16),(set[1].0 as u16,set[1].1 as u16,set[1].2 as u16)];
    }
    
    fn dfs(&mut self,rest:&[[u8;T];T],s:usize,t:usize,mut n:usize)->i64{
        if n==0{
            self.set(s,t,n,&[(!0,!0,!0);2]);
            return 0;
        }
        if s==t{
            self.set(s,t,n,&[(!0,!0,!0);2]);
            return n as i64*WASTE_COEFF;
        }
        
        let len_score=self.len_score[t-s+1];
        let tn=n;
        let mut add=0;

        for _ in 0..rest[s][t] as usize{
            let d=self.split_head[n-1][s];
            assert!(s<d);
            if d<=t{
                let res0=self.get(rest,s,d-1,n);
                let res1=self.get(rest,d,t,n);
                self.set(s,t,tn,&[(s,d-1,n),(d,t,n)]);
                return res0+res1+add;
            }
            add-=len_score;
            n-=1;
            if n==0{
                self.set(s,t,tn,&[(!0,!0,!0);2]);
                return add;
            }
        }

        let d=self.split_head[n-1][s];
        assert!(s<d);
        if d<=t{
            let res0=self.get(rest,s,d-1,n);
            let res1=self.get(rest,d,t,n);
            self.set(s,t,tn,&[(s,d-1,n),(d,t,n)]);
            return res0+res1+add;
        }

        if s+1==t{
            self.set(s,t,tn,&[(!0,!0,!0);2]);
            return 2*n as i64*WASTE_COEFF+add;
        }
        
        let mut ret=INF;

        let mut i=0;
        let mut sum=0;
        let mut sum_len_score=add;
        'a: for j in (s..t).rev(){
            for _ in 0..rest[s][j]{
                i+=1;
                sum+=t-j;
                sum_len_score-=self.len_score[j-s+1];
                // この枝刈り怪しい
                if ret<=((n-i) as i64+sum as i64)*WASTE_COEFF+sum_len_score{
                    break 'a;
                }
                let v=self.get(rest,s+1,t,n-i);
                let new=v+((n-i) as i64+sum as i64)*WASTE_COEFF+sum_len_score;
                if ret>new{
                    ret=new;
                    self.set(s,t,tn,&[(s+1,t,n-i),(!0,i,0)]);
                }
                if i==n || self.split_head[n-i-1][s]<=t{
                    break 'a;
                }
            }
        }
        
        for d in s+1..t-1{
            let v0=self.get(rest,s,d,n)+add;
            if ret<=v0{
                continue;
            }
            let v1=self.get(rest,d+1,t,n);
            let new=v0+v1;
            if ret>new{
                ret=new;
                self.set(s,t,tn,&[(s,d,n),(d+1,t,n)]);
            }
        }
        let v=self.get(rest,s+1,t,n);
        let new=v+n as i64*WASTE_COEFF+add;
        if ret>new{
            ret=new;
            self.set(s,t,tn,&[(s+1,t,n),(!0,!0,!0)]);
        }
        if ret<=n as i64*WASTE_COEFF{
            return ret;
        }
        let v=self.get(rest,s,t-1,n);
        let new=v+n as i64*WASTE_COEFF+add;
        if ret>new{
            ret=new;
            self.set(s,t,tn,&[(s,t-1,n),(!0,!0,!0)]);
        }
        
        ret
    }
}



fn to_id(p:P)->usize{
    p.0 as usize*N+p.1 as usize
}


fn make_tree_sa(input:&Input)->([usize;N*N],Vec<Vec<usize>>){
    let state=State::new(input);
    let mut solver=SA::new(state,get_time()+0.05);
    solver.solve(input);
    (solver.state.prev,solver.state.get_graph())
}



fn get_tree_size(graph:&Vec<Vec<usize>>,p:usize,size:&mut [usize]){
    size[p]=1;
    for &np in &graph[p]{
        get_tree_size(graph,np,size);
        size[p]+=size[np];
    }
}


// 部分木頂点数Nでどれくらいスコアが得られるのかを埋め込み
const SCORE_TABLE:[f64;N*N+1]=[0.,94.68933054393305,94.66274456149952,94.63698854337152,94.33223276929427,94.3406496927129,93.69562780269058,92.43065693430657,90.6794425087108,86.75796178343948,82.54285714285714,72.6,81.85984848484848,67.96666666666667,72.96551724137932,69.8703071672355,62.236486486486484,55.403225806451616,63.674698795180724,47.52173913043478,56.439490445859875,51.4320987654321,43.44444444444444,38.77777777777778,50.467532467532465,44.80113636363637,41.470588235294116,36.333333333333336,39.87755102040816,31.58823529411765,36.79333333333334,30.24264705882353,29.59259259259259,17.375,21.761904761904763,28.83783783783784,21.379310344827587,21.482758620689655,19.05128205128205,9.0,13.74074074074074,17.333333333333332,20.210526315789473,13.444444444444445,14.52808988764045,13.5,9.166666666666666,8.166666666666666,13.369565217391305,13.447368421052632,6.9655172413793105,8.923076923076923,8.0,9.714285714285714,9.935483870967742,7.957894736842105,8.175438596491228,4.0,6.0,3.5714285714285716,6.19672131147541,2.6923076923076925,3.032258064516129,5.809523809523809,3.9655172413793105,3.857142857142857,3.831168831168831,3.25,3.8,2.4210526315789473,2.4571428571428573,1.0,3.7,1.1111111111111112,1.25,1.2608695652173914,0.6,1.7777777777777777,2.076923076923077,0.9473684210526315,1.1785714285714286,0.5517241379310345,1.25,0.0,1.8235294117647058,0.4166666666666667,0.6666666666666666,0.21428571428571427,1.2727272727272727,0.5,0.3793103448275862,0.25,0.21428571428571427,0.7619047619047619,0.0,0.0,0.24324324324324326,0.0,0.47058823529411764,0.0,0.1875,0.5714285714285714,0.25,0.0,0.4666666666666667,0.4375,0.0,0.0,0.21428571428571427,0.42857142857142855,0.0,0.0,0.0,0.0,0.0,0.0,0.6363636363636364,0.0,0.0,0.0,0.17391304347826086,0.0,0.0,0.0,0.0,0.5,0.0,0.3333333333333333,0.0,0.0,0.23076923076923078,0.0,0.0,0.0,0.5,0.23076923076923078,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.16666666666666666,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.,0.0,];


#[derive(Clone)]
struct State{
    edge:Vec<Vec<usize>>,
    cand:Vec<usize>,
    root:usize,
    prev:[usize;N*N],
    size:[usize;N*N],
    tmp:Vec<(usize,usize)>,
}
impl State{
    fn new(input:&Input)->State{
        let mut edge=vec![vec![];N*N];
        for p in iterp(N){
            for i in 0..4{
                let np=p+DD[i];
                if !input.is_wall[p][i] && input.dist[p]>input.dist[np]{
                    edge[to_id(p)].push(to_id(np));
                }
            }
        }

        let root=to_id(input.root);
        assert!(edge[root].is_empty());

        let mut prev=[!0;N*N];
        let mut cand=vec![];
        for i in 0..N*N{
            if edge[i].len()>1{
                cand.push(i);
            }
            if i!=root{
                prev[i]=*edge[i].choice();
            }
        }

        let mut ret=State{edge,cand,root,prev,size:[0;N*N],tmp:vec![]};
        let graph=ret.get_graph();
        get_tree_size(&graph,root,&mut ret.size);

        ret
    }

    fn get_graph(&self)->Vec<Vec<usize>>{
        let mut graph=vec![vec![];N*N];
        for i in 0..N*N{
            if i!=self.root{
                graph[self.prev[i]].push(i);
            }
        }
        graph
    }
    
    fn score(&self)->f64{
        (0..N*N).map(|i|SCORE_TABLE[self.size[i]]).sum()
    }

    #[track_caller]
    fn debug(&self){
        let graph=self.get_graph();
        let mut size=[0;N*N];
        get_tree_size(&graph,self.root,&mut size);
        for i in 0..N*N{
            assert!(0<size[i]);
            if size[i]!=self.size[i]{
                eprintln!("{} {} {}",i,size[i],self.size[i]);
            }
            assert!(size[i]==self.size[i]);

            if self.prev[i]!=!0{
                let p=P::new(i/N,i%N);
                let np=P::new(self.prev[i]/N,self.prev[i]%N);
                assert!(DD.iter().any(|&dd|dd+p==np));
            }
        }
    }
}


fn trans(state:&mut State,iter:usize,add:f64,score:&mut f64)->bool{
    let p=*state.cand.choice();
    let cur=state.prev[p];
    let mut new;
    loop{
        new=*state.edge[p].choice();
        if cur!=new{
            break;
        }
    }

    let sub=state.size[p];

    state.tmp.clear();
    let mut a=cur;
    let mut b=new;
    let mut diff=0.;
    while a!=b{
        diff-=SCORE_TABLE[state.size[a]]+SCORE_TABLE[state.size[b]];
        state.size[a]-=sub;
        state.size[b]+=sub;
        diff+=SCORE_TABLE[state.size[a]]+SCORE_TABLE[state.size[b]];
        state.tmp.push((a,0-sub));
        state.tmp.push((b,sub));
        a=state.prev[a];
        b=state.prev[b];
    }

    if add<=diff{
        state.prev[p]=new;
        *score+=diff;
        true
    } else{
        for &(n,add) in &state.tmp{
            state.size[n]-=add;
        }
        false
    }
}


fn local_search_tree(input:&Input,state:&mut State,score:&mut f64){
    for _ in 0..10{
        for i in 0..N*N{
            if state.size[i]>1{
                continue;
            }
            // 現在繋がっていないやつの中で一番サイズが大きいやつにつなげる
            let cur=state.prev[i];
            let mut new=NULLP;
            let mut best=0;
            for j in 0..4{
                let np=P::new(i/N,i%N)+DD[j];
                let score=rnd::next();
                if !input.is_wall[i/N][i%N][j] && best<score{
                    best=score;
                    new=np;
                }
            }
            if new==NULLP{
                continue;
            }

            state.tmp.clear();
            let mut a=cur;
            let mut b=to_id(new);
            let mut diff=0.;
            while a!=!0{
                diff-=SCORE_TABLE[state.size[a]];
                state.size[a]-=1;
                diff+=SCORE_TABLE[state.size[a]];
                state.tmp.push((a,!0));
                a=state.prev[a];
            }

            while b!=!0{
                diff-=SCORE_TABLE[state.size[b]];
                state.size[b]+=1;
                diff+=SCORE_TABLE[state.size[b]];
                state.tmp.push((b,1));
                b=state.prev[b];
            }

            if 0.<=diff{
                state.prev[i]=to_id(new);
                let p=P::new(i/N,i%N);
                let np=new;
                assert!(DD.iter().any(|&dd|dd+p==np));
                *score+=diff;
            } else{
                for &(n,add) in &state.tmp{
                    state.size[n]-=add;
                }
            }
        }
    }
}



struct SA{
    tl:f64,
    time:f64,
    start:f64,
    heat:f64,
    table:[f64;65536],
    state:State,
}
impl SA{
    fn new(state:State,tl:f64)->SA{
        let start=get_time();
        assert!(start<tl);

        let mut table=[0.;65536];
        for i in 1..=65536{
            table[i-1]=(i as f64/65536.).ln(); // 最大化
        }

        SA{
            start,
            tl:tl-start,
            time:0.,
            heat:0.,
            table,
            state,
        }
    }
    
    fn update(&mut self)->bool{
        self.time=(get_time()-self.start)/self.tl;
        if self.time>=1.{
            return false;
        }

        // todo
        const T0:f64=5e1;
        const T1:f64=1e-3;
        self.heat=T0*(T1/T0).powf(self.time.powf(1.5)); // todo

        true
    }
    
    fn solve(&mut self,input:&Input){
        let mut valid=0;
        let mut score=self.state.score();
        let mut best_score=score;
        let mut iter=0;
        
        while iter&255!=0 || self.update(){
            iter+=1;
            let add=self.heat*self.table[rnd::next()&65535];
            // let add=0.;
            valid+=trans(&mut self.state,iter,add,&mut score) as usize;
            best_score=best_score.max(score);
        }

        assert!((self.state.score()-score).abs()<=1e-5);
        self.state.debug();

        local_search_tree(input,&mut self.state,&mut score);

        assert!((self.state.score()-score).abs()<=1e-5);
        self.state.debug();

        // eprintln!("best = {}",best_score);
        // eprintln!("last = {}",score);
        // eprintln!("iter = {}",iter);
        // eprintln!("ratio = {:.3}",valid as f64/iter as f64);
    }
}



use std::collections::*;
use std::cmp::*;
use itertools::*;
use proconio::{*,marker::*};

const INF:i64=std::i64::MAX/4;
const NULLP:P=P(INF,INF);


const T:usize=100;
const N:usize=20;


struct Input{
    is_wall:[[[bool;4];N];N],
    root:P,
    input_crop:Vec<(usize,usize)>,
    k:usize,
    crop:Vec<(usize,usize)>,
    dist:[[i64;N];N],
}
impl Input{
    // 注意：s_iに植えてt_i以降に刈り取る問題に変換してる
    fn read()->Input{
        input!{
            t:usize,
            h:usize,
            w:usize,
            i0:usize,
            wall_i:[Chars;h-1],
            wall_j:[Chars;h],
            k:usize,
            input_crop:[(Usize1,Usize1);k],
        }
        assert!((t,h,w)==(T,N,N));
        let mut is_wall=[[[true;4];N];N];

        for i in 0..h-1{
            for j in 0..w{
                let f=wall_i[i][j]=='1';
                is_wall[i][j][3]=f;
                is_wall[i+1][j][1]=f;
            }
        }
        for i in 0..h{
            for j in 0..w-1{
                let f=wall_j[i][j]=='1';
                is_wall[i][j][2]=f;
                is_wall[i][j+1][0]=f;
            }
        }

        let crop=input_crop.iter().map(|&(a,b)|(T-1-b,T-1-a)).collect_vec();
        let mut degree=[[0;N];N];
        for i in 0..N{
            for j in 0..N{
                for k in 0..4{
                    degree[i][j]+=is_wall[i][j][k] as u8;
                }
            }
        }

        let root=P(i0 as i64,0);
        
        let mut dist=[[INF;N];N];
        let mut que=VecDeque::new();
        que.push_back(root);
        dist[root]=0;
        while let Some(p)=que.pop_front(){
            for i in 0..4{
                let np=p+DD[i];
                if !is_wall[p][i] && dist[np]==INF{
                    dist[np]=dist[p]+1;
                    que.push_back(np);
                }
            }
        }

        Input{
            is_wall,root,
            k,input_crop,crop,
            dist,
        }
    }

    fn get_event(&self,ans:&[(P,usize)])->Vec<(Vec<usize>,Vec<usize>)>{
        let mut event=vec![(vec![],vec![]);T];
        for (i,&(p,t)) in ans.iter().enumerate(){
            if p!=NULLP{
                event[self.crop[i].0].0.push(i);
                event[t].1.push(i);
                assert!(self.crop[i].1<=t);
            }
        }
        event
    }

    // #[track_caller]
    fn debug(&self,ans:&[(P,usize)]){
        let mut grid=[[!0;N];N];
        let event=self.get_event(ans);

        macro_rules! bfs{
            ($id:expr)=>{{
                    let mut seen=[[false;N];N];
                    let mut que=vec![self.root];
                    seen[self.root]=true;
                    // assert!(grid[self.root]==!0);
                    while let Some(p)=que.pop(){
                        for (i,dd) in DDI{
                            let np=p+dd;
                            if !self.is_wall[p][i] && !seen[np] && (grid[np]==!0 || $id[grid[np]]){
                                seen[np]=true;
                                que.push(np);
                            }
                        }
                    }
                    
                    seen
            }}
        }

        for i in 0..T{
            let mut id=vec![false;self.k];
            let res=bfs!(&id);
            for &n in &event[i].0{
                assert!(res[ans[n].0]);
                assert!(grid[ans[n].0]==!0);
                grid[ans[n].0]=n;
            }

            for &n in &event[i].1{
                id[n]=true;
            }
            let res=bfs!(id);
            for &n in &event[i].1{
                assert!(res[ans[n].0]);
                assert!(grid[ans[n].0]!=!0);
                grid[ans[n].0]=!0;
            }
        }
    }

    // 植える場所、刈り取る時間
    fn write(&self,ans:&[(P,usize)]){
        self.debug(ans); // tood: ちゃんとデバッグする
        
        let m=ans.iter().filter(|t|t.0!=NULLP).count();
        println!("{}",m);

        let mut score=0;
        let mut waste=0;
        for (i,&(p,t)) in ans.iter().enumerate(){
            if p!=NULLP{
                assert!(t!=!0);
                let (a,b)=self.input_crop[i];
                score+=b-a+1;
                waste+=a-(T-1-t);
                println!("{} {} {} {}",i+1,p.0,p.1,T-t);
            }
        }

        eprintln!("waste = {}",waste);
        eprintln!("score = {}",score/4);
    }
}



#[macro_export]#[cfg(not(local))]macro_rules! eprint{($($_:tt)*)=>{}}
#[macro_export]#[cfg(not(local))]macro_rules! eprintln{($($_:tt)*)=>{}}
#[macro_export]#[cfg(not(local))]macro_rules! assert{($($_:tt)*)=>{}}
#[macro_export]#[cfg(not(local))]macro_rules! assert_eq{($($_:tt)*)=>{}}
#[macro_export]#[cfg(not(local))]macro_rules! assert_ne{($($_:tt)*)=>{}}



#[allow(unused)]
fn get_time()->f64{
    static mut START:f64=-1.;
    let time=std::time::SystemTime::now().duration_since(std::time::UNIX_EPOCH).unwrap().as_secs_f64();
    unsafe{
        if START<0.{
            START=time;
        }

        // #[cfg(local)]{
        //     (time-START)*1.6
        // }
        // #[cfg(not(local))]{
            time-START
        // }
    }
}


#[macro_export]
macro_rules! timer{
    ()=>{
        let _timer=Timer(get_time());
    }
}

static mut TIME:f64=0.;
struct Timer(f64);
impl Drop for Timer{
    fn drop(&mut self){
        unsafe{
            TIME+=get_time()-self.0
        }
    }
}


#[allow(unused)]
mod rnd {
    static mut N:usize=0xcafef00dd15ea5e5;

    // 注意: 上の方のbitが0になってる
    pub fn next()->usize{
        unsafe{
            let x=N;
            N*=6364136223846793005;
            (x^x>>22)>>(22+(x>>61))
        }
    }

    pub fn hash()->usize{
        unsafe{
            let x=N;
            N*=6364136223846793005;
            x
        }
    }

    pub fn nextf()->f64{
        unsafe{
            std::mem::transmute::<u64,f64>(0x3ff0000000000000|(next() as u32 as u64)<<20)-1.
        }
    }

    pub fn range(a:usize,b:usize)->usize{
        assert!(a<b);
        next()%(b-a)+a
    }

    pub fn range_skip(a:usize,b:usize,skip:usize)->usize{
        assert!(a<=skip && skip<b);
        let ret=range(a,b-1);
        ret+(skip<=ret) as usize
    }

    pub fn rangei(a:i64,b:i64)->i64{
        assert!(a<b);
        (next()%((b-a) as usize)) as i64+a
    }

    pub fn shuffle<T>(list:&mut [T]){
        for i in (0..list.len()).rev(){
            list.swap(i,next()%(i+1));
        }
    }
}


trait RandomChoice{
    type Output;
    fn choice(&self)->&Self::Output;
}
impl<T> RandomChoice for [T]{
    type Output=T;
    fn choice(&self)->&T{
        &self[rnd::next() as usize%self.len()]
    }
}




#[derive(Clone,Copy,PartialEq,Eq,Debug,Default,Hash,PartialOrd,Ord)]
struct P(i64,i64);
impl P{
    fn new(a:usize,b:usize)->P{
        P(a as i64,b as i64)
    }
}
use std::ops::*;
impl Add for P{
    type Output=P;
    fn add(self,a:P)->P{
        P(self.0+a.0,self.1+a.1)
    }
}
impl Sub for P{
    type Output=P;
    fn sub(self,a:P)->P{
        P(self.0-a.0,self.1-a.1)
    }
}
impl Mul<i64> for P{
    type Output=P;
    fn mul(self,a:i64)->P{
        P(self.0*a,self.1*a)
    }
}
impl Div<i64> for P{
    type Output=P;
    fn div(self,a:i64)->P{
        P(self.0/a,self.1/a)
    }
}
impl Neg for P{
    type Output=P;
    fn neg(self)->P{
        P(-self.0,-self.1)
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
impl<T:Index<usize>> Index<P> for Vec<T>{
    type Output=T::Output;
    fn index(&self,idx:P)->&T::Output{
        &self[idx.0 as usize][idx.1 as usize]
    }
}
impl<T:IndexMut<usize>> IndexMut<P> for Vec<T>{
    fn index_mut(&mut self,idx:P)->&mut T::Output{
        &mut self[idx.0 as usize][idx.1 as usize]
    }
}
impl<T:Index<usize>> Index<P> for [T]{
    type Output=T::Output;
    fn index(&self,idx:P)->&T::Output{
        &self[idx.0 as usize][idx.1 as usize]
    }
}
impl<T:IndexMut<usize>> IndexMut<P> for [T]{
    fn index_mut(&mut self,idx:P)->&mut T::Output{
        &mut self[idx.0 as usize][idx.1 as usize]
    }
}


const DD:[P;4]=[P(0,-1),P(-1,0),P(0,1),P(1,0)];
const DDI:[(usize,P);4]=[(0,DD[0]),(1,DD[1]),(2,DD[2]),(3,DD[3])];


fn iterp(n:usize)->impl Iterator<Item=P>{
    iproduct!(0..n,0..n).map(|(i,j)|P(i as i64,j as i64))
}


fn main(){
    let _=std::thread::Builder::new().stack_size(256*1024*1024).spawn(run).unwrap().join();
}



fn get_mask(a:usize,b:usize)->u128{
    (1<<b-a)-1<<a
}
