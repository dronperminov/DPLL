#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <ctime>
#include "ConjunctiveNormalForm.hpp"

using namespace std;

void TestFromDir(const string& dir, int count, bool isSat) {
    clock_t t0 = clock();
    for (int i = 1; i <= count; i++) {
        ifstream fin(dir + to_string(i) + ".cnf");
        ConjunctiveNormalForm cnf(fin, false);
        fin.close();

        assert(cnf.DPLL() == isSat);
    }
    clock_t t1 = clock();
    double time = double(t1 - t0) / CLOCKS_PER_SEC;

    cout << "Time for test '" << dir << "': " << time << " sec (" << (time / count) << " per iteration)" << endl;
}

int main(int argc, char **argv) {  
    TestFromDir("data/sat20/uf20-0", 1000, true);
    TestFromDir("data/unsat50/uuf50-0", 50, false);
}