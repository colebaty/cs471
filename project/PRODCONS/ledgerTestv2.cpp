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

binary_semaphore account{0};
vector<record> ledger;
record *buffer;
int numProduced = 0, numRead = 0;

int main(int argc, char **argv)
{
    int p, b;
    if (argc == 3) {
        p = atoi(argv[1]);
        b = atoi(argv[2]);
    }
    else {
        p = 10;
        b = 3;
    }

    buffer = new record[b];

    cout << "======= generating random entries in ledger ============" << endl;
    random_device r;

    default_random_engine gen(r());
    uniform_int_distribution<time_t> ddist(YEAR_START, YEAR_END);
    uniform_int_distribution<> storedist(1,p);
    uniform_int_distribution<> regdist(1,6);
    uniform_real_distribution<long double> pricedist(50, 99999); /* [$.50, $999.99] in cents */

    time_t date;
    double price;
    int storeID, regID;

    for (size_t i = 0; i < p; i++)
    {
        date = ddist(gen);
        assert(YEAR_START <= date && date <= YEAR_END);

        storeID = storedist(gen);
        regID = regdist(gen);
        price = pricedist(gen);

        ledger.push_back( { date, storeID, regID, price} );
        buffer[i % b] = record({ date, storeID, regID, price });
    }

    return 0;
}