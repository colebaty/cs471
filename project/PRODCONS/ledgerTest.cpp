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

const time_t YEAR_START = 1451606400;
const time_t YEAR_END = 1483228799;

enum month { JAN, FEB, MAR, APR, JUN, JUL, AUG, SEP, OCT, NOV, DEC };

using namespace std;

/* sales date, store ID, register, sale amt */
typedef tuple<time_t, int, int, long double> record;

int main()
{
    cout << "======= generating random ledger entries ============" << endl;
    random_device r;

    int p = 5;

    default_random_engine gen(r());
    uniform_int_distribution<time_t> ddist(YEAR_START, YEAR_END);
    uniform_int_distribution<> storedist(0,p);
    uniform_int_distribution<> regdist(0,6);
    uniform_real_distribution<long double> pricedist(50, 99999); /* [$.50, $999.99] in cents */

    vector<record> entries;

    time_t date;
    double price;
    int storeID, regID;

    for (size_t i = 0; i < 5; i++)
    {
        date = ddist(gen);
        assert(YEAR_START <= date && date <= YEAR_END);

        storeID = storedist(gen);
        regID = regdist(gen);
        price = pricedist(gen);

        entries.push_back( { date, storeID, regID, price} );
    }

    cout << "ledger contents: " << endl;
    cout << left 
         << setw(20) << "Date" 
         << setw(20) << "Store" 
         << setw(20) << "Register" 
         << setw(20) << "Sale amt." << endl;

    for (auto entry : entries) 
    {
        cout.imbue(locale("C"));
        cout << left 
             << put_time(localtime(&get<0>(entry)), "%x") << setw(12) << ""
             << setw(20) << get<1>(entry)
             << setw(20) << get<2>(entry)
             << setw(20) << showbase << put_money(get<3>(entry)) 
             << setw(20) << get<3>(entry) << endl;
    }

    return 0;
}
