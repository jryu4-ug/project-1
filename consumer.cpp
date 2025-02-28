#include <iostream>
#include <string>
#include <vector>

using namespace std;
int main() {
    string line;
    vector<string> lines;

    // Read data from stdin
    while (getline(cin, line)) {
        lines.push_back(line);
    }

    // Process and display the received data
    cout << "Consumer received " << lines.size() << " lines:" << endl;

    int lineNumber = 1;
    for (const auto& line : lines) {
        cout << lineNumber++ << ": " << line << endl;
    }
}