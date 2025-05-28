#include <iostream>
#include <vector>
#include <cmath>
#include <climits>
#include <pthread.h>
#include <chrono>
using namespace std;
using namespace chrono;

const int NUM_THREADS = 7;

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

vector<Query>* queries_ptr;

struct ThreadArgs {
    int start_idx;
    int end_idx;
};

void* processRange(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    vector<Query>& queries = *queries_ptr;

    for (int i = args->start_idx; i < args->end_idx; ++i) {
        Query& q = queries[i];
        vector<int> primes;
        for (int j = q.L + 1; j < q.R; ++j) {
            if (isPrime(j)) primes.push_back(j);
        }

        if (primes.size() < 2) {
            q.result = {-1, -1};
            continue;
        }

        int minGap = INT_MAX;
        pair<int, int> res = {-1, -1};
        for (int k = 1; k < primes.size(); ++k) {
            int gap = primes[k] - primes[k - 1];
            if (gap < minGap) {
                minGap = gap;
                res = {primes[k - 1], primes[k]};
            }
        }
        q.result = res;
    }
    return nullptr;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int Q;
    cin >> Q;
    vector<Query> queries(Q);
    queries_ptr = &queries;

    for (int i = 0; i < Q; ++i) {
        cin >> queries[i].L >> queries[i].R;
        queries[i].index = i;
    }

    pthread_t threads[NUM_THREADS];
    ThreadArgs thread_args[NUM_THREADS];

    int chunk_size = (Q + NUM_THREADS - 1) / NUM_THREADS;

    auto start = high_resolution_clock::now();

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_args[i].start_idx = i * chunk_size;
        thread_args[i].end_idx = min((i + 1) * chunk_size, Q);
        pthread_create(&threads[i], nullptr, processRange, &thread_args[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], nullptr);
    }

    auto end = high_resolution_clock::now();

    for (int i = 0; i < Q; ++i) {
        cout << queries[i].result.first << " " << queries[i].result.second << "\n";
    }

    auto duration = duration_cast<milliseconds>(end - start);
    cerr << "Execution Time (Pthreads - 4 threads): " << duration.count() << " ms\n";

    return 0;
}
