#![allow(non_snake_case,dead_code)]





fn main(){
    get_time();

    let input=get_input();

    // let w=if input.vias>=500{
    //     7
    // } else{
    //     6
    // };
    // let tour=make_tour(6,0.1); // todo

    // println!("{}",tour.len());
    // println!("{}",tour.iter().join(" "));

    // proconio::input!{
    //     tour:[usize]
    // }
    
    // println!("{}",input.vias);
    // for &p in &input.via{
    //     println!("{p}");
    // }
    // println!("{}",tour.len()-1);
    // for i in 1..tour.len(){
    //     println!("{} {}",tour[i-1],tour[i]);
    // }
    
    // assert!(tour.len()==input.vias);
    // let (root,g,goal,a,score)=solve(&tour,1.95);
    // eprintln!("score = {:.1}",score);
    
    let multi=if input.vias<=300{
        2
    } else{
        1
    };
    
    let mut best_score=f64::INFINITY;
    let mut best=(vec![],!0,vec![],vec![],vec![]);
    
    let mut it=0;
    for tl in multi_start(multi,1.95){
        let time=get_time();
        let tour=make_tour(6-it,time+(tl-time)*0.1);
        it+=1;
        let (root,g,goal,a,score)=solve(&tour,tl);

        eprintln!("#{score:.0}");

        if best_score>score{
            best_score=score;
            best=(tour,root,g,goal,a);
        }
    }

    eprintln!("score = {:.1}",best_score);
    output_ans(&best.0,best.1,&best.2,&best.3,&best.4);
}



