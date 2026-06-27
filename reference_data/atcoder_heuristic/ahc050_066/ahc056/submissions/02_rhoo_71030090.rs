#![allow(non_snake_case)]



fn main(){
    get_time();
    let input=get_input();
    let complex=input.tour.len()*input.N*input.N;
    eprintln!("complex = {complex}");
    
    let mut ans=(INF,(!0,vec![],vec![]));
    if complex<12000{
        let multi=if complex<7000{
            4
        } else if complex<10000{
            3
        } else{
            2
        };
        for tl in multi_start(multi,1.95){ // todo
            let time=get_time();
            let hill_climb_tl=time+(tl-time)*0.9; // todo
            let res=solve(hill_climb_tl,tl);

            let score=res.2.len()+res.2[0].len();
            eprintln!("#{score}");
            if ans.0>score{
                ans=(score,res);
            }
        }
    } else{
        let need=get_need_turn()[0];
        let hill_climb_tl=if need<1500{ // todo
            1.8
        } else{
            1.6
        };

        ans.1=solve(hill_climb_tl,1.95);
        ans.0=ans.1.2.len()+ans.1.2[0].len();
    }

    let (init_q,init_c,trans)=ans.1;
    let (cs,qs)=(trans.len(),trans[0].len());
    // eprintln!("cs = {}",cs);
    // eprintln!("qs = {}",qs);
    eprintln!("score = {}",(cs+qs)*100);

    output(init_q,&init_c,&trans);
    // check(init_q,&init_c,&trans);
}



fn solve(hill_climb_tl:f64,greedy_tl:f64)->(usize,Vec<usize>,Vec<Vec<Item>>){
    let (rot,free,walk)=hill_climb(hill_climb_tl);
    let (segs,need,dd)=make_plan(&rot,&free,&walk);

    let get_qs=|cs:usize|{
        let mut qs=[0;4];
        for i in 0..4{
            assert!(1<=need[i]);
            qs[i]=need[i].div_ceil(cs);
        }

        let rem=qs.iter().sum::<usize>()*cs-dd.iter().sum::<usize>();
        let mut undef=need[4].saturating_sub(rem);
        while undef>0{
            let arg=(0..4).min_by_key(|&i|qs[i]).unwrap();
            qs[arg]+=1;
            undef=undef.saturating_sub(cs);
        }
        qs
    };

    let mut seen=HashSet::default();
    
    let mut cand=(1..100).map(|cs|(cs,get_qs(cs))).collect_vec();
    let (bound,bound_cnt)={
        let arg=(0..cand.len()).min_by_key(|&i|cand[i].0+cand[i].1.iter().sum::<usize>()).unwrap();
        let (cs,cnt)=cand[arg];
        let bound=cs+cnt.iter().sum::<usize>();
        (bound,cnt)
    };

    let mut ans=(!0,vec![],vec![]);
    let mut score=INF;
    let get_score=|trans:&[Vec<Item>]|trans.len()+trans[0].len();
    
    'a: for _ in 0..{
        let Some(arg)=(0..cand.len()).min_by_key(|&i|cand[i].0+cand[i].1.iter().sum::<usize>()) else{break};
        let cnt=cand.swap_remove(arg).1;

        for k in [0.65,0.775,0.9]{ // todo
            if get_time()>greedy_tl{
                break 'a;
            }

            let cnt_mini=cnt.map(|a|(a as f64*k).round() as usize);
            if seen.insert(cnt_mini){
                let new=make_trans(&rot,&free,&walk,&segs,cnt_mini,greedy_tl);
                let new_score=get_score(&new.2);
                if score>new_score{
                    score=new_score;
                    ans=new;
                }
            }
        }
    }

    if score>bound && seen.insert(bound_cnt){
        let new=make_trans(&rot,&free,&walk,&segs,bound_cnt,greedy_tl);
        let new_score=get_score(&new.2);
        if score>new_score{
            // eprintln!("diff = {}",score-new_score);
            score=new_score;
            ans=new;
        }
    }
    assert!(score!=INF);

    ans
}



