mod common {
    use std::time::{Duration, Instant};

    pub trait ChangeMinMax {
        fn change_min(&mut self, v: Self) -> bool;
        fn change_max(&mut self, v: Self) -> bool;
    }

    impl<T: PartialOrd> ChangeMinMax for T {
        fn change_min(&mut self, v: T) -> bool {
            *self > v && {
                *self = v;
                true
            }
        }

        fn change_max(&mut self, v: T) -> bool {
            *self < v && {
                *self = v;
                true
            }
        }
    }

    #[derive(Debug, Clone, Copy)]
    pub struct DurationCorrector {
        mul: f64,
    }

    #[allow(dead_code)]
    impl DurationCorrector {
        pub fn new(mul: f64) -> Self {
            Self { mul }
        }

        pub fn from_env() -> Self {
            let mul = std::env::var("DURATION_MUL")
                .ok()
                .and_then(|v| v.parse().ok())
                .unwrap_or(1.0);
            eprintln!("DURATION_MUL = {}", mul);

            Self::new(mul)
        }

        pub fn elapsed(&self, since: Instant) -> Duration {
            let elapsed = since.elapsed();
            elapsed.mul_f64(self.mul)
        }

        pub fn elapsed_f64(&self, since: Instant) -> f64 {
            let elapsed = since.elapsed();
            elapsed.as_secs_f64() * self.mul
        }
    }
}
mod data_structures {
    /// [0, n) の整数の集合を管理する定数倍が軽いデータ構造
    ///
    /// https://topcoder-tomerun.hatenablog.jp/entry/2021/06/12/134643
    #[derive(Debug, Clone)]
    pub struct IndexSet {
        values: Vec<usize>,
        positions: Vec<Option<usize>>,
    }

    #[allow(dead_code)]
    impl IndexSet {
        pub fn new(n: usize) -> Self {
            Self {
                values: vec![],
                positions: vec![None; n],
            }
        }

        pub fn add(&mut self, value: usize) {
            let pos = &mut self.positions[value];

            if pos.is_none() {
                *pos = Some(self.values.len());
                self.values.push(value);
            }
        }

        pub fn remove(&mut self, value: usize) {
            if let Some(index) = self.positions[value] {
                let last = *self.values.last().unwrap();
                self.values[index] = last;
                self.values.pop();
                self.positions[last] = Some(index);
                self.positions[value] = None;
            }
        }

        pub fn contains(&self, value: usize) -> bool {
            self.positions[value].is_some()
        }

        pub fn len(&self) -> usize {
            self.values.len()
        }

        pub fn iter(&self) -> impl Iterator<Item = &usize> {
            self.values.iter()
        }

        pub fn as_slice(&self) -> &[usize] {
            &self.values
        }
    }
}
mod distributions {
    pub struct GaussianDistribution {
        mean: f64,
        std_dev: f64,
    }

    impl GaussianDistribution {
        pub fn new(mean: f64, std_dev: f64) -> Self {
            Self { mean, std_dev }
        }

        /// 累積分布関数F(x)を計算する
        pub fn calc_cumulative_dist(&self, x: f64) -> f64 {
            // ロジスティック近似する
            let x = (x - self.mean) / self.std_dev;
            1.0 / (1.0 + (-0.07056 * x * x * x - 1.5976 * x).exp())
        }
    }

    #[cfg(test)]
    mod test {
        use super::GaussianDistribution;

        #[test]
        fn calc_cumulative_dist() {
            const TOLERANCE: f64 = 1e-3;
            let cases = [(0.0, 1.0, 0.0, 0.5), (5.0, 10.0, 0.11, 0.312420848)];

            for &(average, std_dev, x, expected) in cases.iter() {
                let gauss = GaussianDistribution::new(average, std_dev);
                let actual = gauss.calc_cumulative_dist(x);
                assert!((actual - expected).abs() < TOLERANCE);
            }
        }
    }
}
mod grid {
    use std::{fmt::Display, vec};

    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Default, Hash)]
    pub struct Coord {
        pub row: usize,
        pub col: usize,
    }

    #[allow(dead_code)]
    impl Coord {
        pub const fn new(row: usize, col: usize) -> Self {
            Self { row, col }
        }

        pub fn in_map(&self, size: usize) -> bool {
            self.row < size && self.col < size
        }

        pub const fn to_index(&self, size: usize) -> CoordIndex {
            CoordIndex(self.row * size + self.col)
        }

        pub const fn dist(&self, other: &Self) -> usize {
            Self::dist_1d(self.row, other.row) + Self::dist_1d(self.col, other.col)
        }

        const fn dist_1d(x0: usize, x1: usize) -> usize {
            (x0 as i64 - x1 as i64).abs() as usize
        }
    }

    impl Display for Coord {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            write!(f, "({}, {})", self.row, self.col)
        }
    }

    impl TryFrom<CoordDiff> for Coord {
        type Error = String;

        fn try_from(diff: CoordDiff) -> Result<Self, Self::Error> {
            if diff.dr < 0 || diff.dc < 0 {
                Err(format!("{} をCoordに変換できません", diff))
            } else {
                Ok(Self::new(diff.dr as usize, diff.dc as usize))
            }
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Default, Hash)]
    pub struct CoordDiff {
        pub dr: isize,
        pub dc: isize,
    }

    #[allow(dead_code)]
    impl CoordDiff {
        pub const fn new(dr: isize, dc: isize) -> Self {
            Self { dr, dc }
        }

        pub const fn invert(&self) -> Self {
            Self::new(-self.dr, -self.dc)
        }
    }

    impl Display for CoordDiff {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            write!(f, "({}, {})", self.dr, self.dc)
        }
    }

    impl From<Coord> for CoordDiff {
        fn from(coord: Coord) -> Self {
            Self::new(coord.row as isize, coord.col as isize)
        }
    }

    impl std::ops::Add<CoordDiff> for Coord {
        type Output = Coord;

        fn add(self, rhs: CoordDiff) -> Self::Output {
            Coord::new(
                self.row.wrapping_add_signed(rhs.dr),
                self.col.wrapping_add_signed(rhs.dc),
            )
        }
    }

    impl std::ops::Add<CoordDiff> for CoordDiff {
        type Output = CoordDiff;

        fn add(self, rhs: CoordDiff) -> Self::Output {
            CoordDiff::new(self.dr + rhs.dr, self.dc + rhs.dc)
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Default)]
    pub struct CoordIndex(pub usize);

    #[allow(dead_code)]
    impl CoordIndex {
        pub const fn new(index: usize) -> Self {
            Self(index)
        }

        pub const fn to_coord(&self, n: usize) -> Coord {
            Coord::new(self.0 / n, self.0 % n)
        }
    }

    #[allow(dead_code)]
    pub const ADJACENTS: [CoordDiff; 4] = [
        CoordDiff::new(!0, 0),
        CoordDiff::new(0, 1),
        CoordDiff::new(1, 0),
        CoordDiff::new(0, !0),
    ];

    #[allow(dead_code)]
    pub const DIRECTIONS: [char; 4] = ['U', 'R', 'D', 'L'];

    #[derive(Debug, Clone)]
    pub struct Map2d<T> {
        pub size: usize,
        map: Vec<T>,
    }

    #[allow(dead_code)]
    impl<T> Map2d<T> {
        pub fn new(map: Vec<T>, size: usize) -> Self {
            assert_eq!(size * size, map.len());
            Self { size, map }
        }

        pub fn iter(&self) -> impl Iterator<Item = &T> {
            self.map.iter()
        }

        pub fn iter_mut(&mut self) -> impl Iterator<Item = &mut T> {
            self.map.iter_mut()
        }
    }

    #[allow(dead_code)]
    impl<T: Clone> Map2d<T> {
        pub fn new_with(v: T, size: usize) -> Self {
            let map = vec![v; size * size];
            Self::new(map, size)
        }
    }

    impl<T> std::ops::Index<Coord> for Map2d<T> {
        type Output = T;

        #[inline]
        fn index(&self, coordinate: Coord) -> &Self::Output {
            &self[coordinate.to_index(self.size)]
        }
    }

    impl<T> std::ops::IndexMut<Coord> for Map2d<T> {
        #[inline]
        fn index_mut(&mut self, coordinate: Coord) -> &mut Self::Output {
            let size = self.size;
            &mut self[coordinate.to_index(size)]
        }
    }

    impl<T> std::ops::Index<CoordIndex> for Map2d<T> {
        type Output = T;

        fn index(&self, index: CoordIndex) -> &Self::Output {
            &self.map[index.0]
        }
    }

    impl<T> std::ops::IndexMut<CoordIndex> for Map2d<T> {
        #[inline]
        fn index_mut(&mut self, index: CoordIndex) -> &mut Self::Output {
            &mut self.map[index.0]
        }
    }
}
mod problem {
    pub mod params {
        use nalgebra::{DMatrix, DVector, DVectorView};