// tour,graph,goal,selector
fn solve(tour:&[usize],tl:f64)->(usize,Vec<[usize;2]>,Vec<usize>,Vec<usize>,f64){
    let input=get_input();
    let n=tour.len();

    let gs=input.goals;
    let mut goal=(0..gs).collect_vec();
    
    let mut selector_cand=vec![vec![];gs];
    assert!(input.get_prob(input.probs-2)==&vec![1.;gs]);
    for i in 0..gs{
        for j in 0..input.probs-2{
            let p=input.get_prob(j)[i];
            let p=p.min(1.-p);
            for _ in 0..(1./p).round() as usize{ // todo
                selector_cand[i].push(j);
            }
        }
        for _ in 0..2{ // todo
            selector_cand[i].push(input.probs-1);
            selector_cand[i].push(input.probs-2);
        }
    }
    
    let ps=chain!(tour.iter().map(|&a|&input.via[a]),&input.goal,once(&input.start)).copied().collect_vec();
    // todo: ここらへんのパラメータは速度との兼ね合いだから頂点数に応じて変えたほうがいい
    let mut es=vec![];
    for i in 0..n{
        // let sub=if n.saturating_sub(10)<=i{
        //     20
        // } else{
        //     5
        // };
        let sub=10;
        for j in i.saturating_sub(sub)..(i+25).min(n){ // todo
            if i!=j{
                es.push((i,j));
            }
        }
    }
    let mut is=(0..n).collect_vec();
    for i in 0..gs{
        is.sort_unstable_by_key(|&j|ps[j].dist(input.goal[i]).ord());
        for &v in &is[..30.min(is.len())]{ // todo
            es.push((v,i+n));
        }
    }

    let sv=ps.len()-1;
    for i in 0..20{ // todo
        es.push((sv,i));
    }
    
    let start=get_time();
    let mut lines=Lines::new(&ps,es);
    let end=get_time();
    
    // eprintln!("elp = {:.3}",end-start);
    
    let mut a=(0..n).map(|_|rnd::get(input.probs)).collect_vec();
    let mut next=vec![[!0;2];n];

    let mut root=0;
    lines.add_edge(sv,root);
    
    for i in 0..n-1{
        next[i][0]=i+1;
        lines.add_edge(i,i+1);
    }

    for i in 0..n{
        let mut arg=!0;
        let mut dist=f64::INFINITY;

        for &j in lines.set[i].to_slice(){
            if i>j{
                continue;
            }
            
            let id=j+lines.si[i];
            let (u,v)=lines.es[id];
            assert!(u==i);

            let d=ps[u].dist(ps[v]);
            if dist>d{
                dist=d;
                arg=v;
            }
        }

        if arg==!0{
            next[i][1]=next[i][0];
        } else{
            lines.add_edge(i,arg);
            next[i][1]=arg;
            assert!(next[i][1]!=next[i][0]);
        }
    }

    // // todo: 初期解
    // for i in 0..n{
    //     for t in 0..2{
    //         let mut arg=!0;
    //         let mut dist=f64::INFINITY;
    
    //         for &j in lines.set[i].to_slice(){
    //             if i>j{
    //                 continue;
    //             }
    
    //             let id=j+lines.si[i];
    //             let (u,v)=lines.es[id];
    //             assert!(u==i);
    
    //             let d=ps[u].dist(ps[v]);
    //             if dist>d{
    //                 dist=d;
    //                 arg=v;
    //             }
    //         }

    //         if arg!=!0{
    //             lines.add_edge(i,arg);
    //             next[i][t]=arg;
    //         } else{
    //             next[i][t]=next[i][t^1];
    //         }
    //     }
    // }

    let mut fdp=vec![0.;(n+gs)*gs];
    let mut bdp=vec![0.;(n+gs)*gs];

    fdp[..gs].fill(1.);
    for v in 0..n{
        let prob=input.get_prob(a[v]);
        let [v0,v1]=next[v];

        for i in 0..gs{
            if v0!=!0{
                fdp[v0*gs+i]+=fdp[v*gs+i]*prob[i];
            }
            if v1!=!0{
                fdp[v1*gs+i]+=fdp[v*gs+i]*(1.-prob[i]);
            }
        }
    }

    let mut score=0.;
    for i in 0..gs{
        let v=i+n;
        bdp[v*gs+goal[i]]=1.;
        score+=fdp[v*gs+goal[i]];
    }
    let mut idx=n-1;

    let mut topo=(0..n+gs).collect_vec();
    let mut ord=(0..n+gs).collect_vec();
    let mut cnt=(0,0);
    let mut que=BinaryHeap::new();
    let mut dp=vec![0.;(n+gs)*gs];
    let mut updated=IndexSet::new(n+gs);
    let mut reach=vec![0;n+gs];
    let mut at=0;
    let mut t0=vec![];
    let mut t1=vec![];
    let mut t2=vec![];

    cap!{
        #![
            input:&Input,&goal:[usize],&selector_cand:[Vec<usize>],gs:usize,n:usize,root:usize,
            &mut cnt:(usize,usize),&mut idx:usize,&mut fdp:[f64],&mut bdp:[f64],
            &mut score:f64,&mut next:[[usize;2]],&mut lines:Lines,&mut a:[usize],
            &mut topo:[usize],&mut ord:[usize],&mut reach:[usize],&mut at:usize,
            &mut que:BinaryHeap<Rev<usize>>,&mut dp:[f64],&mut updated:IndexSet,
            &mut t0:Vec<usize>,&mut t1:Vec<usize>,&mut t2:Vec<usize>
        ]

        fn naive()->f64{
            let mut dp=vec![0.;(n+gs)*gs];
            dp[root*gs..(root+1)*gs].fill(1.);
            for &v in topo[..n].iter(){
                let prob=input.get_prob(a[v]);
                let [v0,v1]=next[v];

                assert!(v0==!0 || ord[v]<ord[v0]);
                assert!(v1==!0 || ord[v]<ord[v1]);

                for i in 0..gs{
                    if v0!=!0{
                        dp[v0*gs+i]+=dp[v*gs+i]*prob[i];
                    }
                    if v1!=!0{
                        dp[v1*gs+i]+=dp[v*gs+i]*(1.-prob[i]);
                    }
                }
            }

            let mut sum=0.;
            for i in 0..gs{
                assert!(topo[i+n]==i+n);
                sum+=dp[(i+n)*gs+goal[i]];
            }

            sum
        }

        #[track_caller]
        fn check_fdp(end:usize){
            let mut dp=vec![0.;(n+gs)*gs];
            dp[root*gs..(root+1)*gs].fill(1.);

            for &v in topo.iter(){
                if v==end{
                    break;
                }

                let prob=input.get_prob(a[v]);
                let [v0,v1]=next[v];
                for i in 0..gs{
                    if v0!=!0{
                        dp[v0*gs+i]+=dp[v*gs+i]*prob[i];
                    }
                    if v1!=!0{
                        dp[v1*gs+i]+=dp[v*gs+i]*(1.-prob[i]);
                    }
                }
            }

            for i in 0..dp.len(){
                assert!((dp[i]-fdp[i]).abs()<=1e-4);
            }
        }

        #[track_caller]
        fn check_bdp(start:usize){
            let mut dp=vec![0.;(n+gs)*gs];
            for i in 0..gs{
                dp[(n+i)*gs+goal[i]]+=1.;
            }

            for &v in topo.iter().rev(){
                if n<=v{
                    if v==start{
                        break;
                    }
                    continue;
                }

                let prob=input.get_prob(a[v]);
                let [v0,v1]=next[v];
                for i in 0..gs{
                    let mut new=0.;
                    if v0!=!0{
                        new+=prob[i]*dp[v0*gs+i];
                    }
                    if v1!=!0{
                        new+=(1.-prob[i])*dp[v1*gs+i];
                    }
                    dp[v*gs+i]=new;
                }
                if v==start{
                    break;
                }
            }

            for i in start*gs..dp.len(){
                assert!((dp[i]-bdp[i]).abs()<=1e-4);
            }
        }
        
        #[track_caller]
        fn check(){
            let sum=naive!();

            if !((sum-*score).abs()<=1e-4){
                eprintln!("{}",sum);
                eprintln!("{}",*score);
                panic!();
            }
        }

        fn init_fdp(end:usize){
            fdp.fill(0.);
            fdp[root*gs..(root+1)*gs].fill(1.);

            for &v in topo.iter(){
                if v==end{
                    break;
                }

                let prob=input.get_prob(a[v]);
                let [v0,v1]=next[v];
                for i in 0..gs{
                    if v0!=!0{
                        fdp[v0*gs+i]+=fdp[v*gs+i]*prob[i];
                    }
                    if v1!=!0{
                        fdp[v1*gs+i]+=fdp[v*gs+i]*(1.-prob[i]);
                    }
                }
            }
        }

        fn get_diff_cur(v:usize)->f64{
            let mut diff=0.;
            for i in 0..gs{
                diff-=fdp[v*gs+i]*bdp[v*gs+i];
            }
            diff
        }

        fn get_diff(v:usize)->f64{
            let mut sum=0.;
            let [v0,v1]=next[v];
            let prob=input.get_prob(a[v]);

            for i in 0..gs{
                if v0!=!0{
                    sum+=fdp[v*gs+i]*prob[i]*bdp[v0*gs+i];
                }
                if v1!=!0{
                    sum+=fdp[v*gs+i]*(1.-prob[i])*bdp[v1*gs+i];
                }
            }

            sum
        }

        fn get_diff_multi(vs:usize)->f64{
            que.clear();
            que.push(Rev(ord[vs]));

            let range=vs*gs..(vs+1)*gs;
            dp[range.clone()].copy_from_slice(&fdp[range]);

            let mut sum=0.;
            let mut last=!0;
            updated.clear();

            while let Some(Rev(idx))=que.pop(){
                if last==idx{
                    continue;
                }
                last=idx;

                if ord[vs]<idx{
                    let v=topo[idx];
                    for i in 0..gs{
                        sum+=dp[v*gs+i]*bdp[v*gs+i];
                    }
                    continue;
                }
                
                let v=topo[idx];
                let prob=input.get_prob(a[v]);
                let [v0,v1]=next[v];

                if v0!=!0{
                    que.push(Rev(ord[v0]));
                    if !updated.contains(v0){
                        updated.add(v0);
                        dp[v0*gs..(v0+1)*gs].fill(0.);
                    }
                }
                if v1!=!0{
                    que.push(Rev(ord[v1]));
                    if !updated.contains(v1){
                        updated.add(v1);
                        dp[v1*gs..(v1+1)*gs].fill(0.);
                    }
                }

                for i in 0..gs{
                    let cur=dp[v*gs+i];
                    if v0!=!0{
                        dp[v0*gs+i]+=cur*prob[i];
                    }
                    if v1!=!0{
                        dp[v1*gs+i]+=cur*(1.-prob[i]);
                    }
                }
            }
            
            sum
        }

        fn do_fdp(v:usize){
            let prob=input.get_prob(a[v]);
            let [v0,v1]=next[v];
            for i in 0..gs{
                if v0!=!0{
                    fdp[v0*gs+i]+=fdp[v*gs+i]*prob[i];
                }
                if v1!=!0{
                    fdp[v1*gs+i]+=fdp[v*gs+i]*(1.-prob[i]);
                }
            }
        }

        fn undo_fdp(v:usize){
            let prob=input.get_prob(a[v]);
            let [v0,v1]=next[v];
            for i in 0..gs{
                if v0!=!0{
                    fdp[v0*gs+i]-=fdp[v*gs+i]*prob[i];
                }
                if v1!=!0{
                    fdp[v1*gs+i]-=fdp[v*gs+i]*(1.-prob[i]);
                }
            }
        }

        fn do_bdp(v:usize){
            let prob=input.get_prob(a[v]);
            let [v0,v1]=next[v];
            for i in 0..gs{
                let mut new=0.;
                if v0!=!0{
                    new+=prob[i]*bdp[v0*gs+i];
                }
                if v1!=!0{
                    new+=(1.-prob[i])*bdp[v1*gs+i];
                }
                bdp[v*gs+i]=new;
            }
        }

        fn update_lines(v:usize,v0:usize,v1:usize,nv0:usize,nv1:usize){
            if v0!=!0 && v0!=nv0 && v0!=nv1{
                lines.rem_edge(v,v0);
            }
            if v1!=!0 && v1!=nv0 && v1!=nv1 && v1!=v0{
                lines.rem_edge(v,v1);
            }
            if nv0!=!0 && nv0!=v0 && nv0!=v1{
                lines.add_edge(v,nv0);
            }
            if nv1!=!0 && nv1!=v0 && nv1!=v1 && nv1!=nv0{
                lines.add_edge(v,nv1);
            }
        }

        fn trans(v:usize,add:f64)->bool{
            let diff=get_diff_cur!(v);
            let [v0,v1]=next[v];
            
            if rnd::nextf()<=0.9{ // todo
                if let Some(nv)=lines.random(v){
                    // 辺の行き先の変更
                    assert!(nv!=v0 && nv!=v1);
                    let (mut nv0,mut nv1)=(v0,v1);

                    if ord[nv]<ord[v]{
                        if rnd::next()&1==0{
                            nv0=nv;
                        } else{
                            nv1=nv;
                        }

                        let i0=ord[nv];
                        let i1=ord[v];

                        *at+=1;
                        reach[i0]=*at;
                        t0.clear();
                        for i in i0..=i1{
                            if reach[i]!=*at{
                                continue;
                            }
                            t0.push(topo[i]);
                            let [v0,v1]=next[topo[i]];
                            if v0!=!0 && ord[v0]<=i1{
                                reach[ord[v0]]=*at;
                            }
                            if v1!=!0 && ord[v1]<=i1{
                                reach[ord[v1]]=*at;
                            }
                        }
                        
                        // let mut rotate:u8=!0;
                        // if (next[nv][0]==!0 || i1<ord[next[nv][0]]) && (next[nv][1]==!0 || i1<ord[next[nv][1]]){
                        //     // nvからi0+1..=i1の範囲に行くやつがない -> rotate left
                        //     rotate=1;
                        // } else if topo[i0..i1].iter().all(|&u|next[u][0]!=v && next[u][1]!=v){
                        //     // i0..i1の範囲にvに行くやつがない -> rotate right
                        //     rotate=0;
                        // }

                        // if rotate!=!0{
                        if reach[i1]!=*at{
                            next[v]=[nv0,nv1];
                            let dd=get_diff_multi!(v); // todo: rotate=0なら高速に求められる

                            if diff+dd>=add{
                                update_lines!(v,v0,v1,nv0,nv1);
                                *score+=diff+dd;

                                *at+=1;
                                reach[i1]=*at;
                                t1.clear();
                                t2.clear();
                                for i in (i0..=i1).rev(){
                                    let [v0,v1]=next[topo[i]];
                                    if v0!=!0 && ord[v0]<=i1 && reach[ord[v0]]==*at{
                                        reach[i]=*at;
                                    }
                                    if v1!=!0 && ord[v1]<=i1 && reach[ord[v1]]==*at{
                                        reach[i]=*at;
                                    }

                                    if reach[i]==*at{
                                        t1.push(topo[i]);
                                    } else if reach[i]!=*at-1{
                                        t2.push(topo[i]);
                                    }
                                }

                                let mut it=chain!(t1.iter().rev(),t2.iter().rev(),t0.iter()).copied();
                                topo[i0..=i1].fill_with(||it.next().unwrap());
                                for i in i0..=i1{
                                    ord[topo[i]]=i;
                                }

                                for &v in updated.to_slice(){
                                    for i in 0..gs{
                                        fdp[v*gs+i]+=dp[v*gs+i];
                                    }
                                }
                                undo_fdp!(topo[i1]);

                                return true;
                                
                                // if rotate==0{
                                //     topo[i0..=i1].rotate_right(1);
                                //     for i in i0..=i1{
                                //         ord[topo[i]]=i;
                                //     }

                                //     for &v in updated.to_slice(){
                                //         for i in 0..gs{
                                //             fdp[v*gs+i]+=dp[v*gs+i];
                                //         }
                                //     }
                                //     undo_fdp!(topo[i1]);

                                //     return true;
                                // } else{
                                //     topo[i0..=i1].rotate_left(1);
                                //     for i in i0..=i1{
                                //         ord[topo[i]]=i;
                                //     }

                                //     undo_fdp!(nv);
                                //     do_fdp!(v);

                                //     return true;
                                // }
                            } else{
                                next[v]=[v0,v1];
                            }
                        }
                    } else{
                        let mut nnv=!0;
                        if rnd::nextf()<=0.3{ // todo
                            for _ in 0..10{
                                let r=lines.random(v).unwrap();
                                if ord[v]<ord[r]{
                                    nnv=r;
                                    break;
                                }
                            }
                        }
                        if nnv!=!0{
                            (nv0,nv1)=(nv,nnv);
                        } else if rnd::next()&1==0{
                            nv0=nv;
                        } else{
                            nv1=nv;
                        }

                        next[v]=[nv0,nv1];
                        let dd=get_diff!(v);

                        if diff+dd>=add{
                            update_lines!(v,v0,v1,nv0,nv1);
                            *score+=diff+dd;
                            return true;
                        } else{
                            next[v]=[v0,v1];
                        }
                    }
                }
            }

            // 分別器の変更
            let maxi=(0..gs).max_by_key(|&i|fdp[v*gs+i].ord()).unwrap();
            let old=a[v];

            // todo
            let mut new0;
            loop{
                new0=selector_cand[maxi].choice();
                if new0!=old{
                    break;
                }
            }
            let mut new1;
            loop{
                new1=selector_cand[maxi].choice();
                if new1!=old && new1!=new0{
                    break;
                }
            }
            // let mut new2;
            // loop{
            //     new2=selector_cand[maxi].choice();
            //     if new2!=old && new2!=new0 && new2!=new1{
            //         break;
            //     }
            // }
            // let new=rnd::range_skip(0,input.probs,old);
            
            a[v]=new0;
            let dd0=get_diff!(v);

            a[v]=new1;
            let dd1=get_diff!(v);

            let (dd,new)=if dd0>dd1{
                (dd0,new0)
            } else{
                (dd1,new1)
            };
            // assert!(dd==dd0.max(dd1));

            // a[v]=new2;
            // let dd2=get_diff!(v);

            // let (dd,new)=if dd0>dd1 && dd0>dd2{
            //     (dd0,new0)
            // } else if dd1>dd2{
            //     (dd1,new1)
            // } else{
            //     (dd2,new2)
            // };
            // assert!(dd==dd0.max(dd1).max(dd2));

            if diff+dd>=add{
                *score+=diff+dd;
                a[v]=new;
                true
            } else{
                a[v]=old;
                false
            }
        }

        fn next_right(add:f64)->bool{
            let f=trans!(topo[*idx],add);
            do_fdp!(topo[*idx]);
            *idx+=1;

            f
        }

        fn next_left(add:f64)->bool{
            do_bdp!(topo[*idx]);
            undo_fdp!(topo[*idx]);

            let f=trans!(topo[*idx],add);
            if f{
                do_bdp!(topo[*idx]);
            }

            *idx-=1;
            f
        }
    }

    check!();

    let log=logs();
    let start=get_time();
    let tl=tl-start;
    assert!(0.<tl);

    let mut valid=0;
    let mut iter=0;
    let mut heat=0.;

    let mut best=score;

    'a: loop{
        const T0:f64=6e-2; // todo
        const T1:f64=4e-5;
        
        macro_rules! gen_add{
            ()=>{{
                if iter&255==0{
                    let time=(get_time()-start)/tl;
                    if time>=1.{
                        break 'a;
                    }
                    heat=T0*(T1/T0).powf(time);
                    // heat=T0+(T1-T0)*time;
                }
                iter+=1;
                heat*log[iter&65535]
            }}
        }

        for _ in 0..2{ // todo
            for gi0 in 0..input.goals{
                let add=gen_add!();
                let gi1=rnd::range_skip(0,input.goals,gi0);
                let gv0=gi0+n;
                let gv1=gi1+n;
    
                let diff=fdp[gv0*gs+goal[gi1]]+fdp[gv1*gs+goal[gi0]]-fdp[gv0*gs+goal[gi0]]-fdp[gv1*gs+goal[gi1]];
                if add<=diff{
                    score+=diff;
                    valid+=1;
                    best=best.max(score);

                    bdp[gv0*gs+goal[gi0]]=0.;
                    bdp[gv0*gs+goal[gi1]]=1.;
                    bdp[gv1*gs+goal[gi1]]=0.;
                    bdp[gv1*gs+goal[gi0]]=1.;
                    goal.swap(gi0,gi1);
                }
            }
        }
        
        while idx!=!0{
            valid+=next_left!(gen_add!()) as usize;
            best=best.max(score);
        }
        idx+=1;

        for _ in 0..1{
            if let Some(nv)=lines.random(sv){
                assert!(nv!=root);
                
                // rootを変える
                let add=gen_add!();
                let new=bdp[nv*gs..(nv+1)*gs].iter().sum::<f64>();
    
                if new-score>=add{
                    let ok=if ord[nv]<ord[root]{
                        true
                    } else if topo[ord[root]..ord[nv]].iter().all(|&u|next[u][0]!=nv && next[u][1]!=nv){
                        topo[ord[root]..=ord[nv]].rotate_right(1);
                        for i in ord[root]..=ord[nv]{
                            ord[topo[i]]=i;
                        }
                        true
                    } else{
                        false
                    };
    
                    if ok{
                        score=new;
                        valid+=1;
                        best=best.max(score);
    
                        update_lines!(sv,root,!0,nv,!0);
                        fdp[root*gs..(root+1)*gs].fill(0.);
                        fdp[nv*gs..(nv+1)*gs].fill(1.);
                        root=nv;
                    }
                }
            }
        }
        
        while idx!=n{
            valid+=next_right!(gen_add!()) as usize;
            best=best.max(score);
        }
        idx-=1;
    }

    check!();

    // eprintln!("# {:.3}",cnt.1 as f64/cnt.0 as f64);

    // eprintln!("sa_best = {:.2}",best);
    // eprintln!("sa      = {:.2}",score);
    // eprintln!("main_iter = {}",iter);
    // eprintln!("main_raito = {:.3}",valid as f64/iter as f64);
    // eprintln!("score = {:.0}",1e4-best*1e4/input.goals as f64);
    // eprintln!("end = {}",get_time());

    let score=1e4-score*1e4/input.goals as f64;

    (root,next,goal,a,score)
}