fn make_trans(rot:&[usize],free:&[bool],walk:&[usize],segs:&[Seg],cnt:[usize;4],tl:f64)->(usize,Vec<usize>,Vec<Vec<Item>>){
    assert!(1<=*cnt.iter().min().unwrap());
    let qs=cnt.iter().sum::<usize>();
    let M=segs.len();

    let mut trans=vec![vec![NULL_ITEM;qs];3];
    let mut dirs=vec![vec![];4];
    {
        let mut is=[0;5];
        for dir in 0..4{
            is[dir+1]=is[dir]+cnt[dir];
            for q in is[dir]..is[dir+1]{
                dirs[dir].push(q);
                trans[0][q]=Item{
                    c:0,q,dir,
                };
            }
        }
    }

    let N=get_input().N;
    let mut init_c=vec![!0;N*N];
    for p in 0..N*N{
        if !free[p]{
            init_c[p]=rot_to_color(rot[p]);
        }
    }

    for dir in 0..4{
        for (i,&q) in dirs[dir].iter().enumerate(){
            let nq={
                let a=&dirs[dir^1];
                a[i%a.len()]
            };
            trans[1][q]=Item{
                c:1,q:nq,dir:dir^1,
            };

            let nq={
                let a=&dirs[dir^3];
                a[a.len()-1-i%a.len()]
            };
            trans[2][q]=Item{
                c:2,q:nq,dir:dir^3,
            };
        }
    }

    if M==0{
        let mut dir=get_dir(walk[walk.len()-2],walk[walk.len()-1]);
        for &p in walk[..walk.len()-1].iter().rev(){
            dir^=rot[p];
        }
        return (dirs[dir][0],init_c,trans);
    }

    let pos_next=(0..M).map(|i|(i+1..M).find(|&j|segs[i].p==segs[j].p).unwrap_or(!0)).collect_vec();

    let mut dir_type=vec![(!0,!0,!0);M];
    {
        let mut freq=HashMap::default();

        for i in 0..M{
            let pd=segs[i].d1;
            let d=segs[i].dir;
            let nd=if i+1<M{
                segs[i+1].d0
            } else{
                let i0=segs[i].i1+1;
                if i0+1<walk.len(){
                    let mut d=get_dir(walk[walk.len()-2],walk[walk.len()-1]);
                    for &p in walk[i0..walk.len()-1].iter().rev(){
                        d^=rot[p];
                    }
                    d
                } else{
                    !0
                }
            };

            dir_type[i]=(pd,d,nd);
            if pd!=!0 && nd!=!0{
                *freq.entry((pd,d,nd)).or_insert(0)+=1;
            }
        }

        for i in 0..M{
            let (pd,d,nd)=dir_type[i];
            if pd==!0 || nd==!0{
                let mut max=(0,(!0,!0,!0));
                for pd_ in 0..4{
                    for nd_ in 0..4{
                        if (pd==!0 || pd==pd_) && (nd==!0 || nd==nd_){
                            let cnt=*freq.get(&(pd_,d,nd_)).unwrap_or(&0);
                            if max.0<=cnt{
                                max=(cnt,(pd_,d,nd_));
                            }
                        }
                    }
                }

                assert!(max.1!=(!0,!0,!0));
                dir_type[i]=max.1;
                *freq.entry(max.1).or_insert(0)+=1;
            }
        }
    }

    let init_dir=(0..M).map(|i|{
        let mut dir=segs[i].d0;
        if dir==!0{
            if i==0{
                dir=dir_type[i].0;
            } else{
                dir=dir_type[i-1].2;
            }
        }
        dir
    }).collect_vec();

    let mut res=vec![vec![];M];
    {
        for i in 0..M{
            let comp=&segs[i].comp;
            for &(mut q) in &dirs[init_dir[i]]{
                for &rot in comp{
                    q=trans[rot_to_color(rot)][q].q;
                }
                res[i].push(q);
            }
        }
    }

    let mut same=Dsu::new(M);
    let mut q_sub=Dsu::new(M);
    let mut q_same=Dsu::new(M);
    let mut c_same=Dsu::new(M);

    cap!{
        #![M:usize,qs:usize,&init_dir:[usize],&dirs:[Vec<usize>],&res:[Vec<usize>],&same:Dsu,&q_sub:Dsu,&q_same:Dsu,&c_same:Dsu]

        // cs,c_assign,q_assign
        fn solve()->Option<(Vec<usize>,Vec<(usize,usize)>)>{
            for &ci in c_same.live.to_slice(){
                let cg=&c_same.gs[ci];
                for i in 0..cg.len(){
                    for j in 0..i{
                        if q_same.same(cg[i],cg[j]) && !same.same(cg[i],cg[j]){
                            return None;
                        }
                    }
                }
            }

            let mut fix=vec![!0;M];
            let mut at=0;
            let mut vis=vec![0;M];

            // q_sub_gs は連動する
            cap!{
                #![&res:[Vec<usize>],same:&Dsu,q_sub:&Dsu,&mut fix:[usize],at:usize,&mut vis:[usize]]

                fn rec(v:usize,i:usize)->bool{
                    if vis[v]==at{
                        return fix[v]==i;
                    }
                    vis[v]=at;
                    assert!(i<res[v].len());
                    fix[v]=i;

                    let nv=q_sub.next[v];
                    assert!(res[v].len()==res[nv].len());
                    if !rec!(nv,i){
                        return false;
                    }

                    let nv=same.next[v];
                    let q=res[v][i];
                    if fix[nv]==!0{
                        if let Some(ni)=(0..res[nv].len()).filter(|&ni|res[nv][ni]==q).min_by_key(|_|rnd::next()){
                            if !rec!(nv,ni){
                                return false;
                            }
                        } else{
                            return false;
                        }
                    } else if res[nv][fix[nv]]!=q{
                        return false;
                    }
                    true
                }
            }

            let mut q_cand=vec![];
            for &gi in q_same.live.to_slice(){
                let group=&q_same.gs[gi];
                let mut new=vec![];
                for i in 0..res[group[0]].len(){
                    fix.fill(!0);
                    at+=1;
                    if rec!(group[0],i){
                        new.push(group.iter().map(|&j|(dirs[init_dir[j]][fix[j]],res[j][fix[j]])).collect_vec());
                    }
                }
                if new.is_empty(){
                    return None;
                }
                q_cand.push((group,new));
            }

            let mut used=vec![vec![!0;qs];c_same.live.len()];
            let mut is=(0..q_cand.len()).collect_vec();
            is.sort_by_cached_key(|&i|{
                let mut gs=q_cand[i].0.iter().map(|&i|same.leader(i)).collect_vec();
                gs.sort_unstable();
                gs.dedup();
                (!gs.len(),q_cand[i].1.len())
            });
            let mut cnt=vec![0;qs];
            let mut q_assign=vec![(!0,!0);M];

            for &i in &is{
                let tar=q_cand[i].0;
                let mut min=(!0,vec![INF;qs],vec![INF;qs]);

                'a: for j in 0..q_cand[i].1.len(){
                    let new=&q_cand[i].1[j];
                    for (&a,&b) in izip!(tar,new){
                        let used=used[c_same.id(a)][b.1];
                        if used!=!0 && used!=same.id(a){
                            continue 'a;
                        }
                    }

                    let mut new_cnt=cnt.clone();
                    for (&a,&b) in izip!(tar,new){
                        if same.leader(a)==a{
                            new_cnt[b.1]+=1;
                        }
                    }

                    let mut new_sort=new_cnt.clone();
                    new_sort.sort_unstable_by_key(|t|!t);

                    if min.2>new_sort{
                        min=(j,new_cnt,new_sort);
                    }
                }

                if min.0==!0{
                    return None;
                }

                cnt=min.1;
                let new=&q_cand[i].1[min.0];
                for (&a,&b) in izip!(tar,new){
                    assert!(used[c_same.id(a)][b.1]==!0 || used[c_same.id(a)][b.1]==same.id(a));
                    used[c_same.id(a)][b.1]=same.id(a);
                    q_assign[a]=b;
                }
            }

            let mut c_assign=vec![!0;M];
            let mut is=c_same.live.to_slice().to_vec();
            is.sort_by_cached_key(|&i|{
                let mut gs=c_same.gs[i].iter().map(|&i|same.leader(i)).collect_vec();
                gs.sort_unstable();
                gs.dedup();
                !gs.len()
            });

            let mut qi=vec![vec![];qs];
            for i in 0..M{
                qi[q_assign[i].1].push(i);
            }

            let mut ng=HashSet::default();
            for &i in &is{
                ng.clear();
                for &j in &c_same.gs[i]{
                    let q=q_assign[j];
                    for &ni in &qi[q.1]{
                        ng.insert(c_assign[ni]);
                    }
                }

                let mex=(0..).find(|&i|!ng.contains(&i)).unwrap();
                for &j in &c_same.gs[i]{
                    c_assign[j]=mex;
                }
            }

            // for i in 0..M{
            //     for j in 0..M{
            //         let (ci,(_,qi))=(c_assign[i],q_assign[i]);
            //         let (cj,(_,qj))=(c_assign[j],q_assign[j]);
            //         assert!(same.same(i,j)==((ci,qi)==(cj,qj)));
            //     }
            // }

            Some((c_assign,q_assign))
        }
    }
    
    let mut gs=HashMap::default();
    for i in 0..M{
        gs.entry(dir_type[i]).or_insert_with(||vec![]).push(i);
    }

    let mut pair=vec![vec![];4];
    for (&k,g) in &gs{
        for i in 0..g.len(){
            for j in 0..i{
                pair[k.0].push((g[i],g[j]));
            }
        }
    }

    // let mut tmp=vec![];
    let mut ans=solve!().unwrap();
    loop{
        let mut c_max=[0;4];
        for i in 0..M{
            let t=&mut c_max[trans[0][ans.1[i].1].dir];
            *t=ans.0[i].max(*t);
        }
        let Some(dir)=(0..4).filter(|&dir|!pair[dir].is_empty()).max_by_key(|&dir|(c_max[dir],rnd::next())) else{
            break;
        };

        if get_time()>=tl{
            break;
        }
        let arg=(0..30).map(|_|rnd::get(pair[dir].len())).min_by_key(|&i|{
            let (u,v)=pair[dir][i];

            same.get_group(u).len().max(same.get_group(v).len())
        }).unwrap();
        let (u,v)=pair[dir].swap_remove(arg);

        if same.same(u,v){
            continue;
        }

        let snap=(same.snap(),q_sub.snap(),q_same.snap(),c_same.snap());
        let (ug,vg)=(same.get_group(u),same.get_group(v));

        if let Some(ur)=ug.iter().map(|&a|a+1).find(|&a|a<M){
            if let Some(vr)=vg.iter().map(|&a|a+1).find(|&a|a<M){
                q_sub.merge(ur,vr);
                q_same.merge(ur,vr);
            }
        }

        if let Some(ur)=ug.iter().map(|&a|pos_next[a]).find(|&a|a<M){
            if let Some(vr)=vg.iter().map(|&a|pos_next[a]).find(|&a|a<M){
                c_same.merge(ur,vr);
            }
        }

        same.merge(u,v);
        q_same.merge(u,v);
        c_same.merge(u,v);

        let get_score=|ans:&(Vec<usize>,Vec<(usize,usize)>)|->(usize,usize){
            let c_max=*ans.0.iter().max().unwrap();
            // todo
            // let mut a=(0..M).filter(|&i|ans.0[i]==c_max).map(|i|ans.1[i].1).collect_vec();
            // a.sort_unstable();
            // a.dedup();
            // (c_max,a.len())
            (c_max,0)
        };

        if let Some(new)=solve!() && get_score(&ans)>=get_score(&new){
            ans=new;
        } else{
            same.revert(snap.0);
            q_sub.revert(snap.1);
            q_same.revert(snap.2);
            c_same.revert(snap.3);
        }
    }

    // for i in 0..M{
    //     for j in 0..M{
    //         if same.same(i,j){
    //             assert!(c_same.same(i,j));
    //             if pos_next[i]<M && pos_next[j]<M{
    //                 assert!(c_same.same(pos_next[i],pos_next[j]));
    //             }
    //             assert!(q_same.same(i,j));
    //             if i+1<M && j+1<M{
    //                 assert!(q_same.same(i+1,j+1));
    //             }
    //         }
    //     }
    // }

    let (mut c_assign,q_assign)=ans;
    c_assign.iter_mut().for_each(|t|*t+=3);
    let cs=c_assign.iter().max().unwrap()+1;

    trans.resize(cs,vec![NULL_ITEM;qs]);
    for i in 0..M{
        let (c,q)=(c_assign[i],q_assign[i].1);
        trans[c][q].dir=dir_type[i].1;

        let nc=if pos_next[i]!=!0{
            c_assign[pos_next[i]]
        } else{
            !0
        };
        if trans[c][q].c==!0{
            trans[c][q].c=nc;
        }

        let nq=if i+1<M{
            q_assign[i+1].0
        } else{
            dirs[dir_type[M-1].2][0]
        };

        if trans[c][q].q==!0{
            trans[c][q].q=nq;
        }
    }

    for p in 0..N*N{
        if let Some(i)=(0..M).find(|&i|segs[i].p==p){
            assert!(free[p]);
            init_c[p]=c_assign[i];
        }
    }

    let init_q=q_assign[0].0;
    (init_q,init_c,trans)
}