        // 初回パラメータ調整分
        const N1: &[u8] = b"AAAAAAAA8D/NzMzMzMzsPzMzMzMzM9M/zczMzMzM7D+amZmZmZnZPwAAAAAAAOA/mpmZmZmZyT9mZmZmZmbmPwAAAAAAAPA/zczMzMzM7D8AAAAAAADwPwAAAAAAAOA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/zczMzMzM7D/NzMzMzMzsP83MzMzMzOw/ZmZmZmZm5j+amZmZmZnJP5qZmZmZmdk/mpmZmZmZuT9mZmZmZmbmP2ZmZmZmZuY/AAAAAAAA4D9mZmZmZmbmP83MzMzMzOw/AAAAAAAAAAAAAAAAAAAAADMzMzMzM+M/mpmZmZmZyT8AAAAAAAAAAAAAAAAAAPA/AAAAAAAA8D/NzMzMzMzsP5qZmZmZmck/zczMzMzM7D+amZmZmZnZP83MzMzMzOw/zczMzMzM7D8AAAAAAADgPwAAAAAAAPA/mpmZmZmZyT8AAAAAAAAAAAAAAAAAAPA/MzMzMzMz0z+amZmZmZnJPwAAAAAAAAAAAAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/MzMzMzMz0z+amZmZmZnZP83MzMzMzOw/zczMzMzM7D9mZmZmZmbmPwAAAAAAAOA/MzMzMzMz4z9mZmZmZmbmPzMzMzMzM+M/MzMzMzMz0z8zMzMzMzPjP2ZmZmZmZuY/AAAAAAAA8D8zMzMzMzPTPzMzMzMzM+M/AAAAAAAA8D8zMzMzMzPjP5qZmZmZmck/mpmZmZmZyT8AAAAAAADgPzMzMzMzM+M/mpmZmZmZuT8zMzMzMzPjPwAAAAAAAPA/zczMzMzM7D+amZmZmZm5PzMzMzMzM+M/mpmZmZmZyT+amZmZmZnZP5qZmZmZmbk/MzMzMzMz0z+amZmZmZnZPzMzMzMzM+M/zczMzMzM7D8zMzMzMzPTPwAAAAAAAAAAzczMzMzM7D+amZmZmZm5P5qZmZmZmbk/";
        const M1: &[u8] = b"zTt/Zp6g1j/q+NKpfyrlPwAAAAAAAOA/6vjSqX8q5T/NO39mnqDmP6j0l5t34+E/zTt/Zp6g1j+qTFjoerbrP6pMWOh6tus/AAAAAAAA4D/aTk+x3vvuP1Nb2jpYTOk/2k5Psd777j+o9Jebd+PhPwAAAAAAAOg/zTt/Zp6g5j/ZbN/MdvjwP807f2aeoNY/qPSXm3fj4T8uIQkUjpjjPy4hCRSOmOM/zTt/Zp6g1j/zWWFEK9jsP0k/aBHq7u0/LiEJFI6Y4z/q+NKpfyrlP0k/aBHq7u0/zTt/Zp6g1j+qTFjoerbbP1Nb2jpYTOk/qkxY6Hq22z+qTFjoerbbP9pOT7He++4/81lhRCvY7D+qTFjoerbrP6pMWOh6tts/2WzfzHb48D8AAAAAAADoP807f2aeoOY/zTt/Zp6g5j+o9Jebd+PhP1Nb2jpYTOk/6vjSqX8q5T+qTFjoerbbP9pOT7He++4/6vjSqX8q5T/q+NKpfyrlPwAAAAAAAOA/zTt/Zp6g5j8H7a9mD37wP/lNZDSDb/E/zTt/Zp6g5j8AAAAAAADoP6j0l5t34+E/AAAAAAAA6D80b/2TcojqPy4hCRSOmOM/qPSXm3fj4T+o9Jebd+PhP807f2aeoNY/qPSXm3fj4T+qTFjoerbrP+r40ql/KuU/zTt/Zp6g5j8uIQkUjpjjP6pMWOh6tus/AAAAAAAA6D/NO39mnqDWP+r40ql/KuU/zTt/Zp6g1j+o9Jebd+PhP807f2aeoOY/qkxY6Hq22z8AAAAAAADoPwftr2YPfvA/zTt/Zp6g1j+qTFjoerbbP6j0l5t34+E/AAAAAAAA4D8AAAAAAADoPwAAAAAAAOA/zTt/Zp6g1j8uIQkUjpjjP6j0l5t34+E/NG/9k3KI6j+o9Jebd+PhPwAAAAAAAOA/2k5Psd777j8uIQkUjpjjPwAAAAAAAOA/";
        const EPS1: &[u8] = b"mpmZmZmZuT9nZmZmZmbmP2ZmZmZmZu4/mpmZmZmZ6T9nZmZmZmbWPzMzMzMzM9M/mpmZmZmZ4T8AAAAAAADwP5qZmZmZmeE/ZmZmZmZm7j9nZmZmZmbmP5qZmZmZmek/mpmZmZmZ4T8zMzMzMzPDPzMzMzMzM9M/mpmZmZmZyT9nZmZmZmbWP2dmZmZmZtY/zczMzMzM5D/MzMzMzMzsP5qZmZmZmeE/AAAAAAAA6D8AAAAAAADoP5qZmZmZmbk/Z2ZmZmZm1j8zMzMzMzPDP2ZmZmZmZu4/zMzMzMzM3D8AAAAAAADwPzQzMzMzM+s/mpmZmZmZ2T+amZmZmZnZP8zMzMzMzOw/mpmZmZmZ2T/NzMzMzMzkP5qZmZmZmeE/MzMzMzMzwz+amZmZmZm5PzQzMzMzM+s/mpmZmZmZuT8zMzMzMzPjPwAAAAAAANA/zczMzMzM5D/NzMzMzMzkP2ZmZmZmZu4/AAAAAAAA0D+amZmZmZnZPzMzMzMzM9M/zczMzMzM5D+amZmZmZmpP2ZmZmZmZu4/NDMzMzMz6z8AAAAAAADQPwAAAAAAAPA/NDMzMzMz6z+amZmZmZnZP5qZmZmZmek/MzMzMzMz0z/NzMzMzMzkP5qZmZmZmak/mpmZmZmZ4T8zMzMzMzPjP5qZmZmZmeE/MzMzMzMz4z8zMzMzMzPTP5qZmZmZmeE/MzMzMzMzwz+amZmZmZnZPzMzMzMzM8M/AAAAAAAA0D8AAAAAAADwP2dmZmZmZtY/AAAAAAAA6D+amZmZmZnpP5qZmZmZmck/mpmZmZmZ6T9nZmZmZmbWP5qZmZmZmak/AAAAAAAA4D80MzMzMzPrP2dmZmZmZtY/mpmZmZmZuT+amZmZmZnhP2dmZmZmZtY/mpmZmZmZ2T9nZmZmZmbmPzMzMzMzM9M/MzMzMzMz4z8zMzMzMzPTP83MzMzMzOQ/";
        const AVG1: &[u8] = b"ST9oEeru7T8AAAAAAADgP6j0l5t34+E/NG/9k3KI2j80b/2TcojaP9pOT7He+94/hR4VuY5U4j8uIQkUjpjTP/NZYUQr2Nw/NG/9k3KI6j8uIQkUjpjTP+r40ql/KtU/AAAAAAAA2D+io7MN5VLnP9ls38x2+OA/+U1kNINv4T/NO39mnqDWP191vH6/v+8/FgR3de4u4z80b/2TcojaP1Nb2jpYTNk/ST9oEeru3T/q+NKpfyrVPwAAAAAAANg/zTt/Zp6g1j9TW9o6WEzZP1Nb2jpYTNk/AAAAAAAA5D8AAAAAAADYPy4hCRSOmNM/2WzfzHb44D/q+NKpfyrVP6j0l5t349E/qkxY6Hq22z8uIQkUjpjTP4UeFbmOVOI/qPSXm3fj0T8uIQkUjpjTPwAAAAAAAOA/qPSXm3fj4T9JP2gR6u7dP1Nb2jpYTNk/6vjSqX8q1T8uIQkUjpjTP6j0l5t349E/qkxY6Hq22z+o9Jebd+PRP6pMWOh6tts/NG/9k3KI2j80b/2TcojaP6j0l5t349E/AAAAAAAA0D8uIQkUjpjTP0k/aBHq7t0/AAAAAAAA4D80b/2TcojaP+r40ql/KtU/AAAAAAAA5D80b/2TcojaP+r40ql/KuU/81lhRCvY3D8AAAAAAADYPwftr2YPfuA/ST9oEeru3T/NO39mnqDWPy4hCRSOmNM/zTt/Zp6g1j/zWWFEK9jsPy4hCRSOmNM/B+2vZg9+4D8AAAAAAADYPy4hCRSOmNM/ST9oEeru3T/NO39mnqDWP807f2aeoNY/ZBImSkfs6T80b/2TcojaP9pOT7He+94/2WzfzHb44D8uIQkUjpjTPy4hCRSOmNM/j6U20q3o5T9TW9o6WEzZPxYEd3XuLuM/6vjSqX8q1T80b/2TcojaPzRv/ZNyiNo/NG/9k3KI2j/NO39mnqDWP6pMWOh6tts/";
        const K: &[u8] = b"g5yDStk8HECfhjvSvhsMQMBHXWZUkvQ/pTrbWwPTAUBQ5pg224ICQB2T+CgXIwdA18yrtEvUBEAsIRd0C8YCQKpMWOh6tvs/1M3T8B2GEkBR5hgKgr0GQKPHujwKQvs/8IA/SKsFBUDRa3eoZUUWQPOXY1hifARAfEDnyoP5CUB1n9dmXGf5P5SODYVrORxAysmaOPJoEUCqTFjoerb7Pw8hXDUJh/Y/tMkT94OQ/z/33PKICJvpP6Yv6kszYf4/qkxY6Hq2+z/4V/FIuOMQQODD+oaTnwJAqvWVfdCpG0AoezlcPDAFQOzlkVQL8+s/oF3wv0CXGUC5DpjmMvMTQDfe//aNaAVAv1CxEBOc+T/NCkT1t//tPzrsEyb9hRpAwQrhchI/AUBUx4EfDDcGQMwynlUNcwBAunT/EpSzD0CZ7lioAfoMQFOi5thGlQBA9g0UJqGV6T98sL6rv5EEQGSoo0mdlAJAKwS7IIX9DUCdmJvHAmjzP+gx6wJkkhNA1gSNHWGq/z9++0dcAP4DQDgPzaKDafo/zTt/Zp6g5j+lXw2WWfvwPyLJURiRagNAu763Avvy5z9YkBn6CqP7P/Ox6GPaYvo/0DxMdeVlE0AERXME1wACQCyBqpU5/wNAlBdKi8ChFECh1pFKKx7nP0118AFaZQdAZru3Fl8W+z+hjm9jz5T8P84rrMHLVuw/t28XnDEtBECAjpL15S0TQA/HS5UuiwdANh3TkD+n+z+gwPszarYHQMs1mBzl0wFAdRzuuZmP9T8q/Ba9njP3PyselC5UcQBA4Xgwpu/9FkDQ3lY6uej/PwWsMA53sBdA7ehEO76u+j8W8ra3Hvz0P38CRu8BYwdAhJzco835FkBo+pM6gO4CQEViIAVIYwRAQD1wOpqD+j/I/uZmPhUEQGnPrH7IFwNAs0oHfZsqBEDylVEAM1cGQKK8MenQa+4/";
        const B: &[u8] = b"MG8+3AupoT8AXYjUR6OQPywgAszZVqc/fKQlTKxOuD+Wammbt52pPzVHf6uAjJw/Y1L+qrKtqj8SAe4zUmirP5qZmZmZmbk/Aq6bty4Brj91RPbPC5+gPwb28E028rQ/sMYzhGaNlD+AKNZSde8mP8QqK/J7Pbk/AL5osqonoz+6/4JTH1GzP0Ozk7YdYqo/SLe1uKEXsz+amZmZmZm5P8ip42YwZYc/pnpNeV5gpD/qtwgYt7SdP71COgzIv50/mpmZmZmZqT/Y/ywQHgGuPw9sj/i1yrM/fVU8Vzh5mz9TYAp3E3SiP39jcJSj8KY/kA/MN6aqsD8lbUaFZnmkP6CyVxLjmHQ/p5efinq7nT9wQx3KNL+GPw39hztsfK4/nL9mwPHUoD8o+lYm5C2PPzI5YbqUKLM/TAsWI+4ZkT8AWoWw38ERP5aWVn19maY/5wlqx18Zmz+Sy9aqmneyPyQeNu6m96U/PKmquj4Poj+9MOJLKECXP5WOAboGCqI/nPWvZAL1oz/Z9Wyf6N2iP7tyy1nfFJg/0Mb30bULqj+s1sE3/U6hP4AM3Uh5hKc/wIDNDK6Mjz8z90Jr2H+vP8VM/zBh5KQ/HbTUoTVLpT/ZbaqJGY6nP//KB1Orf6A/XRtkCgk/kj/Wj3+XrGW4P0sxuxcyP6o/L9U0D7D2nj/Fg57CFIK1P8pD0jGVLqg/dNfPTC4yoz8Jo8UMqPGwP7DAgNAJHak/uwCCTMG4iD+6wIHam8KQP+j3RYbzKWA/hEml83Lunj+hQaFgrTSpPwa6PqbVtqY/CCHyeOZVpT/ZD2YImRSyP/wrshY/FqA/PzK/LWMxqD/goW4UKI2pPwwK3pTFI5k/qOw7OzgvpD9yZpCtKMilP1pWefpPZKc/xi41xM6OsD/BZikAgaymP19Xa+VMyKQ/iH9gi2b7jD8/b6lLLPagP/h7QMygx60/";
        const R: &[u8] = b"zV1nPHWPyT959QoO8S7jP/TUeApNaOA/+BgVBbo91z+cho15kSnqPzzkZCfZ1OM/NJSFT98Q2j8osDUZMjDRP2ZmZmZmZuY/65v1qTgLwj8J46UPpgPZPyA9J6rm/uc/FsqtSauJ6j95jHfCGRLkP/cfIFdxXeQ/zM7Cuxjw6j+TUHxIbS3QPyy2ow32jd4/4vp1zdd91T9mZmZmZmbmPyQz9vhHYuM/0XCsJjP/5D8zeonLeCTpP3+A0LiTnuw/ZmZmZmZm5j/uXl8mTyPiP98pKTIOqOU/rM2pRusl5z8wqM4t2mXoP+hViO5yG+U/WEPdMd4t5z/XBOoztk3oPxwCD+T6UdY/o8lpfS4m5z8y4ATi8/7lP16QvgojM9k/dWfuM5JX1D+KeaLZcMzoPwKN1RVthd0/hl5qVhUf6D8cghr+xNnSPwYnSYkgP+U/mpZoANmn5z/tY+q0LnniPxKMiLTnodk/XaOkh4PA6T9W3buC3t3iPwJYsi7weuc/d8oxASyE5j81qx23QNHfP8X9911ec9g/tgQCHtF55D86QWmZhefrP+BwNYGPnc4/NwOA2uP94D9aitSJGQ7oPxYmAXCSAd8/ZKvu4gdZ4z82rDDMtL/bPy5jVAuxpdI/6FVu4ayj1z9/YgTatrrpPxUXe6oQJeM/0ig4e08C5z83PtJ9HY7mP15WY9MKYek/ywqpwQMg4z/agwwWcO3pP4Lvne9QX+k/z8qK1leZzT/h347t3uzkP0BJ0hixO+c/h4nnlIIuvD+wj7tE2K3jP0w9weogZNs/57UNp1920z+IIF1BqtPjP9TbSA/RVuY/JZTeM71k5D+ntdrxwWblPwzqpdM9fLo/EDsaHK3O2T8tvSqNB9fhP560bMXEh+E/yZAWVxpG5T/9KfA8XL7fP7pR2fWa/us/EqYe95Vruj+7ePuU+H3nPxCyu7oZ978/";
        const MULTI: &[u8] = b"AAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAAAAAAAAAAAAAPA/AAAAAAAA8D8AAAAAAAAAAAAAAAAAAPA/AAAAAAAAAAAAAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAAAAAAAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAAAAAAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAAAAAAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAAAAAAAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAAAAAAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAAAAAAAAAAAAAAAAAAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAAAAAAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAPA/AAAAAAAAAAAAAAAAAADwPwAAAAAAAPA/";
        const PARAM_K: &[u8] = b"2qbZ71+CF0ComgG6gfbVP7Q/WeAhY+I/FtpsUZTW8z8=";
        const PARAM_B: &[u8] = b"ra5HgjkQhz8et+IitFqHPy8JYNrO9u8/3n8aUaKChD8=";
        const PARAM_R: &[u8] = b"nEbw0j98hD9iwKbI6S6WP8xsXBtvwsM/OPLKPXR1mT8=";
        const PARAM_MULTI: &[u8] = b"orrL5ghhsD8o6BdPpba4P9czAbXu07o/6xfyq5SKhD8=";

        // 第2回パラメータ調整分
        const N2: &[u8] = b"ZmZmZmZm5j9mZmZmZmbmP5qZmZmZmdk/ZmZmZmZm5j+amZmZmZm5P5qZmZmZmek/AAAAAAAAAACamZmZmZm5PzMzMzMzM+M/MzMzMzMz0z+amZmZmZnpPwAAAAAAAAAAmpmZmZmZ2T/NzMzMzMzsP5qZmZmZmdk/mpmZmZmZyT+amZmZmZnJP5qZmZmZmdk/zczMzMzM7D8AAAAAAADgPwAAAAAAAOA/AAAAAAAA4D8zMzMzMzPjP2ZmZmZmZuY/AAAAAAAAAABmZmZmZmbmPwAAAAAAAOA/AAAAAAAAAACamZmZmZnpPwAAAAAAAAAAmpmZmZmZuT8AAAAAAADgP5qZmZmZmek/MzMzMzMz4z+amZmZmZnJPwAAAAAAAAAAZmZmZmZm5j8zMzMzMzPjPwAAAAAAAOA/MzMzMzMz4z8AAAAAAAAAAJqZmZmZmdk/AAAAAAAAAAAAAAAAAADgPwAAAAAAAAAAmpmZmZmZyT+amZmZmZm5P5qZmZmZmck/mpmZmZmZ2T8zMzMzMzPjP5qZmZmZmek/mpmZmZmZyT+amZmZmZnJP5qZmZmZmek/mpmZmZmZ6T8zMzMzMzPTPwAAAAAAAAAAmpmZmZmZuT+amZmZmZnJP5qZmZmZmbk/mpmZmZmZ6T8zMzMzMzPjPwAAAAAAAAAA";
        const M2: &[u8] = b"LiEJFI6Y4z8AAAAAAADoP807f2aeoOY/AAAAAAAA4D8uIQkUjpjjPzRv/ZNyiOo/zTt/Zp6g1j/NO39mnqDWP6pMWOh6tus/6vjSqX8q5T8uIQkUjpjjP6pMWOh6tts/LiEJFI6Y4z+qTFjoerbrPwAAAAAAAOA/LiEJFI6Y4z+qTFjoerbbP807f2aeoOY/NG/9k3KI6j8uIQkUjpjjPy4hCRSOmOM/qkxY6Hq22z/NO39mnqDmP+r40ql/KuU/zTt/Zp6g1j+o9Jebd+PhP1Nb2jpYTOk/zTt/Zp6g1j+o9Jebd+PhPwAAAAAAAOA/zTt/Zp6g1j+qTFjoerbbP1Nb2jpYTOk/NG/9k3KI6j/q+NKpfyrlP6j0l5t34+E/zTt/Zp6g5j9TW9o6WEzpP6j0l5t34+E/LiEJFI6Y4z+o9Jebd+PhPwAAAAAAAOA/qkxY6Hq22z80b/2TcojqP6pMWOh6tts/LiEJFI6Y4z+qTFjoerbbPwAAAAAAAOA/LiEJFI6Y4z+o9Jebd+PhP807f2aeoOY/qPSXm3fj4T+o9Jebd+PhP+r40ql/KuU/AAAAAAAA4D+o9Jebd+PhP807f2aeoNY/LiEJFI6Y4z8uIQkUjpjjP6j0l5t34+E/zTt/Zp6g1j8AAAAAAADoP807f2aeoNY/";
        const EPS2: &[u8] = b"Z2ZmZmZm1j+amZmZmZnpP5qZmZmZmak/MzMzMzMz4z/MzMzMzMzcPwAAAAAAAOg/zMzMzMzM3D9mZmZmZmbuP2dmZmZmZuY/AAAAAAAA0D8AAAAAAADoP8zMzMzMzOw/mpmZmZmZuT/NzMzMzMzkP2dmZmZmZuY/MzMzMzMz0z9nZmZmZmbmP5qZmZmZmbk/AAAAAAAA6D8zMzMzMzPDPzMzMzMzM8M/Z2ZmZmZm1j9nZmZmZmbWPzQzMzMzM+s/Z2ZmZmZm1j8AAAAAAADQP2ZmZmZmZu4/MzMzMzMz4z80MzMzMzPrPwAAAAAAAOg/MzMzMzMz4z9mZmZmZmbuPzMzMzMzM+M/zMzMzMzM7D8AAAAAAADQP83MzMzMzOQ/AAAAAAAA4D8AAAAAAADoPzMzMzMzM9M/mpmZmZmZ2T+amZmZmZnhPzMzMzMzM9M/MzMzMzMzwz80MzMzMzPrPwAAAAAAAOg/Z2ZmZmZm1j/MzMzMzMzsPwAAAAAAAOA/mpmZmZmZ6T/NzMzMzMzkPzMzMzMzM9M/ZmZmZmZm7j8AAAAAAADQPzMzMzMzM9M/Z2ZmZmZm1j9nZmZmZmbWP5qZmZmZmek/zMzMzMzM3D8zMzMzMzPTP2ZmZmZmZu4/AAAAAAAA8D9mZmZmZmbuP2dmZmZmZuY/";
        const AVG2: &[u8] = b"AAAAAAAA2D8AAAAAAADgPwAAAAAAANA/w8FoBWRF5j8AAAAAAADYPy4hCRSOmNM/p1ZUWfzC4j+o9Jebd+PhP1Nb2jpYTNk/AAAAAAAA0D8WBHd17i7jP0k/aBHq7t0/AAAAAAAA4D80b/2TcojaPy4hCRSOmOM/U1vaOlhM2T9JP2gR6u7dP6pMWOh6tts/6vjSqX8q1T+qTFjoerbbP9ls38x2+OA/2k5Psd773j/zWWFEK9jcP/NZYUQr2Nw/ST9oEeru3T/2LxJfZWXkPwAAAAAAANA/AAAAAAAA5D8AAAAAAADkPy4hCRSOmNM/AAAAAAAA5D+AOUIu3MjkPwAAAAAAANg/qPSXm3fj0T8AAAAAAADYPwAAAAAAANg/B+2vZg9+4D8AAAAAAADYPzRv/ZNyiNo/2k5Psd773j8uIQkUjpjTP6j0l5t34+E/AAAAAAAA2D/NO39mnqDWPwAAAAAAAOA/AAAAAAAA2D/aTk+x3vveP+r40ql/KtU/2k5Psd773j8AAAAAAADgP/NZYUQr2Nw/NG/9k3KI2j9JP2gR6u7dPzRv/ZNyiNo/B+2vZg9+4D80b/2TcojaPwftr2YPfuA/U1vaOlhM2T/q+NKpfyrVP+r40ql/KtU/Ek+N6/t+7z9TW9o6WEzZPwAAAAAAAOQ/";
        const TABOO: &[u8] = b"+1gXvrDi9T/Gmio5OFj3v5Kikez7F+m/0rVLsFFf6r+tszH50rIKwEuTCl+6wQXARkDCIM9C7L8pgpsp2sD2v7vwSvcIreE/AxALcBX+0L/SEFrCj9v0v0zcdpXzd/S/AAAAAAAAAAAu/Lyyvo3Tv74y5JAt7Oc/AAAAAAAAAAAkYz5YqnP7Pw8UeQ9nUwTAdFJWnCYVEMAR25sKRt/hv9gTQBdhvghAGQvJ2EqJCUCr1wDD57oDwJHDLiBOuQJA5v1pnG6T8D8RD+rrLBoPQHUBhmyfk/K/UYYwb+lz9L8yGZgz5Db6P4ntE21Whuy/0x9DzC068D884hW2ZPvoP/67kvEO9RbANyiV1VyT1T+kcKIoi1YXwFI2al86mADAnwi3TDWF5j+C3CjckQsKQNV4di9Ot/E//DXvW/YO+b8OtpzV/f7xv7zsfwHRFuo/hlEepHtZ/T8V5DiuTMrwv5mv56tmb/2/NV8AtJt38z9fV6AQacL0PxheKZOiRhBAfaJBOxTuFsBew0nLpysKQKJLQbjgTrA/IUFfcI1xBMB0lVkw+6DpvxdulTKP1RlAo0A25H3X6b8Zvj6n+n//P1F/o8rSBQvANrRg2Kyz2L8gbHDiopUAwGlyu9/ovAbAzg4/jWXnlT8LC8HymK7zPyjp6geEi+6/";
        const ENTROPY_LEN: &[u8] = b"/O5jaTPVKkC0uD1pQegqQFqdKgCqWiVAL6TjXG27LUAjuXGCEsYzQB5f50zdVC5AzsIVU8KwKUCIgHbVXJckQNpOT7He+x5AU1vaOlhMKUBbefdG90MuQOARPG3OaCdAwAofAMZIHEBBh2LGMt4iQIUeFbmOVCJAwAofAMZIHEBM+chUwNouQPYvEl9lZTRAQLAaxKm/J0CnVlRZ/MIiQDRv/ZNyiBpAxUJsP1eHLkCAOUIu3MgUQDf5z9pv4SRAp1ZUWfzCIkDQumE/4B8gQMbCdq/ucTRAI9BwHBshG0DkEORzVw8pQAAAAAAAACxAxrENHtVkHUBAVmNFSi0vQCkD7C9DEixAqPSXm3fjEUBiKcDzoz82QKj0l5t34zFA+U1kNINvMUCqxLqmTds0QBwkAtahvzVAzsIVU8KwKUBwxJ05jXYuQMbCdq/ucTRAWCgvhHmuMkAE8W2P3KkxQFZU22QuUDNAV7LbGpqlM0ADq/e2g8U1QGJWY32dsjNA7S4oiiSRNEDGwnav7nE0QB5f50zdVC5ASerUGpJGMkBm7kCfAx4xQNqpUl2+uTVAwlzxgehCM0AI9MvQoMwtQHcvDo0vcyZAp1ZUWfzCMkAG+5VNMNowQGChHSrROipALgAy9EH7KkAtwpPREKoXQPqB/oV6JCxA";
        const PARAM_TABOO: &[u8] = b"1CzxLdPSkz/dEoBpRzn2P6bJtg6xvZU/j/BK4GuXEEA=";
        const PARAM_ENTROPY_LEN: &[u8] = b"MYOjtt/qhz+qdYAoiFCaP/0Ucx+5iKQ/dURErWFFN0A=";

