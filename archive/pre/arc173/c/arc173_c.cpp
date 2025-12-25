#include <iostream>
#include <vector>
#include <set>

using namespace std;

vector<int> find_min_length(int N, const vector<int>& P) {
    vector<int> result(N, -1);

    for (int i = 0; i < N; ++i) {
        multiset<int> left, right;
        int l = i, r = i;
        left.insert(P[i]);

        while (l >= 0 && r < N) {
            // Balance the multisets
            while (left.size() > right.size() + 1) {
                right.insert(*left.rbegin());
                left.erase(--left.end());
            }
            while (right.size() > left.size()) {
                left.insert(*right.begin());
                right.erase(right.begin());
            }

            // Check if the current length is odd
            if ((r - l + 1) % 2 == 1) {
                int median = *left.rbegin();
                if (median != P[i]) {
                    result[i] = r - l + 1;
                    break;
                }
            }

            // Expand the window to the left and right
            if (l > 0) {
                --l;
                left.insert(P[l]);
            }
            if (r + 1 < N) {
                ++r;
                right.insert(P[r]);
            }
        }
    }

    return result;
}

int main() {
    int N;
    cin >> N;
    vector<int> P(N);
    for (int i = 0; i < N; ++i) {
        cin >> P[i];
    }

    vector<int> result = find_min_length(N, P);
    for (int i = 0; i < N; ++i) {
        cout << result[i] << " ";
    }
    cout << endl;

    return 0;
}