// segs,need,cnt
fn make_plan(rot:&[usize],free:&[bool],walk:&[usize])->(Vec<Seg>,[usize;5],[usize;4]){
    let is=(0..walk.len()-1).filter(|&i|free[walk[i]]).collect_vec();
    let mut dd=[0usize;4];
    let mut rots=[[0;4];4];
    let mut i0=0;
    
    let mut segs=vec![];
    let mut undef=0usize;

    for &i1 in &is{
        let p=walk[i1];
        let dir=get_dir(walk[i1],walk[i1+1]);
        
        if i0==i1{
            let seg=Seg{
                p,i0,i1,dir,d0:!0,comp:vec![],d1:!0,
            };
            segs.push(seg);

            undef+=1;

        } else{
            let d1=get_dir(walk[i1-1],walk[i1]);
            dd[d1]+=1;

            let mut vis=1<<d1;
            let mut d0=d1;
            let mut comp=vec![];

            for &p in walk[i0..i1].iter().rev(){
                assert!(!free[p]);
                if rot[p]!=0{
                    comp.push(rot[p]);
                }
                d0^=rot[p];
                vis|=1<<d0;
            }
            for i in 0..4{
                if vis>>i&1==1{
                    rots[i][d1]+=1;
                }
            }

            comp.reverse();
            let seg=Seg{
                p,i0,i1,dir,d0,comp,d1,
            };
            segs.push(seg);
        }
        i0=i1+1;
    }

    let mut max=[0;5];
    max[4]=undef;
    for i in 0..4{
        for j in 0..4{
            max[i]=max[i].max(rots[i][j]);
        }
    }
    for i in 0..4{
        max[i]=max[i].max(dd[i]).max(1);
    }
    
    (segs,max,dd)
}



