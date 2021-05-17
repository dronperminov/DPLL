#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <stack>
#include <string>

// значения термов
enum class TermValue {
    Undefined,
    True,
    False
};

class ConjunctiveNormalForm {
    bool debug; // нужна ли отладка
    int variablesCount; // количество переменных
    int clausesCount; // количество клауз
    std::vector<std::set<int>> clauses; // клаузы
    std::vector<TermValue> values; // значения термов

    void SetVariablesCount(int variablesCount); // обновление количества переменных
    void SetClausesCount(int clausesCount); // обновление количества клауз
    void AddClause(const std::string& line); // добавление клаузы
public:
    ConjunctiveNormalForm(std::istream &fin, bool debug);

    void Print() const; // вывод СКНФ
    void PrintTermValues() const; // вывод значений термов
};

ConjunctiveNormalForm::ConjunctiveNormalForm(std::istream &fin, bool debug) {
    this->debug = debug;
    this->variablesCount = 0;
    this->clausesCount = 0;

    std::string line;

    while (std::getline(fin, line) && line != "0") {
        if (line == "" || line[0] == 'c' || line[0] == '%')
            continue; // игнорируем пустые строки и строки с комментариями

        if (line[0] == 'p') {
            std::string tmp;
            std::stringstream ss(line);
            ss >> tmp >> tmp >> variablesCount >> clausesCount;
            SetVariablesCount(variablesCount);
            SetClausesCount(clausesCount);
            continue;
        }

        AddClause(line);
    }

    if (clauses.size() != clausesCount)
        throw std::string("Invalid file: different clauses count");

    values = std::vector<TermValue>(variablesCount, TermValue::Undefined); // значения переменных не определены
}

// обновление количества переменных
void ConjunctiveNormalForm::SetVariablesCount(int variablesCount) {
    if (variablesCount <= 0)
        throw std::string("ConjunctiveNormalForm::SetVariablesCount: variables count must be positive");

    this->variablesCount = variablesCount;
}

// обновление количества клауз
void ConjunctiveNormalForm::SetClausesCount(int clausesCount) {
    if (clausesCount <= 0)
        throw std::string("ConjunctiveNormalForm::SetClausesCount: clauses count must be positive");

    this->clausesCount = clausesCount;
    clauses.reserve(clausesCount);
}

// добавление клаузы
void ConjunctiveNormalForm::AddClause(const std::string& line) {
    std::stringstream ss(line);
    std::set<int> clause;
    int literal;

    while (ss >> literal && literal != 0) {
        if (literal == 0 || abs(literal) > variablesCount)
            throw std::string("Invalid literal index at line '") + line + "'";

        clause.insert(literal); // считываем клаузы
    }

    clauses.push_back(clause); // добавляем клаузы
}

// вывод СКНФ
void ConjunctiveNormalForm::Print() const {
    std::cout << "Variables count: " << variablesCount << std::endl;
    std::cout << "Clauses count: " << clausesCount << std::endl;
    std::cout << "Clauses:" << std::endl;

    bool wasPrinted = false;

    for (int i = 0; i < clausesCount; i++) {
        if (wasPrinted)
            std::cout << "  AND" << std::endl;

        std::cout << "  (";

        for (auto it = clauses[i].begin(); it != clauses[i].end(); it++) {
            if (it != clauses[i].begin()) {
                std::cout << " OR ";
            }

            if (*it < 0) {
                std::cout << "NOT x" << (-*it);
            }
            else {
                std::cout << "x" << *it;
            }
        }

        std::cout << ")" << std::endl;
        wasPrinted = true;
    }
}

// вывод значений термов
void ConjunctiveNormalForm::PrintTermValues() const {
    std::cout << "+------------+-----------+" << std::endl;
    std::cout << "|    term    |   value   |" << std::endl;
    std::cout << "+------------+-----------+" << std::endl;

    for (size_t i = 0; i < values.size(); i++) {
        std::cout << "| " << std::setw(10) << ("x" + std::to_string(i + 1)) << " | " << std::setw(9);

        switch (values[i]) {
            case TermValue::Undefined:
                std::cout << "undefined";
                break;

            case TermValue::True:
                std::cout << "true";
                break;

            case TermValue::False:
                std::cout << "false";
                break;
        }

        std::cout << " |" << std::endl;
    }

    std::cout << "+------------+-----------+" << std::endl;
}
