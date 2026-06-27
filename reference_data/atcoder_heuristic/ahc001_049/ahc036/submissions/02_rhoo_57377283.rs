#![allow(non_snake_case,unexpected_cfgs)]


#[cfg(local)]
const DEBUG:bool=false;
#[cfg(not(local))]
const DEBUG:bool=false;

const TL:f64=2.9;



fn main(){
    get_time();

    let mut it=0;
    let mut best=(INF,vec![],vec![]);

    let input=get_input();
    let vs=get_vertex_set();
    let abs=|(x,y):(i64,i64)|x*x+y*y;

    let mut is=(0..vs.len()).collect_vec();
    is.sort_unstable_by_key(|&i|-abs(input.xy[vs[i]]));

    for &v in &is{
        let solve=move||{
            let mut a=make_list_greedy(v);
            hill_climb(&mut a,0.4); // todo
            adjust_list(&mut a);
            let ans=solve_bfs(&a);
            (a,ans)
        };

        if let Some((a,ans))=run_with_timeout(solve,TL-get_time()){
            let score=evaluate(&ans);
            if !DEBUG{
                eprintln!("# {score}");
            }
            if best.0>score{
                best=(score,a,ans);
            }
            it+=1;
        } else if get_time()>=TL{
            break;
        }

        // break; // todo
    }
    assert!(best.0!=INF);
    
    if DEBUG{
        output_debug(&best.1);
    } else{
        eprintln!();
        eprintln!("it = {it}");
        output(&best.1,&best.2);
    }
}



fn evaluate(ans:&[Act])->usize{
    ans.iter().filter(|&t|matches!(t,Change{..})).count()
}



#[derive(Clone)]
struct State{
    edge_score:&'static Mat<i64>,
    vertex_type:Vec<usize>,
    a:Vec<usize>,
    es:Mat<usize>,
    vs:Vec<usize>,
    cnt:[usize;3],
    old:i64,
    score:i64,
    inc_v:Vec<usize>,
    dec_v:Vec<usize>,
    inc_e:Vec<(usize,usize)>,
    dec_e:Vec<(usize,usize)>,
}
impl State{
    fn new(a:Vec<usize>)->State{
        let input=get_input();
        let g=get_graph();
        assert!(a.len()==input.La);

        let tour=get_tour();
        let vs=get_necessary_vertex();
        let mut vertex_type=vec![0;g.len()];
        for i in 0..g.len(){
            if vs[i]{
                vertex_type[i]=1;
            }
        }
        for &v in tour{
            vertex_type[v]=2;
        }

        let mut cnt=[0;3];
        for i in 0..g.len(){
            cnt[vertex_type[i]]+=1;
        }

        let mut state=State{
            vertex_type,
            edge_score:get_edge_score_table(),
            a:a.clone(),cnt,
            old:0,score:0,
            es:Mat::new(vec![vec![0;g.len()];g.len()]),
            vs:vec![0;g.len()],
            inc_v:vec![],
            dec_v:vec![],
            inc_e:vec![],
            dec_e:vec![],
        };
        for &v in &a{
            state.inc_vs(v);
        }
        
        for i in 0..a.len(){
            for j in i+1..(i+input.Lb).min(a.len()){
                state.inc_es(a[i],a[j]);
            }
        }
        state.debug();

        state
    }

    fn get_es(&self,u:usize,v:usize)->usize{
        self.es[u][v]
    }

    fn inc_es(&mut self,u:usize,v:usize){
        self.inc_e.push((u,v));
        if self.es[u][v]==0{
            self.score+=self.edge_score[u][v];
        }
        self.es[u][v]+=1;
        self.es[v][u]+=1;
    }

    fn dec_es(&mut self,u:usize,v:usize){
        self.dec_e.push((u,v));
        assert!(self.es[u][v]!=0);
        self.es[u][v]-=1;
        self.es[v][u]-=1;
        if self.es[u][v]==0{
            self.score-=self.edge_score[u][v];
        }
    }

    fn inc_vs(&mut self,v:usize){
        self.inc_v.push(v);
        if self.vs[v]==0{
            self.cnt[self.vertex_type[v]]-=1;
        }
        self.vs[v]+=1;
    }

    fn dec_vs(&mut self,v:usize){
        self.dec_v.push(v);
        assert!(self.vs[v]!=0);
        self.vs[v]-=1;
        if self.vs[v]==0{
            self.cnt[self.vertex_type[v]]+=1;
        }
    }

    fn get_score(&self)->i64{
        let g=get_graph();

        let mut score=0;
        for i in 0..g.len(){
            for j in 0..i{
                if self.get_es(i,j)>0{
                    score+=self.edge_score[i][j];
                }
            }
        }

        score
    }

    fn apply(&mut self,l:usize,r:usize,new:&[usize]){
        self.old=self.score;
        let Lb=get_input().Lb;

        self.inc_v.clear();
        self.dec_v.clear();
        self.inc_e.clear();
        self.dec_e.clear();
        
        for i in l..r{
            self.dec_vs(self.a[i]);
            for j in ((i-Lb+1) as i64).max(0) as usize..l{
                self.dec_es(self.a[i],self.a[j]);
            }
            for j in i+1..(i+Lb).min(self.a.len()){
                self.dec_es(self.a[i],self.a[j]);
            }
        }
        
        for i in (0..l).rev(){
            let range=r..(i+Lb).min(self.a.len());
            if range.is_empty(){
                break;
            }
            for j in range{
                self.dec_es(self.a[i],self.a[j]);
            }
        }

        for i in 0..new.len(){
            self.inc_vs(new[i]);
            for j in i+1..(i+Lb).min(new.len()){
                self.inc_es(new[i],new[j]);
            }
            for j in ((i-Lb+l+1) as i64).max(0) as usize..l{
                self.inc_es(new[i],self.a[j]);
            }
            let lim=i+Lb-new.len()+r;
            if 0<=lim as i64{
                for j in r..lim.min(self.a.len()){
                    self.inc_es(new[i],self.a[j]);
                }
            }
        }

        for i in (0..l).rev(){
            let lim=i+Lb+r-l-new.len();
            if (lim as i64)<0{
                break;
            }

            let range=r..lim.min(self.a.len());
            if range.is_empty(){
                break;
            }
            for j in range{
                self.inc_es(self.a[i],self.a[j]);
            }
        }
    }

