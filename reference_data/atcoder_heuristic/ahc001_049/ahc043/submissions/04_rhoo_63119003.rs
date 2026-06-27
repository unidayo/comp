#![allow(non_snake_case,unexpected_cfgs)]



fn main(){
    get_time();
    get_start_candidates();
    eprintln!("#TIME {:.2}",get_time());

    let mut state=State::new();
    solve(&mut state);

    state.eval();
    let es=state.get_es();
    let close=state.close;
    let ord=state.ord;

    eprintln!("iter = {:.1}",unsafe{ITER as f64/1e4});
    eprintln!("ratio = {:.2}",unsafe{VALID as f64/ITER as f64});
    
    let ans=restore(ord,close,&es);
    for &act in &ans{
        println!("{act}");
    }
}



fn solve(state:&mut State){
    let tl=2.85;
    let time=get_time();
    
    unsafe{
        EVAL_TURN=(T as f64*0.5).round() as usize; // todo
        EVAL_TYPE=Income;
    }

    anneal(state,time+(tl-time)*0.1,30.,5.);

    unsafe{
        EVAL_TURN=T;
        EVAL_TYPE=Money;
    }
    state.score=state.eval();

    anneal(state,tl,5e3,2e2);
    eprintln!("#{:.1}",state.score as f64/1e3);
    
    hill_climb(state,2.9);
    eprintln!("#{:.1}",state.score as f64/1e3);
    
    unsafe{
        EVAL_TYPE=NoCross;
    }
    state.score=state.eval();
    eprintln!("#{:.1}",state.score as f64/1e3);
    hill_climb(state,2.95);
    eprintln!("#{:.1}",state.score as f64/1e3);
    eprintln!();
}



fn trans(state:&mut State,add:f64)->bool{
    // state.debug();
    let a=[0,1,2]; // todo

    let ord=&mut state.ord;
    let new=&mut state.new;
    new.clear();
    let close=&mut state.new_close;
    close.clear();

    if state.score==0 || ord.len()<=1{
        let cands=get_start_candidates();
        let (ps,income,money)=&cands[rnd::get(cands.len())];
        let new=if unsafe{EVAL_TYPE}==Money{
            *money
        } else{
            *income
        };

        if add>(new-state.score) as f64{
            return false;
        }

        for i in 0..state.ord.len(){
            state.rem(state.ord[i]);
        }

        state.ord.clear();
        state.close.clear();

        for &p in ps{
            state.add(p);
            state.ord.push(p);
            state.close.push(NULL);
        }

        state.score=new;

        return true;
    }

    match a.choice(){
        0=>{
            if rnd::next()&15==0{
                let cands=get_start_candidates();
                let (ps,_,_)=&cands[rnd::get(cands.len())];
                let ps=&ps[..rnd::range(2,ps.len()+1)]; // todo

                new.extend(ps);

                close.extend((0..ps.len()).map(|_|NULL));
                'a: for i in 0..ord.len(){
                    let mut minp=state.close[i];
                    for &p in ps{
                        if p==ord[i]{
                            continue 'a;
                        }
                        if minp!=NULL && ord[i].manh(p)<ord[i].manh(minp){
                            minp=p;
                        }
                    }
                    new.push(ord[i]);
                    close.push(minp);
                }

                mem::swap(&mut state.ord,&mut state.new);
                mem::swap(&mut state.close,&mut state.new_close);
                let new=state.eval();

                if add<=(new-state.score) as f64{
                    state.score=new;
                    for &p in ps{
                        if !state.a[p]{
                            state.add(p);
                        }
                    }

                    true
                } else{
                    mem::swap(&mut state.ord,&mut state.new);
                    mem::swap(&mut state.close,&mut state.new_close);

                    false
                }
            } else if rnd::next()&63==0 && state.not.len!=0{
                let people=state.not.to_slice().choice();
                let mut p0=get_people_neighbor()[people*2].choice();
                let mut p1=get_people_neighbor()[people*2+1].choice();

                if p0==p1{
                    return false;
                }
                if rnd::next()&1==0{
                    (p0,p1)=(p1,p0);
                }

                assert!(!state.a[p0] && !state.a[p1]);

                let i=rnd::get(ord.len()+1);

                new.extend(&ord[..i]);
                new.push(p0);
                new.push(p1);
                new.extend(&ord[i..]);

                close.extend(&state.close[..i]);
                close.push(NULL);
                close.push(NULL);
                for j in i..ord.len(){
                    let mut minp=state.close[j];

                    if minp!=NULL && ord[j].manh(p0)<ord[j].manh(minp){
                        minp=p0;
                    }
                    if minp!=NULL && ord[j].manh(p1)<ord[j].manh(minp){
                        minp=p1;
                    }
                    
                    close.push(minp);
                }

                mem::swap(&mut state.ord,&mut state.new);
                mem::swap(&mut state.close,&mut state.new_close);
                let new=state.eval();

                if add<=(new-state.score) as f64{
                    state.score=new;
                    state.add(p0);
                    state.add(p1);

                    true
                } else{
                    mem::swap(&mut state.ord,&mut state.new);
                    mem::swap(&mut state.close,&mut state.new_close);

                    false
                }

            } else if rnd::next()&1==0 && state.set.len!=0{
                // 追加
                let p={
                    let a=state.set.to_slice();
                    // let people=a.choice().min(a.choice()); // todo
                    let people=a.choice();
                    get_people_neighbor()[people].choice()
                };
                assert!(!state.a[p]);

                let i=rnd::get(ord.len()+1);

                new.extend(&ord[..i]);
                new.push(p);
                new.extend(&ord[i..]);

                close.extend(&state.close[..i]);
                close.push(NULL);
                for j in i..ord.len(){
                    if state.close[j]!=NULL && ord[j].manh(p)<ord[j].manh(state.close[j]){
                        close.push(p);
                    } else{
                        close.push(state.close[j]);
                    }
                }
                
                mem::swap(&mut state.ord,&mut state.new);
                mem::swap(&mut state.close,&mut state.new_close);
                let new=state.eval();
                
                if add<=(new-state.score) as f64{
                    state.score=new;
                    state.add(p);
                    
                    true
                } else{
                    mem::swap(&mut state.ord,&mut state.new);
                    mem::swap(&mut state.close,&mut state.new_close);
                    
                    false
                }
                
            } else{
                // 削除
                if ord.is_empty(){
                    return false;
                }

                let i=rnd::get(ord.len());
                let p=ord[i];
                assert!(state.a[p]);
                
                new.extend(&ord[..i]);
                new.extend(&ord[i+1..]);

                close.extend(&state.close[..i]);
                for j in i+1..ord.len(){
                    if state.close[j]==p{
                        close.push(NULL);
                    } else{
                        close.push(state.close[j]);
                    }
                }
                
                mem::swap(&mut state.ord,&mut state.new);
                mem::swap(&mut state.close,&mut state.new_close);
                let new=state.eval();

                if add<=(new-state.score) as f64{
                    state.score=new;
                    state.rem(p);
                    
                    true
                } else{
                    mem::swap(&mut state.ord,&mut state.new);
                    mem::swap(&mut state.close,&mut state.new_close);
                    
                    false
                }
            }
        },

        1=>{
            // shift
            if ord.len()<=1{
                return false;
            }

            let i=rnd::get(ord.len());
            let mut j=rnd::range(i.saturating_sub(10),(i+10).min(ord.len()-1)); // todo
            if i<=j{
                j+=2;
            }
            
            let p=ord[i];
            if i<=j{
                new.extend(&ord[..i]);
                new.extend(&ord[i+1..j]);
                new.push(ord[i]);
                new.extend(&ord[j..]);

                close.extend(&state.close[..i]);
                let mut minp=state.close[i];
                for k in i+1..j{
                    if state.close[k]==p{
                        close.push(NULL);
                    } else{
                        close.push(state.close[k]);
                    }

                    if minp!=NULL && p.manh(ord[k])<p.manh(minp){
                        minp=ord[k];
                    }
                }
                close.push(minp);
                close.extend(&state.close[j..]);

            } else{
                new.extend(&ord[..j]);
                new.push(ord[i]);
                new.extend(&ord[j..i]);
                new.extend(&ord[i+1..]);

                close.extend(&state.close[..j]);
                close.push(state.close[i]);
                for k in j..i{
                    if state.close[k]!=NULL && ord[k].manh(p)<ord[k].manh(state.close[k]){
                        close.push(p);
                    } else{
                        close.push(state.close[k]);
                    }

                    if ord[k]==state.close[i]{
                        close[j]=NULL;
                    }
                }
                close.extend(&state.close[i+1..]);
            }

            mem::swap(&mut state.ord,&mut state.new);
            mem::swap(&mut state.close,&mut state.new_close);
            let new=state.eval();
            
            if add<=(new-state.score) as f64{
                state.score=new;
                
                true
            } else{
                mem::swap(&mut state.ord,&mut state.new);
                mem::swap(&mut state.close,&mut state.new_close);

                false
            }
        },

        2=>{
            // replace
            if ord.len()==0{
                return false;
            }

            let mut i=rnd::get(ord.len());
            let p=ord[i];
            let np=get_neighbor()[p].choice();

            if state.a[np] || rnd::next()&7==0{
                let mut j=if state.a[np]{
                    (0..ord.len()).find(|&j|ord[j]==np).unwrap()
                } else{
                    rnd::range_skip(i.saturating_sub(10),(i+10).min(ord.len()),i)
                };
                if i>j{
                    (i,j)=(j,i);
                }

                let (p,q)=(ord[i],ord[j]);
                close.extend(&state.close[..i]);
                close.push(state.close[j]);

                let mut minp=state.close[i];

                if state.close[j]==p{
                    close[i]=NULL;
                }

                for k in i+1..j{
                    if state.close[j]==ord[k]{
                        close[i]=NULL;
                    }
                    
                    if state.close[k]!=NULL && ord[k].manh(q)<ord[k].manh(state.close[k]){
                        close.push(q);
                    } else if state.close[k]==p{
                        close.push(NULL);
                    } else{
                        close.push(state.close[k]);
                    }

                    if minp!=NULL && p.manh(ord[k])<p.manh(minp){
                        minp=ord[k];
                    }
                }

                if minp!=NULL && p.manh(q)<p.manh(minp){
                    minp=q;
                }
                close.push(minp);
                close.extend(&state.close[j+1..]);
                
                ord.swap(i,j);

                mem::swap(&mut state.close,&mut state.new_close);
                let new=state.eval();

                if add<=(new-state.score) as f64{
                    state.score=new;
                    
                    true
                } else{
                    state.ord.swap(i,j);
                    mem::swap(&mut state.close,&mut state.new_close);

                    false
                }
            } else{
                ord[i]=np;

                close.extend(&state.close[..i]);
                close.push(NULL);
                for j in i+1..ord.len(){
                    if state.close[j]!=NULL && ord[j].manh(np)<ord[j].manh(state.close[j]){
                        close.push(np);
                    } else if state.close[j]==p{
                        close.push(NULL);
                    } else{
                        close.push(state.close[j]);
                    }
                }
    
                mem::swap(&mut state.close,&mut state.new_close);
                let new=state.eval();

                if add<=(new-state.score) as f64{
                    state.score=new;
                    state.add(np);
                    state.rem(p);
                    
                    true
                } else{
                    mem::swap(&mut state.close,&mut state.new_close);
                    state.ord[i]=p;
                    
                    false
                }
            }
        },
        
        _=>panic!(),
    }
}



