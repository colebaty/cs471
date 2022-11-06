#include <iostream>
#include <chrono>
#include <ctime>
#include <random>
#include <cassert>
#include <algorithm>
#include <iomanip>

#define YEAR_START 1451606400
#define YEAR_END 1483228799

using namespace std;

int main()
{
    cout << "2016 epoch start " << YEAR_START << endl;
    cout << "2016 epoch end " << YEAR_END << endl;

    cout << "===================" << endl;

    random_device r;

    default_random_engine dategen(r());
    uniform_int_distribution<time_t> ddist(YEAR_START, YEAR_END);

    time_t date;
    for (size_t i = 0; i < 5; i++)
    {
        date = ddist(dategen);
        assert(YEAR_START <= date && date <= YEAR_END);

        cout << "random date " << i << ": " 
             << put_time(localtime(&date), "%x") << flush << endl;
    }
    

    return 0;
}