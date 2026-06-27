use amplify_num::posit;
use core::time;
use delaunator::EPSILON;
use fixedbitset::FixedBitSet;
use im_rc::hashset;
use itertools::Itertools;
use multiversion::target;
use nalgebra::constraint;
use num_integer::Roots;
use num_traits::float;
use proconio::input;
use proconio::source::line::LineSource; // For interactive judge
use rand::{Error, Fill, distributions, prelude::*};
use rand_distr::uniform::{SampleRange, SampleUniform};
use rand_distr::{Standard, WeightedIndex};
use rand_pcg::Pcg64Mcg;
use std::cmp::{Ordering, max, min};
use std::mem::forget;
// Or other StdRng implementation
use std::collections::{BTreeMap, HashSet};
use std::collections::{BTreeSet, BinaryHeap};
use std::collections::{HashMap, VecDeque};
use std::fmt;
use std::io::BufReader; // For interactive judge and file output
use std::io::{self, BufRead, BufWriter, Write};
use std::time::Instant;
use std::vec::Vec;
use std::{cmp, fs::File, vec}; // Make sure all necessary imports are present
#[macro_use]
extern crate alloc;

mod robust {
    // Copyright 2017 The Spade Developers.
    // Copyright 2020 The GeoRust Project Developers.
    //
    // Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
    // http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
    // <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
    // option. This file may not be copied, modified, or distributed
    // except according to those terms.
    #![allow(non_snake_case)]

    //! # Adaptive Precision Floating-Point Arithmetic and Fast Robust Predicates for Computational Geometry
    //! This is a direct transcript of the source code and algorithms provided by
    //! Jonathan Richard Shewchuk ([https://www.cs.cmu.edu/~quake/robust.html](https://www.cs.cmu.edu/~quake/robust.html))
    //! See the paper and the source code for more information.
    //!
    //! The module offers adaptive and precise calculations for orientation queries
    //! – "on which side of a line (2d) or plane (3d) does a point lie?" – and in-circle / in-sphere queries
    //! – "is a given point contained in the circumference of a triangle?".  
    //! The "adaptive" nature will increase performance only if a simpler calculation
    //! cannot be guaranteed to be accurate enough, yielding higher performance on
    //! average.
    //!
    //! The public API will accept both `f32` and `f64` input points for predicate checking, with input being converted to
    //! `f64` values for internal use.
    //! This has no effect on precision, as the [IEEE-754 standard](https://drive.google.com/file/d/0B3O3Ys97VjtxYXBCY08wanNoZ1U/view) (section 5.3)
    //! guarantees that conversion from `f32` to `f64` must be exact.
    //!
    //! # Features
    //!
    //! - `no_std`: Build without the Rust standard library

    /// A two dimensional coordinate.
    #[derive(Copy, Clone, Debug, PartialEq)]
    pub struct Coord<T: Into<f64>> {
        pub x: T,
        pub y: T,
    }

    /// A three dimensional coordinate.
    #[derive(Copy, Clone, Debug, PartialEq)]
    pub struct Coord3D<T: Into<f64>> {
        pub x: T,
        pub y: T,
        pub z: T,
    }

    // These values are precomputed from the "exactinit" method of the c-source code. They should? be
    // the same in all IEEE-754 environments, including rust f64
    const SPLITTER: f64 = 134_217_729f64;
    const EPSILON: f64 = 0.000_000_000_000_000_111_022_302_462_515_65;
    const RESULTERRBOUND: f64 = (3.0 + 8.0 * EPSILON) * EPSILON;
    const CCWERRBOUND_B: f64 = (2.0 + 12.0 * EPSILON) * EPSILON;
    const CCWERRBOUND_C: f64 = (9.0 + 64.0 * EPSILON) * EPSILON * EPSILON;
    const O3DERRBOUND_A: f64 = (7.0 + 56.0 * EPSILON) * EPSILON;
    const O3DERRBOUND_B: f64 = (3.0 + 28.0 * EPSILON) * EPSILON;
    const O3DERRBOUND_C: f64 = (26.0 + 288.0 * EPSILON) * EPSILON * EPSILON;
    const ICCERRBOUND_A: f64 = (10.0 + 96.0 * EPSILON) * EPSILON;
    const ICCERRBOUND_B: f64 = (4.0 + 48.0 * EPSILON) * EPSILON;
    const ICCERRBOUND_C: f64 = (44.0 + 576.0 * EPSILON) * EPSILON * EPSILON;
    const ISPERRBOUND_A: f64 = (16.0 + 224.0 * EPSILON) * EPSILON;
    const ISPERRBOUND_B: f64 = (5.0 + 72.0 * EPSILON) * EPSILON;
    const ISPERRBOUND_C: f64 = (71.0 + 1408.0 * EPSILON) * EPSILON * EPSILON;

    /// Returns a positive value if the coordinates `pa`, `pb`, and `pc` occur in counterclockwise order
    /// (`pc` lies to the **left** of the directed line defined by coordinates `pa` and `pb`).  
    /// Returns a negative value if they occur in clockwise order (`pc` lies to the **right** of the directed line `pa, pb`).  
    /// Returns `0` if they are **collinear**.  
    pub fn orient2d<T: Into<f64>>(pa: Coord<T>, pb: Coord<T>, pc: Coord<T>) -> f64 {
        let pa = Coord {
            x: pa.x.into(),
            y: pa.y.into(),
        };
        let pb = Coord {
            x: pb.x.into(),
            y: pb.y.into(),
        };
        let pc = Coord {
            x: pc.x.into(),
            y: pc.y.into(),
        };

        let detleft = (pa.x - pc.x) * (pb.y - pc.y);
        let detright = (pa.y - pc.y) * (pb.x - pc.x);
        let det = detleft - detright;

        // The errbound calculation was changed to require only one branch on the likely execution path.
        // This improves performance on modern processors as discussed by Ozaki et al in
        // https://doi.org/10.1007/s10543-015-0574-9
        // The underflow guard "+ u_N" was omitted because orient2dadapt(...) would not guarantee
        // correct results in cases of underflow, the derivation of THETA is given in the reference.
        let detsum = abs(detleft + detright);
        const THETA: f64 = 3.3306690621773722e-16;
        let errbound = THETA * detsum;
        if det >= errbound || -det >= errbound {
            det
        } else {
            orient2dadapt(pa, pb, pc, detsum)
        }
    }

    fn orient2dadapt(pa: Coord<f64>, pb: Coord<f64>, pc: Coord<f64>, detsum: f64) -> f64 {
        let acx = pa.x - pc.x;
        let bcx = pb.x - pc.x;
        let acy = pa.y - pc.y;
        let bcy = pb.y - pc.y;

        let (detleft, detlefttail) = two_product(acx, bcy);
        let (detright, detrighttail) = two_product(acy, bcx);

        let (B3, B2, B1, B0) = two_two_diff(detleft, detlefttail, detright, detrighttail);
        let B = [B0, B1, B2, B3];

        let mut det = estimate(&B);
        let errbound = CCWERRBOUND_B * detsum;
        if det >= errbound || (-det >= errbound) {
            return det;
        }

        let acxtail = two_diff_tail(pa.x, pc.x, acx);
        let bcxtail = two_diff_tail(pb.x, pc.x, bcx);
        let acytail = two_diff_tail(pa.y, pc.y, acy);
        let bcytail = two_diff_tail(pb.y, pc.y, bcy);

        if acxtail == 0.0 && acytail == 0.0 && bcxtail == 0.0 && bcytail == 0.0 {
            return det;
        }

        let errbound = CCWERRBOUND_C * detsum + RESULTERRBOUND * abs(det);
        det += (acx * bcytail + bcy * acxtail) - (acy * bcxtail + bcx * acytail);

        if det >= errbound || -det >= errbound {
            return det;
        }

        let (s1, s0) = two_product(acxtail, bcy);
        let (t1, t0) = two_product(acytail, bcx);
        let (u3, u2, u1, u0) = two_two_diff(s1, s0, t1, t0);
        let U = [u0, u1, u2, u3];

        let mut C1 = [0.0f64; 8];
        let c1length = fast_expansion_sum_zeroelim(&B, &U, &mut C1);

        let (s1, s0) = two_product(acx, bcytail);
        let (t1, t0) = two_product(acy, bcxtail);
        let (u3, u2, u1, u0) = two_two_diff(s1, s0, t1, t0);
        let U = [u0, u1, u2, u3];

        let mut C2 = [0.0f64; 12];
        let c2length = fast_expansion_sum_zeroelim(&C1[..c1length], &U, &mut C2);

        let (s1, s0) = two_product(acxtail, bcytail);
        let (t1, t0) = two_product(acytail, bcxtail);
        let (u3, u2, u1, u0) = two_two_diff(s1, s0, t1, t0);
        let U = [u0, u1, u2, u3];
        let mut D = [0.0f64; 16];
        let dlength = fast_expansion_sum_zeroelim(&C2[..c2length], &U, &mut D);
        D[dlength - 1]
    }

    /// Returns a positive value if the point `pd` lies below the plane passing through `pa`, `pb`, and `pc`
    /// ("below" is defined so that `pa`, `pb`, and `pc` appear in counterclockwise order when viewed from above the plane).  
    /// Returns a negative value if `pd` lies above the plane.  
    /// Returns `0` if they are **coplanar**.  
    pub fn orient3d<T: Into<f64>>(
        pa: Coord3D<T>,
        pb: Coord3D<T>,
        pc: Coord3D<T>,
        pd: Coord3D<T>,
    ) -> f64 {
        let pa = Coord3D {
            x: pa.x.into(),
            y: pa.y.into(),
            z: pa.z.into(),
        };
        let pb = Coord3D {
            x: pb.x.into(),
            y: pb.y.into(),
            z: pb.z.into(),
        };
        let pc = Coord3D {
            x: pc.x.into(),
            y: pc.y.into(),
            z: pc.z.into(),
        };
        let pd = Coord3D {
            x: pd.x.into(),
            y: pd.y.into(),
            z: pd.z.into(),
        };

        let adx = pa.x - pd.x;
        let bdx = pb.x - pd.x;
        let cdx = pc.x - pd.x;
        let ady = pa.y - pd.y;
        let bdy = pb.y - pd.y;
        let cdy = pc.y - pd.y;
        let adz = pa.z - pd.z;
        let bdz = pb.z - pd.z;
        let cdz = pc.z - pd.z;

        let bdxcdy = bdx * cdy;
        let cdxbdy = cdx * bdy;

        let cdxady = cdx * ady;
        let adxcdy = adx * cdy;

        let adxbdy = adx * bdy;
        let bdxady = bdx * ady;

        let det = adz * (bdxcdy - cdxbdy) + bdz * (cdxady - adxcdy) + cdz * (adxbdy - bdxady);

        let permanent = (abs(bdxcdy) + abs(cdxbdy)) * abs(adz)
            + (abs(cdxady) + abs(adxcdy)) * abs(bdz)
            + (abs(adxbdy) + abs(bdxady)) * abs(cdz);

        let errbound = O3DERRBOUND_A * permanent;
        if det > errbound || -det > errbound {
            return det;
        }

        orient3dadapt(pa, pb, pc, pd, permanent)
    }

    fn orient3dadapt(
        pa: Coord3D<f64>,
        pb: Coord3D<f64>,
        pc: Coord3D<f64>,
        pd: Coord3D<f64>,
        permanent: f64,
    ) -> f64 {
        let adx = pa.x - pd.x;
        let bdx = pb.x - pd.x;
        let cdx = pc.x - pd.x;
        let ady = pa.y - pd.y;
        let bdy = pb.y - pd.y;
        let cdy = pc.y - pd.y;
        let adz = pa.z - pd.z;
        let bdz = pb.z - pd.z;
        let cdz = pc.z - pd.z;

        let (bdxcdy1, bdxcdy0) = two_product(bdx, cdy);
        let (cdxbdy1, cdxbdy0) = two_product(cdx, bdy);
        let (bc3, bc2, bc1, bc0) = two_two_diff(bdxcdy1, bdxcdy0, cdxbdy1, cdxbdy0);
        let bc = [bc0, bc1, bc2, bc3];
        let mut adet = [0f64; 8];
        let alen = scale_expansion_zeroelim(&bc[..4], adz, &mut adet);

        let (cdxady1, cdxady0) = two_product(cdx, ady);
        let (adxcdy1, adxcdy0) = two_product(adx, cdy);
        let (ca3, ca2, ca1, ca0) = two_two_diff(cdxady1, cdxady0, adxcdy1, adxcdy0);
        let ca = [ca0, ca1, ca2, ca3];
        let mut bdet = [0f64; 8];
        let blen = scale_expansion_zeroelim(&ca[..4], bdz, &mut bdet);

        let (adxbdy1, adxbdy0) = two_product(adx, bdy);
        let (bdxady1, bdxady0) = two_product(bdx, ady);
        let (ab3, ab2, ab1, ab0) = two_two_diff(adxbdy1, adxbdy0, bdxady1, bdxady0);
        let ab = [ab0, ab1, ab2, ab3];
        let mut cdet = [0f64; 8];
        let clen = scale_expansion_zeroelim(&ab[..4], cdz, &mut cdet);

        let mut abdet = [0f64; 16];
        let ablen = fast_expansion_sum_zeroelim(&adet[..alen], &bdet[..blen], &mut abdet);
        let mut fin1 = [0f64; 192];
        let finlength = fast_expansion_sum_zeroelim(&abdet[..ablen], &cdet[..clen], &mut fin1);

        let mut det = estimate(&fin1[..finlength]);
        let mut errbound = O3DERRBOUND_B * permanent;
        if det >= errbound || -det >= errbound {
            return det;
        }

        let adxtail = two_diff_tail(pa.x, pd.x, adx);
        let bdxtail = two_diff_tail(pb.x, pd.x, bdx);
        let cdxtail = two_diff_tail(pc.x, pd.x, cdx);
        let adytail = two_diff_tail(pa.y, pd.y, ady);
        let bdytail = two_diff_tail(pb.y, pd.y, bdy);
        let cdytail = two_diff_tail(pc.y, pd.y, cdy);
        let adztail = two_diff_tail(pa.z, pd.z, adz);
        let bdztail = two_diff_tail(pb.z, pd.z, bdz);
        let cdztail = two_diff_tail(pc.z, pd.z, cdz);

        if (adxtail == 0.0)
            && (bdxtail == 0.0)
            && (cdxtail == 0.0)
            && (adytail == 0.0)
            && (bdytail == 0.0)
            && (cdytail == 0.0)
            && (adztail == 0.0)
            && (bdztail == 0.0)
            && (cdztail == 0.0)
        {
            return det;
        }

        errbound = O3DERRBOUND_C * permanent + RESULTERRBOUND * abs(det);
        det += (adz * ((bdx * cdytail + cdy * bdxtail) - (bdy * cdxtail + cdx * bdytail))
            + adztail * (bdx * cdy - bdy * cdx))
            + (bdz * ((cdx * adytail + ady * cdxtail) - (cdy * adxtail + adx * cdytail))
                + bdztail * (cdx * ady - cdy * adx))
            + (cdz * ((adx * bdytail + bdy * adxtail) - (ady * bdxtail + bdx * adytail))
                + cdztail * (adx * bdy - ady * bdx));
        if det >= errbound || -det >= errbound {
            return det;
        }

        let mut finnow = fin1;
        let mut finother = [0f64; 192];

        let mut at_b = [0f64; 4];
        let mut at_c = [0f64; 4];
        let mut bt_c = [0f64; 4];
        let mut bt_a = [0f64; 4];
        let mut ct_a = [0f64; 4];
        let mut ct_b = [0f64; 4];
        let at_blen: usize;
        let at_clen: usize;
        let bt_clen: usize;
        let bt_alen: usize;
        let ct_alen: usize;
        let ct_blen: usize;
        if adxtail == 0.0 {
            if adytail == 0.0 {
                at_b[0] = 0.0;
                at_blen = 1;
                at_c[0] = 0.0;
                at_clen = 1;
            } else {
                let negate = -adytail;
                (at_b[1], at_b[0]) = two_product(negate, bdx);
                at_blen = 2;
                (at_c[1], at_c[0]) = two_product(adytail, cdx);
                at_clen = 2;
            }
        } else if adytail == 0.0 {
            (at_b[1], at_b[0]) = two_product(adxtail, bdy);
            at_blen = 2;
            let negate = -adxtail;
            (at_c[1], at_c[0]) = two_product(negate, cdy);
            at_clen = 2;
        } else {
            let (adxt_bdy1, adxt_bdy0) = two_product(adxtail, bdy);
            let (adyt_bdx1, adyt_bdx0) = two_product(adytail, bdx);
            (at_b[3], at_b[2], at_b[1], at_b[0]) =
                two_two_diff(adxt_bdy1, adxt_bdy0, adyt_bdx1, adyt_bdx0);
            at_blen = 4;
            let (adyt_cdx1, adyt_cdx0) = two_product(adytail, cdx);
            let (adxt_cdy1, adxt_cdy0) = two_product(adxtail, cdy);
            (at_c[3], at_c[2], at_c[1], at_c[0]) =
                two_two_diff(adyt_cdx1, adyt_cdx0, adxt_cdy1, adxt_cdy0);
            at_clen = 4;
        }
        if bdxtail == 0.0 {
            if bdytail == 0.0 {
                bt_c[0] = 0.0;
                bt_clen = 1;
                bt_a[0] = 0.0;
                bt_alen = 1;
            } else {
                let negate = -bdytail;
                (bt_c[1], bt_c[0]) = two_product(negate, cdx);
                bt_clen = 2;
                (bt_a[1], bt_a[0]) = two_product(bdytail, adx);
                bt_alen = 2;
            }
        } else if bdytail == 0.0 {
            (bt_c[1], bt_c[0]) = two_product(bdxtail, cdy);
            bt_clen = 2;
            let negate = -bdxtail;
            (bt_a[1], bt_a[0]) = two_product(negate, ady);
            bt_alen = 2;
        } else {
            let (bdxt_cdy1, bdxt_cdy0) = two_product(bdxtail, cdy);
            let (bdyt_cdx1, bdyt_cdx0) = two_product(bdytail, cdx);
            (bt_c[3], bt_c[2], bt_c[1], bt_c[0]) =
                two_two_diff(bdxt_cdy1, bdxt_cdy0, bdyt_cdx1, bdyt_cdx0);
            bt_clen = 4;
            let (bdyt_adx1, bdyt_adx0) = two_product(bdytail, adx);
            let (bdxt_ady1, bdxt_ady0) = two_product(bdxtail, ady);
            (bt_a[3], bt_a[2], bt_a[1], bt_a[0]) =
                two_two_diff(bdyt_adx1, bdyt_adx0, bdxt_ady1, bdxt_ady0);
            bt_alen = 4;
        }
        if cdxtail == 0.0 {
            if cdytail == 0.0 {
                ct_a[0] = 0.0;
                ct_alen = 1;
                ct_b[0] = 0.0;
                ct_blen = 1;
            } else {
                let negate = -cdytail;
                (ct_a[1], ct_a[0]) = two_product(negate, adx);
                ct_alen = 2;
                (ct_b[1], ct_b[0]) = two_product(cdytail, bdx);
                ct_blen = 2;
            }
        } else if cdytail == 0.0 {
            (ct_a[1], ct_a[0]) = two_product(cdxtail, ady);
            ct_alen = 2;
            let negate = -cdxtail;
            (ct_b[1], ct_b[0]) = two_product(negate, bdy);
            ct_blen = 2;
        } else {
            let (cdxt_ady1, cdxt_ady0) = two_product(cdxtail, ady);
            let (cdyt_adx1, cdyt_adx0) = two_product(cdytail, adx);
            (ct_a[3], ct_a[2], ct_a[1], ct_a[0]) =
                two_two_diff(cdxt_ady1, cdxt_ady0, cdyt_adx1, cdyt_adx0);
            ct_alen = 4;
            let (cdyt_bdx1, cdyt_bdx0) = two_product(cdytail, bdx);
            let (cdxt_bdy1, cdxt_bdy0) = two_product(cdxtail, bdy);
            (ct_b[3], ct_b[2], ct_b[1], ct_b[0]) =
                two_two_diff(cdyt_bdx1, cdyt_bdx0, cdxt_bdy1, cdxt_bdy0);
            ct_blen = 4;
        }

        let mut bct = [0f64; 8];
        let mut cat = [0f64; 8];
        let mut abt = [0f64; 8];
        let mut u = [0f64; 4];
        let mut v = [0f64; 12];
        let mut w = [0f64; 16];
        let mut vlength: usize;
        let mut wlength: usize;

        let bctlen = fast_expansion_sum_zeroelim(&bt_c[..bt_clen], &ct_b[..ct_blen], &mut bct);
        wlength = scale_expansion_zeroelim(&bct[..bctlen], adz, &mut w);
        let mut finlength =
            fast_expansion_sum_zeroelim(&finnow[..finlength], &w[..wlength], &mut finother);
        ::core::mem::swap(&mut finnow, &mut finother);

        let catlen = fast_expansion_sum_zeroelim(&ct_a[..ct_alen], &at_c[..at_clen], &mut cat);
        wlength = scale_expansion_zeroelim(&cat[..catlen], bdz, &mut w);
        finlength = fast_expansion_sum_zeroelim(&finnow[..finlength], &w[..wlength], &mut finother);
        ::core::mem::swap(&mut finnow, &mut finother);

        let abtlen = fast_expansion_sum_zeroelim(&at_b[..at_blen], &bt_a[..bt_alen], &mut abt);
        wlength = scale_expansion_zeroelim(&abt[..abtlen], cdz, &mut w);
        finlength = fast_expansion_sum_zeroelim(&finnow[..finlength], &w[..wlength], &mut finother);
        ::core::mem::swap(&mut finnow, &mut finother);

        if adztail != 0.0 {
            vlength = scale_expansion_zeroelim(&bc[..4], adztail, &mut v);
            finlength =
                fast_expansion_sum_zeroelim(&finnow[..finlength], &v[..vlength], &mut finother);
            ::core::mem::swap(&mut finnow, &mut finother);
        }
        if bdztail != 0.0 {
            vlength = scale_expansion_zeroelim(&ca[..4], bdztail, &mut v);
            finlength =
                fast_expansion_sum_zeroelim(&finnow[..finlength], &v[..vlength], &mut finother);
            ::core::mem::swap(&mut finnow, &mut finother);
        }
        if cdztail != 0.0 {
            vlength = scale_expansion_zeroelim(&ab[..4], cdztail, &mut v);
            finlength =
                fast_expansion_sum_zeroelim(&finnow[..finlength], &v[..vlength], &mut finother);
            ::core::mem::swap(&mut finnow, &mut finother);
        }

        if adxtail != 0.0 {
            if bdytail != 0.0 {
                let (adxt_bdyt1, adxt_bdyt0) = two_product(adxtail, bdytail);
                (u[3], u[2], u[1], u[0]) = two_one_product(adxt_bdyt1, adxt_bdyt0, cdz);
                finlength =
                    fast_expansion_sum_zeroelim(&finnow[..finlength], &u[..4], &mut finother);
                ::core::mem::swap(&mut finnow, &mut finother);
                if cdztail != 0.0 {
                    (u[3], u[2], u[1], u[0]) = two_one_product(adxt_bdyt1, adxt_bdyt0, cdztail);
                    finlength =
                        fast_expansion_sum_zeroelim(&finnow[..finlength], &u[..4], &mut finother);
                    ::core::mem::swap(&mut finnow, &mut finother);
                }
            }
            if cdytail != 0.0 {
                let negate = -adxtail;
                let (adxt_cdyt1, adxt_cdyt0) = two_product(negate, cdytail);
                (u[3], u[2], u[1], u[0]) = two_one_product(adxt_cdyt1, adxt_cdyt0, bdz);
                finlength =
                    fast_expansion_sum_zeroelim(&finnow[..finlength], &u[..4], &mut finother);
                ::core::mem::swap(&mut finnow, &mut finother);
                if bdztail != 0.0 {
                    (u[3], u[2], u[1], u[0]) = two_one_product(adxt_cdyt1, adxt_cdyt0, bdztail);
                    finlength =
                        fast_expansion_sum_zeroelim(&finnow[..finlength], &u[..4], &mut finother);
                    ::core::mem::swap(&mut finnow, &mut finother);
                }
            }
        }
        if bdxtail != 0.0 {
            if cdytail != 0.0 {
                let (bdxt_cdyt1, bdxt_cdyt0) = two_product(bdxtail, cdytail);
                (u[3], u[2], u[1], u[0]) = two_one_product(bdxt_cdyt1, bdxt_cdyt0, adz);
                finlength =
                    fast_expansion_sum_zeroelim(&finnow[..finlength], &u[..4], &mut finother);
                ::core::mem::swap(&mut finnow, &mut finother);
                if adztail != 0.0 {
                    (u[3], u[2], u[1], u[0]) = two_one_product(bdxt_cdyt1, bdxt_cdyt0, adztail);
                    finlength =
                        fast_expansion_sum_zeroelim(&finnow[..finlength], &u[..4], &mut finother);
                    ::core::mem::swap(&mut finnow, &mut finother);
                }
            }
            if adytail != 0.0 {
                let negate = -bdxtail;
                let (bdxt_adyt1, bdxt_adyt0) = two_product(negate, adytail);
                (u[3], u[2], u[1], u[0]) = two_one_product(bdxt_adyt1, bdxt_adyt0, cdz);
                finlength =
                    fast_expansion_sum_zeroelim(&finnow[..finlength], &u[..4], &mut finother);
                ::core::mem::swap(&mut finnow, &mut finother);
                if cdztail != 0.0 {
                    (u[3], u[2], u[1], u[0]) = two_one_product(bdxt_adyt1, bdxt_adyt0, cdztail);
                    finlength =
                        fast_expansion_sum_zeroelim(&finnow[..finlength], &u[..4], &mut finother);
                    ::core::mem::swap(&mut finnow, &mut finother);
                }
            }
        }
        if cdxtail != 0.0 {
            if adytail != 0.0 {
                let (cdxt_adyt1, cdxt_adyt0) = two_product(cdxtail, adytail);
                (u[3], u[2], u[1], u[0]) = two_one_product(cdxt_adyt1, cdxt_adyt0, bdz);
                finlength =
                    fast_expansion_sum_zeroelim(&finnow[..finlength], &u[..4], &mut finother);
                ::core::mem::swap(&mut finnow, &mut finother);
                if bdztail != 0.0 {
                    (u[3], u[2], u[1], u[0]) = two_one_product(cdxt_adyt1, cdxt_adyt0, bdztail);
                    finlength =
                        fast_expansion_sum_zeroelim(&finnow[..finlength], &u[..4], &mut finother);
                    ::core::mem::swap(&mut finnow, &mut finother);
                }
            }
            if bdytail != 0.0 {
                let negate = -cdxtail;
                let (cdxt_bdyt1, cdxt_bdyt0) = two_product(negate, bdytail);
                (u[3], u[2], u[1], u[0]) = two_one_product(cdxt_bdyt1, cdxt_bdyt0, adz);
                finlength =
                    fast_expansion_sum_zeroelim(&finnow[..finlength], &u[..4], &mut finother);
                ::core::mem::swap(&mut finnow, &mut finother);
                if adztail != 0.0 {
                    (u[3], u[2], u[1], u[0]) = two_one_product(cdxt_bdyt1, cdxt_bdyt0, adztail);
                    finlength =
                        fast_expansion_sum_zeroelim(&finnow[..finlength], &u[..4], &mut finother);
                    ::core::mem::swap(&mut finnow, &mut finother);
                }
            }
        }

        if adztail != 0.0 {
            wlength = scale_expansion_zeroelim(&bct[..bctlen], adztail, &mut w);
            finlength =
                fast_expansion_sum_zeroelim(&finnow[..finlength], &w[..wlength], &mut finother);
            ::core::mem::swap(&mut finnow, &mut finother);
        }
        if bdztail != 0.0 {
            wlength = scale_expansion_zeroelim(&cat[..catlen], bdztail, &mut w);
            finlength =
                fast_expansion_sum_zeroelim(&finnow[..finlength], &w[..wlength], &mut finother);
            ::core::mem::swap(&mut finnow, &mut finother);
        }
        if cdztail != 0.0 {
            wlength = scale_expansion_zeroelim(&abt[..abtlen], cdztail, &mut w);
            finlength =
                fast_expansion_sum_zeroelim(&finnow[..finlength], &w[..wlength], &mut finother);
            ::core::mem::swap(&mut finnow, &mut finother);
        }

        finnow[finlength - 1]
    }

