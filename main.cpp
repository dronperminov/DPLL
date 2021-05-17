#include <iostream>
#include <fstream>
#include <string>
#include "ConjunctiveNormalForm.hpp"

using namespace std;

void Help() {
    cout << "DPLL algorithm." << endl;
    cout << "Developed by Andrew Perminov" << endl;
    cout << "Usage: ./dpll [path/to/cnf/file] [-d or --debug]" << endl;
}

int main(int argc, char **argv) {
    if (argc == 2 && string(argv[1]) == "--help") {
        Help();
        return 0;
    }

    if (argc != 2 && argc != 3) {
        cout << "Error: invalid arguments count" << endl;
        return -1;
    }

    if (argc == 3 && string(argv[2]) != "--debug" && string(argv[2]) != "-d") {
        cout << "Error: invalid debug flag" << endl;
        return -1;
    }
    
    ifstream fin(argv[1]);

    if (!fin) {
        cout << "Error: unable to open file '" << argv[1] << "'" << endl;
        return -1;
    }

    try {
        ConjunctiveNormalForm cnf(fin, argc == 3);
        fin.close();

        cout << "Readed cnf:" << endl;
        cnf.Print();
        cnf.PrintTermValues();
    }
    catch (const string& error) {
        cout << "Error: " << error << endl;
    }
}