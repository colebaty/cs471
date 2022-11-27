#include <iostream>
#include <chrono>
#include <ctime>
#include <random>
#include <cassert>
#include <algorithm>
#include <iomanip>
#include <stdio.h>
#include <time.h>

const time_t YEAR_START = 1451606400;
const time_t YEAR_END = 1483228799;

enum month { JAN, FEB, MAR, APR, JUN, JUL, AUG, SEP, OCT, NOV, DEC };

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
    char * date_s;
    struct tm * t_m;
    for (size_t i = 0; i < 5; i++)
    {
        date = ddist(dategen);
        t_m = localtime(&date);
        assert(YEAR_START <= date && date <= YEAR_END);

        // strftime(date_s, 100, "%x", localtime(&date));

        /* t_m->tm_mon is "months since january" aka zero-indexed */
        cout << "random date " << i << ": " 
             << put_time(localtime(&date), "%x") 
             << " month: " << t_m->tm_mon + 1
             << flush << endl;
    }

    return 0;
}