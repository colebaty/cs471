#include <iostream>
#include <thread>

using namespace std;

void f1(int & n) {
    cout << "n * n = " << n * n << endl;
}

void f2(int & n) {
    cout << "n + n = " << n + n << endl;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "usage: a.out <n>" << endl;
        exit(1);
    }
    int n = atoi(argv[1]);

    thread t1(f1, ref(n));
    thread t2(f2, ref(n));

    t1.join();
    t2.join();
    return 0;
}