        // 第3回パラメータ調整分
        const N3: &[u8] = b"AAAAAAAA8D9mZmZmZmbmPwAAAAAAAOA/MzMzMzMz0z9mZmZmZmbmPwAAAAAAAOA/ZmZmZmZm5j8AAAAAAADgPzMzMzMzM9M/mpmZmZmZuT9mZmZmZmbmPwAAAAAAAAAAzczMzMzM7D8AAAAAAAAAAJqZmZmZmek/mpmZmZmZ6T8AAAAAAADwP2ZmZmZmZuY/AAAAAAAAAAAzMzMzMzPjPwAAAAAAAAAAAAAAAAAAAACamZmZmZnJPzMzMzMzM+M/mpmZmZmZuT/NzMzMzMzsPzMzMzMzM9M/ZmZmZmZm5j/NzMzMzMzsPwAAAAAAAOA/zczMzMzM7D8AAAAAAADgP5qZmZmZmck/mpmZmZmZ2T9mZmZmZmbmPwAAAAAAAAAA";
        const M3: &[u8] = b"+U1kNINv8T80b/2TcojqP6pMWOh6tts/AAAAAAAA4D8uIQkUjpjjP6pMWOh6tts/U1vaOlhM6T80b/2TcojqP6pMWOh6tts/qkxY6Hq22z9TW9o6WEzpP6j0l5t34+E/AAAAAAAA6D+o9Jebd+PhP6pMWOh6tus/qkxY6Hq26z/NO39mnqDmP807f2aeoNY/AAAAAAAA4D+qTFjoerbbP6j0l5t34+E/zTt/Zp6g1j+qTFjoerbbP6j0l5t34+E/qPSXm3fj4T+o9Jebd+PhPwAAAAAAAOA/ST9oEeru7T8AAAAAAADoP1Nb2jpYTOk/U1vaOlhM6T8AAAAAAADgP6j0l5t34+E/LiEJFI6Y4z/NO39mnqDmP807f2aeoNY/";
        const EPS3: &[u8] = b"Z2ZmZmZm1j8AAAAAAADQP5qZmZmZmdk/AAAAAAAA8D/NzMzMzMzkP2ZmZmZmZu4/mpmZmZmZ6T+amZmZmZmpP83MzMzMzOQ/zMzMzMzM3D8zMzMzMzPTPzQzMzMzM+s/MzMzMzMz0z/MzMzMzMzcP2dmZmZmZuY/AAAAAAAA4D8zMzMzMzPTPwAAAAAAAOg/mpmZmZmZyT8zMzMzMzPDPwAAAAAAANA/mpmZmZmZuT/NzMzMzMzkP83MzMzMzOQ/Z2ZmZmZm5j+amZmZmZnZPzMzMzMzM8M/ZmZmZmZm7j/NzMzMzMzkP2ZmZmZmZu4/mpmZmZmZ4T+amZmZmZnhP5qZmZmZmbk/mpmZmZmZ2T8AAAAAAADoPwAAAAAAAOg/";
        const AVG3: &[u8] = b"AAAAAAAA2D8uIQkUjpjTP4UeFbmOVOI/qPSXm3fj4T8uIQkUjpjjP9ls38x2+OA/LiEJFI6Y0z8AAAAAAADYPy4hCRSOmOM/qPSXm3fj4T80b/2TcojaP+r40ql/KtU/ST9oEeru3T8uIQkUjpjTP6j0l5t349E/81lhRCvY3D8WBHd17i7jP/PZqKRoiuU/zTt/Zp6g1j/ZbN/MdvjgP807f2aeoNY/U1vaOlhM2T+o9Jebd+PhPxYEd3XuLuM/zTt/Zp6g1j/w0GKhbvrmP6j0l5t34+E/6vjSqX8q1T/aTk+x3vveP6j0l5t349E/NG/9k3KI2j+AOUIu3MjkPwAAAAAAANg/2k5Psd773j80b/2TcojaPxYEd3XuLuM/";
        const ANSWER: &[u8] = b"0A7v+mlTEUBgZPJEg5gUQLKOXurmCCJA2n6Ot7X9IkDM5ksEphIXQKljq4rR3yNAtu9IPqtnIkCUxN9HPV0gQOG/oO0qQxZAxW6tSeh7I0B2OuWefYcaQKfHWHdbTyBAwAofAMZIHEDZVwXWAkIgQC/4DldBMSFAwAofAMZIHEBW28ChgiQWQDc0M+RwSiNAwAofAMZIHEBqmDwOooQWQFN8qkhEkRJAdhVeKeoHEkDSCVP9VjQSQDWjoUbO4SJAbHED2G/qGkAApHNOBZUaQG67el+NrhRAwAofAMZIHEAlR9L1f6IjQMAKHwDGSBxA2MvgES3lGEDtoriVDG4UQLbSK3nTshhAHiqGkdmRFUC02oHK4fMRQPCJn6wuohJA";
        const PARAM_ANSWER: &[u8] = b"HmftTm/ChD+DG+dDUHyJP6P7AS4vQeA/pMA2Vho2CkA=";

        pub struct ParamSuggester {
            x_matrix: DMatrix<f64>,
            y_vector: DVector<f64>,
            hyper_param: DVector<f64>,
            y_inv_trans: fn(f64) -> f64,
            lower: f64,
            upper: f64,
        }

        impl ParamSuggester {
            fn new(
                hyper_param: DVector<f64>,
                x_matrix: DMatrix<f64>,
                y_vector: DVector<f64>,
                y_inv_trans: fn(f64) -> f64,
                lower: f64,
                upper: f64,
            ) -> Self {
                Self {
                    hyper_param,
                    x_matrix,
                    y_vector,
                    y_inv_trans,
                    lower,
                    upper,
                }
            }

            fn gen_x_matrix_1() -> DMatrix<f64> {
                let n = DVector::from_vec(decode_base64(N1)).transpose();
                let m = DVector::from_vec(decode_base64(M1)).transpose();
                let eps = DVector::from_vec(decode_base64(EPS1)).transpose();
                let avg = DVector::from_vec(decode_base64(AVG1)).transpose();

                let x_matrix = DMatrix::from_rows(&[n, m, eps, avg]);

                x_matrix
            }

            fn gen_x_matrix_2() -> DMatrix<f64> {
                let n = DVector::from_vec(decode_base64(N2)).transpose();
                let m = DVector::from_vec(decode_base64(M2)).transpose();
                let eps = DVector::from_vec(decode_base64(EPS2)).transpose();
                let avg = DVector::from_vec(decode_base64(AVG2)).transpose();

                let x_matrix = DMatrix::from_rows(&[n, m, eps, avg]);

                x_matrix
            }

            fn gen_x_matrix_3() -> DMatrix<f64> {
                let n = DVector::from_vec(decode_base64(N3)).transpose();
                let m = DVector::from_vec(decode_base64(M3)).transpose();
                let eps = DVector::from_vec(decode_base64(EPS3)).transpose();
                let avg = DVector::from_vec(decode_base64(AVG3)).transpose();

                let x_matrix = DMatrix::from_rows(&[n, m, eps, avg]);

                x_matrix
            }