#[derive(Clone,Debug)]
// i1のfreeを踏んだときのこと
// i0-1がfree i0..i1 はfreeなし
// i0 → i0+1 がd0
// i1-1 → i1 がd1
struct Seg{
    p:usize,
    i0:usize,
    i1:usize,
    dir:usize,
    d0:usize,
    comp:Vec<usize>,
    d1:usize,
}


fn rot_to_color(rot:usize)->usize{
    assert!(rot!=2);
    [0,1,!0,2][rot]
}



const NULL_ITEM:Item=Item{c:!0,q:!0,dir:!0};


#[proconio::fastout]
fn output(init_q:usize,init_c:&[usize],trans:&[Vec<Item>]){
    let N=get_input().N;
    let adjust=|q:usize|{
        if q==0{
            init_q
        } else if q==init_q{
            0
        } else{
            q
        }
    };

    let cs=trans.len();
    let qs=trans[0].len();

    println!("{} {} {}",cs,qs,cs*qs);
    for i in 0..N{
        for j in 0..N{
            let mut c=init_c[i*N+j];
            if c==!0{
                c=0;
            }
            print!("{} ",c);
        }
        println!();
    }
    for i in 0..cs{
        for j in 0..qs{
            let Item{mut c,mut q,mut dir}=trans[i][j];
            if c==!0{
                c=0;
            }
            if q==!0{
                q=0;
            }
            if dir==!0{
                dir=0;
            }
            println!("{} {} {} {} {}",i,adjust(j),c,adjust(q),DC[dir]);
        }
    }
}



