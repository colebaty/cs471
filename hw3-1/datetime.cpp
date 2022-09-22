#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;

int main() {
    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout << "chrono::system_clock: " << ctime(&now)<< endl;

    return 0;
}