    fn revert(&mut self){
        self.score=self.old;

        for &v in &self.inc_v{
            self.vs[v]-=1;
            if self.vs[v]==0{
                self.cnt[self.vertex_type[v]]+=1;
            }
        }
        for &v in &self.dec_v{
            if self.vs[v]==0{
                self.cnt[self.vertex_type[v]]-=1;
            }
            self.vs[v]+=1;
        }
        for &(u,v) in &self.inc_e{
            self.es[u][v]-=1;
            self.es[v][u]-=1;
        }
        for &(u,v) in &self.dec_e{
            self.es[u][v]+=1;
            self.es[v][u]+=1;
        }
    }

    fn update(&mut self,l:usize,r:usize,new:&[usize]){
        self.a.splice(l..r,new.iter().copied());
    }

    fn is_walk(&self)->bool{
        let g=get_graph();
        for w in self.a.windows(2){
            let (u,v)=(w[0],w[1]);
            if g[u].iter().find(|&x|*x==v).is_none(){
                return false;
            }
        }
        true
    }

    #[track_caller]
    fn debug(&self){
        let input=get_input();
        let g=get_graph();
        
        assert!(self.score==self.get_score());
        assert!(self.is_walk());

        let mut vs=vec![0;g.len()];
        for &v in &self.a{
            vs[v]+=1;
        }
        assert!(self.vs==vs);

        let mut es=Mat::new(vec![vec![0;g.len()];g.len()]);
        let mut inc=|a:usize,b:usize|{
            es[a][b]+=1;
            es[b][a]+=1;
        };

        for i in 0..self.a.len(){
            for j in i+1..(i+input.Lb).min(self.a.len()){
                inc(self.a[i],self.a[j]);
            }
        }

        assert!(self.es==es);

        let mut cnt=[0;3];
        for i in 0..g.len(){
            if self.vs[i]==0{
                cnt[self.vertex_type[i]]+=1;
            }
        }
    }

    fn to_score(&self)->i64{
        self.score
        -self.cnt[1] as i64*1000*4 // todo
        -self.cnt[2] as i64*1000
    }
}



struct PathFinder{
    La:usize,
    g:CSR<usize>,
    tour:&'static Vec<usize>,
    dist:&'static Mat<usize>,
    min:usize,
    max:usize,
    seen:FastSet,
    prev:Vec<usize>,
    que:Vec<VecDeque<(usize,usize)>>,
    route:Vec<usize>,
}
impl PathFinder{
    fn new(min:usize,max:usize)->Self{
        let input=get_input();
        let g=get_graph();
        let tour=get_tour();
        let dist=get_graph_distance();
        Self{
            La:input.La,g:g.clone(),tour,dist,min,max,
            seen:FastSet::new(g.len()),
            prev:vec![!0;g.len()],
            que:vec![VecDeque::new();max-min],
            route:vec![],
        }
    }

    fn clear(&mut self){
        self.route.clear();
    }

    fn rand(&self)->usize{
        rnd::range(self.min,self.max)
    }

    fn get_dist(&self,u:usize,v:usize)->usize{
        self.dist[u][v]
    }

    fn random_relay_point(&self)->usize{
        self.tour.choice()
    }

    fn dijkstra(&mut self,start:usize,goal:usize){
        self.seen.clear();
        self.seen.set(goal);
        self.prev[goal]=!0;
        for i in 0..self.que.len(){
            self.que[i].clear();
        }
        self.que[0].push_back((0,goal));

        'a: loop{
            let mut min=INF;
            let mut arg=!0;
            for i in 0..self.que.len(){
                if self.que[i].is_empty(){
                    continue;
                }
                if min>self.que[i][0].0{
                    min=self.que[i][0].0;
                    arg=i;
                }
            }
            
            let (d,v)=self.que[arg].pop_front().unwrap();
            for &nv in &self.g[v]{
                if self.seen.get(nv){
                    continue;
                }
                self.prev[nv]=v;
                if nv==start{
                    break 'a;
                }
                self.seen.set(nv);
                let w=self.rand();
                self.que[w-self.min].push_back((d+w,nv));
            }
        }
    
        let mut v=start;
        loop{
            v=self.prev[v];
            if v==!0{
                self.route.pop().unwrap();
                break;
            }
            self.route.push(v);
        }
    }

    fn bfs(&mut self,start:usize,goal:usize){
        let mut v=start;
        let dist=&self.dist[goal];
        
        loop{
            let mut nv=!0;
            let g=&mut self.g[v];
            for i in (0..g.len()).rev(){
                g.swap(i,rnd::get(i+1));
                nv=g[i];
                if dist[v]>dist[nv]{
                    break;
                }
            }
            if nv==goal{
                break;
            }
            self.route.push(nv);
            v=nv;
        }
    }

    // start,goalは追加されない
    fn find(&mut self,start:usize,goal:usize){
        assert!(start!=goal);
        if rnd::next()&3==0{ // todo
            self.dijkstra(start,goal);
        } else{
            self.bfs(start,goal);
        }
    }
}



fn trans(state:&mut State,add:f64,find:&mut PathFinder)->bool{
    // スコアの最大化ならadd<=diffで更新
    find.clear();
    let dist=|u,v|find.get_dist(u,v);

    let len=rnd::range(5,20); // todo
    assert!(state.a.len()>len);
    let l=rnd::get(state.a.len()-len+1);
    let r=l+len;
    let m=find.random_relay_point();

    if l==0{
        if state.a.len()-len+dist(m,state.a[r])>find.La{
            return false;
        }
        if m==state.a[r]{
            return false;
        }

        find.route.push(m);
        find.find(m,state.a[r]);

    } else if r==state.a.len(){
        if state.a.len()-len+dist(m,state.a[l-1])>find.La{
            return false;
        }
        if state.a[l-1]==m{
            return false;
        }

        find.find(state.a[l-1],m);
        find.route.push(m);

    } else if rnd::get(100)<=50{ // todo
        if state.a.len()-len+dist(m,state.a[l-1])+dist(m,state.a[r])>find.La{
            return false;
        }
        if state.a[l-1]==m || m==state.a[r]{
            return false;
        }

        find.find(state.a[l-1],m);
        find.route.push(m);
        find.find(m,state.a[r]);

    } else{
        if state.a[l-1]==state.a[r]{
            return false;
        }
        
        find.find(state.a[l-1],state.a[r]);
    }

    let old_score=state.to_score();
    let new_len=state.a.len()-len+find.route.len();
    if find.La<new_len || new_len<100{
        return false;
    }

    state.apply(l,r,&find.route);
    if add<=(state.to_score()-old_score) as f64{
        state.update(l,r,&find.route);
        true
    } else{
        state.revert();
        false
    }
}



