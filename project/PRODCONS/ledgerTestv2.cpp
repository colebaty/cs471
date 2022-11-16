#include <iostream>
#include <iomanip>
#include <locale>
#include <random>
#include <cassert>
#include <algorithm>
#include <stdio.h>
#include <time.h>
#include <utility>
#include <tuple>
#include <thread>
#include <semaphore>
#include <map>
#include <sstream>
#include <string>
#include <array>

const time_t YEAR_START = 1451606400;
const time_t YEAR_END = 1483228799;

enum month { JAN, FEB, MAR, APR, JUN, JUL, AUG, SEP, OCT, NOV, DEC };

using namespace std;

/* sales date, store ID, register, sale amt */
typedef tuple<time_t, int, int, long double> record;

vector<record> master_ledger;
record *buffer;
int p, c, b;
int numProduced = 0, numRead = 0;

void producer(
    int id, 
    default_random_engine& gen, 
    uniform_int_distribution<time_t>& ddist,
    uniform_int_distribution<>& storedist, 
    uniform_int_distribution<>& regdist,
    uniform_real_distribution<long double>& pricedist,
    uniform_int_distribution<>& sleepdist,
    binary_semaphore &mutex);

int main(int argc, char **argv)
{
    if (argc == 3) {
        p = atoi(argv[1]);
        b = atoi(argv[2]);
    }
    else {
        p = 10;
        b = 3;
    }

    cout << "======= generating random entries in ledger ============" << endl;
    binary_semaphore mutex{0};/* starts in "acquired" state */

    buffer = new record[b];

    random_device r;
    default_random_engine gen(r());
    uniform_int_distribution<time_t> ddist(YEAR_START, YEAR_END);
    uniform_int_distribution<> storedist(1,p);
    uniform_int_distribution<> regdist(1,6);
    uniform_real_distribution<long double> pricedist(50, 99999); /* [$.50, $999.99] in cents */
    uniform_int_distribution<> sleepdist(5,40);

    thread producers[p];
    for (size_t i = 0; i < p; i++)
    {
        producers[i] = thread(
            producer, 
            i, 
            ref(gen), 
            ref(ddist), 
            ref(storedist), 
            ref(regdist), 
            ref(pricedist),
            ref(sleepdist),
            ref(mutex));
    }

    mutex.release();

    for (size_t i = 0; i < p; i++)
    {
        producers[i].join();
    }

    return 0;
}

void producer(int id, 
    default_random_engine& gen, 
    uniform_int_distribution<time_t>& ddist,
    uniform_int_distribution<>& storedist,
    uniform_int_distribution<>& regdist,
    uniform_real_distribution<long double>& pricedist,
    uniform_int_distribution<>& sleepdist,
    binary_semaphore& mutex)
{
    time_t date;
    int storeID, regID;
    long double price;

    while(numProduced < 1000)
    {
        date = ddist(gen);
        assert(YEAR_START <= date && date <= YEAR_END);

        storeID = storedist(gen);
        regID = regdist(gen);
        price = pricedist(gen);

        /* entry section */
        mutex.acquire();

        /* critical section */
        buffer[numProduced % b] = { date, storeID, regID, price };
        numProduced++;

        mutex.release();

        /* remainder section */
        this_thread::sleep_for(chrono::milliseconds{sleepdist(gen)});
    }
}