fn get_dir(p:usize,q:usize)->usize{
    let input=get_input();
    (0..4).find(|&dd|p+input.DD[dd]==q).unwrap()
}



fn hill_climb(tl:f64)->(Vec<usize>,Vec<bool>,Vec<usize>){
    let input=get_input();
    let N=input.N;
    let is_wall=&input.is_wall;
    let tour=&input.tour;

    let mut que=vec![VecDeque::new();2];
    let mut vis=vec![[0;4];N*N];
    let mut it=0;
    let mut fail=0;

    #[derive(Clone,Copy)]
    struct Node{
        dir:usize,
        p:usize,
        turn:usize,
        step:u16,
        cost:f64,
        dd:[u16;4],
    }

    let DD=input.DD;
    let mut eval=|rot:&[usize],is_free:&[bool],k:f64|->(usize,usize){
        it+=1;
        for i in 0..que.len(){
            que[i].clear();
        }
        
        let mut start=Node{
            dir:0,p:tour[0],turn:1,
            step:0,cost:0.,dd:[0;4],
        };
        if is_free[start.p]{
            start.cost+=1.;
        }
        vis.fill([0;4]);
        vis[start.p].fill(1);
        for dir in 0..4{
            start.dir=dir;
            que[0].push_back(start);
        }

        loop{
            let Some(arg)=(0..que.len()).filter(|&i|!que[i].is_empty()).min_by_key(|&i|que[i][0].cost.ord()) else{
                break;
            };
            
            let Node{dir,p,turn,step,cost,dd,..}=que[arg].pop_front().unwrap();
            
            if turn<vis[p][dir] || step>=input.T as u16{
                continue;
            }

            macro_rules! mov{
                ($np:expr,$cd:expr,$dir:expr)=>{
                    let turn=turn+(tour[turn]==$np) as usize;
                    let step=step+1;
                    if turn>=tour.len(){
                        let cnt=dd.iter().sum::<u16>();
                        return (cnt as usize,step as usize);
                    }
                    
                    if vis[$np][$dir]<turn{
                        vis[$np][$dir]=turn;
                        let mut cost=cost+k;
                        let mut dd=dd;

                        if is_free[$np]{
                            dd[$cd]+=1;
                            cost+=1.;
                        }
                        
                        let node=Node{
                            dir:$dir,p:$np,
                            turn,step,cost,dd,
                        };
                        que[is_free[$np] as usize].push_back(node);
                    }
                }
            }
            
            if is_free[p]{
                for nd in 0..4{
                    if !input.is_wall[p][nd]{
                        let np=p+DD[nd];
                        if is_free[np]{
                            mov!(np,nd,0);
                        } else{
                            for dir in 0..4{
                                mov!(np,nd,dir);
                            }
                        }
                    }
                }
            } else{
                let nd=dir^rot[p];
                if !is_wall[p][nd]{
                    let np=p+DD[nd];
                    mov!(np,nd,nd);
                }
            }
        }
        fail+=1;

        (INF,INF)
    };

    let restore=|rot:&[usize],is_free:&[bool],k:f64|->Vec<usize>{
        let mut que=vec![VecDeque::new();2];
        let mut track=Track::new();

        let mut start=Node{
            dir:0,p:tour[0],turn:1,
            step:0,cost:0.,dd:[0;4],
        };
        if is_free[start.p]{
            start.cost+=1.;
        }
        let mut vis=vec![[0;4];N*N];
        vis[start.p].fill(1);
        let id=track.push(!0,start.p);
        for dir in 0..4{
            start.dir=dir;
            que[0].push_back((start,id));
        }
        
        loop{
            let Some(arg)=(0..que.len()).filter(|&i|!que[i].is_empty()).min_by_key(|&i|que[i][0].0.cost.ord()) else{
                break;
            };

            let (Node{dir,p,turn,step,cost,dd,..},id)=que[arg].pop_front().unwrap();
            
            if turn<vis[p][dir] || step>=input.T as u16{
                continue;
            }
            
            macro_rules! mov{
                ($np:expr,$cd:expr,$dir:expr)=>{
                    let turn=turn+(tour[turn]==$np) as usize;
                    let step=step+1;
                    if turn>=tour.len(){
                        let id=track.push(id,$np);
                        return track.restore(id);
                    }

                    if vis[$np][$dir]<turn{
                        vis[$np][$dir]=turn;
                        let mut cost=cost+k;
                        let mut dd=dd;

                        if is_free[$np]{
                            dd[$cd]+=1;
                            cost+=1.;
                        }

                        let id=track.push(id,$np);
                        let node=Node{
                            dir:$dir,p:$np,
                            turn,step,cost,dd,
                        };
                        que[is_free[$np] as usize].push_back((node,id));
                    }
                }
            }
            
            if is_free[p]{
                for nd in 0..4{
                    if !input.is_wall[p][nd]{
                        let np=p+DD[nd];
                        if is_free[np]{
                            mov!(np,nd,0);
                        } else{
                            for dir in 0..4{
                                mov!(np,nd,dir);
                            }
                        }
                    }
                }
            } else{
                let nd=dir^rot[p];
                if !is_wall[p][nd]{
                    let np=p+DD[nd];
                    mov!(np,nd,nd);
                }
            }
        }

        panic!();
    };

    let mut rot=vec![0;N*N];
    let deg=(0..N*N).map(|p|is_wall[p].iter().filter(|&&f|!f).count()).collect_vec();

    for p in 0..N*N{
        if deg[p]==2{
            let i0=(0..4).find(|&i|!is_wall[p][i]).unwrap();
            let i1=(0..4).rfind(|&i|!is_wall[p][i]).unwrap();
            assert!(i0!=i1);
            rot[p]=i0^i1^2;
        }
    }

    let mut free=vec![false;N*N];
    for i in 0..N{
        for j in 0..N{
            free[i*N+j]=(i^j)&1==1;
        }
    }

    let remain=get_remain();
    // todo
    let k=if remain<=0.1{
        1.
    } else{
        0.0001
    };

    let mut score=eval(&rot,&free,k);
    // assert!(score.1<=input.T);

    let mut ps=(0..N*N).collect_vec();
    // ps.sort_unstable_by_key(|&i|(i%N).abs_diff(N/2)+(i/N).abs_diff(N/2)); // todo
    rnd::shuffle(&mut ps);

    for &p in &ps{
        if free[p]{
            if get_time()>=tl{
                break;
            }
            
            free[p]=false;
            let new=eval(&rot,&free,k);
            if new.1<=input.T && score>=new{
                score=new;
            } else{
                free[p]=true;
            }
        }
    }

    // // todo
    // 'a: for &p in &ps{
    //     if !free[p] && rot[p]==0{
    //         for xor in [1,3]{
    //             if get_time()>=tl{
    //                 break 'a;
    //             }

    //             let old=rot[p];
    //             rot[p]=xor;
    //             let (new,ac)=eval(&rot,&free,k);
    //             if best.0>ac{
    //                 best=(ac,free.clone(),rot.clone());
    //             }
    //             if new.1<=input.T && score>=new{
    //                 score=new;
    //             } else{
    //                 rot[p]=old;
    //             }
    //         }
    //     }
    // }
    
    // eprintln!("initial = {}",score.0);
    let mut nei=vec![vec![];N*N];
    for p in 0..N*N{
        for d0 in 0..4{
            if !is_wall[p][d0]{
                nei[p].push(p+DD[d0]);
            }
        }
    }

    'a: loop{
        rnd::shuffle(&mut ps);
        for &p in &ps{
            if get_time()>=tl{
                break 'a;
            }

            if free[p]{
                if rnd::get(2)==0{ // todo
                    let np=nei[p].choice();

                    if free[np]{
                        continue;
                    }
                    free[p]=false;
                    free[np]=true;
                    let new=eval(&rot,&free,k);
                    if new.1<=input.T && score>=new{
                        score=new;
                    } else{
                        free[p]=true;
                        free[np]=false;
                    }
                } else{
                    free[p]=false;
                    let old=rot[p];
                    rot[p]=[0,0,0,1,3].choice(); // todo
                    
                    let new=eval(&rot,&free,k);
                    if new.1<=input.T && score>=new{
                        score=new;
                    } else{
                        free[p]=true;
                        rot[p]=old;
                    }
                }
            } else if rnd::get(30)==0{ // todo
                free[p]=true;
                let new=eval(&rot,&free,k);
                if new.1<=input.T && score>=new{
                    score=new;
                } else{
                    free[p]=false;
                }
            } else if rnd::get(3)==0 // todo
                && deg[p]!=2
            {
                let old=rot[p];
                let xor=[0,0,0,0,1,3].choice();
                if xor!=old{
                    rot[p]=xor;
                    let new=eval(&rot,&free,k);
                    if new.1<=input.T && score>=new{
                        score=new;
                    } else{
                        rot[p]=old;
                    }
                }
            }
        }
    }

    // eprintln!("it = {}",it);
    // eprintln!("fail = {:.3}",fail as f64/it as f64);
    // eprintln!("latest = {}",score.0);

    let walk=restore(&rot,&free,k);
    (rot,free,walk)
}



