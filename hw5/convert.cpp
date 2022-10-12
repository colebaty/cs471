#include <iostream>
#include <vector>
#include <utility>
#include <tuple>
#include <cmath>

using namespace std;

/* page size, ref addr, page no., offset*/
typedef tuple<int, int, int, int> record;


enum filesize { KB, MB, GB };

/**
 * @brief returns the size in bytes for a given file size
 * 
 * @param size 
 * @param fs one of KB (default), MB, GB
 * @return int the size in bytes
 */
int byteSize(int size, filesize fs = KB);

int main(int argc, char **argv)
{
    vector<int> sizes = {4, 8, 16}; /* sizes in KB */
    vector<int> addresses = { 102050, 12346, 203548 };
    vector<record> solution;

    int p, d;
    p = d = 0;

    int pageSize = 0; /* size in bytes */
    for (size_t i = 0; i < sizes.size(); i++) {
        pageSize = byteSize(sizes[i]);
        for (size_t j = 0; j < addresses.size(); j++) {
            p = floor(addresses[j] / pageSize);
            d = addresses[j] - (p * pageSize);
            solution.push_back( { sizes[i], addresses[j], p, d } );
        }
    }
    
    for (auto it: solution) {
        cout << "Page Size: " << get<0>(it) << "KB\tReference Address: " << get<1>(it)
             << "\tPage# " << get<2>(it) << " \tOffset: " << get<3>(it) <<  endl;
    }

    return 0;
}

int byteSize(int size, filesize fs){
    int scalar = 0;
    switch (fs)
    {
    case KB:
        scalar = (int) pow(2, 10);
        break;
    case MB:
        scalar = (int) pow(2, 20);
        break;
    case GB:
        scalar = (int) pow(2, 30);
        break;
    default:
        break;
    }

    return size * scalar;
}