fn hill_climb(a:&mut Vec<usize>,tl:f64){
    let mut state=State::new(a.clone());
    let mut find=PathFinder::new(3,7); // todo

    let tl=get_time()+tl;
    let mut _valid=0;
    let mut iter=0;

    while iter&255!=0 || get_time()<=tl{
        iter+=1;
        let add=0.;
        _valid+=trans(&mut state,add,&mut find) as usize;
    }

    state.debug();
    // eprintln!("{}",state.to_score());
    // eprintln!("iter = {}",iter);
    // eprintln!("ratio = {:.3}",_valid as f64/iter as f64);

    *a=state.a;
}



fn greedy_walk(a:&mut Vec<usize>,v:usize,w:&[usize]){
    let g=get_graph();

    let start=v;
    let goal=*a.last().unwrap();
    let mut que=BinaryHeap::new();
    que.push((Rev(0),0,start));
    let mut seen=vec![false;g.len()];
    seen[start]=true;
    let mut prev=vec![!0;g.len()];
    
    while let Some((Rev(d),bonus,v))=que.pop(){
        if v==goal{
            break;
        }

        for &nv in &g[v]{
            if !seen[nv]{
                seen[nv]=true;
                prev[nv]=v;
                que.push((Rev(d+1),bonus+w[nv],nv));
            }
        }
    }

    let mut v=goal;
    loop{
        v=prev[v];
        if v==!0{
            break;
        }
        a.push(v);
    }
}



fn make_list_greedy(start:usize)->Vec<usize>{
    let input=get_input();
    let tour=get_tour();
    
    let mut ans=vec![start];
    while ans.len()<input.La{
        let dist=get_distance_table_with_list(&ans);
        let nv=tour.iter().copied().max_by_key(|&v|dist[v]).unwrap();
        greedy_walk(&mut ans,nv,&dist);
    }
    ans.truncate(input.La);

    ans
}



static_data!{
    get_is_tour:Vec<bool>|{
        let g=get_graph();
        let mut is_tour=vec![false;g.len()];
        for &v in get_tour(){
            is_tour[v]=true;
        }
        is_tour
    }
}



static_data!{
    get_tour_graph:Vec<Vec<usize>>|{
        let g=get_graph();
        let mut tour_graph=vec![vec![];g.len()];
        let tour=get_tour();

        for (&a,&b) in tour.iter().tuple_windows(){
            tour_graph[a].push(b);
            tour_graph[b].push(a);
        }

        tour_graph
    }
}



fn adjust_list(a:&mut Vec<usize>){
    let input=get_input();
    let g=get_graph();

    for len in 2..=4{ // todo
        let mut i=0;
        let mut j=len;
        let mut combo=0;
        while j<a.len(){
            let f=a[i]==a[j] && combo<input.Lb;
            if f{
                combo+=1;
                a.remove(j);
                i-=1;
            }
            if i==!0 || !f{
                combo=0;
                j+=1;
                i=j-len;
            }
        }
    }
    
    let tour=get_tour();
    let dist=get_graph_distance();
    let mut is_tour=vec![false;g.len()];
    for &v in tour{
        is_tour[v]=true;
    }
    
    let mut fixed=vec![false;g.len()];
    let vs={
        let mut vs=chain!(a.iter(),tour).copied().collect_vec();
        vs.sort_unstable();
        vs.dedup();
        prim_steiner(g,dist,&vs)
    };
    for &v in &vs{
        fixed[v]=true;
    }

    let is_connected=|fixed:&[bool]|{
        let mut uf=ac_library::Dsu::new(g.len());
        for v in 0..g.len(){
            for &nv in &g[v]{
                if fixed[v] && fixed[nv]{
                    uf.merge(v,nv);
                }
            }
        }
        
        tour.iter().map(|&v|uf.leader(v)).all_equal()
    };

    assert!(is_connected(&fixed));

    let mut cnt=vec![0;g.len()];
    for &v in a.iter(){
        cnt[v]+=1;
    }
    let mut rest=0;
    for i in 0..g.len(){
        if fixed[i] && cnt[i]==0{
            rest+=1;
        }
    }

    while rest+a.len()>input.La{
        let v=a.pop().unwrap();
        cnt[v]-=1;
        if cnt[v]==0{
            fixed[v]=false;
            if is_tour[v] || !is_connected(&fixed){
                rest+=1;
                fixed[v]=true;
            }
        }
    }

    let vs=(0..g.len()).filter(|&i|cnt[i]!=0).collect_vec();
    let is_fixed=|i|cnt[i]==0 && fixed[i];
    let mut is=(0..g.len()).filter(|&i|is_fixed(i)).collect_vec();
    is.sort_by_cached_key(|&i|vs.iter().map(|&v|dist[i][v]).min().unwrap());

    let is_tour=get_is_tour();
    let mut tour_graph=get_tour_graph().clone();

    for i in 0..g.len(){
        if is_fixed(i) && !is_tour[i]{
            let mut uf=ac_library::Dsu::new(g.len());
            for u in 0..g.len(){
                for &v in &g[u]{
                    if u==i || v==i{
                        continue;
                    }
                    if is_fixed(u) && is_fixed(v){
                        uf.merge(u,v);
                    }
                }
            }

            let min=tour.iter().map(|&v|uf.size(v)).min().unwrap();
            for v in 0..g.len(){
                if is_tour[v] && uf.size(v)==min{
                    tour_graph[i].push(v);
                }
            }
        }
    }

    let mut cand=vec![];
    for _ in 0..2{ // todo
        for &i in &is{
            if let Some(idx)=(0..a.len()).find(|&j|a[j]==i){
                a.remove(idx);
            }
            
            cand.clear();
            let mut min=INF;
            for idx in 0..a.len()-1{
                let d0=dist[i][a[idx]];
                let d1=dist[i][a[idx+1]];
                if min>d0.min(d1){
                    min=d0.min(d1);
                    cand.clear();
                }
                if d0.min(d1)==min{
                    cand.push(idx+1);
                }
            }

            let idx=cand.iter().copied().min_by_key(|&idx|{
                let mut mini=INF;
                let mut sum=0;
                for &v in &tour_graph[i]{
                    let mut min=INF;
                    for i in 0..a.len(){
                        if a[i]==v{
                            min=min.min(idx.abs_diff(i));
                        }
                    }
                    if min!=INF{
                        mini=mini.min(min);
                        sum+=min;
                    }
                }

                (mini,sum,rnd::next())
            }).unwrap();

            a.insert(idx,i);
        }
    }
    
    while a.len()<input.La{
        let dist=get_distance_table_with_list(&a);
        let nv=tour.iter().copied().max_by_key(|&v|dist[v]).unwrap();
        greedy_walk(a,nv,&dist);
    }
    a.truncate(input.La);

    let mut seen=vec![false;g.len()];
    for &v in a.iter(){
        seen[v]=true;
    }
    for i in 0..g.len(){
        if is_fixed(i){
            assert!(seen[i]);
        }
    }

    assert!(a.len()==input.La);
}