static mut ITER:usize=0;
static mut VALID:usize=0;



fn anneal(state:&mut State,tl:f64,T0:f64,T1:f64){
    state.debug();
    
    let log=logs();
    let start=get_time();
    let tl=tl-start;
    assert!(0.<tl);

    let mut valid=0;
    let mut iter=0;
    let mut heat=0.;
    let mut time=0.;

    let mut best=state.clone();

    while iter&255!=0 || {
        time=(get_time()-start)/tl;
        heat=T0*(T1/T0).powf(time.powf(1.2)); // todo
        time<=1.
    }{
        iter+=1;
        let add=heat*log[iter&65535]; // 最大化
        valid+=trans(state,add) as usize;

        if 0.8<=time && best.score<state.score{
            best=state.clone();
        }
    }

    state.debug();
    
    unsafe{
        ITER+=iter;
        VALID+=valid;
    }
    
    *state=best;
}



fn hill_climb(state:&mut State,tl:f64){
    while get_time()<=tl{
        let f=trans(state,0.);
        unsafe{
            ITER+=1;
            VALID+=f as usize;
        }
    }
}



#[derive(Clone,Copy,PartialEq,Debug)]
enum EvaluationType{
    Money,
    Income,
    NoCross,
}
use EvaluationType::*;


static mut EVAL_TYPE:EvaluationType=Money;
static mut EVAL_TURN:usize=T;



#[derive(Clone)]
struct State{
    ord:Vec<P>,
    a:[[bool;N];N],
    score:i64,
    close:Vec<P>,
    cover:Vec<u8>,
    set:IndexSet,
    not:IndexSet,

    new:Vec<P>,
    new_close:Vec<P>, // NULL:不明
    seen:[[usize;N];N],
    it:usize,

    pair:Vec<bool>,
    cnt:[[u32;N];N],
    es:Vec<(P,P)>,
    lines:Vec<(P,P)>,
    heap:BinaryHeap<Rev<(usize,u32,P)>>,
}
impl State{
    fn new()->State{
        let input=get_input();
        let mut not=IndexSet::new(input.M);
        not.fill();

        State{
            ord:vec![],
            a:[[false;N];N],
            score:0,
            cover:vec![0;input.M*2],
            close:vec![],
            set:IndexSet::new(input.M*2),
            not,

            new:vec![],
            new_close:vec![],
            seen:[[0;N];N],
            it:0,

            pair:vec![false;input.M*2],
            cnt:[[0;N];N],
            es:vec![],
            lines:vec![],
            heap:BinaryHeap::new(),
        }
    }

    fn add(&mut self,p:P){
        assert!(self.a[p]==false);
        self.a[p]=true;
        for &i in &get_people()[p]{
            let f0=(0<self.cover[i]) ^ (0<self.cover[i^1]);
            self.cover[i]+=1;
            let f1=(0<self.cover[i]) ^ (0<self.cover[i^1]);

            if (f0,f1)==(false,true){
                self.set.add(i^1);
            } else if (f0,f1)==(true,false){
                self.set.rem(i);
            }

            if self.cover[i]==1 && self.cover[i^1]==0{
                self.not.rem(i>>1);
            }
        }
    }

    fn rem(&mut self,p:P){
        assert!(self.a[p]==true);
        self.a[p]=false;
        for &i in &get_people()[p]{
            let f0=(0<self.cover[i]) ^ (0<self.cover[i^1]);
            self.cover[i]-=1;
            let f1=(0<self.cover[i]) ^ (0<self.cover[i^1]);

            if (f0,f1)==(false,true){
                self.set.add(i);
            } else if (f0,f1)==(true,false){
                self.set.rem(i^1);
            }

            if self.cover[i]|self.cover[i^1]==0{
                self.not.add(i>>1);
            }
        }
    }

    fn eval(&mut self)->i64{
        match unsafe{EVAL_TYPE}{
            Income|Money=>self.eval_normal(),
            NoCross=>self.eval_no_cross(),
        }
    }

