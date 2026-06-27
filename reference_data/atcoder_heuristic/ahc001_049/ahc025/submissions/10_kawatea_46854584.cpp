#include <cstdio>
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

const int MAX_N = 100;
const int border[MAX_N + 1][MAX_N / 4 + 1] = {
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {0, 0, 139, 147, 163, 159, 161, 154},
    {0, 0, 145, 153, 169, 165, 166, 162},
    {0, 0, 150, 161, 177, 175, 174, 170, 168},
    {0, 0, 157, 166, 184, 183, 189, 178, 177},
    {0, 0, 163, 173, 192, 191, 193, 188, 186},
    {0, 0, 169, 181, 200, 198, 202, 195, 194},
    {0, 0, 176, 187, 206, 206, 212, 203, 202, 194},
    {0, 0, 182, 194, 214, 213, 215, 210, 213, 204},
    {0, 0, 188, 200, 224, 222, 229, 222, 222, 212},
    {0, 0, 194, 206, 230, 230, 238, 227, 230, 220},
    {0, 0, 200, 213, 238, 239, 245, 242, 241, 230, 228},
    {0, 0, 206, 219, 244, 244, 252, 252, 247, 238, 238},
    {0, 0, 213, 225, 253, 255, 261, 261, 257, 248, 247},
    {0, 0, 220, 234, 261, 264, 270, 267, 266, 259, 255},
    {0, 0, 226, 241, 267, 270, 276, 273, 277, 268, 265, 258},
    {0, 0, 233, 247, 276, 277, 286, 285, 292, 278, 273, 267},
    {0, 0, 238, 255, 285, 285, 295, 299, 296, 288, 282, 279},
    {0, 0, 246, 262, 293, 293, 305, 307, 318, 296, 294, 287},
    {0, 0, 253, 269, 300, 302, 315, 313, 326, 306, 306, 296, 294},
    {0, 0, 260, 275, 307, 311, 321, 321, 333, 316, 319, 305, 306},
    {0, 0, 266, 283, 315, 318, 332, 332, 348, 334, 331, 316, 317},
    {0, 0, 273, 289, 324, 327, 341, 342, 353, 350, 336, 325, 326},
    {0, 0, 279, 296, 331, 338, 349, 353, 369, 358, 345, 337, 335, 326},
    {0, 0, 287, 305, 339, 343, 360, 362, 377, 365, 356, 345, 345, 337},
    {0, 0, 293, 312, 347, 353, 368, 370, 387, 375, 383, 357, 356, 349},
    {0, 0, 300, 318, 356, 361, 377, 380, 396, 387, 392, 370, 365, 358},
    {0, 0, 307, 327, 363, 369, 385, 387, 405, 400, 399, 380, 378, 366, 362},
    {0, 0, 315, 334, 372, 376, 394, 394, 410, 407, 409, 388, 390, 376, 373},
    {0, 0, 321, 340, 379, 387, 403, 404, 424, 416, 417, 401, 404, 385, 385},
    {0, 0, 329, 348, 387, 394, 413, 417, 433, 432, 431, 405, 415, 397, 393},
    {0, 0, 335, 354, 394, 401, 419, 426, 445, 442, 437, 415, 422, 406, 406, 397},
    {0, 0, 342, 363, 402, 410, 428, 436, 455, 455, 445, 431, 433, 416, 412, 407},
    {0, 0, 349, 370, 411, 418, 438, 445, 464, 460, 458, 458, 444, 434, 421, 418},
    {0, 0, 357, 378, 420, 428, 448, 452, 478, 468, 465, 470, 454, 445, 430, 426},
    {0, 0, 363, 384, 427, 435, 456, 462, 482, 480, 481, 479, 460, 460, 444, 437, 440},
    {0, 0, 372, 393, 435, 444, 462, 472, 492, 491, 496, 485, 492, 470, 458, 449, 450},
    {0, 0, 378, 400, 443, 453, 473, 481, 503, 500, 505, 496, 510, 477, 467, 457, 458},
    {0, 0, 386, 407, 452, 461, 482, 489, 513, 511, 514, 506, 518, 486, 480, 470, 469},
    {0, 0, 392, 415, 459, 470, 490, 499, 530, 522, 527, 513, 527, 496, 494, 483, 482, 467},
    {0, 0, 400, 423, 466, 479, 499, 509, 540, 535, 537, 528, 538, 505, 507, 493, 492, 482},
    {0, 0, 406, 430, 476, 487, 512, 516, 546, 539, 547, 540, 551, 542, 517, 508, 500, 493},
    {0, 0, 414, 438, 485, 494, 519, 525, 554, 553, 558, 546, 566, 551, 550, 544, 513, 503},
    {0, 0, 420, 445, 491, 504, 527, 533, 563, 566, 567, 566, 565, 570, 541, 530, 520, 515, 507},
    {0, 0, 429, 452, 501, 513, 534, 542, 577, 574, 577, 579, 580, 577, 548, 543, 538, 525, 520},
    {0, 0, 435, 460, 511, 522, 542, 553, 584, 582, 591, 587, 592, 587, 592, 553, 558, 537, 530},
    {0, 0, 444, 467, 518, 530, 552, 563, 594, 592, 606, 597, 603, 600, 596, 569, 565, 543, 543},
    {0, 0, 450, 475, 526, 538, 564, 572, 605, 602, 611, 605, 610, 603, 613, 578, 578, 555, 550, 539},
    {0, 0, 458, 483, 533, 546, 570, 583, 615, 616, 621, 616, 628, 623, 620, 586, 591, 565, 562, 554},
    {0, 0, 464, 489, 543, 555, 579, 589, 625, 624, 638, 630, 643, 630, 638, 620, 604, 579, 572, 566},
    {0, 0, 472, 498, 551, 565, 588, 597, 640, 640, 646, 637, 656, 643, 643, 636, 645, 614, 582, 576},
    {0, 0, 478, 506, 559, 570, 596, 608, 644, 650, 652, 644, 669, 653, 661, 649, 627, 616, 594, 589, 582},
    {0, 0, 488, 514, 568, 580, 607, 618, 657, 659, 661, 665, 671, 672, 669, 667, 665, 632, 606, 596, 597},
    {0, 0, 494, 521, 575, 587, 616, 628, 666, 664, 677, 671, 687, 681, 681, 676, 675, 631, 615, 607, 606},
    {0, 0, 502, 529, 584, 597, 629, 638, 676, 672, 684, 689, 697, 692, 689, 693, 695, 646, 683, 618, 620},
    {0, 0, 509, 537, 593, 608, 632, 648, 682, 685, 696, 698, 704, 699, 703, 699, 704, 657, 649, 627, 634, 622},
    {0, 0, 516, 545, 600, 613, 642, 656, 695, 698, 711, 709, 723, 717, 724, 716, 717, 671, 708, 640, 641, 633},
    {0, 0, 523, 552, 609, 623, 651, 663, 707, 709, 727, 719, 728, 730, 722, 728, 735, 715, 684, 646, 655, 645},
    {0, 0, 531, 562, 619, 631, 661, 676, 715, 720, 728, 728, 748, 737, 735, 737, 743, 724, 692, 663, 663, 657},
    {0, 0, 539, 568, 627, 641, 669, 686, 722, 733, 738, 737, 762, 759, 748, 747, 768, 735, 699, 697, 676, 667, 661},
    {0, 0, 546, 576, 636, 649, 678, 697, 730, 741, 750, 746, 769, 763, 763, 756, 786, 749, 714, 718, 686, 680, 676},
    {0, 0, 554, 585, 645, 660, 688, 702, 744, 752, 761, 766, 783, 780, 780, 763, 789, 780, 720, 753, 772, 688, 687},
    {0, 0, 561, 592, 653, 667, 695, 714, 757, 762, 768, 777, 796, 793, 790, 785, 814, 776, 733, 770, 782, 701, 697},
    {0, 0, 569, 601, 662, 676, 704, 718, 764, 765, 779, 784, 805, 798, 808, 796, 825, 811, 785, 787, 795, 714, 708, 699},
    {0, 0, 577, 609, 670, 686, 715, 730, 777, 780, 791, 792, 817, 814, 823, 815, 819, 813, 803, 790, 799, 720, 719, 711},
    {0, 0, 584, 617, 680, 695, 728, 742, 786, 789, 802, 815, 825, 827, 845, 823, 846, 838, 809, 773, 812, 731, 731, 723},
    {0, 0, 593, 625, 688, 704, 733, 754, 796, 807, 815, 827, 835, 839, 844, 837, 862, 865, 839, 776, 838, 742, 744, 739},
    {0, 0, 599, 632, 697, 712, 741, 763, 807, 815, 837, 834, 849, 851, 858, 871, 864, 856, 856, 791, 839, 760, 751, 746, 750},
    {0, 0, 608, 640, 705, 721, 753, 770, 817, 827, 838, 847, 867, 858, 871, 871, 874, 885, 878, 843, 845, 858, 765, 760, 760},
    {0, 0, 614, 649, 714, 731, 762, 780, 826, 836, 849, 852, 879, 866, 878, 877, 892, 879, 887, 856, 813, 857, 780, 768, 772},
    {0, 0, 624, 658, 724, 739, 773, 785, 834, 840, 859, 868, 883, 888, 877, 890, 903, 890, 908, 867, 872, 859, 870, 778, 785},
    {0, 0, 630, 665, 732, 747, 781, 795, 846, 857, 876, 873, 897, 900, 884, 911, 909, 905, 912, 883, 877, 863, 880, 789, 794, 787}
};

