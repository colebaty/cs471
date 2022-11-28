// #define DEBUG

#include <iostream>
#include <iomanip>
#include <locale>
#include <random>
#include <cassert>
#include <algorithm>
#include <utility>
#include <tuple>
#include <chrono>
#include <map>

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

// #define MAX_RECORDS 10
// #define MAX_RECORDS 100
#define MAX_RECORDS 1000

const time_t YEAR_START = 1451606400;
const time_t YEAR_END = 1483228799;

using namespace std;

/* sales date, store ID, register, sale amt */
typedef tuple<time_t, int, int, long double> record;
record EMPTY = { 0, 0, 0, 0 };
record *rptr;

/* shared variables */
sem_t buff_full, buff_empty, buff_mutex;
sem_t check_done, proceed, compute;
pthread_mutex_t mutex;
bool done = false;

record *buffer;
int p, c, b;
int numProduced = 0, numRead = 0;

vector<record> *master_ledger;
map<int, long double> *aggregate_store_wide_sales;
map<int, long double> *aggregate_monthly_sales;
long double *aggregate_sales;

default_random_engine * gen;
uniform_int_distribution<time_t> ddist;
uniform_int_distribution<> storedist;
uniform_int_distribution<> regdist;
uniform_real_distribution<long double> pricedist;
uniform_int_distribution<> sleepdist;

#ifdef DEBUG
int *prodsemfullval, *prodsememptyval;
int *conssemfullval, *conssememptyval;
#endif

/* thread control */
void *producer(void * arg);
void *consumer(void * arg);
void *allread(void *arg);

/* helper functions */
void computestats(vector<record> &ledger);
void print(record r);
void print(const map<int, long double>& swts, const map<int, long double>& mwts, const long double& all);

int main(int argc, char **argv) {
    auto start = chrono::steady_clock::now();

    if (argc == 4) {
        p = atoi(argv[1]);
        c = atoi(argv[2]);
        b = atoi(argv[3]);
    }
    else {
        cout << "usage: prodcons <p> <c> <b>" << endl;
        return 1;
    }

    buffer = new record[b];

    #ifdef DEBUG
    prodsemfullval = new int[b];
    prodsememptyval = new int[b];
    conssemfullval = new int[b];
    conssememptyval = new int[b];
    #endif
    
    for (size_t i = 0; i < b; i++) buffer[i] = { 0, 0, 0, 0 };
    
    cout << "========= info =================" << endl;
    printf("p: %d | c: %d | b: %d\n", p, c, b);
    #ifdef DEBUG
    cout << "buffer contents: " << endl
         << "======================================" << endl;
        for (int i = 0; i < b; i++) {
            print(buffer[i]);
        }
    cout << "======================================" << endl;
    #endif
    cout << "================================" << endl;

    master_ledger = new vector<record>;
    aggregate_store_wide_sales = new map<int, long double>;
    aggregate_monthly_sales = new map<int, long double>;
    aggregate_sales = new long double;

    /* random generator initialization */
    random_device r;
    gen = new default_random_engine(r());
    ddist = uniform_int_distribution<time_t>(YEAR_START, YEAR_END);
    storedist = uniform_int_distribution<>(1,p);
    regdist = uniform_int_distribution<>(1,6);
    pricedist = uniform_real_distribution<long double>(50, 99999); /* [$.50, $999.99] in cents */
    sleepdist = uniform_int_distribution<>(5,40);

    /* semaphore/mutex initialization */
    sem_init(&buff_empty, 0, b);
    sem_init(&buff_full, 0, 0);
    sem_init(&check_done, 0, 0);
    sem_init(&proceed, 0, 1);
    sem_init(&compute, 0, 1);
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_lock(&mutex); /* hold your horses, everybody */

    /* thread initialization, spawning */
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t producers[p];
    int rc;
    for (int i = 0; i < p; i++) {
        rc = pthread_create(&producers[i], &attr, producer, (void *) i);
        if (rc) {
            fprintf(stderr, "ERROR: return code from pthread_create is %d\n", rc);
            exit(1);
        }
    }

    pthread_t consumers[c];
    for (int i = 0; i < c; i++) {
        rc = pthread_create(&consumers[i], &attr, consumer, (void *) i);
        if (rc) {
            fprintf(stderr, "ERROR: return code from pthread_create is %d\n", rc);
            exit(1);
        }
    }

    pthread_t thread_allread;
    if(rc = pthread_create(&thread_allread, &attr, allread, NULL)) {
        fprintf(stderr, "ERROR: return code from pthread_create is %d\n", rc);
    }

    pthread_mutex_unlock(&mutex); /* release the hounds */

    for (int i = 0; i < p; i++) pthread_join(producers[i], NULL);
    for (int i = 0; i < c; i++) pthread_join(consumers[i], NULL);
    pthread_join(thread_allread, NULL);

    cout << "\\/\\/\\/\\/ AGGREGATE SALES \\/\\/\\/\\/" << endl;
    // print(*aggregate_store_wide_sales, *aggregate_monthly_sales, *aggregate_sales);
    computestats(*master_ledger);
    cout << "/\\/\\/\\/\\ AGGREGATE SALES /\\/\\/\\/\\" << endl;

    cout << "=====================" << endl;
    cout << "numProduced: " << numProduced  << endl
         << "numRead: " << numRead << endl;

    auto end = chrono::steady_clock::now();
    chrono::duration<double> time_elapsed = end - start;

    printf("elapsed time: %.3fs\n", time_elapsed.count());
    /* pointer housekeeping */
    delete buffer;
    delete master_ledger;
    delete aggregate_store_wide_sales;
    delete aggregate_monthly_sales;
    delete aggregate_sales;
    delete gen;

    #ifdef DEBUG
    delete prodsemfullval;
    delete prodsememptyval;
    delete conssemfullval;
    delete conssememptyval;
    #endif

    pthread_exit(NULL);

    return 0;
}

