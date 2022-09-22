#include <iostream>
#include <thread>
#include <ctime>
#include <chrono>

using namespace std;

bool flag[2];
int i = 0;
int j = 1;
int turn = 0;

char license;

int picount = 0, pjcount = 0;

void pi (char & license) {
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
    flag[i] = flag[j] = false;

    thread t1(pi, ref(license));

    t1.join();
    return 0;
}
