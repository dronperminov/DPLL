#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <chrono>
#include "ConjunctiveNormalForm.hpp"

using namespace std;

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::time_point<Time> TimePoint;
typedef std::chrono::milliseconds ms;

struct Task {
    string dirname;
    int count;
    bool isSat;
};

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

    if (strategy == DecisionStrategy::Up)
        return "up";

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

double TestFromDir(Task task, DecisionStrategy strategy, int loops = 10) {
    TimePoint t0 = Time::now();

    for (int loop = 0; loop < loops; loop++) {
        for (int i = 1; i <= task.count; i++) {
            ifstream fin(task.dirname + to_string(i) + ".cnf");
            ConjunctiveNormalForm cnf(fin);
            fin.close();
            assert(cnf.DPLL(strategy) == task.isSat);
        }
    }

    TimePoint t1 = Time::now();
    ms ellapsed = std::chrono::duration_cast<ms>(t1 - t0);

    return (double)ellapsed.count() / task.count / loops;
}

void PrintHeader(const vector<DecisionStrategy> &strategies) {
    cout << "## Performance of DPLL SAT solver" << endl;
    cout << "|     cnf \\ strategy     |";

    for (size_t i = 0; i < strategies.size(); i++)
        cout << setw(8) << StrategyToString(strategies[i]) << " |";

    cout << endl;

    cout << "|           :-:          |";

    for (size_t i = 0; i < strategies.size(); i++)
        cout << "     :-: |";

    cout << endl;
}

int main(int argc, char **argv) {
    vector<DecisionStrategy> strategies = {
        DecisionStrategy::Max,
        DecisionStrategy::Moms,
        DecisionStrategy::Weighted,
        DecisionStrategy::Up,
        DecisionStrategy::First,
        DecisionStrategy::Random
    };

    vector<Task> tasks = {
        { "data/sat20/uf20-0", 1000, true },

        { "data/sat50/uf50-0", 100, true },
        { "data/unsat50/uuf50-0", 100, false },

        { "data/sat75/uf75-0", 100, true },
        { "data/unsat75/uuf75-0", 100, false },

        { "data/sat100/uf100-0", 100, true },
        { "data/unsat100/uuf100-0", 100, false },
    };

    PrintHeader(strategies);

    for (size_t i = 0; i < tasks.size(); i++) {
        cout << "| " << setw(22) << tasks[i].dirname << " |";

        for (int j = 0; j < strategies.size(); j++)
            cout << " " << setw(7) << setprecision(3) << TestFromDir(tasks[i], strategies[j]) << " |";

        cout << endl;
    }

    cout << endl;

    for (int i = 0; i < strategies.size(); i++) {
        TestOneFile("data/hanoi/hanoi4.cnf", true, strategies[i]);
    }
}