fn make_tour(W:usize,sa_time:f64)->Vec<usize>{
    let input=get_input();
    let si={
        let mut is0=(0..input.vias).collect_vec();
        is0.sort_unstable_by_key(|&i|input.via[i].x);
        
        let mut is1=(0..input.vias).collect_vec();
        is1.sort_unstable_by_key(|&i|input.via[i].dist(input.start).ord());
        
        let dist=|p:P|{
            input.goal.iter().map(|&q|p.dist(q).ord()).min().unwrap().0
        };

        // todo
        chain!(&is0[..20],&is1[..0]).copied().max_by_key(|&i|dist(input.via[i]).ord()).unwrap()
    };

    let mut ps=input.via.to_vec();
    ps.swap(0,si);

    let pair=find_nearest_goal_pair();
    ps.insert(1,input.goal[pair.0]);
    ps.insert(1,input.goal[pair.1]);

    let mut tour=Tour::new(ps.clone(),ps.len()/W,W);
    make_tour_greedy(&mut tour);
    make_tour_sa(&mut tour,sa_time);

    assert!(tour.tour[0]==0 || tour.tour[W-1]==0);
    if tour.tour[W-1]==0{
        for i in 0..tour.H{
            tour.reverse(i*W..i*W+W);
        }
    }
    
    for i in 0..tour.n{
        assert!(tour.pos[tour.tour[i]]==i);
    }

    // println!("{}",tour.n);
    // for i in 0..tour.n{
    //     println!("{}",tour.ps[i]);
    // }
    // println!("{}",tour.H*tour.W-1);
    // for i in 1..tour.H*tour.W{
    //     println!("{} {}",tour.tour[i-1],tour.tour[i]);
    // }
    
    let HW=tour.H*tour.W;
    let mut a=tour.tour[..HW].to_vec();
    let rem=|a:&mut Vec<usize>,v:usize|{
        let i=(0..a.len()).find(|&i|a[i]==v).unwrap();
        a.remove(i);
    };
    rem(&mut a,1);
    rem(&mut a,2);

    for &v in &tour.tour[HW..]{
        let arg=(1..a.len()-1).min_by_key(|&i|{
            let diff=tour.dist(a[i-1],v)+tour.dist(v,a[i])-tour.dist(a[i-1],a[i]);
            diff.ord()
        }).unwrap();
        a.insert(arg,v);
    }

    ps.push(input.start);
    a.insert(0,ps.len()-1);
    make_no_cross_tour(&ps,&mut a);
    // assert!(tour.tour[0]==0);
    rem(&mut a,ps.len()-1);

    let mut id=HashMap::default();
    for i in 0..input.vias{
        id.insert(input.via[i],i);
    }

    a.iter().map(|&i|id[&ps[i]]).collect()
}