fn get_distance_table_with_list(a:&[usize])->Vec<usize>{
    let input=get_input();
    let graph=get_graph();
    
    let mut g=vec![vec![];graph.len()+a.len()];
    let mut add_edge=|u:usize,v:usize|{
        g[u].push(v);
        g[v].push(u);
    };
    for v in 0..graph.len(){
        for &nv in &graph[v]{
            if v<nv{
                add_edge(v,nv);
            }
        }
    }

    for (i,w) in a.windows(input.Lb).enumerate(){
        let v=i+graph.len();
        for &nv in w{
            add_edge(v,nv);
        }
    }

    let start=*a.last().unwrap();
    let mut dist=distance_table(&g,start);
    dist.truncate(graph.len());
    dist
}



static_data!{
    get_edge_score_table:Mat<i64>|{
        let input=get_input();
        let g=get_graph();
        let tour=get_tour();
        
        let mut a=Mat::new(vec![vec![0;g.len()];g.len()]);
        let mut add=|u:usize,v:usize,add:i64|{
            if u==v{
                return;
            }
            a[u][v]+=add;
            a[v][u]+=add;
        };

        let mut prev=vec![!0;g.len()];
        let mut seen=FastSet::new(g.len());
        let mut que=VecDeque::new();
        let mut path=|start:usize,goal:usize|->Vec<usize>{
            seen.clear();
            seen.set(start);
            que.clear();
            que.push_back(start);
            prev[start]=!0;

            'a: while let Some(v)=que.pop_front(){
                for &nv in &g[v]{
                    if !seen.get(nv){
                        prev[nv]=v;
                        if nv==goal{
                            break 'a;
                        }
                        seen.set(nv);
                        que.push_back(nv);
                    }
                }
            }

            let mut route=vec![];
            let mut v=goal;
            while v!=!0{
                route.push(v);
                v=prev[v];
            }
            route.reverse();

            route
        };

        let min=4.;
        let max=22.;
        let step=((max-min)*(input.Lb as f64-4.)/20.+min).round() as usize;

        for w in tour.windows(2){
            let route=path(w[0],w[1]);

            let mut i=0;
            while i<route.len(){
                let a=i;
                let b=(i+step-1).min(route.len()-1);
                add(route[a],route[b],1000);
                i+=step;
            }
        }
        
        a
    }
}