    /// Returns a positive value if the coordinate `pd` lies **inside** the circle passing through `pa`, `pb`, and `pc`.  
    /// Returns a negative value if it lies **outside** the circle.  
    /// Returns `0` if the four points are **cocircular**.  
    /// **Note**: The points `pa`, `pb`, and `pc` must be in **counterclockwise order**, or the sign of the result will be reversed.
    pub fn incircle<T: Into<f64>>(pa: Coord<T>, pb: Coord<T>, pc: Coord<T>, pd: Coord<T>) -> f64 {
        let pa = Coord {
            x: pa.x.into(),
            y: pa.y.into(),
        };
        let pb = Coord {
            x: pb.x.into(),
            y: pb.y.into(),
        };
        let pc = Coord {
            x: pc.x.into(),
            y: pc.y.into(),
        };
        let pd = Coord {
            x: pd.x.into(),
            y: pd.y.into(),
        };

        let adx = pa.x - pd.x;
        let bdx = pb.x - pd.x;
        let cdx = pc.x - pd.x;
        let ady = pa.y - pd.y;
        let bdy = pb.y - pd.y;
        let cdy = pc.y - pd.y;

        let bdxcdy = bdx * cdy;
        let cdxbdy = cdx * bdy;
        let alift = adx * adx + ady * ady;

        let cdxady = cdx * ady;
        let adxcdy = adx * cdy;
        let blift = bdx * bdx + bdy * bdy;

        let adxbdy = adx * bdy;
        let bdxady = bdx * ady;
        let clift = cdx * cdx + cdy * cdy;

        let det = alift * (bdxcdy - cdxbdy) + blift * (cdxady - adxcdy) + clift * (adxbdy - bdxady);

        let permanent = (abs(bdxcdy) + abs(cdxbdy)) * alift
            + (abs(cdxady) + abs(adxcdy)) * blift
            + (abs(adxbdy) + abs(bdxady)) * clift;
        let errbound = ICCERRBOUND_A * permanent;
        if det > errbound || -det > errbound {
            return det;
        }
        incircleadapt(pa, pb, pc, pd, permanent)
    }

    fn incircleadapt(
        pa: Coord<f64>,
        pb: Coord<f64>,
        pc: Coord<f64>,
        pd: Coord<f64>,
        permanent: f64,
    ) -> f64 {
        let mut temp8 = [0f64; 8];
        let mut temp16a = [0f64; 16];
        let mut temp16b = [0f64; 16];
        let mut temp16c = [0f64; 16];
        let mut temp32a = [0f64; 32];
        let mut temp32b = [0f64; 32];
        let mut temp48 = [0f64; 48];
        let mut temp64 = [0f64; 64];

        let adx = pa.x - pd.x;
        let bdx = pb.x - pd.x;
        let cdx = pc.x - pd.x;
        let ady = pa.y - pd.y;
        let bdy = pb.y - pd.y;
        let cdy = pc.y - pd.y;

        let (bdxcdy1, bdxcdy0) = two_product(bdx, cdy);
        let (cdxbdy1, cdxbdy0) = two_product(cdx, bdy);
        let (bc3, bc2, bc1, bc0) = two_two_diff(bdxcdy1, bdxcdy0, cdxbdy1, cdxbdy0);
        let bc = [bc0, bc1, bc2, bc3];

        let mut axbc = [0f64; 8];
        let axbclen = scale_expansion_zeroelim(&bc, adx, &mut axbc);
        let mut axxbc = [0f64; 16];
        let axxbclen = scale_expansion_zeroelim(&axbc[..axbclen], adx, &mut axxbc);
        let mut aybc = [0f64; 8];
        let aybclen = scale_expansion_zeroelim(&bc, ady, &mut aybc);
        let mut ayybc = [0f64; 16];
        let ayybclen = scale_expansion_zeroelim(&aybc[..aybclen], ady, &mut ayybc);
        let mut adet = [0f64; 32];
        let alen = fast_expansion_sum_zeroelim(&axxbc[0..axxbclen], &ayybc[0..ayybclen], &mut adet);

        let (cdxady1, cdxady0) = two_product(cdx, ady);
        let (adxcdy1, adxcdy0) = two_product(adx, cdy);
        let (c3, c2, c1, c0) = two_two_diff(cdxady1, cdxady0, adxcdy1, adxcdy0);
        let ca = [c0, c1, c2, c3];

        let mut bxca = [0f64; 8];
        let bxcalen = scale_expansion_zeroelim(&ca, bdx, &mut bxca);
        let mut bxxca = [0f64; 16];
        let bxxcalen = scale_expansion_zeroelim(&bxca[..bxcalen], bdx, &mut bxxca);
        let mut byca = [0f64; 8];
        let bycalen = scale_expansion_zeroelim(&ca, bdy, &mut byca);
        let mut byyca = [0f64; 16];
        let byycalen = scale_expansion_zeroelim(&byca[..bycalen], bdy, &mut byyca);
        let mut bdet = [0f64; 32];
        let blen = fast_expansion_sum_zeroelim(&bxxca[..bxxcalen], &byyca[0..byycalen], &mut bdet);

        let (adxbdy1, adxbdy0) = two_product(adx, bdy);
        let (bdxady1, bdxady0) = two_product(bdx, ady);
        let (ab3, ab2, ab1, ab0) = two_two_diff(adxbdy1, adxbdy0, bdxady1, bdxady0);
        let ab = [ab0, ab1, ab2, ab3];

        let mut cxab = [0f64; 8];
        let cxablen = scale_expansion_zeroelim(&ab, cdx, &mut cxab);
        let mut cxxab = [0f64; 16];
        let cxxablen = scale_expansion_zeroelim(&cxab[..cxablen], cdx, &mut cxxab);
        let mut cyab = [0f64; 8];
        let cyablen = scale_expansion_zeroelim(&ab, cdy, &mut cyab);
        let mut cyyab = [0f64; 16];
        let cyyablen = scale_expansion_zeroelim(&cyab[..cyablen], cdy, &mut cyyab);
        let mut cdet = [0f64; 32];
        let clen = fast_expansion_sum_zeroelim(&cxxab[..cxxablen], &cyyab[..cyyablen], &mut cdet);

        let mut abdet = [0f64; 64];
        let ablen = fast_expansion_sum_zeroelim(&adet[..alen], &bdet[..blen], &mut abdet);
        let mut fin1 = [0f64; 1152];
        let mut finlength = fast_expansion_sum_zeroelim(&abdet[..ablen], &cdet[..clen], &mut fin1);

        let mut det = estimate(&fin1[..finlength]);
        let errbound = ICCERRBOUND_B * permanent;
        if det >= errbound || -det >= errbound {
            return det;
        }

        let adxtail = two_diff_tail(pa.x, pd.x, adx);
        let adytail = two_diff_tail(pa.y, pd.y, ady);
        let bdxtail = two_diff_tail(pb.x, pd.x, bdx);
        let bdytail = two_diff_tail(pb.y, pd.y, bdy);
        let cdxtail = two_diff_tail(pc.x, pd.x, cdx);
        let cdytail = two_diff_tail(pc.y, pd.y, cdy);
        if adxtail == 0.0
            && bdxtail == 0.0
            && cdxtail == 0.0
            && adytail == 0.0
            && bdytail == 0.0
            && cdytail == 0.0
        {
            return det;
        }

        let errbound = ICCERRBOUND_C * permanent + RESULTERRBOUND * abs(det);
        det += ((adx * adx + ady * ady)
            * ((bdx * cdytail + cdy * bdxtail) - (bdy * cdxtail + cdx * bdytail))
            + 2.0 * (adx * adxtail + ady * adytail) * (bdx * cdy - bdy * cdx))
            + ((bdx * bdx + bdy * bdy)
                * ((cdx * adytail + ady * cdxtail) - (cdy * adxtail + adx * cdytail))
                + 2.0 * (bdx * bdxtail + bdy * bdytail) * (cdx * ady - cdy * adx))
            + ((cdx * cdx + cdy * cdy)
                * ((adx * bdytail + bdy * adxtail) - (ady * bdxtail + bdx * adytail))
                + 2.0 * (cdx * cdxtail + cdy * cdytail) * (adx * bdy - ady * bdx));

        if det >= errbound || -det >= errbound {
            return det;
        }

        let mut fin2 = [0f64; 1152];

        let aa = if bdxtail != 0.0 || bdytail != 0.0 || cdxtail != 0.0 || cdytail != 0.0 {
            let (adxadx1, adxadx0) = square(adx);
            let (adyady1, adyady0) = square(ady);
            let (aa3, aa2, aa1, aa0) = two_two_sum(adxadx1, adxadx0, adyady1, adyady0);
            [aa0, aa1, aa2, aa3]
        } else {
            [0f64; 4]
        };

        let bb = if cdxtail != 0.0 || cdytail != 0.0 || adxtail != 0.0 || adytail != 0.0 {
            let (bdxbdx1, bdxbdx0) = square(bdx);
            let (bdybdy1, bdybdy0) = square(bdy);
            let (bb3, bb2, bb1, bb0) = two_two_sum(bdxbdx1, bdxbdx0, bdybdy1, bdybdy0);
            [bb0, bb1, bb2, bb3]
        } else {
            [0f64; 4]
        };

        let cc = if adxtail != 0.0 || adytail != 0.0 || bdxtail != 0.0 || bdytail != 0.0 {
            let (cdxcdx1, cdxcdx0) = square(cdx);
            let (cdycdy1, cdycdy0) = square(cdy);
            let (cc3, cc2, cc1, cc0) = two_two_sum(cdxcdx1, cdxcdx0, cdycdy1, cdycdy0);
            [cc0, cc1, cc2, cc3]
        } else {
            [0f64; 4]
        };

        let mut axtbclen = 9;
        let mut axtbc = [0f64; 8];
        if adxtail != 0.0 {
            axtbclen = scale_expansion_zeroelim(&bc, adxtail, &mut axtbc);
            let mut temp16a = [0f64; 16];
            let temp16alen = scale_expansion_zeroelim(&axtbc[..axtbclen], 2.0 * adx, &mut temp16a);

            let mut axtcc = [0f64; 8];
            let axtcclen = scale_expansion_zeroelim(&cc, adxtail, &mut axtcc);
            let mut temp16b = [0f64; 16];
            let temp16blen = scale_expansion_zeroelim(&axtcc[..axtcclen], bdy, &mut temp16b);

            let mut axtbb = [0f64; 8];
            let axtbblen = scale_expansion_zeroelim(&bb, adxtail, &mut axtbb);
            let mut temp16c = [0f64; 16];
            let temp16clen = scale_expansion_zeroelim(&axtbb[..axtbblen], -cdy, &mut temp16c);

            let mut temp32a = [0f64; 32];
            let temp32alen = fast_expansion_sum_zeroelim(
                &temp16a[..temp16alen],
                &temp16b[..temp16blen],
                &mut temp32a,
            );
            let mut temp48 = [0f64; 48];
            let temp48len = fast_expansion_sum_zeroelim(
                &temp16c[..temp16clen],
                &temp32a[..temp32alen],
                &mut temp48,
            );
            finlength =
                fast_expansion_sum_zeroelim(&fin1[..finlength], &temp48[..temp48len], &mut fin2);
            ::core::mem::swap(&mut fin1, &mut fin2)
        }

        let mut aytbclen = 9;
        let mut aytbc = [0f64; 8];
        if adytail != 0.0 {
            aytbclen = scale_expansion_zeroelim(&bc, adytail, &mut aytbc);
            let temp16alen = scale_expansion_zeroelim(&aytbc[..aytbclen], 2.0 * ady, &mut temp16a);

            let mut aytbb = [0f64; 8];
            let aytbblen = scale_expansion_zeroelim(&bb, adytail, &mut aytbb);
            let temp16blen = scale_expansion_zeroelim(&aytbb[..aytbblen], cdx, &mut temp16b);

            let mut aytcc = [0f64; 8];
            let aytcclen = scale_expansion_zeroelim(&cc, adytail, &mut aytcc);
            let temp16clen = scale_expansion_zeroelim(&aytcc[..aytcclen], -bdx, &mut temp16c);

            let temp32alen = fast_expansion_sum_zeroelim(
                &temp16a[..temp16alen],
                &temp16b[..temp16blen],
                &mut temp32a,
            );

            let temp48len = fast_expansion_sum_zeroelim(
                &temp16c[..temp16clen],
                &temp32a[..temp32alen],
                &mut temp48,
            );
            finlength =
                fast_expansion_sum_zeroelim(&fin1[..finlength], &temp48[..temp48len], &mut fin2);
            ::core::mem::swap(&mut fin1, &mut fin2)
        }

        let mut bxtcalen = 9;
        let mut bxtca = [0f64; 8];
        if bdxtail != 0.0 {
            bxtcalen = scale_expansion_zeroelim(&ca, bdxtail, &mut bxtca);
            let temp16alen = scale_expansion_zeroelim(&bxtca[..bxtcalen], 2.0 * bdx, &mut temp16a);

            let mut bxtaa = [0f64; 8];
            let bxtaalen = scale_expansion_zeroelim(&aa, bdxtail, &mut bxtaa);
            let temp16blen = scale_expansion_zeroelim(&bxtaa[..bxtaalen], cdy, &mut temp16b);

            let mut bxtcc = [0f64; 8];
            let bxtcclen = scale_expansion_zeroelim(&cc, bdxtail, &mut bxtcc);
            let temp16clen = scale_expansion_zeroelim(&bxtcc[..bxtcclen], -ady, &mut temp16c);

            let temp32alen = fast_expansion_sum_zeroelim(
                &temp16a[..temp16alen],
                &temp16b[..temp16blen],
                &mut temp32a,
            );
            let temp48len = fast_expansion_sum_zeroelim(
                &temp16c[..temp16clen],
                &temp32a[..temp32alen],
                &mut temp48,
            );
            finlength =
                fast_expansion_sum_zeroelim(&fin1[..finlength], &temp48[..temp48len], &mut fin2);
            ::core::mem::swap(&mut fin1, &mut fin2)
        }

        let mut bytcalen = 9;
        let mut bytca = [0f64; 8];
        if bdytail != 0.0 {
            bytcalen = scale_expansion_zeroelim(&ca, bdytail, &mut bytca);
            let temp16alen = scale_expansion_zeroelim(&bytca[..bytcalen], 2.0 * bdy, &mut temp16a);

            let mut bytcc = [0f64; 8];
            let bytcclen = scale_expansion_zeroelim(&cc, bdytail, &mut bytcc);
            let temp16blen = scale_expansion_zeroelim(&bytcc[..bytcclen], adx, &mut temp16b);

            let mut bytaa = [0f64; 8];
            let bytaalen = scale_expansion_zeroelim(&aa, bdytail, &mut bytaa);
            let temp16clen = scale_expansion_zeroelim(&bytaa[..bytaalen], -cdx, &mut temp16c);

            let temp32alen = fast_expansion_sum_zeroelim(
                &temp16a[..temp16alen],
                &temp16b[..temp16blen],
                &mut temp32a,
            );
            let temp48len = fast_expansion_sum_zeroelim(
                &temp16c[..temp16clen],
                &temp32a[..temp32alen],
                &mut temp48,
            );

            finlength =
                fast_expansion_sum_zeroelim(&fin1[..finlength], &temp48[..temp48len], &mut fin2);
            ::core::mem::swap(&mut fin1, &mut fin2)
        }

        let mut cxtab = [0f64; 8];
        let mut cxtablen = 9;
        if cdxtail != 0.0 {
            cxtablen = scale_expansion_zeroelim(&ab, cdxtail, &mut cxtab);
            let temp16alen = scale_expansion_zeroelim(&cxtab[..cxtablen], 2.0 * cdx, &mut temp16a);

            let mut cxtbb = [0f64; 8];
            let cxtbblen = scale_expansion_zeroelim(&bb, cdxtail, &mut cxtbb);
            let temp16blen = scale_expansion_zeroelim(&cxtbb[..cxtbblen], ady, &mut temp16b);

            let mut cxtaa = [0f64; 8];
            let cxtaalen = scale_expansion_zeroelim(&aa, cdxtail, &mut cxtaa);
            let temp16clen = scale_expansion_zeroelim(&cxtaa[..cxtaalen], -bdy, &mut temp16c);

            let temp32alen = fast_expansion_sum_zeroelim(
                &temp16a[..temp16alen],
                &temp16b[..temp16blen],
                &mut temp32a,
            );
            let temp48len = fast_expansion_sum_zeroelim(
                &temp16c[..temp16clen],
                &temp32a[..temp32alen],
                &mut temp48,
            );
            finlength =
                fast_expansion_sum_zeroelim(&fin1[..finlength], &temp48[..temp48len], &mut fin2);
            ::core::mem::swap(&mut fin1, &mut fin2);
        }

        let mut cytab = [0f64; 8];
        let mut cytablen = 9;
        if cdytail != 0.0 {
            cytablen = scale_expansion_zeroelim(&ab, cdytail, &mut cytab);
            let temp16alen = scale_expansion_zeroelim(&cytab[..cytablen], 2.0 * cdy, &mut temp16a);

            let mut cytaa = [0f64; 8];
            let cytaalen = scale_expansion_zeroelim(&aa, cdytail, &mut cytaa);
            let temp16blen = scale_expansion_zeroelim(&cytaa[..cytaalen], bdx, &mut temp16b);

            let mut cytbb = [0f64; 8];
            let cytbblen = scale_expansion_zeroelim(&bb, cdytail, &mut cytbb);
            let temp16clen = scale_expansion_zeroelim(&cytbb[..cytbblen], -adx, &mut temp16c);

            let temp32alen = fast_expansion_sum_zeroelim(
                &temp16a[..temp16alen],
                &temp16b[..temp16blen],
                &mut temp32a,
            );
            let temp48len = fast_expansion_sum_zeroelim(
                &temp16c[..temp16clen],
                &temp32a[..temp32alen],
                &mut temp48,
            );
            finlength =
                fast_expansion_sum_zeroelim(&fin1[..finlength], &temp48[..temp48len], &mut fin2);
            ::core::mem::swap(&mut fin1, &mut fin2);
        }

        if adxtail != 0.0 || adytail != 0.0 {
            let mut bctt = [0f64; 4];
            let mut bct = [0f64; 8];
            let bcttlen;
            let bctlen;
            if bdxtail != 0.0 || bdytail != 0.0 || cdxtail != 0.0 || cdytail != 0.0 {
                let (ti1, ti0) = two_product(bdxtail, cdy);
                let (tj1, tj0) = two_product(bdx, cdytail);
                let (u3, u2, u1, u0) = two_two_sum(ti1, ti0, tj1, tj0);
                let u = [u0, u1, u2, u3];
                let negate = -bdy;
                let (ti1, ti0) = two_product(cdxtail, negate);
                let negate = -bdytail;
                let (tj1, tj0) = two_product(cdx, negate);
                let (v3, v2, v1, v0) = two_two_sum(ti1, ti0, tj1, tj0);
                let v = [v0, v1, v2, v3];
                bctlen = fast_expansion_sum_zeroelim(&u, &v, &mut bct);
                let (ti1, ti0) = two_product(bdxtail, cdytail);
                let (tj1, tj0) = two_product(cdxtail, bdytail);
                let (bctt3, bctt2, bctt1, bctt0) = two_two_diff(ti1, ti0, tj1, tj0);
                bctt = [bctt0, bctt1, bctt2, bctt3];
                bcttlen = 4;
            } else {
                bct[0] = 0.0;
                bctlen = 1;
                bctt[0] = 0.0;
                bcttlen = 1;
            }

            if adxtail != 0.0 {
                let temp16alen =
                    scale_expansion_zeroelim(&axtbc[..axtbclen], adxtail, &mut temp16a);
                let mut axtbct = [0f64; 16];
                let axtbctlen = scale_expansion_zeroelim(&bct[..bctlen], adxtail, &mut axtbct);
                let temp32alen =
                    scale_expansion_zeroelim(&axtbct[..axtbctlen], 2.0 * adx, &mut temp32a);
                let temp48len = fast_expansion_sum_zeroelim(
                    &temp16a[..temp16alen],
                    &temp32a[..temp32alen],
                    &mut temp48,
                );
                finlength = fast_expansion_sum_zeroelim(
                    &fin1[..finlength],
                    &temp48[..temp48len],
                    &mut fin2,
                );
                ::core::mem::swap(&mut fin1, &mut fin2);

                if bdytail != 0.0 {
                    let temp8len = scale_expansion_zeroelim(&cc, adxtail, &mut temp8);
                    let temp16alen =
                        scale_expansion_zeroelim(&temp8[..temp8len], bdytail, &mut temp16a);
                    finlength = fast_expansion_sum_zeroelim(
                        &fin1[..finlength],
                        &temp16a[..temp16alen],
                        &mut fin2,
                    );
                    ::core::mem::swap(&mut fin1, &mut fin2);
                }
                if cdytail != 0.0 {
                    let temp8len = scale_expansion_zeroelim(&bb, -adxtail, &mut temp8);
                    let temp16alen =
                        scale_expansion_zeroelim(&temp8[..temp8len], cdytail, &mut temp16a);
                    finlength = fast_expansion_sum_zeroelim(
                        &fin1[..finlength],
                        &temp16a[..temp16alen],
                        &mut fin2,
                    );
                    ::core::mem::swap(&mut fin1, &mut fin2);
                }

                let temp32alen =
                    scale_expansion_zeroelim(&axtbct[..axtbctlen], adxtail, &mut temp32a);
                let mut axtbctt = [0f64; 8];
                let axtbcttlen = scale_expansion_zeroelim(&bctt[..bcttlen], adxtail, &mut axtbctt);
                let temp16alen =
                    scale_expansion_zeroelim(&axtbctt[..axtbcttlen], 2.0 * adx, &mut temp16a);
                let temp16blen =
                    scale_expansion_zeroelim(&axtbctt[..axtbcttlen], adxtail, &mut temp16b);
                let temp32blen = fast_expansion_sum_zeroelim(
                    &temp16a[..temp16alen],
                    &temp16b[..temp16blen],
                    &mut temp32b,
                );
                let temp64len = fast_expansion_sum_zeroelim(
                    &temp32a[..temp32alen],
                    &temp32b[..temp32blen],
                    &mut temp64,
                );
                finlength = fast_expansion_sum_zeroelim(
                    &fin1[..finlength],
                    &temp64[..temp64len],
                    &mut fin2,
                );
                ::core::mem::swap(&mut fin1, &mut fin2);
            }

            if adytail != 0.0 {
                let temp16alen =
                    scale_expansion_zeroelim(&aytbc[..aytbclen], adytail, &mut temp16a);
                let mut aytbct = [0f64; 16];
                let aytbctlen = scale_expansion_zeroelim(&bct[..bctlen], adytail, &mut aytbct);
                let temp32alen =
                    scale_expansion_zeroelim(&aytbct[..aytbctlen], 2.0 * ady, &mut temp32a);
                let temp48len = fast_expansion_sum_zeroelim(
                    &temp16a[..temp16alen],
                    &temp32a[..temp32alen],
                    &mut temp48,
                );
                finlength = fast_expansion_sum_zeroelim(
                    &fin1[..finlength],
                    &temp48[..temp48len],
                    &mut fin2,
                );
                ::core::mem::swap(&mut fin1, &mut fin2);

                let temp32alen =
                    scale_expansion_zeroelim(&aytbct[..aytbctlen], adytail, &mut temp32a);
                let mut aytbctt = [0f64; 8];
                let aytbcttlen = scale_expansion_zeroelim(&bctt[..bcttlen], adytail, &mut aytbctt);
                let temp16alen =
                    scale_expansion_zeroelim(&aytbctt[..aytbcttlen], 2.0 * ady, &mut temp16a);
                let temp16blen =
                    scale_expansion_zeroelim(&aytbctt[..aytbcttlen], adytail, &mut temp16b);
                let temp32blen = fast_expansion_sum_zeroelim(
                    &temp16a[..temp16alen],
                    &temp16b[..temp16blen],
                    &mut temp32b,
                );
                let temp64len = fast_expansion_sum_zeroelim(
                    &temp32a[..temp32alen],
                    &temp32b[..temp32blen],
                    &mut temp64,
                );
                finlength = fast_expansion_sum_zeroelim(
                    &fin1[..finlength],
                    &temp64[..temp64len],
                    &mut fin2,
                );
                ::core::mem::swap(&mut fin1, &mut fin2);
            }
        }

        if bdxtail != 0.0 || bdytail != 0.0 {
            let mut catt = [0f64; 4];
            let mut cat = [0f64; 8];
            let cattlen;
            let catlen;

            if cdxtail != 0.0 || cdytail != 0.0 || adxtail != 0.0 || adytail != 0.0 {
                let (ti1, ti0) = two_product(cdxtail, ady);
                let (tj1, tj0) = two_product(cdx, adytail);
                let (u3, u2, u1, u0) = two_two_sum(ti1, ti0, tj1, tj0);
                let u = [u0, u1, u2, u3];
                let negate = -cdy;
                let (ti1, ti0) = two_product(adxtail, negate);
                let negate = -cdytail;
                let (tj1, tj0) = two_product(adx, negate);
                let (v3, v2, v1, v0) = two_two_sum(ti1, ti0, tj1, tj0);
                let v = [v0, v1, v2, v3];
                catlen = fast_expansion_sum_zeroelim(&u, &v, &mut cat);

                let (ti1, ti0) = two_product(cdxtail, adytail);
                let (tj1, tj0) = two_product(adxtail, cdytail);
                let (catt3, catt2, catt1, catt0) = two_two_diff(ti1, ti0, tj1, tj0);
                catt = [catt0, catt1, catt2, catt3];
                cattlen = 4;
            } else {
                cat[0] = 0.0;
                catlen = 1;
                catt[0] = 0.0;
                cattlen = 1;
            }

            if bdxtail != 0.0 {
                let temp16alen =
                    scale_expansion_zeroelim(&bxtca[..bxtcalen], bdxtail, &mut temp16a);
                let mut bxtcat = [0f64; 16];
                let bxtcatlen = scale_expansion_zeroelim(&cat[..catlen], bdxtail, &mut bxtcat);
                let temp32alen =
                    scale_expansion_zeroelim(&bxtcat[..bxtcatlen], 2.0 * bdx, &mut temp32a);
                let temp48len = fast_expansion_sum_zeroelim(
                    &temp16a[..temp16alen],
                    &temp32a[..temp32alen],
                    &mut temp48,
                );
                finlength = fast_expansion_sum_zeroelim(
                    &fin1[..finlength],
                    &temp48[..temp48len],
                    &mut fin2,
                );
                ::core::mem::swap(&mut fin1, &mut fin2);

                if cdytail != 0.0 {
                    let temp8len = scale_expansion_zeroelim(&aa, bdxtail, &mut temp8);
                    let temp16alen =
                        scale_expansion_zeroelim(&temp8[..temp8len], cdytail, &mut temp16a);
                    finlength = fast_expansion_sum_zeroelim(
                        &fin1[..finlength],
                        &temp16a[..temp16alen],
                        &mut fin2,
                    );
                    ::core::mem::swap(&mut fin1, &mut fin2);
                }
                if adytail != 0.0 {
                    let temp8len = scale_expansion_zeroelim(&cc, -bdxtail, &mut temp8);
                    let temp16alen =
                        scale_expansion_zeroelim(&temp8[..temp8len], adytail, &mut temp16a);
                    finlength = fast_expansion_sum_zeroelim(
                        &fin1[..finlength],
                        &temp16a[..temp16alen],
                        &mut fin2,
                    );
                    ::core::mem::swap(&mut fin1, &mut fin2);
                }

                let temp32alen =
                    scale_expansion_zeroelim(&bxtcat[..bxtcatlen], bdxtail, &mut temp32a);
                let mut bxtcatt = [0f64; 8];
                let bxtcattlen = scale_expansion_zeroelim(&catt[..cattlen], bdxtail, &mut bxtcatt);
                let temp16alen =
                    scale_expansion_zeroelim(&bxtcatt[..bxtcattlen], 2.0 * bdx, &mut temp16a);
                let temp16blen =
                    scale_expansion_zeroelim(&bxtcatt[..bxtcattlen], bdxtail, &mut temp16b);
                let temp32blen = fast_expansion_sum_zeroelim(
                    &temp16a[..temp16alen],
                    &temp16b[..temp16blen],
                    &mut temp32b,
                );
                let temp64len = fast_expansion_sum_zeroelim(
                    &temp32a[..temp32alen],
                    &temp32b[..temp32blen],
                    &mut temp64,
                );
                finlength = fast_expansion_sum_zeroelim(
                    &fin1[..finlength],
                    &temp64[..temp64len],
                    &mut fin2,
                );
                ::core::mem::swap(&mut fin1, &mut fin2);
            }
            if bdytail != 0.0 {
                let temp16alen =
                    scale_expansion_zeroelim(&bytca[..bytcalen], bdytail, &mut temp16a);
                let mut bytcat = [0f64; 16];
                let bytcatlen = scale_expansion_zeroelim(&cat[..catlen], bdytail, &mut bytcat);
                let temp32alen =
                    scale_expansion_zeroelim(&bytcat[..bytcatlen], 2.0 * bdy, &mut temp32a);
                let temp48len = fast_expansion_sum_zeroelim(
                    &temp16a[..temp16alen],
                    &temp32a[..temp32alen],
                    &mut temp48,
                );
                finlength = fast_expansion_sum_zeroelim(
                    &fin1[..finlength],
                    &temp48[..temp48len],
                    &mut fin2,
                );
                ::core::mem::swap(&mut fin1, &mut fin2);

                let temp32alen =
                    scale_expansion_zeroelim(&bytcat[..bytcatlen], bdytail, &mut temp32a);
                let mut bytcatt = [0f64; 8];
                let bytcattlen = scale_expansion_zeroelim(&catt[..cattlen], bdytail, &mut bytcatt);
                let temp16alen =
                    scale_expansion_zeroelim(&bytcatt[..bytcattlen], 2.0 * bdy, &mut temp16a);
                let temp16blen =
                    scale_expansion_zeroelim(&bytcatt[..bytcattlen], bdytail, &mut temp16b);
                let temp32blen = fast_expansion_sum_zeroelim(
                    &temp16a[..temp16alen],
                    &temp16b[..temp16blen],
                    &mut temp32b,
                );
                let temp64len = fast_expansion_sum_zeroelim(
                    &temp32a[..temp32alen],
                    &temp32b[..temp32blen],
                    &mut temp64,
                );
                finlength = fast_expansion_sum_zeroelim(
                    &fin1[..finlength],
                    &temp64[..temp64len],
                    &mut fin2,
                );
                ::core::mem::swap(&mut fin1, &mut fin2);
            }
        }

        if cdxtail != 0.0 || cdytail != 0.0 {
            let mut abtt = [0f64; 4];
            let mut abt = [0f64; 8];
            let abttlen;
            let abtlen;

            if adxtail != 0.0 || adytail != 0.0 || bdxtail != 0.0 || bdytail != 0.0 {
                let (ti1, ti0) = two_product(adxtail, bdy);
                let (tj1, tj0) = two_product(adx, bdytail);
                let (u3, u2, u1, u0) = two_two_sum(ti1, ti0, tj1, tj0);
                let u = [u0, u1, u2, u3];
                let negate = -ady;
                let (ti1, ti0) = two_product(bdxtail, negate);
                let negate = -adytail;
                let (tj1, tj0) = two_product(bdx, negate);
                let (v3, v2, v1, v0) = two_two_sum(ti1, ti0, tj1, tj0);
                let v = [v0, v1, v2, v3];
                abtlen = fast_expansion_sum_zeroelim(&u, &v, &mut abt);

                let (ti1, ti0) = two_product(adxtail, bdytail);
                let (tj1, tj0) = two_product(bdxtail, adytail);
                let (abtt3, abtt2, abtt1, abtt0) = two_two_diff(ti1, ti0, tj1, tj0);
                abtt = [abtt0, abtt1, abtt2, abtt3];
                abttlen = 4;
            } else {
                abt[0] = 0.0;
                abtlen = 1;
                abtt[0] = 0.0;
                abttlen = 1;
            }

            if cdxtail != 0.0 {
                let temp16alen =
                    scale_expansion_zeroelim(&cxtab[..cxtablen], cdxtail, &mut temp16a);
                let mut cxtabt = [0f64; 16];
                let cxtabtlen = scale_expansion_zeroelim(&abt[..abtlen], cdxtail, &mut cxtabt);
                let temp32alen =
                    scale_expansion_zeroelim(&cxtabt[..cxtabtlen], 2.0 * cdx, &mut temp32a);
                let temp48len = fast_expansion_sum_zeroelim(
                    &temp16a[..temp16alen],
                    &temp32a[..temp32alen],
                    &mut temp48,
                );
                finlength = fast_expansion_sum_zeroelim(
                    &fin1[..finlength],
                    &temp48[..temp48len],
                    &mut fin2,
                );
                ::core::mem::swap(&mut fin1, &mut fin2);

                if adytail != 0.0 {
                    let temp8len = scale_expansion_zeroelim(&bb, cdxtail, &mut temp8);
                    let temp16alen =
                        scale_expansion_zeroelim(&temp8[..temp8len], adytail, &mut temp16a);
                    finlength = fast_expansion_sum_zeroelim(
                        &fin1[..finlength],
                        &temp16a[..temp16alen],
                        &mut fin2,
                    );
                    ::core::mem::swap(&mut fin1, &mut fin2);
                }
                if bdytail != 0.0 {
                    let temp8len = scale_expansion_zeroelim(&aa, -cdxtail, &mut temp8);
                    let temp16alen =
                        scale_expansion_zeroelim(&temp8[..temp8len], bdytail, &mut temp16a);
                    finlength = fast_expansion_sum_zeroelim(
                        &fin1[..finlength],
                        &temp16a[..temp16alen],
                        &mut fin2,
                    );
                    ::core::mem::swap(&mut fin1, &mut fin2);
                }

                let temp32alen =
                    scale_expansion_zeroelim(&cxtabt[..cxtabtlen], cdxtail, &mut temp32a);
                let mut cxtabtt = [0f64; 8];
                let cxtabttlen = scale_expansion_zeroelim(&abtt[..abttlen], cdxtail, &mut cxtabtt);
                let temp16alen =
                    scale_expansion_zeroelim(&cxtabtt[..cxtabttlen], 2.0 * cdx, &mut temp16a);
                let temp16blen =
                    scale_expansion_zeroelim(&cxtabtt[..cxtabttlen], cdxtail, &mut temp16b);
                let temp32blen = fast_expansion_sum_zeroelim(
                    &temp16a[..temp16alen],
                    &temp16b[..temp16blen],
                    &mut temp32b,
                );
                let temp64len = fast_expansion_sum_zeroelim(
                    &temp32a[..temp32alen],
                    &temp32b[..temp32blen],
                    &mut temp64,
                );
                finlength = fast_expansion_sum_zeroelim(
                    &fin1[..finlength],
                    &temp64[..temp64len],
                    &mut fin2,
                );
                ::core::mem::swap(&mut fin1, &mut fin2);
            }
            if cdytail != 0.0 {
                let temp16alen =
                    scale_expansion_zeroelim(&cytab[..cytablen], cdytail, &mut temp16a);
                let mut cytabt = [0f64; 16];
                let cytabtlen = scale_expansion_zeroelim(&abt[..abtlen], cdytail, &mut cytabt);
                let temp32alen =
                    scale_expansion_zeroelim(&cytabt[..cytabtlen], 2.0 * cdy, &mut temp32a);
                let temp48len = fast_expansion_sum_zeroelim(
                    &temp16a[..temp16alen],
                    &temp32a[..temp32alen],
                    &mut temp48,
                );
                finlength = fast_expansion_sum_zeroelim(
                    &fin1[..finlength],
                    &temp48[..temp48len],
                    &mut fin2,
                );
                ::core::mem::swap(&mut fin1, &mut fin2);

                let temp32alen =
                    scale_expansion_zeroelim(&cytabt[..cytabtlen], cdytail, &mut temp32a);
                let mut cytabtt = [0f64; 8];
                let cytabttlen = scale_expansion_zeroelim(&abtt[..abttlen], cdytail, &mut cytabtt);
                let temp16alen =
                    scale_expansion_zeroelim(&cytabtt[..cytabttlen], 2.0 * cdy, &mut temp16a);
                let temp16blen =
                    scale_expansion_zeroelim(&cytabtt[..cytabttlen], cdytail, &mut temp16b);
                let temp32blen = fast_expansion_sum_zeroelim(
                    &temp16a[..temp16alen],
                    &temp16b[..temp16blen],
                    &mut temp32b,
                );
                let temp64len = fast_expansion_sum_zeroelim(
                    &temp32a[..temp32alen],
                    &temp32b[..temp32blen],
                    &mut temp64,
                );
                finlength = fast_expansion_sum_zeroelim(
                    &fin1[..finlength],
                    &temp64[..temp64len],
                    &mut fin2,
                );
                ::core::mem::swap(&mut fin1, &mut fin2);
            }
        }
        fin1[finlength - 1]
    }

