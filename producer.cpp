#include <iostream>
#include <string>

using namespace std;

int main() {
    // Data being sent through the pipe
    string data = "This is a test message sent through the pipe\n";
    data += "Second line of the message\n";
    data += "Third line with some numbers: 123, 456, 789\n";

    // Write data to stdout
    cout << data;
}