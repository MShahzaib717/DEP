#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>

using namespace std;

string readFile(const string &filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: Could not open the file " << filename << endl;
        exit(1);
    }

    string contents((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    return contents;
}

void writeFile(const string &filename, const string &data) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: Could not write to the file " << filename << endl;
        exit(1);
    }

    file << data;
    file.close();
}

string compressRLE(const string &data) {
    if (data.empty()) return "";

    ostringstream compressed;
    int n = data.length();

    for (int i = 0; i < n; ++i) {
        int count = 1;
        while (i < n - 1 && data[i] == data[i + 1]) {
            ++count;
            ++i;
        }
        compressed << data[i];
        if (count > 9) {
            while (count > 9) {
                compressed << 9;
                count -= 9;
            }
        }
        compressed << count;
    }

    return compressed.str();
}

string decompressRLE(const string &data) {
    ostringstream decompressed;
    int n = data.length();

    for (int i = 0; i < n; ++i) {
        char ch = data[i];
        ++i;
        int count = data[i] - '0';
        decompressed << string(count, ch);
    }

    return decompressed.str();
}

bool isCompressed(const string &data) {
    for (char ch : data) {
        if (isdigit(ch)) {
            return true;
        }
    }
    return false;
}

int main() {
    while (true) {
        string choice;
        string inputFile;
        string outputFile;

        cout << "Enter 'c' to compress or 'd' to decompress: ";
        cin >> choice;

        if (choice != "c" && choice != "d") {
            cerr << "Invalid choice!" << endl;
            continue;
        }

        cout << "Enter input file name: ";
        cin >> inputFile;
        cout << "Enter output file name: ";
        cin >> outputFile;

        string fileContents = readFile(inputFile);
        string result;

        if (choice == "c") {
            if (isCompressed(fileContents)) {
                cerr << "Compression not possible: file is already compressed." << endl;
            } else {
                result = compressRLE(fileContents);
                writeFile(outputFile, result);
                cout << "File compressed successfully." << endl;
            }
        } else if (choice == "d") {
            if (!isCompressed(fileContents)) {
                cerr << "Decompression not possible: file is already in original form." << endl;
            } else {
                result = decompressRLE(fileContents);
                writeFile(outputFile, result);
                cout << "File decompressed successfully." << endl;
            }
        }

        char cont;
        cout << "Press 'q' to quit or any other key to continue: ";
        cin >> cont;
        system("cls");
        if (cont == 'q') {
            break;
        }
    }

    return 0;
}