    fn eval_normal(&mut self)->i64{
        let ord=&self.ord;
        let pair=&mut self.pair;
        let cnt=&mut self.cnt;
        let es=&mut self.es;
        let seen=&mut self.seen;
        
        let input=get_input();
        let mut money=input.K;

        if ord.len()==0{
            return 0;
        }
        
        pair.fill(false);
        es.clear();

        let people=get_people();
        let mut add_station=|p:P|->i64{
            let mut income=0;
            for &n in &people[p]{
                if !pair[n]{
                    pair[n]=true;
                    if pair[n^1]{
                        income+=input.pair[n>>1].2;
                    }
                }
            }
            income
        };

        let mut income=add_station(ord[0]);
        let mut turn=1;

        money+=income-STATION_COST;

        self.it+=2;
        let it=self.it;
        let add_line=|u:P,v:P,seen:&mut [[usize;N];N]|{
            assert!(is_line(u,v));
            if u.i==v.i{
                let (mut a,mut b)=(u.j,v.j);
                if a>b{
                    (a,b)=(b,a);
                }

                seen[u.i][a..b].fill(it+1);
            } else{
                let (mut a,mut b)=(u.i,v.i);
                if a>b{
                    (a,b)=(b,a);
                }

                for i in a..b{
                    seen[i][u.j]=it;
                }
            }
        };

        let add_edge=|u:P,v:P,seen:&mut [[usize;N];N],es:&mut Vec<(P,P)>|{
            if is_line(u,v){
                add_line(u,v,seen);
            } else{
                es.push((u,v));
            }
        };

        let get_initial_bit=|p:P|->u32{
            DEF+((get_deg(p) as u32)<<24)
        };
        cnt[ord[0]]=get_initial_bit(ord[0]);

        for i in 1..ord.len(){
            cnt[ord[i]]=get_initial_bit(ord[i]);

            let p=ord[i];
            let rails;

            'a: {
                if self.it<=seen[p]{
                    if seen[p]==it{
                        cnt[p]+=SAME_J;
                    } else{
                        assert!(seen[p]==it+1);
                        cnt[p]+=SAME_I;
                    }
                    rails=0;
                    break 'a;
                }

                let mut min=i64::MAX;
                let mut minj=!0;
                for (j,&(u,v)) in es.iter().enumerate(){
                    if u.manh(v)!=p.manh(u)+p.manh(v){
                        continue;
                    }

                    if !isok(cnt[u]-get_bit(u,v)+get_bit(u,p))
                        || !isok(cnt[v]-get_bit(v,u)+get_bit(v,p))
                    {
                        continue;
                    }

                    let area=p.get_area(P{i:u.i,j:v.j})+p.get_area(P{i:v.i,j:u.j});
                    if min>area{
                        min=area;
                        minj=j;
                    }
                }

                if minj!=!0{
                    let (u,v)=es.swap_remove(minj);

                    cnt[u]+=get_bit(u,p)-get_bit(u,v);
                    cnt[v]+=get_bit(v,p)-get_bit(v,u);
                    cnt[p]+=get_bit(p,u)+get_bit(p,v);
                    
                    add_edge(u,p,seen,es);
                    add_edge(v,p,seen,es);
                    rails=0;
                    break 'a;
                }

                let mut minp=self.close[i];

                if minp==NULL{
                    let mut min=(usize::MAX,u32::MAX);

                    let mut min4=usize::MAX;
                    let mut minp4=NULL;

                    for &np in &ord[..i]{
                        if !isok(cnt[np]+get_bit(np,p)){
                            let new=p.manh(np);
                            if min4>new{
                                min4=new;
                                minp4=np;
                            }
                        } else{
                            let new=(p.manh(np),cnt[np]>>24);
                            if min>new{
                                min=new;
                                minp=np;
                            }
                        }
                    }

                    if min.0>min4{
                        self.close[i]=minp4;
                    } else{
                        self.close[i]=minp;
                    }
                    
                } else if !isok(cnt[minp]+get_bit(minp,p)){
                    let mut min=(usize::MAX,u32::MAX);
                    minp=NULL;

                    for &np in &ord[..i]{
                        if !isok(cnt[np]+get_bit(np,p)){
                            continue;
                        }
                        
                        let new=(p.manh(np),cnt[np]>>24);
                        if min>new{
                            min=new;
                            minp=np;
                        }
                    }
                }
                
                cnt[minp]+=get_bit(minp,p);
                cnt[p]+=get_bit(p,minp);

                add_edge(minp,p,seen,es);
                rails=p.manh(minp)-1;
            }

            assert!(rails!=!0);

            if turn+rails+1>=unsafe{EVAL_TURN}{
                return 0;
            }

            money+=(income-RAIL_COST)*rails as i64-STATION_COST;

            if money<0{
                if income==0{
                    return 0;
                }

                let wait=((-money+income-1)/income) as usize;
                if turn+rails+1+wait>=unsafe{EVAL_TURN}{
                    return 0;
                }
                
                money+=income*wait as i64;
                turn+=wait;
            }

            income+=add_station(p);
            money+=income;
            turn+=rails+1;
        }