void *producer(void * arg) {

    /* producers assigned fixed store ID; store IDs on range [1, p]*/
    int id = (int) (int*) arg + 1;
    int sleepdur;

    time_t date;
    int regID;
    long double price;

    do
    {
        /* entry section */
        #ifdef DEBUG
        sem_getvalue(&buff_empty, &prodsememptyval[id]);
        printf("producer %d: buff_empty before: %d\n", id, prodsememptyval[id]);
        cout << "producer " << id << " requesting buff_empty" << endl;
        #endif

        sem_wait(&buff_empty);

        #ifdef DEBUG
        cout << "producer " << id << " acquired buff_empty" << endl;
        sem_getvalue(&buff_empty, &prodsememptyval[id]);
        printf("producer %d: buff_empty after: %d\n", id, prodsememptyval[id]);
        #endif

        #ifdef DEBUG
        cout << "producer " << id << " requesting mutex lock" << endl;
        #endif

        pthread_mutex_lock(&mutex);

        #ifdef DEBUG
        cout << "producer " << id << " acquired mutex lock" << endl;
        #endif

        /* critical section */
        if (numProduced < MAX_RECORDS) {

            #ifdef DEBUG
            cout << "producer " << id << ": put new record "
                        << numProduced << " in buffer " << numProduced % b << endl;
            #endif

            date = ddist(*gen);
            regID = regdist(*gen);
            price = pricedist(*gen);
            buffer[numProduced % b] = { date, id, regID, price };
            numProduced++;

            #ifdef DEBUG
            cout << "producer " << id << ": buffer contents: " << endl
                << "======================================" << endl;
            for (int i = 0; i < b; i++) {
                print(buffer[i]);
            }
            cout << "======================================" << endl;
            #endif

            #ifdef DEBUG
            sem_getvalue(&buff_full, &prodsemfullval[id]);
            printf("producer %d: buff_full before: %d\n", id, prodsemfullval[id]);
            #endif

            sem_post(&buff_full);

            #ifdef DEBUG
            cout << "producer " << id << " released buff_full" << endl;
            sem_getvalue(&buff_full, &prodsemfullval[id]);
            printf("producer %d: buff_full after: %d\n", id, prodsemfullval[id]);
            #endif
        }
        else {
            #ifdef DEBUG
            cout << "producer " << id << ": MAX_RECORDS met, undoing buff_empty wait" << endl;
            sem_getvalue(&buff_empty, &prodsememptyval[id]);
            printf("producer %d: buff_empty before: %d\n", id, prodsememptyval[id]);
            #endif

            sem_post(&buff_empty);

            #ifdef DEBUG
            cout << "producer " << id << " released buff_empty" << endl;
            sem_getvalue(&buff_empty, &prodsememptyval[id]);
            printf("producer %d: buff_empty after: %d\n", id, prodsememptyval[id]);
            #endif
        }

        pthread_mutex_unlock(&mutex);

        #ifdef DEBUG
        cout << "producer " << id << " released mutex lock" << endl;
        #endif

        sleepdur = sleepdist(*gen) * 1000;
        #ifdef DEBUG
        cout << "producer " << id << " sleeping for " << sleepdur / 1000 << "ms" << endl;
        #endif
        usleep(sleepdur);

        /* remainder section */
    } while(numProduced < MAX_RECORDS);

    int numfull = 0, numempty= 0;

    sem_getvalue(&buff_full, &numfull);
    sem_getvalue(&buff_empty, &numempty);

    #ifdef DEBUG
    printf("producer %d: numempty: %d; numfull: %d\n", id, numempty, numfull);
    #endif
    
    /* 
        if we've reached here, all the records have been generated, but there
        may still be threads waiting for buff_full
    */
    if (numempty == b) sem_post(&buff_full);
    
    #ifdef DEBUG
    cout << "producer " << id << " finished; terminating" << endl;
    #endif

    pthread_exit(NULL);
}

