#include <list>
#include <vector>
#include <tuple>
#include <string>

using namespace std;

class State {
    public:
        State();

        void readFile(string filename);

        bool safe();
        void request();

    private:
        vector<int> Available;
        vector<vector<int>> Max;
        vector<vector<int>> Allocation;
        vector<vector<int>> Need;
        vector<vector<int>> Request;
};