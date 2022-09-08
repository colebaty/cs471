#include <functional>
#include <queue>
#include <vector>
#include <iostream>

using namespace std;

class Process {
    public:
        int id, arr, burst;

        Process() {
            id = arr = burst = 0;
        }
} *p;

class Compare {
    public:
        bool operator() (const Process &left, const Process &right)  const
        {
            return left.arr < right.arr;
        }
};

int main(int argc, char **argv) {
    for (int i = 0; i < 3; i++) {
        p = new Process();
        p->id = i;
        p->arr = i;
        p->burst = i;
    }

    return 0;
}