void *consumer(void * arg) {
    int id = (int) (int*) arg;

    vector<record> thread_ledger;
    int sleepdur;
    int semval = 0;

    do
    {
        #ifdef DEBUG
        sem_getvalue(&buff_full, &conssemfullval[id]);
        printf("consumer %d: buff_full before: %d\n", id, conssemfullval[id]);
        cout << "consumer " << id << " requesting buff_full" << endl;
        #endif

        sem_wait(&buff_full);

        #ifdef DEBUG
        cout << "consumer " << id << " acquired buff_full" << endl;
        sem_getvalue(&buff_full, &conssemfullval[id]);
        printf("consumer %d: buff_full after: %d\n", id, conssemfullval[id]);
        cout << "consumer " << id << " requesting mutex lock" << endl;
        #endif

        pthread_mutex_lock(&mutex);

        #ifdef DEBUG
        cout << "consumer " << id << " acquired mutex lock" << endl;
        #endif

        if (numRead < MAX_RECORDS) {
            #ifdef DEBUG
            cout << "consumer " << id << ": read thing " << numRead << " from buffer " << numRead % b << endl;
            #endif
            rptr = &buffer[numRead % b];
            thread_ledger.push_back(*rptr);
            master_ledger->push_back(*rptr);
            buffer[numRead % b] = EMPTY;

            numRead++;
        }

        #ifdef DEBUG
        cout << "consumer " << id << ": buffer contents: " << endl
             << "======================================" << endl;
        for (int i = 0; i < b; i++) {
            print(buffer[i]);
        }
        cout << "======================================" << endl;
        #endif

        pthread_mutex_unlock(&mutex);

        #ifdef DEBUG
        cout << "consumer " << id << " released mutex lock" << endl;
        sem_getvalue(&buff_empty, &conssememptyval[id]);
        printf("consumer %d: buff_empty before: %d\n", id, conssememptyval[id]);
        #endif
        
        sem_post(&buff_empty);

        #ifdef DEBUG
        cout << "consumer " << id << " released buff_empty" << endl;
        sem_getvalue(&buff_empty, &conssememptyval[id]);
        printf("consumer %d: buff_empty after: %d\n", id, conssememptyval[id]);
        #endif

        /* check for all records read */
        #ifdef DEBUG
        cout << "consumer " << id << " releasing check_done" << endl;
        #endif

        sem_post(&check_done);

        #ifdef DEBUG
        cout << "consumer " << id << " requesting proceed" << endl;
        #endif
        
        sem_wait(&proceed);

    } while (!done);

    int numfull = 0, numempty= 0;
    sem_getvalue(&buff_full, &numfull);
    sem_getvalue(&buff_empty, &numempty);

    #ifdef DEBUG
    printf("consumer %d: numempty: %d; numfull: %d\n", id, numempty, numfull);
    #endif

    if(numfull == 0) sem_post(&buff_full);

    sem_post(&proceed);

    sem_wait(&compute);
    pthread_mutex_lock(&mutex);

    printf("\\/\\/\\/\\/ consumer %d statistics \\/\\/\\/\\/\n", id);
    computestats(thread_ledger);
    printf("/\\/\\/\\/\\ consumer %d statistics /\\/\\/\\/\\\n\n", id);

    pthread_mutex_unlock(&mutex);

    #ifdef DEBUG
    cout << "consumer " << id << " finished; terminating" << endl;
    #endif

    pthread_exit(NULL);
}