    /// Returns a positive value if the point `pe` lies inside the sphere passing through `pa`, `pb`, `pc`, and `pd`.  
    /// Returns a negative value if it lies outside.  
    /// Returns `0` if the five points are **cospherical**.  
    /// **NOTE**: The points `pa`, `pb`, `pc`, and `pd` must be ordered so that they have a positive orientation.
    pub fn insphere<T: Into<f64>>(
        pa: Coord3D<T>,
        pb: Coord3D<T>,
        pc: Coord3D<T>,
        pd: Coord3D<T>,
        pe: Coord3D<T>,
    ) -> f64 {
        let pa = Coord3D {
            x: pa.x.into(),
            y: pa.y.into(),
            z: pa.z.into(),
        };
        let pb = Coord3D {
            x: pb.x.into(),
            y: pb.y.into(),
            z: pb.z.into(),
        };
        let pc = Coord3D {
            x: pc.x.into(),
            y: pc.y.into(),
            z: pc.z.into(),
        };
        let pd = Coord3D {
            x: pd.x.into(),
            y: pd.y.into(),
            z: pd.z.into(),
        };
        let pe = Coord3D {
            x: pe.x.into(),
            y: pe.y.into(),
            z: pe.z.into(),
        };

        let aex = pa.x - pe.x;
        let bex = pb.x - pe.x;
        let cex = pc.x - pe.x;
        let dex = pd.x - pe.x;
        let aey = pa.y - pe.y;
        let bey = pb.y - pe.y;
        let cey = pc.y - pe.y;
        let dey = pd.y - pe.y;
        let aez = pa.z - pe.z;
        let bez = pb.z - pe.z;
        let cez = pc.z - pe.z;
        let dez = pd.z - pe.z;

        let aexbey = aex * bey;
        let bexaey = bex * aey;
        let ab = aexbey - bexaey;
        let bexcey = bex * cey;
        let cexbey = cex * bey;
        let bc = bexcey - cexbey;
        let cexdey = cex * dey;
        let dexcey = dex * cey;
        let cd = cexdey - dexcey;
        let dexaey = dex * aey;
        let aexdey = aex * dey;
        let da = dexaey - aexdey;

        let aexcey = aex * cey;
        let cexaey = cex * aey;
        let ac = aexcey - cexaey;
        let bexdey = bex * dey;
        let dexbey = dex * bey;
        let bd = bexdey - dexbey;

        let abc = aez * bc - bez * ac + cez * ab;
        let bcd = bez * cd - cez * bd + dez * bc;
        let cda = cez * da + dez * ac + aez * cd;
        let dab = dez * ab + aez * bd + bez * da;

        let alift = aex * aex + aey * aey + aez * aez;
        let blift = bex * bex + bey * bey + bez * bez;
        let clift = cex * cex + cey * cey + cez * cez;
        let dlift = dex * dex + dey * dey + dez * dez;

        let det = (dlift * abc - clift * dab) + (blift * cda - alift * bcd);

        let aezplus = abs(aez);
        let bezplus = abs(bez);
        let cezplus = abs(cez);
        let dezplus = abs(dez);
        let aexbeyplus = abs(aexbey);
        let bexaeyplus = abs(bexaey);
        let bexceyplus = abs(bexcey);
        let cexbeyplus = abs(cexbey);
        let cexdeyplus = abs(cexdey);
        let dexceyplus = abs(dexcey);
        let dexaeyplus = abs(dexaey);
        let aexdeyplus = abs(aexdey);
        let aexceyplus = abs(aexcey);
        let cexaeyplus = abs(cexaey);
        let bexdeyplus = abs(bexdey);
        let dexbeyplus = abs(dexbey);
        let permanent = ((cexdeyplus + dexceyplus) * bezplus
            + (dexbeyplus + bexdeyplus) * cezplus
            + (bexceyplus + cexbeyplus) * dezplus)
            * alift
            + ((dexaeyplus + aexdeyplus) * cezplus
                + (aexceyplus + cexaeyplus) * dezplus
                + (cexdeyplus + dexceyplus) * aezplus)
                * blift
            + ((aexbeyplus + bexaeyplus) * dezplus
                + (bexdeyplus + dexbeyplus) * aezplus
                + (dexaeyplus + aexdeyplus) * bezplus)
                * clift
            + ((bexceyplus + cexbeyplus) * aezplus
                + (cexaeyplus + aexceyplus) * bezplus
                + (aexbeyplus + bexaeyplus) * cezplus)
                * dlift;
        let errbound = ISPERRBOUND_A * permanent;
        if det > errbound || -det > errbound {
            return det;
        }

        insphereadapt(pa, pb, pc, pd, pe, permanent)
    }

    fn insphereadapt<T: Into<f64>>(
        pa: Coord3D<T>,
        pb: Coord3D<T>,
        pc: Coord3D<T>,
        pd: Coord3D<T>,
        pe: Coord3D<T>,
        permanent: f64,
    ) -> f64 {
        let pa = Coord3D {
            x: pa.x.into(),
            y: pa.y.into(),
            z: pa.z.into(),
        };
        let pb = Coord3D {
            x: pb.x.into(),
            y: pb.y.into(),
            z: pb.z.into(),
        };
        let pc = Coord3D {
            x: pc.x.into(),
            y: pc.y.into(),
            z: pc.z.into(),
        };
        let pd = Coord3D {
            x: pd.x.into(),
            y: pd.y.into(),
            z: pd.z.into(),
        };
        let pe = Coord3D {
            x: pe.x.into(),
            y: pe.y.into(),
            z: pe.z.into(),
        };

        let aex = pa.x - pe.x;
        let bex = pb.x - pe.x;
        let cex = pc.x - pe.x;
        let dex = pd.x - pe.x;
        let aey = pa.y - pe.y;
        let bey = pb.y - pe.y;
        let cey = pc.y - pe.y;
        let dey = pd.y - pe.y;
        let aez = pa.z - pe.z;
        let bez = pb.z - pe.z;
        let cez = pc.z - pe.z;
        let dez = pd.z - pe.z;

        let (aexbey1, aexbey0) = two_product(aex, bey);
        let (bexaey1, bexaey0) = two_product(bex, aey);
        let (ab3, ab2, ab1, ab0) = two_two_diff(aexbey1, aexbey0, bexaey1, bexaey0);
        let ab = [ab0, ab1, ab2, ab3];

        let (bexcey1, bexcey0) = two_product(bex, cey);
        let (cexbey1, cexbey0) = two_product(cex, bey);
        let (bc3, bc2, bc1, bc0) = two_two_diff(bexcey1, bexcey0, cexbey1, cexbey0);
        let bc = [bc0, bc1, bc2, bc3];

        let (cexdey1, cexdey0) = two_product(cex, dey);
        let (dexcey1, dexcey0) = two_product(dex, cey);
        let (cd3, cd2, cd1, cd0) = two_two_diff(cexdey1, cexdey0, dexcey1, dexcey0);
        let cd = [cd0, cd1, cd2, cd3];

        let (dexaey1, dexaey0) = two_product(dex, aey);
        let (aexdey1, aexdey0) = two_product(aex, dey);
        let (da3, da2, da1, da0) = two_two_diff(dexaey1, dexaey0, aexdey1, aexdey0);
        let da = [da0, da1, da2, da3];

        let (aexcey1, aexcey0) = two_product(aex, cey);
        let (cexaey1, cexaey0) = two_product(cex, aey);
        let (ac3, ac2, ac1, ac0) = two_two_diff(aexcey1, aexcey0, cexaey1, cexaey0);
        let ac = [ac0, ac1, ac2, ac3];

        let (bexdey1, bexdey0) = two_product(bex, dey);
        let (dexbey1, dexbey0) = two_product(dex, bey);
        let (bd3, bd2, bd1, bd0) = two_two_diff(bexdey1, bexdey0, dexbey1, dexbey0);
        let bd = [bd0, bd1, bd2, bd3];

        let mut temp8a = [0f64; 8];
        let mut temp8b = [0f64; 8];
        let mut temp8c = [0f64; 8];
        let mut temp16 = [0f64; 16];
        let mut temp24 = [0f64; 24];
        let mut temp48 = [0f64; 48];
        let mut xdet = [0f64; 96];
        let mut ydet = [0f64; 96];
        let mut zdet = [0f64; 96];
        let mut xydet = [0f64; 192];
        let mut adet = [0f64; 288];
        let mut bdet = [0f64; 288];
        let mut cdet = [0f64; 288];
        let mut ddet = [0f64; 288];
        let mut abdet = [0f64; 576];
        let mut cddet = [0f64; 576];
        let mut fin1 = [0f64; 1152];

        let mut temp8alen = scale_expansion_zeroelim(&cd[..4], bez, &mut temp8a);
        let mut temp8blen = scale_expansion_zeroelim(&bd[..4], -cez, &mut temp8b);
        let mut temp8clen = scale_expansion_zeroelim(&bc[..4], dez, &mut temp8c);
        let mut temp16len =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp8b[..temp8blen], &mut temp16);
        let mut temp24len =
            fast_expansion_sum_zeroelim(&temp8c[..temp8clen], &temp16[..temp16len], &mut temp24);
        let temp48len = scale_expansion_zeroelim(&temp24[..temp24len], aex, &mut temp48);
        let mut xlen = scale_expansion_zeroelim(&temp48[..temp48len], -aex, &mut xdet);
        let temp48len = scale_expansion_zeroelim(&temp24[..temp24len], aey, &mut temp48);
        let mut ylen = scale_expansion_zeroelim(&temp48[..temp48len], -aey, &mut ydet);
        let mut temp48len = scale_expansion_zeroelim(&temp24[..temp24len], aez, &mut temp48);
        let mut zlen = scale_expansion_zeroelim(&temp48[..temp48len], -aez, &mut zdet);
        let mut xylen = fast_expansion_sum_zeroelim(&xdet[..xlen], &ydet[..ylen], &mut xydet);
        let alen = fast_expansion_sum_zeroelim(&xydet[..xylen], &zdet[..zlen], &mut adet);

