#include <iostream>
#include <vector>
#include <utility>
#include <tuple>
#include <cmath>

using namespace std;

/* page size, ref addr, page no., offset*/
typedef tuple<int, int, int, int> record;

int main(int argc, char **argv)
{
    vector<int> sizes = {4, 8, 16};
    vector<int> addresses = { 102050, 12346, 203548, 10812 };
    vector<record> solution;

    int p, d;
    p = d = 0;

    for (size_t i = 0; i < sizes.size(); i++) {
        for (size_t j = 0; j < addresses.size(); j++) {
            p = floor(addresses[j] / sizes[i]);
            d = addresses[j] - (p * sizes[i]);
            solution.push_back( { sizes[i], addresses[j], p, d } );
        }
    }
    
    for (auto it: solution) {
        cout << "Page Size: " << get<0>(it) << "\tReference Address: " << get<1>(it)
             << "\tPage# " << get<2>(it) << " \tOffset: " << get<3>(it) <<  endl;
    }

    return 0;
}
