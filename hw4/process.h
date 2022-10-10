#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <vector>

using namespace std;

class Process {
    private:
        vector<int> *Max;
        vector<int> *Allocation;
        vector<int> *Need;

        int _id;
        //private methods
    public:
        Process();
        Process(int id, int numResources);

        //public methods
        void allocate(vector<int> allocation);

        void display();
};


#endif //__PROCESS_H__