fn make_tour_greedy(tour:&mut Tour){
    let mut a=vec![!0;tour.n];
    let mut rem=(3..tour.n).collect_vec();
    let mut to=vec![];

    let HW=tour.H*tour.W;
    a[0]=0;
    a[HW-1]=1;
    a[HW-2]=2;

    for i in 1..{
        let i0=i;
        to.clear();
        to.extend(tour.g[i0].iter().filter(|&j|i0>*j).map(|&j|a[j]));
        let arg=(0..rem.len()).min_by_key(|&j|{
            let mut sum=0.;
            for &v in &to{
                sum+=tour.dist(v,rem[j]);
            }
            sum.ord()
        }).unwrap();
        a[i0]=rem.swap_remove(arg);

        let i1=HW-2-i;
        if i1==i0{
            break;
        }

        to.clear();
        to.extend(tour.g[i1].iter().filter(|&j|i1<*j).map(|&j|a[j]));
        let arg=(0..rem.len()).min_by_key(|&j|{
            let mut sum=0.;
            for &v in &to{
                sum+=tour.dist(v,rem[j]);
            }
            sum.ord()
        }).unwrap();
        a[i1]=rem.swap_remove(arg);
        
        if i1==i0+1{
            break;
        }
    }

    for i in HW..tour.n{
        a[i]=rem.pop().unwrap();
    }
    assert!(rem.is_empty());

    for i in 0..tour.n{
        tour.pos[a[i]]=i;
    }
    tour.tour=a;
}