        temp8alen = scale_expansion_zeroelim(&da[..4], cez, &mut temp8a);
        temp8blen = scale_expansion_zeroelim(&ac[..4], dez, &mut temp8b);
        temp8clen = scale_expansion_zeroelim(&cd[..4], aez, &mut temp8c);
        temp16len =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp8b[..temp8blen], &mut temp16);
        temp24len =
            fast_expansion_sum_zeroelim(&temp8c[..temp8clen], &temp16[..temp16len], &mut temp24);
        temp48len = scale_expansion_zeroelim(&temp24[..temp24len], bex, &mut temp48);
        xlen = scale_expansion_zeroelim(&temp48[..temp48len], bex, &mut xdet);
        temp48len = scale_expansion_zeroelim(&temp24[..temp24len], bey, &mut temp48);
        ylen = scale_expansion_zeroelim(&temp48[..temp48len], bey, &mut ydet);
        temp48len = scale_expansion_zeroelim(&temp24[..temp24len], bez, &mut temp48);
        zlen = scale_expansion_zeroelim(&temp48[..temp48len], bez, &mut zdet);
        xylen = fast_expansion_sum_zeroelim(&xdet[..xlen], &ydet[..ylen], &mut xydet);
        let blen = fast_expansion_sum_zeroelim(&xydet[..xylen], &zdet[..zlen], &mut bdet);

        temp8alen = scale_expansion_zeroelim(&ab[..4], dez, &mut temp8a);
        temp8blen = scale_expansion_zeroelim(&bd[..4], aez, &mut temp8b);
        temp8clen = scale_expansion_zeroelim(&da[..4], bez, &mut temp8c);
        temp16len =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp8b[..temp8blen], &mut temp16);
        temp24len =
            fast_expansion_sum_zeroelim(&temp8c[..temp8clen], &temp16[..temp16len], &mut temp24);
        temp48len = scale_expansion_zeroelim(&temp24[..temp24len], cex, &mut temp48);
        xlen = scale_expansion_zeroelim(&temp48[..temp48len], -cex, &mut xdet);
        temp48len = scale_expansion_zeroelim(&temp24[..temp24len], cey, &mut temp48);
        ylen = scale_expansion_zeroelim(&temp48[..temp48len], -cey, &mut ydet);
        temp48len = scale_expansion_zeroelim(&temp24[..temp24len], cez, &mut temp48);
        zlen = scale_expansion_zeroelim(&temp48[..temp48len], -cez, &mut zdet);
        xylen = fast_expansion_sum_zeroelim(&xdet[..xlen], &ydet[..ylen], &mut xydet);
        let clen = fast_expansion_sum_zeroelim(&xydet[..xylen], &zdet[..zlen], &mut cdet);

        temp8alen = scale_expansion_zeroelim(&bc[..4], aez, &mut temp8a);
        temp8blen = scale_expansion_zeroelim(&ac[..4], -bez, &mut temp8b);
        temp8clen = scale_expansion_zeroelim(&ab[..4], cez, &mut temp8c);
        temp16len =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp8b[..temp8blen], &mut temp16);
        temp24len =
            fast_expansion_sum_zeroelim(&temp8c[..temp8clen], &temp16[..temp16len], &mut temp24);
        temp48len = scale_expansion_zeroelim(&temp24[..temp24len], dex, &mut temp48);
        xlen = scale_expansion_zeroelim(&temp48[..temp48len], dex, &mut xdet);
        temp48len = scale_expansion_zeroelim(&temp24[..temp24len], dey, &mut temp48);
        ylen = scale_expansion_zeroelim(&temp48[..temp48len], dey, &mut ydet);
        temp48len = scale_expansion_zeroelim(&temp24[..temp24len], dez, &mut temp48);
        zlen = scale_expansion_zeroelim(&temp48[..temp48len], dez, &mut zdet);
        xylen = fast_expansion_sum_zeroelim(&xdet[..xlen], &ydet[..ylen], &mut xydet);
        let dlen = fast_expansion_sum_zeroelim(&xydet[..xylen], &zdet[..zlen], &mut ddet);

        let ablen = fast_expansion_sum_zeroelim(&adet[..alen], &bdet[..blen], &mut abdet);
        let cdlen = fast_expansion_sum_zeroelim(&cdet[..clen], &ddet[..dlen], &mut cddet);
        let finlength = fast_expansion_sum_zeroelim(&abdet[..ablen], &cddet[..cdlen], &mut fin1);

        let mut det = estimate(&fin1[..finlength]);
        let mut errbound = ISPERRBOUND_B * permanent;
        if det >= errbound || -det >= errbound {
            return det;
        }

        let aextail = two_diff_tail(pa.x, pe.x, aex);
        let aeytail = two_diff_tail(pa.y, pe.y, aey);
        let aeztail = two_diff_tail(pa.z, pe.z, aez);
        let bextail = two_diff_tail(pb.x, pe.x, bex);
        let beytail = two_diff_tail(pb.y, pe.y, bey);
        let beztail = two_diff_tail(pb.z, pe.z, bez);
        let cextail = two_diff_tail(pc.x, pe.x, cex);
        let ceytail = two_diff_tail(pc.y, pe.y, cey);
        let ceztail = two_diff_tail(pc.z, pe.z, cez);
        let dextail = two_diff_tail(pd.x, pe.x, dex);
        let deytail = two_diff_tail(pd.y, pe.y, dey);
        let deztail = two_diff_tail(pd.z, pe.z, dez);
        if (aextail == 0.0)
            && (aeytail == 0.0)
            && (aeztail == 0.0)
            && (bextail == 0.0)
            && (beytail == 0.0)
            && (beztail == 0.0)
            && (cextail == 0.0)
            && (ceytail == 0.0)
            && (ceztail == 0.0)
            && (dextail == 0.0)
            && (deytail == 0.0)
            && (deztail == 0.0)
        {
            return det;
        }

        errbound = ISPERRBOUND_C * permanent + RESULTERRBOUND * abs(det);
        let abeps = (aex * beytail + bey * aextail) - (aey * bextail + bex * aeytail);
        let bceps = (bex * ceytail + cey * bextail) - (bey * cextail + cex * beytail);
        let cdeps = (cex * deytail + dey * cextail) - (cey * dextail + dex * ceytail);
        let daeps = (dex * aeytail + aey * dextail) - (dey * aextail + aex * deytail);
        let aceps = (aex * ceytail + cey * aextail) - (aey * cextail + cex * aeytail);
        let bdeps = (bex * deytail + dey * bextail) - (bey * dextail + dex * beytail);
        det += (((bex * bex + bey * bey + bez * bez)
            * ((cez * daeps + dez * aceps + aez * cdeps)
                + (ceztail * da3 + deztail * ac3 + aeztail * cd3))
            + (dex * dex + dey * dey + dez * dez)
                * ((aez * bceps - bez * aceps + cez * abeps)
                    + (aeztail * bc3 - beztail * ac3 + ceztail * ab3)))
            - ((aex * aex + aey * aey + aez * aez)
                * ((bez * cdeps - cez * bdeps + dez * bceps)
                    + (beztail * cd3 - ceztail * bd3 + deztail * bc3))
                + (cex * cex + cey * cey + cez * cez)
                    * ((dez * abeps + aez * bdeps + bez * daeps)
                        + (deztail * ab3 + aeztail * bd3 + beztail * da3))))
            + 2.0
                * (((bex * bextail + bey * beytail + bez * beztail)
                    * (cez * da3 + dez * ac3 + aez * cd3)
                    + (dex * dextail + dey * deytail + dez * deztail)
                        * (aez * bc3 - bez * ac3 + cez * ab3))
                    - ((aex * aextail + aey * aeytail + aez * aeztail)
                        * (bez * cd3 - cez * bd3 + dez * bc3)
                        + (cex * cextail + cey * ceytail + cez * ceztail)
                            * (dez * ab3 + aez * bd3 + bez * da3)));
        if det >= errbound || -det >= errbound {
            return det;
        }

        insphereexact(pa, pb, pc, pd, pe)
    }

    fn insphereexact<T: Into<f64>>(
        pa: Coord3D<T>,
        pb: Coord3D<T>,
        pc: Coord3D<T>,
        pd: Coord3D<T>,
        pe: Coord3D<T>,
    ) -> f64 {
        let pa = Coord3D {
            x: pa.x.into(),
            y: pa.y.into(),
            z: pa.z.into(),
        };
        let pb = Coord3D {
            x: pb.x.into(),
            y: pb.y.into(),
            z: pb.z.into(),
        };
        let pc = Coord3D {
            x: pc.x.into(),
            y: pc.y.into(),
            z: pc.z.into(),
        };
        let pd = Coord3D {
            x: pd.x.into(),
            y: pd.y.into(),
            z: pd.z.into(),
        };
        let pe = Coord3D {
            x: pe.x.into(),
            y: pe.y.into(),
            z: pe.z.into(),
        };

        let (axby1, axby0) = two_product(pa.x, pb.y);
        let (bxay1, bxay0) = two_product(pb.x, pa.y);
        let (ab3, ab2, ab1, ab0) = two_two_diff(axby1, axby0, bxay1, bxay0);
        let ab = [ab0, ab1, ab2, ab3];

        let (bxcy1, bxcy0) = two_product(pb.x, pc.y);
        let (cxby1, cxby0) = two_product(pc.x, pb.y);
        let (bc3, bc2, bc1, bc0) = two_two_diff(bxcy1, bxcy0, cxby1, cxby0);
        let bc = [bc0, bc1, bc2, bc3];

        let (cxdy1, cxdy0) = two_product(pc.x, pd.y);
        let (dxcy1, dxcy0) = two_product(pd.x, pc.y);
        let (cd3, cd2, cd1, cd0) = two_two_diff(cxdy1, cxdy0, dxcy1, dxcy0);
        let cd = [cd0, cd1, cd2, cd3];

        let (dxey1, dxey0) = two_product(pd.x, pe.y);
        let (exdy1, exdy0) = two_product(pe.x, pd.y);
        let (de3, de2, de1, de0) = two_two_diff(dxey1, dxey0, exdy1, exdy0);
        let de = [de0, de1, de2, de3];

        let (exay1, exay0) = two_product(pe.x, pa.y);
        let (axey1, axey0) = two_product(pa.x, pe.y);
        let (ea3, ea2, ea1, ea0) = two_two_diff(exay1, exay0, axey1, axey0);
        let ea = [ea0, ea1, ea2, ea3];

        let (axcy1, axcy0) = two_product(pa.x, pc.y);
        let (cxay1, cxay0) = two_product(pc.x, pa.y);
        let (ac3, ac2, ac1, ac0) = two_two_diff(axcy1, axcy0, cxay1, cxay0);
        let ac = [ac0, ac1, ac2, ac3];

        let (bxdy1, bxdy0) = two_product(pb.x, pd.y);
        let (dxby1, dxby0) = two_product(pd.x, pb.y);
        let (bd3, bd2, bd1, bd0) = two_two_diff(bxdy1, bxdy0, dxby1, dxby0);
        let bd = [bd0, bd1, bd2, bd3];

        let (cxey1, cxey0) = two_product(pc.x, pe.y);
        let (excy1, excy0) = two_product(pe.x, pc.y);
        let (ce3, ce2, ce1, ce0) = two_two_diff(cxey1, cxey0, excy1, excy0);
        let ce = [ce0, ce1, ce2, ce3];

        let (dxay1, dxay0) = two_product(pd.x, pa.y);
        let (axdy1, axdy0) = two_product(pa.x, pd.y);
        let (da3, da2, da1, da0) = two_two_diff(dxay1, dxay0, axdy1, axdy0);
        let da = [da0, da1, da2, da3];

        let (exby1, exby0) = two_product(pe.x, pb.y);
        let (bxey1, bxey0) = two_product(pb.x, pe.y);
        let (eb3, eb2, eb1, eb0) = two_two_diff(exby1, exby0, bxey1, bxey0);
        let eb = [eb0, eb1, eb2, eb3];

        let mut temp8a = [0f64; 8];
        let mut temp8b = [0f64; 8];
        let mut temp16 = [0f64; 16];
        let mut temp48a = [0f64; 48];
        let mut temp48b = [0f64; 48];

        let mut abc = [0f64; 24];
        let mut bcd = [0f64; 24];
        let mut cde = [0f64; 24];
        let mut dea = [0f64; 24];
        let mut eab = [0f64; 24];
        let mut abd = [0f64; 24];
        let mut bce = [0f64; 24];
        let mut cda = [0f64; 24];
        let mut deb = [0f64; 24];
        let mut eac = [0f64; 24];

        let mut abcd = [0f64; 96];
        let mut bcde = [0f64; 96];
        let mut cdea = [0f64; 96];
        let mut deab = [0f64; 96];
        let mut eabc = [0f64; 96];

        let mut temp192 = [0f64; 192];
        let mut det384x = [0f64; 384];
        let mut det384y = [0f64; 384];
        let mut det384z = [0f64; 384];

        let mut detxy = [0f64; 768];

        let mut adet = [0f64; 1152];
        let mut bdet = [0f64; 1152];
        let mut cdet = [0f64; 1152];
        let mut ddet = [0f64; 1152];
        let mut edet = [0f64; 1152];

        let mut abdet = [0f64; 2304];
        let mut cddet = [0f64; 2304];
        let mut cdedet = [0f64; 3456];

        let mut deter = [0f64; 5760];

        let mut temp8alen = scale_expansion_zeroelim(&bc[..4], pa.z, &mut temp8a);
        let mut temp8blen = scale_expansion_zeroelim(&ac[..4], -pb.z, &mut temp8b);
        let mut temp16len =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp8b[..temp8blen], &mut temp16);
        temp8alen = scale_expansion_zeroelim(&ab[..4], pc.z, &mut temp8a);
        let abclen =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp16[..temp16len], &mut abc);

        temp8alen = scale_expansion_zeroelim(&cd[..4], pb.z, &mut temp8a);
        temp8blen = scale_expansion_zeroelim(&bd[..4], -pc.z, &mut temp8b);
        temp16len =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp8b[..temp8blen], &mut temp16);
        temp8alen = scale_expansion_zeroelim(&bc[..4], pd.z, &mut temp8a);
        let bcdlen =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp16[..temp16len], &mut bcd);

        temp8alen = scale_expansion_zeroelim(&de[..4], pc.z, &mut temp8a);
        temp8blen = scale_expansion_zeroelim(&ce[..4], -pd.z, &mut temp8b);
        temp16len =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp8b[..temp8blen], &mut temp16);
        temp8alen = scale_expansion_zeroelim(&cd[..4], pe.z, &mut temp8a);
        let cdelen =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp16[..temp16len], &mut cde);

        temp8alen = scale_expansion_zeroelim(&ea[..4], pd.z, &mut temp8a);
        temp8blen = scale_expansion_zeroelim(&da[..4], -pe.z, &mut temp8b);
        temp16len =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp8b[..temp8blen], &mut temp16);
        temp8alen = scale_expansion_zeroelim(&de[..4], pa.z, &mut temp8a);
        let dealen =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp16[..temp16len], &mut dea);

        temp8alen = scale_expansion_zeroelim(&ab[..4], pe.z, &mut temp8a);
        temp8blen = scale_expansion_zeroelim(&eb[..4], -pa.z, &mut temp8b);
        temp16len =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp8b[..temp8blen], &mut temp16);
        temp8alen = scale_expansion_zeroelim(&ea[..4], pb.z, &mut temp8a);
        let eablen =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp16[..temp16len], &mut eab);

        temp8alen = scale_expansion_zeroelim(&bd[..4], pa.z, &mut temp8a);
        temp8blen = scale_expansion_zeroelim(&da[..4], pb.z, &mut temp8b);
        temp16len =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp8b[..temp8blen], &mut temp16);
        temp8alen = scale_expansion_zeroelim(&ab[..4], pd.z, &mut temp8a);
        let abdlen =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp16[..temp16len], &mut abd);

        temp8alen = scale_expansion_zeroelim(&ce[..4], pb.z, &mut temp8a);
        temp8blen = scale_expansion_zeroelim(&eb[..4], pc.z, &mut temp8b);
        temp16len =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp8b[..temp8blen], &mut temp16);
        temp8alen = scale_expansion_zeroelim(&bc[..4], pe.z, &mut temp8a);
        let bcelen =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp16[..temp16len], &mut bce);

        temp8alen = scale_expansion_zeroelim(&da[..4], pc.z, &mut temp8a);
        temp8blen = scale_expansion_zeroelim(&ac[..4], pd.z, &mut temp8b);
        temp16len =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp8b[..temp8blen], &mut temp16);
        temp8alen = scale_expansion_zeroelim(&cd[..4], pa.z, &mut temp8a);
        let cdalen =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp16[..temp16len], &mut cda);

        temp8alen = scale_expansion_zeroelim(&eb[..4], pd.z, &mut temp8a);
        temp8blen = scale_expansion_zeroelim(&bd[..4], pe.z, &mut temp8b);
        temp16len =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp8b[..temp8blen], &mut temp16);
        temp8alen = scale_expansion_zeroelim(&de[..4], pb.z, &mut temp8a);
        let deblen =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp16[..temp16len], &mut deb);

        temp8alen = scale_expansion_zeroelim(&ac[..4], pe.z, &mut temp8a);
        temp8blen = scale_expansion_zeroelim(&ce[..4], pa.z, &mut temp8b);
        temp16len =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp8b[..temp8blen], &mut temp16);
        let temp8alen = scale_expansion_zeroelim(&ea[..4], pc.z, &mut temp8a);
        let eaclen =
            fast_expansion_sum_zeroelim(&temp8a[..temp8alen], &temp16[..temp16len], &mut eac);

        let mut temp48alen =
            fast_expansion_sum_zeroelim(&cde[..cdelen], &bce[..bcelen], &mut temp48a);
        let mut temp48blen =
            fast_expansion_sum_zeroelim(&deb[..deblen], &bcd[..bcdlen], &mut temp48b);
        for i in 0..temp48blen {
            temp48b[i] = -temp48b[i];
        }

        let bcdelen =
            fast_expansion_sum_zeroelim(&temp48a[..temp48alen], &temp48b[..temp48blen], &mut bcde);
        let mut xlen = scale_expansion_zeroelim(&bcde[..bcdelen], pa.x, &mut temp192);
        xlen = scale_expansion_zeroelim(&temp192[..xlen], pa.x, &mut det384x);
        let mut ylen = scale_expansion_zeroelim(&bcde[..bcdelen], pa.y, &mut temp192);
        ylen = scale_expansion_zeroelim(&temp192[..ylen], pa.y, &mut det384y);
        let mut zlen = scale_expansion_zeroelim(&bcde[..bcdelen], pa.z, &mut temp192);
        zlen = scale_expansion_zeroelim(&temp192[..zlen], pa.z, &mut det384z);
        let mut xylen = fast_expansion_sum_zeroelim(&det384x[..xlen], &det384y[..ylen], &mut detxy);
        let alen = fast_expansion_sum_zeroelim(&detxy[..xylen], &det384z[..zlen], &mut adet);

        temp48alen = fast_expansion_sum_zeroelim(&dea[..dealen], &cda[..cdalen], &mut temp48a);
        temp48blen = fast_expansion_sum_zeroelim(&eac[..eaclen], &cde[..cdelen], &mut temp48b);
        for i in 0..temp48blen {
            temp48b[i] = -temp48b[i];
        }
        let cdealen =
            fast_expansion_sum_zeroelim(&temp48a[..temp48alen], &temp48b[..temp48blen], &mut cdea);
        xlen = scale_expansion_zeroelim(&cdea[..cdealen], pb.x, &mut temp192);
        xlen = scale_expansion_zeroelim(&temp192[..xlen], pb.x, &mut det384x);
        ylen = scale_expansion_zeroelim(&cdea[..cdealen], pb.y, &mut temp192);
        ylen = scale_expansion_zeroelim(&temp192[..ylen], pb.y, &mut det384y);
        zlen = scale_expansion_zeroelim(&cdea[..cdealen], pb.z, &mut temp192);
        zlen = scale_expansion_zeroelim(&temp192[..zlen], pb.z, &mut det384z);
        xylen = fast_expansion_sum_zeroelim(&det384x[..xlen], &det384y[..ylen], &mut detxy);
        let blen = fast_expansion_sum_zeroelim(&detxy[..xylen], &det384z[..zlen], &mut bdet);

        temp48alen = fast_expansion_sum_zeroelim(&eab[..eablen], &deb[..deblen], &mut temp48a);
        temp48blen = fast_expansion_sum_zeroelim(&abd[..abdlen], &dea[..dealen], &mut temp48b);
        for i in 0..temp48blen {
            temp48b[i] = -temp48b[i];
        }
        let deablen =
            fast_expansion_sum_zeroelim(&temp48a[..temp48alen], &temp48b[..temp48blen], &mut deab);
        xlen = scale_expansion_zeroelim(&deab[..deablen], pc.x, &mut temp192);
        xlen = scale_expansion_zeroelim(&temp192[..xlen], pc.x, &mut det384x);
        ylen = scale_expansion_zeroelim(&deab[..deablen], pc.y, &mut temp192);
        ylen = scale_expansion_zeroelim(&temp192[..ylen], pc.y, &mut det384y);
        zlen = scale_expansion_zeroelim(&deab[..deablen], pc.z, &mut temp192);
        zlen = scale_expansion_zeroelim(&temp192[..zlen], pc.z, &mut det384z);
        xylen = fast_expansion_sum_zeroelim(&det384x[..xlen], &det384y[..ylen], &mut detxy);
        let clen = fast_expansion_sum_zeroelim(&detxy[..xylen], &det384z[..zlen], &mut cdet);

        temp48alen = fast_expansion_sum_zeroelim(&abc[..abclen], &eac[..eaclen], &mut temp48a);
        temp48blen = fast_expansion_sum_zeroelim(&bce[..bcelen], &eab[..eablen], &mut temp48b);
        for i in 0..temp48blen {
            temp48b[i] = -temp48b[i];
        }
        let eabclen =
            fast_expansion_sum_zeroelim(&temp48a[..temp48alen], &temp48b[..temp48blen], &mut eabc);
        xlen = scale_expansion_zeroelim(&eabc[..eabclen], pd.x, &mut temp192);
        xlen = scale_expansion_zeroelim(&temp192[..xlen], pd.x, &mut det384x);
        ylen = scale_expansion_zeroelim(&eabc[..eabclen], pd.y, &mut temp192);
        ylen = scale_expansion_zeroelim(&temp192[..ylen], pd.y, &mut det384y);
        zlen = scale_expansion_zeroelim(&eabc[..eabclen], pd.z, &mut temp192);
        zlen = scale_expansion_zeroelim(&temp192[..zlen], pd.z, &mut det384z);
        xylen = fast_expansion_sum_zeroelim(&det384x[..xlen], &det384y[..ylen], &mut detxy);
        let dlen = fast_expansion_sum_zeroelim(&detxy[..xylen], &det384z[..zlen], &mut ddet);

        temp48alen = fast_expansion_sum_zeroelim(&bcd[..bcdlen], &abd[..abdlen], &mut temp48a);
        temp48blen = fast_expansion_sum_zeroelim(&cda[..cdalen], &abc[..abclen], &mut temp48b);
        for i in 0..temp48blen {
            temp48b[i] = -temp48b[i];
        }
        let abcdlen =
            fast_expansion_sum_zeroelim(&temp48a[..temp48alen], &temp48b[..temp48blen], &mut abcd);
        xlen = scale_expansion_zeroelim(&abcd[..abcdlen], pe.x, &mut temp192);
        xlen = scale_expansion_zeroelim(&temp192[..xlen], pe.x, &mut det384x);
        ylen = scale_expansion_zeroelim(&abcd[..abcdlen], pe.y, &mut temp192);
        ylen = scale_expansion_zeroelim(&temp192[..ylen], pe.y, &mut det384y);
        zlen = scale_expansion_zeroelim(&abcd[..abcdlen], pe.z, &mut temp192);
        zlen = scale_expansion_zeroelim(&temp192[..zlen], pe.z, &mut det384z);
        xylen = fast_expansion_sum_zeroelim(&det384x[..xlen], &det384y[..ylen], &mut detxy);
        let elen = fast_expansion_sum_zeroelim(&detxy[..xylen], &det384z[..zlen], &mut edet);

        let ablen = fast_expansion_sum_zeroelim(&adet[..alen], &bdet[..blen], &mut abdet);
        let cdlen = fast_expansion_sum_zeroelim(&cdet[..clen], &ddet[..dlen], &mut cddet);
        let cdelen = fast_expansion_sum_zeroelim(&cddet[..cdlen], &edet[..elen], &mut cdedet);
        let deterlen = fast_expansion_sum_zeroelim(&abdet[..ablen], &cdedet[..cdelen], &mut deter);

        deter[deterlen - 1]
    }

    fn scale_expansion_zeroelim(e: &[f64], b: f64, h: &mut [f64]) -> usize {
        let (bhi, blo) = split(b);
        let (mut Q, hh) = two_product_presplit(e[0], b, bhi, blo);
        let mut hindex = 0;
        if hh != 0.0 {
            h[hindex] = hh;
            hindex += 1;
        }
        for eindex in 1..e.len() {
            let enow = e[eindex];
            let (product1, product0) = two_product_presplit(enow, b, bhi, blo);
            let (sum, hh) = two_sum(Q, product0);
            if hh != 0.0 {
                h[hindex] = hh;
                hindex += 1;
            }
            let (new_q, hh) = fast_two_sum(product1, sum);
            Q = new_q;
            if hh != 0.0 {
                h[hindex] = hh;
                hindex += 1;
            }
        }
        if Q != 0.0 || hindex == 0 {
            h[hindex] = Q;
            hindex += 1;
        }
        hindex
    }

    #[inline]
    fn two_product(a: f64, b: f64) -> (f64, f64) {
        let x = a * b;
        (x, two_product_tail(a, b, x))
    }

    #[inline]
    fn two_product_tail(a: f64, b: f64, x: f64) -> f64 {
        let (ahi, alo) = split(a);
        let (bhi, blo) = split(b);
        let err1 = x - (ahi * bhi);
        let err2 = err1 - (alo * bhi);
        let err3 = err2 - (ahi * blo);
        (alo * blo) - err3
    }

    #[inline]
    fn split(a: f64) -> (f64, f64) {
        let c = SPLITTER * a;
        let abig = c - a;
        let ahi = c - abig;
        let alo = a - ahi;
        (ahi, alo)
    }

    #[inline]
    fn two_product_presplit(a: f64, b: f64, bhi: f64, blo: f64) -> (f64, f64) {
        let x = a * b;
        let (ahi, alo) = split(a);
        let err1 = x - ahi * bhi;
        let err2 = err1 - alo * bhi;
        let err3 = err2 - ahi * blo;
        let y = alo * blo - err3;
        (x, y)
    }

    #[inline]
    fn two_two_diff(a1: f64, a0: f64, b1: f64, b0: f64) -> (f64, f64, f64, f64) {
        let (j, _r0, x0) = two_one_diff(a1, a0, b0);
        let (x3, x2, x1) = two_one_diff(j, _r0, b1);
        (x3, x2, x1, x0)
    }

    #[inline]
    fn two_one_diff(a1: f64, a0: f64, b: f64) -> (f64, f64, f64) {
        let (i, x0) = two_diff(a0, b);
        let (x2, x1) = two_sum(a1, i);
        (x2, x1, x0)
    }

    #[inline]
    fn two_diff(a: f64, b: f64) -> (f64, f64) {
        let x = a - b;
        (x, two_diff_tail(a, b, x))
    }

    #[inline]
    fn two_diff_tail(a: f64, b: f64, x: f64) -> f64 {
        let bvirt = a - x;
        let avirt = x + bvirt;
        let bround = bvirt - b;
        let around = a - avirt;
        around + bround
    }

    #[inline]
    fn two_sum(a: f64, b: f64) -> (f64, f64) {
        let x = a + b;
        (x, two_sum_tail(a, b, x))
    }

    #[inline]
    fn two_sum_tail(a: f64, b: f64, x: f64) -> f64 {
        let bvirt = x - a;
        let avirt = x - bvirt;
        let bround = b - bvirt;
        let around = a - avirt;
        around + bround
    }

    fn estimate(e: &[f64]) -> f64 {
        let mut q = e[0];
        for cur in &e[1..] {
            q += *cur;
        }
        q
    }

    fn fast_expansion_sum_zeroelim(e: &[f64], f: &[f64], h: &mut [f64]) -> usize {
        let mut enow = e[0];
        let mut fnow = f[0];
        let mut eindex = 0;
        let mut findex = 0;
        let mut Qnew;
        let mut hh;
        let mut Q;
        if (fnow > enow) == (fnow > -enow) {
            Q = enow;
            eindex += 1;
        } else {
            Q = fnow;
            findex += 1;
        }

        let mut hindex = 0;
        if eindex < e.len() && findex < f.len() {
            enow = e[eindex];
            fnow = f[findex];
            if (fnow > enow) == (fnow > -enow) {
                let r = fast_two_sum(enow, Q);
                Qnew = r.0;
                hh = r.1;
                eindex += 1;
            } else {
                let r = fast_two_sum(fnow, Q);
                Qnew = r.0;
                hh = r.1;
                findex += 1;
            }
            Q = Qnew;
            if hh != 0.0 {
                h[hindex] = hh;
                hindex += 1;
            }

            while eindex < e.len() && findex < f.len() {
                enow = e[eindex];
                fnow = f[findex];
                if (fnow > enow) == (fnow > -enow) {
                    let r = two_sum(Q, enow);
                    Qnew = r.0;
                    hh = r.1;
                    eindex += 1;
                } else {
                    let r = two_sum(Q, fnow);
                    Qnew = r.0;
                    hh = r.1;
                    findex += 1;
                };
                Q = Qnew;
                if hh != 0.0 {
                    h[hindex] = hh;
                    hindex += 1;
                }
            }
        }

        while eindex < e.len() {
            enow = e[eindex];
            let r = two_sum(Q, enow);
            Qnew = r.0;
            hh = r.1;
            Q = Qnew;
            eindex += 1;
            if hh != 0.0 {
                h[hindex] = hh;
                hindex += 1
            }
        }

        while findex < f.len() {
            fnow = f[findex];
            let r = two_sum(Q, fnow);
            Qnew = r.0;
            hh = r.1;
            Q = Qnew;
            findex += 1;
            if hh != 0.0 {
                h[hindex] = hh;
                hindex += 1
            }
        }

        if Q != 0.0 || hindex == 0 {
            h[hindex] = Q;
            hindex += 1;
        }
        hindex
    }

    #[inline]
    fn fast_two_sum_tail(a: f64, b: f64, x: f64) -> f64 {
        let bvirt = x - a;
        b - bvirt
    }

    #[inline]
    fn fast_two_sum(a: f64, b: f64) -> (f64, f64) {
        let x = a + b;
        (x, fast_two_sum_tail(a, b, x))
    }

    #[inline]
    fn square_tail(a: f64, x: f64) -> f64 {
        let (ahi, alo) = split(a);
        let err1 = x - ahi * ahi;
        let err3 = err1 - (ahi + ahi) * alo;
        alo * alo - err3
    }

    #[inline]
    fn square(a: f64) -> (f64, f64) {
        let x = a * a;
        (x, square_tail(a, x))
    }

    #[inline]
    fn two_one_sum(a1: f64, a0: f64, b: f64) -> (f64, f64, f64) {
        let (_i, x0) = two_sum(a0, b);
        let (x2, x1) = two_sum(a1, _i);
        (x2, x1, x0)
    }

    #[inline]
    fn two_two_sum(a1: f64, a0: f64, b1: f64, b0: f64) -> (f64, f64, f64, f64) {
        let (_j, _r0, x0) = two_one_sum(a1, a0, b0);
        let (x3, x2, x1) = two_one_sum(_j, _r0, b1);
        (x3, x2, x1, x0)
    }

    #[inline]
    fn two_one_product(a1: f64, a0: f64, b: f64) -> (f64, f64, f64, f64) {
        let (bhi, blo) = split(b);
        let (mut _i, x0) = two_product_presplit(a0, b, bhi, blo);
        let (mut _j, _0) = two_product_presplit(a1, b, bhi, blo);
        let (_k, x1) = two_sum(_i, _0);
        let (x3, x2) = fast_two_sum(_j, _k);
        (x3, x2, x1, x0)
    }

    // f64::abs is not yet available in libcore, so we’ll need to use a separate
    // crate for this functionality.
    //
    // https://github.com/rust-lang/rust/issues/50145
    #[cfg(feature = "no_std")]
    #[inline]
    fn abs(x: f64) -> f64 {
        ieee754::Ieee754::abs(x)
    }
    #[cfg(not(feature = "no_std"))]
    #[inline]
    fn abs(x: f64) -> f64 {
        x.abs()
    }
}
mod delaunator {
    /*!
    A very fast 2D [Delaunay Triangulation](https://en.wikipedia.org/wiki/Delaunay_triangulation) library for Rust.
    A port of [Delaunator](https://github.com/mapbox/delaunator).

    # Example

    ```rust
    use delaunator::{Point, triangulate};

    let points = vec![
        Point { x: 0., y: 0. },
        Point { x: 1., y: 0. },
        Point { x: 1., y: 1. },
        Point { x: 0., y: 1. },
    ];

    let result = triangulate(&points);
    println!("{:?}", result.triangles); // [0, 2, 1, 0, 3, 2]
    ```
    */

    #![no_std]
    #![allow(clippy::many_single_char_names)]

    #[cfg(feature = "std")]
    extern crate std;

    use alloc::vec::Vec;
    use core::{cmp::Ordering, f64, fmt};
    use robust::orient2d;

    use crate::robust;

    /// Near-duplicate points (where both `x` and `y` only differ within this value)
    /// will not be included in the triangulation for robustness.
    pub const EPSILON: f64 = f64::EPSILON * 2.0;

