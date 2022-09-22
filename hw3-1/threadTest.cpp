#include <iostream>
#include <thread>
#include <ctime>
#include <random>

using namespace std;

void f1(int & n) {
    cout << "n * n = " << n * n << endl;
}

void f2(int & n) {
    cout << "n + n = " << n + n << endl;
}

void f3(int& n, default_random_engine& gen, binomial_distribution<time_t>& ndist) {
    for (int i = 0; i < 5; i++)
    {
        cout << "sleep for " << ndist(gen) << "ms" << endl;
    }
    
}

int main(int argc, char **argv) {
    default_random_engine gen(time(NULL));
    binomial_distribution<time_t> ndist(1000);

    if (argc < 2) {
        cout << "usage: a.out <n>" << endl;
        exit(1);
    }
    int n = atoi(argv[1]);

    thread t1(f1, ref(n));
    thread t2(f2, ref(n));

    thread t3(f3, ref(n), ref(gen), ref(ndist));

    t1.join();
    t2.join();
    t3.join();

    return 0;
}