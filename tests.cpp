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

double TestOneFile(const string& path, bool isSat, DecisionStrategy strategy, int loops = 10) {
    TimePoint t0 = Time::now();

    for (int loop = 0; loop < loops; loop++) {
        ifstream fin(path);
        ConjunctiveNormalForm cnf(fin, true, true);
        fin.close();

        assert(cnf.DPLL(strategy) == isSat);
    }

    TimePoint t1 = Time::now();
    ms ellapsed = std::chrono::duration_cast<ms>(t1 - t0);

    return (double)ellapsed.count() / loops;
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
    cout << "|         cnf \\ strategy         |";

    for (size_t i = 0; i < strategies.size(); i++)
        cout << setw(8) << StrategyToString(strategies[i]) << " |";

    cout << endl;

    cout << "|               :-:              |";

    for (size_t i = 0; i < strategies.size(); i++)
        cout << "     :-: |";

    cout << endl;
}

int main(int argc, char **argv) {
    vector<DecisionStrategy> strategies = {
        DecisionStrategy::Max,
        DecisionStrategy::Moms,
        DecisionStrategy::Weighted,
        DecisionStrategy::AUPC,
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

    vector<Task> oneTasks = {
        { "data/pigeon-hole/hole6.cnf", 1, false },
        { "data/pigeon-hole/hole7.cnf", 1, false },
        { "data/pigeon-hole/hole8.cnf", 1, false },
        { "data/pigeon-hole/hole9.cnf", 1, false },
        { "data/hanoi/hanoi4.cnf", 1, true },
    };

    PrintHeader(strategies);

    for (size_t i = 0; i < tasks.size(); i++) {
        cout << "| " << setw(30) << tasks[i].dirname << " |";

        for (int j = 0; j < strategies.size(); j++)
            cout << " " << setw(7) << setprecision(3) << TestFromDir(tasks[i], strategies[j]) << " |";

        cout << endl;
    }

    for (size_t i = 0; i < oneTasks.size(); i++) {
        cout << "| " << setw(30) << oneTasks[i].dirname << " |";

        for (int j = 0; j < strategies.size(); j++)
            cout << " " << setw(7) << setprecision(3) << TestOneFile(oneTasks[i].dirname, oneTasks[i].isSat, strategies[j]) << " |";

        cout << endl;
    }
}