        if unsafe{EVAL_TYPE}==Money{
            money+=(unsafe{EVAL_TURN}-turn) as i64*income;
            money
        } else{
            income
        }
    }

    fn eval_no_cross(&mut self)->i64{
        let ord=&self.ord;
        let pair=&mut self.pair;
        let cnt=&mut self.cnt;
        let es=&mut self.es;
        let lines=&mut self.lines;
        let seen=&mut self.seen;
        let heap=&mut self.heap;
        
        let input=get_input();
        let mut money=input.K;

        if ord.len()==0{
            return 0;
        }
        
        pair.fill(false);
        es.clear();
        lines.clear();

        let people=get_people();
        let mut add_station=|p:P|->i64{
            let mut income=0;
            for &n in &people[p]{
                if !pair[n]{
                    pair[n]=true;
                    if pair[n^1]{
                        income+=input.pair[n>>1].2;
                    }
                }
            }
            income
        };

        let mut income=add_station(ord[0]);
        let mut turn=1;

        money+=income-STATION_COST;

        self.it+=2;
        let it=self.it;
        let try_add_line=|u:P,v:P,cnt:&[[u32;N];N]|->bool{
            assert!(is_line(u,v));
            if u.i==v.i{
                let (mut a,mut b)=(u.j,v.j);
                if a>b{
                    (a,b)=(b,a);
                }

                for j in a..b{
                    if self.a[u.i][j]{
                        continue;
                    }

                    let p=P::new(u.i-1,j);
                    if p.in_range() && !isok(cnt[p]+ADD[3]){
                        return false;
                    }

                    let p=P::new(u.i+1,j);
                    if p.in_range() && !isok(cnt[p]+ADD[1]){
                        return false;
                    }
                }
            } else{
                let (mut a,mut b)=(u.i,v.i);
                if a>b{
                    (a,b)=(b,a);
                }

                for i in a..b{
                    if self.a[i][u.j]{
                        continue;
                    }

                    let p=P::new(i,u.j-1);
                    if p.in_range() && !isok(cnt[p]+ADD[2]){
                        return false;
                    }

                    let p=P::new(i,u.j+1);
                    if p.in_range() && !isok(cnt[p]+ADD[0]){
                        return false;
                    }
                }
            }
            
            true
        };

        let try_add_edge=|u:P,v:P,cnt:&[[u32;N];N]|->bool{
            if is_line(u,v){
                return try_add_line(u,v,cnt);
            }
            true
        };
        
        let add_line=|u:P,v:P,seen:&mut [[usize;N];N],cnt:&mut [[u32;N];N]|{
            assert!(is_line(u,v));
            if u.i==v.i{
                let (mut a,mut b)=(u.j,v.j);
                if a>b{
                    (a,b)=(b,a);
                }

                for j in a..b{
                    seen[u.i][j]=it+1;
                    if self.a[u.i][j]{
                        continue;
                    }

                    let p=P::new(u.i-1,j);
                    if p.in_range(){
                        cnt[p]+=ADD[3];
                    }

                    let p=P::new(u.i+1,j);
                    if p.in_range(){
                        cnt[p]+=ADD[1];
                    }
                }
            } else{
                let (mut a,mut b)=(u.i,v.i);
                if a>b{
                    (a,b)=(b,a);
                }

                for i in a..b{
                    seen[i][u.j]=it;

                    if self.a[i][u.j]{
                        continue;
                    }

                    let p=P::new(i,u.j-1);
                    if p.in_range(){
                        cnt[p]+=ADD[2];
                    }

                    let p=P::new(i,u.j+1);
                    if p.in_range(){
                        cnt[p]+=ADD[0];
                    }
                }
            }
        };

        let add_edge=|u:P,v:P,seen:&mut [[usize;N];N],es:&mut Vec<(P,P)>,lines:&mut Vec<(P,P)>,cnt:&mut [[u32;N];N]|{
            if is_line(u,v){
                add_line(u,v,seen,cnt);
                lines.push((u,v));
            } else{
                es.push((u,v));
            }
        };

        let get_initial_bit=|p:P|->u32{
            DEF+((get_deg(p) as u32)<<24)
        };

        let is_cross=|u:P,v:P,lines:&[(P,P)],es:&[(P,P)]|->bool{
            chain!(lines,es).any(|&(a,b)|is_cross(u,v,a,b))
        };

        let no_cross_position=|p:P,heap:&mut BinaryHeap<Rev<(usize,u32,P)>>,lines:&[(P,P)],es:&[(P,P)]|->P{
            while let Some(&Rev((_,_,np)))=heap.peek(){
                if !is_cross(p,np,lines,es){
                    return np;
                }
                heap.pop();
            }

            NULL
        };
        
        *cnt=[[0;N];N];
        for &p in ord{
            cnt[p]+=get_initial_bit(p);
        }

        for i in 1..ord.len(){
            let p=ord[i];
            let rails;

            'a: {
                if self.it<=seen[p]{
                    if seen[p]==it{
                        cnt[p]+=SAME_J;
                    } else{
                        assert!(seen[p]==it+1);
                        cnt[p]+=SAME_I;
                    }
                    rails=0;
                    break 'a;
                }

                let mut min=i64::MAX;
                let mut minj=!0;
                for (j,&(u,v)) in es.iter().enumerate(){
                    if u.manh(v)!=p.manh(u)+p.manh(v){
                        continue;
                    }

                    if !isok(cnt[u]-get_bit(u,v)+get_bit(u,p))
                        || !isok(cnt[v]-get_bit(v,u)+get_bit(v,p))
                        || !try_add_edge(u,p,cnt)
                        || !try_add_edge(v,p,cnt)
                    {
                        continue;
                    }

                    let area=p.get_area(P{i:u.i,j:v.j})+p.get_area(P{i:v.i,j:u.j});
                    if min>area{
                        min=area;
                        minj=j;
                    }
                }

                if minj!=!0{
                    let (u,v)=es.swap_remove(minj);

                    cnt[u]+=get_bit(u,p)-get_bit(u,v);
                    cnt[v]+=get_bit(v,p)-get_bit(v,u);
                    cnt[p]+=get_bit(p,u)+get_bit(p,v);
                    
                    add_edge(u,p,seen,es,lines,cnt);
                    add_edge(v,p,seen,es,lines,cnt);
                    rails=0;
                    break 'a;
                }

                let mut minp=self.close[i];
                heap.clear();

                if minp==NULL{
                    let mut min4=usize::MAX;
                    let mut minp4=NULL;

                    for &np in &ord[..i]{
                        let new=p.manh(np);
                        if min4>new{
                            min4=new;
                            minp4=np;
                        }

                        if isok(cnt[np]+get_bit(np,p)) && try_add_edge(p,np,cnt){
                            heap.push(Rev((new,cnt[np]>>24,np)));
                        }
                    }

                    self.close[i]=minp4;
                    minp=no_cross_position(p,heap,lines,es);
                } else if !isok(cnt[minp]+get_bit(minp,p)) || try_add_edge(p,minp,cnt) || is_cross(p,minp,lines,es){
                    for &np in &ord[..i]{
                        if isok(cnt[np]+get_bit(np,p)) && try_add_edge(p,np,cnt){
                            heap.push(Rev((p.manh(np),cnt[np]>>24,np)));
                        }
                    }
                    
                    minp=no_cross_position(p,heap,lines,es);
                }

                if minp==NULL{
                    return 0;
                }
                
                cnt[minp]+=get_bit(minp,p);
                cnt[p]+=get_bit(p,minp);

                add_edge(minp,p,seen,es,lines,cnt);
                rails=p.manh(minp)-1;
            }

            assert!(rails!=!0);

            if turn+rails+1>=unsafe{EVAL_TURN}{
                return 0;
            }

            money+=(income-RAIL_COST)*rails as i64-STATION_COST;

            if money<0{
                if income==0{
                    return 0;
                }

                let wait=((-money+income-1)/income) as usize;
                if turn+rails+1+wait>=unsafe{EVAL_TURN}{
                    return 0;
                }
                
                money+=income*wait as i64;
                turn+=wait;
            }

            income+=add_station(p);
            money+=income;
            turn+=rails+1;
        }

        money+=(unsafe{EVAL_TURN}-turn) as i64*income;
        money
    }

    fn get_es(&mut self)->Vec<(P,P)>{
        let ord=&self.ord;
        let cnt=&mut self.cnt;
        let es=&mut self.es;
        let seen=&mut self.seen;
        let heap=&mut self.heap;

        assert!(ord.len()!=0);
        es.clear();

        self.it+=2;
        let it=self.it;

        let try_add_line=|u:P,v:P,seen:&[[usize;N];N],cnt:&[[u32;N];N]|->bool{
            assert!(is_line(u,v));
            if u.i==v.i{
                let (mut a,mut b)=(u.j,v.j);
                if a>b{
                    (a,b)=(b,a);
                }

                for j in a..b{
                    if self.a[u.i][j] || seen[u.i][j]==it{
                        continue;
                    }

                    let p=P::new(u.i-1,j);
                    if p.in_range() && !isok(cnt[p]+ADD[3]){
                        return false;
                    }

                    let p=P::new(u.i+1,j);
                    if p.in_range() && !isok(cnt[p]+ADD[1]){
                        return false;
                    }
                }
            } else{
                let (mut a,mut b)=(u.i,v.i);
                if a>b{
                    (a,b)=(b,a);
                }

                for i in a..b{
                    if self.a[i][u.j] || seen[i][u.j]==it{
                        continue;
                    }

                    let p=P::new(i,u.j-1);
                    if p.in_range() && !isok(cnt[p]+ADD[2]){
                        return false;
                    }

                    let p=P::new(i,u.j+1);
                    if p.in_range() && !isok(cnt[p]+ADD[0]){
                        return false;
                    }
                }
            }
            
            true
        };

        let try_add_edge=|u:P,v:P,seen:&[[usize;N];N],cnt:&[[u32;N];N]|->bool{
            if is_line(u,v){
                return try_add_line(u,v,seen,cnt);
            }
            true
        };
        
        let add_line=|u:P,v:P,seen:&mut [[usize;N];N],cnt:&mut [[u32;N];N]|{
            assert!(is_line(u,v));
            if u.i==v.i{
                let (mut a,mut b)=(u.j,v.j);
                if a>b{
                    (a,b)=(b,a);
                }

                for j in a..b{
                    if self.a[u.i][j] || seen[u.i][j]==it{
                        continue;
                    }
                    seen[u.i][j]=it;

                    let p=P::new(u.i-1,j);
                    if p.in_range(){
                        cnt[p]+=ADD[3];
                    }

                    let p=P::new(u.i+1,j);
                    if p.in_range(){
                        cnt[p]+=ADD[1];
                    }
                }
            } else{
                let (mut a,mut b)=(u.i,v.i);
                if a>b{
                    (a,b)=(b,a);
                }

                for i in a..b{
                    if self.a[i][u.j] || seen[i][u.j]==it{
                        continue;
                    }
                    seen[i][u.j]=it;

                    let p=P::new(i,u.j-1);
                    if p.in_range(){
                        cnt[p]+=ADD[2];
                    }

                    let p=P::new(i,u.j+1);
                    if p.in_range(){
                        cnt[p]+=ADD[0];
                    }
                }
            }
        };

        let add_edge=|u:P,v:P,seen:&mut [[usize;N];N],es:&mut Vec<(P,P)>,cnt:&mut [[u32;N];N]|{
            es.push((u,v));
            if is_line(u,v){
                add_line(u,v,seen,cnt);
            }
        };

        let get_initial_bit=|p:P|->u32{
            DEF+((get_deg(p) as u32)<<24)
        };

        let is_cross=|u:P,v:P,es:&[(P,P)]|->bool{
            es.iter().any(|&(a,b)|is_cross(u,v,a,b))
        };

        let no_cross_position=|p:P,heap:&mut BinaryHeap<Rev<(usize,u32,P)>>,es:&[(P,P)]|->P{
            while let Some(&Rev((_,_,np)))=heap.peek(){
                if !is_cross(p,np,es){
                    return np;
                }
                heap.pop();
            }

            NULL
        };
        
        *cnt=[[0;N];N];
        for &p in ord{
            cnt[p]+=get_initial_bit(p);
        }

        for i in 1..ord.len(){
            let p=ord[i];
            let rails;

            'a: {
                let mut min=i64::MAX;
                let mut minj=!0;
                for (j,&(u,v)) in es.iter().enumerate(){
                    if u.manh(v)!=p.manh(u)+p.manh(v){
                        continue;
                    }

                    if !isok(cnt[u]-get_bit(u,v)+get_bit(u,p))
                        || !isok(cnt[v]-get_bit(v,u)+get_bit(v,p))
                        || !try_add_edge(u,p,seen,cnt)
                        || !try_add_edge(v,p,seen,cnt)
                        || (!on_line(u,v,p) && (is_cross(u,p,&es) || is_cross(v,p,&es)))
                    {
                        continue;
                    }

                    let area=p.get_area(P{i:u.i,j:v.j})+p.get_area(P{i:v.i,j:u.j});
                    if min>area{
                        min=area;
                        minj=j;
                    }
                }

                if minj!=!0{
                    let (u,v)=es.swap_remove(minj);

                    cnt[u]+=get_bit(u,p)-get_bit(u,v);
                    cnt[v]+=get_bit(v,p)-get_bit(v,u);
                    cnt[p]+=get_bit(p,u)+get_bit(p,v);
                    
                    add_edge(u,p,seen,es,cnt);
                    add_edge(v,p,seen,es,cnt);
                    rails=0;
                    break 'a;
                }

                let mut minp=self.close[i];
                heap.clear();

                if minp==NULL{
                    let mut min4=usize::MAX;
                    let mut minp4=NULL;

                    for &np in &ord[..i]{
                        let new=p.manh(np);
                        if min4>new{
                            min4=new;
                            minp4=np;
                        }

                        if isok(cnt[np]+get_bit(np,p)) && try_add_edge(p,np,seen,cnt){
                            heap.push(Rev((new,cnt[np]>>24,np)));
                        }
                    }

                    self.close[i]=minp4;
                    minp=no_cross_position(p,heap,es);
                } else if !isok(cnt[minp]+get_bit(minp,p)) || try_add_edge(p,minp,seen,cnt) || is_cross(p,minp,es){
                    for &np in &ord[..i]{
                        if isok(cnt[np]+get_bit(np,p)) && try_add_edge(p,np,seen,cnt){
                            heap.push(Rev((p.manh(np),cnt[np]>>24,np)));
                        }
                    }
                    
                    minp=no_cross_position(p,heap,es);
                }

                assert!(minp!=NULL);
                
                cnt[minp]+=get_bit(minp,p);
                cnt[p]+=get_bit(p,minp);

                add_edge(minp,p,seen,es,cnt);
                rails=p.manh(minp)-1;
            }

            assert!(rails!=!0);
        }

        es.clone()
    }

    #[track_caller]
    fn debug(&self){
        let mut cnt=[[0;N];N];
        for &p in &self.ord{
            cnt[p]+=1;
        }

        for p in iter_pos(){
            assert!(cnt[p]<=2);
            assert!(self.a[p] as usize==cnt[p]);
        }

        for i in 1..self.ord.len(){
            if self.close[i]!=NULL{
                let p=self.ord[i];
                let min=(0..i).map(|j|self.ord[i].manh(self.ord[j])).min().unwrap();
                assert!(p.manh(self.close[i])==min);
                assert!((0..i).any(|j|self.ord[j]==self.close[i]));
            }
        }

        let input=get_input();
        let mut cover=vec![0;input.M*2];
        let mut set=IndexSet::new(input.M*2);
        let mut not=IndexSet::new(input.M);
        not.fill();
        for &p in &self.ord{
            for &i in &get_people()[p]{
                let f0=(0<cover[i]) ^ (0<cover[i^1]);
                cover[i]+=1;
                let f1=(0<cover[i]) ^ (0<cover[i^1]);
    
                if (f0,f1)==(false,true){
                    set.add(i^1);
                } else if (f0,f1)==(true,false){
                    set.rem(i);
                }

                if not.contains(i>>1){
                    not.rem(i>>1);
                }
            }
        }

        assert!(self.cover==cover);
        for i in 0..input.M*2{
            assert!(self.set.contains(i)==set.contains(i));
        }

        for i in 0..input.M{
            assert!(self.not.contains(i)==not.contains(i));
        }
    }
}



