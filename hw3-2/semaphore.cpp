#include <iostream>
#include <thread>
#include <queue>
#include <semaphore>
#include <random>
#include <ctime>

using namespace std;

binary_semaphore account{0};

int balance = 0;
int withdrawal = 0;
int deposit = 0;

void Mary(default_random_engine & gen, uniform_int_distribution<> & idist) {
    int count = 0;
    do
    {
        /* deposit */
        account.acquire();

        deposit = idist(gen);
        balance += deposit;
        printf("mary deposited $%d; new balance is $%d\n", deposit, balance);
        deposit = 0;
        idist.reset();
        account.release();

        this_thread::sleep_for(20ms);
        idist(gen);

        /* withdrawal */
        account.acquire();

        withdrawal = idist(gen);
        balance -= withdrawal;
        printf("mary withdrew $%d; new balance is $%d\n", withdrawal, balance);
        withdrawal = 0;
        account.release();

        count++;

    } while (count < 1);
    
}

void Joe(default_random_engine & gen, uniform_int_distribution<> & idist) {
    int count = 0;
    do
    {
        /* deposit */
        account.acquire();

        deposit = idist(gen);
        balance += deposit;
        printf("joe deposited $%d; new balance is $%d\n", deposit, balance);
        deposit = 0;
        idist.reset();
        account.release();

        this_thread::sleep_for(20ms);
        
        /* withdrawal */
        account.acquire();
        idist(gen);

        withdrawal = idist(gen);
        balance -= withdrawal;
        printf("joe withdrew $%d; new balance is $%d\n", withdrawal, balance);
        withdrawal = 0;
        account.release();

        count ++;
        
    } while (count < 1);
}

int main()
{
    default_random_engine  gen(time(NULL));
    uniform_int_distribution<>  idist(200, 800);

    // idist(gen);// discard first call - always returns same result

    thread mary(Mary, ref(gen), ref(idist));
    thread joe(Joe, ref(gen), ref(idist));

    account.release();

    mary.join();
    joe.join();
    return 0;
}