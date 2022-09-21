#include <iostream>
#include <chrono>

using namespace std;

int main() {
    auto now = chrono::steady_clock::now();
    cout << "steady_clock" << now << endl;

    return 0;
}