fn solve_bfs(a:&[usize])->Vec<Act>{
    let input=get_input();
    let cg=get_graph();
    let corr=get_vertex_correspond();

    let init_ok=|ok:&mut FastSet,a:&[usize]|{
        ok.clear();
        for &v in a{
            if v!=!0{
                ok.set(v);
            }
        }
    };
    
    let mut reach=vec![(vec![],vec![],FixedBitSet::with_capacity(cg.len()));a.len()];
    let mut ok=FastSet::new(cg.len());
    let mut seen=FastSet::new(cg.len());

    let mut get_reachable=|ok:&FastSet,v:usize|->(Vec<usize>,Vec<usize>){
        assert!(ok.get(v));
        let mut all=vec![v];
        let mut que=vec![v];
        let mut i=0;

        seen.clear();
        seen.set(v);

        while let Some(&v)=que.get(i){
            i+=1;
            for &nv in &cg[v]{
                if !seen.get(nv){
                    all.push(nv);
                    seen.set(nv);
                    if ok.get(nv){
                        que.push(nv);
                    }
                }
            }
        }
        
        (all,que)
    };

    const KRANGE:usize=2; // todo
    let mut min_range=vec![vec![[(0,INF);KRANGE];cg.len()];cg.len()];
    for len in 1..=input.Lb{
        for r in len..=a.len(){
            let l=r-len;
            init_ok(&mut ok,&a[l..r]);
            let (all,set)=get_reachable(&ok,a[l]);
            
            for &u in &all{
                for &v in &set{
                    for t in 0..KRANGE{
                        if min_range[u][v][t]==(0,INF){
                            min_range[u][v][t]=(l,r);
                            break;
                        } else{
                            let (a,b)=min_range[u][v][t];
                            assert!(a<=b && l<=r);
                            if !(b<=l || r<=a){
                                break;
                            }
                            assert!((a,b)!=(l,r));
                        }
                    }
                }
            }
        }
    }

    for idx in 0..a.len(){
        init_ok(&mut ok,&a[idx..(idx+input.Lb).min(input.La)]);
        let (all,set,exist)=&mut reach[idx];
        (*all,*set)=get_reachable(&ok,a[idx]);

        for &v in set.iter(){
            exist.put(v);
        }
    }

    let mut g=vec![vec![];cg.len()+a.len()];
    let sub_node=|v|v+cg.len();
    let is_sub_node=|v|cg.len()<=v;
    let restore=|v|{
        assert!(is_sub_node(v));
        v-cg.len()
    };

    for i in 0..a.len(){
        for &v in &reach[i].0{
            g[sub_node(i)].push(v);
        }
        for &v in &reach[i].1{
            g[v].push(sub_node(i));
        }
    }

    let tour=&get_tour()[1..];

    let mut que=VecDeque::new();
    let V=g.len()*(tour.len()+1);

    let mut dist=vec![INF;V+1];
    let mut next=vec![!0;V+1];
    let node=|v:usize,turn:usize|{
        assert!(v<g.len());
        turn*g.len()+v
    };

    for v in 0..g.len(){
        que.push_back((v,tour.len()));
        let v=node(v,tour.len());
        dist[v]=0;
    }

    while let Some((v,turn))=que.pop_front(){
        let d=dist[node(v,turn)];
        for &nv in &g[v]{
            let mut t=turn;
            
            if is_sub_node(nv){
                let idx=restore(nv);
                while 0<t && reach[idx].2.contains(tour[t-1]){
                    t-=1;
                }
            }

            for t in t..=turn{
                if dist[node(nv,t)]==INF{
                    dist[node(nv,t)]=d+is_sub_node(nv) as usize;
                    next[node(nv,t)]=node(v,turn);
                    que.push_back((nv,t));
                }
            }
        }
    }

    let next_state=|v:usize,turn:usize|->(usize,(usize,usize)){ // (e,(nv,nt))
        let s=next[node(v,turn)];
        let nv=next[s];
        (restore(s%g.len()),(nv%g.len(),nv/g.len()))
    };

    let get_reachable_fast=|ok:&FastSet,v:usize,set:&mut IndexSet|{
        set.clear();
        set.add(v);
        let mut i=0;

        while let Some(&v)=set.to_slice().get(i){
            i+=1;
            for &nv in &cg[v]{
                if ok.get(nv) && !set.contains(nv){
                    set.add(nv);
                }
            }
        }
    };

    let mut set=IndexSet::new(cg.len());
    let mut mini_change=|node:&Node,idx:usize,a:&[usize],a_idx:usize,cand:&mut Vec<Node>,track:&mut Track<H>|{
        let mut na=node.a;
        na[idx..idx+a.len()].copy_from_slice(a);
        init_ok(&mut ok,&na[..input.Lb]);
        get_reachable_fast(&ok,node.v,&mut set);

        let mut turn=node.turn;
        let mut first=true;
        while turn<tour.len() && set.contains(tour[turn]){
            if first{
                first=false;
                get_reachable_fast(&ok,tour[turn],&mut set);
            }
            turn+=1;
        }
        assert!(turn!=node.turn);

        for i in 0..input.Lb{
            // if (idx..idx+a.len()).contains(&i){
            //     continue;
            // }
            let v=node.a[i];
            if v==!0{
                break;
            }
            if set.contains(v){
                let id=track.push(node.id,H{len:a.len(),a:a_idx,b:idx,mov:v,turn});
                let new=Node{
                    id,v,turn,a:na,
                    hash:0,score:0,
                };
                cand.push(new);
            }
        }
    };

    let mut ok=FastSet::new(cg.len());
    let mut set=IndexSet::new(cg.len());
    let mut append_cands=|node:&Node,cand:&mut Vec<Node>,track:&mut Track<H>|{
        let (e,(_nv,nt))=next_state(node.v,node.turn);
        
        let mut na=node.a;
        let f=(e+input.Lb).min(a.len());
        na[..f-e].copy_from_slice(&a[e..f]);

        init_ok(&mut ok,&na[..input.Lb]);
        let cv=if node.turn==nt{
            node.v
        } else{
            tour[node.turn]
        };
        get_reachable_fast(&ok,cv,&mut set);
        for &nv in set.to_slice(){
            let id=track.push(node.id,H{len:f-e,a:e,b:0,mov:nv,turn:nt});
            let new=Node{
                id,v:nv,turn:nt,a:na,
                hash:0,score:0,
            };
            cand.push(new);
        }

        if node.turn==nt{
            for i in 0..KRANGE{
                let (l,r)=min_range[node.v][_nv][i];
                if (e..e+input.Lb).contains(&l) || (l,r)==(0,INF){
                    continue;
                }
                let e=l;

                let mut na=node.a;
                let f=(e+input.Lb).min(a.len());
                na[..f-e].copy_from_slice(&a[e..f]);
        
                init_ok(&mut ok,&na[..input.Lb]);
                get_reachable_fast(&ok,node.v,&mut set);
                for &nv in set.to_slice(){
                    let id=track.push(node.id,H{len:f-e,a:e,b:0,mov:nv,turn:node.turn});
                    let new=Node{
                        id,v:nv,turn:nt,a:na,
                        hash:0,score:0,
                    };
                    cand.push(new);
                }
            }
        }
        
        let nv=tour[node.turn];
        for i in 0..KRANGE{
            let (l,r)=min_range[node.v][nv][i];
            let len=r-l;
            
            if len>=input.Lb{
                return;
            }
            let sa=&a[l..r];
            
            mini_change(node,0,sa,l,cand,track);
            mini_change(node,input.Lb-len,sa,l,cand,track);
            // if input.Lb-len>1{
            //     mini_change(node,(input.Lb-len)/2,sa,l,cand,track);
            // }
        }
    };

    let zob=get_zobrist();
    let set_hash=|n:&mut Node|{
        n.hash=0;
        for &a in &n.a[..input.Lb]{
            n.hash+=zob[a+1];
        }
    };
    let set_score=|n:&mut Node|{
        n.score=dist[node(n.v,n.turn)];
    };

    let mut cur={
        let node=Node{
            id:!0,
            v:corr[0],
            turn:0,
            a:[!0;24],
            hash:0,
            score:100,
        };
        vec![node]
    };
    let mut next=vec![];
    let mut hashset=FxHashSet::default();
    let mut track=Track::new();

    let BEAM_WIDTH=50; // todo
    let res;
    loop{
        for node in &cur{
            append_cands(node,&mut next,&mut track);
        }
        for node in &mut next{
            set_hash(node);
            set_score(node);
        }

        next.sort_unstable_by_key(|node|node.score);
        
        if next[0].score==0{
            res=track.restore(next[0].id);
            break;
        }

        hashset.clear();
        cur.clear();
        'a: for node in next.drain(..){
            if hashset.insert((node.v,node.hash)){ // todo
                cur.push(node);
                if cur.len()>BEAM_WIDTH{
                    break 'a;
                }
            }
        }
    }

    let mut que=VecDeque::new();
    let mut prev=vec![!0;cg.len()];
    let mut seen=FastSet::new(cg.len());
    let mut path=|goal:usize,start:usize,ok:&FastSet,ans:&mut Vec<Act>|{
        assert!(goal<cg.len() && start<cg.len());
        if goal==start{
            return;
        }
        assert!(ok.get(start));

        que.clear();
        prev[start]=!0;
        seen.clear();
        seen.set(start);
        que.push_back(start);

        let mut reach=false;
        'a: while let Some(v)=que.pop_front(){
            for &nv in &cg[v]{
                if !seen.get(nv){
                    if nv==goal{
                        prev[nv]=v;
                        reach=true;
                        break 'a;
                    }
                    seen.set(nv);
                    if ok.get(nv){
                        prev[nv]=v;
                        que.push_back(nv);
                    }
                }
            }
        }
        assert!(reach);
        assert!(prev[start]==!0);

        let mut v=goal;
        loop{
            v=prev[v];
            if v==!0{
                break;
            }
            ans.push(Move{v});
        }
    };

    let mut ans=vec![];
    let mut turn=0;
    let mut cur=corr[0];
    let mut arr=vec![!0;input.Lb];
    for &H{len,a:ai,b:bi,mov,turn:nt} in &res{
        ans.push(Change{len,a:ai,b:bi});
        arr[bi..bi+len].copy_from_slice(&a[ai..ai+len]);
        init_ok(&mut ok,&arr);
        get_reachable_fast(&ok,cur,&mut set);
        
        let mut first=true;
        while turn<nt && set.contains(tour[turn]){
            assert!(set.contains(tour[turn]));
            path(cur,tour[turn],&ok,&mut ans);
            cur=tour[turn];
            if first{
                first=false;
                get_reachable_fast(&ok,cur,&mut set);
            }
            turn+=1;
        }
        assert!(set.contains(mov));
        
        path(cur,mov,&ok,&mut ans);
        cur=mov;
    }

    ans
}