fn get_deg(p:P)->usize{
    let is_bound=|n:usize|n==0 || n==N-1;
    is_bound(p.i) as usize+is_bound(p.j) as usize
}



const DEF:u32    =  0b0011_000_000_000_000_001_001_001_001;
const SAME_I:u32 =  0b0010_001_010_001_010_001_001_001_001;
const SAME_J:u32 =  0b0010_010_001_010_001_001_001_001_001;
const MASK:u32   =  0b1000_100_100_100_100_100_100_100_100;

const ADD:[u32;4]= [0b0001_001_001_000_001_000_001_001_000,
                    0b0001_001_001_001_000_001_001_000_000,
                    0b0001_000_001_001_001_001_000_000_001,
                    0b0001_001_000_001_001_000_000_001_001];


// pが自分
fn get_bit(p:P,q:P)->u32{
    let i0=(p.i<=q.i) as u32;
    let i1=(p.i>=q.i) as u32;
    let j0=(p.j<=q.j) as u32;
    let j1=(p.j>=q.j) as u32;

    (i0&j0)
    | ((i0&j1)<<3)
    | ((i1&j1)<<6)
    | ((i1&j0)<<9)
    | (i0<<12)
    | (j0<<15)
    | (i1<<18)
    | (j1<<21)
    | (1<<24)
}

fn isok(a:u32)->bool{
    a&MASK==0
}



