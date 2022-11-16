#include <iostream>
#include <iomanip>
#include <locale>
#include <ctime>
#include <random>
#include <cassert>
#include <algorithm>
#include <stdio.h>
#include <time.h>
#include <utility>
#include <tuple>
#include <thread>
#include <semaphore>

/* 
    these values represent the beinning and end of 
    the year 2016 in epoch time, respectively
*/
const time_t YEAR_START = 1451606400;
const time_t YEAR_END = 1483228799;

enum month { JAN, FEB, MAR, APR, JUN, JUL, AUG, SEP, OCT, NOV, DEC };

using namespace std;

/* sales date, store ID, register, sale amt */
typedef tuple<time_t, int, int, long double> record;

/* shared variables */
int numProduced = 0, numRead = 0;
binary_semaphore ledger_sem{0};/* starts in "acquired" state */
vector<record> ledger;

/**
 * @brief producer thread
 * 
 * @param id 
 * @param gen 
 * @param ddist 
 * @param storedist 
 * @param regdist 
 * @param pricedist 
 * @param sleepdist 
 */
void producer(
    int id, 
    default_random_engine& gen, 
    uniform_int_distribution<time_t>& ddist,
    uniform_int_distribution<>& storedist, 
    uniform_int_distribution<>& regdist,
    uniform_real_distribution<long double>& pricedist,
    uniform_int_distribution<>& sleepdist);

void print();

int main(int argc, char **argv)
{
    auto start = chrono::steady_clock::now();

    int p, c;
    int b = 1000; /* max buffer size */
    if (argc == 3)
    {
        p = atoi(argv[1]);
        c = atoi(argv[2]);
    }
    else
    {
        cout << "usage:\ta.out <p> <c>" << endl
             << "\t<p>\tnumber of producers" << endl
             << "\t<c>\tnumber of consumers" << endl;
        
        return 1;
    }

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
            ref(sleepdist));
    }

    ledger_sem.release();

    for (size_t i = 0; i < p; i++)
    {
        producers[i].join();
    }
    
    print();

    auto end = chrono::steady_clock::now();
    chrono::duration<double> elapsed_time = end - start;
    cout << "total elapsed time: " << elapsed_time.count() << "s\n";
    return 0;
}

void producer(int id, 
    default_random_engine& gen, 
    uniform_int_distribution<time_t>& ddist,
    uniform_int_distribution<>& storedist,
    uniform_int_distribution<>& regdist,
    uniform_real_distribution<long double>& pricedist,
    uniform_int_distribution<>& sleepdist)
{
    time_t date;
    long double price;
    int storeID, regID;

    while(numProduced < 1000)
    {
        date = ddist(gen);
        assert(YEAR_START <= date && date <= YEAR_END);

        storeID = storedist(gen);
        regID = regdist(gen);
        price = pricedist(gen);

        /* entry section */
        ledger_sem.acquire();

        /* critical section */
        ledger.push_back( { date, storeID, regID, price } );
        numProduced++;

        ledger_sem.release();

        /* remainder section */
        this_thread::sleep_for(chrono::milliseconds{sleepdist(gen)});
    }
}

/**
 * @brief print the ledger
 * 
 */
void print()
{
    cout << "ledger contents: " << endl;
    cout << left 
         << setw(20) << "Date" 
         << setw(20) << "Store" 
         << setw(20) << "Register" 
         << setw(20) << "Sale amt." << endl;

    cout.imbue(locale(""));
    for (auto entry : ledger) 
    {
        cout << left 
             << put_time(localtime(&get<0>(entry)), "%x") << setw(12) << ""
             << setw(20) << get<1>(entry)
             << setw(20) << get<2>(entry)
             << setw(20) << showbase << put_money(get<3>(entry)) << endl;
            //  << setw(20) << get<3>(entry) << endl;
    }
    cout << "ledger size: " << ledger.size() << endl;
}