#[derive(Clone,Default)]
struct Node{
    id:usize,
    v:usize,
    turn:usize,
    a:[usize;24],
    hash:u64,
    score:usize,
}


#[derive(Clone,Copy)]
struct H{
    len:usize,
    a:usize,
    b:usize,
    mov:usize,
    turn:usize,
}


static_data!{
    get_vertex_correspond:Vec<usize>|{
        let vs=get_vertex_set();
        let mut a=vec![!0;N];
        for i in 0..vs.len(){
            a[vs[i]]=i;
        }
        a
    }
}


static_data!{
    get_tour:Vec<usize>|{
        let t=&get_input().t;
        let corr=get_vertex_correspond();
        t.iter().map(|&v|corr[v]).collect_vec()
    }
}



static_data!{
    get_graph:CSR<usize>|{
        let g=&get_input().g;
        let mut exist=[false;N];
        let vs=get_vertex_set();
        for &v in vs{
            exist[v]=true;
        }

        let corr=get_vertex_correspond();
        let mut new=vec![vec![];vs.len()];
        for i in 0..vs.len(){
            for &nv in &g[vs[i]]{
                if exist[nv]{
                    new[i].push(corr[nv]);
                }
            }
        }
        for i in 0..vs.len(){
            new[i].sort_unstable();
        }

        CSR::new(new)
    }
}


static_data!{
    get_graph_distance:Mat<usize>|{
        let g=get_graph();
        Mat::new((0..g.len()).map(|i|distance_table_csr(&g,i)).collect_vec())
    }
}



// todo: SPTを差分更新すれば高速化できるが、
static_data!{
    get_vertex_set:Vec<usize>|{
        // return (0..N).collect_vec();
        
        let input=get_input();
    
        let mut set={
            let mut t=input.t.clone();
            t.sort_unstable();
            t.dedup();
            prim_steiner(&input.g,get_original_distance(),&t)
        };
        let mut ok=[false;N];
        let mut g=vec![vec![];N];
        for i in 0..set.len(){
            ok[set[i]]=true;
            add_edge(&ok,&mut g,set[i]);
        }

        let mut cur=input.t.windows(2).map(|w|{
            let (u,v)=(w[0],w[1]);
            distance_table(&g,u)[v]
        }).collect_vec();

        let t=(input.La-N) as f64/N as f64;
        let min=40.;
        let max=45.;
        let mut i=((max-min)*t+min).round() as i64; // todo
        while i>=0{
            let mut max_score=0;
            let mut best=(!0,!0);
            let mut new=vec![!0;N];

            for v in 0..N{
                if ok[v]{
                    continue;
                }
                
                add_edge(&ok,&mut g,v);
                let dist=distance_table(&g,v);

                let mut score=0;
                let mut next=vec![!0;N];

                for j in 0..input.t.len()-1{
                    let (u,v)=(input.t[j],input.t[j+1]);
                    next[j]=cur[j].min(dist[u]+dist[v]);
                    score+=cur[j]-next[j];
                }
                
                score*=2; // todo
                if max_score<score{
                    max_score=score;
                    best=(v,!0);
                    new=next;
                }

                ok[v]=true;
                for &u in &input.g[v]{
                    if ok[u] || u>v{
                        continue;
                    }

                    add_edge(&ok,&mut g,u);

                    let dist=distance_table(&g,v);

                    let mut score=0;
                    let mut next=vec![!0;N];
    
                    for j in 0..input.t.len()-1{
                        let (u,v)=(input.t[j],input.t[j+1]);
                        next[j]=cur[j].min(dist[u]+dist[v]);
                        score+=cur[j]-next[j];
                    }
                    
                    if max_score<score{
                        max_score=score;
                        best=(u,v);
                        new=next;
                    }
                    
                    rem_edge(&mut g,u);
                }
                ok[v]=false;
                
                rem_edge(&mut g,v);
            }

            if max_score==0{
                break;
            }

            cur=new;
            for v in [best.0,best.1]{
                if v==!0{
                    continue;
                }
                i-=1;
                set.push(v);
                ok[v]=true;
                add_edge(&ok,&mut g,v);
            }
        }
    
        set
    }
}



