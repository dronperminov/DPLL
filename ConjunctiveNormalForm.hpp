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

struct Assignment {
    int literal;
    TermValue value;
};

class ConjunctiveNormalForm {
    bool debug; // нужна ли отладка
    int literalsCount; // количество литералов
    int clausesCount; // количество клауз
    std::vector<std::set<int>> clauses; // клаузы
    std::vector<TermValue> values; // значения термов

    void SetLiteralsCount(int literalsCount); // обновление количества литералов
    void SetClausesCount(int clausesCount); // обновление количества клауз
    void AddClause(const std::string& line); // добавление клаузы

    TermValue GetLiteralValue(int literal) const; // получение значения литерала
    bool IsUnitClause(size_t index) const; // является ли клауза единичной
    bool IsRemovedClause(size_t index) const; // удалена ли клауза (есть ли единичные литералы)
    bool IsEmptyClause(size_t index) const; // пустая ли клауза

    int GetUnitLiteral(size_t index) const; // получение литерала из единичной клаузы
    void UnitPropagation(std::stack<int> &assignments); // распространение констант

    bool IsSolve() const; // все ли клаузы удалены
    bool IsConflict() const; // есть ли пустые клаузы

    void RollBack(std::stack<int> &assignments, std::stack<Assignment> &decisions); // откат
public:
    ConjunctiveNormalForm(std::istream &fin, bool debug);

    void Print() const; // вывод СКНФ
    void PrintTermValues() const; // вывод значений термов

    bool DPLL(); // алгоритм DPLL
};

ConjunctiveNormalForm::ConjunctiveNormalForm(std::istream &fin, bool debug) {
    this->debug = debug;
    this->literalsCount = 0;
    this->clausesCount = 0;

    std::string line;

    while (std::getline(fin, line) && line != "0") {
        if (line == "" || line[0] == 'c' || line[0] == '%')
            continue; // игнорируем пустые строки и строки с комментариями

        if (line[0] == 'p') {
            std::string tmp;
            std::stringstream ss(line);
            ss >> tmp >> tmp >> literalsCount >> clausesCount;
            SetLiteralsCount(literalsCount);
            SetClausesCount(clausesCount);
            continue;
        }

        AddClause(line);
    }

    if (clauses.size() != clausesCount)
        throw std::string("Invalid file: different clauses count");

    values = std::vector<TermValue>(literalsCount, TermValue::Undefined); // значения литералов не определены
}

// обновление количества литералов
void ConjunctiveNormalForm::SetLiteralsCount(int literalsCount) {
    if (literalsCount <= 0)
        throw std::string("ConjunctiveNormalForm::SetLiteralsCount: variables count must be positive");

    this->literalsCount = literalsCount;
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
        if (literal == 0 || abs(literal) > literalsCount)
            throw std::string("Invalid literal index at line '") + line + "'";

        clause.insert(literal); // считываем клаузы
    }

    clauses.push_back(clause); // добавляем клаузы
}

// вывод СКНФ
void ConjunctiveNormalForm::Print() const {
    std::cout << "Literals count: " << literalsCount << std::endl;
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

// получение значения литерала
TermValue ConjunctiveNormalForm::GetLiteralValue(int literal) const {
    size_t index = abs(literal) - 1;

    if (literal > 0 || values[index] == TermValue::Undefined)
        return values[index];

    return values[index] == TermValue::True ? TermValue::False : TermValue::True;
}

// является ли клауза единичной
bool ConjunctiveNormalForm::IsUnitClause(size_t index) const {
    int undefinedCount = 0; // количество неопределённых значений

    for (auto it = clauses[index].begin(); it != clauses[index].end() && undefinedCount < 2; it++)
        if (GetLiteralValue(*it) == TermValue::Undefined)
            undefinedCount++;

    return undefinedCount == 1; // ровно один неопределённый литерал
}

// удалена ли клауза (есть ли единичные литералы)
bool ConjunctiveNormalForm::IsRemovedClause(size_t index) const {
    for (auto it = clauses[index].begin(); it != clauses[index].end(); it++)
        if (GetLiteralValue(*it) == TermValue::True)
            return true;

    return false;
}

// пустая ли клауза
bool ConjunctiveNormalForm::IsEmptyClause(size_t index) const {
    for (auto it = clauses[index].begin(); it != clauses[index].end(); it++)
        if (GetLiteralValue(*it) != TermValue::False)
            return false;

    return true; // пуста, если все значения ложны
}

// получение литерала из единичной клаузы
int ConjunctiveNormalForm::GetUnitLiteral(size_t index) const {
    for (auto it = clauses[index].begin(); it != clauses[index].end(); it++)
        if (GetLiteralValue(*it) == TermValue::Undefined)
            return *it;

    throw std::string("GetUnitLiteral: clause is not unit");
}

// распространение констант
void ConjunctiveNormalForm::UnitPropagation(std::stack<int> &assignments) {
    bool findUnitClause = true;

    while (findUnitClause) {
        findUnitClause = false;

        for (size_t i = 0; i < clauses.size(); i++) {
            if (IsRemovedClause(i) || !IsUnitClause(i))
                continue;

            int literal = GetUnitLiteral(i);
            TermValue value = literal > 0 ? TermValue::True : TermValue::False;

            values[abs(literal) - 1] = value;
            assignments.push(literal);
            findUnitClause = true;
        }
    }
}

// все ли клаузы удалены
bool ConjunctiveNormalForm::IsSolve() const {
    for (size_t i = 0; i < clauses.size(); i++)
        if (!IsRemovedClause(i))
            return false;

    return true; // все клаузы удалены, решение
}

// есть ли пустые клаузы
bool ConjunctiveNormalForm::IsConflict() const {
    for (size_t i = 0; i < clauses.size(); i++)
        if (IsEmptyClause(i))
            return true; // конфликт

    return false; // пустых клауз нет
}

// откат
void ConjunctiveNormalForm::RollBack(std::stack<int> &assignments, std::stack<Assignment> &decisions) {
    // удаляем все присваивания, выполненные на последнем разделении
    while (assignments.top() != decisions.top().literal) {
        values[abs(assignments.top()) - 1] = TermValue::Undefined;
        assignments.pop();
    }

    Assignment &decision = decisions.top();

    if (decision.value == TermValue::False) { // сли это была ложная ветвь
        decision.value = TermValue::True; // заменяем на истинную ветвь
        values[abs(decision.literal) - 1] = TermValue::True;
    }
    else { // иначе попробовали оба варианта
        assignments.pop(); // извлекаем присваивание
        decisions.pop(); // извлекаем выбор
        values[abs(decision.literal) - 1] = TermValue::Undefined;

        if (decisions.size()) {
            RollBack(assignments, decisions);
        }
    }
}

// алгоритм DPLL
bool ConjunctiveNormalForm::DPLL() {
    std::stack<int> assignments;
    std::stack<Assignment> decisions;

    while (true) {
        UnitPropagation(assignments);

        if (IsConflict()) {
            RollBack(assignments, decisions);

            if (decisions.size() == 0) // если выбора больше нет
                return false; // то невыполнима

            continue;
        }

        if (IsSolve())
            return true;

        int index = 0;

        while (index < literalsCount && GetLiteralValue(index + 1) != TermValue::Undefined)
            index++;

        decisions.push({ index + 1, TermValue::False });
        assignments.push(index + 1);
        values[index] = TermValue::False;
    }
}