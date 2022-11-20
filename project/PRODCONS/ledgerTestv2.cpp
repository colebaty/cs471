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

#define MAX_RECORDS 5

const time_t YEAR_START = 1451606400;
const time_t YEAR_END = 1483228799;

enum month { JAN, FEB, MAR, APR, JUN, JUL, AUG, SEP, OCT, NOV, DEC };

using namespace std;

/* sales date, store ID, register, sale amt */
typedef tuple<time_t, int, int, long double> record;

/* shared variables */
sem_t buff_full, buff_empty, buff_mutex;

vector<record> master_ledger;
record *buffer;
int p, c, b, index = 0;
int numProduced = 0, numRead = 0;

default_random_engine gen;

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

    cout << "========= info =================" << endl;
    cout << "b: " << b << endl;
    cout << "================================" << endl;

    buffer = new record[b];

    random_device r;
    default_random_engine gen(r());

    sem_init(&buff_empty, 0, b);
    sem_init(&buff_full, 0, 0);
    sem_init(&buff_mutex, 0, 1);

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t prod;
    int rc;
    rc = pthread_create(&prod, &attr, producer, NULL);
    if (rc) {
        fprintf(stderr, "ERROR: return code from pthread_create is %d\n", rc);
        exit(1);
    }

    pthread_t cons;
    rc = pthread_create(&cons, &attr, consumer, NULL);
    if (rc) {
        fprintf(stderr, "ERROR: return code from pthread_create is %d\n", rc);
        exit(1);
    }

    // sem_post(&buff_empty);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    pthread_exit(NULL);

    // return 0;
}

void *producer(void * arg)
{
    uniform_int_distribution<> sleepdist(5,40);

    int sleepdur;
    while(numProduced < MAX_RECORDS)
    {
        /* entry section */
        sem_wait(&buff_empty);
        cout << "producer acquired buff_empty" << endl;
        // pthread_mutex_lock(&buff_mutex);
        sem_wait(&buff_mutex);
        cout << "producer acquired mutex lock" << endl;

        // /* critical section */
        // buffer[index % b] = newRecord(gen);
        // cout << "producer: ";
        // print(buffer[index % b]);
        // index++;
        cout << "producer: put new thing in buffer " << numProduced % b << endl;
        cout << "producer: " << ++numProduced << " new things made so far " << endl;


        sleepdur = sleepdist(gen) * 1000;
        cout << "producer sleeping for " << sleepdur / 1000 << "ms" << endl;
        usleep(sleepdur);

        // pthread_mutex_unlock(&buff_mutex);
        sem_post(&buff_mutex);
        cout << "producer released mutex lock" << endl;
        sem_post(&buff_full);
        cout << "producer released buff_full" << endl;

        /* remainder section */
        // this_thread::sleep_for(chrono::milliseconds{sleepdist(gen)});
    }

    pthread_exit(NULL);
}

void *consumer(void * arg) {
    uniform_int_distribution<> sleepdist(5,40);

    vector<record> thread_ledger;
    int sleepdur;

    while (numRead < MAX_RECORDS) 
    {
        sem_wait(&buff_full);
        cout << "consumer acquired buff_full" << endl;
        sem_wait(&buff_mutex);
        cout << "consumer acquired buff_mutex" << endl;
        // pthread_mutex_lock(&buff_mutex);

        // thread_ledger.push_back(buffer[numRead % b]);
        // cout << "consumer: ";
        // print(buffer[index % b]);
        // buffer[index % b] = { 0, 0, 0, 0 };
        // index--;
        cout << "consumer: read thing from buffer " << numRead % b << endl;
        cout << "consumer: read " << ++numRead <<  " things so far" << endl;

        sleepdur = sleepdist(gen) * 1000;
        cout << "consumer sleeping for " << sleepdur / 1000 << "ms" << endl;
        usleep(sleepdur);


        // pthread_mutex_unlock(&buff_mutex);
        sem_post(&buff_mutex);
        cout << "consumer released buff_mutex" << endl;
        sem_post(&buff_empty);
        cout << "consumer released buff_full" << endl;
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