            pub fn gen_k_pred() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_K));
                let y_vector = DVector::from_vec(decode_base64(K));
                Self::new(
                    hyper_param,
                    Self::gen_x_matrix_1(),
                    y_vector,
                    |x| x * x,
                    0.0,
                    50.0,
                )
            }

            pub fn gen_b_pred() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_B));
                let y_vector = DVector::from_vec(decode_base64(B));
                Self::new(
                    hyper_param,
                    Self::gen_x_matrix_1(),
                    y_vector,
                    |x| x,
                    0.0,
                    1.0,
                )
            }

            pub fn gen_r_pred() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_R));
                let y_vector = DVector::from_vec(decode_base64(R));
                Self::new(
                    hyper_param,
                    Self::gen_x_matrix_1(),
                    y_vector,
                    |x| x,
                    0.1,
                    0.9,
                )
            }

            pub fn gen_multi_pred() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_MULTI));
                let y_vector = DVector::from_vec(decode_base64(MULTI));
                Self::new(
                    hyper_param,
                    Self::gen_x_matrix_1(),
                    y_vector,
                    |x| x,
                    0.0,
                    1.0,
                )
            }

            pub fn gen_taboo_pred() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_TABOO));
                let y_vector = DVector::from_vec(decode_base64(TABOO));
                Self::new(
                    hyper_param,
                    Self::gen_x_matrix_2(),
                    y_vector,
                    |x| x.exp() / (1.0 + x.exp()),
                    0.0,
                    1.0,
                )
            }

            pub fn gen_entropy_len_pred() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_ENTROPY_LEN));
                let y_vector = DVector::from_vec(decode_base64(ENTROPY_LEN));
                Self::new(
                    hyper_param,
                    Self::gen_x_matrix_2(),
                    y_vector,
                    |x| x * x,
                    20.0,
                    500.0,
                )
            }

            pub fn gen_answer_threshold_pred() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_ANSWER));
                let y_vector = DVector::from_vec(decode_base64(ANSWER));
                Self::new(
                    hyper_param,
                    Self::gen_x_matrix_3(),
                    y_vector,
                    |x| x * x,
                    20.0,
                    100.0,
                )
            }

            pub fn suggest(&self, n: usize, m: usize, eps: f64, avg: f64) -> f64 {
                let n = (n - 10) as f64 / 10.0;
                let m = (m as f64).sqrt() / 4.0;
                let eps = eps * 5.0;
                let avg = avg.sqrt() / 8.0;

                let len = self.x_matrix.shape().1;
                let y_mean = self.y_vector.mean();
                let y_mean = DVector::from_element(self.y_vector.len(), y_mean);
                let new_x = DMatrix::from_vec(4, 1, vec![n, m, eps, avg]);
                let noise = DMatrix::from_diagonal_element(len, len, self.hyper_param[3]);

                let k = self.calc_kernel_matrix(&self.x_matrix, &self.x_matrix) + noise;
                let kk = self.calc_kernel_matrix(&self.x_matrix, &new_x);

                let kernel_lu = k.lu();
                let new_y = kk.transpose() * kernel_lu.solve(&(&self.y_vector - &y_mean)).unwrap();

                (self.y_inv_trans)(new_y[(0, 0)] + y_mean[(0, 0)]).clamp(self.lower, self.upper)
            }

            fn calc_kernel_matrix(&self, x1: &DMatrix<f64>, x2: &DMatrix<f64>) -> DMatrix<f64> {
                let n = x1.shape().1;
                let m = x2.shape().1;
                let mut kernel = DMatrix::zeros(n, m);

                for i in 0..n {
                    for j in 0..m {
                        kernel[(i, j)] = self.gaussian_kernel(&x1.column(i), &x2.column(j));
                    }
                }

                kernel
            }

            fn gaussian_kernel(&self, x1: &DVectorView<f64>, x2: &DVectorView<f64>) -> f64 {
                let t1 = self.hyper_param[0];
                let t2 = self.hyper_param[1];
                let t3 = self.hyper_param[2];

                let diff = x1 - x2;
                let norm_diff = diff.dot(&diff);
                let dot = x1.dot(&x2);
                t1 * dot + t2 * (-norm_diff / t3).exp()
            }
        }

        fn decode_base64(data: &[u8]) -> Vec<f64> {
            const BASE64_MAP: &[u8] =
                b"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
            let mut stream = vec![];

            let mut cursor = 0;

            while cursor + 4 <= data.len() {
                let mut buffer = 0u32;

                for i in 0..4 {
                    let c = data[cursor + i];
                    let shift = 6 * (3 - i);

                    for (i, &d) in BASE64_MAP.iter().enumerate() {
                        if c == d {
                            buffer |= (i as u32) << shift;
                        }
                    }
                }

                for i in 0..3 {
                    let shift = 8 * (2 - i);
                    let value = (buffer >> shift) as u8;
                    stream.push(value);
                }

                cursor += 4;
            }

            let mut result = vec![];
            cursor = 0;

            while cursor + 8 <= stream.len() {
                let p = stream.as_ptr() as *const f64;
                let x = unsafe { *p.offset(cursor as isize / 8) };
                result.push(x);
                cursor += 8;
            }

            result
        }
    }

    use crate::{
        common::DurationCorrector,
        grid::{Coord, CoordDiff, Map2d},
    };
    use bitset_fixed::BitSet;
    use im_rc::HashMap;
    use itertools::Itertools;
    use proconio::{input, source::line::LineSource};
    use rand::Rng as _;
    use rand_pcg::Pcg64Mcg;
    use rustc_hash::FxHashSet;
    use std::{
        cmp::Reverse,
        fmt::Display,
        io::{self, BufReader, BufWriter, StdoutLock, Write},
        time::{Duration, Instant},
    };

    use self::params::ParamSuggester;

    #[derive(Debug, Clone)]
    pub struct Input {
        pub map_size: usize,
        pub oil_count: usize,
        pub eps: f64,
        pub oils: Vec<Oils>,
        pub avg_oil_size: f64,
        pub total_oil_tiles: usize,
        pub hashes: Vec<Map2d<u64>>,
        pub duration_corrector: DurationCorrector,
        pub time_conductor: TimeConductor,
        pub since: Instant,
        pub params: Params,
    }

    impl Input {
        pub const TIME_LIMIT: Duration = Duration::new(2, 970_000_000);

        fn new(map_size: usize, oil_count: usize, eps: f64, oils: Vec<Oils>) -> Self {
            let mut counts = HashMap::new();

            for oil in &oils {
                let entry = counts.entry(oil.pos.clone()).or_insert(0);
                *entry += 1;
            }

            let mut rng = Pcg64Mcg::new(42);
            let mut hashes = vec![];

            for _ in 0..oil_count {
                let mut map = Map2d::new_with(0, map_size);

                for row in 0..map_size {
                    for col in 0..map_size {
                        map[Coord::new(row, col)] = rng.gen();
                    }
                }

                hashes.push(map);
            }

            let total_oil_tiles = oils.iter().map(|o| o.len).sum();
            let avg_oil_size = total_oil_tiles as f64 / oil_count as f64;
            let duration_corrector = DurationCorrector::from_env();
            let time_conductor = TimeConductor::new(map_size, oil_count, eps, avg_oil_size);
            let params = Params::new(map_size, oil_count, eps, avg_oil_size);
            let since = Instant::now();

            Self {
                map_size,
                oil_count,
                eps,
                oils,
                total_oil_tiles,
                avg_oil_size,
                hashes,
                duration_corrector,
                time_conductor,
                since,
                params,
            }
        }
    }

    #[derive(Debug, Clone)]
    pub struct Oils {
        pub pos: Vec<Coord>,
        pub width: usize,
        pub height: usize,
        pub len: usize,
        pub bitset: BitSet,
    }

    impl Oils {
        fn new(mut pos: Vec<Coord>, map_size: usize) -> Self {
            pos.sort();
            let height = pos.iter().map(|c| c.row).max().unwrap() + 1;
            let width = pos.iter().map(|c| c.col).max().unwrap() + 1;
            let len = pos.len();

            let mut bitset = BitSet::new(map_size * map_size);

            for p in &pos {
                bitset.set(p.row * map_size + p.col, true);
            }

            Self {
                pos,
                width,
                height,
                len,
                bitset,
            }
        }

        pub fn get_shifted_bitset(&self, input: &Input, shift: CoordDiff) -> BitSet {
            let result =
                self.bitset.clone() << (shift.dr as usize * input.map_size + shift.dc as usize);
            result
        }
    }

    impl Display for Oils {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            let mut map = vec![vec![false; self.width]; self.height];

            for p in &self.pos {
                map[p.row][p.col] = true;
            }

            for row in 0..self.height {
                for col in 0..self.width {
                    write!(f, "{}", if map[row][col] { '#' } else { '.' })?;
                }

                writeln!(f)?;
            }

            Ok(())
        }
    }

    #[derive(Debug, Clone)]
    pub struct Params {
        pub use_multi_dig_solver: f64,
        pub taboo_prob: f64,
        pub max_entropy_len: usize,
        pub answer_threshold_ratio: f64,
    }

    impl Params {
        pub fn new(map_size: usize, oil_count: usize, eps: f64, avg: f64) -> Self {
            /*
            let use_multi_dig_solver = match std::env::args().nth(1) {
                Some(s) => {
                    if s == "1" {
                        1.0
                    } else {
                        0.0
                    }
                }
                None => ParamSuggester::gen_multi_pred().suggest(map_size, oil_count, eps, avg),
            };
            */

            let use_multi_dig_solver =
                ParamSuggester::gen_multi_pred().suggest(map_size, oil_count, eps, avg);

            let taboo_prob =
                ParamSuggester::gen_taboo_pred().suggest(map_size, oil_count, eps, avg);
            let max_entropy_len = ParamSuggester::gen_entropy_len_pred()
                .suggest(map_size, oil_count, eps, avg)
                .round() as usize;
            let answer_threshold_ratio = std::env::args()
                .nth(1)
                .and_then(|s| s.parse().ok())
                .unwrap_or_else(|| {
                    ParamSuggester::gen_answer_threshold_pred()
                        .suggest(map_size, oil_count, eps, avg)
                });

            Self {
                use_multi_dig_solver,
                taboo_prob,
                max_entropy_len,
                answer_threshold_ratio,
            }
        }

        pub fn use_multi_dig_solver(&self) -> bool {
            self.use_multi_dig_solver >= 0.5
        }
    }

    #[derive(Debug, Clone, Copy)]
    pub struct TimeConductor {
        k: f64,
        b: f64,
        ratio: f64,
    }

    impl TimeConductor {
        fn new(map_size: usize, oil_count: usize, eps: f64, avg: f64) -> Self {
            // ターンごとの実行時間は (1-x)^k + bx とする
            /*
            let k = std::env::args()
                .nth(2)
                .and_then(|s| s.parse().ok())
                .unwrap_or_else(|| ParamSuggester::gen_k_pred().suggest(map_size, oil_count, eps, avg));

            let b = std::env::args()
                .nth(3)
                .and_then(|s| s.parse().ok())
                .unwrap_or_else(|| ParamSuggester::gen_b_pred().suggest(map_size, oil_count, eps, avg));

            let ratio = std::env::args()
                .nth(4)
                .and_then(|s| s.parse().ok())
                .unwrap_or_else(|| ParamSuggester::gen_r_pred().suggest(map_size, oil_count, eps, avg));
            */

            let k = ParamSuggester::gen_k_pred().suggest(map_size, oil_count, eps, avg);
            let b = ParamSuggester::gen_b_pred().suggest(map_size, oil_count, eps, avg);
            let ratio = ParamSuggester::gen_r_pred().suggest(map_size, oil_count, eps, avg);

            Self { k, b, ratio }
        }

        pub fn get_time_table(
            &self,
            elapsed: Duration,
            time_limit: Duration,
            max_turn: usize,
        ) -> Vec<Duration> {
            let mut times_f64 = vec![];

            for t in 0..max_turn {
                let x = t as f64 / max_turn as f64;

                let t = (1.0 - x).powf(self.k) + self.b * x;
                times_f64.push(t);
            }

            for i in 1..max_turn {
                times_f64[i] += times_f64[i - 1];
            }

            let total_time = times_f64.last().unwrap();

            let mut durations = vec![];

            let remaining = time_limit.saturating_sub(elapsed);

            for &t in times_f64.iter() {
                let t = elapsed + remaining.mul_f64(t / total_time);
                durations.push(t);
            }

            durations
        }

        pub fn k(&self) -> f64 {
            self.k
        }

        pub fn b(&self) -> f64 {
            self.b
        }

        pub fn phase_ratio(&self) -> f64 {
            self.ratio
        }
    }

    pub struct Judge<'a> {
        source: LineSource<BufReader<io::Stdin>>,
        writer: BufWriter<StdoutLock<'a>>,
        wa: FxHashSet<Vec<Coord>>,
        comment_buf: Vec<String>,
        color_buf: Vec<Map2d<f64>>,
        query_limit: usize,
        query_count: usize,
        show_comment: bool,
    }

    #[allow(dead_code)]
    impl<'a> Judge<'a> {
        pub fn new() -> Self {
            let source = LineSource::new(BufReader::new(io::stdin()));
            let writer = BufWriter::new(io::stdout().lock());
            let show_comment = std::env::var("AHC030_SHOW_COMMENT").is_ok_and(|s| s == "1");

            Self {
                source,
                writer,
                wa: FxHashSet::default(),
                comment_buf: vec![],
                color_buf: vec![],
                query_limit: 0,
                query_count: 0,
                show_comment,
            }
        }

        pub fn read_input(&mut self) -> Input {
            input! {
                from &mut self.source,
                n: usize,
                m: usize,
                eps: f64,
            }

            self.query_limit = n * n * 2;

            let mut oils = vec![];

            for _ in 0..m {
                input! {
                    from &mut self.source,
                    d: usize,
                }

                let mut pos = vec![];

                for _ in 0..d {
                    input! {
                        from &mut self.source,
                        row: usize,
                        col: usize,
                    }

                    pos.push(Coord::new(row, col));
                }

                oils.push(Oils::new(pos, n));
            }

            oils.sort_by_key(|o| Reverse(o.len));

            Input::new(n, m, eps, oils)
        }

        pub fn query_count(&self) -> usize {
            self.query_count
        }

        pub fn max_query_count(&self) -> usize {
            self.query_limit
        }

        pub fn can_query(&self) -> bool {
            self.query_count < self.query_limit
        }

        pub fn remaining_query_count(&self) -> usize {
            self.query_limit - self.query_count
        }

        pub fn query_single(&mut self, coord: Coord) -> i32 {
            self.query_multiple(&[coord])
        }

        pub fn query_multiple(&mut self, coords: &[Coord]) -> i32 {
            assert!(coords.len() > 0);

            if self.query_count >= self.query_limit {
                // do nothing
                return 0;
            }

            self.query_count += 1;
            self.flush_comments();
            let len = coords.len();
            let coords = coords
                .iter()
                .map(|c| format!("{} {}", c.row, c.col))
                .join(" ");

            writeln!(self.writer, "q {} {}", len, coords).unwrap();
            self.writer.flush().unwrap();

            input! {
                from &mut self.source,
                value: i32
            }

            value
        }

        pub fn answer(&mut self, coords: &[Coord]) -> Result<(), ()> {
            if self.query_count >= self.query_limit {
                // do nothing
                return Ok(());
            }

            let len = coords.len();
            let coords_vec = coords.iter().copied().collect_vec();

            if self.wa.contains(&coords_vec) {
                return Err(());
            }

            self.query_count += 1;
            self.flush_comments();

            let coords_str = coords
                .iter()
                .map(|c| format!("{} {}", c.row, c.col))
                .join(" ");

            writeln!(self.writer, "a {} {}", len, coords_str).unwrap();
            self.writer.flush().unwrap();

            input! {
                from &mut self.source,
                value: i32
            }

            if value == 1 {
                Ok(())
            } else {
                self.wa.insert(coords_vec);
                Err(())
            }
        }

        pub fn comment(&mut self, message: &str) {
            self.comment_buf.push(message.to_string());
        }

        pub fn comment_colors(&mut self, colors: &Map2d<f64>) {
            self.color_buf.push(colors.clone());
        }

        fn flush_comments(&mut self) {
            if !self.show_comment {
                return;
            }

            for comment in &self.comment_buf {
                writeln!(self.writer, "# {}", comment).unwrap();
            }

            self.comment_buf.clear();

            for colors in self.color_buf.iter() {
                for row in 0..colors.size {
                    for col in 0..colors.size {
                        let c = Coord::new(row, col);
                        let v = ((1.0 - colors[c].clamp(0.0, 1.0)) * 255.0).round() as u8;
                        writeln!(self.writer, "#c {row} {col} #{:02x}{:02x}{:02x}", v, v, 255)
                            .unwrap();
                    }
                }
            }

            self.writer.flush().unwrap();
            self.color_buf.clear();
        }
    }
}
mod solver {
    pub mod multi_dig {
        mod generator {
            use crate::{
                common::ChangeMinMax as _,
                grid::{Coord, CoordDiff, Map2d},
                problem::Input,
            };
            use itertools::{izip, Itertools as _};
            use ordered_float::OrderedFloat;
            use rand::{
                seq::{IteratorRandom as _, SliceRandom as _},
                Rng,
            };
            use rand_distr::{Distribution as _, WeightedAliasIndex, WeightedIndex};
            use rustc_hash::{FxHashMap, FxHashSet};
            use std::hash::Hash;

            use super::Env;

            #[derive(Debug, Clone)]
            pub(super) struct State {
                pub shift: Vec<CoordDiff>,
                pub log_likelihood: f64,
                counts: Vec<usize>,
                counts_u32: Vec<u32>,
                pub hash: u64,
            }

            impl State {
                pub(super) fn new(shift: Vec<CoordDiff>, env: &Env) -> Self {
                    let mut log_likelihood = 0.0;
                    let counts = vec![0; env.obs.observations.len()];
                    let counts_i32 = vec![0; env.obs.observations.len()];

                    for (obs, &count) in env.obs.observations.iter().zip(counts.iter()) {
                        log_likelihood += obs.log_likelihoods[count];
                    }

                    let mut hash = 0;

                    for (i, shift) in shift.iter().enumerate() {
                        hash ^= env.input.hashes[i][Coord::try_from(*shift).unwrap()];
                    }

                    let mut state = Self {
                        shift,
                        log_likelihood,
                        counts,
                        counts_u32: counts_i32,
                        hash,
                    };

                    let shift = state.shift.clone();

                    for (i, &shift) in shift.iter().enumerate() {
                        state.add_oil(env, i, shift);
                    }

                    state
                }

                fn add_oil(&mut self, env: &Env, oil_i: usize, shift: CoordDiff) {
                    self.shift[oil_i] = shift;
                    let cnt =
                        &env.obs.relative_observation_cnt[oil_i][Coord::try_from(shift).unwrap()];
                    self.hash ^= env.input.hashes[oil_i][Coord::try_from(shift).unwrap()];

                    for (obs_i, &cnt) in cnt.iter().enumerate() {
                        let observation = &env.obs.observations[obs_i];
                        let count = &mut self.counts[obs_i];
                        self.log_likelihood -= observation.log_likelihoods[*count];
                        *count += cnt;
                        self.log_likelihood += observation.log_likelihoods[*count];
                        self.counts_u32[obs_i] = *count as u32;
                    }
                }

                fn remove_oil(&mut self, env: &Env, oil_i: usize) {
                    let shift = self.shift[oil_i];
                    let cnt =
                        &env.obs.relative_observation_cnt[oil_i][Coord::try_from(shift).unwrap()];
                    self.hash ^= env.input.hashes[oil_i][Coord::try_from(shift).unwrap()];

                    for (obs_i, &cnt) in cnt.iter().enumerate() {
                        let observation = &env.obs.observations[obs_i];
                        let count = &mut self.counts[obs_i];
                        self.log_likelihood -= observation.log_likelihoods[*count];
                        *count -= cnt;
                        self.log_likelihood += observation.log_likelihoods[*count];
                        self.counts_u32[obs_i] = *count as u32;
                    }
                }