int N, D, Q;
bool full_comparison;
int comparison[MAX_N][MAX_N];
int comparison2[MAX_N][MAX_N][MAX_N];
int right2[MAX_N][MAX_N];
map<vector<int>, map<vector<int>, int>> cache;

void update_comparison(int x, int y) {
    for (int i : {x, y}) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                if (comparison[j][i] != -2 && comparison[i][k] != -2) {
                    if (comparison[j][i] >= 0 && comparison[i][k] >= 0) comparison[j][k] = max(comparison[j][i], comparison[i][k]);
                    if (comparison[j][i] <= 0 && comparison[i][k] <= 0) comparison[j][k] = min(comparison[j][i], comparison[i][k]);
                }
            }
        }
    }
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (comparison[i][j] == -2) return;
        }
    }
    full_comparison = true;
}

void input() {
    scanf("%d %d %d", &N, &D, &Q);
}

int query0(vector<int> vl, vector<int> vr) {
    if (cache[vl].count(vr)) return cache[vl][vr];
    if (vl.empty() && vr.empty()) return 0;
    if (vl.empty()) return -1;
    if (vr.empty()) return 1;
    
    if (vl.size() == 1 && vr.size() == 1 && comparison[vl[0]][vr[0]] != -2) {
        cache[vl][vr] = comparison[vl[0]][vr[0]];
        cache[vr][vl] = -comparison[vl[0]][vr[0]];
        return comparison[vl[0]][vr[0]];
    }
    
    if (vl.size() == 2 && vr.size() == 1) {
        if (comparison[vl[0]][vr[0]] >= 0 || comparison[vl[1]][vr[0]] >= 0) {
            cache[vl][vr] = 1;
            cache[vr][vl] = -1;
            return 1;
        }
        if (comparison2[vl[0]][vl[1]][vr[0]] != -2) {
            cache[vl][vr] = comparison2[vl[0]][vl[1]][vr[0]];
            cache[vr][vl] = -comparison2[vl[0]][vl[1]][vr[0]];
            return comparison2[vl[0]][vl[1]][vr[0]];
        }
    }
    
    if (vl.size() == 1 && vr.size() == 2) {
        if (comparison[vr[0]][vl[0]] >= 0 || comparison[vr[1]][vl[0]] >= 0) {
            cache[vl][vr] = -1;
            cache[vr][vl] = 1;
            return -1;
        }
        if (comparison2[vr[0]][vr[1]][vl[0]] != -2) {
            cache[vl][vr] = -comparison2[vr[0]][vr[1]][vl[0]];
            cache[vr][vl] = comparison2[vr[0]][vr[1]][vl[0]];
            return -comparison2[vr[0]][vr[1]][vl[0]];
        }
    }
    
    if (vl.size() == 2 && vr.size() == 2) {
        if ((comparison[vl[0]][vr[0]] >= 0 && comparison[vl[1]][vr[1]] >= 0) || (comparison[vl[0]][vr[1]] >= 0 && comparison[vl[1]][vr[0]] >= 0)) {
            cache[vl][vr] = 1;
            cache[vr][vl] = -1;
            return 1;
        }
        if ((comparison[vr[0]][vl[0]] >= 0 && comparison[vr[1]][vl[1]] >= 0) || (comparison[vr[0]][vl[1]] >= 0 && comparison[vr[1]][vl[0]] >= 0)) {
            cache[vl][vr] = -1;
            cache[vr][vl] = 1;
            return -1;
        }
        if (comparison2[vl[0]][vl[1]][0] != -2 && comparison2[vr[0]][vr[1]][0] != -2 && right2[vl[0]][vl[1]] != right2[vr[0]][vr[1]]) {
            int cmp = 0;
            if (right2[vl[0]][vl[1]] < right2[vr[0]][vr[1]]) cmp = 1;
            if (right2[vl[0]][vl[1]] > right2[vr[0]][vr[1]]) cmp = -1;
            cache[vl][vr] = cmp;
            cache[vr][vl] = -cmp;
            return cmp;
        }
    }
    
    return -2;
}

