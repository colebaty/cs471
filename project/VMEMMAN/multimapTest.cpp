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
    while (refs.size() < 20) {
        refs.insert({ kdist(gen), idist(gen) });
    }

    cout << "==============" << endl;
    for (const auto& [key, value] : refs) {
        cout << "[" << key << "] = " << value << endl;
    }
    cout << "==============" << endl;

    return 0;
}