                #[target_feature(enable = "avx,avx2")]
                unsafe fn add_oil_whatif(&self, env: &Env, oil_i: usize, shift: CoordDiff) -> f64 {
                    use std::arch::x86_64::*;

                    // AVX2を使って高速化
                    let log_likelihoods = &env.obs.obs_log_likelihoods;
                    let mut offsets: &[u32] = &env.obs.obs_log_likelihoods_offsets;
                    let mut cnts: &[u32] = &self.counts_u32;

                    let mut cnts_added: &[u32] = &env.obs.relative_observation_cnt_u32[oil_i]
                        [Coord::try_from(shift).unwrap()];
                    let mut log_likelihood = _mm256_broadcast_ss(&0.0);

                    const ELEMENTS_PER_256BIT: usize = 8;

                    while offsets.len() >= ELEMENTS_PER_256BIT {
                        // j番目の観測が保存されている箇所の先頭のindex
                        let offset = _mm256_loadu_si256(offsets.as_ptr() as *const __m256i);

                        // j番目の観測について、現在の配置だと仮定したときの真の埋蔵量
                        let cnt = _mm256_loadu_si256(cnts.as_ptr() as *const __m256i);

                        // oil_iを追加したときの真の埋蔵量の増加量
                        let cnt_added = _mm256_loadu_si256(cnts_added.as_ptr() as *const __m256i);

                        // offset, cnt, cnt_addedを足す
                        let offset = _mm256_add_epi32(offset, cnt);
                        let offset = _mm256_add_epi32(offset, cnt_added);

                        // log_likelihoodsからoffsetの位置にある値を取得
                        let likelihoods =
                            _mm256_i32gather_ps::<4>(log_likelihoods.as_ptr(), offset);

                        // 取得した値をlog_likelihoodに足す
                        log_likelihood = _mm256_add_ps(log_likelihood, likelihoods);

                        // ポインタを進める（float型は256bitレジスタに8個入る）
                        offsets = offsets.get_unchecked(ELEMENTS_PER_256BIT..);
                        cnts = cnts.get_unchecked(ELEMENTS_PER_256BIT..);
                        cnts_added = cnts_added.get_unchecked(ELEMENTS_PER_256BIT..);
                    }

                    // TODO: 水平加算を理解する
                    let mut buffer = [0.0; ELEMENTS_PER_256BIT];
                    _mm256_storeu_ps(buffer.as_mut_ptr(), log_likelihood);
                    let mut log_likelihood = 0.0;

                    for &v in buffer.iter() {
                        log_likelihood += v;
                    }

                    for (&offset, &cnt, &cnt_added) in izip!(offsets, cnts, cnts_added) {
                        let index = offset + cnt + cnt_added;
                        log_likelihood += log_likelihoods.get_unchecked(index as usize);
                    }

                    log_likelihood as f64
                }

                pub(super) fn add_last_observation(&mut self, env: &Env) {
                    assert!(self.counts.len() + 1 == env.obs.observations.len());

                    let mut count = 0;
                    let overlaps = env.obs.overlaps.last().unwrap();
                    let observation = env.obs.observations.last().unwrap();

                    for (&shift, overlap) in self.shift.iter().zip(overlaps.iter()) {
                        count += overlap[Coord::try_from(shift).unwrap()];
                    }

                    self.log_likelihood += observation.log_likelihoods[count];

                    self.counts.push(count);
                    self.counts_u32.push(count as u32);
                }

                fn neigh(mut self, env: &Env, rng: &mut impl Rng, choose_cnt: usize) -> Self {
                    let mut oil_indices = (0..env.input.oil_count).choose_multiple(rng, choose_cnt);
                    oil_indices.shuffle(rng);

                    // 同じ場所への配置を許可しない
                    let last_shifts = self.shift.clone();
                    let mut taboos = vec![false; env.input.oil_count];

                    if rng.gen_bool(env.input.params.taboo_prob) {
                        let taboo = oil_indices.choose(rng).copied().unwrap();
                        if env.obs.shift_candidates[taboo].len() > 1 {
                            taboos[taboo] = true;
                        }
                    }

                    for &oil_i in oil_indices.iter() {
                        self.remove_oil(env, oil_i);

                        // 消したままだと貪欲の判断基準がおかしくなるので、ランダムな適当な場所に置いておく
                        let shift = env.obs.shift_candidates[oil_i]
                            .choose(rng)
                            .copied()
                            .unwrap();
                        self.add_oil(env, oil_i, shift);
                    }

                    // ランダム性を入れた貪欲で場所を決めていく
                    for &oil_i in oil_indices.iter() {
                        let height = env.input.map_size - env.input.oils[oil_i].height + 1;
                        let width = env.input.map_size - env.input.oils[oil_i].width + 1;
                        let candidate_count = height * width;
                        let mut shifts = Vec::with_capacity(candidate_count);
                        let mut log_likelihoods = Vec::with_capacity(candidate_count);
                        let mut max_log_likelihood = f64::NEG_INFINITY;

                        // 消し直す
                        self.remove_oil(env, oil_i);

                        for &shift in env.obs.shift_candidates[oil_i].iter() {
                            // 同じ場所への配置を許可しない
                            if taboos[oil_i] && shift == last_shifts[oil_i] {
                                continue;
                            }

                            let log_likelihood = unsafe { self.add_oil_whatif(env, oil_i, shift) };
                            shifts.push(shift);
                            log_likelihoods.push(log_likelihood);
                            max_log_likelihood.change_max(log_likelihood);
                        }

                        let likelihoods = log_likelihoods
                            .iter()
                            .map(|&log_likelihood| f64::exp(log_likelihood - max_log_likelihood))
                            .collect_vec();

                        let dist = WeightedIndex::new(likelihoods).unwrap();
                        let sample_i = dist.sample(rng);
                        let shift = shifts[sample_i];

                        self.add_oil(env, oil_i, shift);
                    }

                    self.normalize(&env.input);

                    self
                }

                fn normalize(&mut self, input: &Input) {
                    let mut groups = FxHashMap::default();

                    for (i, shift) in self.shift.iter().enumerate() {
                        self.hash ^= input.hashes[i][Coord::try_from(*shift).unwrap()];
                    }

                    for (i, (oil, shift)) in input.oils.iter().zip(&self.shift).enumerate() {
                        let entry = groups.entry(&oil.pos).or_insert_with(|| vec![]);
                        entry.push((i, shift));
                    }

                    let mut new_shift = vec![CoordDiff::new(0, 0); input.oil_count];

                    for group in groups.values() {
                        let mut shifts = group.iter().map(|&(_, shift)| shift).collect_vec();
                        shifts.sort_unstable();

                        for (i, shift) in group.iter().map(|&(i, _)| i).zip(shifts) {
                            new_shift[i] = *shift;
                        }
                    }

                    self.shift = new_shift;

                    for (i, shift) in self.shift.iter().enumerate() {
                        self.hash ^= input.hashes[i][Coord::try_from(*shift).unwrap()];
                    }
                }

                pub(super) fn to_answer(&self, input: &Input) -> Vec<Coord> {
                    let mut map = Map2d::new_with(0, input.map_size);

                    for (oil, &shift) in input.oils.iter().zip(self.shift.iter()) {
                        for &p in oil.pos.iter() {
                            let pos = p + shift;

                            map[pos] = 1;
                        }
                    }

                    let mut results = vec![];

                    for row in 0..input.map_size {
                        for col in 0..input.map_size {
                            let c = Coord::new(row, col);

                            if map[c] != 0 {
                                results.push(c);
                            }
                        }
                    }

                    results
                }
            }

            impl Hash for State {
                fn hash<H: std::hash::Hasher>(&self, state: &mut H) {
                    self.hash.hash(state);
                }
            }

            impl PartialEq for State {
                fn eq(&self, other: &Self) -> bool {
                    self.hash == other.hash
                }
            }

            impl Eq for State {}

            impl PartialOrd for State {
                fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
                    self.hash.partial_cmp(&other.hash)
                }
            }

            impl Ord for State {
                fn cmp(&self, other: &Self) -> std::cmp::Ordering {
                    self.hash.cmp(&other.hash)
                }
            }

            pub(super) fn generate_states(
                env: &Env,
                mut states: Vec<State>,
                duration: f64,
                rng: &mut impl Rng,
            ) -> Vec<State> {
                let mut hashes = FxHashSet::default();

                for state in states.iter() {
                    hashes.insert(state.hash);
                }

                let base_log_likelihood = states
                    .iter()
                    .map(|s| s.log_likelihood)
                    .fold(f64::MIN, f64::max);

                let mut prefix_prob = vec![OrderedFloat(
                    (states[0].log_likelihood - base_log_likelihood).exp(),
                )];

                for i in 1..states.len() {
                    let p = OrderedFloat(
                        (states[i].log_likelihood - base_log_likelihood).exp()
                            + prefix_prob[i - 1].0,
                    );
                    prefix_prob.push(p);
                }

                let mut all_iter = 0;
                let mut valid_iter = 0;
                let mut accepted_count = 0;

                let duration_inv = 1.0 / duration;
                let since = std::time::Instant::now();

                let oil_count_dist = WeightedAliasIndex::new(vec![0, 0, 60, 20, 10, 5]).unwrap();

                loop {
                    let time =
                        env.input.duration_corrector.elapsed(since).as_secs_f64() * duration_inv;

                    if time >= 1.0 {
                        break;
                    }

                    all_iter += 1;

                    // 変形
                    let x = rng.gen_range(0.0..prefix_prob.last().unwrap().0);
                    let index = prefix_prob
                        .binary_search(&OrderedFloat(x))
                        .unwrap_or_else(|x| x);
                    let state = states[index].clone();
                    let oil_count = oil_count_dist.sample(rng).min(env.input.oil_count);
                    let new_state = state.neigh(env, rng, oil_count);

                    if hashes.insert(new_state.hash) {
                        // 凄く大きな値を引いてしまうとオーバーフローする可能性があるため注意
                        // 適切にサンプリングできればよいので、重みは適当に上限を設ける
                        let mut p = (new_state.log_likelihood - base_log_likelihood).exp()
                            + prefix_prob.last().unwrap().0;
                        p.change_min(1e200);
                        prefix_prob.push(OrderedFloat(p));
                        states.push(new_state);

                        accepted_count += 1;
                    }

                    valid_iter += 1;
                }

                eprintln!(
                    "[Generator] all_iter: {} valid_iter: {} accepted_count: {}",
                    all_iter, valid_iter, accepted_count
                );

                states
            }
        }
        mod observation {
            use crate::{
                common::ChangeMinMax,
                distributions::GaussianDistribution,
                grid::{Coord, CoordDiff, Map2d},
                problem::Input,
            };
            use itertools::Itertools;

            #[derive(Debug, Clone)]
            pub(super) struct ObservationManager {
                pub observations: Vec<Observation>,
                pub obs_log_likelihoods: Vec<f32>,
                pub obs_log_likelihoods_offsets: Vec<u32>,
                /// indices[oil_i][shift] := 影響するobs_iのVec
                pub relative_observation_indices: Vec<Map2d<Vec<usize>>>,
                /// indices[oil_i][shift] := 影響するcountのVec
                pub relative_observation_cnt: Vec<Map2d<Vec<usize>>>,
                pub relative_observation_cnt_u32: Vec<Map2d<Vec<u32>>>,
                /// indices[obs_i] := 影響する (oil_i, count, shift) のVec
                pub inv_relative_observation_indices: Vec<Vec<(usize, usize, CoordDiff)>>,
                /// matrix[obs_i][oil_i][shift] := oil_iをshiftだけ動かした領域とobs_iの重なりの数
                pub overlaps: Vec<Vec<Map2d<usize>>>,
                pub shift_candidates: Vec<Vec<CoordDiff>>,
                /// matrix[obs_i][oil_i] := oil_i以外の油田とobs_iとの重なりの最小値と最大値
                overlap_min_max: Vec<Vec<(usize, usize)>>,
            }

            impl ObservationManager {
                pub(super) fn new(input: &Input) -> Self {
                    let relative_observation_indices = (0..input.oil_count)
                        .map(|_| Map2d::new_with(vec![], input.map_size))
                        .collect();
                    let relative_observation_cnt = (0..input.oil_count)
                        .map(|_| Map2d::new_with(vec![], input.map_size))
                        .collect();
                    let relative_observation_cnt_u32 = (0..input.oil_count)
                        .map(|_| Map2d::new_with(vec![], input.map_size))
                        .collect();
                    let observations = vec![];
                    let inv_relative_observation_indices = vec![];
                    let overlaps = vec![];

                    let shift_candidates = input
                        .oils
                        .iter()
                        .map(|oil| {
                            let mut candidates = vec![];
                            for row in 0..=input.map_size - oil.height {
                                for col in 0..=input.map_size - oil.width {
                                    candidates.push(CoordDiff::new(row as isize, col as isize));
                                }
                            }

                            candidates
                        })
                        .collect_vec();

                    let overlap_min_max = vec![];

                    Self {
                        observations,
                        obs_log_likelihoods: vec![],
                        obs_log_likelihoods_offsets: vec![],
                        relative_observation_indices,
                        relative_observation_cnt,
                        relative_observation_cnt_u32,
                        inv_relative_observation_indices,
                        overlaps,
                        shift_candidates,
                        overlap_min_max,
                    }
                }

                pub(super) fn add_observation(&mut self, input: &Input, observation: Observation) {
                    let obs_id = self.observations.len();

                    self.obs_log_likelihoods_offsets
                        .push(self.obs_log_likelihoods.len() as u32);

                    for &v in observation.log_likelihoods.iter() {
                        self.obs_log_likelihoods.push(v as f32);
                    }

                    // 観測による条件式の更新
                    let mut observed_map = Map2d::new_with(false, input.map_size);

                    for &p in observation.pos.iter() {
                        observed_map[p] = true;
                    }

                    let mut inv_relative_observation_indices = vec![];
                    let mut overlaps = vec![];
                    let mut overlap_mins = vec![];
                    let mut overlap_maxs = vec![];

                    for (oil_i, oil) in input.oils.iter().enumerate() {
                        let mut overlap = Map2d::new_with(0, input.map_size);
                        let mut overlap_min = usize::MAX;
                        let mut overlap_max = usize::MIN;

                        let relative_observation_indices =
                            &mut self.relative_observation_indices[oil_i];
                        let relative_observation_cnt = &mut self.relative_observation_cnt[oil_i];
                        let relative_observation_cnt_u32 =
                            &mut self.relative_observation_cnt_u32[oil_i];

                        for row in 0..=input.map_size - oil.height {
                            for col in 0..=input.map_size - oil.width {
                                let c = Coord::new(row, col);
                                let shift = CoordDiff::new(row as isize, col as isize);
                                let mut count = 0;

                                for &p in oil.pos.iter() {
                                    let p = p + shift;

                                    if observed_map[p] {
                                        count += 1;
                                    }
                                }

                                overlap[c] = count;
                                overlap_min.change_min(count);
                                overlap_max.change_max(count);
                                relative_observation_indices[c].push(obs_id);
                                relative_observation_cnt[c].push(count);
                                relative_observation_cnt_u32[c].push(count as u32);
                                inv_relative_observation_indices.push((oil_i, count, shift));
                            }
                        }

                        overlaps.push(overlap);
                        overlap_mins.push(overlap_min);
                        overlap_maxs.push(overlap_max);
                    }

                    self.overlaps.push(overlaps);

                    let overlap_min_sum = overlap_mins.iter().sum::<usize>();
                    let overlap_max_sum = overlap_maxs.iter().sum::<usize>();
                    let overlap_min_max = (0..input.oil_count)
                        .map(|i| {
                            let min = overlap_min_sum - overlap_mins[i];
                            let max = overlap_max_sum - overlap_maxs[i];
                            (min, max)
                        })
                        .collect_vec();
                    self.overlap_min_max.push(overlap_min_max);

                    self.inv_relative_observation_indices
                        .push(inv_relative_observation_indices);
                    self.observations.push(observation);
                }
            }

            #[derive(Debug, Clone)]
            pub(super) struct Observation {
                pub pos: Vec<Coord>,
                /// k番目の要素はΣv(pi)=kとなる対数尤度を表す
                pub log_likelihoods: Vec<f64>,
            }