static_data!{
    get_necessary_vertex:Vec<bool>|{
        let g=get_graph();
        let set={
            let mut t=get_tour().clone();
            t.sort_unstable();
            t.dedup();
            prim_steiner(g,get_graph_distance(),&t)
        };

        let mut a=vec![false;g.len()];
        for &v in &set{
            a[v]=true;
        }
        a
    }
}



fn add_edge(ok:&[bool],g:&mut Vec<Vec<usize>>,v:usize){
    let input=&get_input();
    assert!(g[v].is_empty());
    for &nv in &input.g[v]{
        if ok[nv]{
            g[v].push(nv);
            g[nv].push(v);
        }
    }
}

fn rem_edge(g:&mut Vec<Vec<usize>>,v:usize){
    for i in 0..g[v].len(){
        let nv=g[v][i];
        let i=(0..g[nv].len()).find(|&i|g[nv][i]==v).unwrap();
        g[nv].swap_remove(i);
    }
    g[v].clear();
}



fn prim_steiner(g:&CSR<usize>,dist:&Mat<usize>,vs:&[usize])->Vec<usize>{
    let mut ret=vec![];
    let mut vs=vs.iter().map(|&v|(v,(INF,!0))).collect_vec();

    while let Some(i)=(0..vs.len()).min_by_key(|&i|vs[i].1){
        let (u,(_,mut v))=vs.swap_remove(i);
        let mut update=|v:usize|{
            ret.push(v);
            for i in 0..vs.len(){
                let d=dist[v][vs[i].0];
                if vs[i].1.0>d{
                    vs[i].1=(d,v);
                }
            }
        };
        
        if v==!0{
            update(u);
            continue;
        }
        
        while v!=u{
            'a: for &nv in &g[v]{
                if dist[u][v]>dist[u][nv]{
                    v=nv;
                    continue 'a;
                }
            }

            update(v);
        }
    }
    
    ret
}



fn output(a:&[usize],ans:&[Act]){
    let vs=get_vertex_set();
    for &v in a{
        print!("{} ",vs[v]);
    }
    print!("\n");

    for i in 0..ans.len(){
        let mut a=ans[i];
        if let Move{v}=&mut a{
            *v=vs[*v];
        }
        a.print();
    }

    let score=evaluate(&ans);
    eprintln!("score = {score}");
    eprintln!("time = {:.2}",get_time());
}



fn output_debug(a:&[usize]){
    let input=get_input();
    let vs=get_vertex_set();
    let g=get_graph();

    let get=|v:usize|(v+g.len()-a[0])%g.len();

    let add_edge=|es:&mut HashSet<_>,mut a,mut b|{
        if a>b{
            (a,b)=(b,a);
        }
        es.insert((a,b));
    };
    
    let mut es=HashSet::new();
    for a in 0..g.len(){
        for &b in &g[a]{
            add_edge(&mut es,a,b);
        }
    }

    let valid=es.clone();
    let is_valid=move|mut a,mut b|{
        if a>b{
            (a,b)=(b,a);
        }
        valid.contains(&(a,b))
    };

    for w in a.windows(2){
        add_edge(&mut es,w[0],w[1]);
    }

    eprint!("{} {} {} {} {}\n",g.len(),es.len(),0,input.La,1);
    for &(a,b) in &es{
        eprint!("{} {}\n",get(a),get(b));
    }

    let mut pos=vec![(0,0);g.len()];
    for i in 0..g.len(){
        pos[get(i)]=input.xy[vs[i]];
    }

    for &(x,y) in &pos{
        eprint!("{} {}\n",x+500,y+500);
    }

    for &v in a{
        print!("{} ",get(v));
    }
    print!("\n");
    for i in 1..a.len(){
        if !is_valid(a[i-1],a[i]){
            print!("#INVALID!!!\n");
        }
        Change{len:1,a:i,b:0}.print();

        if !is_valid(a[i-1],a[i]){
            print!("#INVALID!!!\n");
        }
        Move{v:get(a[i])}.print();
    }
}



#[derive(Clone,Copy)]
enum Act{
    Change{
        len:usize,
        a:usize,
        b:usize,
    },
    Move{
        v:usize,
    }
}
use Act::*;
impl Act{
    fn print(&self){
        match *self{
            Change{len,a,b}=>{
                print!("s {len} {a} {b}\n");
            },
            Move{v}=>{
                print!("m {v}\n");
            },
        }
    }
}



#[derive(Clone)]
struct FastSet{
    a:Vec<usize>,
    at:usize,
}
impl FastSet{
    fn new(n:usize)->FastSet{
        FastSet{
            a:vec![0;n],
            at:1,
        }
    }

    fn clear(&mut self){
        self.at+=1;
    }
    
    fn get(&self,i:usize)->bool{
        self.a[i]==self.at
    }
    
    fn set(&mut self,i:usize){
        self.a[i]=self.at;
    }
}


use std::iter::*;
use itertools::*;
use std::collections::*;
use std::cmp::Reverse as Rev;
use fixedbitset::FixedBitSet;
use rustc_hash::FxHashSet;
const INF:usize=usize::MAX/8;


const N:usize=600;
struct Input{
    La:usize,
    Lb:usize,
    g:CSR<usize>,
    t:Vec<usize>,
    xy:Vec<(i64,i64)>,
}


static_data!{
    get_input:Input|{
        proconio::input!{
            n:usize,
            M:usize,
            T:usize,
            La:usize,
            Lb:usize,
            es:[(usize,usize);M],
            mut t:[usize;n],
            mut xy:[(i64,i64);n],
        }
        assert!((n,T)==(N,N));

        let mut g=vec![vec![];N];
        for &(u,v) in &es{
            g[u].push(v);
            g[v].push(u);
        }
        for i in 0..N{
            g[i].sort_unstable();
        }
        t.insert(0,0);
        for i in 0..n{
            xy[i].0-=500;
            xy[i].1-=500;
        }
        let g=CSR::new(g);

        Input{
            La,Lb,g,t,xy,
        }
    }
}



