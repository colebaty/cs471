#include <iostream>
#include <thread>
#include <ctime>
#include <chrono>
#include <random>
#include <string>

using namespace std;

bool flag[2];
int i = 0;
int j = 1;
int turn = 0;

int license; /* will contain id of process "using" the license */

int picount = 0, pjcount = 0;

string getTime() {
    auto now(chrono::system_clock::now());

    auto seconds_since_epoch(
        chrono::duration_cast<chrono::seconds>(now.time_since_epoch()));
    
    time_t now_t(
        chrono::system_clock::to_time_t(
            chrono::system_clock::time_point(seconds_since_epoch)));

    char s[100];
    if (!strftime(s, sizeof(s), "%T.", localtime(&now_t)));
        return "";

     return string(s) + 
        to_string((now.time_since_epoch() - seconds_since_epoch).count());
}

void pi (int & license, default_random_engine & gen, binomial_distribution<time_t> & ndist) {
    do {
        /* entry section */
        flag[i] = true;
        turn = j; /* generous */
        while (flag[j] == true && turn == j); /* busy wait */

        /* critical section */

        /* exit section */
    } while (picount++ < 3);
}

int main() {
    default_random_engine gen(time(NULL));
    binomial_distribution<time_t> ndist(1000);

    flag[i] = flag[j] = false;

    thread t1(pi, ref(license), ref(gen), ref(ndist));

    t1.join();
    return 0;
}