fn make_tour_sa(tour:&mut Tour,tl:f64){
    let pena=1.;

    let dist=|tour:&Tour,i:usize,j:usize|{
        let mut d=tour.dist(i,j);
        if i<=2 || j<=2{
            d*=pena;
        }
        d
    };
    
    let get_score=|tour:&Tour|->f64{
        let mut score=0.;
        for v in 0..tour.n{
            for &nv in &tour.g[v]{
                if v<nv{
                    score+=dist(tour,tour.tour[v],tour.tour[nv]);
                }
            }
        }
        score
    };
    let mut score=get_score(&tour);

    let log=logs();
    let mut valid=0;
    let mut lis=vec![];
    lis.extend((0..10).map(|_|0)); // todo
    lis.push(1);
    lis.push(2);

    let H=tour.H;
    let W=tour.W;
    let HW=tour.H*tour.W;

    let start=get_time();
    let tl=tl-start;
    assert!(0.<tl);
    let mut it=0;
    let mut heat=0.;

    loop{
        if it&255==0{
            const T0:f64=1e3; // todo
            const T1:f64=0.;
            let time=(get_time()-start)/tl;
            if time>=1.{
                break;
            }
            // let heat=T0*(T1/T0).powf(time);
            heat=T0+(T1-T0)*time;
        }
        it+=1;

        let add=-heat*log[it&65535]; // 最小化
        // let add=0.;
        let a=&tour.tour;

        let idx0=rnd::get(HW);
        let v0=a[idx0];

        let v1=tour.nei[v0].choice();
        let idx1=tour.pos[v1];

        match lis.choice(){
            0=>{
                if v0==0 || v1==0{
                    let ok=idx0==0 && idx1==W-1 || idx0==W-1 && idx1==0;
                    if !ok{
                        continue;
                    }
                } else if v0<=2 || v1<=2{
                    let ok=idx0/W==H-1 && idx1/W==H-1;
                    if !ok{
                        continue;
                    }
                }
                
                let mut diff=0.;
                for (idx,v) in [(idx0,v0),(idx1,v1)]{
                    if idx>=HW{
                        continue;
                    }
                    for &nidx in &tour.g[idx]{
                        if nidx!=idx0^idx1^idx{
                            diff-=dist(tour,v,a[nidx]);
                            diff+=dist(tour,v0^v1^v,a[nidx]);
                        }
                    }
                }

                if diff<=add{
                    score+=diff;
                    valid+=1;
                    tour.tour.swap(idx0,idx1);
                    tour.pos.swap(v0,v1);
                }
            },
            1=>{
                let i0=idx0/W;
                if i0+1>=H{
                    continue;
                }

                // idxまでの列をすべて反転させる
                let mut diff=0.;
                for j in 0..W{
                    diff-=dist(tour,a[tour.id(i0,j)],a[tour.id(i0+1,j)]);
                    diff+=dist(tour,a[tour.id(i0,j)],a[tour.id(i0+1,W-1-j)]);
                }

                if diff<=add{
                    score+=diff;
                    valid+=1;
                    for i in 0..=i0{
                        tour.reverse(tour.id(i,0)..tour.id(i,W));
                    }
                }
            },
            2=>{
                // 2-opt
                let mut i0=idx0/W;
                let mut i1=idx1/W;
                if i0>i1{
                    (i0,i1)=(i1,i0);
                }
                if i1<=i0+1 || i1>=H || i0==0{
                    continue;
                }

                let mut diff=0.;
                for j in 0..W{
                    diff-=dist(tour,a[tour.id(i0-1,j)],a[tour.id(i0,j)]);
                    diff+=dist(tour,a[tour.id(i0-1,j)],a[tour.id(i1-1,W-1-j)]);
                    diff-=dist(tour,a[tour.id(i1-1,j)],a[tour.id(i1,j)]);
                    diff+=dist(tour,a[tour.id(i0,j)],a[tour.id(i1,W-1-j)]);
                }

                if diff<=add{
                    score+=diff;
                    valid+=1;
                    tour.reverse(tour.id(i0,0)..tour.id(i1,0));
                }
            },
            _=>panic!(),
        }
    }

    assert!((score-get_score(&tour)).abs()<=1.);
    // eprintln!("tour_iter = {}",it);
    // eprintln!("dist_sum = {:.0}",score);
    // eprintln!("ratio_tour = {:.3}",valid as f64/it as f64);
}