            impl Observation {
                pub(super) fn new(pos: Vec<Coord>, sampled: i32, input: &Input) -> Self {
                    assert!(pos.len() > 0);

                    let likelihoods_len = input.total_oil_tiles + 1;
                    let k = pos.len() as f64;
                    let x = sampled as f64;

                    // k = 1のときは特別扱い
                    let log_likelihoods = if pos.len() == 1 {
                        let mut log_likelihoods = vec![f64::MIN_POSITIVE.ln(); likelihoods_len];
                        log_likelihoods[sampled as usize] = 0.0;
                        log_likelihoods
                    } else {
                        let mut log_likelihoods = Vec::with_capacity(likelihoods_len);

                        for true_v in 0..likelihoods_len {
                            let v = true_v as f64;
                            let mean = (k - v) * input.eps + v * (1.0 - input.eps);
                            let variance = k * input.eps * (1.0 - input.eps);
                            let std_dev = variance.sqrt();
                            let gauss = GaussianDistribution::new(mean, std_dev);

                            let likelihood = if sampled == 0 {
                                gauss.calc_cumulative_dist(x + 0.5)
                            } else {
                                gauss.calc_cumulative_dist(x + 0.5)
                                    - gauss.calc_cumulative_dist(x - 0.5)
                            }
                            .max(f64::MIN_POSITIVE);
                            let log_likelihood = likelihood.ln();

                            log_likelihoods.push(log_likelihood);
                        }

                        log_likelihoods
                    };

                    let likelihoods = log_likelihoods.iter().copied().map(f64::exp).collect_vec();

                    let mut likelihood_prefix_sum = vec![0.0];

                    for i in 0..log_likelihoods.len() {
                        likelihood_prefix_sum.push(likelihood_prefix_sum[i] + likelihoods[i]);
                    }

                    Self {
                        pos,
                        log_likelihoods,
                    }
                }
            }
        }
        mod sampler {
            use crate::{
                common::ChangeMinMax as _,
                data_structures::IndexSet,
                distributions::GaussianDistribution,
                grid::{Coord, Map2d},
                problem::Input,
            };
            use itertools::{izip, Itertools};
            use ordered_float::OrderedFloat;
            use rand::{seq::SliceRandom as _, Rng};
            use std::{cmp::Reverse, ops::Range};

            use super::generator::{self};

            pub(super) fn select_sample_points(
                input: &Input,
                prob_table: &mut ProbTable,
                mcmc_states: Vec<generator::State>,
                max_sample_count: usize,
                duration: f64,
                rng: &mut impl Rng,
            ) -> Vec<Coord> {
                assert!(max_sample_count > 0);
                assert!(input.map_size * input.map_size >= max_sample_count);

                let all_coords = (0..input.map_size)
                    .flat_map(|row| (0..input.map_size).map(move |col| Coord::new(row, col)))
                    .collect::<Vec<_>>();
                let sample_count = max_sample_count / 2;
                let sampled_coords = all_coords
                    .choose_multiple(rng, sample_count)
                    .copied()
                    .collect_vec();

                let Ok(env) = Env::new(input, mcmc_states.clone(), max_sample_count) else {
                    // エントロピーを計算できないのでランダムなものを返す
                    return sampled_coords;
                };

                let mut map = Map2d::new_with(false, input.map_size);

                for (&shift, oil) in mcmc_states[0].shift.iter().zip(input.oils.iter()) {
                    for &p in oil.pos.iter() {
                        map[p + shift] = true;
                    }
                }

                for (&shift, oil) in mcmc_states[1].shift.iter().zip(input.oils.iter()) {
                    for &p in oil.pos.iter() {
                        map[p + shift] = true;
                    }
                }

                let mut sampled_coords = vec![];

                for row in 0..input.map_size {
                    for col in 0..input.map_size {
                        let c = Coord::new(row, col);

                        if map[c] {
                            sampled_coords.push(c);
                        }
                    }
                }

                let state = State::new(&env, &sampled_coords);
                let state = annealing(&env, state, prob_table, duration, rng);

                let mut result = vec![];

                for row in 0..input.map_size {
                    for col in 0..input.map_size {
                        let c = Coord::new(row, col);

                        if state.map[c] {
                            result.push(c);
                        }
                    }
                }

                result
            }

            pub(super) struct ProbTable {
                prob: Vec<Vec<Vec<f64>>>,
                prob_log: Vec<Vec<Vec<f64>>>,
                targets: Vec<Vec<Range<usize>>>,
            }

            impl ProbTable {
                const MIN_PROB: f64 = f64::MIN_POSITIVE;
                const MIN_PROB_LN: f64 = f64::MIN;

                pub(super) fn new(input: &Input) -> Self {
                    let sq_cnt = input.map_size * input.map_size;
                    let oil_cnt = input.oils.iter().map(|o| o.len).sum::<usize>();

                    let prob = vec![vec![vec![]; oil_cnt + 1]; sq_cnt + 1];
                    let prob_log = vec![vec![vec![]; oil_cnt + 1]; sq_cnt + 1];
                    let targets = vec![vec![Range::default(); oil_cnt + 1]; sq_cnt + 1];

                    Self {
                        prob,
                        prob_log,
                        targets,
                    }
                }

                fn range(&mut self, input: &Input, sq_cnt: usize, true_cnt: usize) -> Range<usize> {
                    assert!(sq_cnt > 0);

                    if self.targets[sq_cnt][true_cnt] == Range::default() {
                        self.lazy_init(input, sq_cnt, true_cnt);
                    }

                    self.targets[sq_cnt][true_cnt].clone()
                }

                fn probs(
                    &mut self,
                    input: &Input,
                    sq_cnt: usize,
                    true_cnt: usize,
                ) -> (&[f64], &[f64], Range<usize>) {
                    assert!(sq_cnt > 0);

                    if self.targets[sq_cnt][true_cnt] == Range::default() {
                        self.lazy_init(input, sq_cnt, true_cnt);
                    }

                    let range = self.targets[sq_cnt][true_cnt].clone();

                    (
                        &self.prob[sq_cnt][true_cnt][range.clone()],
                        &self.prob_log[sq_cnt][true_cnt][range.clone()],
                        range,
                    )
                }

                fn lazy_init(&mut self, input: &Input, sq_cnt: usize, true_cnt: usize) {
                    assert!(sq_cnt > 0);

                    let mut prob = vec![Self::MIN_PROB; input.map_size * input.map_size];
                    let mut prob_log = vec![Self::MIN_PROB_LN; input.map_size * input.map_size];

                    if sq_cnt == 1 {
                        prob[true_cnt] = 1.0;
                        prob_log[true_cnt] = 0.0;

                        self.prob[sq_cnt][true_cnt] = prob;
                        self.prob_log[sq_cnt][true_cnt] = prob_log;
                        self.targets[sq_cnt][true_cnt] = true_cnt..true_cnt + 1;
                        return;
                    }

                    let mean = (sq_cnt as f64 - true_cnt as f64) * input.eps
                        + true_cnt as f64 * (1.0 - input.eps);
                    let varinace = sq_cnt as f64 * input.eps * (1.0 - input.eps);
                    let std_dev = varinace.sqrt();
                    let gauss = GaussianDistribution::new(mean, std_dev);

                    let f = |v: usize| -> f64 {
                        if v == 0 {
                            gauss.calc_cumulative_dist(0.5)
                        } else {
                            gauss.calc_cumulative_dist(v as f64 + 0.5)
                                - gauss.calc_cumulative_dist(v as f64 - 0.5)
                        }
                        .max(Self::MIN_PROB)
                    };

                    let p = f(true_cnt);
                    let p_log2 = p.log2();
                    prob[true_cnt] = p;
                    prob_log[true_cnt] = p_log2;

                    let mut sum_p = p;
                    let mut l_seen = true_cnt;
                    let mut r_seen = true_cnt;

                    let (mut left, mut left_p) = if true_cnt == 0 {
                        (None, 0.0)
                    } else {
                        (Some(true_cnt - 1), f(true_cnt - 1))
                    };
                    let mut right = true_cnt + 1;
                    let mut right_p = f(true_cnt + 1);

                    // ±4σ分をカバーできる範囲を計算
                    while sum_p < 0.99994 {
                        if left_p >= right_p {
                            let l = left.unwrap();
                            l_seen = l;
                            let p_log2 = left_p.log2();
                            prob[l] = left_p;
                            prob_log[l] = p_log2;
                            sum_p += left_p;

                            if l == 0 {
                                left = None;
                                left_p = 0.0;
                            } else {
                                left = Some(l - 1);
                                left_p = f(l - 1);
                            }
                        } else {
                            r_seen = right;
                            let p_log2 = right_p.log2();
                            prob[right] = right_p;
                            prob_log[right] = p_log2;
                            sum_p += right_p;

                            right += 1;
                            right_p = f(right);
                        }
                    }

                    self.prob[sq_cnt][true_cnt] = prob;
                    self.prob_log[sq_cnt][true_cnt] = prob_log;
                    self.targets[sq_cnt][true_cnt] = l_seen..r_seen + 1;
                }
            }

            struct Env<'a> {
                input: &'a Input,
                states: Vec<generator::State>,
                state_maps: Vec<Map2d<usize>>,
                probs: Vec<f64>,
                probs_log2: Vec<f64>,
                base_entropy: f64,
                max_sample_count: usize,
            }

            impl<'a> Env<'a> {
                fn new(
                    input: &'a Input,
                    mut states: Vec<generator::State>,
                    max_sample_count: usize,
                ) -> Result<Self, ()> {
                    if states.len() <= 1 {
                        return Err(());
                    }

                    states.sort_unstable_by_key(|s| Reverse(OrderedFloat(s.log_likelihood)));

                    // 適当に尤度の大きい上位max_entropy_len個を選ぶ
                    if states.len() > input.params.max_entropy_len {
                        states.truncate(input.params.max_entropy_len);
                    }

                    // 確率計算に支障のない範囲でさらにstateの個数を絞る
                    let max_log_likelihood = states[0].log_likelihood;
                    let mut probs = states
                        .iter()
                        .map(|s| (s.log_likelihood - max_log_likelihood).exp())
                        .collect_vec();
                    let sum_probs = probs.iter().sum::<f64>();

                    for p in probs.iter_mut() {
                        *p /= sum_probs;
                    }

                    let mut take_count = states.len();
                    let mut prob_sum = 0.0;

                    for i in 0..states.len() {
                        prob_sum += probs[i];

                        if i >= 1 && prob_sum >= 0.9999 {
                            take_count = i + 1;
                            break;
                        }
                    }

                    states.truncate(take_count);
                    probs.truncate(take_count);

                    // 確率計算をやり直し
                    let sum_probs = probs.iter().sum::<f64>();

                    for p in probs.iter_mut() {
                        *p /= sum_probs;
                    }

                    let probs_log2 = probs.iter().map(|&p| p.log2()).collect_vec();

                    // マスの選択状況
                    let mut state_maps = vec![];

                    for state in states.iter() {
                        let mut map = Map2d::new_with(0, input.map_size);

                        for (oil, &shift) in input.oils.iter().zip(state.shift.iter()) {
                            for &p in oil.pos.iter() {
                                map[p + shift] += 1;
                            }
                        }

                        state_maps.push(map);
                    }

                    // 初期エントロピーの計算
                    let mut base_entropy = 0.0;

                    for &prob in probs.iter() {
                        base_entropy -= prob * prob.log2();
                    }

                    Ok(Self {
                        input,
                        states,
                        state_maps,
                        probs,
                        probs_log2,
                        base_entropy,
                        max_sample_count,
                    })
                }
            }

            #[derive(Debug, Clone)]
            struct State {
                map: Map2d<bool>,
                selected_count: usize,
                conditional_entropy: Option<f64>,
                selected: IndexSet,
                not_selected: IndexSet,
                overlap_counts: Vec<usize>,
            }

            static mut STATIC_INIT: bool = false;
            static mut P_OBS: Vec<f64> = vec![];
            static mut P_OIL_OBS: Vec<Vec<f64>> = vec![];
            static mut P_LOG2_OIL_OBS: Vec<Vec<f64>> = vec![];
            static mut P_OIL_OBS_RANGES: Vec<Range<usize>> = vec![];

            impl State {
                fn new(env: &Env, selected_coords: &[Coord]) -> Self {
                    let mut selected_map = Map2d::new_with(false, env.input.map_size);

                    for &c in selected_coords.iter() {
                        selected_map[c] = true;
                    }

                    let selected_count = selected_coords.len();

                    let n2 = env.input.map_size * env.input.map_size;
                    let mut selected = IndexSet::new(n2);
                    let mut not_selected = IndexSet::new(n2);

                    for row in 0..env.input.map_size {
                        for col in 0..env.input.map_size {
                            let c = Coord::new(row, col);
                            let i = row * env.input.map_size + col;

                            if selected_map[c] {
                                selected.add(i);
                            } else {
                                not_selected.add(i);
                            }
                        }
                    }

                    let mut overlap_counts = vec![];

                    for state_map in env.state_maps.iter() {
                        let mut count = 0;

                        for (&v, &b) in state_map.iter().zip(selected_map.iter()) {
                            count += v * b as usize;
                        }

                        overlap_counts.push(count);
                    }

                    Self {
                        map: selected_map,
                        selected_count,
                        conditional_entropy: None,
                        selected,
                        not_selected,
                        overlap_counts,
                    }
                }

                fn neigh_flip_single(mut self, env: &Env, rng: &mut impl Rng) -> Self {
                    let add = if self.selected_count <= 1 {
                        true
                    } else if self.selected_count >= env.max_sample_count {
                        false
                    } else {
                        rng.gen_bool(0.5)
                    };

                    let index = if add {
                        self.not_selected.as_slice().choose(rng).copied().unwrap()
                    } else {
                        self.selected.as_slice().choose(rng).copied().unwrap()
                    };

                    let row = index / env.input.map_size;
                    let col = index % env.input.map_size;
                    let coord = Coord::new(row, col);

                    self.flip(env, coord);

                    self
                }

                fn neigh_flip_double(mut self, env: &Env, rng: &mut impl Rng) -> Self {
                    let index0 = self.selected.as_slice().choose(rng).copied();
                    let index1 = self.not_selected.as_slice().choose(rng).copied();

                    let Some(index0) = index0 else {
                        return self;
                    };
                    let Some(index1) = index1 else {
                        return self;
                    };

                    let row0 = index0 / env.input.map_size;
                    let col0 = index0 % env.input.map_size;
                    let coord0 = Coord::new(row0, col0);
                    let row1 = index1 / env.input.map_size;
                    let col1 = index1 % env.input.map_size;
                    let coord1 = Coord::new(row1, col1);

                    self.flip(env, coord0);
                    self.flip(env, coord1);

                    self
                }

                fn flip(&mut self, env: &Env, c: Coord) {
                    let index = c.row * env.input.map_size + c.col;

                    if self.map[c] {
                        self.selected_count -= 1;
                        self.selected.remove(index);
                        self.not_selected.add(index);

                        for (cnt, map) in self.overlap_counts.iter_mut().zip(env.state_maps.iter())
                        {
                            *cnt -= map[c];
                        }
                    } else {
                        self.selected_count += 1;
                        self.selected.add(index);
                        self.not_selected.remove(index);

                        for (cnt, map) in self.overlap_counts.iter_mut().zip(env.state_maps.iter())
                        {
                            *cnt += map[c];
                        }
                    }

                    self.map[c] ^= true;
                    self.conditional_entropy = None;
                }

