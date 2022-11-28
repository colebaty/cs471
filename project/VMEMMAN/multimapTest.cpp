#include <iostream>
#include <map>
#include <random>

using namespace std;

int main() {
    random_device r;
    default_random_engine gen(r());

    uniform_int_distribution<> idist(1, 10); /* index distribution */
    uniform_int_distribution<> kdist(1000, 1005); /* key distribution */
    multimap<int, int> refs;

    int i = 0;
    while (refs.size() < 100) {
        refs.insert({ kdist(gen), idist(gen) });
    }

    cout << "==============" << endl;
    for (const auto& [key, value] : refs) {
        cout << "[" << key << "] = " << value << endl;
    }
    cout << "==============" << endl;

    auto search = refs.lower_bound(1005);
    while (search != refs.end()) { /* while this key is populated */
        printf("removing pair [%d]:%d\n", search->first, search->second);
        refs.erase(search);
        search = refs.lower_bound(1005);
    }

    cout << "==============" << endl;
    for (const auto& [key, value] : refs) {
        cout << "[" << key << "] = " << value << endl;
    }
    cout << "==============" << endl;

    return 0;
}