fn make_no_cross_tour(ps:&[P],tour:&mut [usize]){
    loop{
        let mut update=false;
        for len in 2..tour.len(){
            for i1 in len+1..tour.len(){
                let i0=i1-len;
                if !isok(ps[tour[i0-1]],ps[tour[i0]],ps[tour[i1-1]],ps[tour[i1]]){
                    update=true;
                    tour[i0..i1].reverse();
                }
            }
        }

        if !update{
            break;
        }
    }
}



#[derive(Clone)]
struct Tour{
    n:usize,
    ps:Vec<P>,
    nei:Vec<Vec<usize>>,
    H:usize,
    W:usize,
    g:Vec<Vec<usize>>,
    tour:Vec<usize>,
    pos:Vec<usize>,
}
impl Tour{
    // 0がスタート,1,2がゴール
    fn new(ps:Vec<P>,H:usize,W:usize)->Tour{
        let n=ps.len();
        assert!(H*W<=n);

        let NTH=20; // todo
        let mut nei=vec![vec![];n];
        let mut is=(0..n).collect_vec();

        for i in 0..n{
            is.select_nth_unstable_by_key(NTH,|&j|ps[i].dist(ps[j]).ord());
            nei[i]=is[..NTH].iter().copied().filter(|&j|i!=j).collect_vec();
        }
        
        let id=|i:usize,j:usize|i*W+j;

        let mut g=vec![vec![];n];
        let mut add_edge=|u:usize,v:usize|{
            g[u].push(v);
            g[v].push(u);
        };

        for i in 0..H{
            for j in 0..W{
                if i!=0{
                    add_edge(id(i-1,j),id(i,j));
                }
                if j!=0{
                    add_edge(id(i,j-1),id(i,j));
                }
            }
        }
        
        Tour{
            n,ps,nei,H,W,g,
            tour:(0..n).collect(),
            pos:(0..n).collect(),
        }
    }

    fn dist(&self,i:usize,j:usize)->f64{
        self.ps[i].dist(self.ps[j])
    }

    fn id(&self,i:usize,j:usize)->usize{
        i*self.W+j
    }

    fn reverse(&mut self,Range{start,end}:Range<usize>){
        assert!(start<=end);
        for i in 0..(end-start)/2{
            let idx0=start+i;
            let idx1=end-1-i;
            self.tour.swap(idx0,idx1);
            self.pos.swap(self.tour[idx0],self.tour[idx1]);
        }
    }
}



fn find_nearest_goal_pair()->(usize,usize){
    let input=get_input();

    let mut arg=(!0,!0);
    let mut min=f64::INFINITY;

    for i in 0..input.goals{
        for j in i+1..input.goals{
            let d=input.goal[i].dist(input.goal[j]);
            if min>d{
                min=d;
                arg=(i,j);
            }
        }
    }

    arg
}



type HashMap<U,V>=rustc_hash::FxHashMap::<U,V>;
type HashSet<T>=rustc_hash::FxHashSet::<T>;



struct Lines{
    es:Vec<(usize,usize)>,
    csr:Vec<usize>,
    csri:Vec<usize>,
    si:Vec<usize>,
    used:Vec<bool>,
    cnt:Vec<usize>,
    deg:Vec<usize>,
    set:Vec<IndexSet>,
}
impl Lines{
    fn new(ps:&[P],mut es:Vec<(usize,usize)>)->Lines{
        // 自分は自分とcrossしていることにする
        let n=ps.len();

        es.sort_unstable();
        es.dedup();

        let mut si=vec![0;n+1];
        for &(u,_) in &es{
            si[u+1]+=1;
        }
        for i in 1..=n{
            si[i]+=si[i-1];
        }

        let set=(0..n).map(|i|{
            let mut set=IndexSet::new(si[i+1]-si[i]);
            set.fill();
            set
        }).collect_vec();
        let mut cross=vec![HashSet::default();es.len()];
        
        let W=1e3 as usize;
        let mut block:Vec<Vec<Vec<usize>>>=vec![vec![vec![];MAX/W+1];MAX/W+1];
        let mut cells=vec![];

        let isok=|a:usize,b:usize,c:usize,d:usize|{
            (a.min(b),a.max(b))!=(c.min(d),c.max(d))
            && isok(ps[a],ps[b],ps[c],ps[d])
        };

        let mut add=|i:usize,j:usize|{
            cross[i].insert(j);
            cross[j].insert(i);
        };
        
        for i in 0..es.len(){
            enumerate_cells(ps[es[i].0],ps[es[i].1],W,&mut cells);
            add(i,i);
            for &(x,y) in &cells{
                for &j in &block[x][y]{
                    if !isok(es[i].0,es[i].1,es[j].0,es[j].1){
                        add(i,j);
                    }
                }
                block[x][y].push(i);
            }
        }

        let csr=cross.iter().map(|t|t.iter()).flatten().copied().collect_vec();
        let mut csri=vec![0;cross.len()+1];
        for i in 0..cross.len(){
            csri[i+1]=csri[i]+cross[i].len();
        }
        
        let len=es.len();
        
        Lines{
            es,csr,csri,si,set,
            used:vec![false;len],
            cnt:vec![0;len],
            deg:vec![0;n],
        }
    }