void *allread(void *arg){
    int numwaiting;
    do {
        #ifdef DEBUG
        cout << "allread: requesting check_done" << endl;
        #endif

        sem_wait(&check_done);

        #ifdef DEBUG
        cout << "allread: received check_done" << endl;
        #endif

        done = numRead >= MAX_RECORDS;
        #ifdef DEBUG
        if (done) cout << "allread: all records read" << endl;
        #endif

        #ifdef DEBUG
        cout << "allread: releasing proceed" << endl;
        #endif

        sem_post(&proceed);
    } while (!done);

    pthread_exit(NULL);
}

void computestats(vector<record> &ledger) {
    struct tm *t_m;
    /* storeID : sale amt */
    map<int, long double> store_wide_total_sales;

    /* month : sale amt */
    map<int, long double> month_wise_total_sales;
    long double all_sales = 0;

    #ifdef DEBUG
    printf("computestats: ledger addr: %X\n", &ledger);
    #endif

    for (auto entry : ledger) {
        /* store-wide total sales */
        store_wide_total_sales[get<1>(entry)] += get<3>(entry);
        (*aggregate_store_wide_sales)[get<1>(entry)] += get<3>(entry);

        /* monthly total sales in all stores*/
        t_m = localtime(&get<0>(entry));
        month_wise_total_sales[t_m->tm_mon] += get<3>(entry);
        (*aggregate_monthly_sales)[t_m->tm_mon] += get<3>(entry);

        /* aggregate sales */
        all_sales += get<3>(entry);
        *aggregate_sales += get<3>(entry);
    }

    print(store_wide_total_sales, month_wise_total_sales, all_sales);
    sem_post(&compute);
}

void print(record r) {
    cout.imbue((locale("")));
    cout << left
         << put_time(localtime(&get<0>(r)), "%x") << setw(12) << ""
         << setw(20) << get<1>(r)
         << setw(20) << get<2>(r)
         << setw(20) << showbase << put_money(get<3>(r)) << endl;

}

void print(const map<int, long double>& swts, const map<int, long double>& mwts, const long double& all)
{
    cout.imbue(locale(""));
    cout << showbase;
    cout << "store-wide total sales" << endl;
    cout << left
         << setw(20) << "STORE"
         << setw(20) << "$ AMT"
         << endl;

    for (const auto& [key, value] : swts) {
        cout << setw(20) << key << setw(20) << put_money(value) << endl;
    }

    cout << endl;

    cout << "month-wise total sales" << endl;
    cout << left
         << setw(20) << "MONTH"
         << setw(20) << "$ AMT"
         << endl;

    struct tm *t_m;
    for (const auto& [key, value] : mwts) {
        t_m->tm_mon = key;
        cout << left 
             << setw(17) << put_time(t_m, "%b") << " " 
             << setw(20) << put_money(value) << endl;
    }

    cout << endl
         << "=================" << endl;

    cout << "total sales: " << put_money(all) << endl;
    cout << endl;
}