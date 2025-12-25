#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>

using namespace std;

// Function to convert a number to the corresponding variable name
string numberToVariableName(long long n) {
    string result;
    while (n > 0) {
        n--;  // Decrease by 1 to make it 0-indexed
        result += 'a' + (n % 26);
        n /= 26;
    }
    reverse(result.begin(), result.end());
    return result;
}

// Function to get the N-th variable name, skipping reserved words
string getNthVariableName(long long n, const unordered_set<string>& reservedWords) {
    long long count = 0;
    long long current = 1;
    
    while (true) {
        string varName = numberToVariableName(current);
        if (reservedWords.find(varName) == reservedWords.end()) {
            count++;
            if (count == n) {
                return varName;
            }
        }
        current++;
    }
}

int main() {
    int K, Q;
    cin >> K;
    unordered_set<string> reservedWords;
    for (int i = 0; i < K; ++i) {
        string word;
        cin >> word;
        reservedWords.insert(word);
    }
    cin >> Q;
    vector<long long> queries(Q);
    for (int i = 0; i < Q; ++i) {
        cin >> queries[i];
    }

    for (int i = 0; i < Q; ++i) {
        cout << getNthVariableName(queries[i], reservedWords) << endl;
    }

    return 0;
}
