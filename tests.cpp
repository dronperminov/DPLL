#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <ctime>
#include "ConjunctiveNormalForm.hpp"

using namespace std;

string StrategyToString(DecisionStrategy strategy) {
    if (strategy == DecisionStrategy::First)
        return "first";

    if (strategy == DecisionStrategy::Random)
        return "random";

    if (strategy == DecisionStrategy::Max)
        return "max";

    if (strategy == DecisionStrategy::Moms)
        return "moms";

    if (strategy == DecisionStrategy::Weighted)
        return "weighted";

    return "";
}

void TestOneFile(const string& path, bool isSat, DecisionStrategy strategy) {
    clock_t t0 = clock();
    ifstream fin(path);
    ConjunctiveNormalForm cnf(fin);
    fin.close();

    assert(cnf.DPLL(strategy) == isSat);

    clock_t t1 = clock();
    double time = double(t1 - t0) / CLOCKS_PER_SEC;

    cout << "Time for test '" << path << "' (" + StrategyToString(strategy) + "): " << time << " sec" << endl;
}

void TestFromDir(const string& dir, int count, bool isSat, DecisionStrategy strategy) {
    clock_t t0 = clock();

    for (int i = 1; i <= count; i++) {
        ifstream fin(dir + to_string(i) + ".cnf");
        ConjunctiveNormalForm cnf(fin);
        fin.close();
        assert(cnf.DPLL(strategy) == isSat);
    }

    clock_t t1 = clock();
    double time = double(t1 - t0) / CLOCKS_PER_SEC;

    cout << "Time for test '" << dir << "' (" + StrategyToString(strategy) + "): " << time << " sec (" << (time / count * 1000) << " ms per iteration)" << endl;
}

int main(int argc, char **argv) {
    DecisionStrategy strategies[5] = {
        DecisionStrategy::Max,
        DecisionStrategy::Moms,
        DecisionStrategy::Weighted,
        DecisionStrategy::First,
        DecisionStrategy::Random
    };

    for (int i = 0; i < 5; i++) {
        DecisionStrategy strategy = strategies[i];

        cout << endl << StrategyToString(strategy) << ":" << endl;
        TestFromDir("data/sat20/uf20-0", 1000, true, strategy);
        TestFromDir("data/sat50/uf50-0", 100, true, strategy);
        TestFromDir("data/sat75/uf75-0", 100, true, strategy);
        TestFromDir("data/sat100/uf100-0", 100, true, strategy);
        TestFromDir("data/unsat50/uuf50-0", 100, false, strategy);
        TestFromDir("data/unsat75/uuf75-0", 100, false, strategy);
        TestFromDir("data/unsat100/uuf100-0", 100, false, strategy);
    }

    cout << endl;

    for (int i = 0; i < 5; i++) {
        TestOneFile("data/hanoi/hanoi4.cnf", true, strategies[i]);
    }
}