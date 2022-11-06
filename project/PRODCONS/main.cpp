#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <semaphore>
#include <tuple>

using namespace std;

/* sales date, store ID, register, sale amt */
typedef tuple<time_t, int, int, double> record;

void producer() {
    auto start = chrono::steady_clock::now();

    cout << "i'm producing things; ";
    this_thread::sleep_for(20ms);
    
    auto end = chrono::steady_clock::now();
    chrono::duration<double> elapsed_time = end - start;
    cout << "this took " << elapsed_time.count() << "s" << endl;
}

void consumer() {
    auto start = chrono::steady_clock::now();

    cout << "i'm producing things; ";
    this_thread::sleep_for(25ms);

    auto end = chrono::steady_clock::now();
    chrono::duration<double> elapsed_time = end - start;
    cout << "this took " << elapsed_time.count() << "s" << endl;
}

int main()
{
    auto start = chrono::steady_clock::now();
    thread p(producer);
    thread c(consumer);

    p.join();
    c.join();

    auto end = chrono::steady_clock::now();

    chrono::duration<double> elapsed_time = end - start;
    cout << "total time was " << elapsed_time.count() << "s" << endl;

    return 0;
}