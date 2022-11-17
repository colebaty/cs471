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
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_RECORDS 10 

const time_t YEAR_START = 1451606400;
const time_t YEAR_END = 1483228799;

enum month { JAN, FEB, MAR, APR, JUN, JUL, AUG, SEP, OCT, NOV, DEC };

using namespace std;

/* sales date, store ID, register, sale amt */
typedef tuple<time_t, int, int, long double> record;

/* shared variables */
sem_t buff_full, buff_empty;
pthread_mutex_t buff_mutex;

vector<record> master_ledger;
record *buffer;
int p, c, b, index = 0;
int numProduced = 0, numRead = 0;

/**
 * @brief producer thread
 * 
 * @param id 
 * @param gen 
 */
void *producer(void * arg);
void *consumer(void * arg);
record newRecord(default_random_engine& gen);
void print(record r);

int main(int argc, char **argv)
{
    if (argc == 4) {
        p = atoi(argv[1]);
        c = atoi(argv[2]);
        b = atoi(argv[3]);
    }
    else {
        p = 5;
        c = 2;
        b = 3;
    }

    // cout << "======= generating random entries in ledger ============" << endl;

    buffer = new record[b];

    random_device r;
    default_random_engine gen(r());

    sem_init(&buff_empty, 0, b);
    sem_init(&buff_full, 0, 0);
    pthread_mutex_init(&buff_mutex, NULL);

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t producers[p];
    int rc;
    for (size_t i = 0; i < p; i++)
    {
        rc = pthread_create(&producers[i], &attr, producer, (void *) &gen);
        if (rc) {
            fprintf(stderr, "ERROR: return code from pthread_create is %d\n", rc);
            exit(1);
        }
    }

    pthread_t consumers[c];
    for (size_t i = 0; i < c; i++)
    {
        rc = pthread_create(&consumers[i], &attr, consumer, NULL);
        if (rc) {
            fprintf(stderr, "ERROR: return code from pthread_create is %d\n", rc);
            exit(1);
        }
    }

    // sem_post(&buff_empty);

    for (size_t i = 0; i < p; i++) pthread_join(producers[i], NULL);
    for (size_t i = 0; i < c; i++) pthread_join(consumers[i], NULL);

    pthread_exit(NULL);

    // return 0;
}

void *producer(void * arg)
{
    default_random_engine gen = *(default_random_engine *) arg;
    uniform_int_distribution<> sleepdist(5,40);

    while(numProduced < MAX_RECORDS)
    {
        /* entry section */
        sem_wait(&buff_empty);
        pthread_mutex_lock(&buff_mutex);

        /* critical section */
        buffer[index % b] = newRecord(gen);
        cout << "producer: ";
        print(buffer[index % b]);
        index++;
        numProduced++;

        pthread_mutex_unlock(&buff_mutex);
        sem_post(&buff_full);

        /* remainder section */
        // this_thread::sleep_for(chrono::milliseconds{sleepdist(gen)});
        usleep(sleepdist(gen) * 1000);
    }

    pthread_exit(NULL);
}

void *consumer(void * arg) {
    vector<record> thread_ledger;
    while (numRead < MAX_RECORDS) 
    {
        sem_wait(&buff_full);
        pthread_mutex_lock(&buff_mutex);

        thread_ledger.push_back(buffer[numRead % b]);
        cout << "consumer: ";
        print(buffer[index % b]);
        buffer[index % b] = { 0, 0, 0, 0 };
        index--;
        numRead++;

        pthread_mutex_unlock(&buff_mutex);
        sem_post(&buff_empty);
    }

    pthread_exit(NULL);
}

record newRecord(default_random_engine& gen)
{
        uniform_int_distribution<time_t> ddist(YEAR_START, YEAR_END);
        uniform_int_distribution<> storedist(1,p);
        uniform_int_distribution<> regdist(1,6);
        uniform_real_distribution<long double> pricedist(50, 99999); /* [$.50, $999.99] in cents */

        time_t date;
        int storeID, regID;
        long double price;

        date = ddist(gen);
        assert(YEAR_START <= date && date <= YEAR_END);

        storeID = storedist(gen);
        regID = regdist(gen);
        price = pricedist(gen);

       return { date, storeID, regID, price };
}

void print(record r) {
    cout.imbue((locale("")));
    cout << left
         << put_time(localtime(&get<0>(r)), "%x") << setw(12) << ""
         << setw(20) << get<1>(r)
         << setw(20) << get<2>(r)
         << setw(20) << showbase << put_money(get<3>(r)) << endl;

}