    /// Represents a 2D point in the input vector.
    #[derive(Clone, PartialEq, Default)]
    pub struct Point {
        pub x: f64,
        pub y: f64,
    }

    impl fmt::Debug for Point {
        fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
            write!(f, "[{}, {}]", self.x, self.y)
        }
    }

    impl From<&Point> for robust::Coord<f64> {
        fn from(p: &Point) -> robust::Coord<f64> {
            robust::Coord::<f64> { x: p.x, y: p.y }
        }
    }

    impl Point {
        pub fn dist2(&self, p: &Self) -> f64 {
            let dx = self.x - p.x;
            let dy = self.y - p.y;
            dx * dx + dy * dy
        }

        /// Returns a **negative** value if ```self```, ```q``` and ```r``` occur in counterclockwise order (```r``` is to the left of the directed line ```self``` --> ```q```)
        /// Returns a **positive** value if they occur in clockwise order(```r``` is to the right of the directed line ```self``` --> ```q```)
        /// Returns zero is they are collinear
        fn orient(&self, q: &Self, r: &Self) -> f64 {
            // robust-rs orients Y-axis upwards, our convention is Y downwards. This means that the interpretation of the result must be flipped
            orient2d(self.into(), q.into(), r.into())
        }

        fn circumdelta(&self, b: &Self, c: &Self) -> (f64, f64) {
            let dx = b.x - self.x;
            let dy = b.y - self.y;
            let ex = c.x - self.x;
            let ey = c.y - self.y;

            let bl = dx * dx + dy * dy;
            let cl = ex * ex + ey * ey;
            let d = 0.5 / (dx * ey - dy * ex);

            let x = (ey * bl - dy * cl) * d;
            let y = (dx * cl - ex * bl) * d;
            (x, y)
        }

        fn circumradius2(&self, b: &Self, c: &Self) -> f64 {
            let (x, y) = self.circumdelta(b, c);
            x * x + y * y
        }

        fn circumcenter(&self, b: &Self, c: &Self) -> Self {
            let (x, y) = self.circumdelta(b, c);
            Self {
                x: self.x + x,
                y: self.y + y,
            }
        }

        fn in_circle(&self, b: &Self, c: &Self, p: &Self) -> bool {
            let dx = self.x - p.x;
            let dy = self.y - p.y;
            let ex = b.x - p.x;
            let ey = b.y - p.y;
            let fx = c.x - p.x;
            let fy = c.y - p.y;

            let ap = dx * dx + dy * dy;
            let bp = ex * ex + ey * ey;
            let cp = fx * fx + fy * fy;

            dx * (ey * cp - bp * fy) - dy * (ex * cp - bp * fx) + ap * (ex * fy - ey * fx) < 0.0
        }

        fn nearly_equals(&self, p: &Self) -> bool {
            f64_abs(self.x - p.x) <= EPSILON && f64_abs(self.y - p.y) <= EPSILON
        }
    }

    /// Represents the area outside of the triangulation.
    /// Halfedges on the convex hull (which don't have an adjacent halfedge)
    /// will have this value.
    pub const EMPTY: usize = usize::max_value();

    /// Next halfedge in a triangle.
    pub fn next_halfedge(i: usize) -> usize {
        if i % 3 == 2 { i - 2 } else { i + 1 }
    }

    /// Previous halfedge in a triangle.
    pub fn prev_halfedge(i: usize) -> usize {
        if i % 3 == 0 { i + 2 } else { i - 1 }
    }

    /// Result of the Delaunay triangulation.
    #[derive(Debug, Clone)]
    pub struct Triangulation {
        /// A vector of point indices where each triple represents a Delaunay triangle.
        /// All triangles are directed counter-clockwise.
        pub triangles: Vec<usize>,

        /// A vector of adjacent halfedge indices that allows traversing the triangulation graph.
        ///
        /// `i`-th half-edge in the array corresponds to vertex `triangles[i]`
        /// the half-edge is coming from. `halfedges[i]` is the index of a twin half-edge
        /// in an adjacent triangle (or `EMPTY` for outer half-edges on the convex hull).
        pub halfedges: Vec<usize>,

        /// A vector of indices that reference points on the convex hull of the triangulation,
        /// counter-clockwise.
        pub hull: Vec<usize>,
    }

    impl Triangulation {
        fn new(n: usize) -> Self {
            let max_triangles = if n > 2 { 2 * n - 5 } else { 0 };

            Self {
                triangles: Vec::with_capacity(max_triangles * 3),
                halfedges: Vec::with_capacity(max_triangles * 3),
                hull: Vec::new(),
            }
        }

        /// The number of triangles in the triangulation.
        pub fn len(&self) -> usize {
            self.triangles.len() / 3
        }

        pub fn is_empty(&self) -> bool {
            self.triangles.is_empty()
        }

        fn add_triangle(
            &mut self,
            i0: usize,
            i1: usize,
            i2: usize,
            a: usize,
            b: usize,
            c: usize,
        ) -> usize {
            let t = self.triangles.len();

            self.triangles.push(i0);
            self.triangles.push(i1);
            self.triangles.push(i2);

            self.halfedges.push(a);
            self.halfedges.push(b);
            self.halfedges.push(c);

            if a != EMPTY {
                self.halfedges[a] = t;
            }
            if b != EMPTY {
                self.halfedges[b] = t + 1;
            }
            if c != EMPTY {
                self.halfedges[c] = t + 2;
            }

            t
        }

        fn legalize(&mut self, a: usize, points: &[Point], hull: &mut Hull) -> usize {
            let b = self.halfedges[a];

            // if the pair of triangles doesn't satisfy the Delaunay condition
            // (p1 is inside the circumcircle of [p0, pl, pr]), flip them,
            // then do the same check/flip recursively for the new pair of triangles
            //
            //           pl                    pl
            //          /||\                  /  \
            //       al/ || \bl            al/    \a
            //        /  ||  \              /      \
            //       /  a||b  \    flip    /___ar___\
            //     p0\   ||   /p1   =>   p0\---bl---/p1
            //        \  ||  /              \      /
            //       ar\ || /br             b\    /br
            //          \||/                  \  /
            //           pr                    pr
            //
            let ar = prev_halfedge(a);

            if b == EMPTY {
                return ar;
            }

            let al = next_halfedge(a);
            let bl = prev_halfedge(b);

            let p0 = self.triangles[ar];
            let pr = self.triangles[a];
            let pl = self.triangles[al];
            let p1 = self.triangles[bl];

            let illegal = points[p0].in_circle(&points[pr], &points[pl], &points[p1]);
            if illegal {
                self.triangles[a] = p1;
                self.triangles[b] = p0;

                let hbl = self.halfedges[bl];
                let har = self.halfedges[ar];

                // edge swapped on the other side of the hull (rare); fix the halfedge reference
                if hbl == EMPTY {
                    let mut e = hull.start;
                    loop {
                        if hull.tri[e] == bl {
                            hull.tri[e] = a;
                            break;
                        }
                        e = hull.prev[e];
                        if e == hull.start {
                            break;
                        }
                    }
                }

                self.halfedges[a] = hbl;
                self.halfedges[b] = har;
                self.halfedges[ar] = bl;

                if hbl != EMPTY {
                    self.halfedges[hbl] = a;
                }
                if har != EMPTY {
                    self.halfedges[har] = b;
                }
                if bl != EMPTY {
                    self.halfedges[bl] = ar;
                }

                let br = next_halfedge(b);

                self.legalize(a, points, hull);
                return self.legalize(br, points, hull);
            }
            ar
        }
    }

    // data structure for tracking the edges of the advancing convex hull
    struct Hull {
        prev: Vec<usize>,
        next: Vec<usize>,
        tri: Vec<usize>,
        hash: Vec<usize>,
        start: usize,
        center: Point,
    }

    impl Hull {
        fn new(n: usize, center: Point, i0: usize, i1: usize, i2: usize, points: &[Point]) -> Self {
            let hash_len = f64_sqrt(n as f64) as usize;

            let mut hull = Self {
                prev: vec![0; n],            // edge to prev edge
                next: vec![0; n],            // edge to next edge
                tri: vec![0; n],             // edge to adjacent halfedge
                hash: vec![EMPTY; hash_len], // angular edge hash
                start: i0,
                center,
            };

            hull.next[i0] = i1;
            hull.prev[i2] = i1;
            hull.next[i1] = i2;
            hull.prev[i0] = i2;
            hull.next[i2] = i0;
            hull.prev[i1] = i0;

            hull.tri[i0] = 0;
            hull.tri[i1] = 1;
            hull.tri[i2] = 2;

            hull.hash_edge(&points[i0], i0);
            hull.hash_edge(&points[i1], i1);
            hull.hash_edge(&points[i2], i2);

            hull
        }

        fn hash_key(&self, p: &Point) -> usize {
            let dx = p.x - self.center.x;
            let dy = p.y - self.center.y;

            let p = dx / (f64_abs(dx) + f64_abs(dy));
            let a = (if dy > 0.0 { 3.0 - p } else { 1.0 + p }) / 4.0; // [0..1]

            let len = self.hash.len();
            (f64_floor((len as f64) * a) as usize) % len
        }

        fn hash_edge(&mut self, p: &Point, i: usize) {
            let key = self.hash_key(p);
            self.hash[key] = i;
        }

        fn find_visible_edge(&self, p: &Point, points: &[Point]) -> (usize, bool) {
            let mut start: usize = 0;
            let key = self.hash_key(p);
            let len = self.hash.len();
            for j in 0..len {
                start = self.hash[(key + j) % len];
                if start != EMPTY && self.next[start] != EMPTY {
                    break;
                }
            }
            start = self.prev[start];
            let mut e = start;

            while p.orient(&points[e], &points[self.next[e]]) <= 0. {
                e = self.next[e];
                if e == start {
                    return (EMPTY, false);
                }
            }
            (e, e == start)
        }
    }

    fn calc_bbox_center(points: &[Point]) -> Point {
        let mut min_x = f64::INFINITY;
        let mut min_y = f64::INFINITY;
        let mut max_x = f64::NEG_INFINITY;
        let mut max_y = f64::NEG_INFINITY;
        for p in points.iter() {
            min_x = min_x.min(p.x);
            min_y = min_y.min(p.y);
            max_x = max_x.max(p.x);
            max_y = max_y.max(p.y);
        }
        Point {
            x: (min_x + max_x) / 2.0,
            y: (min_y + max_y) / 2.0,
        }
    }

    fn find_closest_point(points: &[Point], p0: &Point) -> Option<usize> {
        let mut min_dist = f64::INFINITY;
        let mut k: usize = 0;
        for (i, p) in points.iter().enumerate() {
            let d = p0.dist2(p);
            if d > 0.0 && d < min_dist {
                k = i;
                min_dist = d;
            }
        }
        if min_dist == f64::INFINITY {
            None
        } else {
            Some(k)
        }
    }

    fn find_seed_triangle(points: &[Point]) -> Option<(usize, usize, usize)> {
        // pick a seed point close to the center
        let bbox_center = calc_bbox_center(points);
        let i0 = find_closest_point(points, &bbox_center)?;
        let p0 = &points[i0];

        // find the point closest to the seed
        let i1 = find_closest_point(points, p0)?;
        let p1 = &points[i1];

        // find the third point which forms the smallest circumcircle with the first two
        let mut min_radius = f64::INFINITY;
        let mut i2: usize = 0;
        for (i, p) in points.iter().enumerate() {
            if i == i0 || i == i1 {
                continue;
            }
            let r = p0.circumradius2(p1, p);
            if r < min_radius {
                i2 = i;
                min_radius = r;
            }
        }

        if min_radius == f64::INFINITY {
            None
        } else {
            // swap the order of the seed points for counter-clockwise orientation
            Some(if p0.orient(p1, &points[i2]) > 0. {
                (i0, i2, i1)
            } else {
                (i0, i1, i2)
            })
        }
    }

    fn sortf(f: &mut [(usize, f64)]) {
        f.sort_unstable_by(|&(_, da), &(_, db)| da.partial_cmp(&db).unwrap_or(Ordering::Equal));
    }

    /// Order collinear points by dx (or dy if all x are identical) and return the list as a hull
    fn handle_collinear_points(points: &[Point]) -> Triangulation {
        let Point { x, y } = points.first().cloned().unwrap_or_default();

        let mut dist: Vec<_> = points
            .iter()
            .enumerate()
            .map(|(i, p)| {
                let mut d = p.x - x;
                if d == 0.0 {
                    d = p.y - y;
                }
                (i, d)
            })
            .collect();
        sortf(&mut dist);

        let mut triangulation = Triangulation::new(0);
        let mut d0 = f64::NEG_INFINITY;
        for (i, distance) in dist {
            if distance > d0 {
                triangulation.hull.push(i);
                d0 = distance;
            }
        }

        triangulation
    }

    /// Triangulate a set of 2D points.
    /// Returns the triangulation for the input points.
    /// For the degenerated case when all points are collinear, returns an empty triangulation where all points are in the hull.
    pub fn triangulate(points: &[Point]) -> Triangulation {
        let seed_triangle = find_seed_triangle(points);
        if seed_triangle.is_none() {
            return handle_collinear_points(points);
        }

        let n = points.len();
        let (i0, i1, i2) =
            seed_triangle.expect("At this stage, points are guaranteed to yeild a seed triangle");
        let center = points[i0].circumcenter(&points[i1], &points[i2]);

        let mut triangulation = Triangulation::new(n);
        triangulation.add_triangle(i0, i1, i2, EMPTY, EMPTY, EMPTY);

        // sort the points by distance from the seed triangle circumcenter
        let mut dists: Vec<_> = points
            .iter()
            .enumerate()
            .map(|(i, point)| (i, center.dist2(point)))
            .collect();

        sortf(&mut dists);

        let mut hull = Hull::new(n, center, i0, i1, i2, points);

        for (k, &(i, _)) in dists.iter().enumerate() {
            let p = &points[i];

            // skip near-duplicates
            if k > 0 && p.nearly_equals(&points[dists[k - 1].0]) {
                continue;
            }
            // skip seed triangle points
            if i == i0 || i == i1 || i == i2 {
                continue;
            }

            // find a visible edge on the convex hull using edge hash
            let (mut e, walk_back) = hull.find_visible_edge(p, points);
            if e == EMPTY {
                continue; // likely a near-duplicate point; skip it
            }

            // add the first triangle from the point
            let t = triangulation.add_triangle(e, i, hull.next[e], EMPTY, EMPTY, hull.tri[e]);

            // recursively flip triangles from the point until they satisfy the Delaunay condition
            hull.tri[i] = triangulation.legalize(t + 2, points, &mut hull);
            hull.tri[e] = t; // keep track of boundary triangles on the hull

            // walk forward through the hull, adding more triangles and flipping recursively
            let mut n = hull.next[e];
            loop {
                let q = hull.next[n];
                if p.orient(&points[n], &points[q]) <= 0. {
                    break;
                }
                let t = triangulation.add_triangle(n, i, q, hull.tri[i], EMPTY, hull.tri[n]);
                hull.tri[i] = triangulation.legalize(t + 2, points, &mut hull);
                hull.next[n] = EMPTY; // mark as removed
                n = q;
            }

            // walk backward from the other side, adding more triangles and flipping
            if walk_back {
                loop {
                    let q = hull.prev[e];
                    if p.orient(&points[q], &points[e]) <= 0. {
                        break;
                    }
                    let t = triangulation.add_triangle(q, i, e, EMPTY, hull.tri[e], hull.tri[q]);
                    triangulation.legalize(t + 2, points, &mut hull);
                    hull.tri[q] = t;
                    hull.next[e] = EMPTY; // mark as removed
                    e = q;
                }
            }

            // update the hull indices
            hull.prev[i] = e;
            hull.next[i] = n;
            hull.prev[n] = i;
            hull.next[e] = i;
            hull.start = e;

            // save the two new edges in the hash table
            hull.hash_edge(p, i);
            hull.hash_edge(&points[e], e);
        }

        // expose hull as a vector of point indices
        let mut e = hull.start;
        loop {
            triangulation.hull.push(e);
            e = hull.next[e];
            if e == hull.start {
                break;
            }
        }

        triangulation.triangles.shrink_to_fit();
        triangulation.halfedges.shrink_to_fit();

        triangulation
    }

    #[cfg(feature = "std")]
    #[inline]
    fn f64_abs(f: f64) -> f64 {
        f.abs()
    }

    #[cfg(not(feature = "std"))]
    #[inline]
    fn f64_abs(f: f64) -> f64 {
        const SIGN_BIT: u64 = 1 << 63;
        f64::from_bits(f64::to_bits(f) & !SIGN_BIT)
    }

    #[cfg(feature = "std")]
    #[inline]
    fn f64_floor(f: f64) -> f64 {
        f.floor()
    }

    #[cfg(not(feature = "std"))]
    #[inline]
    fn f64_floor(f: f64) -> f64 {
        let mut res = (f as i64) as f64;
        if res > f {
            res -= 1.0;
        }
        res as f64
    }

    #[cfg(feature = "std")]
    #[inline]
    fn f64_sqrt(f: f64) -> f64 {
        f.sqrt()
    }

    #[cfg(not(feature = "std"))]
    #[inline]
    fn f64_sqrt(f: f64) -> f64 {
        if f < 2.0 {
            return f;
        };

        let sc = f64_sqrt(f / 4.0) * 2.0;
        let lc = sc + 1.0;

        if lc * lc > f { sc } else { lc }
    }
}

// Union-Find (Disjoint Set Union) Structure
struct UnionFind {
    parent: Vec<usize>,
    rank: Vec<usize>,
}

impl UnionFind {
    fn new(n: usize) -> Self {
        UnionFind {
            parent: (0..n).collect(),
            rank: vec![0; n],
        }
    }

    // Find the root of the set containing `i`, with path compression.
    fn find(&mut self, i: usize) -> usize {
        if self.parent[i] == i {
            i
        } else {
            let root = self.find(self.parent[i]);
            self.parent[i] = root; // Path compression
            root
        }
    }

    // Unite the sets containing `i` and `j`, by rank.
    // Returns true if they were different sets (union succeeded), false otherwise.
    fn unite(&mut self, i: usize, j: usize) -> bool {
        let root_i = self.find(i);
        let root_j = self.find(j);
        if root_i != root_j {
            // Union by rank
            if self.rank[root_i] < self.rank[root_j] {
                self.parent[root_i] = root_j;
            } else {
                self.parent[root_j] = root_i;
                if self.rank[root_i] == self.rank[root_j] {
                    self.rank[root_i] += 1;
                }
            }
            true
        } else {
            false // Already in the same set
        }
    }

    // Check if i and j are in the same set.
    #[allow(dead_code)]
    fn same(&mut self, i: usize, j: usize) -> bool {
        self.find(i) == self.find(j)
    }

    fn components(&mut self) -> Vec<Vec<usize>> {
        let mut components: Vec<Vec<usize>> = vec![Vec::new(); self.parent.len()];
        for i in 0..self.parent.len() {
            let root = self.find(i);
            components[root].push(i);
        }
        components.into_iter().filter(|c| !c.is_empty()).collect()
    }
}
use num_traits::{Float, One, ToPrimitive, Zero};
/// Represents a 2D point in the input vector.
use std::ops::{Add, AddAssign, Div, DivAssign, Mul, MulAssign, Neg, Sub, SubAssign}; // Add num_traits for Float, Zero, One

#[cfg_attr(feature = "visualization", derive(Serialize, Deserialize))]
#[derive(Clone, Copy, PartialEq, Default, Debug)] // Use PartialEq, suitable for floats too
pub struct Point<T = i64> {
    pub x: T,
    pub y: T,
}

trait SQRTable {
    fn sqrt_(self) -> Self;
}
impl SQRTable for f64 {
    #[inline(always)]
    fn sqrt_(self) -> Self {
        return self.sqrt();
    }
}

