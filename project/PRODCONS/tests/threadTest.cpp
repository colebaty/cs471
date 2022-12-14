// #define DEBUG

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

#define MAX_RECORDS 1000

const time_t YEAR_START = 1451606400;
const time_t YEAR_END = 1483228799;

enum month { JAN, FEB, MAR, APR, JUN, JUL, AUG, SEP, OCT, NOV, DEC };

using namespace std;

/* sales date, store ID, register, sale amt */
typedef tuple<time_t, int, int, long double> record;

/* shared variables */
sem_t buff_full, buff_empty, buff_mutex;
pthread_mutex_t mutex;

vector<record> master_ledger;
// record *buffer;
int * buffer;
int p, c, b, index = 0;
int numProduced = 0, numRead = 0;

default_random_engine * gen;

#ifdef DEBUG
int *prodsemfullval, *prodsememptyval;
int *conssemfullval, *conssememptyval;
#endif

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

    // buffer = new record[b];
    #ifdef DEBUG
    prodsemfullval = new int[b];
    prodsememptyval = new int[b];
    conssemfullval = new int[b];
    conssememptyval = new int[b];
    #endif
    buffer = new int[b];
    for (size_t i = 0; i < b; i++) buffer[i] = -1;
    
    cout << "========= info =================" << endl;
    cout << "b: " << b << endl;
    cout << "buffer: ";
    for (int i = 0; i < b; i++) cout << buffer[i] << " ";
    cout << endl;

    cout << "================================" << endl;


    random_device r;
    gen = new default_random_engine(r());

    sem_init(&buff_empty, 0, b);
    sem_init(&buff_full, 0, 0);
    // sem_init(&buff_mutex, 0, 0);/* start locked */
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_lock(&mutex);

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // pthread_t prod1;
    // int rc, id;
    // id = 1;
    // rc = pthread_create(&prod1, &attr, producer, (void *) &id);
    // if (rc) {
    //     fprintf(stderr, "ERROR: return code from pthread_create is %d\n", rc);
    //     exit(1);
    // }

    // usleep(1000);

    // pthread_t prod2;
    // id = 2;
    // rc = pthread_create(&prod2, &attr, producer, (void *) &id);
    // if (rc) {
    //     fprintf(stderr, "ERROR: return code from pthread_create is %d\n", rc);
    //     exit(1);
    // }

    pthread_t producers[p];
    int rc;
    for (int i = 0; i < p; i++) {
        rc = pthread_create(&producers[i], &attr, producer, (void *) i);
        if (rc) {
            fprintf(stderr, "ERROR: return code from pthread_create is %d\n", rc);
            exit(1);
        }
    }

    // pthread_t cons;
    // rc = pthread_create(&cons, &attr, consumer, NULL);
    // if (rc) {
    //     fprintf(stderr, "ERROR: return code from pthread_create is %d\n", rc);
    //     exit(1);
    // }

    pthread_t consumers[c];
    for (int i = 0; i < c; i++) {
        rc = pthread_create(&consumers[i], &attr, consumer, (void *) i);
        if (rc) {
            fprintf(stderr, "ERROR: return code from pthread_create is %d\n", rc);
            exit(1);
        }
    }

    // sem_post(&buff_mutex); /* release the hounds */
    pthread_mutex_unlock(&mutex);

    // pthread_join(prod1, NULL);
    // pthread_join(prod2, NULL);

    for (int i = 0; i < p; i++) pthread_join(producers[i], NULL);
    for (int i = 0; i < c; i++) pthread_join(consumers[i], NULL);

    cout << "=====================" << endl;
    cout << "numProduced: " << numProduced  << endl
         << "numRead: " << numRead << endl;

    pthread_exit(NULL);

    return 0;
}

void *producer(void * arg)
{
    uniform_int_distribution<> sleepdist(5,40);

    int id = (int) (int*) arg;

    int sleepdur;
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
        // sem_wait(&buff_mutex);
        #ifdef DEBUG
        cout << "producer " << id << " acquired mutex lock" << endl;
        #endif

        /* critical section */
        if (numProduced < MAX_RECORDS) {

            cout << "producer " << id << ": put new thing "
                        << numProduced << " in buffer " << numProduced % b << endl;
            buffer[numProduced % b] = numProduced;
            numProduced++;

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
            cout << "producer " << id << ": MAX_RECORDS met, undoing buff_empty wait" << endl;
            #ifdef DEBUG
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
        
        cout << "producer " << id << ": buffer: ";
        for (int i = 0; i < b; i++) cout << buffer[i] << " ";
        cout << endl;

        pthread_mutex_unlock(&mutex);

        #ifdef DEBUG
        cout << "producer " << id << " released mutex lock" << endl;
        #endif

        sleepdur = sleepdist(*gen) * 1000;
        cout << "producer " << id << " sleeping for " << sleepdur / 1000 << "ms" << endl;
        usleep(sleepdur);

        /* remainder section */
        // this_thread::sleep_for(chrono::milliseconds{sleepdist(gen)});
    } while(numProduced < MAX_RECORDS);

    int numfull = 0, numempty= 0;
    sem_getvalue(&buff_full, &numfull);
    sem_getvalue(&buff_empty, &numempty);
    printf("producer %d: numempty: %d; numfull: %d\n", id, numempty, numfull);
    
    if (numempty == b) sem_post(&buff_full);
    
    cout << "producer " << id << " finished; terminating" << endl;

    pthread_exit(NULL);
}

void *consumer(void * arg) {
    uniform_int_distribution<> sleepdist(5,40);

    int id = (int) (int*) arg;

    vector<record> thread_ledger;
    int sleepdur;
    int semval = 0;

    // while (numRead < MAX_RECORDS) 
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
        // sem_wait(&buff_mutex);

        #ifdef DEBUG
        cout << "consumer " << id << " acquired mutex lock" << endl;
        #endif

        // thread_ledger.push_back(buffer[numRead % b]);
        // cout << "consumer: ";
        // print(buffer[index % b]);
        // buffer[index % b] = { 0, 0, 0, 0 };
        // index--;
        if (numRead < MAX_RECORDS) {
            cout << "consumer " << id << ": read thing " << numRead << " from buffer " << numRead % b << endl;
            buffer[numRead % b] = -1;

            numRead++;
        }

        cout << "consumer " << id << ": buffer: ";
        for (int i = 0; i < b; i++) cout << buffer[i] << " ";
        cout << endl;

        // pthread_mutex_unlock(&buff_mutex);
        // sem_post(&buff_mutex);
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

        sleepdur = sleepdist(*gen) * 1000;
        cout << "consumer " << id << " sleeping for " << sleepdur / 1000 << "ms" << endl;
        usleep(sleepdur);
    } while (numRead < MAX_RECORDS);

    int numfull = 0, numempty= 0;
    sem_getvalue(&buff_full, &numfull);
    sem_getvalue(&buff_empty, &numempty);
    printf("consumer %d: numempty: %d; numfull: %d\n", id, numempty, numfull);

    if(numfull == 0) sem_post(&buff_full);

    cout << "consumer " << id << " finished; terminating" << endl;

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