    #[track_caller]
    fn add_edge(&mut self,u:usize,v:usize){
        let id=self.get_edge_id(u,v);
        
        assert!(!self.used[id] && self.cnt[id]==0);
        self.used[id]=true;
        self.deg[v]+=1;

        for &ni in &self.csr[self.csri[id]..self.csri[id+1]]{
            if self.cnt[ni]==0{
                let v=self.es[ni].0;
                self.set[v].rem(ni-self.si[v]);
            }
            self.cnt[ni]+=1;
        }
    }
    
    fn rem_edge(&mut self,u:usize,v:usize){
        let id=self.get_edge_id(u,v);

        assert!(self.used[id]);
        self.used[id]=false;
        assert!(self.deg[v]!=0);
        self.deg[v]-=1;

        for &ni in &self.csr[self.csri[id]..self.csri[id+1]]{
            assert!(self.cnt[ni]!=0);
            self.cnt[ni]-=1;
            if self.cnt[ni]==0{
                let v=self.es[ni].0;
                self.set[v].add(ni-self.si[v]);
            }
        }
    }

    fn get_edge_id(&self,u:usize,v:usize)->usize{
        self.es.binary_search(&(u,v)).unwrap()
    }

    fn is_ok_edge(&self,u:usize,v:usize)->bool{
        let Ok(id)=self.es.binary_search(&(u,v)) else{return false};
        self.cnt[id]==0
    }

    fn random(&self,v:usize)->Option<usize>{
        if self.set[v].len()==0{
            return None;
        }
        // todo
        let res=(0..4).map(|_|self.es[self.set[v].choice()+self.si[v]].1)
            .min_by_key(|&v|self.deg[v]).unwrap();
        // let res=(0..4).map(|_|self.es[self.set[v].choice()+self.si[v]].1)
        //     .min_by_key(|&v|(self.deg[v]!=0) as u8).unwrap();
        // let res=self.es[self.set[v].choice()+self.si[v]].1;
        Some(res)
    }
}



fn enumerate_cells(P{x:x0,y:y0}:P,P{x:x1,y:y1}:P,w:usize,cells:&mut Vec<(usize,usize)>){
    let w=w as i64;
    cells.clear();

    let mut i = x0.div_euclid(w);
    let mut j = y0.div_euclid(w);
    let i1 = x1.div_euclid(w);
    let j1 = y1.div_euclid(w);

    let dx = x1 - x0;
    let dy = y1 - y0;
    let adx = dx.abs();
    let ady = dy.abs();
    let sx = if dx > 0 { 1 } else if dx < 0 { -1 } else { 0 };
    let sy = if dy > 0 { 1 } else if dy < 0 { -1 } else { 0 };

    cells.push((i as usize, j as usize));

    if i == i1 && j == j1 {
        return;
    }

    if adx == 0 {
        let mut jj = j;
        while jj != j1 {
            jj += sy;
            cells.push((i as usize, jj as usize));
        }
        return;
    }
    if ady == 0 {
        let mut ii = i;
        while ii != i1 {
            ii += sx;
            cells.push((ii as usize, j as usize));
        }
        return;
    }

    let rx0 = x0.rem_euclid(w);
    let ry0 = y0.rem_euclid(w);

    let dist_x = if sx > 0 { if rx0 == 0 { w } else { w - rx0 } } else { rx0 };
    let dist_y = if sy > 0 { if ry0 == 0 { w } else { w - ry0 } } else { ry0 };

    let mut acc_x = dist_x * ady;
    let mut acc_y = dist_y * adx;
    let step_x = w * ady;
    let step_y = w * adx;

    while !(i == i1 && j == j1) {
        if acc_x < acc_y {
            i += sx;
            acc_x += step_x;
        } else if acc_y < acc_x {
            j += sy;
            acc_y += step_y;
        } else {
            i += sx;
            j += sy;
            acc_x += step_x;
            acc_y += step_y;
        }
        cells.push((i as usize, j as usize));
    }

    // let i0 = x0.div_euclid(w);
    // let j0 = y0.div_euclid(w);
    // let i1 = x1.div_euclid(w);
    // let j1 = y1.div_euclid(w);
    // for i in i0.min(i1)..=i0.max(i1){
    //     for j in j0.min(j1)..=j0.max(j1){
    //         self.cells.push((i as usize,j as usize));
    //     }
    // }
}



fn output_ans(tour:&[usize],root:usize,g:&[[usize;2]],goal:&[usize],a:&[usize]){
    let input=get_input();

    let mut rev=vec![!0;tour.len()];
    for i in 0..tour.len(){
        rev[tour[i]]=i;
    }

    let mut res=vec![None;g.len()];
    type A=(usize,usize,usize);
    const NULL:A=(!0,!0,!0);

    fn rec(g:&[[usize;2]],a:&[usize],v:usize,res:&mut [Option<A>])->bool{
        let input=get_input();
        if v==!0{
            return false;
        }
        if input.vias<=v{
            return true;
        }
        if res[v].is_some(){
            return res[v].unwrap()!=NULL;
        }

        let [mut v0,mut v1]=g[v];

        let id;
        if a[v]/2==input.types{
            id=0;
            if a[v]%2==0{
                v1=v0;
            } else{
                v0=v1;
            }
        } else{
            id=a[v]/2;
            if a[v]%2==1{
                (v0,v1)=(v1,v0);
            }
        }

        if !rec(g,a,v0,res) && !rec(g,a,v1,res){
            res[v]=Some(NULL);
            return false;
        }

        if !rec(g,a,v0,res){
            v0=v1;
        }
        if !rec(g,a,v1,res){
            v1=v0;
        }

        assert!(v0!=!0 && v1!=!0);
        res[v]=Some((id,v0,v1));
        true
    }

    println!("{}",goal.iter().join(" "));
    println!("{}",tour[root]+input.goals);

    let get_point=|v:usize|{
        if input.vias<=v{
            input.goal[v-input.vias]
        } else{
            input.via[tour[v]]
        }
    };

    let conv=|v:usize|{
        if input.vias<=v{
            v-input.vias
        } else{
            tour[v]+input.goals
        }
    };

    let mut es=vec![(input.start,input.via[tour[root]])];
    
    for i in 0..tour.len(){
        rec(g,a,rev[i],&mut res);
        let (id,v0,v1)=res[rev[i]].unwrap();
        
        if (id,v0,v1)==NULL{
            println!("-1");
        } else{
            es.push((input.via[i],get_point(v0)));
            es.push((input.via[i],get_point(v1)));

            println!("{} {} {}",id,conv(v0),conv(v1));
        }
    }

    for i in 0..es.len(){
        for j in 0..i{
            assert!(isok(es[i].0,es[i].1,es[j].0,es[j].1));
        }
    }
}