static_data!{
    get_original_distance:Mat<usize>|{
        let g=&get_input().g;
        Mat::new((0..N).map(|i|distance_table_csr(&g,i)).collect_vec())
    }
}



static_data!{
    get_zobrist:Vec<u64>|{
        (0..=N).map(|_|rnd::next64()).collect_vec()
    }
}



fn distance_table(g:&Vec<Vec<usize>>,v:usize)->Vec<usize>{
    let mut d=vec![INF;g.len()];
    d[v]=0;
    let mut que=VecDeque::new();
    que.push_back(v);
    while let Some(v)=que.pop_front(){
        for &nv in &g[v]{
            if d[nv]==INF{
                d[nv]=d[v]+1;
                que.push_back(nv);
            }
        }
    }
    d
}



fn distance_table_csr(g:&CSR<usize>,v:usize)->Vec<usize>{
    let mut d=vec![INF;g.len()];
    d[v]=0;
    let mut que=VecDeque::new();
    que.push_back(v);
    while let Some(v)=que.pop_front(){
        for &nv in &g[v]{
            if d[nv]==INF{
                d[nv]=d[v]+1;
                que.push_back(nv);
            }
        }
    }
    d
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



#[derive(Clone)]
struct IndexSet{
    pos:Vec<usize>,
    que:Vec<usize>,
    len:usize,
}
#[allow(unused)]
impl IndexSet{
    fn new(n:usize)->Self{
        IndexSet{
            pos:(0..n).collect(),
            que:(0..n).collect(),
            len:0,
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
        self.len-=1;
        self.que.swap(p,self.len);
        self.pos.swap(a,self.que[p]);
    }

    fn to_slice(&self)->&[usize]{
        &self.que[..self.len]
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





struct Track<T>(Vec<(usize,T)>);
impl<T> Track<T>{
    fn new()->Self{
        Track(vec![])
    }

    fn push(&mut self,prev:usize,v:T)->usize{
        self.0.push((prev,v));
        self.0.len()-1
    }

    fn restore(&self,mut idx:usize)->Vec<T> where T:Clone{
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



// https://github.com/MiSawa/ralgo/blob/e806f94598c8b619b27f62833adb2312d2699a2a/src/utils/alloc.rs
use std::alloc::{GlobalAlloc, System};

pub struct LeakyAlloc<A: GlobalAlloc = System>(pub A);
impl LeakyAlloc<System> {
    pub const fn system() -> Self {
        Self(System)
    }
}

unsafe impl<A: GlobalAlloc> GlobalAlloc for LeakyAlloc<A> {
    unsafe fn alloc(&self, layout: std::alloc::Layout) -> *mut u8 {
        self.0.alloc(layout)
    }

    unsafe fn dealloc(&self, _ptr: *mut u8, _layout: std::alloc::Layout) {
        // leak!
    }

    unsafe fn alloc_zeroed(&self, layout: std::alloc::Layout) -> *mut u8 {
        self.0.alloc_zeroed(layout)
    }

    unsafe fn realloc(&self, ptr: *mut u8, layout: std::alloc::Layout, new_size: usize) -> *mut u8 {
        self.0.realloc(ptr, layout, new_size)
    }
}


#[global_allocator]
static GLOBAL_ALLOC: LeakyAlloc = LeakyAlloc::system();



#[derive(Clone,PartialEq)]
struct CSR<T>{
    n:usize,
    sum:Vec<usize>,
    a:Vec<T>,
}
impl<T:Copy> CSR<T>{
    fn new(a:Vec<Vec<T>>)->Self{
        let n=a.len();
        let mut sum=vec![0;n+1];
        for i in 0..n{
            sum[i+1]=sum[i]+a[i].len();
        }

        Self{
            n,sum,
            a:a.into_iter().flatten().collect()
        }
    }

    fn len(&self)->usize{
        self.n
    }
}
impl<T> std::ops::Index<usize> for CSR<T>{
    type Output=[T];
    fn index(&self,a:usize)->&[T]{
        &self.a[self.sum[a]..self.sum[a+1]]
    }
}
impl<T> std::ops::IndexMut<usize> for CSR<T>{
    fn index_mut(&mut self,a:usize)->&mut [T]{
        &mut self.a[self.sum[a]..self.sum[a+1]]
    }
}



#[derive(Clone,PartialEq)]
struct Mat<T>{
    m:usize,
    a:Vec<T>,
}
impl<T> Mat<T>{
    fn new(a:Vec<Vec<T>>)->Self{
        let m=a[0].len();
        assert!(a.iter().all(|t|t.len()==m));

        Self{
            m,
            a:a.into_iter().flatten().collect()
        }
    }
}
impl<T> std::ops::Index<usize> for Mat<T>{
    type Output=[T];
    fn index(&self,a:usize)->&[T]{
        let idx=a*self.m;
        &self.a[idx..idx+self.m]
    }
}
impl<T> std::ops::IndexMut<usize> for Mat<T>{
    fn index_mut(&mut self,a:usize)->&mut [T]{
        let idx=a*self.m;
        &mut self.a[idx..idx+self.m]
    }
}



fn run_with_timeout<F:Send+FnOnce()->T+'static,T:Send+'static>(f:F,tl:f64)->Option<T>{
    if tl<0.{
        return None;
    }
    let (tx,rx)=std::sync::mpsc::channel();
    std::thread::spawn(move||tx.send(f()));
    rx.recv_timeout(std::time::Duration::from_secs_f64(tl)).ok()
}




#[macro_export]#[cfg(not(local))]macro_rules! eprint{($($_:tt)*)=>{}}
#[macro_export]#[cfg(not(local))]macro_rules! eprintln{($($_:tt)*)=>{}}
#[macro_export]#[cfg(not(local))]macro_rules! assert{($($_:tt)*)=>{}}
#[macro_export]#[cfg(not(local))]macro_rules! assert_eq{($($_:tt)*)=>{}}
#[macro_export]#[cfg(not(local))]macro_rules! assert_ne{($($_:tt)*)=>{}}