#[allow(unused)]
fn check(es:&[(P,P)]){
    let mut cross=0;
    for i in 0..es.len(){
        for j in 0..i{
            cross+=is_cross(es[i].0,es[i].1,es[j].0,es[j].1) as usize;
        }
    }
    eprintln!("cross = {cross}");

    let mut cnt=vec![vec![[0;8];N];N];
    let add=|a:&mut [u8],p:P,q:P|{
        let i=(q.i-p.i) as i64;
        let j=(q.j-p.j) as i64;
        a[0]+=(0<=i && 0<=j) as u8;
        a[1]+=(0<=i && 0>=j) as u8;
        a[2]+=(0>=i && 0>=j) as u8;
        a[3]+=(0>=i && 0<=j) as u8;
        a[4]+=(0<=i) as u8;
        a[5]+=(0<=j) as u8;
        a[6]+=(0>=i) as u8;
        a[7]+=(0>=j) as u8;
    };
    
    for &(u,v) in es{
        add(&mut cnt[u],u,v);
        add(&mut cnt[v],v,u);
    }

    let mut mm0=0;
    let mut mm1=0;
    for p in iter_pos(){
        for i in 0..4{
            mm0+=(cnt[p][i]>=3) as usize;
        }
        for i in 4..8{
            mm1+=(cnt[p][i]>=4) as usize;
        }
    }

    eprintln!("mismatch0 = {mm0}");
    eprintln!("mismatch1 = {mm1}");
}



#[allow(unused)]
fn print_es(es:&[(P,P)]){
    for &(u,v) in es{
        println!("{} {} {} {}",u.i,u.j,v.i,v.j);
    }
}



fn assign_neighbor(es:&[(P,P)])->Result<Vec<(P,P)>,Vec<P>>{
    let mut g=vec![vec![vec![];N];N];
    let mut line=[[false;N];N];

    for &(u,v) in es{
        g[u].push(v);
        g[v].push(u);
        line[u]=true;
        line[v]=true;

        if u.i==v.i{
            let (mut a,mut b)=(u.j,v.j);
            if a>b{
                (a,b)=(b,a);
            }

            line[u.i][a..b].fill(true);
        } else if u.j==v.j{
            let (mut a,mut b)=(u.i,v.i);
            if a>b{
                (a,b)=(b,a);
            }

            for i in a..b{
                line[i][u.j]=true;
            }
        }
    }

    let mut done=FxHashMap::<(P,P),P>::default();
    let mut ps=iter_pos().collect_vec();
    ps.sort_unstable_by_key(|&p|!g[p].len());
    
    for &u in &ps{
        let mut todo=vec![];
        for &v in &g[u]{
            let mut nv=done.get(&(v,u)).copied().unwrap_or(v);
            if nv==u{
                nv=v;
            }

            if is_line(u,nv){
                let nu=(0..4).map(|i|u+DD[i]).find(|&nu|u.manh(nv)==nu.manh(nv)+1).unwrap();
                line[nu]=true;
                done.insert((u,v),nu);
            } else{
                todo.push((v,nv));
            }
        }

        let a=(0..4).map(|i|u+DD[i]).filter(|&nu|nu.in_range() && !line[nu]).collect_vec();

        let mut min=usize::MAX;
        let mut best_nus=vec![];
        
        'a: for nus in a.into_iter().permutations(todo.len()){
            assert!(todo.len()==nus.len());

            for i in 0..todo.len(){
                let (nu,(v,_))=(nus[i],todo[i]);
                if u.manh(v)!=nu.manh(v)+1{
                    continue 'a;
                }

                for j in 0..i{
                    if is_cross(nu,v,nus[j],todo[j].0){
                        continue 'a;
                    }
                }

                for &(a,b) in es{
                    if is_cross(a,b,nu,v) && !is_cross(a,b,u,nu){
                        continue 'a;
                    }
                }
            }

            let mut line=0;
            for (&nu,&(_,nv)) in izip!(&nus,&todo){
                line+=is_line(nu,nv) as usize;
            }

            if min>line{
                min=line;
                best_nus=nus;
            }
        }

        if min==usize::MAX{
            let mut ret=g[u].clone();
            ret.push(u);
            return Err(ret);
        }

        for (&nu,&(v,_)) in izip!(&best_nus,&todo){
            done.insert((u,v),nu);
        }
    }

    let mut ret=vec![];
    for &(u,v) in es{
        if u.manh(v)==1{
            ret.push((u,v));
            continue;
        }

        let nu=done[&(u,v)];
        let nv=done[&(v,u)];

        if nu!=nv{
            ret.push((nu,nv));
        }
        ret.push((u,nu));
        ret.push((v,nv));
    }

    Ok(ret)
}



fn make_tree(mut es:Vec<(P,P)>)->Result<Vec<(P,P)>,Vec<P>>{
    let mut ok=[[true;N];N];
    for &(u,v) in &es{
        ok[u]=false;
        ok[v]=false;
    }

    let mut que=VecDeque::new();
    es.sort_unstable_by_key(|&(u,v)|(u.i.abs_diff(v.i).min(u.j.abs_diff(v.j)),u.get_area(v)));

    let mut prev=[[NULL;N];N];
    let mut seen=[[0;N];N];
    let mut it=0;

    let mut ret=vec![];

    for &(u,v) in &es{
        que.clear();
        que.push_back(u);
        it+=1;
        seen[u]=it;
        prev[u]=NULL;
        
        let mut goal=false;
        
        'a: while let Some(p)=que.pop_front(){
            for dd in DD{
                let np=p+dd;
                if np==v{
                    goal=true;
                    prev[np]=p;
                    break 'a;
                }
                if np.in_range() && ok[np] && seen[np]!=it{
                    seen[np]=it;
                    prev[np]=p;
                    que.push_back(np);
                }
            }
        }
        
        assert!(goal);

        let mut len=0;
        let mut p=v;
        loop{
            let np=prev[p];
            if np==NULL{
                assert!(p==u);
                break;
            }
            ret.push((p,np));
            p=np;
            ok[np]=false;
            len+=1;
        }

        if u.manh(v)!=len{
            return Err(vec![u,v]);
        }
    }

    Ok(ret)
}



fn rollback(mut ord:Vec<P>,mut close:Vec<P>,ps:Vec<P>)->Vec<Act>{
    let get_idx=|p:P|(0..ord.len()).find(|&i|ord[i]==p).unwrap();
    let i=ps.iter().map(|&p|get_idx(p)).max().unwrap();

    ord.remove(i);
    close.remove(i);

    let mut state=State::new();
    for (&p,&q) in izip!(&ord,&close){
        state.ord.push(p);
        state.close.push(q);
        state.add(p);
    }

    let es=state.get_es();
    restore(state.ord,state.close,&es)
}



fn restore(ord:Vec<P>,close:Vec<P>,es:&[(P,P)])->Vec<Act>{
    let es=match assign_neighbor(es){
        Ok(x)=>x,
        Err(ps)=>return rollback(ord,close,ps),
    };

    let mut g=vec![vec![];N*N];
    for &(u,v) in &es{
        g[id(u)].push(id(v));
        g[id(v)].push(id(u));
    }
    
    let es=match make_tree(es){
        Ok(x)=>x,
        Err(res)=>{
            let mut a=[[false;N];N];
            for &p in &ord{
                a[p]=true;
            }
            
            let mut ps=vec![];
            for &p in &res{
                for &np in &g[id(p)]{
                    let np=to(np);
                    if a[np]{
                        ps.push(np);
                    }
                }
            }

            return rollback(ord,close,ps);
        },
    };

    fn id(p:P)->usize{
        p.i*N+p.j
    }
    fn to(n:usize)->P{
        P::new(n/N,n%N)
    }

    let mut g=vec![vec![];N*N];
    for &(u,v) in &es{
        g[id(u)].push(id(v));
        g[id(v)].push(id(u));
    }

    let input=get_input();
    let people=get_people();

    let mut pair=vec![false;input.M*2];
    let mut add_station=|p:P|->i64{
        let mut income=0;
        for &n in &people[p]{
            if !pair[n]{
                pair[n]=true;
                if pair[n^1]{
                    income+=input.pair[n>>1].2;
                }
            }
        }
        income
    };

    let mut done=[false;N*N];
    let mut ans=vec![PutStation(ord[0])];

    let mut money=input.K;
    let mut income=add_station(ord[0]);
    let mut turn=1;
    done[id(ord[0])]=true;

    fn dfs(g:&Vec<Vec<usize>>,p:usize,v:usize,map:&mut [bool],rails:&mut Vec<P>)->bool{
        if map[v]{
            rails.push(to(v));
            return true;
        }

        for &nv in &g[v]{
            if nv!=p && dfs(g,v,nv,map,rails){
                rails.push(to(v));
                map[v]=true;
                return true;
            }
        }
        
        false
    }

    money+=income-STATION_COST;

    for &p in &ord[1..]{
        let mut rails=vec![];
        let f=dfs(&g,!0,id(p),&mut done,&mut rails);
        assert!(f);

        let rail_num=rails.len().saturating_sub(2);
        assert!(turn+rail_num+1<T);

        money+=(income-RAIL_COST)*rail_num as i64-STATION_COST;
        if money<0{
            assert!(income!=0);

            let wait=((-money+income-1)/income) as usize;
            assert!(turn+rail_num+1+wait<T);

            money+=income*wait as i64;
            turn+=wait;

            ans.extend((0..wait).map(|_|Wait));
        }

        for w in rails.windows(3){
            ans.push(PutRail(get_rail_id(w[0],w[1],w[2]),w[1]));
        }
        ans.push(PutStation(p));
        
        income+=add_station(p);
        money+=income;
        turn+=rail_num+1;
    }

    for _ in 0..T-turn{
        ans.push(Wait);
    }
    assert!(ans.len()==T);

    money+=(T-turn) as i64*income;
    eprintln!("score = {}",money as i64);

    ans
}