const TABLE_SIZE: usize = 120000; // テーブルの最大値
const fn sqrt_approx(x: i64) -> i32 {
    if x == 0 {
        return 0;
    }
    let mut left = 1;
    let mut right = (x >> 1) + 1;
    while left < right {
        let mid = (left + right) >> 1;
        if mid * mid > x {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    return (left - 1) as i32; // leftが最初にxを超えたときの値がsqrt(x)の近似値になる
}
const fn generate_sqrt_table() -> [i32; TABLE_SIZE] {
    let mut table = [0; TABLE_SIZE];
    let mut i = 0;
    while i < TABLE_SIZE {
        table[i] = sqrt_approx(i as i64);
        i += 1;
    }
    table
}
const SQRT_TABLE: [i32; TABLE_SIZE] = generate_sqrt_table();
impl SQRTable for i64 {
    #[inline(always)]
    fn sqrt_(self) -> Self {
        if self < TABLE_SIZE as i64 {
            return SQRT_TABLE[self as usize] as i64;
        }
        return (self as f64).sqrt() as i64;
    }
}
impl SQRTable for i32 {
    #[inline(always)]
    fn sqrt_(self) -> Self {
        self.sqrt()
    }
}

trait Num_:
    Copy
    + Zero
    + One
    + ToPrimitive
    + fmt::Debug
    + fmt::Display
    + SQRTable
    + Sub<Output = Self>
    + SubAssign
    + Add<Output = Self>
    + AddAssign
    + Mul<Output = Self>
    + MulAssign
    + Div<Output = Self>
    + DivAssign
    + Neg<Output = Self>
    + PartialOrd
    + PartialEq
{
}
impl Num_ for f64 {}
impl Num_ for i64 {}
impl Num_ for i32 {}

impl<T: Num_> Point<T> {
    /// Creates a new 2D point from its x and y coordinates.
    #[inline(always)]
    pub fn new(x: T, y: T) -> Self {
        Point { x, y }
    }

    // Accessors are less necessary now since fields are public,
    // but can keep them for consistency if preferred.
    // Let's remove them for simplicity as `p.x`, `p.y` are direct.

    /// Calculates the squared Euclidean distance between two points.
    /// This avoids the square root calculation and is often sufficient for comparisons.
    /// Requires T to support subtraction, addition, multiplication, and copying.
    #[inline(always)]
    pub fn dist_sq(&self, other: &Self) -> T
    where
        T: Sub<Output = T> + Add<Output = T> + Mul<Output = T> + Copy,
    {
        let dx = self.x - other.x;
        let dy = self.y - other.y;
        dx * dx + dy * dy
    }
}

impl Point<i64> {
    /// Creates a new 2D point from its x and y coordinates.
    #[inline(always)]
    pub fn dist_sq_i64(&self, other: &Point) -> i64 {
        let dx: i64 = self.x.wrapping_sub(other.x); // wrapping_sub でオーバーフローをラップアラウンド
        let dy = self.y.wrapping_sub(other.y);
        dx.wrapping_mul(dx).wrapping_add(dy.wrapping_mul(dy))
    }
}

impl<T: Num_> Point<T> {
    #[inline(always)]
    pub fn dist(&self, other: &Self) -> T {
        self.dist_sq(other).sqrt_()
    }
}

// --- Implementations for Floating Point Types ---
impl<T: Num_> Point<T> {
    /// Calculates the squared length (magnitude) of the point vector from the origin.
    #[inline(always)]
    pub fn length_sq(&self) -> T {
        self.x * self.x + self.y * self.y
    }

    /// Calculates the length (magnitude) of the point vector from the origin.
    #[inline(always)]
    pub fn length(&self) -> T {
        self.length_sq().sqrt_()
    }

    /// Returns a normalized version of the point (unit vector).
    /// If the length is zero, returns a zero point.
    #[inline(always)]
    pub fn normalize(&self) -> Self {
        let len = self.length();
        let zero = T::zero();

        if len > zero {
            let inv_len = T::one() / len;
            Point::new(self.x * inv_len, self.y * inv_len)
        } else {
            Point::new(zero, zero)
        }
    }

    #[inline(always)]
    fn add(self, other: Self) -> Self {
        Point::new(self.x + other.x, self.y + other.y)
    }
    #[inline(always)]
    fn sub(self, other: Self) -> Self {
        Point::new(self.x - other.x, self.y - other.y)
    }
    #[inline(always)]
    fn mul(self, scalar: T) -> Self {
        Point::new(self.x * scalar, self.y * scalar)
    }
}

// --- Optional: From<(T, T)> ---
impl<T> From<(T, T)> for Point<T> {
    #[inline(always)]
    fn from(tuple: (T, T)) -> Self {
        Point {
            x: tuple.0,
            y: tuple.1,
        } // Directly construct with named fields
    }
}

// --- Feature gate for visualization ---
use once_cell::sync::Lazy; // または unsync::Lazy も可
#[cfg(feature = "visualization")]
use serde::{Deserialize, Serialize};
#[cfg(feature = "visualization")]
use serde_json;
use std::env; // 実行時に環境変数を読むために必要

// マクロを少し変えて、直接 Lazy static を定義するようにする
macro_rules! define_meta_param {
    ($name:ident, $type:ty, $env_var_name:expr, $default_value:expr) => {
        static $name: Lazy<$type> = Lazy::new(|| {
            // 環境変数読み込みとパースロジック (実行時)
            let value = if cfg!(feature = "local_env") {
                env::var($env_var_name)
                    .ok()
                    .and_then(|s| s.parse::<$type>().ok())
                    .unwrap_or($default_value)
            } else {
                $default_value
            };
            value
        });
    };
}

const TIME_LIMIT: f64 = 1.98;
define_meta_param!(POST_ADJUST_TIME_LIMIT, f64, "POST_ADJUST_TIME_LIMIT", 0.1);
define_meta_param!(MST_CUT_TIME_LIMIT, f64, "MST_CUT_TIME_LIMIT", 0.3);

define_meta_param!(N_PARTICLES, usize, "N_PARTICLES", 10); // 1 ~ 100
define_meta_param!(MH_STEPS_PER_PARTICLE, usize, "MH_STEPS_PER_PARTICLE", 10000); //1 ~ 10000
define_meta_param!(MAX_DELTA, i32, "MAX_DELTA", 337); // 1 ~ 1000
define_meta_param!(N_QUERY_CANDIDATES, usize, "N_QUERY_CANDIDATES", 400); // 1 ~ 100
define_meta_param!(N_QUERY_RET, usize, "N_QUERY_RET", 44); // 1 ~ 100
define_meta_param!(N_QUERY_ALLOW_DUP_RATE, f64, "N_QUERY_ALLOW_DUP_RATE", 0.1);
define_meta_param!(
    MST_FINAL_PENALTY_WEIGHT,
    f64,
    "MST_FINAL_PENALTY_WEIGHT",
    240.0
);
define_meta_param!(
    MST_ANNEAL_START_TEMP_RATIO,
    f64,
    "MST_ANNEAL_START_TEMP_RATIO",
    0.01
); // MHの温度パラメータ 0 ~ 1
define_meta_param!(SAMPLE_RECT_W, i64, "SAMPLE_RECT_W", 10); // 1 ~ 1000
define_meta_param!(SAMPLE_RECT_H, i64, "SAMPLE_RECT_H", 10); // 1 ~ 1000
define_meta_param!(DIRECT_OPT_LOWER, usize, "DIRECT_OPT_LOWER", 10); // 3 ~ 15

#[cfg_attr(feature = "visualization", derive(Serialize, Deserialize))]
#[derive(Clone, Copy, Debug)]
struct Rect {
    lx: i64,
    rx: i64,
    ly: i64,
    ry: i64,
}
impl Rect {
    #[inline(always)]
    fn center(&self) -> Point<i64> {
        Point {
            x: (self.lx + self.rx) / 2,
            y: (self.ly + self.ry) / 2,
        }
    }
    #[inline(always)]
    fn area(&self) -> i64 {
        ((self.rx - self.lx) * (self.ry - self.ly)).max(0)
    }
    #[inline(always)]
    fn clamp(&self, p: Point<i64>) -> Point<i64> {
        Point {
            x: p.x.clamp(self.lx, self.rx),
            y: p.y.clamp(self.ly, self.ry),
        }
    }
    #[inline(always)]
    fn random_point(&self, rng: &mut impl Rng) -> Point<i64> {
        Point {
            x: rng.gen_range(self.lx..=self.rx),
            y: rng.gen_range(self.ly..=self.ry),
        }
    }
    fn intersect(&self, other: &Self) -> Self {
        Rect {
            lx: max(self.lx, other.lx),
            rx: min(self.rx, other.rx),
            ly: max(self.ly, other.ly),
            ry: min(self.ry, other.ry),
        }
    }
    fn corner(&self, corner_idx: usize) -> Point<i64> {
        match corner_idx {
            0 => Point {
                x: self.lx,
                y: self.ly,
            },
            1 => Point {
                x: self.rx,
                y: self.ly,
            },
            2 => Point {
                x: self.rx,
                y: self.ry,
            },
            3 => Point {
                x: self.lx,
                y: self.ry,
            },
            _ => panic!("Invalid corner index"),
        }
    }
}

#[derive(Clone, Debug)]
struct City {
    id: usize,
    rect: Rect,
}

// --- MST Calculation (Prim's Algorithm, same as before) ---
#[derive(PartialEq)]
struct PrimEdge<T: Num_> {
    cost: T,
    to: usize,
    from: usize,
}
impl<T: Num_> PartialOrd for PrimEdge<T> {
    #[inline(always)]
    fn partial_cmp(&self, other: &Self) -> Option<cmp::Ordering> {
        other.cost.partial_cmp(&self.cost)
    }
}
impl<T: Num_> Ord for PrimEdge<T> {
    #[inline(always)]
    fn cmp(&self, other: &Self) -> cmp::Ordering {
        self.partial_cmp(other).unwrap_or(Ordering::Equal)
    }
}
impl<T: Num_> Eq for PrimEdge<T> {}

fn save_points_as_svg(
    points: &Vec<(i32, i32)>,
    edges: &Vec<(usize, usize)>,
    filename: &str,
) -> io::Result<()> {
    let mut file = File::create(filename)?;

    let minX = points.iter().map(|p| p.0).min().unwrap() - 100;
    let minY = points.iter().map(|p| p.1).min().unwrap() - 100;
    let maxX = points.iter().map(|p| p.0).max().unwrap() + 100;
    let maxY = points.iter().map(|p| p.1).max().unwrap() + 100;

    let width = maxX - minX;
    let height = maxY - minY;

    writeln!(
        file,
        r#"<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="{minX} {minY} {width} {height}" style="background-color: white">"#
    )?;
    writeln!(file, r#"<rect width="100%" height="100%" fill="white"/>"#)?;
    writeln!(file, r#"<g stroke="black" fill="none">"#)?;
    for (x, y) in points {
        writeln!(file, r#"<circle cx="{x}" cy="{y}" r="10" fill="black"/>"#)?;
    }
    for (i, j) in edges {
        let (x1, y1) = points[*i];
        let (x2, y2) = points[*j];
        writeln!(
            file,
            r#"<line x1="{x1}" y1="{y1}" x2="{x2}" y2="{y2}" stroke="black" stroke-width="5"/>"#
        )?;
    }
    writeln!(file, r#"</g>"#)?;
    writeln!(file, r#"</svg>"#)?;
    Ok(())
}

fn build_mst(points: &[delaunator::Point]) -> (f64, Vec<(usize, usize)>) {
    let n = points.len();
    if n < 2 {
        return (0.0, Vec::new());
    }

    let triangulation = delaunator::triangulate(points);

    // Handle cases with few points or collinear points explicitly returned by triangulate
    if n == 2 {
        let dist = (points[0].x - points[1].x).hypot(points[0].y - points[1].y);
        return (dist, vec![(0, 1)]); // MST is just the single edge
    }
    let mut edges: Vec<(f64, usize, usize)> = Vec::new();

    for i in 0..triangulation.triangles.len() {
        let p = triangulation.triangles[i];
        let q = triangulation.triangles[delaunator::next_halfedge(i)];

        // Ensure we add edges only once, e.g., when p < q
        if p != q {
            let dist = (points[p].x - points[q].x).hypot(points[p].y - points[q].y);
            edges.push((dist, p, q)); // Store distance squared initially
        }
    }

    // Sort edges by weight (distance squared)
    edges.sort_unstable_by(|a, b| a.0.partial_cmp(&b.0).unwrap_or(Ordering::Equal));

    // Kruskal's algorithm using Union-Find
    let mut uf = UnionFind::new(n);
    let mut mst_total_dist = 0.0;
    let mut mst_edges: Vec<(usize, usize)> = Vec::new();
    let mut edges_count = 0;

    for (dist, u, v) in edges {
        // Check for potential floating point issues leading to dist_sq = 0 for distinct points
        if dist <= delaunator::EPSILON && u != v {
            // Skip edges with near-zero length between different indices unless needed
            // But Kruskal should handle this fine if UF unites them.
            // Let's proceed normally.
        }

        if uf.unite(u, v) {
            mst_total_dist += dist; // Add the actual distance to the total cost
            mst_edges.push((u, v));
            edges_count += 1;
            if edges_count == n - 1 {
                break; // MST is complete
            }
        }
    }

    // Check if MST is fully formed (might not be if graph was disconnected,
    // though Delaunay of non-degenerate points should be connected).
    if edges_count != n - 1 && n > 1 {
        // This might happen with duplicate points sufficiently far apart
        // that they weren't skipped but formed separate components.
        // eprintln!(
        //     "Warning: MST may not be complete. Edges found: {}, Expected: {} num_triangles: {}",
        //     edges_count,
        //     n - 1,
        //     triangulation.triangles.len()
        // );

        // eprintln!("points: {:?}", points);
        save_points_as_svg(
            &points.iter().map(|p| (p.x as i32, p.y as i32)).collect(),
            &mst_edges,
            "mst.svg",
        );
    }

    (mst_total_dist, mst_edges)
}

// --- fast_mst: Adapter function using Fortune's algorithm ---
// Input: Vec of Point<i64> (original points)
// Output: (Total MST cost (i64), Vec of MST edges (original indices))
fn fast_mst(positions: &[Point<i64>]) -> (i64, Vec<(usize, usize)>) {
    let n_orig = positions.len();
    if n_orig <= 1 {
        return (0, Vec::new());
    }

    let mut rng = Pcg64Mcg::new(0);
    let vpoints = positions
        .iter()
        .map(|&p| delaunator::Point {
            x: p.x as f64 + rng.gen_range(-0.01..=0.01),
            y: p.y as f64 + rng.gen_range(-0.01..=0.01),
        })
        .collect::<Vec<_>>();
    let mst = build_mst(&vpoints);
    return (
        mst.0.to_i64().unwrap(),
        mst.1
            .iter()
            .map(|&(u, v)| (u as usize, v as usize))
            .collect(),
    );
}
fn naive_mst(positions: &[Point<i64>]) -> (i64, Vec<(usize, usize)>) {
    let n = positions.len();
    let mut total_cost = 0;
    let mut mst_edges = Vec::new();
    let mut visited = vec![false; n];
    let mut pq = BinaryHeap::new();

    visited[0] = true;
    for i in 1..n {
        let cost = positions[0].dist(&positions[i]);
        pq.push(PrimEdge {
            cost,
            to: i,
            from: 0,
        });
    }

    while let Some(PrimEdge { cost, to, from }) = pq.pop() {
        if visited[to] {
            continue;
        }
        visited[to] = true;
        total_cost += cost.to_i64().unwrap();
        mst_edges.push((from, to));

        for next in 0..n {
            if !visited[next] {
                let next_cost = positions[to].dist(&positions[next]);
                pq.push(PrimEdge {
                    cost: next_cost,
                    to: next,
                    from: to,
                });
            }
        }
    }
    (total_cost, mst_edges)
}

fn calculate_mst(
    group_member_indices: &[usize],
    positions: &[Point],
) -> (i64, Vec<(usize, usize)>) {
    let group_size = group_member_indices.len();
    if group_size <= 1 {
        return (0, vec![]);
    }
    let local_to_global = group_member_indices; // Reuse the input slice
    let mst = fast_mst(&local_to_global.iter().map(|&i| positions[i]).collect_vec());
    let edges = mst
        .1
        .iter()
        .map(|&(u, v)| {
            (
                local_to_global[u], // Convert to global index
                local_to_global[v],
            )
        })
        .collect_vec();
    let cost = eval_edges_cost(&edges, positions);
    return (cost, edges);
}

// --- Constraints ---
#[cfg_attr(feature = "visualization", derive(Serialize, Deserialize))]
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
struct InequalityConstraint {
    longer_edge: (usize, usize),
    shorter_edge: (usize, usize),
    id: u64,
}
impl InequalityConstraint {
    fn get_cities(&self) -> Vec<usize> {
        vec![
            self.longer_edge.0,
            self.longer_edge.1,
            self.shorter_edge.0,
            self.shorter_edge.1,
        ]
    }
    fn id_to_constraint(id: u64) -> Self {
        let longer_edge_0 = ((id >> 32) & 0xFFFF) as usize;
        let longer_edge_1 = ((id >> 16) & 0xFFFF) as usize;
        let shorter_edge_0 = ((id >> 8) & 0xFFFF) as usize;
        let shorter_edge_1 = (id & 0xFFFF) as usize;
        InequalityConstraint {
            longer_edge: (longer_edge_0, longer_edge_1),
            shorter_edge: (shorter_edge_0, shorter_edge_1),
            id,
        }
    }
    fn new(longer_edge: (usize, usize), shorter_edge: (usize, usize)) -> Self {
        // Unique ID for the constraint
        let mut id = 0;
        id |= (longer_edge.0 as u64) << 32;
        id |= (longer_edge.1 as u64) << 16;
        id |= (shorter_edge.0 as u64) << 8;
        id |= shorter_edge.1 as u64;
        InequalityConstraint {
            longer_edge,
            shorter_edge,
            id,
        }
    }
    #[inline(always)]
    fn check(&self, cities_pos: &[Point<i64>]) -> bool {
        cities_pos[self.longer_edge.0].dist(&cities_pos[self.longer_edge.1])
            >= cities_pos[self.shorter_edge.0].dist(&cities_pos[self.shorter_edge.1])
    }
    // Calculate continuous penalty (for MH energy)
    #[inline(always)]
    fn penalty(&self, cities_pos: &[Point]) -> i64 {
        // distの方がパフォーマンス出ることも、ままあるな
        let dist_uv_sq = cities_pos[self.longer_edge.0].dist_sq(&cities_pos[self.longer_edge.1]);
        let dist_ab_sq = cities_pos[self.shorter_edge.0].dist_sq(&cities_pos[self.shorter_edge.1]);
        return dist_ab_sq.wrapping_sub(dist_uv_sq).max(0);
    }
}

fn min_max_dist_rects(cities: &[City], edge: (usize, usize)) -> (i64, i64) {
    // Calculate the maximum distance between two rectangles
    // 4 x 4 = 16 combinations
    let mut min_dist = i64::MAX;
    let mut max_dist = i64::MIN;
    for i in 0..4 {
        for j in 0..4 {
            let dist = cities[edge.0]
                .rect
                .corner(i)
                .dist(&cities[edge.1].rect.corner(j));
            if dist < min_dist {
                min_dist = dist;
            }
            if dist > max_dist {
                max_dist = dist;
            }
        }
    }
    (min_dist, max_dist)
}

fn if_this_is_obvious_constraint(cities: &[City], constraint: &InequalityConstraint) -> bool {
    let min_dist_longer_edge = min_max_dist_rects(cities, constraint.longer_edge).0; // min dist of longer edge
    let max_dist_shorter_edge = min_max_dist_rects(cities, constraint.shorter_edge).1; // max dist of shorter edge
    // If the minimum distance of the longer edge is greater than the maximum distance of the shorter edge,
    // then the constraint is trivially satisfied.
    return min_dist_longer_edge >= max_dist_shorter_edge;
}

fn find_path_in_mst(
    start_node: usize,
    end_node: usize,
    mst_edges: &[(usize, usize)],
    query_nodes: &HashSet<usize>,
) -> Option<Vec<(usize, usize)>> {
    if !query_nodes.contains(&start_node) || !query_nodes.contains(&end_node) {
        return None;
    }
    if start_node == end_node {
        return Some(vec![]);
    }
    let mut mst_adj: HashMap<usize, Vec<usize>> = HashMap::new();
    for &(u, v) in mst_edges {
        if query_nodes.contains(&u) && query_nodes.contains(&v) {
            mst_adj.entry(u).or_default().push(v);
            mst_adj.entry(v).or_default().push(u);
        }
    }
    let mut q = VecDeque::new();
    let mut visited = HashSet::new();
    q.push_back((start_node, Vec::<(usize, usize)>::new()));
    visited.insert(start_node);
    while let Some((current, path)) = q.pop_front() {
        if current == end_node {
            return Some(path);
        }
        if let Some(neighbors) = mst_adj.get(&current) {
            for &neighbor in neighbors {
                if !visited.contains(&neighbor) {
                    visited.insert(neighbor);
                    let mut new_path = path.clone();
                    let edge = (current.min(neighbor), current.max(neighbor));
                    new_path.push(edge);
                    q.push_back((neighbor, new_path));
                }
            }
        }
    }
    None
}

fn generate_constraints_from_mst(
    query_indices: &[usize],
    mst_edges: &[(usize, usize)],
) -> Vec<InequalityConstraint> {
    if query_indices.len() < 2 {
        return vec![];
    }
    let mut constraints = Vec::new();
    let query_nodes: HashSet<usize> = query_indices.iter().cloned().collect();
    let mst_edge_set: HashSet<(usize, usize)> = mst_edges
        .iter()
        .filter_map(|&(u, v)| {
            if query_nodes.contains(&u) && query_nodes.contains(&v) {
                Some((u.min(v), u.max(v)))
            } else {
                None
            }
        })
        .collect();

    for i in 0..query_indices.len() {
        for j in i + 1..query_indices.len() {
            let u = query_indices[i];
            let v = query_indices[j];
            let edge_uv = (u.min(v), u.max(v));
            if !mst_edge_set.contains(&edge_uv) {
                if let Some(path_edges) = find_path_in_mst(u, v, mst_edges, &query_nodes) {
                    for &path_edge in &path_edges {
                        // Ensure path_edge is also valid before adding constraint
                        if path_edge.0 != path_edge.1 {
                            // Avoid self-loops if any weirdness happens
                            constraints.push(InequalityConstraint::new(edge_uv, path_edge));
                        }
                    }
                }
            }
        }
    }
    constraints
}

// --- Constraint Index (for redundancy check, same as before) ---
struct ConstraintIndex {
    constraints_by_city: Vec<Vec<InequalityConstraint>>,
    seen_constraints: HashSet<u64>,
    total_constraints: usize,
}
impl ConstraintIndex {
    fn new(n: usize) -> Self {
        ConstraintIndex {
            constraints_by_city: vec![vec![]; n],
            seen_constraints: HashSet::new(),
            total_constraints: 0,
        }
    }
    fn add_constraint(&mut self, constraint: InequalityConstraint, cities: &[City]) -> bool {
        let longer_edge = (
            constraint.longer_edge.0.min(constraint.longer_edge.1),
            constraint.longer_edge.0.max(constraint.longer_edge.1),
        );
        let shorter_edge = (
            constraint.shorter_edge.0.min(constraint.shorter_edge.1),
            constraint.shorter_edge.0.max(constraint.shorter_edge.1),
        );
        if longer_edge == shorter_edge {
            return false;
        }
        if if_this_is_obvious_constraint(cities, &constraint) {
            return false; // Skip if trivially satisfied
        }

        if !self.seen_constraints.insert(constraint.id) {
            return false;
        }
        self.constraints_by_city[longer_edge.0].push(constraint.clone());
        self.constraints_by_city[longer_edge.1].push(constraint.clone());
        self.constraints_by_city[shorter_edge.0].push(constraint.clone());
        self.constraints_by_city[shorter_edge.1].push(constraint);
        self.total_constraints += 1;
        true
    }
    fn get_all_constraints(&self) -> HashSet<InequalityConstraint> {
        let mut all_constraints = HashSet::new();
        for constraints in &self.constraints_by_city {
            for constraint in constraints {
                all_constraints.insert(constraint.clone());
            }
        }
        all_constraints
    }
}

#[derive(Clone, Debug)]
struct Particle {
    pos: Vec<Point>,
    violated_cities: FixedBitSet,
    penalties: Vec<i64>,
}
impl Particle {
    fn new_random(n_cities: usize, city_rects: &[Rect], rng: &mut impl Rng) -> Self {
        let pos = (0..n_cities)
            .map(|i| city_rects[i].random_point(rng))
            .collect::<Vec<_>>();
        Self {
            pos,
            violated_cities: FixedBitSet::with_capacity(n_cities),
            penalties: vec![0; n_cities],
        }
    }

    fn set_violated_cities(&mut self, city_id: usize, violated: bool, penalty: i64) {
        self.violated_cities.set(city_id, violated);
        self.penalties[city_id] = penalty;
    }

    fn add_violated_cities(&mut self, city_id: usize, violated: bool, penalty: i64) {
        self.violated_cities.set(city_id, violated);
        self.penalties[city_id] += penalty;
    }

    fn num_violated_cities(&self) -> usize {
        self.violated_cities.count_ones(..)
    }
}
// --- Particle Initialization ---
fn initialize_particles(n_particles: usize, city_rects: &[Rect]) -> Vec<Particle> {
    (0..n_particles)
        .map(|_| {
            let mut rng = rand::thread_rng();
            Particle::new_random(city_rects.len(), &city_rects, &mut rng)
        })
        .collect()
}

fn choose_random_point_under_constraints(
    city_rects: &[Rect],
    particle: &Particle,
    city_id: usize,
    constraints_index: &ConstraintIndex,
    rng: &mut impl Rng,
) -> Point<i64> {
    let mut attempts = 0;
    let mut min_violation = i64::MAX;
    let mut min_violation_pos = Point { x: 0, y: 0 };

    while attempts < 100 {
        let proposed_pos = city_rects[city_id].random_point(rng);
        let mut violation = 0;
        for constraint in &constraints_index.constraints_by_city[city_id] {
            if constraint.check(&particle.pos) {
                violation += constraint.penalty(&particle.pos);
            }
        }
        if violation < min_violation {
            min_violation = violation;
            min_violation_pos = proposed_pos;
        }
        attempts += 1;
    }
    min_violation_pos
}

fn update_particles_local_mh(
    particle: &mut Particle,
    city_rects: &[Rect],
    constraints_index: &ConstraintIndex, // Read-only access needed
    max_seconds: f64,
    n_mh_steps_per_target: usize, // Steps *per affected city* per particle
    if_no_violated_stop: bool,
    rng: &mut impl Rng,
) {
    let mut affected_cities_vec = particle.violated_cities.ones().collect::<Vec<_>>();
    let max_delta = *MAX_DELTA as i64;
    //let mh_beta = *MH_BETA;
    let start = Instant::now();
    let mut iter = 0;
    while start.elapsed().as_secs_f64() < max_seconds
        && iter < n_mh_steps_per_target * affected_cities_vec.len()
        && (!if_no_violated_stop || particle.num_violated_cities() > 0)
    {
        iter += 1;
        let mut city_idx = affected_cities_vec.choose(rng).copied().unwrap_or(0); // Randomly select a city to move
        if !particle.violated_cities[city_idx] {
            city_idx = rng.gen_range(0..city_rects.len());
        }

        let current_city_pos = particle.pos[city_idx];
        let rect = city_rects[city_idx];
        let city_relevant_constraints = &constraints_index.constraints_by_city[city_idx];

        let max_delta_rect = Rect {
            lx: current_city_pos.x - max_delta,
            rx: current_city_pos.x + max_delta,
            ly: current_city_pos.y - max_delta,
            ry: current_city_pos.y + max_delta,
        };
        let candidate_rect = rect.intersect(&max_delta_rect);
        let proposed_pos = candidate_rect.random_point(rng);

        let mut energy_delta = 0;

        if city_relevant_constraints.is_empty() {
            particle.pos[city_idx] = proposed_pos;
            continue;
        }
        let mut next_violated = false;
        let mut penalty_next = 0;
        for idx in 0..city_relevant_constraints.len() {
            let constraint = &city_relevant_constraints[idx];
            let penalty_current = constraint.penalty(&particle.pos);
            particle.pos[city_idx] = proposed_pos; // Set to proposed position
            let penalty_proposed = constraint.penalty(&particle.pos);
            particle.pos[city_idx] = current_city_pos; // Restore original position
            energy_delta += penalty_proposed - penalty_current;
            penalty_next += penalty_proposed;
            next_violated |= penalty_proposed > 0;
        }

        // --- Acceptance check (Metropolis) ---
        if energy_delta <= 0 {
            //  || rng.r#gen::<f64>() < (-(energy_delta as f64) * mh_beta).exp() {
            // Accept the move - update the actual particle state
            particle.pos[city_idx] = proposed_pos;
            particle.set_violated_cities(city_idx, next_violated, penalty_next.sqrt());
        }
    } // End MH steps for this particle

    //eprintln!("MH step after: {}", particle.num_violated_cities(),);
}

fn sample_rect_query(
    pos: &[Point],
    representative_city_id: usize,
    l: usize,
    rng: &mut impl Rng,
) -> Vec<usize> {
    let n = pos.len();
    let mut query_cities = FixedBitSet::with_capacity(n);
    query_cities.insert(representative_city_id);
    let mut query_cities_count = 1;
    let repr_city = &pos[representative_city_id];
    let mut w = *SAMPLE_RECT_W;
    let mut h = *SAMPLE_RECT_H;
    let mut left = repr_city.x - w / 2;
    let mut up = repr_city.y - h / 2;

    let mut indices = (0..n).into_iter().collect_vec();
    indices.shuffle(rng);
    while query_cities_count < l {
        for &c in indices.iter() {
            let city = &pos[c];
            if city.x >= left
                && city.x <= (left + w)
                && city.y >= up
                && city.y <= (up + h)
                && query_cities[c] == false
            {
                query_cities_count += 1;
                query_cities.insert(c);
                if query_cities_count >= l {
                    break;
                }
            }
        }
        w += 200 * 2;
        h += 200 * 2;
        left -= 200;
        up -= 200;
    }
    query_cities.ones().collect()
}

fn eval_edges_cost(edges: &[(usize, usize)], positions: &[Point]) -> i64 {
    edges
        .iter()
        .map(|&(u, v)| positions[u].dist(&positions[v]))
        .sum::<i64>()
}

const PENALTY_RATIO: f64 = 0.05;

fn calc_avg_pos(particles: &[Particle]) -> Vec<Point> {
    let n = particles.len();
    let m = particles[0].pos.len();
    let mut avg_pos = vec![Point { x: 0, y: 0 }; m];
    for i in 0..m {
        let mut sum_x = 0.0;
        let mut sum_y = 0.0;
        let mut valid_count = 0f64;
        for k in 0..n {
            let w = 1.0 / (particles[k].penalties[i] as f64 * PENALTY_RATIO + 1f64);

            valid_count += w;
            let px = particles[k].pos[i].x;
            let py = particles[k].pos[i].y;
            sum_x += px as f64 * w;
            sum_y += py as f64 * w;
        }
        avg_pos[i] = Point {
            x: (sum_x / valid_count) as i64,
            y: (sum_y / valid_count) as i64,
        };
    }
    avg_pos
}

// この値が多いグループに対して、やったとしても、必ず制約が生まれるわけではない?
// 大きい方が効用が高い
fn eval_query_mst_loss(query: &[usize], particle: &[Particle], avg_poss: &[Point]) -> f64 {
    // eval expected loss
    let avg_mst = calculate_mst(query, avg_poss);
    let mut total_loss = 0.0;
    let mut valid_count = 0.0;
    for p in particle {
        // もしviolationがあったらカウントしない
        //let num_v = query.iter().filter(|&q| p.violated_cities[*q]).count();
        let w = 1.0
            / (query.iter().map(|&q| p.penalties[q]).max().unwrap_or(1) as f64 * PENALTY_RATIO
                + 1f64);

        valid_count += w;
        let (true_mst_cost_k, _true_mst_edges_k) = calculate_mst(query, &p.pos);
        let cost_if_avg_used_k = eval_edges_cost(&avg_mst.1, &p.pos);
        // assert!(
        //     cost_if_avg_used_k >= true_mst_cost_k,
        //     "cost_if_avg_used_k: {}, true_mst_cost_k: {}",
        //     cost_if_avg_used_k,
        //     true_mst_cost_k,
        // );
        // avg_mstを採用していたとしたら受けていた損失.
        let loss = cost_if_avg_used_k - true_mst_cost_k;
        total_loss += loss as f64 * w;
    }
    if valid_count == 0.0 {
        return f64::MIN; // Avoid division by zero
    }
    // eprintln!(
    //     "eval_query_mst_loss: {} {} {}",
    //     total_loss / valid_count as f64,
    //     avg_mst.0,
    //     valid_count
    // );
    total_loss / valid_count as f64
}
fn eval_query_mst_cost_variance(
    query: &[usize],
    particle_positions: &[Particle],
    avg_poss: &[Point],
) -> f64 {
    // eval cost variance
    let mut sum_cost = 0.0;
    let mut sum_cost_sq = 0.0;
    let mut valid_count = 0.0;

    for p in particle_positions {
        // もしviolationがあったらカウントしない
        let num_v = query.iter().filter(|&q| p.violated_cities[*q]).count();
        let w = 1.0 / (num_v + 1) as f64;

        valid_count += w;
        let (true_mst_cost_k, _true_mst_edges_k) = calculate_mst(query, &p.pos);
        sum_cost += true_mst_cost_k as f64 * w;
        sum_cost_sq += (true_mst_cost_k * true_mst_cost_k) as f64 * w;
    }
    if valid_count == 0.0 {
        return f64::MIN; // Avoid division by zero
    }
    let mean_cost = sum_cost / valid_count;
    let mean_cost_sq = sum_cost_sq / valid_count;
    let variance = mean_cost_sq - (mean_cost * mean_cost);
    return variance;
}

fn expected_mst_loss(particle_positions: &[Vec<Point>], avg_poss: &[Point]) -> f64 {
    // eval expected loss
    let indices = &(0..avg_poss.len()).collect::<Vec<_>>();
    let avg_mst = calculate_mst(indices, avg_poss);
    let mut total_loss = 0.0;
    for p in particle_positions {
        let (true_mst_cost_k, _true_mst_edges_k) = calculate_mst(indices, p);
        let cost_if_avg_used_k = eval_edges_cost(&avg_mst.1, p);
        // if !(cost_if_avg_used_k >= true_mst_cost_k) {
        //     eprintln!(
        //         "cost_if_avg_used_k: {}, true_mst_cost_k: {}",
        //         cost_if_avg_used_k, true_mst_cost_k
        //     );
        // }
        // avg_mstを採用していたとしたら受けていた損失.
        let loss = cost_if_avg_used_k - true_mst_cost_k;
        total_loss += loss as f64;
    }
    total_loss / particle_positions.len() as f64
}

// --- Query Selection by Cost Variance ---
fn select_query_by_cost_variance(
    n_cities: usize,
    l: usize, // max query size
    particles: &[Particle],
    n_candidates: usize,
    rng: &mut impl Rng,
) -> Vec<usize> {
    let avg_poss: Vec<Point> = calc_avg_pos(particles);
    let mut best_query: Vec<usize> = vec![];
    let mut best_cost = f64::MIN;

    for _candidate in 0..n_candidates {
        let query: Vec<usize> = sample_rect_query(&avg_poss, rng.gen_range(0..n_cities), l, rng);
        let cost = eval_query_mst_cost_variance(&query, &particles, &avg_poss);
        if cost > best_cost {
            best_cost = cost;
            best_query = query;
        }
    }
    return best_query;
}

fn sample_query_from_subtree(
    tree: &[Vec<usize>],
    l: usize,
    root: usize,
    rng: &mut impl Rng,
) -> Vec<usize> {
    // 適当な頂点から始めて、幅優先で
    let mut query = vec![];
    let mut visited = vec![false; tree.len()];
    let mut queue = VecDeque::new();
    let start_node = root; //rng.gen_range(0..tree.len());
    queue.push_back(start_node);
    visited[start_node] = true;
    while let Some(node) = queue.pop_front() {
        query.push(node);
        if query.len() >= l {
            break;
        }
        for &neighbor in &tree[node] {
            if !visited[neighbor] {
                visited[neighbor] = true;
                queue.push_back(neighbor);
            }
        }
    }
    query
}
fn select_query_by_position_variance(
    n_cities: usize,
    l: usize, // Target query size (max L, min 2)
    particles: &[Particle],
    rng: &mut impl Rng,
) -> Vec<usize> {
    if n_cities < 2 || l < 2 {
        // Cannot form a meaningful query
        return if n_cities >= 1 { vec![0] } else { vec![] }; // Return something valid if possible
    }
    let n_particles = particles.len() as i64;
    let mut variances: Vec<i64> = Vec::with_capacity(n_cities); // (variance, city_id)
    let mut avg_pos: Vec<Point> = vec![Point { x: 0, y: 0 }; n_cities];

    for i in 0..n_cities {
        let mut sum_x = 0;
        let mut sum_y = 0;
        let mut sum_x2 = 0;
        let mut sum_y2 = 0;
        for k in 0..n_particles {
            let px = particles[k as usize].pos[i].x;
            let py = particles[k as usize].pos[i].y;
            sum_x += px;
            sum_y += py;
            sum_x2 += px * px;
            sum_y2 += py * py;
        }
        let mean_x = sum_x / n_particles;
        let mean_y = sum_y / n_particles;
        avg_pos[i] = Point {
            x: mean_x as i64,
            y: mean_y as i64,
        };

        let var_x = (sum_x2 / n_particles) - (mean_x * mean_x);
        let var_y = (sum_y2 / n_particles) - (mean_y * mean_y);
        //variances.push(f64::exp((var_x + var_y) / 1000000.0)); // Variance as a measure of spread
        variances.push(var_x + var_y);
    }

    // 2. Find the city with the weighted variance
    let representative_city_id = WeightedIndex::new(variances).unwrap().sample(rng);
    sample_rect_query(&avg_pos, representative_city_id, l, rng)
}
fn select_query_by_cost_variance_on_tree(
    n_cities: usize,
    l: usize, // max query size
    particles: &[Particle],
    n_candidates: usize,
    num_ret: usize, //allow_dup個以上重複してない、num_ret個をtopから返す
    allow_dup: usize,
    rng: &mut impl Rng,
) -> Vec<Vec<usize>> {
    let avg_poss: Vec<Point> = calc_avg_pos(particles);
    let repr_particle = rng.gen_range(0..particles.len());
    let mut best_queries: Vec<(FixedBitSet, f64)> = vec![];
    let roots = (0..n_cities).choose_multiple(rng, n_candidates);
    for _candidate in 0..n_candidates {
        let query = sample_rect_query(&particles[repr_particle].pos, roots[_candidate], l, rng);
        let pos = particles[repr_particle].pos[roots[_candidate]];
        let cost = eval_query_mst_loss(&query, &particles, &avg_poss);
        best_queries.push((query.into_iter().collect::<FixedBitSet>(), cost));
    }
    best_queries.sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap());
    let mut ret: Vec<FixedBitSet> = vec![];
    for i in 0..num_ret {
        if ret
            .iter()
            .any(|q| q.intersection(&best_queries[i].0).count() > allow_dup)
        {
            continue;
        }
        if ret.len() >= num_ret {
            break;
        }
        ret.push(best_queries[i].0.clone());
    }
    ret.into_iter()
        .map(|q| q.ones().collect::<Vec<_>>())
        .collect()
}

fn reorder_groups(groups: &Vec<Vec<usize>>, target_group_sizes: &[usize]) -> Vec<Vec<usize>> {
    assert_eq!(
        groups.len(),
        target_group_sizes.len(),
        "groups and target_group_sizes must have the same length"
    );
    let mut new_groups = vec![vec![]; groups.len()];
    for group in groups {
        let target_group_id = target_group_sizes
            .iter()
            .enumerate()
            .position(|(i, &size)| size == group.len() && new_groups[i].is_empty())
            .unwrap_or(0);
        new_groups[target_group_id].extend(group.iter());
    }
    new_groups
}

use std::time::Duration;

fn calculate_size_penalty(
    current_sizes: &[usize],       // Current group sizes (length M)
    target_sizes_sorted: &[usize], // Target group sizes, sorted (length M)
    weight: f64,
) -> f64 {
    if current_sizes.len() != target_sizes_sorted.len() {
        return f64::MAX;
    }
    let mut current_sizes_sorted = current_sizes.to_vec();
    current_sizes_sorted.sort_unstable();

    let mut squared_error = 0;
    for i in 0..current_sizes_sorted.len() {
        let diff = current_sizes_sorted[i] as isize - target_sizes_sorted[i] as isize;
        squared_error += if diff != 0 { diff.abs() } else { 0 };
    }
    weight * squared_error as f64
}

use rand::seq::SliceRandom;

// --- 依存関数・構造体 ---
// Point, City, Rect, UnionFind, StdRng
// calculate_size_penalty

/// Assigns groups using SA on MST cuts with a simplified cost function (sum of kept edge costs).
/// Size penalty is highly recommended (`use_size_penalty = true`).
///
/// # Arguments
/// * ... (same as previous SA cut function) ...
///
/// # Returns
/// Tuple `(best_assignment, best_groups, best_cost)` where cost is the *actual* MST cost sum
/// calculated from the best assignment found by the SA using the *simplified* energy.
fn assign_groups_sa_mst_cut_simplified_cost(
    n: usize,
    m: usize,
    final_avg_pos: &[Point],
    avg_length_mat: &[Vec<i64>],
    target_group_sizes: &[usize],
    time_limit_duration: Duration,
    initial_penalty_weight: f64,
    final_penalty_weight: f64,
    rng: &mut impl Rng,
) -> (Vec<usize>, Vec<Vec<usize>>, i64) {
    if m <= 1 || n < m {
        /* Handle edge cases */
        return (vec![0; n], vec![(0..n).collect()], 0);
    }

    let sa_start_time = Instant::now();
    // --- 1. Global MST and Edge Costs ---
    let all_city_indices: Vec<usize> = (0..n).collect();
    let (_global_mst_cost, global_mst_edges) = calculate_mst(&all_city_indices, &final_avg_pos);
    if global_mst_edges.len() != n - 1 {
        /* Handle error */
        return (vec![0; n], vec![(0..n).collect()], 0);
    }

    let mut adj: Vec<Vec<usize>> = vec![vec![]; n]; // MST adjacency list
    let mut edge_costs: HashMap<(usize, usize), i64> = HashMap::new();
    let mut all_mst_edges_canonical: HashSet<(usize, usize)> = HashSet::with_capacity(n - 1);

    for &(u, v) in &global_mst_edges {
        let cost = avg_length_mat[u][v];
        let edge = (u.min(v), u.max(v));
        edge_costs.insert(edge, cost);
        all_mst_edges_canonical.insert(edge);
        adj[u].push(v);
        adj[v].push(u);
    }

    // --- 2. Initial State ---
    let mut current_cut_edges: HashSet<(usize, usize)>;
    let mut edges_with_cost_vec: Vec<(i64, (usize, usize))> =
        edge_costs.iter().map(|(e, c)| (*c, *e)).collect();
    edges_with_cost_vec.sort_unstable_by_key(|k| k.0); // Ascending cost

    current_cut_edges = HashSet::new();

    for i in (0..edges_with_cost_vec.len()).rev().take(m - 1) {
        current_cut_edges.insert(edges_with_cost_vec[i].1);
    }
    let mut current_kept_edges: HashSet<(usize, usize)> = all_mst_edges_canonical
        .difference(&current_cut_edges)
        .cloned()
        .collect();
    let mut current_kept_cost: i64 = current_kept_edges.iter().map(|e| edge_costs[e]).sum();

    // --- Helper for penalty and groups ---
    let calculate_penalty_and_groups =
        |kept_edges: &HashSet<(usize, usize)>, penalty_weight: f64| -> (f64, Vec<Vec<usize>>) {
            let mut uf = UnionFind::new(n);
            for &edge in kept_edges {
                uf.unite(edge.0, edge.1);
            }
            let groups = uf.components();
            let penalty = {
                let current_sizes: Vec<usize> = groups.iter().map(|g| g.len()).collect();
                let mut target_sizes_s = target_group_sizes.to_vec();
                target_sizes_s.sort_unstable();
                calculate_size_penalty(&current_sizes, &target_sizes_s, penalty_weight)
            };
            (penalty, groups)
        };

    // --- 3. Initial Energy & Best State ---
    let mut target_sizes_sorted = target_group_sizes.to_vec();
    target_sizes_sorted.sort_unstable();
    let (mut current_penalty, mut current_groups) =
        calculate_penalty_and_groups(&current_kept_edges, initial_penalty_weight);

    let mut current_energy = current_kept_cost as f64 + current_penalty;
    let mut current_sizes: Vec<usize> = current_groups.iter().map(|g| g.len()).collect(); // Initial sizes

    let mut best_kept_edges = current_kept_edges.clone();
    let mut best_groups = current_groups.clone();
    let mut best_kept_cost = current_kept_cost;
    let (mut best_penalty, _) =
        calculate_penalty_and_groups(&best_kept_edges, final_penalty_weight);
    let mut best_eval_cost = best_kept_cost as f64 + best_penalty;

    // --- 4. SA Parameters & Loop ---
    let time_limit = sa_start_time + time_limit_duration;
    let start_temp = (current_kept_cost as f64 * (*MST_ANNEAL_START_TEMP_RATIO)).max(1.0);
    let end_temp = 0.0;
    let mut iteration = 0;
    let mut accepted_moves = 0;

    // --- 5. SA Loop ---
    while Instant::now() < time_limit {
        iteration += 1;
        let elapsed_ratio = sa_start_time
            .elapsed()
            .as_secs_f64()
            .min(time_limit_duration.as_secs_f64())
            / time_limit_duration.as_secs_f64();
        let temp = start_temp * (end_temp / start_temp).powf(elapsed_ratio);
        let current_penalty_weight = initial_penalty_weight
            + (final_penalty_weight - initial_penalty_weight) * elapsed_ratio;

        // --- 6. Near Neighbor Operation: Swap Cut ---
        if current_cut_edges.is_empty() || current_kept_edges.is_empty() {
            continue;
        }
        let &edge_to_uncut = current_cut_edges.iter().choose(rng).unwrap(); // e_cut
        let &edge_to_cut = current_kept_edges.iter().choose(rng).unwrap(); // e_in

        // --- 7. Calculate Energy Change (Delta E) ---
        let cost_e_cut = edge_costs[&edge_to_uncut];
        let cost_e_in = edge_costs[&edge_to_cut];
        let delta_kept_cost = cost_e_cut - cost_e_in; // Delta for the simplified MST cost part

        let delta_penalty = {
            // Calculate penalty change: need groups before and after swap
            // Before: current_groups (already computed if needed, or recompute sizes)
            // After: Simulate the swap in UnionFind to get new group sizes
            let mut next_kept_edges_temp = current_kept_edges.clone();
            next_kept_edges_temp.remove(&edge_to_cut);
            next_kept_edges_temp.insert(edge_to_uncut);
            let (next_penalty, _) =
                calculate_penalty_and_groups(&next_kept_edges_temp, current_penalty_weight);
            // Recalculate current penalty with current weight
            current_sizes = current_groups.iter().map(|g| g.len()).collect::<Vec<_>>();
            let current_penalty_recalc = calculate_size_penalty(
                &current_sizes,
                &target_sizes_sorted,
                current_penalty_weight,
            ); // Need current_sizes
            next_penalty - current_penalty_recalc
        };

        let delta_energy = delta_kept_cost as f64 + delta_penalty;

        // --- 8. Acceptance Check ---
        if delta_energy <= 0.0 {//|| rng.gen_bool(f64::exp(-delta_energy / temp)) {
            accepted_moves += 1;
            // Accept the move
            current_kept_edges.remove(&edge_to_cut);
            current_kept_edges.insert(edge_to_uncut);
            current_cut_edges.remove(&edge_to_uncut);
            current_cut_edges.insert(edge_to_cut);

            current_kept_cost += delta_kept_cost; // Update simplified cost

            // Recalculate penalty and total energy with new state and current weight
            // This recalculates groups too, which might be needed anyway
            let (new_penalty, new_groups) =
                calculate_penalty_and_groups(&current_kept_edges, current_penalty_weight);
            current_penalty = new_penalty;
            current_groups = new_groups; // Update groups
            current_energy = current_kept_cost as f64 + current_penalty;

            // Update best solution based on evaluation with *final* penalty weight
            let (eval_penalty, _) =
                calculate_penalty_and_groups(&current_kept_edges, final_penalty_weight);
            let eval_cost = current_kept_cost as f64 + eval_penalty;

            if eval_cost < best_eval_cost {
                best_eval_cost = eval_cost;
                best_penalty = eval_penalty;
                best_kept_edges = current_kept_edges.clone();
                best_groups = current_groups.clone(); // Store best groups
                best_kept_cost = current_kept_cost; // Store best simplified cost
                // eprintln!(
                //     "SA CutSC Iter {}: New best eval_cost: {:.3e} (KeptCost: {}, Pen: {:.3e})",
                //     iteration, best_eval_cost, best_kept_cost, eval_penalty
                // );
            }
        }
    } // End SA Loop

    // eprintln!(
    //     "SA MST Cut (Simplified Cost) finished. Iter {}. Accepted: {}. Best KeptCost: {} Penalty: {:.3e}",
    //     iteration, accepted_moves, best_kept_cost, best_penalty
    // );

    // --- Final Step: Calculate *actual* MST cost for the best partition found ---
    let mut final_real_mst_cost: i64 = 0;
    for group in &best_groups {
        if group.len() > 0 {
            final_real_mst_cost += calculate_mst(group, &final_avg_pos).0;
        }
    }
    // eprintln!(
    //     "Final Real MST Cost for best cut found: {}",
    //     final_real_mst_cost
    // );

    let mut best_assignment = vec![usize::MAX; n];
    for (group_id, group_members) in best_groups.iter().enumerate() {
        for &city_id in group_members {
            if city_id < n {
                best_assignment[city_id] = group_id;
            } // Temporary ID based on component order
        }
    }
    (best_assignment, best_groups, final_real_mst_cost)
}

use ac_library::MinCostFlowGraph;

// --- ヘルパー関数: グループの重心計算 ---
fn calculate_group_centers(
    groups: &[Vec<usize>], // 現在のグループ分け
    points: &[Point],      // 都市の座標
    m: usize,              // グループ数
) -> Vec<Point> {
    let mut centers = vec![Point { x: 0, y: 0 }; m];

    for group_id in 0..m {
        if groups[group_id].is_empty() {
            // 空グループの場合、適当なデフォルト値（例：グリッド中心付近など）
            // または、割り当てられる可能性のある都市の重心などを事前に計算しておく
            // ここでは仮に (5000, 5000) とする
            centers[group_id] = Point { x: 5000, y: 5000 };
            continue;
        }
        let mut sum_x = 0;
        let mut sum_y = 0;
        for &city_id in &groups[group_id] {
            if city_id >= points.len() {
                continue;
            } // 範囲外チェック
            sum_x += points[city_id].x;
            sum_y += points[city_id].y;
        }
        let count = groups[group_id].len();
        if count > 0 {
            centers[group_id] = Point {
                x: sum_x / count as i64,
                y: sum_y / count as i64,
            };
        } else {
            // 再度空チェック (理論上は上のifで捕捉されるはず)
            centers[group_id] = Point { x: 5000, y: 5000 };
        }
    }
    centers
}

// --- ヘルパー関数: 重心再計算 (単一グループ用) ---
fn recalc_single_group_center(group_id: usize, groups: &[Vec<usize>], points: &[Point]) -> Point {
    if groups[group_id].is_empty() {
        return Point { x: 5000, y: 5000 };
    }
    let mut sx = 0;
    let mut sy = 0;
    for &cid in &groups[group_id] {
        if cid >= points.len() {
            continue;
        }
        sx += points[cid].x;
        sy += points[cid].y;
    }
    let count = groups[group_id].len();
    Point {
        x: sx / count as i64,
        y: sy / count as i64,
    }
}

// --- ヘルパー関数: 流量1が流れたパスを復元 ---
/// MCF計算後のグラフ状態から、実際に流量1が流れたパスをSからTへ見つける
fn find_flow1_path(
    graph: &MinCostFlowGraph<i64>,
    s: usize,
    t: usize,
    m: usize,
) -> Option<Vec<usize>> {
    let mut path = vec![s];
    let mut visited_nodes = HashSet::new();
    visited_nodes.insert(s);
    let mut current_node = s;

    // ノード数以上のパスはありえない（サイクル防止）
    for _ in 0..=m + 1 {
        // 最大 m+2 ノード
        if current_node == t {
            return Some(path); // Tに到達
        }

        let mut next_node = usize::MAX;
        // S またはグループノードから次へ
        if current_node == s || current_node < m {
            for edge in graph.edges() {
                // Sからのエッジ、またはグループ間のエッジで flow=1 のものを探す
                if edge.from == current_node && edge.flow == 1 && (edge.to < m || edge.to == t) {
                    if !visited_nodes.contains(&edge.to) {
                        next_node = edge.to;
                        break;
                    }
                }
            }
        }
        // else: current_node == t なのでループは終了するはず

        if next_node != usize::MAX {
            path.push(next_node);
            visited_nodes.insert(next_node);
            current_node = next_node;
        } else {
            // eprintln!(
            //     "Debug: find_flow1_path: Path broken at node {}",
            //     current_node
            // );
            // for edge in graph.edges() {
            //     if edge.from == current_node && edge.flow == 1 {
            //         eprintln!(
            //             "  Debug: Candidate next node {} (visited: {})",
            //             edge.to,
            //             visited_nodes.contains(&edge.to)
            //         );
            //     }
            // }
            return None; // Tに到達する前にパスが途切れた
        }
    }
    //eprintln!("Debug: find_flow1_path: Path too long, possible cycle?");
    None // パスが長すぎる場合（エラー）
}

fn measure_avg_dist(particles: &[Particle], from: usize, to: usize) -> i64 {
    let mut total_dist = 0f64;
    let mut count = 0f64;
    for p in particles {
        let w = 1.0 / ((p.penalties[from] + p.penalties[to]) as f64 / 2f64 as f64 * 0.01 + 1f64);
        total_dist += p.pos[from].dist(&p.pos[to]) as f64 * w;
        count += w;
    }
    (total_dist / count) as i64
}

fn construct_avg_length_matrix(particles: &[Particle], n: usize) -> Vec<Vec<i64>> {
    let mut avg_length_matrix = vec![vec![0; n]; n];
    for i in 0..n {
        for j in 0..n {
            if i > j {
                avg_length_matrix[i][j] = measure_avg_dist(particles, i, j);
                avg_length_matrix[j][i] = avg_length_matrix[i][j];
            }
        }
    }
    avg_length_matrix
}

fn adjust_group_sizes_with_sequential_mcf_path(
    // 関数名を変更
    n: usize,
    m: usize,
    initial_groups: Vec<Vec<usize>>,
    target_group_sizes: &[usize],
    //final_avg_pos: &[Point],
    avg_length_mat: &[Vec<i64>],
    particles: &[Particle],
) -> Vec<Vec<usize>> {
    let mut current_groups = initial_groups; // 可変にする

    // --- 目標サイズのソート済み情報 (不変) ---
    let mut target_size_info: Vec<(usize, usize)> = target_group_sizes
        .iter()
        .enumerate()
        .map(|(id, &sz)| (sz, id))
        .collect();
    target_size_info.sort_unstable_by_key(|k| k.0);

    // --- 都市ID -> 現在のグループID マッピング ---
    let mut city_to_group_map: Vec<usize> = vec![usize::MAX; n];
    for group_id in 0..m {
        for &city_id in &current_groups[group_id] {
            if city_id < n {
                city_to_group_map[city_id] = group_id;
            }
        }
    }

    let mut moved_count_total = 0i64;
    // --- グループ重心 (移動ごとに更新) ---

    // --- 調整ループ ---
    for _adjust_step in 0..n {
        // 安全のための最大反復回数

        // 1. 現在の過不足を計算 (diff_current)
        let mut diff_current = vec![0i64; m];
        let mut current_excess_group_ids: Vec<usize> = Vec::new();
        let mut current_deficit_group_ids: Vec<usize> = Vec::new();
        let mut current_total_excess: i64 = 0;
        let current_sizes_now: Vec<usize> = current_groups.iter().map(|g| g.len()).collect();
        let mut current_size_info_now: Vec<(usize, usize)> = current_sizes_now
            .iter()
            .enumerate()
            .map(|(id, &sz)| (sz, id))
            .collect();
        current_size_info_now.sort_unstable_by_key(|k| k.0);

        for i in 0..m {
            let current_original_id = current_size_info_now[i].1;
            let current_sz = current_size_info_now[i].0;
            let target_sz = target_size_info[i].0;
            let d = current_sz as i64 - target_sz as i64;
            diff_current[current_original_id] = d;
            if d > 0 {
                current_total_excess += d;
                current_excess_group_ids.push(current_original_id);
            } else if d < 0 {
                current_deficit_group_ids.push(current_original_id);
            }
        }

        // 調整完了判定
        if current_total_excess == 0 {
            // eprintln!(
            //     "MCF Adjust Path: Adjustment completed after {} moves.",
            //     moved_count_total
            // );
            break;
        }
        if _adjust_step == 0 {
            // eprintln!(
            //     "MCF Adjust Path: Initial total excess = {}",
            //     current_total_excess
            // );
        }

        // 2. MCFグラフ構築 (流量1を求めるため)
        let source_node = m;
        let sink_node = m + 1;
        let graph_size = m + 2;
        let mut mcf_graph_step = MinCostFlowGraph::new(graph_size);
        // ... (エッジ追加 - 前回と同じ、ただしグループ間エッジの容量は1で良い) ...
        // ソース -> 過剰グループ
        for &group_id in &current_excess_group_ids {
            if diff_current[group_id] > 0 {
                mcf_graph_step.add_edge(source_node, group_id, diff_current[group_id], 0); // 容量は過剰分
            }
        }
        // 不足グループ -> シンク
        for &group_id in &current_deficit_group_ids {
            if diff_current[group_id] < 0 {
                mcf_graph_step.add_edge(group_id, sink_node, -diff_current[group_id], 0); // 容量は不足分
            }
        }

        // --- グループ間エッジ (コスト計算を修正) ---
        for i in 0..m {
            // from_group_id
            if current_groups[i].is_empty() {
                continue;
            }
            for j in 0..m {
                // to_group_id
                if i == j {
                    continue;
                }
                let mut min_delta_cost = i64::MAX;

                // G_i の各都市 v についてループ
                for &v_city_id in &current_groups[i] {
                    if v_city_id >= n {
                        continue;
                    } // 範囲チェック

                    // 1. v の G_j における最近傍距離 (dist_out)
                    let mut dist_out = i64::MAX;
                    if current_groups[j].is_empty() {
                        dist_out = 0;
                    } else {
                        for &u_city_id in &current_groups[j] {
                            if u_city_id >= n {
                                continue;
                            }
                            dist_out = dist_out.min(avg_length_mat[v_city_id][u_city_id]);
                        }
                    }

                    // 2. v の G_i における最近傍距離 (dist_in, v自身を除く)
                    let mut dist_in = i64::MAX;
                    if current_groups[i].len() <= 1 {
                        dist_in = 0;
                    } else {
                        for &u_city_id in &current_groups[i] {
                            if u_city_id == v_city_id {
                                continue;
                            }
                            dist_in = dist_in.min(avg_length_mat[v_city_id][u_city_id]);
                        }
                    }
                    let delta_cost = dist_out - dist_in;
                    min_delta_cost = min_delta_cost.min(delta_cost);
                }

                // エッジコストを設定 (min_delta_cost が更新されなかった場合は MAX のまま)
                if min_delta_cost == i64::MAX {
                    // 移動できる都市がいなかった、または移動先が常に孤立する場合
                    // このエッジは使えないと見なす (コスト無限大)
                    // mcf_graph_step.add_edge(i, j, current_total_excess.max(1), i64::MAX / 2);
                    // 何もしないか、非常に大きなコストでエッジを追加
                } else {
                    // 最小のコスト差分をエッジコストとして追加
                    mcf_graph_step.add_edge(
                        i,
                        j,
                        current_total_excess.max(1),
                        max(0, min_delta_cost + 1),
                    );
                }
            } // End loop for j (to_group_id)
        } // End loop for i (from_group_id)

        // 3. 最小費用流計算 (流量 F=1)
        let (flow, cost) = mcf_graph_step.flow(source_node, sink_node, 1);
        // eprintln!(
        //     "  MCF Step {}: Flow = {}, Cost = {}",
        //     moved_count_total + 1,
        //     flow,
        //     cost
        // );

        // 4. **増強パスの復元**
        if let Some(path) = find_flow1_path(&mcf_graph_step, source_node, sink_node, m) {
            // eprintln!(
            //     "  MCF Step {}: Found path: {:?}",
            //     moved_count_total + 1,
            //     path
            // );
            // パスは [S, G_a, G_b, ..., G_z, T] の形

            // 5. **パスに沿って都市を移動**
            let mut successful_move_along_path = true;
            // パスのグループ部分 [G_a, G_b, ..., G_z] を取得
            let group_path_nodes: Vec<usize> =
                path.iter().skip(1).take(path.len() - 2).cloned().collect();

            if !group_path_nodes.is_empty() {
                // 各ステップ (G_i, G_{i+1}) で移動
                for i in 0..group_path_nodes.len() {
                    let from_group_id = group_path_nodes[i];
                    // 次のノードを決定 (パスの最後ならシンク T に対応する不足グループ)
                    let to_group_id = if i + 1 < group_path_nodes.len() {
                        group_path_nodes[i + 1]
                    } else {
                        // パスの最後のステップ: from_group_id -> T
                        // T に接続している不足グループ (to_group_id) を特定する必要がある
                        // (find_flow1_path が T を含んでいるので、pathの最後から2番目が G_z)
                        path[path.len() - 2] // これが G_z
                    };

                    if from_group_id == to_group_id {
                        // G_z -> G_z のようなケースを除外
                        continue;
                    }

                    // eprintln!("    Executing move: G{} -> G{}", from_group_id, to_group_id);

                    // 最適な都市を選択 (from_group から to_group の重心に近い都市)
                    let mut best_city_to_move = usize::MAX;
                    let mut min_dist = i64::MAX;

                    // from_group に現在所属している都市リスト (Mapを使う方が効率的かも)
                    let candidate_cities: Vec<usize> = current_groups[from_group_id]
                        .iter()
                        .filter(|&&cid| {
                            city_to_group_map
                                .get(cid)
                                .map_or(false, |&gid| gid == from_group_id)
                        })
                        .cloned()
                        .collect();

                    if candidate_cities.is_empty() {
                        // eprintln!(
                        //     "Error: Group {} became empty during path move. Path: {:?}",
                        //     from_group_id, path
                        // );
                        successful_move_along_path = false;
                        break; // このパスでの移動は中断
                    }

                    for &city_id in &candidate_cities {
                        let dist = current_groups[to_group_id]
                            .iter()
                            .map(|&cid| avg_length_mat[city_id][cid])
                            .min()
                            .unwrap_or(i64::MAX);
                        if dist < min_dist {
                            min_dist = dist;
                            best_city_to_move = city_id;
                        }
                    }

                    if best_city_to_move != usize::MAX {
                        // 都市移動を実行
                        // 1. マッピング更新
                        city_to_group_map[best_city_to_move] = to_group_id;
                        // 2. グループリスト更新
                        current_groups[from_group_id].retain(|&c| c != best_city_to_move);
                        current_groups[to_group_id].push(best_city_to_move);
                    } else {
                        // eprintln!(
                        //     "Error: Could not find suitable city to move from G{} to G{}.",
                        //     from_group_id, to_group_id
                        // );
                        successful_move_along_path = false;
                        break; // このパスでの移動は中断
                    }
                } // End loop for steps in path

                if successful_move_along_path {
                    moved_count_total += 1; // 1単位のフロー(パス全体)が完了
                } else {
                    //eprintln!("Warning: Failed to complete move along path. Stopping adjustment.");
                    break; // 調整ループ自体を中断
                }
            } else {
                // パスが S -> T のみだった場合など (通常は起こらないはず)
                // eprintln!(
                //     "Warning: Path found but no inter-group moves required? Path: {:?}",
                //     path
                // );
                break; // 調整ループ中断
            }
        } else {
            // パスが見つからなかった場合 (エラー)
            // eprintln!(
            //     "Error: Could not find flow path at step {}. Stopping adjustment.",
            //     moved_count_total
            // );
            break;
        }
    } // End adjustment loop

    // --- 最終的なサイズ検証 ---
    // ... (前回と同じ検証コード) ...
    let final_adjusted_sizes: Vec<usize> = current_groups.iter().map(|g| g.len()).collect();
    let mut final_adjusted_sizes_sorted = final_adjusted_sizes.clone();
    final_adjusted_sizes_sorted.sort_unstable();
    let target_group_sizes_sorted: Vec<usize> =
        target_size_info.iter().map(|(sz, _)| *sz).collect();

    if final_adjusted_sizes_sorted == target_group_sizes_sorted {
        //eprintln!("MCF Adjust Path: Final group sizes correctly match target sizes (multiset).");
    } else {
        // eprintln!("ERROR: MCF Adjust Path: Final group sizes DO NOT match target sizes!");
        // eprintln!("  Final sorted: {:?}", final_adjusted_sizes_sorted);
        // eprintln!("  Target sorted: {:?}", target_group_sizes_sorted);
    }
    current_groups
}
#[cfg(feature = "visualization")]
#[derive(Debug, Clone, Serialize, Deserialize)]
struct LogEntry {
    step: usize,
    time_elapsed_sec: f64,
    // --- パーティクル情報は統計値のみ ---
    average_position: Vec<Point>,
    position_std_dev: Vec<(i64, i64)>, // (std_dev_x, std_dev_y) for each city
    // --- 制約情報は新規追加分のみ ---
    newly_added_constraints: Vec<InequalityConstraint>, // This step's new constraints
    total_constraints_count: usize,                     // Total count for reference
    // --- 間引いて記録する全パーティクル情報 (オプション) ---
    // #[serde(skip_serializing_if = "Option::is_none")] // JSONでnullなら省略
    // sampled_particle_positions: Option<Vec<Pos>>,
    // --- その他 ---
    message: Option<String>,
    query_issued: Option<Vec<usize>>,
    query_response_mst: Option<Vec<(usize, usize)>>,
    // 必要に応じて他の統計情報 (e.g., energy, violation count if calculated)
}

#[cfg_attr(feature = "visualization", derive(Serialize, Deserialize))]
#[derive(Debug, Clone)]
struct ProblemInfo {
    n: usize,
    m: usize,
    q: usize,
    l: usize,
    w: i64,
    target_group_sizes: Vec<usize>,
    city_rects: Vec<Rect>,
}

#[cfg(feature = "visualization")]
#[derive(Serialize, Deserialize, Debug)]
struct VisualizationData {
    problem_info: ProblemInfo,
    log_entries: Vec<LogEntry>,
}
#[cfg(feature = "visualization")]
fn record_log(
    log_entries: &mut Vec<LogEntry>,
    step: usize,
    start_time: Instant,
    particles: &[Particle],
    newly_added_constraints_this_step: &[InequalityConstraint], // 新規制約を渡す
    total_constraints_count_now: usize,                         // 現在の総数を渡す
    message: Option<String>,
    query_issued: Option<Vec<usize>>,
    query_response_mst: Option<Vec<(usize, usize)>>,
    log_all_particles_freq: usize, // 例: 50 (50ステップごとに記録)
) {
    if particles.is_empty() {
        return;
    }
    let n_cities = particles[0].pos.len();
    let n_particles = particles.len();

    // 平均と標準偏差を計算
    let mut avg_pos = calc_avg_pos(particles);
    let mut std_dev_pos: Vec<(i64, i64)> = vec![(0, 0); n_cities];

    if n_particles > 0 {
        for i in 0..n_cities {
            let mut sum_x = 0;
            let mut sum_y = 0;
            let mut sum_x2 = 0;
            let mut sum_y2 = 0;
            for k in 0..n_particles {
                let px = particles[k].pos[i].x;
                let py = particles[k].pos[i].y;
                sum_x += px;
                sum_y += py;
                sum_x2 += px * px;
                sum_y2 += py * py;
            }
            let mean_x = sum_x / n_particles as i64;
            let mean_y = sum_y / n_particles as i64;

            // 分散 -> 標準偏差
            let var_x = (sum_x2 / n_particles as i64) - (mean_x * mean_x);
            let var_y = (sum_y2 / n_particles as i64) - (mean_y * mean_y);
            std_dev_pos[i] = (var_x.max(0).sqrt(), var_y.max(0).sqrt());
        }
    }

    // // 間引き記録
    // let sampled_particles = if log_all_particles_freq > 0 && step % log_all_particles_freq == 0 {
    //     Some(particles.to_vec())
    // } else {
    //     None
    // };

    log_entries.push(LogEntry {
        step,
        time_elapsed_sec: start_time.elapsed().as_secs_f64(),
        average_position: avg_pos,
        position_std_dev: std_dev_pos,
        newly_added_constraints: newly_added_constraints_this_step.to_vec(), // クローン
        total_constraints_count: total_constraints_count_now,
        //sampled_particle_positions: sampled_particles,
        message,
        query_issued,
        query_response_mst,
    });
}

// --- main 内での呼び出し ---
// ConstraintIndex::add_constraint が成功した制約をリストで集める
// let mut added_constraints_in_step: Vec<InequalityConstraint> = Vec::new();
// for constraint in new_constraints_from_query {
//     if constraint_index.add_constraint(constraint.clone()) { // cloneが必要かも
//         added_constraints_in_step.push(constraint);
//     }
// }
// let total_count = constraint_index.get_all_constraints().len(); // 総数を取得
// record_log(..., &added_constraints_in_step, total_count, ...);

// --- ファイル出力 ---
// serde_json::to_writer を使う (prettyではない方)
#[cfg(feature = "visualization")]
fn save_log(problem_info: &ProblemInfo, log_entries: &[LogEntry], filename: &str) {
    eprintln!("Attempting to save compact JSON log to {}...", filename);
    let vis_data = VisualizationData {
        problem_info: problem_info.clone(),
        log_entries: log_entries.to_vec(),
    };
    match File::create(filename) {
        Ok(file) => {
            // Use BufWriter for efficiency
            let writer = BufWriter::new(file);
            // Use to_writer for compact JSON
            match serde_json::to_writer(writer, &vis_data) {
                Ok(_) => eprintln!("Successfully saved compact JSON log to {}", filename),
                Err(e) => eprintln!("ERROR: Failed to write compact JSON: {}", e),
            }
        }
        Err(e) => eprintln!("ERROR: Failed to create file {}: {}", filename, e),
    }
}

/// 簡易的なXorShiftによる乱数生成器
struct XorShift {
    x: u64,
}

impl XorShift {
    fn new(seed: u64) -> Self {
        XorShift { x: seed }
    }
    #[inline(always)]
    fn next(&mut self) -> u64 {
        let mut x = self.x;
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        self.x = x;
        x
    }
    /// 0..n-1 の乱数を返す
    fn next_usize(&mut self, n: usize) -> usize {
        (self.next() as usize) % n
    }
    /// 0.0 以上 1.0 未満の乱数を返す
    fn next_f64(&mut self) -> f64 {
        // 53ビット精度を確保するには工夫が要るが、簡易的に行う
        // 下位52ビットを利用して 0~1 の範囲に正規化する
        const MASK52: u64 = (1u64 << 52) - 1;
        let v = self.next() & MASK52;
        (v as f64) / (MASK52 as f64 + 1.0)
    }
}
impl RngCore for XorShift {
    fn next_u32(&mut self) -> u32 {
        (self.next() & 0xFFFFFFFF) as u32
    }

    #[inline(always)]
    fn next_u64(&mut self) -> u64 {
        self.next()
    }
    fn fill_bytes(&mut self, dest: &mut [u8]) {
        for chunk in dest.chunks_mut(8) {
            let n = self.next();
            chunk.copy_from_slice(&n.to_le_bytes());
        }
    }
    fn try_fill_bytes(&mut self, dest: &mut [u8]) -> Result<(), Error> {
        self.fill_bytes(dest);
        Ok(())
    }
}
fn direct_optimize_each_group(
    problem: &ProblemInfo,
    current_groups: &[Vec<usize>], // Input groups
    direct_opt_lower: usize,
    direct_opt_upper: usize,
    q_rem_ptr: &mut usize, // Pointer to remaining queries
    source: &mut LineSource<BufReader<io::StdinLock>>, // For reading responses
) -> HashMap<usize, Vec<(usize, usize)>> {
    // Returns the original group memberships

    // eprintln!(
    //     "Starting direct optimization for groups sized [{}, {}]",
    //     direct_opt_lower, direct_opt_upper
    // );
    let l = problem.l; // Max query size from problem info

    if direct_opt_upper > l {
        // eprintln!(
        //     "Warning: direct_opt_upper ({}) > max query size L ({}). Adjusting upper bound.",
        //     direct_opt_upper, l
        // );
        // direct_opt_upper = l; // Adjust if needed, or just skip groups > L
    }

    let mut optimized_group_msts: HashMap<usize, Vec<(usize, usize)>> = HashMap::new(); // Store results

    for (group_idx, group_members) in current_groups.iter().enumerate() {
        let group_size = group_members.len();

        // Check if group size is within the target range and <= L
        if group_size >= direct_opt_lower && group_size <= direct_opt_upper && group_size <= l {
            // Check if we have queries remaining
            if *q_rem_ptr == 0 {
                // eprintln!("No more queries left for direct optimization.");
                break;
            }

            // eprintln!(
            //     "Optimizing group {} (size {}) with 1 query...",
            //     group_idx, group_size
            // );

            // --- Prepare and Issue Query ---
            let query_indices: &[usize] = group_members; // Use the group members directly as the query

            if query_indices.len() < 2 {
                // Should be >= direct_opt_lower >= 2 usually
                //eprintln!("Skipping group {} - size < 2", group_idx);
                continue;
            }

            print!("? {}", query_indices.len());
            for &idx in query_indices {
                print!(" {}", idx);
            }
            println!(); // Flush stdout

            *q_rem_ptr -= 1; // Decrement remaining queries

            // --- Read Response ---
            let num_edges_in_response = query_indices.len() - 1;
            if num_edges_in_response > 0 {
                input! {
                    from &mut *source,
                    edges_response: [(usize, usize); num_edges_in_response]
                }
                // --- Store the "true" MST edges obtained from the query ---
                // We assume this response reflects the optimal connections for this group
                // based on the hidden true coordinates.
                optimized_group_msts.insert(group_idx, edges_response);
                //eprintln!(" -> Received MST for group {}", group_idx);
            } else {
                // eprintln!(
                //     " -> No edges in response for group {} (size {})",
                //     group_idx, group_size
                // );
            }
        }
    }
    if !optimized_group_msts.is_empty() {
        // eprintln!(
        //     "Direct optimization finished. Gathered MST info for {} groups.",
        //     optimized_group_msts.len()
        // );
    } else {
        //eprintln!("Direct optimization finished. No groups were queried (or no queries left).");
    }
    optimized_group_msts
}

fn main() {
    let time_limit = TIME_LIMIT;
    let n_particles = *N_PARTICLES; // パーティクル数
    let mh_steps_per_particle = *MH_STEPS_PER_PARTICLE; // 各パーティクルのMHステップ数

    let program_start_time = Instant::now();
    let stdin = io::stdin();
    let mut source = LineSource::new(BufReader::new(stdin.lock()));
    let mut rng = XorShift::new(0x123456789abcdef0);
    input! {
        from &mut source,
        n: usize, m: usize, q: usize, l: usize, w: i64,
        target_group_sizes: [usize; m],
        rects_input: [(i64, i64, i64, i64); n],
    }
    let initial_cities: Vec<City> = rects_input
        .iter()
        .enumerate()
        .map(|(i, &(lx, rx, ly, ry))| {
            let rect = Rect { lx, rx, ly, ry };
            City { id: i, rect }
        })
        .collect();
    let city_rects: Vec<Rect> = initial_cities.iter().map(|c| c.rect).collect();
    let mut particles = initialize_particles(n_particles, &city_rects);
    let mut constraint_index = ConstraintIndex::new(n);

    let direct_opt_lower = *DIRECT_OPT_LOWER;
    let direct_opt_upper = l;
    let direct_optimize_query_num = target_group_sizes
        .iter()
        .filter(|&&sz| sz >= direct_opt_lower && sz <= direct_opt_upper)
        .count();

    // --- Logging Initialization ---
    #[cfg(feature = "visualization")]
    let mut log_entries: Vec<LogEntry> = Vec::new();
    let problem_info = ProblemInfo {
        n,
        m,
        q,
        l,
        w,
        target_group_sizes: target_group_sizes.clone(),
        city_rects: city_rects.clone(),
    };
    let do_half_query = l >= 12;

    #[cfg(feature = "visualization")]
    record_log(
        &mut log_entries,
        0, // Step number (0-indexed)
        program_start_time,
        &particles,
        &[],  // No constraints at start
        0,    // Total constraints count
        None, // No message at start
        None, // No query issued at start
        None, // No response at start
        50,   // Log every 50 steps
    );

    let n_particles = particles.len();

    let time_limit_for_query = time_limit
        - program_start_time.elapsed().as_secs_f64()
        - *POST_ADJUST_TIME_LIMIT
        - 0.1
        - *MST_CUT_TIME_LIMIT;

    let mut query_queue: VecDeque<Vec<usize>> = VecDeque::new();
    for k in 0..(q - direct_optimize_query_num) {
        let elapsed = program_start_time.elapsed().as_secs_f64();
        // ２乗で増やしてこうかな
        // expected_time(q - direct_optimize_query_num - 1) = time_limit_for_query
        // expected_time(0) = 0
        // expected_time(k) = k * k / (q - direct_optimize_query_num - 1) / (q - direct_optimize_query_num - 1) * time_limit_for_query
        // let expected_time = (k as f64 * k as f64
        //     / ((q - direct_optimize_query_num - 1) as f64).powi(2))
        //     * time_limit_for_query;
        let expected_time =
            (k as f64 / (q - direct_optimize_query_num - 1) as f64) * time_limit_for_query;

        let remaining_time = time_limit_for_query - elapsed;
        let remain_num_query = (q - direct_optimize_query_num) - k;
        // eprintln!(
        //     "Query {}: Elapsed: {:.3}s, Expected: {:.3}s, Remaining: {:.3}s",
        //     k, elapsed, expected_time, remaining_time
        // );
        if remaining_time <= 0.0 {
            //eprintln!("Time limit approaching at query {}. Stopping.", k);
            //break;
        }

        if query_queue.is_empty() || do_half_query && query_queue.len() < 2 {
            if expected_time - elapsed < 0.0 {
                // red char escape seq
                // eprintln!("\x1b[31mWarning: Expected time exceeded elapsed time!\x1b[0m");
            }
            // もし、スケジュールより遅れてたらこれをスキップ
            for i in 0..n_particles {
                update_particles_local_mh(
                    &mut particles[i],
                    &city_rects,
                    &constraint_index,
                    (expected_time - elapsed) / n_particles as f64,
                    if elapsed < expected_time {
                        mh_steps_per_particle
                    } else {
                        0
                    },
                    true,
                    &mut rng,
                );
                //eprintln!("Particle {}: penalty = {:?}", i, particles[i].penalties);
            }
            let mut r = select_query_by_cost_variance_on_tree(
                n,
                if do_half_query { l / 2 } else { l },
                &particles,
                *N_QUERY_CANDIDATES,
                *N_QUERY_RET,
                ((if do_half_query { l / 2 } else { l }) as f64 * *N_QUERY_ALLOW_DUP_RATE) as usize,
                &mut rng,
            );
            query_queue.extend(r);
            //eprintln!("Query {}: {} candidates selected.", k, query_queue.len());
        };
        let query_indices = if do_half_query {
            let mut r = query_queue.pop_front().unwrap_or_default();
            r.extend(query_queue.pop_front().unwrap_or_default());
            r.sort_unstable();
            r.dedup();
            r
        } else {
            query_queue.pop_front().unwrap_or_default()
        };

        // if query_history.contains(&query_indices) {
        //     eprintln!("\x1b[31mWarning: Duplicate query detected!\x1b[0m");
        //     continue;
        // }
        print!("? {}", query_indices.len());
        for idx in &query_indices {
            print!(" {}", idx);
        }
        println!(); // Flush is important

        // 4. Read Response and Update Constraints
        let num_edges_in_response = query_indices.len().saturating_sub(1);
        let mut edges_response_opt: Option<Vec<(usize, usize)>> = None; // For logging
        let mut newly_added_constraints = 0;
        if num_edges_in_response > 0 {
            input! { from &mut source, edges_response: [(usize, usize); num_edges_in_response] }
            edges_response_opt = Some(edges_response.clone()); // Clone for logging

            // Generate and add new constraints
            let new_constraints = generate_constraints_from_mst(&query_indices, &edges_response);

            for constraint in new_constraints {
                let mut violated = false;
                for i in 0..particles.len() {
                    let pena = constraint.penalty(&particles[i].pos).sqrt();
                    if pena > 0 {
                        violated = true;
                        particles[i].add_violated_cities(constraint.longer_edge.0, true, pena);
                        particles[i].add_violated_cities(constraint.longer_edge.1, true, pena);
                        particles[i].add_violated_cities(constraint.shorter_edge.0, true, pena);
                        particles[i].add_violated_cities(constraint.shorter_edge.1, true, pena);
                    }
                }
                if !violated {
                    continue;
                }
                if constraint_index.add_constraint(constraint.clone(), &initial_cities) {
                    newly_added_constraints += 1;
                }
            }
            // Optional: Log constraint addition info
            if newly_added_constraints > 0 {
                // eprintln!(
                //     "Query {}: Added {} non-redundant constraints. Total: {}",
                //     k, newly_added_constraints, constraint_index.total_constraints
                // );
            }
        }

        // 5. Logging (Feature Gated)
        #[cfg(feature = "visualization")]
        {
            let mst_loss = 0.0; //expected_mst_loss(&particle_positions, &calc_avg_pos(&particle_positions));
            let msg = format!(
                "After Query {} ({} added) MSTLoss: {:.3}",
                k, newly_added_constraints, mst_loss
            );
            record_log(
                &mut log_entries,
                k + 1, // Step number (1-indexed)
                program_start_time,
                &particles,
                &vec![],
                constraint_index.get_all_constraints().len(),
                Some(msg),
                Some(query_indices.clone()),
                edges_response_opt,
                50, // Log every 50 steps
            );
        }
    }
    let remaining_time =
        (TIME_LIMIT - program_start_time.elapsed().as_secs_f64() - 0.1 - *MST_CUT_TIME_LIMIT)
            .max(*POST_ADJUST_TIME_LIMIT); // Use remaining time for final SA
    for i in 0..particles.len() {
        update_particles_local_mh(
            &mut particles[i],
            &city_rects,
            &constraint_index,
            remaining_time / n_particles as f64,
            mh_steps_per_particle,
            false,
            &mut rng,
        );
    }

    let elapsed_time = program_start_time.elapsed().as_secs_f64();
    //eprintln!("Query loop completed in {:.3}s", elapsed_time);

    // --- Final Decision ---
    //eprintln!("Calculating final assignment...");
    // Calculate average position from particles
    //let final_avg_pos = calc_avg_pos(&particles);
    let final_avg_pos = calc_avg_pos(&particles);
    // Run partitioning (e.g., SA) on the average positions
    let sa_time_limit =
        (TIME_LIMIT - program_start_time.elapsed().as_secs_f64() - 0.1).max(*MST_CUT_TIME_LIMIT); // Use remaining time for final SA

    let avg_length_mat = construct_avg_length_matrix(&particles, n);
    let (_, final_groups, _) = assign_groups_sa_mst_cut_simplified_cost(
        n,
        m,
        &final_avg_pos,
        &avg_length_mat,
        &target_group_sizes,
        Duration::from_secs_f64(sa_time_limit),
        0.0,                       // Initial penalty weight
        *MST_FINAL_PENALTY_WEIGHT, // Final penalty weight
        &mut rng,
    );

    // --- Save Log File (Feature Gated) ---
    #[cfg(feature = "visualization")]
    {
        //set answer mst
        let mut edges_response_opt = vec![];
        for group_members in &final_groups {
            let (_cost, mst_edges) = calculate_mst(group_members, &final_avg_pos);
            if mst_edges.len() != group_members.len() - 1 {
                // eprintln!("Warning: Outputting incomplete MST for final group size {}. Found {} edges, expected {}.", group_members.len(), mst_edges.len(), group_members.len().saturating_sub(1));
            }
            for (u, v) in mst_edges {
                edges_response_opt.push((u, v));
            }
        }

        record_log(
            &mut log_entries,
            99999, // Step number (arbitrary large number for final step)
            program_start_time,
            &particles,
            &vec![],
            0,
            Some("After MST cutting SA.".to_string()),
            None, // No query issued at end
            Some(edges_response_opt),
            50, // Log every 50 steps
        );
    }
    let final_groups = adjust_group_sizes_with_sequential_mcf_path(
        n,
        m,
        final_groups,
        &target_group_sizes,
        &avg_length_mat,
        &particles,
    );

    let final_groups = reorder_groups(&final_groups, &target_group_sizes);

    let mut rem_queries = direct_optimize_query_num;
    let optimized_mst_map = direct_optimize_each_group(
        &problem_info,
        &final_groups,
        direct_opt_lower,
        direct_opt_upper,
        &mut rem_queries,
        &mut source,
    );

    // --- Final Output ---
    println!("!");
    for (g_idx, group_members) in final_groups.iter().enumerate() {
        for &city_idx in group_members {
            print!("{} ", city_idx);
        }
        println!();

        if group_members.len() > 1 {
            let mst_edges = if let Some(opt_mst) = optimized_mst_map.get(&g_idx) {
                opt_mst.clone()
            } else {
                calculate_mst(group_members, &final_avg_pos).1
            };
            if mst_edges.len() != group_members.len() - 1 {
                // eprintln!("Warning: Outputting incomplete MST for final group size {}. Found {} edges, expected {}.", group_members.len(), mst_edges.len(), group_members.len().saturating_sub(1));
            }
            for (u, v) in mst_edges {
                println!("{} {}", u, v);
            }
        }
    }

    // --- Save Log File (Feature Gated) ---
    #[cfg(feature = "visualization")]
    {
        //set answer mst
        let mut edges_response_opt = vec![];
        for group_members in &final_groups {
            let (_cost, mst_edges) = calculate_mst(group_members, &final_avg_pos);
            if mst_edges.len() != group_members.len() - 1 {
                // eprintln!("Warning: Outputting incomplete MST for final group size {}. Found {} edges, expected {}.", group_members.len(), mst_edges.len(), group_members.len().saturating_sub(1));
            }
            for (u, v) in mst_edges {
                edges_response_opt.push((u, v));
            }
        }

        record_log(
            &mut log_entries,
            1000000, // Step number (arbitrary large number for final step)
            program_start_time,
            &particles,
            &vec![],
            0,
            Some("Final assignment completed.".to_string()),
            None, // No query issued at end
            Some(edges_response_opt),
            50, // Log every 50 steps
        );
        save_log(&problem_info, &log_entries, "visualization_log.json");
    }

    // forget all
    forget(particles);
    forget(constraint_index);
    forget(query_queue);
    forget(final_groups);
    forget(optimized_mst_map);
    forget(final_avg_pos);
    forget(city_rects);
    forget(initial_cities);
    forget(avg_length_mat);

    //let final_elapsed = program_start_time.elapsed().as_secs_f64();
    //eprintln!("Total execution time: {:.3}s", final_elapsed);
}

// --- Helper: Normalize (Needed by MH/Force-Directed if used) ---
#[inline(always)]
fn normalize(dx: f64, dy: f64) -> Option<(f64, f64)> {
    let norm_sq = dx * dx + dy * dy;
    if norm_sq < 1e-12 {
        None
    } else {
        let norm = norm_sq.sqrt();
        Some((dx / norm, dy / norm))
    }
}

// 0006が、M=1なので、推定性能を図りたいときに便利

// TODO: 一回も動いてない時は、すでに使用済みのrootsは採用しない

// MCFが多いと、当然スコア下がる
// クエリー被りがやっぱ嫌だな

// 0091
/*
800 18 400 5 2342
32 23 9 106 83 7 70 9 8 35 41 18 18 236 10 41 49 5

MCF 8回 -> 289979
MCF 7回 -> 294323, 294591



TL: 10.98sec M_SPLIT: 5 -> 279382
TL: 10.98sec M_SPLIT: 1 -> 279382
*/