// fn check(init_q:usize,init_c:&[usize],trans:&[Vec<Item>]){
//     let input=get_input();
//     let mut p=input.tour[0];
//     let mut q=init_q;
//     let mut c=init_c.to_vec();
//     let mut i=1;

//     for _ in 0..input.T{
//         let next=&trans[c[p]][q];
//         c[p]=next.c;
//         q=next.q;
//         p+=input.DD[next.dir];
//         if p==input.tour[i]{
//             i+=1;
//             if i>=input.tour.len(){
//                 break;
//             }
//         }
//     }

//     assert!(i==input.tour.len());
// }



#[derive(Clone,Copy,PartialEq,Eq,Debug)]
struct Item{
    c:usize,
    q:usize,
    dir:usize,
}



use std::collections::*;
use rustc_hash::{FxHashSet as HashSet,FxHashMap as HashMap};
use itertools::*;


const INF:usize=1e8 as usize;
const DC:[char;5]=['L','U','R','D','S'];


struct Input{
    N:usize,
    K:usize,
    T:usize,
    is_wall:Vec<[bool;4]>,
    tour:Vec<usize>,
    DD:[usize;4],
}


static_data!{
    get_input:Input|{
        use proconio::marker::*;
        proconio::input!{
            N:usize,
            K:usize,
            T:usize,
            v:[Chars;N],
            h:[Chars;N-1],
            tour:[(usize,usize);K],
        }

        let mut is_wall=vec![[false;4];N*N];
        let (L,U,R,D)=(0,1,2,3);
        let DD=[(0,!0),(!0,0),(0,1),(1,0)];
        
        for i in 0..N{
            for j in 0..N{
                for dd in 0..4{
                    let np=(i+DD[dd].0,j+DD[dd].1);
                    if N<=np.0 || N<=np.1{
                        is_wall[i*N+j][dd]=true;
                    }
                }
                if j!=N-1 && v[i][j]=='1'{
                    is_wall[i*N+j][R]=true;
                    is_wall[i*N+j+1][L]=true;
                }
                if i!=N-1 && h[i][j]=='1'{
                    is_wall[i*N+j][D]=true;
                    is_wall[(i+1)*N+j][U]=true;
                }
            }
        }
        
        Input{
            N,K,T,is_wall,
            tour:tour.into_iter().map(|(i,j)|i*N+j).collect(),
            DD:[!0,N.wrapping_neg(),1,N],
        }
    }
}



