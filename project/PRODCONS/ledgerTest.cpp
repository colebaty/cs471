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
#include <map>
#include <sstream>
#include <string>

const time_t YEAR_START = 1451606400;
const time_t YEAR_END = 1483228799;

enum month { JAN, FEB, MAR, APR, JUN, JUL, AUG, SEP, OCT, NOV, DEC };

using namespace std;

/* sales date, store ID, register, sale amt */
typedef tuple<time_t, int, int, long double> record;

binary_semaphore account{0};
vector<record> ledger;
int numRead = 0;

/**
 * @brief print the entire ledger
 * 
 */
void print();

/**
 * @brief print an individual record
 * 
 * @param r 
 */
void print(const record & r);

/**
 * @brief print the maps used to store montly, store-wide, etc. data
 * 
 * @param swts 
 * @param mwts 
 * @param all 
 */
void print(const map<int, long double>& swts, const map<string, long double>& mwts, const long double& all);

int main(int argc, char **argv)
{
    int p;
    if (argc == 2)
    {
        p = atoi(argv[1]);
    }
    else
    {
        p = 10;
    }
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
    }

    print();

    cout << "====== processing ledger ====== " << endl;

    /* store ID : sale amt */
    map<int, long double> store_wide_total_sales;
    /* month : sale amt */
    map<string, long double> month_wise_total_sales;
    long double all_sales = 0;

    struct tm * t_m;
    stringstream temp;

    for (auto entry : ledger) {
        temp.str("");
        store_wide_total_sales[get<1>(entry)] += get<3>(entry);

        t_m = localtime(&get<0>(entry));
        temp << put_time(t_m, "%b") << flush;
        month_wise_total_sales[temp.str()] += get<3>(entry);

        all_sales += get<3>(entry);
        numRead++;
    }
    cout << "num records read: " << numRead << endl;
    cout << "==== stats ====" << endl;
    print(store_wide_total_sales, month_wise_total_sales, all_sales);

    return 0;
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

void print(const record & r)
{
    cout.imbue(locale(""));
    cout << left 
            << put_time(localtime(&get<0>(r)), "%x") << setw(12) << ""
            << setw(20) << get<1>(r)
            << setw(20) << get<2>(r)
            << setw(20) << showbase << put_money(get<3>(r)) << endl;
        //  << setw(20) << get<3>(r) << endl;
}

void print(const map<int, long double>& swts, const map<string, long double>& mwts, const long double& all)
{
    cout.imbue(locale(""));
    cout << "store-wide total sales" << endl;
    for (const auto& [key, value] : swts)
    {
        cout << "[" << key << "] = " << put_money(value) << "; ";
    }

    cout << endl;

    cout << "month-wise total sales" << endl;
    for (const auto& [key, value] : mwts)
    {
        cout << "[" << key << "] = " << put_money(value) << "; ";
    }

    cout << endl;

    cout << "total sales: " << put_money(all) << endl;
    cout << endl;
}