                /// 条件付きエントロピーを計算する
                fn calc_conditional_entropy(
                    &mut self,
                    env: &Env,
                    prob_table: &mut ProbTable,
                ) -> f64 {
                    if let Some(entropy) = self.conditional_entropy {
                        return entropy;
                    };

                    unsafe {
                        if !STATIC_INIT {
                            P_OBS.resize(1000, f64::MIN_POSITIVE);
                            P_OIL_OBS.resize(env.states.len(), vec![0.0; 1000]);
                            P_LOG2_OIL_OBS.resize(env.states.len(), vec![0.0; 1000]);
                            STATIC_INIT = true;
                        }

                        // obs_vの最大値を取得する
                        let mut max_obs_v = 0;

                        for &count in self.overlap_counts.iter() {
                            let range = prob_table.range(&env.input, self.selected_count, count);
                            max_obs_v.change_max(range.end);
                        }

                        let p_obs = &mut P_OBS[0..max_obs_v];
                        p_obs.fill(f64::MIN_POSITIVE);
                        let p_oil_obs = &mut P_OIL_OBS;
                        let p_log2_oil_obs = &mut P_LOG2_OIL_OBS;
                        let p_oil_obs_ranges = &mut P_OIL_OBS_RANGES;
                        p_oil_obs_ranges.clear();

                        // 確率の表を作成する
                        for (counts, state_prob, state_prob_log2, p_oil_obs, p_log2_oil_obs) in izip!(
                            &self.overlap_counts,
                            &env.probs,
                            &env.probs_log2,
                            p_oil_obs.iter_mut(),
                            p_log2_oil_obs.iter_mut()
                        ) {
                            let (prob, prob_log2, range) =
                                prob_table.probs(&env.input, self.selected_count, *counts);

                            let p_oil_obs = &mut p_oil_obs[..range.end - range.start];
                            let p_log2_oil_obs = &mut p_log2_oil_obs[..range.end - range.start];
                            p_oil_obs_ranges.push(range.clone());
                            let p_obs = &mut p_obs[range];

                            Self::add_p(p_obs, p_oil_obs, prob, *state_prob);
                            Self::add_p_log(p_log2_oil_obs, prob_log2, *state_prob_log2);
                        }

                        for p in p_obs.iter_mut() {
                            if *p == f64::MIN_POSITIVE {
                                *p = f64::MIN;
                            } else {
                                *p = p.log2();
                            }
                        }

                        let p_obs_log2 = p_obs;

                        let entropy = Self::sum_entropy(
                            p_oil_obs,
                            p_log2_oil_obs,
                            p_obs_log2,
                            p_oil_obs_ranges,
                        );

                        self.conditional_entropy = Some(entropy);
                        entropy
                    }
                }

                #[target_feature(enable = "avx,avx2")]
                unsafe fn add_p(
                    p_obs: &mut [f64],
                    p_oil_obs: &mut [f64],
                    prob: &[f64],
                    state_prob: f64,
                ) {
                    for ((p_obs, p_oil_obs), &prob) in
                        p_obs.iter_mut().zip(p_oil_obs.iter_mut()).zip(prob.iter())
                    {
                        // probは配置が確定したときに v_obs が観測される確率 p(v_obs | 配置)
                        // p(v_obs, 配置) = p(v_obs | 配置) * p(配置)
                        let p = prob * state_prob;
                        *p_obs += p;
                        *p_oil_obs = p;
                    }
                }

                #[target_feature(enable = "avx,avx2")]
                unsafe fn add_p_log(
                    p_log2_oil_obs: &mut [f64],
                    prob_log2: &[f64],
                    state_prob_log2: f64,
                ) {
                    for (p_log2_oil_obs, &prob_log2) in
                        p_log2_oil_obs.iter_mut().zip(prob_log2.iter())
                    {
                        // add_pのlog2版
                        let p_log2 = prob_log2 + state_prob_log2;
                        *p_log2_oil_obs = p_log2;
                    }
                }

                #[target_feature(enable = "avx,avx2")]
                #[inline(never)]
                unsafe fn sum_entropy(
                    p_oil_obs: &[Vec<f64>],
                    p_log2_oil_obs: &[Vec<f64>],
                    p_obs_log2: &[f64],
                    p_oil_obs_ranges: &[Range<usize>],
                ) -> f64 {
                    unsafe {
                        use std::arch::x86_64::*;

                        let mut entropy256 = _mm256_broadcast_sd(&0.0);
                        let mut entropy = 0.0;
                        const ELEMENTS_PER_256BIT: usize = 4;

                        for (p_oil_obs, p_log2_oil_obs, range) in izip!(
                            p_oil_obs.iter(),
                            p_log2_oil_obs.iter(),
                            p_oil_obs_ranges.iter()
                        ) {
                            let mut p_obs_log2: &[f64] = &p_obs_log2[range.clone()];

                            // p_oil_obs, p_log2_oil_obsは長めに取られているため、長さを揃える
                            // p_obs_log2と同じ長さまでしか要素は入っていない
                            let len = p_obs_log2.len();
                            let mut p_oil_obs: &[f64] = &p_oil_obs[..len];
                            let mut p_log2_oil_obs: &[f64] = &p_log2_oil_obs[..len];

                            while p_oil_obs.len() >= ELEMENTS_PER_256BIT {
                                // エントロピー計算をAVX/AVX2で行う
                                // 計算式はフォールバック版も参照

                                // メモリからデータを読み込み
                                let pp = _mm256_loadu_pd(p_oil_obs.as_ptr());
                                let p_l2 = _mm256_loadu_pd(p_log2_oil_obs.as_ptr());
                                let p_obs_l2 = _mm256_loadu_pd(p_obs_log2.as_ptr());

                                // -pp * (p_log2 - p_obs_log2) をする
                                // 符号を調整するため順番が逆になる
                                let sub = _mm256_sub_pd(p_obs_l2, p_l2);
                                let mul = _mm256_mul_pd(pp, sub);
                                entropy256 = _mm256_add_pd(entropy256, mul);

                                p_oil_obs = &p_oil_obs[ELEMENTS_PER_256BIT..];
                                p_log2_oil_obs = &p_log2_oil_obs[ELEMENTS_PER_256BIT..];
                                p_obs_log2 = &p_obs_log2[ELEMENTS_PER_256BIT..];
                            }

                            // フォールバック
                            for (&pp, &p_log2, &p_obs_log2) in
                                izip!(p_oil_obs, p_log2_oil_obs, p_obs_log2)
                            {
                                // dH = -p(v_obs) * p(配置 | v_obs) * log(p(配置 | v_obs))
                                //    = -p(配置, v_obs) * log(p(配置, v_obs) / p(v_obs))
                                //    = -p(配置, v_obs) * (log(p(配置, v_obs)) - log(p(v_obs)))
                                entropy -= pp * (p_log2 - p_obs_log2);
                            }
                        }

                        // TODO: 水平加算を理解する
                        let mut buffer = [0.0; ELEMENTS_PER_256BIT];
                        _mm256_storeu_pd(buffer.as_mut_ptr(), entropy256);

                        for &v in buffer.iter() {
                            entropy += v;
                        }

                        entropy
                    }
                }

                /// スコア（= 相互情報量 / 調査コスト）を計算する
                ///
                /// スコアは大きいほどよい
                fn calc_score(&mut self, env: &Env, prob_table: &mut ProbTable) -> f64 {
                    let mutual_information =
                        env.base_entropy - self.calc_conditional_entropy(env, prob_table);
                    let score = mutual_information * (self.selected_count as f64).sqrt();
                    score
                }
            }

