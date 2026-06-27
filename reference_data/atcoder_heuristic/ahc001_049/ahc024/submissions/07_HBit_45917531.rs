pub fn solve() {
    let now = std::time::Instant::now();
    crate::prepare!();
    sc!(n, m, c: [[usize; n]; n]);
    let (n, m, c) = {
        let mut d = vec![vec![0; n + 2]; n + 2];
        for i in 0..n {
            for j in 0..n {
                d[i + 1][j + 1] = c[i][j];
            }
        }
        (n + 2, m + 1, d)
    };
    let get_adj = |c: &[Vec<usize>]| {
        let (h, w) = (c.len(), c[0].len());
        let mut adj = vec![0u128; m];
        for i in 0..m {
            adj[i] |= 1 << i;
        }
        for i in 0..h {
            for j in 0..w {
                if i > 0 {
                    let (x, y) = (c[i - 1][j], c[i][j]);
                    adj[x] |= 1 << y;
                    adj[y] |= 1 << x;
                }
                if j > 0 {
                    let (x, y) = (c[i][j - 1], c[i][j]);
                    adj[x] |= 1 << y;
                    adj[y] |= 1 << x;
                }
            }
        }
        adj
    };
    let cadj = get_adj(&c);
    let check_adj = |c: &[Vec<usize>]| {
        let (h, w) = (c.len(), c[0].len());
        let g = GridGraph::new_adj4(h, w);
        let mut adj = vec![0u128; m];
        for i in 0..m {
            adj[i] |= 1 << i;
        }
        for i in 0..h {
            for j in 0..w {
                if i > 0 {
                    let (x, y) = (c[i - 1][j], c[i][j]);
                    if cadj[x] >> y & 1 == 0 {
                        return false;
                    }
                    adj[x] |= 1 << y;
                    adj[y] |= 1 << x;
                }
                if j > 0 {
                    let (x, y) = (c[i][j - 1], c[i][j]);
                    if cadj[x] >> y & 1 == 0 {
                        return false;
                    }
                    adj[x] |= 1 << y;
                    adj[y] |= 1 << x;
                }
            }
        }
        if adj != cadj {
            return false;
        }
        let mut vis = vec![0u64; h];
        let mut used = 0u128;
        let mut deq = VecDeque::new();
        for i in 0..h {
            for j in 0..w {
                if vis[i] >> j & 1 != 0 {
                    continue;
                }
                let a = c[i][j];
                if used >> a & 1 != 0 {
                    return false;
                }
                vis[i] |= 1 << j;
                used |= 1 << a;
                deq.push_back((i, j));
                while let Some((i, j)) = deq.pop_front() {
                    for ((x, y), _) in g.adj4((i, j)) {
                        if vis[x] >> y & 1 == 0 && c[x][y] == a {
                            vis[x] |= 1 << y;
                            deq.push_back((x, y));
                        }
                    }
                }
            }
        }
        true
    };
    let check_single = |c: &[Vec<usize>], i: usize, j: usize, _a: usize| {
        let (h, w) = (c.len(), c[0].len());
        let g = GridGraph::new_adj4(h, w);
        let mut adj = vec![0u128; m];
        for i in 0..m {
            adj[i] |= 1 << i;
        }
        for i in 0..h {
            for j in 0..w {
                if i > 0 {
                    let (x, y) = (c[i - 1][j], c[i][j]);
                    if cadj[x] >> y & 1 == 0 {
                        return false;
                    }
                    adj[x] |= 1 << y;
                    adj[y] |= 1 << x;
                }
                if j > 0 {
                    let (x, y) = (c[i][j - 1], c[i][j]);
                    if cadj[x] >> y & 1 == 0 {
                        return false;
                    }
                    adj[x] |= 1 << y;
                    adj[y] |= 1 << x;
                }
            }
        }
        if adj != cadj {
            return false;
        }
        let mut vis = vec![0u64; h];
        let mut used = 0u128;
        let mut deq = VecDeque::new();
        for (i, j) in g.adj4((i, j)).map(|t| t.0).chain(std::iter::once((i, j))) {
            if vis[i] >> j & 1 != 0 {
                continue;
            }
            let a = c[i][j];
            if used >> a & 1 != 0 {
                return false;
            }
            vis[i] |= 1 << j;
            used |= 1 << a;
            deq.push_back((i, j));
            while let Some((i, j)) = deq.pop_front() {
                for ((x, y), _) in g.adj4((i, j)) {
                    if vis[x] >> y & 1 == 0 && c[x][y] == a {
                        vis[x] |= 1 << y;
                        deq.push_back((x, y));
                    }
                }
            }
        }
        true
    };
    let mut rng = Xorshift::default();
    let mut s = c.clone();
    let mut score = 1;
    let mut best = c.clone();
    let mut iter = 0usize;
    while now.elapsed().as_secs_f64() < 1.98 {
        iter += 1;
        let (h, w) = (s.len(), s[0].len());
        const W1: u64 = 4;
        const AL1: u64 = 2;
        const AR1: u64 = AL1 + W1 - 1;
        const BL1: u64 = AL1 + W1;
        const BR1: u64 = BL1 + W1 - 1;
        const W2: u64 = 8;
        const AL2: u64 = BR1 + 1;
        const AR2: u64 = AL2 + W2 - 1;
        const BL2: u64 = AL2 + W2;
        const BR2: u64 = BL2 + W2 - 1;
        let ty = if iter <= 1000 {
            rng.rand(2)
        } else if iter <= 10000 {
            rng.rand(2 + W1 * 2)
        } else {
            rng.rand(2 + W1 * 2 + W2 * 2)
        };
        let i = rng.rand(h as u64 - 2) as usize + 1;
        let j = rng.rand(w as u64 - 2) as usize + 1;
        let a = s[i][j];
        macro_rules! go {
            (1, $c:expr) => {
                let c = $c;
                if check_adj(&c) {
                    s = c;
                    if s[1].iter().all(|&s| s == 0) {
                        s.remove(0);
                    }
                    if s[s.len() - 2].iter().all(|&s| s == 0) {
                        s.pop();
                    }
                    if s.iter().all(|s| s[1] == 0) {
                        for s in &mut s {
                            s.remove(0);
                        }
                    }
                    if s.iter().all(|s| s[s.len() - 2] == 0) {
                        for s in &mut s {
                            s.pop();
                        }
                    }
                    let sc =
                        (n - 2) * (n - 2) + 1 - s.iter().flatten().filter(|&&s| s != 0).count();
                    if sc > score {
                        score = sc;
                        eprintln!("(score, iter) = {:?}", (score, iter));
                        best = {
                            let mut d = vec![vec![0; n - 2]; n - 2];
                            for (i, s) in s.iter().enumerate() {
                                for (j, &s) in s.iter().enumerate() {
                                    if (1..=n - 2).contains(&i) && (1..=n - 2).contains(&j) {
                                        d[i - 1][j - 1] = s;
                                    }
                                }
                            }
                            d
                        };
                        // if ty <= 4 {
                        //     pp!(@it2d &best);
                        // }
                    }
                }
            };
            (2, $s:ident) => {
                if check_single(&s, i, j, a) {
                    if s[1].iter().all(|&s| s == 0) {
                        s.remove(0);
                    }
                    if s[s.len() - 2].iter().all(|&s| s == 0) {
                        s.pop();
                    }
                    if s.iter().all(|s| s[1] == 0) {
                        for s in &mut s {
                            s.remove(0);
                        }
                    }
                    if s.iter().all(|s| s[s.len() - 2] == 0) {
                        for s in &mut s {
                            s.pop();
                        }
                    }
                    let sc =
                        (n - 2) * (n - 2) + 1 - s.iter().flatten().filter(|&&s| s != 0).count();
                    if sc > score {
                        score = sc;
                        eprintln!("(score, iter) = {:?}", (score, iter));
                        best = {
                            let mut d = vec![vec![0; n - 2]; n - 2];
                            for (i, s) in s.iter().enumerate() {
                                for (j, &s) in s.iter().enumerate() {
                                    if (1..=n - 2).contains(&i) && (1..=n - 2).contains(&j) {
                                        d[i - 1][j - 1] = s;
                                    }
                                }
                            }
                            d
                        };
                        // if ty <= 4 {
                        //     pp!(@it2d &best);
                        // }
                    }
                } else {
                    s[i][j] = a;
                }
            };
        }
        match ty {
            0 => {
                let mut c = s.clone();
                c.remove(i);
                go!(1, c);
            }
            1 => {
                let mut c = s.clone();
                for c in c.iter_mut() {
                    c.remove(j);
                }
                go!(1, c);
            }
            AL1..=AR1 => {
                let mut c = s.clone();
                match rng.rand(6) {
                    0 => {
                        for x in (1..=i).rev() {
                            for y in 0..=j {
                                c[x][y] = c[x - 1][y];
                            }
                        }
                    }
                    1 => {
                        for x in i..h - 1 {
                            for y in 0..=j {
                                c[x][y] = c[x + 1][y];
                            }
                        }
                    }
                    2 => {
                        for x in (1..=i).rev() {
                            for y in j..w {
                                c[x][y] = c[x - 1][y];
                            }
                        }
                    }
                    3 => {
                        for x in i..h - 1 {
                            for y in j..w {
                                c[x][y] = c[x + 1][y];
                            }
                        }
                    }
                    4 => {
                        let k = rng.rand(w as _) as usize + 1;
                        let l = rng.rand((w - k + 1) as _) as usize;
                        let r = l + k;
                        for x in (1..=i).rev() {
                            for y in l..r {
                                c[x][y] = c[x - 1][y];
                            }
                        }
                    }
                    _ => {
                        let k = rng.rand(w as _) as usize + 1;
                        let l = rng.rand((w - k + 1) as _) as usize;
                        let r = l + k;
                        for x in i..h - 1 {
                            for y in l..r {
                                c[x][y] = c[x - 1][y];
                            }
                        }
                    }
                }
                go!(1, c);
            }
            BL1..=BR1 => {
                let mut c = s.clone();
                match rng.rand(6) {
                    0 => {
                        for y in (1..=j).rev() {
                            for x in 0..=i {
                                c[x][y] = c[x][y - 1];
                            }
                        }
                    }
                    1 => {
                        for y in j..w - 1 {
                            for x in 0..=i {
                                c[x][y] = c[x][y + 1];
                            }
                        }
                    }
                    2 => {
                        for y in (1..=j).rev() {
                            for x in i..h {
                                c[x][y] = c[x][y - 1];
                            }
                        }
                    }
                    3 => {
                        for y in j..w - 1 {
                            for x in i..h {
                                c[x][y] = c[x][y + 1];
                            }
                        }
                    }
                    4 => {
                        let k = rng.rand(h as _) as usize + 1;
                        let l = rng.rand((h - k + 1) as _) as usize;
                        let r = l + k;
                        for y in (1..=j).rev() {
                            for x in l..r {
                                c[x][y] = c[x][y - 1];
                            }
                        }
                    }
                    _ => {
                        let k = rng.rand(h as _) as usize + 1;
                        let l = rng.rand((h - k + 1) as _) as usize;
                        let r = l + k;
                        for y in j..w - 1 {
                            for x in l..r {
                                c[x][y] = c[x][y + 1];
                            }
                        }
                    }
                }
                go!(1, c);
            }
            AL2..=AR2 if s[i][j] != 0 => {
                if rng.gen_bool(0.5) {
                    if s[i][j] == s[i - 1][j] {
                        continue;
                    }
                    s[i][j] = s[i - 1][j];
                } else {
                    if s[i][j] == s[i + 1][j] {
                        continue;
                    }
                    s[i][j] = s[i + 1][j];
                }
                go!(2, s);
            }
            BL2..=BR2 if s[i][j] != 0 => {
                if rng.gen_bool(0.5) {
                    if s[i][j] == s[i][j - 1] {
                        continue;
                    }
                    s[i][j] = s[i][j - 1];
                } else {
                    if s[i][j] == s[i][j + 1] {
                        continue;
                    }
                    s[i][j] = s[i][j + 1];
                }
                go!(2, s);
            }
            _ => {
                continue;
            }
        }
    }
    pp!(@it2d best);
    eprintln!("iter = {:?}", iter);
}
crate::main!();
#[allow(unused_imports)]use std::{cmp::{Ordering,Reverse},collections::{BTreeMap,BTreeSet,BinaryHeap,HashMap,HashSet,VecDeque}};
mod main_macros{#[doc=" Prepare useful macros."]#[doc=" - `prepare!();`: default (all input scanner (`sc!`, `sv!`) + buf print (`pp!`, `dg!`))"]#[doc=" - `prepare!(?);`: interactive (line scanner (`scln!`) + buf print (`pp!`, `dg!`))"]#[macro_export]#[allow(clippy::crate_in_macro_def)]macro_rules!prepare{(@output($dol:tt))=>{#[allow(unused_imports)]use std::io::Write as _;let __out=std::io::stdout();#[allow(unused_mut,unused_variables)]let mut __out=std::io::BufWriter::new(__out.lock());#[allow(unused_macros)]#[doc=" [`iter_print!`] for buffered stdout."]macro_rules!pp{($dol($dol t:tt)*)=>{$dol crate::iter_print!(__out,$dol($dol t)*)}}#[cfg(debug_assertions)]#[allow(unused_macros)]#[doc=" [`iter_print!`] for buffered stderr. Do nothing in release mode."]macro_rules!dg{($dol($dol t:tt)*)=>{{#[allow(unused_imports)]use std::io::Write as _;let __err=std::io::stderr();#[allow(unused_mut,unused_variables)]let mut __err=std::io::BufWriter::new(__err.lock());$dol crate::iter_print!(__err,$dol($dol t)*);let _=__err.flush();}}}#[cfg(not(debug_assertions))]#[allow(unused_macros)]#[doc=" [`iter_print!`] for buffered stderr. Do nothing in release mode."]macro_rules!dg{($dol($dol t:tt)*)=>{}}};(@normal($dol:tt))=>{let __in_buf=read_stdin_all_unchecked();#[allow(unused_mut,unused_variables)]let mut __scanner=Scanner::new(&__in_buf);#[allow(unused_macros)]macro_rules!sc{($dol($dol t:tt)*)=>{$dol crate::scan!(__scanner,$dol($dol t)*)}}#[allow(unused_macros)]macro_rules!sv{($dol($dol t:tt)*)=>{$dol crate::scan_value!(__scanner,$dol($dol t)*)}}};(@interactive($dol:tt))=>{#[allow(unused_macros)]#[doc=" Scan a line, and previous line will be truncated in the next call."]macro_rules!scln{($dol($dol t:tt)*)=>{let __in_buf=read_stdin_line();#[allow(unused_mut,unused_variables)]let mut __scanner=Scanner::new(&__in_buf);$dol crate::scan!(__scanner,$dol($dol t)*)}}};()=>{$crate::prepare!(@output($));$crate::prepare!(@normal($))};(?)=>{$crate::prepare!(@output($));$crate::prepare!(@interactive($))};}#[macro_export]macro_rules!main{()=>{fn main(){solve();}};(avx2)=>{fn main(){#[target_feature(enable="avx2")]unsafe fn solve_avx2(){solve();}unsafe{solve_avx2()}}};(large_stack)=>{fn main(){const STACK_SIZE:usize=512*1024*1024;::std::thread::Builder::new().stack_size(STACK_SIZE).spawn(solve).unwrap().join().unwrap();}};}}
pub use self::iter_print::IterPrint;
mod iter_print{use std::{fmt::Display,io::{Error,Write}};pub trait IterPrint{fn iter_print<W,S>(self,writer:&mut W,sep:S,is_head:bool)->Result<(),Error>where W:Write,S:Display;}macro_rules!impl_iter_print_tuple{(@impl$($A:ident$a:ident)?,$($B:ident$b:ident)*)=>{impl<$($A,)?$($B),*>IterPrint for($($A,)?$($B),*)where$($A:Display,)?$($B:Display),*{#[allow(unused_variables)]fn iter_print<W,S>(self,writer:&mut W,sep:S,is_head:bool)->Result<(),Error>where W:Write,S:Display{let($($a,)?$($b,)*)=self;$(if is_head{::std::write!(writer,"{}",$a)?;}else{::std::write!(writer,"{}{}",sep,$a)?;})?$(::std::write!(writer,"{}{}",sep,$b)?;)*Ok(())}}};(@inc,,$C:ident$c:ident$($D:ident$d:ident)*)=>{impl_iter_print_tuple!(@impl,);impl_iter_print_tuple!(@inc$C$c,,$($D$d)*);};(@inc$A:ident$a:ident,$($B:ident$b:ident)*,$C:ident$c:ident$($D:ident$d:ident)*)=>{impl_iter_print_tuple!(@impl$A$a,$($B$b)*);impl_iter_print_tuple!(@inc$A$a,$($B$b)*$C$c,$($D$d)*);};(@inc$A:ident$a:ident,$($B:ident$b:ident)*,)=>{impl_iter_print_tuple!(@impl$A$a,$($B$b)*);};($($t:tt)*)=>{impl_iter_print_tuple!(@inc,,$($t)*);};}impl_iter_print_tuple!(A a B b C c D d E e F f G g H h I i J j K k);#[doc=" Print expressions with a separator."]#[doc=" - `iter_print!(writer, args...)`"]#[doc=" - `@sep $expr`: set separator (default: `' '`)"]#[doc=" - `@ns`: alias for `@sep \"\"`"]#[doc=" - `@lf`: alias for `@sep '\\n'`"]#[doc=" - `@sp`: alias for `@sep ' '`"]#[doc=" - `@fmt ($lit, $($expr),*)`: print `format!($lit, $($expr),*)`"]#[doc=" - `@flush`: flush writer (auto insert `!`)"]#[doc=" - `@it $expr`: print iterator"]#[doc=" - `@it1 $expr`: print iterator as 1-indexed"]#[doc=" - `@cw ($char $expr)`: print iterator as `(elem as u8 + $char as u8) as char`"]#[doc=" - `@bw ($byte $expr)`: print iterator as `(elem as u8 + $byte) as char`"]#[doc=" - `@it2d $expr`: print 2d-iterator"]#[doc=" - `@tup $expr`: print tuple (need to import [`IterPrint`])"]#[doc=" - `@ittup $expr`: print iterative tuple (need to import [`IterPrint`])"]#[doc=" - `$expr`: print expr"]#[doc=" - `{ args... }`: scoped"]#[doc=" - `;`: print `'\\n'`"]#[doc=" - `!`: not print `'\\n'` at the end"]#[macro_export]macro_rules!iter_print{(@@fmt$writer:expr,$sep:expr,$is_head:expr,($lit:literal$(,$e:expr)*$(,)?))=>{if!$is_head{::std::write!($writer,"{}",$sep).expect("io error");}::std::write!($writer,$lit,$($e),*).expect("io error");};(@@item$writer:expr,$sep:expr,$is_head:expr,$e:expr)=>{$crate::iter_print!(@@fmt$writer,$sep,$is_head,("{}",$e));};(@@line_feed$writer:expr$(,)?)=>{::std::writeln!($writer).expect("io error");};(@@it$writer:expr,$sep:expr,$is_head:expr,$iter:expr)=>{{let mut iter=$iter.into_iter();if let Some(item)=iter.next(){$crate::iter_print!(@@item$writer,$sep,$is_head,item);}for item in iter{$crate::iter_print!(@@item$writer,$sep,false,item);}}};(@@it1$writer:expr,$sep:expr,$is_head:expr,$iter:expr)=>{{let mut iter=$iter.into_iter();if let Some(item)=iter.next(){$crate::iter_print!(@@item$writer,$sep,$is_head,item+1);}for item in iter{$crate::iter_print!(@@item$writer,$sep,false,item+1);}}};(@@cw$writer:expr,$sep:expr,$is_head:expr,($ch:literal$iter:expr))=>{{let mut iter=$iter.into_iter();let b=$ch as u8;if let Some(item)=iter.next(){$crate::iter_print!(@@item$writer,$sep,$is_head,(item as u8+b)as char);}for item in iter{$crate::iter_print!(@@item$writer,$sep,false,(item as u8+b)as char);}}};(@@bw$writer:expr,$sep:expr,$is_head:expr,($b:literal$iter:expr))=>{{let mut iter=$iter.into_iter();let b:u8=$b;if let Some(item)=iter.next(){$crate::iter_print!(@@item$writer,$sep,$is_head,(item as u8+b)as char);}for item in iter{$crate::iter_print!(@@item$writer,$sep,false,(item as u8+b)as char);}}};(@@it2d$writer:expr,$sep:expr,$is_head:expr,$iter:expr)=>{let mut iter=$iter.into_iter();if let Some(item)=iter.next(){$crate::iter_print!(@@it$writer,$sep,$is_head,item);}for item in iter{$crate::iter_print!(@@line_feed$writer);$crate::iter_print!(@@it$writer,$sep,true,item);}};(@@tup$writer:expr,$sep:expr,$is_head:expr,$tuple:expr)=>{IterPrint::iter_print($tuple,&mut$writer,$sep,$is_head).expect("io error");};(@@ittup$writer:expr,$sep:expr,$is_head:expr,$iter:expr)=>{let mut iter=$iter.into_iter();if let Some(item)=iter.next(){$crate::iter_print!(@@tup$writer,$sep,$is_head,item);}for item in iter{$crate::iter_print!(@@line_feed$writer);$crate::iter_print!(@@tup$writer,$sep,true,item);}};(@@assert_tag item)=>{};(@@assert_tag it)=>{};(@@assert_tag it1)=>{};(@@assert_tag it2d)=>{};(@@assert_tag tup)=>{};(@@assert_tag ittup)=>{};(@@assert_tag$tag:ident)=>{::std::compile_error!(::std::concat!("invalid tag in `iter_print!`: `",std::stringify!($tag),"`"));};(@@inner$writer:expr,$sep:expr,$is_head:expr,@sep$e:expr,$($t:tt)*)=>{$crate::iter_print!(@@inner$writer,$e,$is_head,$($t)*);};(@@inner$writer:expr,$sep:expr,$is_head:expr,@ns$($t:tt)*)=>{$crate::iter_print!(@@inner$writer,"",$is_head,$($t)*);};(@@inner$writer:expr,$sep:expr,$is_head:expr,@lf$($t:tt)*)=>{$crate::iter_print!(@@inner$writer,'\n',$is_head,$($t)*);};(@@inner$writer:expr,$sep:expr,$is_head:expr,@sp$($t:tt)*)=>{$crate::iter_print!(@@inner$writer,' ',$is_head,$($t)*);};(@@inner$writer:expr,$sep:expr,$is_head:expr,@flush$($t:tt)*)=>{$writer.flush().expect("io error");$crate::iter_print!(@@inner$writer,$sep,$is_head,!$($t)*);};(@@inner$writer:expr,$sep:expr,$is_head:expr,@fmt$arg:tt$($t:tt)*)=>{$crate::iter_print!(@@fmt$writer,$sep,$is_head,$arg);$crate::iter_print!(@@inner$writer,$sep,$is_head,$($t)*);};(@@inner$writer:expr,$sep:expr,$is_head:expr,@cw$arg:tt$($t:tt)*)=>{$crate::iter_print!(@@cw$writer,$sep,$is_head,$arg);$crate::iter_print!(@@inner$writer,$sep,$is_head,$($t)*);};(@@inner$writer:expr,$sep:expr,$is_head:expr,@bw$arg:tt$($t:tt)*)=>{$crate::iter_print!(@@bw$writer,$sep,$is_head,$arg);$crate::iter_print!(@@inner$writer,$sep,$is_head,$($t)*);};(@@inner$writer:expr,$sep:expr,$is_head:expr,@$tag:ident$e:expr,$($t:tt)*)=>{$crate::iter_print!(@@assert_tag$tag);$crate::iter_print!(@@$tag$writer,$sep,$is_head,$e);$crate::iter_print!(@@inner$writer,$sep,false,$($t)*);};(@@inner$writer:expr,$sep:expr,$is_head:expr,@$tag:ident$e:expr;$($t:tt)*)=>{$crate::iter_print!(@@assert_tag$tag);$crate::iter_print!(@@$tag$writer,$sep,$is_head,$e);$crate::iter_print!(@@line_feed$writer);$crate::iter_print!(@@inner$writer,$sep,true,$($t)*);};(@@inner$writer:expr,$sep:expr,$is_head:expr,@$tag:ident$e:expr)=>{$crate::iter_print!(@@assert_tag$tag);$crate::iter_print!(@@$tag$writer,$sep,$is_head,$e);$crate::iter_print!(@@inner$writer,$sep,false,);};(@@inner$writer:expr,$sep:expr,$is_head:expr,@$tag:ident$($t:tt)*)=>{::std::compile_error!(::std::concat!("invalid expr in `iter_print!`: `",std::stringify!($($t)*),"`"));};(@@inner$writer:expr,$sep:expr,$is_head:expr,,$($t:tt)*)=>{$crate::iter_print!(@@inner$writer,$sep,$is_head,$($t)*);};(@@inner$writer:expr,$sep:expr,$is_head:expr,;$($t:tt)*)=>{$crate::iter_print!(@@line_feed$writer);$crate::iter_print!(@@inner$writer,$sep,$is_head,$($t)*);};(@@inner$writer:expr,$sep:expr,$is_head:expr,!$(,)?)=>{};(@@inner$writer:expr,$sep:expr,$is_head:expr,!$($t:tt)*)=>{$crate::iter_print!(@@inner$writer,$sep,$is_head,$($t)*);};(@@inner$writer:expr,$sep:expr,$is_head:expr,)=>{$crate::iter_print!(@@line_feed$writer);};(@@inner$writer:expr,$sep:expr,$is_head:expr,{$($t:tt)*}$($rest:tt)*)=>{$crate::iter_print!(@@inner$writer,$sep,$is_head,$($t)*,!);$crate::iter_print!(@@inner$writer,$sep,$is_head,$($rest)*);};(@@inner$writer:expr,$sep:expr,$is_head:expr,$($t:tt)*)=>{$crate::iter_print!(@@inner$writer,$sep,$is_head,@item$($t)*);};($writer:expr,$($t:tt)*)=>{{$crate::iter_print!(@@inner$writer,' ',true,$($t)*);}};}}
mod array{#[macro_export]macro_rules!array{[@inner$data:ident=[$init:expr;$len:expr]]=>{{use::std::mem::{ManuallyDrop,MaybeUninit};let mut$data:[MaybeUninit<_>;$len]=unsafe{MaybeUninit::uninit().assume_init()};$init;#[repr(C)]union __Transmuter<const N:usize,T:Clone>{src:ManuallyDrop<[MaybeUninit<T>;N]>,dst:ManuallyDrop<[T;N]>,}ManuallyDrop::into_inner(unsafe{__Transmuter{src:ManuallyDrop::new($data)}.dst})}};[||$e:expr;$len:expr]=>{$crate::array![@inner data=[data.iter_mut().for_each(|item|*item=MaybeUninit::new($e));$len]]};[|$i:pat_param|$e:expr;$len:expr]=>{$crate::array![@inner data=[data.iter_mut().enumerate().for_each(|($i,item)|*item=MaybeUninit::new($e));$len]]};[$e:expr;$len:expr]=>{{let e=$e;$crate::array![||Clone::clone(&e);$len]}};}}
pub use self::scanner::*;
mod scanner{use std::{iter::{from_fn,repeat_with,FromIterator},marker::PhantomData};pub fn read_stdin_all()->String{use std::io::Read as _;let mut s=String::new();std::io::stdin().read_to_string(&mut s).expect("io error");s}pub fn read_stdin_all_unchecked()->String{use std::io::Read as _;let mut buf=Vec::new();std::io::stdin().read_to_end(&mut buf).expect("io error");unsafe{String::from_utf8_unchecked(buf)}}pub fn read_all(mut reader:impl std::io::Read)->String{let mut s=String::new();reader.read_to_string(&mut s).expect("io error");s}pub fn read_all_unchecked(mut reader:impl std::io::Read)->String{let mut buf=Vec::new();reader.read_to_end(&mut buf).expect("io error");unsafe{String::from_utf8_unchecked(buf)}}pub fn read_stdin_line()->String{let mut s=String::new();std::io::stdin().read_line(&mut s).expect("io error");s}pub trait IterScan:Sized{type Output;fn scan<'a,I:Iterator<Item=&'a str>>(iter:&mut I)->Option<Self::Output>;}pub trait MarkedIterScan:Sized{type Output;fn mscan<'a,I:Iterator<Item=&'a str>>(self,iter:&mut I)->Option<Self::Output>;}#[derive(Clone,Debug)]pub struct Scanner<'a>{iter:std::str::SplitAsciiWhitespace<'a>}impl<'a>Scanner<'a>{#[inline]pub fn new(s:&'a str)->Self{let iter=s.split_ascii_whitespace();Self{iter}}#[inline]pub fn scan<T>(&mut self)-><T as IterScan>::Output where T:IterScan{<T as IterScan>::scan(&mut self.iter).expect("scan error")}#[inline]pub fn mscan<T>(&mut self,marker:T)-><T as MarkedIterScan>::Output where T:MarkedIterScan{marker.mscan(&mut self.iter).expect("scan error")}#[inline]pub fn scan_vec<T>(&mut self,size:usize)->Vec<<T as IterScan>::Output>where T:IterScan{(0..size).map(|_|<T as IterScan>::scan(&mut self.iter).expect("scan error")).collect()}#[inline]pub fn iter<'b,T>(&'b mut self)->ScannerIter<'a,'b,T>where T:IterScan{ScannerIter{inner:self,_marker:std::marker::PhantomData}}}macro_rules!impl_iter_scan{($($t:ty)*)=>{$(impl IterScan for$t{type Output=Self;#[inline]fn scan<'a,I:Iterator<Item=&'a str>>(iter:&mut I)->Option<Self>{iter.next()?.parse::<$t>().ok()}})*};}impl_iter_scan!(char u8 u16 u32 u64 usize i8 i16 i32 i64 isize f32 f64 u128 i128 String);macro_rules!impl_iter_scan_tuple{(@impl$($T:ident)*)=>{impl<$($T:IterScan),*>IterScan for($($T,)*){type Output=($(<$T as IterScan>::Output,)*);#[inline]fn scan<'a,It:Iterator<Item=&'a str>>(_iter:&mut It)->Option<Self::Output>{Some(($(<$T as IterScan>::scan(_iter)?,)*))}}};(@inner$($T:ident)*,)=>{impl_iter_scan_tuple!(@impl$($T)*);};(@inner$($T:ident)*,$U:ident$($Rest:ident)*)=>{impl_iter_scan_tuple!(@impl$($T)*);impl_iter_scan_tuple!(@inner$($T)*$U,$($Rest)*);};($($T:ident)*)=>{impl_iter_scan_tuple!(@inner,$($T)*);};}impl_iter_scan_tuple!(A B C D E F G H I J K);pub struct ScannerIter<'a,'b,T>{inner:&'b mut Scanner<'a>,_marker:std::marker::PhantomData<fn()->T>}impl<'a,'b,T>Iterator for ScannerIter<'a,'b,T>where T:IterScan{type Item=<T as IterScan>::Output;#[inline]fn next(&mut self)->Option<Self::Item>{<T as IterScan>::scan(&mut self.inner.iter)}}#[doc=" scan a value with Scanner"]#[doc=""]#[doc=" - `scan_value!(scanner, ELEMENT)`"]#[doc=""]#[doc=" ELEMENT :="]#[doc=" - `$ty`: IterScan"]#[doc=" - `@$expr`: MarkedIterScan"]#[doc=" - `[ELEMENT; $expr]`: vector"]#[doc=" - `[ELEMENT; const $expr]`: array"]#[doc=" - `[ELEMENT]`: iterator"]#[doc=" - `($(ELEMENT)*,)`: tuple"]#[macro_export]macro_rules!scan_value{(@repeat$scanner:expr,[$($t:tt)*]$($len:expr)?)=>{::std::iter::repeat_with(||$crate::scan_value!(@inner$scanner,[]$($t)*))$(.take($len).collect::<Vec<_>>())?};(@array$scanner:expr,[$($t:tt)*]$len:expr)=>{$crate::array![||$crate::scan_value!(@inner$scanner,[]$($t)*);$len]};(@tuple$scanner:expr,[$([$($args:tt)*])*])=>{($($($args)*,)*)};(@$tag:ident$scanner:expr,[[$($args:tt)*]])=>{$($args)*};(@$tag:ident$scanner:expr,[$($args:tt)*]@$e:expr)=>{$crate::scan_value!(@$tag$scanner,[$($args)*[$scanner.mscan($e)]])};(@$tag:ident$scanner:expr,[$($args:tt)*]@$e:expr,$($t:tt)*)=>{$crate::scan_value!(@$tag$scanner,[$($args)*[$scanner.mscan($e)]]$($t)*)};(@$tag:ident$scanner:expr,[$($args:tt)*]($($tuple:tt)*)$($t:tt)*)=>{$crate::scan_value!(@$tag$scanner,[$($args)*[$crate::scan_value!(@tuple$scanner,[]$($tuple)*)]]$($t)*)};(@$tag:ident$scanner:expr,[$($args:tt)*][@$e:expr;const$len:expr]$($t:tt)*)=>{$crate::scan_value!(@$tag$scanner,[$($args)*[$crate::scan_value!(@array$scanner,[@$e]$len)]]$($t)*)};(@$tag:ident$scanner:expr,[$($args:tt)*][@$e:expr;$len:expr]$($t:tt)*)=>{$crate::scan_value!(@$tag$scanner,[$($args)*[$crate::scan_value!(@repeat$scanner,[@$e]$len)]]$($t)*)};(@$tag:ident$scanner:expr,[$($args:tt)*][[$($tt:tt)*];const$len:expr]$($t:tt)*)=>{$crate::scan_value!(@$tag$scanner,[$($args)*[$crate::scan_value!(@array$scanner,[[$($tt)*]]$len)]]$($t)*)};(@$tag:ident$scanner:expr,[$($args:tt)*][[$($tt:tt)*];$len:expr]$($t:tt)*)=>{$crate::scan_value!(@$tag$scanner,[$($args)*[$crate::scan_value!(@repeat$scanner,[[$($tt)*]]$len)]]$($t)*)};(@$tag:ident$scanner:expr,[$($args:tt)*][($($tt:tt)*);const$len:expr]$($t:tt)*)=>{$crate::scan_value!(@$tag$scanner,[$($args)*[$crate::scan_value!(@array$scanner,[($($tt)*)]$len)]]$($t)*)};(@$tag:ident$scanner:expr,[$($args:tt)*][($($tt:tt)*);$len:expr]$($t:tt)*)=>{$crate::scan_value!(@$tag$scanner,[$($args)*[$crate::scan_value!(@repeat$scanner,[($($tt)*)]$len)]]$($t)*)};(@$tag:ident$scanner:expr,[$($args:tt)*][$ty:ty;const$len:expr]$($t:tt)*)=>{$crate::scan_value!(@$tag$scanner,[$($args)*[$crate::scan_value!(@array$scanner,[$ty]$len)]]$($t)*)};(@$tag:ident$scanner:expr,[$($args:tt)*][$ty:ty;$len:expr]$($t:tt)*)=>{$crate::scan_value!(@$tag$scanner,[$($args)*[$crate::scan_value!(@repeat$scanner,[$ty]$len)]]$($t)*)};(@$tag:ident$scanner:expr,[$($args:tt)*][$($tt:tt)*]$($t:tt)*)=>{$crate::scan_value!(@$tag$scanner,[$($args)*[$crate::scan_value!(@repeat$scanner,[$($tt)*])]]$($t)*)};(@$tag:ident$scanner:expr,[$($args:tt)*]$ty:ty)=>{$crate::scan_value!(@$tag$scanner,[$($args)*[$scanner.scan::<$ty>()]])};(@$tag:ident$scanner:expr,[$($args:tt)*]$ty:ty,$($t:tt)*)=>{$crate::scan_value!(@$tag$scanner,[$($args)*[$scanner.scan::<$ty>()]]$($t)*)};(@$tag:ident$scanner:expr,[$($args:tt)*],$($t:tt)*)=>{$crate::scan_value!(@$tag$scanner,[$($args)*]$($t)*)};(@$tag:ident$scanner:expr,[$($args:tt)*])=>{::std::compile_error!(::std::stringify!($($args)*))};($scanner:expr,$($t:tt)*)=>{$crate::scan_value!(@inner$scanner,[]$($t)*)}}#[doc=" scan and bind values with Scanner"]#[doc=""]#[doc=" - `scan!(scanner, $($pat $(: ELEMENT)?),*)`"]#[macro_export]macro_rules!scan{(@assert$p:pat)=>{};(@assert$($p:tt)*)=>{::std::compile_error!(::std::concat!("expected pattern, found `",::std::stringify!($($p)*),"`"));};(@pat$scanner:expr,[][])=>{};(@pat$scanner:expr,[][],$($t:tt)*)=>{$crate::scan!(@pat$scanner,[][]$($t)*)};(@pat$scanner:expr,[$($p:tt)*][]$x:ident$($t:tt)*)=>{$crate::scan!(@pat$scanner,[$($p)*$x][]$($t)*)};(@pat$scanner:expr,[$($p:tt)*][]::$($t:tt)*)=>{$crate::scan!(@pat$scanner,[$($p)*::][]$($t)*)};(@pat$scanner:expr,[$($p:tt)*][]&$($t:tt)*)=>{$crate::scan!(@pat$scanner,[$($p)*&][]$($t)*)};(@pat$scanner:expr,[$($p:tt)*][]($($x:tt)*)$($t:tt)*)=>{$crate::scan!(@pat$scanner,[$($p)*($($x)*)][]$($t)*)};(@pat$scanner:expr,[$($p:tt)*][][$($x:tt)*]$($t:tt)*)=>{$crate::scan!(@pat$scanner,[$($p)*[$($x)*]][]$($t)*)};(@pat$scanner:expr,[$($p:tt)*][]{$($x:tt)*}$($t:tt)*)=>{$crate::scan!(@pat$scanner,[$($p)*{$($x)*}][]$($t)*)};(@pat$scanner:expr,[$($p:tt)*][]:$($t:tt)*)=>{$crate::scan!(@ty$scanner,[$($p)*][]$($t)*)};(@pat$scanner:expr,[$($p:tt)*][]$($t:tt)*)=>{$crate::scan!(@let$scanner,[$($p)*][usize]$($t)*)};(@ty$scanner:expr,[$($p:tt)*][$($tt:tt)*]@$e:expr)=>{$crate::scan!(@let$scanner,[$($p)*][$($tt)*@$e])};(@ty$scanner:expr,[$($p:tt)*][$($tt:tt)*]@$e:expr,$($t:tt)*)=>{$crate::scan!(@let$scanner,[$($p)*][$($tt)*@$e],$($t)*)};(@ty$scanner:expr,[$($p:tt)*][$($tt:tt)*]($($x:tt)*)$($t:tt)*)=>{$crate::scan!(@let$scanner,[$($p)*][$($tt)*($($x)*)]$($t)*)};(@ty$scanner:expr,[$($p:tt)*][$($tt:tt)*][$($x:tt)*]$($t:tt)*)=>{$crate::scan!(@let$scanner,[$($p)*][$($tt)*[$($x)*]]$($t)*)};(@ty$scanner:expr,[$($p:tt)*][$($tt:tt)*]$ty:ty)=>{$crate::scan!(@let$scanner,[$($p)*][$($tt)*$ty])};(@ty$scanner:expr,[$($p:tt)*][$($tt:tt)*]$ty:ty,$($t:tt)*)=>{$crate::scan!(@let$scanner,[$($p)*][$($tt)*$ty],$($t)*)};(@let$scanner:expr,[$($p:tt)*][$($tt:tt)*]$($t:tt)*)=>{$crate::scan!{@assert$($p)*}let$($p)* =$crate::scan_value!($scanner,$($tt)*);$crate::scan!(@pat$scanner,[][]$($t)*)};($scanner:expr,$($t:tt)*)=>{$crate::scan!(@pat$scanner,[][]$($t)*)}}#[derive(Debug,Copy,Clone)]pub enum Usize1{}impl IterScan for Usize1{type Output=usize;#[inline]fn scan<'a,I:Iterator<Item=&'a str>>(iter:&mut I)->Option<Self::Output>{<usize as IterScan>::scan(iter)?.checked_sub(1)}}#[derive(Debug,Copy,Clone)]pub struct CharWithBase(pub char);impl MarkedIterScan for CharWithBase{type Output=usize;#[inline]fn mscan<'a,I:Iterator<Item=&'a str>>(self,iter:&mut I)->Option<Self::Output>{Some((<char as IterScan>::scan(iter)?as u8-self.0 as u8)as usize)}}#[derive(Debug,Copy,Clone)]pub enum Chars{}impl IterScan for Chars{type Output=Vec<char>;#[inline]fn scan<'a,I:Iterator<Item=&'a str>>(iter:&mut I)->Option<Self::Output>{Some(iter.next()?.chars().collect())}}#[derive(Debug,Copy,Clone)]pub struct CharsWithBase(pub char);impl MarkedIterScan for CharsWithBase{type Output=Vec<usize>;#[inline]fn mscan<'a,I:Iterator<Item=&'a str>>(self,iter:&mut I)->Option<Self::Output>{Some(iter.next()?.chars().map(|c|(c as u8-self.0 as u8)as usize).collect())}}#[derive(Debug,Copy,Clone)]pub enum Byte1{}impl IterScan for Byte1{type Output=u8;#[inline]fn scan<'a,I:Iterator<Item=&'a str>>(iter:&mut I)->Option<Self::Output>{let bytes=iter.next()?.as_bytes();assert_eq!(bytes.len(),1);Some(bytes[0])}}#[derive(Debug,Copy,Clone)]pub struct ByteWithBase(pub u8);impl MarkedIterScan for ByteWithBase{type Output=usize;#[inline]fn mscan<'a,I:Iterator<Item=&'a str>>(self,iter:&mut I)->Option<Self::Output>{Some((<char as IterScan>::scan(iter)?as u8-self.0)as usize)}}#[derive(Debug,Copy,Clone)]pub enum Bytes{}impl IterScan for Bytes{type Output=Vec<u8>;#[inline]fn scan<'a,I:Iterator<Item=&'a str>>(iter:&mut I)->Option<Self::Output>{Some(iter.next()?.bytes().collect())}}#[derive(Debug,Copy,Clone)]pub struct BytesWithBase(pub u8);impl MarkedIterScan for BytesWithBase{type Output=Vec<usize>;#[inline]fn mscan<'a,I:Iterator<Item=&'a str>>(self,iter:&mut I)->Option<Self::Output>{Some(iter.next()?.bytes().map(|c|(c-self.0)as usize).collect())}}#[derive(Debug,Copy,Clone)]pub struct Collect<T,B=Vec<<T as IterScan>::Output>>where T:IterScan,B:FromIterator<<T as IterScan>::Output>{size:usize,_marker:PhantomData<fn()->(T,B)>}impl<T,B>Collect<T,B>where T:IterScan,B:FromIterator<<T as IterScan>::Output>{pub fn new(size:usize)->Self{Self{size,_marker:PhantomData}}}impl<T,B>MarkedIterScan for Collect<T,B>where T:IterScan,B:FromIterator<<T as IterScan>::Output>{type Output=B;#[inline]fn mscan<'a,I:Iterator<Item=&'a str>>(self,iter:&mut I)->Option<Self::Output>{repeat_with(||<T as IterScan>::scan(iter)).take(self.size).collect()}}#[derive(Debug,Copy,Clone)]pub struct SizedCollect<T,B=Vec<<T as IterScan>::Output>>where T:IterScan,B:FromIterator<<T as IterScan>::Output>{_marker:PhantomData<fn()->(T,B)>}impl<T,B>IterScan for SizedCollect<T,B>where T:IterScan,B:FromIterator<<T as IterScan>::Output>{type Output=B;#[inline]fn scan<'a,I:Iterator<Item=&'a str>>(iter:&mut I)->Option<Self::Output>{let size=usize::scan(iter)?;repeat_with(||<T as IterScan>::scan(iter)).take(size).collect()}}#[derive(Debug,Copy,Clone)]pub struct Splitted<T,P>where T:IterScan{pat:P,_marker:PhantomData<fn()->T>}impl<T,P>Splitted<T,P>where T:IterScan{pub fn new(pat:P)->Self{Self{pat,_marker:PhantomData}}}impl<T>MarkedIterScan for Splitted<T,char>where T:IterScan{type Output=Vec<<T as IterScan>::Output>;fn mscan<'a,I:Iterator<Item=&'a str>>(self,iter:&mut I)->Option<Self::Output>{let mut iter=iter.next()?.split(self.pat);Some(from_fn(||<T as IterScan>::scan(&mut iter)).collect())}}impl<T>MarkedIterScan for Splitted<T,&str>where T:IterScan{type Output=Vec<<T as IterScan>::Output>;fn mscan<'a,I:Iterator<Item=&'a str>>(self,iter:&mut I)->Option<Self::Output>{let mut iter=iter.next()?.split(self.pat);Some(from_fn(||<T as IterScan>::scan(&mut iter)).collect())}}impl<T,F>MarkedIterScan for F where F:Fn(&str)->Option<T>{type Output=T;fn mscan<'a,I:Iterator<Item=&'a str>>(self,iter:&mut I)->Option<Self::Output>{self(iter.next()?)}}}
pub use self::xorshift::Xorshift;
mod xorshift{use std::{collections::hash_map::RandomState,hash::{BuildHasher,Hasher},iter::repeat_with};#[derive(Clone,Debug)]pub struct Xorshift{y:u64}impl Xorshift{pub fn new_with_seed(seed:u64)->Self{Xorshift{y:seed}}pub fn new()->Self{Xorshift::new_with_seed(RandomState::new().build_hasher().finish())}pub fn rand64(&mut self)->u64{self.y^=self.y<<5;self.y^=self.y>>17;self.y^=self.y<<11;self.y}pub fn rand(&mut self,k:u64)->u64{self.rand64()%k}pub fn rands(&mut self,k:u64,n:usize)->Vec<u64>{repeat_with(||self.rand(k)).take(n).collect()}pub fn randf(&mut self)->f64{const UPPER_MASK:u64=0x3FF0_0000_0000_0000;const LOWER_MASK:u64=0x000F_FFFF_FFFF_FFFF;let x=self.rand64();let tmp=UPPER_MASK|(x&LOWER_MASK);let result:f64=f64::from_bits(tmp);f64::from_bits(f64::to_bits(result-1.0)^x>>63)}pub fn gen_bool(&mut self,p:f64)->bool{self.randf()<p}pub fn shuffle<T>(&mut self,slice:&mut[T]){let mut n=slice.len();while n>1{let i=self.rand(n as _)as usize;n-=1;slice.swap(i,n);}}}impl Default for Xorshift{fn default()->Self{Xorshift::new_with_seed(0x2b99_2ddf_a232_49d6)}}}
pub use self::grid::GridGraph;
mod grid{use super::{Adjacencies,AdjacenciesWithValue,AdjacencyView,AdjacencyViewIterFromValue,GraphBase,VIndexWithValue,VertexMap,VertexView,Vertices};use std::{iter::Map,marker::PhantomData,ops::Range};#[derive(Debug,Clone,Copy)]pub struct GridGraph<A>{pub height:usize,pub width:usize,_marker:PhantomData<fn()->A>}impl GridGraph<Adj4>{pub fn new_adj4(height:usize,width:usize)->Self{Self::new(height,width)}pub fn adj4(&self,vid:(usize,usize))->GridAdjacency<Adj4>{GridAdjacency{g:self,xy:vid,diter:GridDirectionIter::default(),_marker:PhantomData}}}impl GridGraph<Adj8>{pub fn new_adj8(height:usize,width:usize)->Self{Self::new(height,width)}pub fn adj8(&self,vid:(usize,usize))->GridAdjacency<Adj8>{GridAdjacency{g:self,xy:vid,diter:GridDirectionIter::default(),_marker:PhantomData}}}impl<A>GridGraph<A>{pub fn new(height:usize,width:usize)->Self{Self{height,width,_marker:PhantomData}}pub fn move_by_diff(&self,xy:(usize,usize),dxdy:(isize,isize))->Option<(usize,usize)>{let nx=xy.0 .wrapping_add(dxdy.0 as usize);let ny=xy.1 .wrapping_add(dxdy.1 as usize);if nx<self.height&&ny<self.width{Some((nx,ny))}else{None}}pub fn flat(&self,xy:(usize,usize))->usize{xy.0*self.width+xy.1}pub fn unflat(&self,pos:usize)->(usize,usize){(pos/self.width,pos%self.width)}}impl<A>GraphBase<'_>for GridGraph<A>{type VIndex=(usize,usize);}impl<A>Vertices<'_>for GridGraph<A>{type VIter=GridVertices;fn vertices(&self)->Self::VIter{GridVertices{xrange:0..self.height,yrange:0..self.width}}}#[derive(Debug,Clone)]pub struct GridVertices{xrange:Range<usize>,yrange:Range<usize>}impl Iterator for GridVertices{type Item=(usize,usize);fn next(&mut self)->Option<Self::Item>{if self.xrange.start>=self.xrange.end{None}else if let Some(ny)=self.yrange.next(){Some((self.xrange.start,ny))}else{self.yrange.start=0;self.xrange.start+=1;self.next()}}}#[derive(Debug,Clone,Copy,PartialEq,Eq,PartialOrd,Ord,Hash)]pub enum GridDirection{U=0isize,L=1isize,R=2isize,D=3isize,UL=4isize,UR=5isize,DL=6isize,DR=7isize}impl GridDirection{pub fn dxdy(self)->(isize,isize){match self{GridDirection::U=>(-1,0),GridDirection::L=>(0,-1),GridDirection::R=>(0,1),GridDirection::D=>(1,0),GridDirection::UL=>(-1,-1),GridDirection::UR=>(-1,1),GridDirection::DL=>(1,-1),GridDirection::DR=>(1,1),}}pub fn ndxdy(self,d:usize)->(isize,isize){let d=d as isize;match self{GridDirection::U=>(-d,0),GridDirection::L=>(0,-d),GridDirection::R=>(0,d),GridDirection::D=>(d,0),GridDirection::UL=>(-d,-d),GridDirection::UR=>(-d,d),GridDirection::DL=>(d,-d),GridDirection::DR=>(d,d),}}}impl<'g>Adjacencies<'g>for GridGraph<Adj4>{type AIndex=VIndexWithValue<(usize,usize),GridDirection>;type AIter=Map<GridAdjacency<'g,Adj4>,fn(((usize,usize),GridDirection))->VIndexWithValue<(usize,usize),GridDirection>>;fn adjacencies(&'g self,vid:Self::VIndex)->Self::AIter{self.adj4(vid).map(Into::into)}}impl<'g>Adjacencies<'g>for GridGraph<Adj8>{type AIndex=VIndexWithValue<(usize,usize),GridDirection>;type AIter=Map<GridAdjacency<'g,Adj8>,fn(((usize,usize),GridDirection))->VIndexWithValue<(usize,usize),GridDirection>>;fn adjacencies(&'g self,vid:Self::VIndex)->Self::AIter{self.adj8(vid).map(Into::into)}}impl<'g>AdjacenciesWithValue<'g,GridDirection>for GridGraph<Adj4>{type AIndex=VIndexWithValue<(usize,usize),GridDirection>;type AIter=Map<GridAdjacency<'g,Adj4>,fn(((usize,usize),GridDirection))->VIndexWithValue<(usize,usize),GridDirection>>;fn adjacencies_with_value(&'g self,vid:Self::VIndex)->Self::AIter{self.adjacencies(vid)}}impl<'g>AdjacenciesWithValue<'g,GridDirection>for GridGraph<Adj8>{type AIndex=VIndexWithValue<(usize,usize),GridDirection>;type AIter=Map<GridAdjacency<'g,Adj8>,fn(((usize,usize),GridDirection))->VIndexWithValue<(usize,usize),GridDirection>>;fn adjacencies_with_value(&'g self,vid:Self::VIndex)->Self::AIter{self.adjacencies(vid)}}impl<'g,'a,M,T>AdjacencyView<'g,'a,M,T>for GridGraph<Adj4>where M:'a+Fn(GridDirection)->T{type AViewIter=AdjacencyViewIterFromValue<'g,'a,Self,M,GridDirection,T>;fn aviews(&'g self,map:&'a M,vid:Self::VIndex)->Self::AViewIter{AdjacencyViewIterFromValue::new(self.adjacencies(vid),map)}}impl<'g,'a,M,T>AdjacencyView<'g,'a,M,T>for GridGraph<Adj8>where M:'a+Fn(GridDirection)->T{type AViewIter=AdjacencyViewIterFromValue<'g,'a,Self,M,GridDirection,T>;fn aviews(&'g self,map:&'a M,vid:Self::VIndex)->Self::AViewIter{AdjacencyViewIterFromValue::new(self.adjacencies(vid),map)}}#[derive(Debug,Clone,Copy)]pub enum Adj4{}#[derive(Debug,Clone,Copy)]pub enum Adj8{}#[derive(Debug,Clone)]pub struct GridDirectionIter<A>{dir:Option<GridDirection>,_marker:PhantomData<fn()->A>}impl<A>Default for GridDirectionIter<A>{fn default()->Self{Self{dir:Some(GridDirection::U),_marker:PhantomData}}}impl Iterator for GridDirectionIter<Adj4>{type Item=GridDirection;fn next(&mut self)->Option<Self::Item>{if let Some(dir)=&mut self.dir{let cdir=Some(*dir);self.dir=match dir{GridDirection::U=>Some(GridDirection::L),GridDirection::L=>Some(GridDirection::R),GridDirection::R=>Some(GridDirection::D),_=>None,};cdir}else{None}}}impl Iterator for GridDirectionIter<Adj8>{type Item=GridDirection;fn next(&mut self)->Option<Self::Item>{if let Some(dir)=&mut self.dir{let cdir=Some(*dir);self.dir=match dir{GridDirection::U=>Some(GridDirection::L),GridDirection::L=>Some(GridDirection::R),GridDirection::R=>Some(GridDirection::D),GridDirection::D=>Some(GridDirection::UL),GridDirection::UL=>Some(GridDirection::UR),GridDirection::UR=>Some(GridDirection::DL),GridDirection::DL=>Some(GridDirection::DR),GridDirection::DR=>None,};cdir}else{None}}}#[derive(Debug,Clone)]pub struct GridAdjacency<'g,A>{g:&'g GridGraph<A>,xy:(usize,usize),diter:GridDirectionIter<A>,_marker:PhantomData<fn()->A>}impl<A>Iterator for GridAdjacency<'_,A>where GridDirectionIter<A>:Iterator<Item=GridDirection>{type Item=((usize,usize),GridDirection);fn next(&mut self)->Option<Self::Item>{for dir in self.diter.by_ref(){match self.g.move_by_diff(self.xy,dir.dxdy()){Some(nxy)=>return Some((nxy,dir)),None=>continue,}}None}}impl<A,T>VertexMap<'_,T>for GridGraph<A>{type Vmap=Vec<Vec<T>>;fn construct_vmap<F>(&self,mut f:F)->Self::Vmap where F:FnMut()->T{(0..self.height).map(|_|(0..self.width).map(|_|f()).collect()).collect()}fn vmap_get<'a>(&self,map:&'a Self::Vmap,(x,y):Self::VIndex)->&'a T{assert!(x<self.height,"expected 0..{}, but {}",self.height,x);assert!(y<self.width,"expected 0..{}, but {}",self.width,y);unsafe{map.get_unchecked(x).get_unchecked(y)}}fn vmap_get_mut<'a>(&self,map:&'a mut Self::Vmap,(x,y):Self::VIndex)->&'a mut T{assert!(x<self.height,"expected 0..{}, but {}",self.height,x);assert!(y<self.width,"expected 0..{}, but {}",self.width,y);unsafe{map.get_unchecked_mut(x).get_unchecked_mut(y)}}}impl<A,T>VertexView<'_,Vec<Vec<T>>,T>for GridGraph<A>where T:Clone{fn vview(&self,map:&Vec<Vec<T>>,vid:Self::VIndex)->T{self.vmap_get(map,vid).clone()}}}
pub use self::graph_base::*;
mod graph_base{use std::marker::PhantomData;pub trait GraphBase<'g>{type VIndex:Copy+Eq;}pub trait EIndexedGraph<'g>:GraphBase<'g>{type EIndex:Copy+Eq;}pub trait VertexSize<'g>:GraphBase<'g>{fn vsize(&'g self)->usize;}pub trait EdgeSize<'g>:GraphBase<'g>{fn esize(&'g self)->usize;}pub trait Vertices<'g>:GraphBase<'g>{type VIter:'g+Iterator<Item=Self::VIndex>;fn vertices(&'g self)->Self::VIter;}pub trait Edges<'g>:EIndexedGraph<'g>{type EIter:'g+Iterator<Item=Self::EIndex>;fn edges(&'g self)->Self::EIter;}pub trait AdjacencyIndex{type VIndex:Copy+Eq;fn vindex(&self)->Self::VIndex;}pub trait Adjacencies<'g>:GraphBase<'g>{type AIndex:'g+AdjacencyIndex<VIndex=Self::VIndex>;type AIter:'g+Iterator<Item=Self::AIndex>;fn adjacencies(&'g self,vid:Self::VIndex)->Self::AIter;}pub trait AdjacenciesWithEindex<'g>:EIndexedGraph<'g>{type AIndex:'g+AdjacencyIndexWithEindex<VIndex=Self::VIndex,EIndex=Self::EIndex>;type AIter:'g+Iterator<Item=Self::AIndex>;fn adjacencies_with_eindex(&'g self,vid:Self::VIndex)->Self::AIter;}pub trait AdjacencyIndexWithEindex:AdjacencyIndex{type EIndex:Copy+Eq;fn eindex(&self)->Self::EIndex;}pub trait AdjacencyIndexWithValue:AdjacencyIndex{type AValue:Clone;fn avalue(&self)->Self::AValue;}pub trait AdjacenciesWithValue<'g,T>:GraphBase<'g>where T:Clone{type AIndex:'g+AdjacencyIndexWithValue<VIndex=Self::VIndex,AValue=T>;type AIter:'g+Iterator<Item=Self::AIndex>;fn adjacencies_with_value(&'g self,vid:Self::VIndex)->Self::AIter;}impl AdjacencyIndex for usize{type VIndex=usize;fn vindex(&self)->Self::VIndex{*self}}impl<V,E>AdjacencyIndex for(V,E)where V:Copy+Eq,E:Copy+Eq{type VIndex=V;fn vindex(&self)->Self::VIndex{self.0}}impl<V,E>AdjacencyIndexWithEindex for(V,E)where V:Copy+Eq,E:Copy+Eq{type EIndex=E;fn eindex(&self)->Self::EIndex{self.1}}#[derive(Debug,Clone,Copy,PartialEq,Eq,PartialOrd,Ord,Hash)]pub struct VIndex<V>(V);#[derive(Debug,Clone,Copy,PartialEq,Eq,PartialOrd,Ord,Hash)]pub struct VIndexWithEIndex<V,E>(V,E);#[derive(Debug,Clone,Copy,PartialEq,Eq,PartialOrd,Ord,Hash)]pub struct VIndexWithValue<V,T>(V,T);#[derive(Debug,Clone,Copy,PartialEq,Eq,PartialOrd,Ord,Hash)]pub struct VIndexWithEIndexValue<V,E,T>(V,E,T);impl<V>AdjacencyIndex for VIndex<V>where V:Eq+Copy{type VIndex=V;fn vindex(&self)->Self::VIndex{self.0}}impl<V,E>AdjacencyIndex for VIndexWithEIndex<V,E>where V:Eq+Copy{type VIndex=V;fn vindex(&self)->Self::VIndex{self.0}}impl<V,E>AdjacencyIndexWithEindex for VIndexWithEIndex<V,E>where V:Eq+Copy,E:Eq+Copy{type EIndex=E;fn eindex(&self)->Self::EIndex{self.1}}impl<V,T>AdjacencyIndex for VIndexWithValue<V,T>where V:Eq+Copy{type VIndex=V;fn vindex(&self)->Self::VIndex{self.0}}impl<V,T>AdjacencyIndexWithValue for VIndexWithValue<V,T>where V:Eq+Copy,T:Clone{type AValue=T;fn avalue(&self)->Self::AValue{self.1 .clone()}}impl<V,E,T>AdjacencyIndex for VIndexWithEIndexValue<V,E,T>where V:Eq+Copy{type VIndex=V;fn vindex(&self)->Self::VIndex{self.0}}impl<V,E,T>AdjacencyIndexWithEindex for VIndexWithEIndexValue<V,E,T>where V:Eq+Copy,E:Eq+Copy{type EIndex=E;fn eindex(&self)->Self::EIndex{self.1}}impl<V,E,T>AdjacencyIndexWithValue for VIndexWithEIndexValue<V,E,T>where V:Eq+Copy,T:Clone{type AValue=T;fn avalue(&self)->Self::AValue{self.2 .clone()}}impl<V>From<V>for VIndex<V>{fn from(vid:V)->Self{VIndex(vid)}}impl<V,E>From<(V,E)>for VIndexWithEIndex<V,E>{fn from((vid,eid):(V,E))->Self{VIndexWithEIndex(vid,eid)}}impl<V,T>From<(V,T)>for VIndexWithValue<V,T>{fn from((vid,value):(V,T))->Self{VIndexWithValue(vid,value)}}impl<V,E,T>From<(V,E,T)>for VIndexWithEIndexValue<V,E,T>{fn from((vid,eid,value):(V,E,T))->Self{VIndexWithEIndexValue(vid,eid,value)}}impl<V,T>VIndexWithValue<V,T>{pub fn map<U,F>(self,mut f:F)->VIndexWithValue<V,U>where F:FnMut(T)->U{VIndexWithValue(self.0,f(self.1))}}impl<V,E,T>VIndexWithEIndexValue<V,E,T>{pub fn map<U,F>(self,mut f:F)->VIndexWithEIndexValue<V,E,U>where F:FnMut(T)->U{VIndexWithEIndexValue(self.0,self.1,f(self.2))}}pub trait VertexMap<'g,T>:GraphBase<'g>{type Vmap;fn construct_vmap<F>(&self,f:F)->Self::Vmap where F:FnMut()->T;fn vmap_get<'a>(&self,map:&'a Self::Vmap,vid:Self::VIndex)->&'a T;fn vmap_get_mut<'a>(&self,map:&'a mut Self::Vmap,vid:Self::VIndex)->&'a mut T;fn vmap_set(&self,map:&mut Self::Vmap,vid:Self::VIndex,x:T){*self.vmap_get_mut(map,vid)=x;}}pub trait VertexView<'g,M,T>:GraphBase<'g>where M:?Sized{fn vview(&self,map:&M,vid:Self::VIndex)->T;}pub trait EdgeMap<'g,T>:EIndexedGraph<'g>{type Emap;fn construct_emap<F>(&self,f:F)->Self::Emap where F:FnMut()->T;fn emap_get<'a>(&self,map:&'a Self::Emap,eid:Self::EIndex)->&'a T;fn emap_get_mut<'a>(&self,map:&'a mut Self::Emap,eid:Self::EIndex)->&'a mut T;fn emap_set(&self,map:&mut Self::Emap,eid:Self::EIndex,x:T){*self.emap_get_mut(map,eid)=x;}}pub trait EdgeView<'g,M,T>:EIndexedGraph<'g>where M:?Sized{fn eview(&self,map:&M,eid:Self::EIndex)->T;}impl<'g,G,F,T>VertexView<'g,F,T>for G where G:GraphBase<'g>,F:Fn(Self::VIndex)->T{fn vview(&self,map:&F,vid:Self::VIndex)->T{(map)(vid)}}impl<'g,G,F,T>EdgeView<'g,F,T>for G where G:EIndexedGraph<'g>,F:Fn(Self::EIndex)->T{fn eview(&self,map:&F,eid:Self::EIndex)->T{(map)(eid)}}pub trait AdjacencyView<'g,'a,M,T>:GraphBase<'g>where M:?Sized{type AViewIter:Iterator<Item=VIndexWithValue<Self::VIndex,T>>;fn aviews(&'g self,map:&'a M,vid:Self::VIndex)->Self::AViewIter;}pub struct AdjacencyViewIterFromEindex<'g,'a,G,M,T>where G:AdjacenciesWithEindex<'g>{iter:G::AIter,g:&'g G,map:&'a M,_marker:PhantomData<fn()->T>}impl<'g,'a,G,M,T>AdjacencyViewIterFromEindex<'g,'a,G,M,T>where G:AdjacenciesWithEindex<'g>{pub fn new(iter:G::AIter,g:&'g G,map:&'a M)->Self{Self{iter,g,map,_marker:PhantomData}}}impl<'g,'a,G,M,T>Iterator for AdjacencyViewIterFromEindex<'g,'a,G,M,T>where G:'g+AdjacenciesWithEindex<'g>+EdgeView<'g,M,T>,M:'a{type Item=VIndexWithValue<G::VIndex,T>;fn next(&mut self)->Option<Self::Item>{self.iter.next().map(|adj|(adj.vindex(),self.g.eview(self.map,adj.eindex())).into())}}pub struct AdjacencyViewIterFromValue<'g,'a,G,M,T,U>where G:AdjacenciesWithValue<'g,T>,T:Clone{iter:G::AIter,map:&'a M,_marker:PhantomData<fn()->U>}impl<'g,'a,G,M,T,U>AdjacencyViewIterFromValue<'g,'a,G,M,T,U>where G:AdjacenciesWithValue<'g,T>,T:Clone{pub fn new(iter:G::AIter,map:&'a M)->Self{Self{iter,map,_marker:PhantomData}}}impl<'g,'a,G,M,T,U>Iterator for AdjacencyViewIterFromValue<'g,'a,G,M,T,U>where G:'g+AdjacenciesWithValue<'g,T>,T:Clone,M:'a+Fn(T)->U{type Item=VIndexWithValue<G::VIndex,U>;fn next(&mut self)->Option<Self::Item>{self.iter.next().map(|adj|(adj.vindex(),(self.map)(adj.avalue())).into())}}}