use std::mem;
use std::collections::*;
use std::cmp::Reverse as Rev;
use itertools::*;
use rustc_hash::*;

const N:usize=50;
const T:usize=800;

const RAIL_COST:i64=100;
const STATION_COST:i64=5000;



// const DC:[char;4]=['L','U','R','D'];
const DD:[P;4]=[
    P{i:0,j:!0},
    P{i:!0,j:0},
    P{i:0,j:1},
    P{i:1,j:0},
];
const RAIL_ID:[[usize;4];4]=[
    [0,4,1,3],
    [4,0,5,2],
    [1,5,0,6],
    [3,2,6,0],
];
const DX:[P;13]=[
    P{i:0,j:0},
    P{i:0,j:!0},
    P{i:!0,j:0},
    P{i:0,j:1},
    P{i:1,j:0},
    P{i:!0,j:!0},
    P{i:1,j:1},
    P{i:!0,j:1},
    P{i:1,j:!0},
    P{i:0,j:!1},
    P{i:!1,j:0},
    P{i:0,j:2},
    P{i:2,j:0},
];



fn get_dir(p:P,q:P)->usize{
    let d=q-p;
    (0..4).find(|&i|DD[i]==d).unwrap()
}



fn get_rail_id(p:P,q:P,r:P)->usize{
    let i=get_dir(q,p);
    let j=get_dir(q,r);
    assert!(i!=j);
    RAIL_ID[i][j]
}



struct Input{
    M:usize,
    K:i64,
    pair:Vec<(P,P,i64)>,
}


#[derive(Clone,Copy,PartialEq,Debug)]
enum Act{
    PutRail(usize,P),
    PutStation(P),
    Wait,
}
use Act::*;


impl std::fmt::Display for Act{
    fn fmt(&self,fmt:&mut std::fmt::Formatter)->std::fmt::Result{
        match self{
            PutRail(dir,P{i,j})=>write!(fmt,"{dir} {i} {j}"),
            PutStation(P{i,j})=>write!(fmt,"0 {i} {j}"),
            Wait=>write!(fmt,"-1"),
        }
    }
}


static_data!{
    get_input:Input|{
        proconio::input!{
            n:usize,
            m:usize,
            k:i64,
            t:usize,
            pair:[(usize,usize,usize,usize);m],
        }
        assert!((n,t)==(N,T));

        let mut pair=pair.into_iter()
            .map(|(is,js,it,jt)|{
                let s=P::new(is,js);
                let t=P::new(it,jt);
                (s,t,s.manh(t) as i64)
            })
            .collect_vec();
        pair.sort_unstable_by_key(|t|-t.2);
        Input{M:m,K:k,pair}
    }
}



static_data!{
    get_neighbor:Vec<Vec<Vec<P>>>|{
        let mut neighbor=vec![vec![vec![];N];N];
        let is_candidate=get_is_candidate();
        
        for p in iter_pos(){
            for &dd in &DX[1..]{ //todo
                let np=p+dd;
                if np.in_range() && is_candidate[np]{
                    neighbor[p].push(np);
                }
            }
        }

        neighbor
    }
}



static_data!{
    get_people:Vec<Vec<Vec<usize>>>|{
        let pair=&get_input().pair;

        let mut start=vec![vec![vec![];N];N];
        let mut goal=vec![vec![vec![];N];N];
        for (i,&(s,t,_)) in pair.iter().enumerate(){
            start[s].push(i<<1);
            goal[t].push((i<<1)|1);
        }

        let mut people=vec![vec![vec![];N];N];

        for p in iter_pos(){
            for d in DX{
                let np=p+d;
                if np.in_range(){
                    people[p].extend(&start[np]);
                    people[p].extend(&goal[np]);
                }
            }
        }
        
        people
    }
}



static_data!{
    get_people_neighbor:Vec<Vec<P>>|{
        let input=get_input();
        let neighbor=get_neighbor();
        let mut a=vec![];

        for i in 0..input.M{
            let mut t=neighbor[input.pair[i].0].clone();
            t.push(input.pair[i].0);
            a.push(t);

            let mut t=neighbor[input.pair[i].1].clone();
            t.push(input.pair[i].1);
            a.push(t);
        }

        a
    }
}



static_data!{
    get_candidates:Vec<P>|{
        let neighbor=get_people();
        iter_pos().filter(|&p|!neighbor[p].is_empty()).collect_vec()
    }
}



static_data!{
    get_is_candidate:[[bool;N];N]|{
        let cands=get_candidates();
        let mut is_cand=[[false;N];N];
        for &p in cands{
            is_cand[p]=true;
        }
        is_cand
    }
}



const N_STATION:usize=6; // todo




#[derive(Clone,Copy,Debug)]
struct D{
    ord:[P;N_STATION],
    a:[(P,P);N_STATION-1],
    money:i64,
    turn:usize,
    income:i64,
    hash:u64,
    phash:u64,
    score:i64,
}



static_data!{
    get_zob:Vec<u64>|{
        let input=get_input();
        (0..input.M*2).map(|_|rnd::next64()).collect_vec()
    }
}



