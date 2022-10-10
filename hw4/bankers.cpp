#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <cassert>
#include <string>
#include <utility>

using namespace std;



void run();

/**
 * @brief resource request
 * 
 * @param next 
 */
void rrequest(auto &next);

int main(int argc, char** argv)
{
    if (argc != 2) {
        cout << "\tUsage:\t./a.out <data file>" << endl;
        return -1;
    }

    readFile(argv[1]);

    run();


    return 0;
}

void run() {

    vector<vector<int>>::iterator rit = Request.front();
    do
    {
        rit = Request.front();
        rrequest(request);
        request++;
    } while (!Request.empty());
    
};
bool isSafe() {
    return false;
}
void rrequest(auto &it) {

}