            fn annealing(
                env: &Env,
                initial_solution: State,
                prob_table: &mut ProbTable,
                duration: f64,
                rng: &mut impl Rng,
            ) -> State {
                let mut solution = initial_solution;
                let mut best_solution = solution.clone();
                let mut current_score = solution.calc_score(env, prob_table);
                let mut best_score = current_score;
                let init_score = current_score;

                let mut all_iter = 0;

                let duration_inv = 1.0 / duration;
                let since = std::time::Instant::now();

                let mut counts = [0; 2];
                let mut acc = [0; 2];

                loop {
                    all_iter += 1;

                    let time =
                        env.input.duration_corrector.elapsed(since).as_secs_f64() * duration_inv;

                    if time >= 1.0 {
                        break;
                    }

                    // 変形
                    let (mut new_state, t) = if rng.gen_bool(0.5) {
                        (solution.clone().neigh_flip_single(env, rng), 0)
                    } else {
                        (solution.clone().neigh_flip_double(env, rng), 1)
                    };

                    counts[t] += 1;

                    // スコア計算
                    let new_score = new_state.calc_score(env, prob_table);
                    let score_diff = new_score - current_score;

                    // 速度が稼げていないので山登りにしている
                    if score_diff >= 0.0 {
                        // || rng.gen_bool(f64::exp(score_diff as f64 * inv_temp)) {
                        // 解の更新
                        current_score = new_score;
                        solution = new_state;
                        acc[t] += 1;

                        if best_score.change_max(current_score) {
                            best_solution = solution.clone();
                        }
                    }
                }

                eprintln!(
                    "[Sampler]   {:.5} -> {:.5}, iter: {}",
                    init_score, best_score, all_iter
                );

                best_solution
            }
        }

        use crate::{
            grid::{Coord, CoordDiff, Map2d},
            problem::{Input, Judge},
            solver::multi_dig::{generator::State, observation::Observation, sampler::ProbTable},
        };
        use bitset_fixed::BitSet;
        use itertools::Itertools;
        use ordered_float::OrderedFloat;
        use rand::seq::SliceRandom;
        use rand_core::SeedableRng;
        use rand_pcg::Pcg64Mcg;
        use std::{cmp::Reverse, vec};

        use self::observation::ObservationManager;

        use super::Solver;

        pub struct MultiDigSolver<'a> {
            judge: Judge<'a>,
        }

        impl<'a> MultiDigSolver<'a> {
            pub fn new(judge: Judge<'a>) -> Self {
                Self { judge }
            }

            fn answer_all(&mut self, states: &Vec<State>, input: &Input) -> Result<(), ()> {
                // 全部順番に答える
                for state in states.iter() {
                    let answer = state.to_answer(input);
                    if self.judge.answer(&answer).is_ok() {
                        return Ok(());
                    }
                }

                Err(())
            }
        }

        impl<'a> Solver for MultiDigSolver<'a> {
            fn solve(&mut self, input: &crate::problem::Input) {
                let mut env = Env::new(input);
                let mut rng = Pcg64Mcg::from_entropy();

                let mut states = vec![State::new(
                    vec![CoordDiff::new(0, 0); input.oil_count],
                    &env,
                )];
                let mut prob_table = ProbTable::new(input);

                let elapsed = input.duration_corrector.elapsed(input.since);
                let time_table = input.time_conductor.get_time_table(
                    elapsed,
                    Input::TIME_LIMIT,
                    self.judge.max_query_count(),
                );

                let answer_threshold_ratio = input.params.answer_threshold_ratio;
                const RETAIN_THRESHOLD_RATIO: f64 = 100.0;

                while self.judge.can_query() {
                    let turn = self.judge.query_count() + 1;
                    eprintln!(
                        "===== turn {} / {} =====",
                        turn,
                        self.judge.max_query_count()
                    );

                    // TLE緊急回避モード
                    if input.duration_corrector.elapsed(input.since) >= Input::TIME_LIMIT {
                        if self.answer_all(&states, input).is_ok() {
                            return;
                        }

                        for _ in 0..input.map_size * input.map_size * 2 {
                            self.judge.query_single(Coord::new(0, 0));
                        }

                        return;
                    }

                    let time_limit_turn = time_table[turn as usize - 1];

                    // 新たな置き方を生成
                    let duration = (time_limit_turn
                        .saturating_sub(input.duration_corrector.elapsed(input.since)))
                    .mul_f64(input.time_conductor.phase_ratio());
                    states =
                        generator::generate_states(&env, states, duration.as_secs_f64(), &mut rng);
                    states.sort_unstable();
                    states.dedup();
                    states.shuffle(&mut rng);
                    states.sort_unstable_by(|a, b| {
                        b.log_likelihood.partial_cmp(&a.log_likelihood).unwrap()
                    });

                    let max_log_likelihood = states
                        .iter()
                        .map(|s| s.log_likelihood)
                        .fold(f64::MIN, f64::max);

                    let answer_threshold_ln = answer_threshold_ratio.ln();

                    let mut likelihoods = states
                        .iter()
                        .filter(|s| max_log_likelihood - s.log_likelihood <= answer_threshold_ln)
                        .group_by(|s| {
                            let mut map = BitSet::new(input.map_size * input.map_size);
                            for i in 0..input.oil_count {
                                map |= &input.oils[i].get_shifted_bitset(input, s.shift[i]);
                            }
                            map
                        })
                        .into_iter()
                        .map(|(_, ss)| {
                            let mut likelihood = 0.0;
                            let mut hash = None;

                            for p in ss.into_iter() {
                                likelihood += (p.log_likelihood - states[0].log_likelihood).exp();
                                if hash.is_none() {
                                    hash = Some(p.hash);
                                }
                            }

                            (likelihood, hash.unwrap())
                        })
                        .collect_vec();
                    likelihoods
                        .sort_unstable_by_key(|(likelihood, _)| Reverse(OrderedFloat(*likelihood)));

                    let state = &states[0];
                    let ratio = if likelihoods.len() >= 2 {
                        likelihoods[0].0 / likelihoods[1].0
                    } else {
                        f64::INFINITY
                    };

                    self.judge.comment(&format!(
                        "found: {} log_likelihood: {:.3}, ratio: {:.3}",
                        likelihoods.len(),
                        likelihoods[0].0,
                        ratio
                    ));

                    let mut map = Map2d::new_with(0.0, input.map_size);

                    for i in 0..input.oil_count {
                        for p in input.oils[i].pos.iter() {
                            map[*p + state.shift[i]] += 1.0 / input.oil_count as f64;
                        }
                    }

                    self.judge.comment_colors(&map);

                    if ratio >= answer_threshold_ratio {
                        let state = states.iter().find(|s| s.hash == likelihoods[0].1).unwrap();

                        if self.judge.answer(&state.to_answer(input)).is_ok() {
                            return;
                        }
                    } else if self.judge.remaining_query_count() <= 10
                        && self.judge.remaining_query_count() <= states.len()
                    {
                        if self.answer_all(&states, input).is_ok() {
                            return;
                        }
                    }

                    let max_sample_count = input.map_size * input.map_size;
                    let duration = time_limit_turn
                        .saturating_sub(input.duration_corrector.elapsed(input.since));

                    let targets = sampler::select_sample_points(
                        input,
                        &mut prob_table,
                        states.clone(),
                        max_sample_count,
                        duration.as_secs_f64(),
                        &mut rng,
                    );

                    let sampled = self.judge.query_multiple(&targets);
                    let observation = Observation::new(targets, sampled, input);

                    env.obs.add_observation(input, observation);

                    for state in states.iter_mut() {
                        state.add_last_observation(&env);
                    }

                    let max_log_likelihood = states
                        .iter()
                        .map(|s| s.log_likelihood)
                        .fold(f64::MIN, f64::max);

                    let retain_threshold = RETAIN_THRESHOLD_RATIO.ln();
                    states.retain(|s| max_log_likelihood - s.log_likelihood <= retain_threshold);
                }
            }
        }

        #[derive(Debug, Clone)]
        struct Env<'a> {
            input: &'a Input,
            obs: ObservationManager,
        }

        impl<'a> Env<'a> {
            fn new(input: &'a Input) -> Self {
                let obs = ObservationManager::new(input);

                Self { input, obs }
            }
        }
    }
    pub mod single_dig {
        use itertools::Itertools as _;
        use ordered_float::OrderedFloat;
        use rand::{
            seq::{IteratorRandom, SliceRandom as _},
            Rng,
        };
        use rand_core::SeedableRng as _;
        use rand_distr::{Distribution as _, WeightedAliasIndex};
        use rand_pcg::Pcg64Mcg;
        use rustc_hash::{FxHashMap, FxHashSet};

        use crate::{
            common::ChangeMinMax as _,
            grid::{Coord, CoordDiff, Map2d},
            problem::{Input, Judge},
        };

        use super::Solver;

        pub struct SingleDigSolver<'a> {
            judge: Judge<'a>,
        }

        impl<'a> SingleDigSolver<'a> {
            pub fn new(judge: Judge<'a>) -> Self {
                Self { judge }
            }
        }

        impl<'a> Solver for SingleDigSolver<'a> {
            fn solve(&mut self, input: &crate::problem::Input) {
                let mut env = Env::new(input);
                let mut rng = Pcg64Mcg::from_entropy();

                let elapsed = input.duration_corrector.elapsed(input.since);
                let time_table = input.time_conductor.get_time_table(
                    elapsed,
                    Input::TIME_LIMIT,
                    self.judge.max_query_count(),
                );

                let mut states = vec![State::new(
                    vec![CoordDiff::new(0, 0); input.oil_count],
                    env.map.clone(),
                    &env,
                )];

                for turn in 0..input.map_size * input.map_size {
                    let time_limit_turn = time_table[turn];
                    let duration = time_limit_turn
                        .saturating_sub(input.duration_corrector.elapsed(input.since));
                    states = generate_states(&env, states, duration.as_secs_f64(), &mut rng);
                    states.sort_unstable();
                    states.dedup();

                    let min_violation = states.iter().map(|s| s.violations).min().unwrap();
                    states.retain(|s| s.violations == min_violation);
                    eprintln!("turn: {}, states: {}", turn, states.len());

                    color_map(&states, &mut rng, &env, &mut self.judge);

                    if states.len() == 1 {
                        let state = states.iter().next().unwrap();

                        if state.calc_score() == 0 {
                            let answer = state.to_answer(input);

                            if self.judge.answer(&answer).is_ok() {
                                return;
                            }
                        }
                    }

                    let next_pos = choose_next_pos(&states, &mut rng, input, &env, &mut self.judge);

                    let observation = self.judge.query_single(next_pos);
                    env.add_observation(next_pos, observation);

                    for s in states.iter_mut() {
                        s.add_observation(&env, next_pos, observation);
                    }

                    let min_violation = states.iter().map(|s| s.violations).min().unwrap();
                    states.retain(|s| s.violations == min_violation);
                }

                let mut answer = vec![];

                for row in 0..input.map_size {
                    for col in 0..input.map_size {
                        let c = Coord::new(row, col);

                        if env.map[c].unwrap() > 0 {
                            answer.push(c);
                        }
                    }
                }

                self.judge.answer(&answer).unwrap();
            }
        }

        fn choose_next_pos(
            solutions: &[State],
            rng: &mut rand_pcg::Mcg128Xsl64,
            input: &Input,
            env: &Env<'_>,
            judge: &mut Judge,
        ) -> Coord {
            let mut maps = vec![];
            let sample_count = 200.min(solutions.len());
            let mut states = solutions.choose_multiple(rng, sample_count);

            if states.len() == 1 {
                let mut map = Map2d::new_with(0, input.map_size);
                let state = states.next().unwrap();

                for (&shift, oil) in state.shift.iter().zip(input.oils.iter()) {
                    for &p in oil.pos.iter() {
                        let p = p + shift;

                        map[p] += 1;
                    }
                }

                let mut exist = vec![];
                let mut not_exist = vec![];

                for row in 0..input.map_size {
                    for col in 0..input.map_size {
                        let c = Coord::new(row, col);

                        if env.map[c].is_some() {
                            continue;
                        }

                        if map[c] > 0 {
                            exist.push(c);
                        } else {
                            not_exist.push(c);
                        }
                    }
                }

                // ブロックがある場所の面積の方が小さいので、そのような場所から選ぶことで早く間違いに気付かせる
                return exist
                    .choose(rng)
                    .copied()
                    .unwrap_or_else(|| not_exist.choose(rng).copied().unwrap());
            }

            for state in states {
                let mut map = Map2d::new_with(0, input.map_size);

                for (&shift, oil) in state.shift.iter().zip(input.oils.iter()) {
                    for &p in oil.pos.iter() {
                        let p = p + shift;

                        map[p] += 1;
                    }
                }

                maps.push(map);
            }

            let max_v = maps
                .iter()
                .flat_map(|m| m.iter())
                .max()
                .copied()
                .unwrap_or(0) as usize;

            // log2のテーブルを作成
            // p log p はp=1のとき0になるため、適当な値を入れてよい
            let log_table = (0..=sample_count)
                .map(|v| (1.0 / v.max(1) as f64).log2())
                .collect::<Vec<_>>();

            let mut counts = Map2d::new_with(vec![0; max_v + 1], input.map_size);

            for map in maps.iter() {
                for (&m, cnt) in map.iter().zip(counts.iter_mut()) {
                    cnt[m as usize] += 1;
                }
            }

            let mut candidates = vec![];
            let mut best_entropy = f64::MAX;

            for row in 0..input.map_size {
                for col in 0..input.map_size {
                    let c = Coord::new(row, col);

                    if env.map[c].is_some() {
                        continue;
                    }

                    // エントロピーを計算し、最小となる箇所（相互情報量が最大となる箇所）を選ぶ
                    let mut entropy = 0.0;

                    for &c in counts[c].iter() {
                        // H(配置|観測値) = Σ_i p(観測値=i)H(配置|観測値=i)
                        //               = Σ_i p(観測値=i) log(n(配置|観測値=i))
                        let p = c as f64 / sample_count as f64;
                        entropy -= p * log_table[c];
                    }

                    if best_entropy.change_min(entropy) {
                        candidates.clear();
                    }

                    if best_entropy == entropy {
                        candidates.push(c);
                    }
                }
            }

            let c = candidates.choose(rng).copied().unwrap();
            let mutual_info = -log_table[sample_count] - best_entropy;

            judge.comment(&format!(
                "next: {} with mutual information {:.3}",
                c, mutual_info
            ));

            c
        }

        fn color_map(
            states: &[State],
            rng: &mut rand_pcg::Mcg128Xsl64,
            env: &Env<'_>,
            judge: &mut Judge,
        ) {
            let state = states.choose(rng).unwrap().clone();
            let mut colors = Map2d::new_with(0.0, env.input.map_size);

            for &c in state.to_answer(env.input).iter() {
                colors[c] = 1.0 / states.len() as f64;
            }

            judge.comment(&format!(
                "score: {}, remaining: {}",
                state.calc_score(),
                states.len()
            ));
            judge.comment_colors(&colors);
        }

        #[derive(Debug, Clone)]
        struct Env<'a> {
            input: &'a Input,
            map: Map2d<Option<i32>>,
            shift_candidates: Vec<Vec<CoordDiff>>,
        }

        impl<'a> Env<'a> {
            fn new(input: &'a Input) -> Self {
                let map = Map2d::new_with(None, input.map_size);
                let shift_candidates = input
                    .oils
                    .iter()
                    .map(|oil| {
                        let mut cands = vec![];

                        for row in 0..=input.map_size - oil.height {
                            for col in 0..=input.map_size - oil.width {
                                cands.push(CoordDiff::new(row as isize, col as isize));
                            }
                        }

                        cands
                    })
                    .collect_vec();

                Self {
                    input,
                    map,
                    shift_candidates,
                }
            }

            fn add_observation(&mut self, pos: Coord, value: i32) {
                self.map[pos] = Some(value);

                for (oil, cand) in self.input.oils.iter().zip(self.shift_candidates.iter_mut()) {
                    if value == 0 {
                        cand.retain(|&shift| {
                            oil.pos.iter().all(|&p| {
                                let p = p + shift;
                                p != pos
                            })
                        });
                    } else if value == self.input.oil_count as i32 {
                        cand.retain(|&shift| {
                            oil.pos.iter().any(|&p| {
                                let p = p + shift;
                                p == pos
                            })
                        });
                    }
                }
            }
        }

        #[derive(Debug, Clone)]
        struct State {
            shift: Vec<CoordDiff>,
            violations: i32,
            map: Map2d<Option<i32>>,
            hash: u64,
        }

        static mut BEST_SHIFT_BUF: Vec<CoordDiff> = vec![];

        impl State {
            fn new(shift: Vec<CoordDiff>, mut map: Map2d<Option<i32>>, env: &Env) -> Self {
                for (oil, &shift) in env.input.oils.iter().zip(shift.iter()) {
                    for &p in oil.pos.iter() {
                        let pos = p + shift;

                        let Some(v) = &mut map[pos] else {
                            continue;
                        };

                        *v -= 1;
                    }
                }

                let mut violations = 0;

                for &v in map.iter().flatten() {
                    violations += v.abs();
                }

                let mut hash = 0;

                for (&shift, h) in shift.iter().zip(env.input.hashes.iter()) {
                    hash ^= h[Coord::try_from(shift).unwrap()];
                }

                let mut state = Self {
                    shift,
                    violations,
                    map,
                    hash,
                };

                state.normalize(&env);
                state
            }

            fn add_observation(&mut self, env: &Env, pos: Coord, value: i32) {
                self.map[pos] = Some(value);

                for (oil, &shift) in env.input.oils.iter().zip(self.shift.iter()) {
                    for &p in oil.pos.iter() {
                        let p = p + shift;

                        if p != pos {
                            continue;
                        }

                        let v = self.map[p].as_mut().unwrap();

                        *v -= 1;
                    }
                }

                self.violations += self.map[pos].unwrap().abs();
            }

            fn neigh(mut self, env: &Env, rng: &mut impl Rng, choose_cnt: usize) -> Self {
                let mut oil_indices = (0..env.input.oil_count).choose_multiple(rng, choose_cnt);
                oil_indices.shuffle(rng);

                for &oil_i in &oil_indices {
                    self.remove_oil(env, oil_i);

                    let shift = env.shift_candidates[oil_i].choose(rng).copied().unwrap();
                    self.add_oil(env, oil_i, shift);
                }

                for &oil_i in &oil_indices {
                    let mut best_violation = i32::MAX;
                    let best_shifts = unsafe { &mut BEST_SHIFT_BUF };
                    best_shifts.clear();
                    self.remove_oil(env, oil_i);

                    for &shift in env.shift_candidates[oil_i].iter() {
                        let violation = self.add_oil_whatif(env, oil_i, shift);

                        if best_violation.change_min(violation) {
                            best_shifts.clear();
                        }

                        if violation == best_violation {
                            best_shifts.push(shift);
                        }
                    }

                    let best_shift = *best_shifts.choose(rng).unwrap();

                    self.add_oil(env, oil_i, best_shift);
                }

                self.normalize(&env);

                self
            }

            fn add_oil(&mut self, env: &Env, oil: usize, shift: CoordDiff) {
                for &p in env.input.oils[oil].pos.iter() {
                    let p = p + shift;

                    let Some(v) = &mut self.map[p] else {
                        continue;
                    };

                    if *v > 0 {
                        self.violations -= 1;
                    } else {
                        self.violations += 1;
                    }

                    *v -= 1;
                }

                self.shift[oil] = shift;
                self.hash ^= env.input.hashes[oil][Coord::try_from(shift).unwrap()];
            }

            fn remove_oil(&mut self, env: &Env, oil: usize) {
                let shift = self.shift[oil];

                for &p in env.input.oils[oil].pos.iter() {
                    let p = p + shift;

                    let Some(v) = &mut self.map[p] else {
                        continue;
                    };

                    if *v >= 0 {
                        self.violations += 1;
                    } else {
                        self.violations -= 1;
                    }

                    *v += 1;
                }

                self.hash ^= env.input.hashes[oil][Coord::try_from(shift).unwrap()];
            }

            fn add_oil_whatif(&self, env: &Env, oil: usize, shift: CoordDiff) -> i32 {
                let mut violation = 0;

                for &p in env.input.oils[oil].pos.iter() {
                    let p = p + shift;

                    let Some(v) = self.map[p] else {
                        continue;
                    };

                    if v > 0 {
                        violation -= 1;
                    } else {
                        violation += 1;
                    }
                }

                violation
            }

            fn normalize(&mut self, env: &Env) {
                let mut groups = FxHashMap::default();

                for (i, (oil, shift)) in env
                    .input
                    .oils
                    .iter()
                    .zip(self.shift.iter().copied())
                    .enumerate()
                {
                    let entry = groups.entry(&oil.pos).or_insert_with(|| vec![]);
                    entry.push((i, shift));
                }

                let mut new_shift = vec![CoordDiff::new(0, 0); env.input.oil_count];

                for group in groups.values() {
                    for &(i, shift) in group {
                        self.hash ^= env.input.hashes[i][Coord::try_from(shift).unwrap()];
                    }

                    let mut shifts = group.iter().map(|&(_, shift)| shift).collect_vec();

                    shifts.sort_unstable();

                    for (i, &shift) in group.iter().map(|&(i, _)| i).zip(shifts.iter()) {
                        self.hash ^= env.input.hashes[i][Coord::try_from(shift).unwrap()];
                        new_shift[i] = shift;
                    }
                }

                self.shift = new_shift;
            }

            fn calc_score(&self) -> i32 {
                self.violations
            }

            fn to_answer(&self, input: &Input) -> Vec<Coord> {
                let mut map = Map2d::new_with(0, input.map_size);

                for (oil, &shift) in input.oils.iter().zip(self.shift.iter()) {
                    for &p in oil.pos.iter() {
                        let pos = p + shift;

                        map[pos] = 1;
                    }
                }

                let mut results = vec![];

                for row in 0..input.map_size {
                    for col in 0..input.map_size {
                        let c = Coord::new(row, col);

                        if map[c] != 0 {
                            results.push(c);
                        }
                    }
                }

                results
            }
        }

        impl PartialEq for State {
            fn eq(&self, other: &Self) -> bool {
                self.hash == other.hash
            }
        }

        impl Eq for State {}

        impl std::hash::Hash for State {
            fn hash<H: std::hash::Hasher>(&self, state: &mut H) {
                self.hash.hash(state);
            }
        }

        impl PartialOrd for State {
            fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
                self.hash.partial_cmp(&other.hash)
            }
        }

        impl Ord for State {
            fn cmp(&self, other: &Self) -> std::cmp::Ordering {
                self.hash.cmp(&other.hash)
            }
        }

        fn generate_states(
            env: &Env,
            mut states: Vec<State>,
            duration: f64,
            rng: &mut impl Rng,
        ) -> Vec<State> {
            let mut hashes = FxHashSet::default();

            for state in states.iter() {
                hashes.insert(state.hash);
            }

            let mut prefix_prob = vec![OrderedFloat(prob(&states[0]))];

            for i in 1..states.len() {
                let p = OrderedFloat(prob(&states[i]) + prefix_prob[i - 1].0);
                prefix_prob.push(p);
            }

            let mut all_iter = 0;

            let duration_inv = 1.0 / duration;
            let since = std::time::Instant::now();

            let oil_count_dist = WeightedAliasIndex::new(vec![0, 10, 60, 20, 10]).unwrap();

            loop {
                all_iter += 1;
                if (all_iter & ((1 << 4) - 1)) == 0 {
                    let time =
                        env.input.duration_corrector.elapsed(since).as_secs_f64() * duration_inv;

                    if time >= 1.0 {
                        break;
                    }
                }

                // 変形
                let x = rng.gen_range(0.0..prefix_prob.last().unwrap().0);
                let index = prefix_prob
                    .binary_search(&OrderedFloat(x))
                    .unwrap_or_else(|x| x);
                let state = states[index].clone();

                let oil_count = (oil_count_dist.sample(rng)).min(env.input.oil_count);
                let new_state = state.neigh(env, rng, oil_count);

                if hashes.insert(new_state.hash) {
                    let p = prob(&new_state) + prefix_prob.last().unwrap().0;
                    prefix_prob.push(OrderedFloat(p));
                    states.push(new_state);
                }
            }

            eprintln!(
                "all_iter: {}, violation: {}",
                all_iter,
                states.iter().map(|s| s.violations).min().unwrap()
            );

            states
        }

        fn prob(state: &State) -> f64 {
            2.0f64.powi(-state.violations)
        }
    }

    use crate::problem::Input;

    pub trait Solver {
        fn solve(&mut self, input: &Input);
    }
}

#[allow(unused_imports)]
use proconio::*;
#[allow(unused_imports)]
use rand::prelude::*;
use solver::Solver;

use crate::problem::Judge;

fn main() {
    let mut judge = Judge::new();
    let input = judge.read_input();

    eprintln!(
        "k={:.3}, b={:.3}, r={:.3}, multi={:.3}",
        input.time_conductor.k(),
        input.time_conductor.b(),
        input.time_conductor.phase_ratio(),
        input.params.use_multi_dig_solver
    );

    let mut solver: Box<dyn Solver> = if input.params.use_multi_dig_solver() {
        Box::new(solver::multi_dig::MultiDigSolver::new(judge))
    } else {
        Box::new(solver::single_dig::SingleDigSolver::new(judge))
    };

    solver.solve(&input);
}
