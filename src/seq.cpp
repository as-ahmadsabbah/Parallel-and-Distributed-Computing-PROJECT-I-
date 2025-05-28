#include <iostream>
#include <vector>
#include <cmath>
#include <climits>
#include <chrono>
using namespace std;
using namespace chrono;

bool isPrime(int n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    int sqrtN = sqrt(n);
    for (int i = 3; i <= sqrtN; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

pair<int, int> closestPrimes(int L, int R) {
    vector<int> primes;
    for (int i = L + 1; i < R; ++i) {
        if (isPrime(i)) {
            primes.push_back(i);
        }
    }

    if (primes.size() < 2) return {-1, -1};

    int minGap = INT_MAX;
    pair<int, int> result = {-1, -1};

    for (int i = 1; i < primes.size(); ++i) {
        int gap = primes[i] - primes[i - 1];
        if (gap < minGap) {
            minGap = gap;
            result = {primes[i - 1], primes[i]};
        }
    }

    return result;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int Q;
    cin >> Q;

    auto start = high_resolution_clock::now();

    while (Q--) {
        int L, R;
        cin >> L >> R;
        pair<int, int> ans = closestPrimes(L, R);
        cout << ans.first << " " << ans.second << "\n";
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    cerr << "Execution Time: " << duration.count() << " ms\n";

    return 0;
}
