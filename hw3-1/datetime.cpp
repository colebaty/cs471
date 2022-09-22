#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <string>

using namespace std;

int main() {
    auto now(chrono::system_clock::now());

    auto since_epoch(
        chrono::duration_cast<chrono::seconds>(now.time_since_epoch()));
    
    time_t now_t(
        chrono::system_clock::to_time_t(
            chrono::system_clock::time_point(since_epoch)));

    char s[100];
    strftime(s, sizeof(s), "%T.", localtime(&now_t));

    string result(s);

    cout << "Time: " << result;
    printf("%2d\n", (now.time_since_epoch() - since_epoch).count() / 100);

    return 0;
}
