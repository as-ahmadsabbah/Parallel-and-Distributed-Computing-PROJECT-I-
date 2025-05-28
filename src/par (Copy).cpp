#include <iostream>
#include <vector>
#include <cmath>
#include <climits>
#include <pthread.h>
#include <chrono>
using namespace std;
using namespace chrono;

struct Query {
    int L, R;
    int index;
    pair<int, int> result;
};

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

void* processQuery(void* arg) {
    Query* q = (Query*)arg;
    vector<int> primes;
    for (int i = q->L + 1; i < q->R; ++i) {
        if (isPrime(i)) primes.push_back(i);
    }

    if (primes.size() < 2) {
        q->result = {-1, -1};
        return nullptr;
    }

    int minGap = INT_MAX;
    pair<int, int> res = {-1, -1};

    for (int i = 1; i < primes.size(); ++i) {
        int gap = primes[i] - primes[i - 1];
        if (gap < minGap) {
            minGap = gap;
            res = {primes[i - 1], primes[i]};
        }
    }

    q->result = res;
    return nullptr;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int Q;
    cin >> Q;
    vector<Query> queries(Q);
    vector<pthread_t> threads(Q);

    for (int i = 0; i < Q; ++i) {
        cin >> queries[i].L >> queries[i].R;
        queries[i].index = i;
    }

    auto start = high_resolution_clock::now();

    for (int i = 0; i < Q; ++i) {
        pthread_create(&threads[i], nullptr, processQuery, &queries[i]);
    }

    for (int i = 0; i < Q; ++i) {
        pthread_join(threads[i], nullptr);
    }

    auto end = high_resolution_clock::now();

    for (int i = 0; i < Q; ++i) {
        cout << queries[i].result.first << " " << queries[i].result.second << "\n";
    }
    auto duration = duration_cast<milliseconds>(end - start);
    cerr << "Execution Time (Pthreads): " << duration.count() << " ms\n";

    return 0;
}
