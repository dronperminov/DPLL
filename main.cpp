#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "ConjunctiveNormalForm.hpp"

using namespace std;

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::time_point<Time> TimePoint;
typedef std::chrono::milliseconds ms;

void Help() {
    cout << "DPLL algorithm." << endl;
    cout << "Developed by Andrew Perminov" << endl << endl;

    cout << "Usage: ./dpll [path/to/cnf/file] [strategy] [-d] [-s]" << endl;
    cout << endl;
    cout << "Decision strategies:" << endl;
    cout << "  first    - get first undefined literal" << endl;
    cout << "  random   - get random undefined literal" << endl;
    cout << "  max      - get literal with max occurencies in clauses" << endl;
    cout << "  moms     - get literal with max occurencies in clauses with minimal size" << endl;
    cout << "  weighted - get literal with max weighted sum (score of l = 2^-|clause with l|)" << endl;
    cout << "  up       - get literal with max up value (up in unit propagation)" << endl << endl;

    cout << "Flags:" << endl;
    cout << "  -d  - remove duplicate clauses during reading (increase time, false for default)" << endl;
    cout << "  -s  - use subsumption after read (increase time even more, false for default)" << endl;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        cout << "No arguments. Run ./dpll --help for usage" << endl;
        return 0;
    }

    if (argc == 2 && string(argv[1]) == "--help") {
        Help();
        return 0;
    }

    if (argc > 5) {
        cout << "Error: invalid arguments count" << endl;
        return -1;
    }
    
    try {
        DecisionStrategy strategy = DecisionStrategy::First; // стратегия выбора литералов
        bool haveStrategy = false; // определена ли стратегия уже
        bool removeDuplicates = false; // удалять ли дублирующиеся клаузы
        bool useSubsumption = false; // удалять ли включающие клаузы

        for (int i = 2; i < argc; i++) {
            string arg = argv[i];

            if (arg == "-d") {
                removeDuplicates = true;
            }
            else if (arg == "-s") {
                useSubsumption = true;
            }
            else if (!haveStrategy) {
                strategy = GetStrategy(arg);
                haveStrategy = true;
            }
            else {
                throw std::string("Invalid argument '") + arg + "'";
            }
        }

        ifstream fin(argv[1]);

        if (!fin) {
            cout << "Error: unable to open file '" << argv[1] << "'" << endl;
            return -1;
        }

        cout << "Run parameters: " << endl;
        cout << "  Strategy: " << StrategyToString(strategy) << endl;
        cout << "  Remove duplicates: " << (removeDuplicates ? "yes" : "no") << endl;
        cout << "  Use subsumption: " << (useSubsumption ? "yes" : "no") << endl;

        TimePoint t0 = Time::now();
        ConjunctiveNormalForm cnf(fin, removeDuplicates, useSubsumption);
        fin.close();
        TimePoint t1 = Time::now();

        cout << "  DPLL verdict: ";

        if (cnf.DPLL(strategy)) {
            cout << "SAT" << endl;
        }
        else {
            cout << "UNSAT" << endl;
        }

        TimePoint t2 = Time::now();

        cout << endl;
        cout << "  Reading/preprocessing time: " << (std::chrono::duration_cast<ms>(t1 - t0).count()) << " ms" << endl;
        cout << "  DPLL time: " << (std::chrono::duration_cast<ms>(t2 - t1).count()) << " ms" << endl;
    }
    catch (const string& error) {
        cout << "Error: " << error << endl;
    }
}