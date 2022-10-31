#include <iostream>
#include <vector>
#include <cstdlib> /* for abs(int n) */
#include <iterator>

using namespace std;

void fcfs(int start, vector<int>& seq);
void sstf();
void scan();
void look();

int main() {

    int start = 3250;
    int numCylinders = 6000;

    vector<int> sequence = {
        1515, 1212, 1296, 
        3800, 544, 618, 
        356, 2523, 5965
    };

    fcfs(start, sequence);

    return 0;
}

void fcfs(int start, vector<int>& seq) {
    int sum = 0;
    sum += abs(start - seq.front());

    vector<int>::iterator it = seq.begin();

    int diff = 0;

    while (it != seq.last()) {
        if (next(it) != seq.end()) {
            diff = abs(*it - *next(it));
            sum += diff;
        }
        it++;
    }

    cout << "fcfs sum: " << sum << endl;

}