int query(vector<int> vl, vector<int> vr) {
    sort(vl.begin(), vl.end());
    sort(vr.begin(), vr.end());
    
    int cmp0 = query0(vl, vr);
    if (cmp0 != -2) return cmp0;
    
    for (const pair<vector<int>, int>& p : cache[vl]) {
        if (cache[vr].count(p.first)) {
            int cmp1 = p.second;
            int cmp2 = -cache[vr][p.first];
            int cmp = -2;
            if (cmp1 <= 0 && cmp2 <= 0) cmp = min(cmp1, cmp2);
            if (cmp1 >= 0 && cmp2 >= 0) cmp = max(cmp1, cmp2);
            if (cmp != -2) {
                cache[vl][vr] = cmp;
                cache[vr][vl] = -cmp;
                if (vl.size() == 1 && vr.size() == 1) {
                    comparison[vl[0]][vr[0]] = cmp;
                    comparison[vr[0]][vl[0]] = -cmp;
                    update_comparison(vl[0], vr[0]);
                }
                return cmp;
            }
        }
        if (vr.size() <= 2 && p.first.size() <= 2) {
            int cmp1 = p.second;
            int cmp2 = query0(p.first, vr);
            if (cmp2 == -2) continue;
            int cmp = -2;
            if (cmp1 <= 0 && cmp2 <= 0) cmp = min(cmp1, cmp2);
            if (cmp1 >= 0 && cmp2 >= 0) cmp = max(cmp1, cmp2);
            if (cmp != -2) {
                cache[vl][vr] = cmp;
                cache[vr][vl] = -cmp;
                if (vl.size() == 1 && vr.size() == 1) {
                    comparison[vl[0]][vr[0]] = cmp;
                    comparison[vr[0]][vl[0]] = -cmp;
                    update_comparison(vl[0], vr[0]);
                }
                return cmp;
            }
        }
    }
    for (const pair<vector<int>, int>& p : cache[vr]) {
        if (vl.size() <= 2 && p.first.size() <= 2) {
            int cmp1 = p.second;
            int cmp2 = query0(p.first, vl);
            if (cmp2 == -2) continue;
            int cmp = -2;
            if (cmp1 <= 0 && cmp2 <= 0) cmp = min(cmp1, cmp2);
            if (cmp1 >= 0 && cmp2 >= 0) cmp = max(cmp1, cmp2);
            if (cmp != -2) {
                cache[vl][vr] = -cmp;
                cache[vr][vl] = cmp;
                if (vl.size() == 1 && vr.size() == 1) {
                    comparison[vl[0]][vr[0]] = -cmp;
                    comparison[vr[0]][vl[0]] = cmp;
                    update_comparison(vl[0], vr[0]);
                }
                return -cmp;
            }
        }
    }
    
    if (vl.size() <= 4 && vr.size() <= 2) {
        for (int i = 0; i < vl.size(); i++) {
            for (int j = i + 1; j < vl.size(); j++) {
                if (query0({vl[i], vl[j]}, vr) >= 0) {
                    cache[vl][vr] = 1;
                    cache[vr][vl] = -1;
                    return 1;
                }
            }
        }
    }
    if (vr.size() <= 4 && vl.size() <= 2) {
        for (int i = 0; i < vr.size(); i++) {
            for (int j = i + 1; j < vr.size(); j++) {
                if (query0({vr[i], vr[j]}, vl) >= 0) {
                    cache[vl][vr] = -1;
                    cache[vr][vl] = 1;
                    return -1;
                }
            }
        }
    }
    
    if (Q == 0) {
        fprintf(stderr, "ERROR\n");
        exit(0);
    }
    Q--;
    
    printf("%d %d", vl.size(), vr.size());
    for (int l : vl) printf(" %d", l);
    for (int r : vr) printf(" %d", r);
    puts("");
    fflush(stdout);
    
    char s[2];
    scanf("%s", s);
    int res;
    if (s[0] == '<') {
        res = -1;
    } else if (s[0] == '>') {
        res = 1;
    } else {
        res = 0;
    }
    cache[vl][vr] = res;
    cache[vr][vl] = -res;
    if (vl.size() == 1 && vr.size() == 1) {
        comparison[vl[0]][vr[0]] = res;
        comparison[vr[0]][vl[0]] = -res;
        update_comparison(vl[0], vr[0]);
    }
    return res;
}

