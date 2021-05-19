#include <iostream>
#include <fstream>
#include <string>
#include "ConjunctiveNormalForm.hpp"

using namespace std;

void Help() {
    cout << "DPLL algorithm." << endl;
    cout << "Developed by Andrew Perminov" << endl;
    cout << "Usage: ./dpll [path/to/cnf/file] [strategy]" << endl;
    cout << endl;
    cout << "Decision strategies:" << endl;
    cout << "  first    - get first undefined literal" << endl;
    cout << "  random   - get random undefined literal" << endl;
    cout << "  max      - get literal with max occurencies in clauses" << endl;
    cout << "  moms     - get literal with max occurencies in clauses with minimal size" << endl;
    cout << "  weighted - get literal with max weighted sum (score of l = 2^-|clause with l|)" << endl;
    cout << "  up       - get literal with max up value (up in unit propagation)" << endl;
}

DecisionStrategy GetStrategy(const string& strategy) {
    if (strategy == "first")
        return DecisionStrategy::First;

    if (strategy == "random")
        return DecisionStrategy::Random;

    if (strategy == "max")
        return DecisionStrategy::Max;

    if (strategy == "moms")
        return DecisionStrategy::Moms;

    if (strategy == "weighted")
        return DecisionStrategy::Weighted;

    if (strategy == "up")
        return DecisionStrategy::Up;

    throw string("Invalid strategy name '") + strategy + "'";
}

int main(int argc, char **argv) {
    if (argc == 1) {
        cout << "no arguments. Run ./dpll --help for usage" << endl;
        return 0;
    }

    if (argc == 2 && string(argv[1]) == "--help") {
        Help();
        return 0;
    }

    if (argc != 2 && argc != 3) {
        cout << "Error: invalid arguments count" << endl;
        return -1;
    }
    
    ifstream fin(argv[1]);

    if (!fin) {
        cout << "Error: unable to open file '" << argv[1] << "'" << endl;
        return -1;
    }

    DecisionStrategy strategy = DecisionStrategy::First;

    try {
        if (argc == 3) {
            strategy = GetStrategy(argv[2]);
        }

        ConjunctiveNormalForm cnf(fin);
        fin.close();

        if (cnf.DPLL(strategy)) {
            cout << "SAT" << endl;
        }
        else {
            cout << "UNSAT" << endl;
        }
    }
    catch (const string& error) {
        cout << "Error: " << error << endl;
    }
}