use std::ops::Range;
use std::iter::*;
use std::collections::*;
use std::cmp::Reverse as Rev;
use itertools::*;

const MAX:usize=1e4 as usize;


#[allow(unused)]
struct Input{
    goals:usize,
    vias:usize,
    types:usize,
    v:usize,
    start:P,
    goal:Vec<P>,
    via:Vec<P>,
    probs:usize,
    prob:Vec<f64>,
}
impl Input{
    fn get_prob(&self,i:usize)->&[f64]{
        let idx=i*self.goals;
        &self.prob[idx..idx+self.goals]
    }
}



static_data!{
    get_input:Input|{
        proconio::input!{
            goals:usize,
            vias:usize,
            types:usize,
            dp:[(i64,i64);goals],
            sp:[(i64,i64);vias],
            pt:[[f64;goals];types],
        }
        let start=P::new(0,MAX as i64/2);
        let goal=dp.iter().map(|&(x,y)|P::new(x,y)).collect_vec();
        let via=sp.iter().map(|&(x,y)|P::new(x,y)).collect_vec();
        let v=1+goals+vias;
        
        let mut prob=vec![];
        for i in 0..types{
            prob.extend(&pt[i]);
            prob.extend(pt[i].iter().map(|&p|1.-p));
        }
        prob.extend(vec![1.;goals]);
        prob.extend(vec![0.;goals]);

        let probs=prob.len()/goals;
        Input{goals,vias,types,v,start,goal,via,probs,prob}
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
            f64::from_bits(0x3ff0000000000000|(next() as u64)<<20)-1.
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



#[derive(Clone,Copy,PartialEq,Eq,PartialOrd,Ord,Hash)]
struct P{
    x:i64,
    y:i64,
}
impl P{
    fn new(x:i64,y:i64)->P{
        P{x,y}
    }

    fn det(self,p:P)->i64{
        self.x*p.y-self.y*p.x
    }

    fn dot(self,p:P)->i64{
        self.x*p.x+self.y*p.y
    }

    fn abs2(self)->i64{
        self.dot(self)
    }

    fn dist(self,a:P)->f64{
        ((self-a).abs2() as f64).sqrt()
    }
}


impl std::fmt::Debug for P{
    fn fmt(&self,f:&mut std::fmt::Formatter)->std::fmt::Result{
        write!(f,"({}, {})",self.x,self.y)
    }
}
impl std::fmt::Display for P{
    fn fmt(&self,f:&mut std::fmt::Formatter)->std::fmt::Result{
        write!(f,"{} {}",self.x,self.y)
    }
}


impl std::ops::Add for P{
    type Output=P;
    fn add(self,a:P)->P{
        P{
            x:self.x+a.x,
            y:self.y+a.y,
        }
    }
}
impl std::ops::Sub for P{
    type Output=P;
    fn sub(self,a:P)->P{
        P{
            x:self.x-a.x,
            y:self.y-a.y,
        }
    }
}
impl std::ops::Mul<i64> for P{
    type Output=P;
    fn mul(self,a:i64)->P{
        P{
            x:self.x*a,
            y:self.y*a,
        }
    }
}
impl std::ops::Div<i64> for P{
    type Output=P;
    fn div(self,a:i64)->P{
        P{
            x:self.x/a,
            y:self.y/a,
        }
    }
}
impl std::ops::Neg for P{
    type Output=P;
    fn neg(self)->P{
        P{
            x:-self.x,
            y:-self.y,
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
impl_p_ops!(i64,MulAssign,mul_assign,*);
impl_p_ops!(i64,DivAssign,div_assign,/);




// 線分p0p1とq0q1が端点以外に共有点を持つかどうか
fn isok(p0:P,p1:P,q0:P,q1:P)->bool{
    if p0==q0 || p0==q1 || p1==q0 || p1==q1{
        return true;
    }
    
    let sign=|x:i64|{
        if x>0{
            1
        } else if x<0{
            -1
        } else{
            0
        }
    };

    let orient=|a:P,b:P,c:P|sign((b-a).det(c-a));
    
    if  p0.x.max(p1.x) < q0.x.min(q1.x) || q0.x.max(q1.x) < p0.x.min(p1.x)
        || p0.y.max(p1.y) < q0.y.min(q1.y) || q0.y.max(q1.y) < p0.y.min(p1.y)
    {
        return true;
    }
    
    let o0=orient(p0,p1,q0);
    let o1=orient(p0,p1,q1);
    let o2=orient(q0,q1,p0);
    let o3=orient(q0,q1,p1);

    !(o0*o1<=0 && o2*o3<=0)
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



#[cfg(feature = "local")]
include!("/home/akatsuti/cp/lib/debug_mod.rs");
#[cfg(not(feature = "local"))]
#[macro_export]macro_rules! debug{($($t:tt)*)=>{}}



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
    ([],[$($ga:tt)*],[$($gn:tt)*],[$($ge:tt)*],[$(#[$($att:tt)*])? fn $name:ident($($arg:tt)*) $body:block $($rem:tt)*],[$($info:tt)*],[$($fn:tt)*])=>{
        cap!([],[$($ga)*],[$($gn)*],[$($ge)*],[$($rem)*],[(($(#[$($att)*])?),$name,($($arg)*),(),$body),$($info)*],[$name,$($fn)*]);
    };
    ([],[$($ga:tt)*],[$($gn:tt)*],[$($ge:tt)*],[$(#[$($att:tt)*])? fn $name:ident($($arg:tt)*)->$ret:ty $body:block $($rem:tt)*],[$($info:tt)*],[$($fn:tt)*])=>{
        cap!([],[$($ga)*],[$($gn)*],[$($ge)*],[$($rem)*],[(($(#[$($att)*])?),$name,($($arg)*),$ret,$body),$($info)*],[$name,$($fn)*]);
    };
    ([],[$($ga:tt)*],[$($gn:tt)*],[$($ge:tt)*],[],[$($info:tt)*],[$($fn:tt)*])=>{
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