int query(int l, int r) {
    return query(vector<int>(1, l), vector<int>(1, r));
}

void output(const vector<int> d) {
    for (int i = 0; i < d.size(); i++) {
        if (i > 0) putchar(' ');
        printf("%d", d[i]);
    }
    puts("");
    fflush(stdout);
}

void init() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i == j) {
                comparison[i][j] = 0;
            } else {
                comparison[i][j] = -2;
            }
        }
    }
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                comparison2[i][j][k] = -2;
            }
        }
    }
}

bool compare2(int item1, int item2) {
    if (comparison[item2][item1] > 0) swap(item1, item2);
    if (comparison2[item1][item2][0] != -2) return true;
    
    vector<int> order;
    for (int i = 0; i < N; i++) order.push_back(i);
    sort(order.begin(), order.end(), [&](int item1, int item2) {
        return comparison[item1][item2] > 0;
    });
    
    int index1, index2;
    for (int i = 0; i < N; i++) {
        if (order[i] == item1) index1 = i;
        if (order[i] == item2) index2 = i;
    }
    int l = -1, r = index1;
    vector<int> vl = {min(item1, item2), max(item1, item2)};
    for (int i = 0; i < N; i++) {
        vector<int> vr = {order[i]};
        if (cache[vl].count(vr)) {
            int cmp = cache[vl][vr];
            if (cmp == 0) {
                for (int j = 0; j < N; j++) comparison2[item1][item2][j] = comparison2[item2][item1][j] = comparison[order[i]][j];
                right2[item1][item2] = right2[item2][item1] = i;
                return Q > 0;
            } else if (cmp > 0) {
                r = min(r, i);
            } else {
                l = max(l, i);
            }
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            if (comparison2[order[i]][order[j]][0] == -2) continue;
            if (i >= index1 && j >= index2) {
                r = min(r, right2[order[i]][order[j]]);
            } else if (i <= index1 && j <= index2) {
                l = max(l, right2[order[i]][order[j]] - 1);
            } else {
                vector<int> vr = {min(order[i], order[j]), max(order[i], order[j])};
                if (cache[vl].count(vr)) {
                    int cmp = cache[vl][vr];
                    if (cmp == 0) {
                        for (int k = 0; k < N; k++) comparison2[item1][item2][k] = comparison2[item2][item1][k] = comparison2[order[i]][order[j]][k];
                        right2[item1][item2] = right2[item2][item1] = right2[order[i]][order[j]];
                        return Q > 0;
                    } else if (cmp > 0) {
                        r = min(r, right2[order[i]][order[j]]);
                    } else {
                        l = max(l, right2[order[i]][order[j]] - 1);
                    }
                }
            }
        }
    }
    
    while (r - l > 1 && Q > 0) {
        int m = (l + r) / 2;
        int cmp = query({item1, item2}, {order[m]});
        if (cmp == 0) {
            for (int i = 0; i < N; i++) comparison2[item1][item2][i] = comparison2[item2][item1][i] = comparison[order[m]][i];
            right2[item1][item2] = right2[item2][item1] = m;
            return Q > 0;
        } else if (cmp > 0) {
            r = m;
        } else {
            l = m;
        }
    }
    
    for (int i = 0; i < r; i++) comparison2[item1][item2][order[i]] = comparison2[item2][item1][order[i]] = -1;
    for (int i = r; i < N; i++) comparison2[item1][item2][order[i]] = comparison2[item2][item1][order[i]] = 1;
    right2[item1][item2] = right2[item2][item1] = r;
    return Q > 0;
}