static_data!{
    get_start_candidates:Vec<(Vec<P>,i64,i64)>|{
        timer!();
        let X=60000; // todo
        let WIDTH=200; // todo
    
        let eval=|d:&D|->i64{
            (X-d.score+d.income-1)/d.income+d.turn as i64
        };
    
        let input=get_input();
        let people=get_people();
        let cands=get_candidates();
        let zob=get_zob();
    
        let mut set=FxHashSet::default();
        let mut pair=vec![false;input.M*2];
        let mut t0=vec![];
        let mut t1=vec![];
        let mut next=vec![];
        let mut beam=vec![];

        let mut ans=vec![];
        
        for i in 0..cands.len(){
            for j in 0..i{
                let p=cands[i];
                let q=cands[j];
    
                let mut money=input.K-STATION_COST*2-RAIL_COST*(p.manh(q)-1) as i64;
                if 0<=money{
                    t0.clear();
                    let mut income=0;
                    let mut hash=0;
                    let mut phash=0;

                    for p in [p,q]{
                        for &n in &people[p]{
                            if !pair[n]{
                                t0.push(n);
                                hash^=zob[n];
                                pair[n]=true;
                                if pair[n^1]{
                                    income+=input.pair[n>>1].2;
                                    phash^=zob[n>>1];
                                }
                            }
                        }
                    }

                    if 0<income{
                        let turn=p.manh(q)+1;
                        money+=income;
    
                        let mut ord=[NULL;N_STATION];
                        ord[0]=p;
                        ord[1]=q;
    
                        let mut a=[(NULL,NULL);N_STATION-1];
                        a[0]=(p,q);
                        
                        let new=D{
                            ord,a,
                            money,turn,income,score:income,
                            hash,phash,
                        };
                        next.push(new);
                    }
    
                    for &n in &t0{
                        pair[n]=false;
                    }
                }
            }
        }
    
        next.sort_unstable_by_key(|t|eval(t));
    
        for &d in &next{
            if set.insert(d.hash){
                beam.push(d);
                if beam.len()>=WIDTH{
                    break;
                }
            }
        }
    
        for i in 0..N_STATION.saturating_sub(2){
            next.clear();
    
            for d in &beam{
                let ps=&d.a[..i+1];
                t0.clear();
    
                for &p in ps{
                    for p in [p.0,p.1]{
                        for &n in &people[p]{
                            if !pair[n]{
                                pair[n]=true;
                                t0.push(n);
                            }
                        }
                    }
                }

                for &p in cands{
                    t1.clear();
                    let mut income=d.income;
                    let mut hash=d.hash;
                    let mut phash=d.phash;

                    for &n in &people[p]{
                        if !pair[n]{
                            t1.push(n);
                            hash^=zob[n];
                            pair[n]=true;
                            if pair[n^1]{
                                income+=input.pair[n>>1].2;
                                phash^=zob[n>>1];
                            }
                        }
                    }

                    if income!=d.income{
                        let mut money=d.money;
                        let mut turn=d.turn;
                        let mut score=d.score;
                        let mut a=d.a;

                        let rails;
                        if let Some(j)=(0..ps.len()).find(|&j|{
                            let (u,v)=ps[j];
                            u.manh(v)==p.manh(u)+p.manh(v)
                        }){
                            let (u,v)=ps[j];
                            a[j]=(u,p);
                            a[i+1]=(p,v);

                            rails=0;
                        } else{
                            let mut minp=NULL;
                            for &(u,v) in ps{
                                for np in [u,v]{
                                    if minp==NULL || p.manh(np)<p.manh(minp){
                                        minp=np;
                                    }
                                }
                            }

                            rails=p.manh(minp)-1;
                            a[i+1]=(p,minp);
                        }

                        money+=(d.income-RAIL_COST)*rails as i64-STATION_COST;
                        score+=d.income*rails as i64;

                        if money<0{
                            assert!(d.income!=0);

                            let wait=((-money+d.income-1)/d.income) as usize;
                            money+=d.income*wait as i64;
                            score+=d.income*wait as i64;
                            turn+=wait;
                        }

                        money+=income;
                        score+=income;
                        turn+=rails+1;

                        let mut ord=d.ord;
                        ord[i+2]=p;

                        let new=D{
                            ord,a,money,turn,income,hash,phash,score
                        };
                        next.push(new);
                    }

                    for &n in &t1{
                        pair[n]=false;
                    }
                }

                for &n in &t0{
                    pair[n]=false;
                }
            }
    
            next.sort_unstable_by_key(|t|eval(t));
            
            if i+3==N_STATION{
                break;
            }
        
            beam.clear();
            set.clear();
            for &d in &next{
                if set.insert(d.hash){
                    beam.push(d);
                    if beam.len()>=WIDTH{
                        break;
                    }
                }
            }
        }

        set.clear();
        let mut add=0;
        // eprintln!("{:?}",next[0]);
        for &d in &next{
            if set.insert(d.phash){
                let a=d.ord.iter().copied().collect();
                ans.push((a,d.income,d.money+d.income*(T-d.turn) as i64));
                add+=1;
                if add>=100{ // todo
                    break;
                }
            }
        }
    
        ans
    }
}



const NULL:P=P{i:!0,j:!0};



#[derive(Clone,Copy,PartialEq,Eq,Debug,Hash,PartialOrd,Ord)]
struct P{
    i:usize,
    j:usize,
}
impl P{
    fn new(i:usize,j:usize)->P{
        P{i,j}
    }

    fn manh(self,p:P)->usize{
        fn abs_diff(a:usize,b:usize)->usize{
            (a as i64-b as i64).abs() as usize
        }
        abs_diff(self.i,p.i)+abs_diff(self.j,p.j)
    }

    fn det(self,p:P)->i64{
        (self.i*p.j-self.j*p.i) as i64
    }
    
    fn in_range(self)->bool{
        self.i<N && self.j<N
    }
    
    fn get_area(self,p:P)->i64{
        ((self.i-p.i) as i64*(self.j-p.j) as i64).abs()
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

impl<T> std::ops::Index<P> for Vec<Vec<T>>{
    type Output=T;
    fn index(&self,idx:P)->&T{
        &self[idx.i][idx.j]
    }
}
impl<T> std::ops::IndexMut<P> for Vec<Vec<T>>{
    fn index_mut(&mut self,idx:P)->&mut T{
        &mut self[idx.i][idx.j]
    }
}

impl<T> std::ops::Index<P> for [[T;N];N]{
    type Output=T;
    fn index(&self,idx:P)->&T{
        &self[idx.i][idx.j]
    }
}
impl<T> std::ops::IndexMut<P> for [[T;N];N]{
    fn index_mut(&mut self,idx:P)->&mut T{
        &mut self[idx.i][idx.j]
    }
}


fn iter_pos()->impl Iterator<Item=P>{
    iproduct!(0..N,0..N).map(|(i,j)|P::new(i,j))
}


fn is_line(p:P,q:P)->bool{
    p.i==q.i || p.j==q.j
}



// 端点で交差するときはfalse
// 共線は一点より多くの点を共有するならtrue
// 線分が一点の場合はfalse
fn is_cross(p0:P,p1:P,q0:P,q1:P)->bool{
    let d0=(q0-p0).det(p1-p0);
    let d1=(q1-p0).det(p1-p0);
    let d2=(p0-q0).det(q1-q0);
    let d3=(p1-q0).det(q1-q0);

    if p0==p1 || q0==q1{
        return false;
    }

    if d0|d1|d2|d3==0{
        let pi_min=p0.i.min(p1.i);
        let pi_max=p0.i.max(p1.i);
        let qi_min=q0.i.min(q1.i);
        let qi_max=q0.i.max(q1.i);

        let pj_min=p0.j.min(p1.j);
        let pj_max=p0.j.max(p1.j);
        let qj_min=q0.j.min(q1.j);
        let qj_max=q0.j.max(q1.j);

        return (pi_min<qi_max && qi_min<pi_max) || (pj_min<qj_max && qj_min<pj_max);
    }

    d0*d1<0 && d2*d3<0
}



fn on_line(a:P,b:P,p:P)->bool{
    (a-p).det(b-p)==0
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



#[macro_export]
macro_rules! timer{
    ()=>{
        #[cfg(local)] let _timer=Timer(get_time());
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



#[derive(Clone)]
struct IndexSet{
    pos:Vec<usize>,
    que:Vec<usize>,
    len:usize,
}
impl IndexSet{
    fn new(n:usize)->Self{
        Self{
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

    fn contains(&self,v:usize)->bool{
        self.pos[v]<self.len
    }

    fn fill(&mut self){
        self.len=self.pos.len();
    }
}




// todo
#[macro_export]#[cfg(not(local))]macro_rules! assert{($($_:tt)*)=>{}}
#[macro_export]#[cfg(not(local))]macro_rules! assert_eq{($($_:tt)*)=>{}}
#[macro_export]#[cfg(not(local))]macro_rules! assert_ne{($($_:tt)*)=>{}}