static_data!{
    get_dist_table:Vec<Vec<usize>>|{
        let input=get_input();
        let N=input.N;
        let DD=input.DD;
        let is_wall=&input.is_wall;
        let mut que=VecDeque::new();

        let mut bfs=|s:usize|->Vec<usize>{
            que.clear();
            que.push_back(s);
            let mut dist=vec![!0;N*N];
            dist[s]=0;

            while let Some(v)=que.pop_front(){
                for dd in 0..4{
                    let nv=v+DD[dd];
                    if !is_wall[v][dd] && dist[nv]==!0{
                        dist[nv]=dist[v]+1;
                        que.push_back(nv);
                    }
                }
            }

            dist
        };

        (0..N*N).map(|s|bfs(s)).collect_vec()
    }
}



static_data!{
    get_need_turn:Vec<usize>|{
        let input=get_input();
        let tour=&input.tour;
        let dist=get_dist_table();

        let mut need=vec![0;tour.len()];
        for i in (0..tour.len()-1).rev(){
            need[i]=need[i+1]+dist[tour[i]][tour[i+1]];
        }

        need
    }
}



fn get_remain()->f64{
    let T=get_input().T as f64;
    1.-get_need_turn()[0] as f64/T
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



#[cfg(feature = "local")]
include!("/home/akatsuti/cp/lib/debug_mod.rs");
#[cfg(not(feature = "local"))]
#[macro_export]macro_rules! debug{($($t:tt)*)=>{}}



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



struct Dsu{
    gs:Vec<Vec<usize>>,
    live:IndexSet,
    id:Vec<usize>,
    track:Vec<(usize,usize)>,
    next:Vec<usize>,
}
impl Dsu{
    fn new(n:usize)->Dsu{
        let mut live=IndexSet::new(n);
        live.fill();
        Dsu{
            gs:(0..n).map(|i|vec![i]).collect_vec(),
            live,
            id:(0..n).collect_vec(),
            track:vec![],
            next:(0..n).collect_vec(),
        }
    }

    fn id(&self,v:usize)->usize{
        self.live.pos[self.id[v]]
    }

    fn leader(&self,v:usize)->usize{
        self.get_group(v)[0]
    }

    fn get_group(&self,v:usize)->&[usize]{
        &self.gs[self.id[v]]
    }

    fn same(&self,u:usize,v:usize)->bool{
        self.id[u]==self.id[v]
    }

    fn merge(&mut self,mut u:usize,mut v:usize){
        if self.id[u]==self.id[v]{
            self.track.push((!0,!0));
            return;
        }

        (u,v)=(self.id[u],self.id[v]);
        if self.gs[u].len()<self.gs[v].len(){
            (u,v)=(v,u);
        }
        self.next.swap(u,v);

        for i in 0..self.gs[v].len(){
            let i=self.gs[v][i];
            self.id[i]=u;
            self.gs[u].push(i);
        }
        self.live.rem(v);
        self.track.push((u,v));
    }

    fn revert(&mut self,until:usize){
        for &(u,v) in self.track[until..].iter().rev(){
            if (u,v)==(!0,!0){
                continue;
            }
            self.live.add(v);
    
            let ns=self.gs[u].len()-self.gs[v].len();
            self.gs[u].truncate(ns);
    
            for &i in &self.gs[v]{
                self.id[i]=v;
            }
            self.next.swap(u,v);
        }
        self.track.truncate(until);
    }

    fn snap(&self)->usize{
        self.track.len()
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



fn multi_start(mut n:usize,tl:f64)->impl Iterator<Item=f64>{
    std::iter::from_fn(move||{
        if n==0{
            return None;
        }
        let time=get_time();
        let ret=Some((tl-time)/n as f64+time);
        n-=1;
        ret
    })
}