template<class T> void binary_insertion(vector<T>& items, const T& item, int l, int r) {
    while (r - l > 1 && Q > 0) {
        int m = (l + r) / 2;
        int cmp = query(items[m], item);
        if (cmp == 0) {
            r = m;
            break;
        } else if (cmp > 0) {
            l = m;
        } else {
            r = m;
        }
    }
    items.insert(items.begin() + r, item);
}

template<class T> void binary_insertion(vector<T>& items, const T& item) {
    binary_insertion(items, item, -1, items.size());
}

template<class T> vector<T> merge_insertion_sort(const vector<T>& origin) {
    vector<T> sorted;
    if (origin.size() <= 1) {
        sorted = origin;
    } else {
        vector<T> small;
        map<T, T> couple;
        for (int i = 0; i + 1 < origin.size(); i += 2) {
            if (Q == 0) return origin;
            if (query(origin[i], origin[i + 1]) < 0) {
                small.push_back(origin[i]);
            } else {
                small.push_back(origin[i + 1]);
            }
            couple[origin[i]] = origin[i + 1];
            couple[origin[i + 1]] = origin[i];
        }
        sorted = merge_insertion_sort(small);
        
        vector<T> large;
        for (int i = 0; i < sorted.size(); i++) large.push_back(couple[sorted[i]]);
        if (origin.size() % 2 == 1) large.push_back(origin.back());
        sorted.insert(sorted.begin(), large[0]);
        for (int i = 1, x = 2, y = 0; i < large.size(); i += y) {
            for (int l = i, r = min(i + x, (int)large.size()) - 1; l < r; l++, r--) swap(large[l], large[r]);
            x += y * 2;
            y = x - y * 2;
        }
        for (int i = 1; i < large.size(); i++) {
            int l = -1, r = 0;
            if (couple.count(large[i])) {
                for (; r < sorted.size(); r++) {
                    if (couple[large[i]] == sorted[r]) break;
                }
            } else {
                r = sorted.size();
            }
            binary_insertion(sorted, large[i], l, r);
        }
    }
    return sorted;
}

bool update(const vector<int>& bl, const vector<int>& br, const vector<int>& nl, const vector<int>& nr) {
    if (bl.empty() && br.empty()) return true;
    vector<int> vl, vr;
    for (int l : bl) vl.push_back(l);
    for (int r : nr) vl.push_back(r);
    for (int r : br) vr.push_back(r);
    for (int l : nl) vr.push_back(l);
    sort(vl.begin(), vl.end());
    sort(vr.begin(), vr.end());
    for (int i = 0, j = 0; i < vl.size() && j < vr.size(); ) {
        if (vl[i] == vr[j]) {
            vl.erase(vl.begin() + i);
            vr.erase(vr.begin() + j);
        } else if (vl[i] < vr[j]) {
            i++;
        } else {
            j++;
        }
    }
    return query(vl, vr) > 0;
}

bool improve_swap12(vector<vector<int>>& items) {
    for (int i = 0; i < D; i++) sort(items[i].begin(), items[i].end(), [&](int item1, int item2) {
        return comparison[item1][item2] < 0;
    });
    
    static int smallest = -1;
    for (int interval = D - 1; interval > 0 && Q > 0; interval--) {
        for (int heavy = 0; heavy + interval < D && Q > 0; heavy++) {
            int light = heavy + interval;
            bool all = (heavy == 0 && light == D - 1);
            vector<int> bl, br;
            if (items[heavy].size() > 1) {
                for (int item1 : items[heavy]) {
                    if (Q == 0) return false;
                    int item2 = -1;
                    for (int item : items[light]) {
                        if (comparison[item][item1] >= 0) break;
                        item2 = item;
                    }
                    if (item2 == -1 && smallest >= 0 && comparison[item1][smallest] >= 0) continue;
                    
                    vector<int> nl = {item1};
                    vector<int> nr;
                    if (item2 != -1) nr.push_back(item2);
                    if (!update(bl, br, nl, nr)) continue;
                    if (Q == 0) return false;
                    bl = nl;
                    br = nr;
                    
                    vector<int> vl, vr;
                    for (int item : items[heavy]) {
                        if (item != item1) vl.push_back(item);
                    }
                    for (int item : items[light]) {
                        if (item != item2) vr.push_back(item);
                    }
                    if (query(vl, vr) <= 0) {
                        if (all && item2 == -1) smallest = item1;
                        continue;
                    }
                    
                    if (item2 >= 0) vl.push_back(item2);
                    vr.push_back(item1);
                    items.erase(items.begin() + light);
                    items.erase(items.begin() + heavy);
                    binary_insertion(items, vl, heavy - 1, light - 1);
                    binary_insertion(items, vr, heavy - 1, light);
                    return true;
                }
            }
            vector<pair<int, int>> candidates_heavy;
            if (items[heavy].size() > 2) {
                for (int i = 0; i < items[heavy].size(); i++) {
                    for (int j = i + 1; j < items[heavy].size(); j++) {
                        if (comparison[items[heavy][j]][items[light].back()] >= 0) continue;
                        candidates_heavy.emplace_back(i, j);
                    }
                }
                sort(candidates_heavy.begin(), candidates_heavy.end(), [](const pair<int, int>& p1, const pair<int, int>& p2) {
                    if (p1.first + p1.second != p2.first + p2.second) p1.first + p1.second < p2.first + p2.second;
                    return p1.second < p2.second;
                });
                for (const pair<int, int>& p : candidates_heavy) {
                    if (Q == 0) return false;
                    int item1 = items[heavy][p.first];
                    int item2 = items[heavy][p.second];
                    int item3 = -1;
                    if (!compare2(item1, item2)) return false;
                    for (int item : items[light]) {
                        if (comparison2[item1][item2][item] <= 0) break;
                        item3 = item;
                    }
                    if (item3 == -1 || comparison[item1][item3] >= 0 || comparison[item2][item3] >= 0) continue;
                    
                    vector<int> nl = {item1, item2};
                    vector<int> nr = {item3};
                    if (!update(bl, br, nl, nr)) continue;
                    if (Q == 0) return false;
                    bl = nl;
                    br = nr;
                    
                    vector<int> vl, vr;
                    for (int item : items[heavy]) {
                        if (item != item1 && item != item2) vl.push_back(item);
                    }
                    for (int item : items[light]) {
                        if (item != item3) vr.push_back(item);
                    }
                    if (query(vl, vr) <= 0) continue;
                    
                    vl.push_back(item3);
                    vr.push_back(item1);
                    vr.push_back(item2);
                    items.erase(items.begin() + light);
                    items.erase(items.begin() + heavy);
                    binary_insertion(items, vl, heavy - 1, light - 1);
                    binary_insertion(items, vr, heavy - 1, light);
                    return true;
                }
            }
            vector<pair<int, int>> candidates_light;
            if (items[light].size() > 2) {
                for (int i = 0; i < items[light].size(); i++) {
                    for (int j = i + 1; j < items[light].size(); j++) {
                        if (comparison[items[light][j]][items[heavy].back()] >= 0) continue;
                        candidates_light.emplace_back(i, j);
                    }
                }
                sort(candidates_light.begin(), candidates_light.end(), [](const pair<int, int>& p1, const pair<int, int>& p2) {
                    if (p1.first + p1.second != p2.first + p2.second) p1.first + p1.second < p2.first + p2.second;
                    return p1.second < p2.second;
                });
                for (const pair<int, int>& p : candidates_light) {
                    if (Q == 0) return false;
                    int item1 = items[light][p.first];
                    int item2 = items[light][p.second];
                    int item3 = -1;
                    if (!compare2(item1, item2)) return false;
                    for (int item : items[heavy]) {
                        if (comparison2[item1][item2][item] < 0) {
                            item3 = item;
                            break;
                        }
                    }
                    if (item3 == -1) continue;
                    
                    vector<int> nl = {item3};
                    vector<int> nr = {item1, item2};
                    if (!update(bl, br, nl, nr)) continue;
                    if (Q == 0) return false;
                    bl = nl;
                    br = nr;
                    
                    vector<int> vl, vr;
                    for (int item : items[heavy]) {
                        if (item != item3) vl.push_back(item);
                    }
                    for (int item : items[light]) {
                        if (item != item1 && item != item2) vr.push_back(item);
                    }
                    if (query(vl, vr) <= 0) continue;
                    
                    vl.push_back(item1);
                    vl.push_back(item2);
                    vr.push_back(item3);
                    items.erase(items.begin() + light);
                    items.erase(items.begin() + heavy);
                    binary_insertion(items, vl, heavy - 1, light - 1);
                    binary_insertion(items, vr, heavy - 1, light);
                    return true;
                }
            }
            if (items[heavy].size() > 2 && items[light].size() > 2) {
                for (const pair<int, int>& p1 : candidates_heavy) {
                    for (const pair<int, int>& p2 : candidates_light) {
                        if (Q == 0) return false;
                        int item1 = items[heavy][p1.first];
                        int item2 = items[heavy][p1.second];
                        int item3 = items[light][p2.first];
                        int item4 = items[light][p2.second];
                        if (comparison[item1][item4] >= 0) continue;
                        if (comparison[item2][item3] <= 0) continue;
                        if (comparison[item1][item3] >= 0 && comparison[item2][item4] >= 0) continue;
                        if (comparison2[item3][item4][item2] <= 0) continue;
                        if (query({item1, item2}, {item3, item4}) <= 0) continue;
                        if (Q == 0) return false;
                        
                        vector<int> nl = {item1, item2};
                        vector<int> nr = {item3, item4};
                        if (!update(bl, br, nl, nr)) continue;
                        if (Q == 0) return false;
                        bl = nl;
                        br = nr;
                        
                        vector<int> vl, vr;
                        for (int item : items[heavy]) {
                            if (item != item1 && item != item2) vl.push_back(item);
                        }
                        for (int item : items[light]) {
                            if (item != item3 && item != item4) vr.push_back(item);
                        }
                        if (query(vl, vr) <= 0) continue;
                        
                        vl.push_back(item3);
                        vl.push_back(item4);
                        vr.push_back(item1);
                        vr.push_back(item2);
                        items.erase(items.begin() + light);
                        items.erase(items.begin() + heavy);
                        binary_insertion(items, vl, heavy - 1, light - 1);
                        binary_insertion(items, vr, heavy - 1, light);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool improve_swap(vector<vector<int>>& items) {
    if (full_comparison) return improve_swap12(items);
    int sum[N] = {};
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            sum[i] += comparison[i][j];
        }
    }
    for (int i = 0; i < D; i++) sort(items[i].begin(), items[i].end(), [&](int item1, int item2) {
        return sum[item1] < sum[item2];
    });
    
    static int smallest = -1;
    for (int iteration = 0; iteration < 2; iteration++) {
        for (int interval = D - 1; interval > 0 && Q > 0; interval--) {
            for (int heavy = 0; heavy + interval < D && Q > 0; heavy++) {
                int light = heavy + interval;
                if (iteration == 0) {
                    if (heavy != 0 && light != D - 1) continue;
                    if (interval * 2 < D) continue;
                }
                if (items[heavy].size() <= 1) continue;
                bool all = (heavy == 0 && light == D - 1);
                vector<int> bl, br;
                for (int item1 : items[heavy]) {
                    if (Q == 0) return false;
                    if (smallest >= 0 && comparison[item1][smallest] >= 0) continue;
                    
                    vector<int> nl = {item1};
                    vector<int> nr = {};
                    if (!update(bl, br, nl, nr)) continue;
                    if (Q == 0) return false;
                    bl = nl;
                    br = nr;
                    
                    vector<int> vl, vr;
                    for (int item : items[heavy]) {
                        if (item != item1) vl.push_back(item);
                    }
                    vr = items[light];
                    if (query(vl, vr) <= 0) {
                        if (all) smallest = item1;
                        continue;
                    }
                    
                    vr.push_back(item1);
                    items.erase(items.begin() + light);
                    items.erase(items.begin() + heavy);
                    binary_insertion(items, vl, heavy - 1, light - 1);
                    binary_insertion(items, vr, heavy - 1, light);
                    return true;
                }
                for (int item1 : items[heavy]) {
                    for (int item2 : items[light]) {
                        if (Q == 0) return false;
                        query(item1, item2);
                        if (Q == 0) return false;
                        if (comparison[item1][item2] <= 0) continue;
                        
                        vector<int> nl = {item1};
                        vector<int> nr = {item2};
                        if (!update(bl, br, nl, nr)) break;
                        if (Q == 0) return false;
                        bl = nl;
                        br = nr;
                        
                        vector<int> vl, vr;
                        for (int item : items[heavy]) {
                            if (item != item1) vl.push_back(item);
                        }
                        for (int item : items[light]) {
                            if (item != item2) vr.push_back(item);
                        }
                        int q = Q;
                        if (query(vl, vr) <= 0) {
                            if (Q == q) continue;
                            break;
                        }
                        
                        vl.push_back(item2);
                        vr.push_back(item1);
                        items.erase(items.begin() + light);
                        items.erase(items.begin() + heavy);
                        binary_insertion(items, vl, heavy - 1, light - 1);
                        binary_insertion(items, vr, heavy - 1, light);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool improve_swap2(vector<vector<int>>& items) {
    if (full_comparison) return improve_swap12(items);
    int sum[N] = {};
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            sum[i] += comparison[i][j];
        }
    }
    for (int i = 0; i < D; i++) sort(items[i].begin(), items[i].end(), [&](int item1, int item2) {
        return sum[item1] < sum[item2];
    });
    
    for (int interval = D - 1; interval > 0 && Q > 0; interval--) {
        for (int heavy = 0; heavy + interval < D && Q > 0; heavy++) {
            int light = heavy + interval;
            if (items[heavy].size() > 2) {
                vector<pair<int, int>> candidates;
                for (int i = 0; i < items[heavy].size(); i++) {
                    for (int j = i + 1; j < items[heavy].size(); j++) {
                        candidates.emplace_back(i, j);
                    }
                }
                sort(candidates.begin(), candidates.end(), [&](const pair<int, int>& p1, const pair<int, int>& p2) {
                    int x1 = sum[items[heavy][p1.first]];
                    int y1 = sum[items[heavy][p1.second]];
                    int x2 = sum[items[heavy][p2.first]];
                    int y2 = sum[items[heavy][p2.second]];
                    if (x1 + y1 != x2 + y2) return x1 + y1 < x2 + y2;
                    return max(x1, y1) < max(x2, y2);
                });
                for (const pair<int, int>& p : candidates) {
                    if (Q == 0) return false;
                    int item1 = items[heavy][p.first];
                    int item2 = items[heavy][p.second];
                    int item3 = -1;
                    for (int item : items[light]) {
                        int cmp = query({item1, item2}, {item});
                        if (Q == 0) return false;
                        if (cmp > 0) {
                            item3 = item;
                            break;
                        }
                    }
                    if (item3 == -1) continue;
                    
                    vector<int> vl, vr;
                    for (int item : items[heavy]) {
                        if (item != item1 && item != item2) vl.push_back(item);
                    }
                    for (int item : items[light]) {
                        if (item != item3) vr.push_back(item);
                    }
                    if (query(vl, vr) <= 0) continue;
                    
                    vl.push_back(item3);
                    vr.push_back(item1);
                    vr.push_back(item2);
                    items.erase(items.begin() + light);
                    items.erase(items.begin() + heavy);
                    binary_insertion(items, vl, heavy - 1, light - 1);
                    binary_insertion(items, vr, heavy - 1, light);
                    return true;
                }
            }
            if (items[light].size() > 2) {
                vector<pair<int, int>> candidates;
                for (int i = 0; i < items[light].size(); i++) {
                    for (int j = i + 1; j < items[light].size(); j++) {
                        candidates.emplace_back(i, j);
                    }
                }
                sort(candidates.begin(), candidates.end(), [&](const pair<int, int>& p1, const pair<int, int>& p2) {
                    int x1 = sum[items[light][p1.first]];
                    int y1 = sum[items[light][p1.second]];
                    int x2 = sum[items[light][p2.first]];
                    int y2 = sum[items[light][p2.second]];
                    if (x1 + y1 != x2 + y2) return x1 + y1 < x2 + y2;
                    return max(x1, y1) < max(x2, y2);
                });
                for (const pair<int, int>& p : candidates) {
                    if (Q == 0) return false;
                    int item1 = items[light][p.first];
                    int item2 = items[light][p.second];
                    int item3 = -1;
                    for (int item : items[heavy]) {
                        int cmp = query({item1, item2}, {item});
                        if (Q == 0) return false;
                        if (cmp < 0) {
                            item3 = item;
                            break;
                        }
                    }
                    if (item3 == -1) continue;
                    
                    vector<int> vl, vr;
                    for (int item : items[heavy]) {
                        if (item != item3) vl.push_back(item);
                    }
                    for (int item : items[light]) {
                        if (item != item1 && item != item2) vr.push_back(item);
                    }
                    if (query(vl, vr) <= 0) continue;
                    
                    vl.push_back(item1);
                    vl.push_back(item2);
                    vr.push_back(item3);
                    items.erase(items.begin() + light);
                    items.erase(items.begin() + heavy);
                    binary_insertion(items, vl, heavy - 1, light - 1);
                    binary_insertion(items, vr, heavy - 1, light);
                    return true;
                }
            }
        }
    }
    return false;
}

vector<int> strict() {
    vector<int> order;
    for (int i = 0; i < N; i++) order.push_back(i);
    order = merge_insertion_sort(order);
    vector<vector<int>> items(D);
    for (int i = 0; i < D; i++) items[i].push_back(order[i]);
    for (int i = D; i < N; i++) {
        if (Q == 0) {
            for (int j = i; j < N; j++) items[D - 1 - (j - i) % D].push_back(order[j]);
            break;
        } else {
            vector<int> lightest = items[D - 1];
            items.pop_back();
            lightest.push_back(order[i]);
            if (i * 1.5 < N) {
                if (query(items[0], lightest) <= 0) {
                    items.insert(items.begin(), lightest);
                } else {
                    binary_insertion(items, lightest, 0, items.size());
                }
            } else {
                binary_insertion(items, lightest);
            }
        }
    }
    while (Q > 0) {
        if (!improve_swap12(items)) break;
    }
    
    vector<int> d(N);
    for (int i = 0; i < D; i++) {
        for (int item : items[i]) d[item] = i;
    }
    return d;
}

vector<int> rough() {
    vector<int> order;
    for (int i = 0; i < N; i++) order.push_back(i);
    vector<vector<int>> items(D);
    for (int i = 0; i < N; i++) {
        int index = i % (D * 2);
        if (index >= D) index = D * 2 - 1 - index;
        items[index].push_back(order[i]);
    }
    items = merge_insertion_sort(items);
    while (Q > 0) {
        int q = Q;
        if (!improve_swap(items) && Q == q) break;
    }
    while (Q > 0) {
        if (!improve_swap2(items)) break;
    }
    
    vector<int> d(N);
    for (int i = 0; i < D; i++) {
        for (int item : items[i]) d[item] = i;
    }
    return d;
}

int main() {
    input();
    
    init();
    
    vector<int> d;
    if (border[N][D] <= Q) {
        d = strict();
    } else {
        d = rough();
    }
    
    while (Q > 0) {
        Q--;
        puts("1 1 0 1");
    }
    
    output(d);
    
    return 0;
}
