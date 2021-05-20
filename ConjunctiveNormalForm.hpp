#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <stack>
#include <string>
#include <unordered_map>

// значения термов
enum class TermValue {
    Undefined,
    True,
    False
};

// стратегия выбора литерала
enum class DecisionStrategy {
    First, // первый неопределённый
    Random, // случайный
    Max, // максимальное число вхождений в клаузы
    Moms, // вхождения в минимальные клаузы
    Weighted, // взвешенная сумму
    Up // стратегия UP
};

struct Assignment {
    int literal;
    bool isFirst;
    TermValue value;
};

class ConjunctiveNormalForm {
    int literalsCount; // количество литералов
    int clausesCount; // количество клауз
    std::vector<std::vector<int>> clauses; // клаузы
    std::vector<TermValue> values; // значения термов
    std::unordered_map<int, int> up;

    void SetLiteralsCount(int literalsCount); // обновление количества литералов
    void SetClausesCount(int clausesCount); // обновление количества клауз
    void AddClause(const std::string& line); // добавление клаузы

    TermValue GetLiteralValue(int literal) const; // получение значения литерала
    int GetClauseSize(size_t index) const; // получкение размера клаузы
    bool IsUnitClause(size_t index) const; // является ли клауза единичной
    bool IsRemovedClause(size_t index) const; // удалена ли клауза (есть ли единичные литералы)
    bool IsEmptyClause(size_t index) const; // пустая ли клауза

    int GetUnitLiteral(size_t index) const; // получение литерала из единичной клаузы
    void PropagateLiteral(size_t clause, std::stack<int> &assignments); // распространение константы
    void UnitPropagation(std::stack<int> &assignments); // распространение констант

    bool IsSolve() const; // все ли клаузы удалены
    bool IsConflict() const; // есть ли пустые клаузы

    int GetFirstUndefinedLiteral() const; // первый неопределённый литерал
    int GetRandomUndefinedLiteral() const; // случайный неопределённый литерал
    int GetMaxOccurencesLiteral() const; // литерал с наибольшим числом вхождений
    int GetMomsOccurencesLiteral() const; // литерал с наибольшим числом вхождений в кратчайшие клаузы
    int GetWeightedLiteral() const; // литерал по взвешенной сумме
    int GetUpLiteral() const; // литерал по стратегии Up
    int GetDecisionLiteral(DecisionStrategy strategy) const; // выбор литерала для разветвления

    void RollBack(std::stack<int> &assignments, std::stack<Assignment> &decisions); // откат
    void Decision(std::stack<int> &assignments, std::stack<Assignment> &decisions, DecisionStrategy strategy); // разветвление
public:
    ConjunctiveNormalForm(std::istream &fin);

    void Print() const; // вывод СКНФ
    void PrintTermValues() const; // вывод значений термов

    bool DPLL(DecisionStrategy strategy); // алгоритм DPLL
};

ConjunctiveNormalForm::ConjunctiveNormalForm(std::istream &fin) {
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

    values = std::vector<TermValue>(literalsCount + 1, TermValue::Undefined); // значения литералов не определены

    for (int i = 1; i <= literalsCount; i++)
        up[i] = 0;
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
    std::vector<int> clause;
    int literal;

    while (ss >> literal && literal != 0) {
        if (literal == 0 || abs(literal) > literalsCount)
            throw std::string("Invalid literal index at line '") + line + "'";

        clause.push_back(literal); // считываем клаузы
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

    for (size_t i = 1; i <= values.size(); i++) {
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
    size_t index = abs(literal);

    if (literal > 0 || values[index] == TermValue::Undefined)
        return values[index];

    return values[index] == TermValue::True ? TermValue::False : TermValue::True;
}

// получкение размера клаузы
int ConjunctiveNormalForm::GetClauseSize(size_t index) const {
    int size = 0;

    for (auto it = clauses[index].begin(); it != clauses[index].end(); it++)
        if (values[abs(*it)] == TermValue::Undefined)
            size++;

    return size;
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

// распространение константы
void ConjunctiveNormalForm::PropagateLiteral(size_t clause, std::stack<int> &assignments) {
    int literal = GetUnitLiteral(clause);
    TermValue value = literal > 0 ? TermValue::True : TermValue::False;
    up[abs(literal)]++;

    values[abs(literal)] = value;
    assignments.push(literal);
}

// распространение констант
void ConjunctiveNormalForm::UnitPropagation(std::stack<int> &assignments) {
    bool findUnitClause = true;

    while (findUnitClause) {
        findUnitClause = false;

        for (size_t i = 0; i < clauses.size(); i++) {
            if (IsRemovedClause(i) || !IsUnitClause(i))
                continue;

            PropagateLiteral(i, assignments);
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

// первый неопределённый литерал
int ConjunctiveNormalForm::GetFirstUndefinedLiteral() const {
    for (int i = 1; i <= literalsCount; i++)
        if (values[i] == TermValue::Undefined)
            return i;

    return 0; // нет неопределённых литералов
}

// случайный неопределённый литерал
int ConjunctiveNormalForm::GetRandomUndefinedLiteral() const {
    std::vector<int> undefinedLiterals;

    for (int i = 1; i <= literalsCount; i++)
        if (values[i] == TermValue::Undefined)
            undefinedLiterals.push_back(i);

    return undefinedLiterals[rand() % undefinedLiterals.size()];
}

// литерал с наибольшим числом вхождений
int ConjunctiveNormalForm::GetMaxOccurencesLiteral() const {
    std::unordered_map<int, int> counts;

    for (int i = 1; i <= literalsCount; i++)
        counts[i] = 0;

    for (size_t i = 0; i < clauses.size(); i++) {
        if (IsRemovedClause(i))
            continue;

        for (auto j = clauses[i].begin(); j != clauses[i].end(); j++) {
            counts[abs(*j)]++;
        }
    }

    int literal = 0;

    for (auto it = counts.begin(); it != counts.end(); it++) {
        if (values[abs(it->first)] != TermValue::Undefined)
            continue;

        if (literal == 0 || it->second > counts[literal])
            literal = it->first;
    }

    return literal;
}

// литерал с наибольшим числом вхождений в кратчайшие клаузы
int ConjunctiveNormalForm::GetMomsOccurencesLiteral() const {
    int minLength = literalsCount;

    for (size_t i = 0; i < clauses.size(); i++) {
        int size = GetClauseSize(i);

        if (!IsRemovedClause(i) && size < minLength)
            minLength = size;
    }

    std::unordered_map<int, int> counts;

    for (int i = 1; i <= literalsCount; i++)
        counts[i] = 0;

    for (size_t i = 0; i < clauses.size(); i++) {
        if (IsRemovedClause(i) || GetClauseSize(i) != minLength)
            continue;

        for (auto j = clauses[i].begin(); j != clauses[i].end(); j++) {
            counts[abs(*j)]++;
        }
    }

    int literal = 0;

    for (auto it = counts.begin(); it != counts.end(); it++) {
        if (values[abs(it->first)] != TermValue::Undefined)
            continue;

        if (literal == 0 || it->second > counts[literal])
            literal = it->first;
    }

    return literal;
}

// литерал по взвешенной сумме
int ConjunctiveNormalForm::GetWeightedLiteral() const {
    std::unordered_map<int, double> weights;

    for (int i = 1; i <= literalsCount; i++)
        weights[i] = 0;

    for (size_t i = 0; i < clauses.size(); i++) {
        if (IsRemovedClause(i))
            continue;

        double weight = 1.0 / (1 << GetClauseSize(i));

        for (auto j = clauses[i].begin(); j != clauses[i].end(); j++) {
            weights[abs(*j)] += weight;
        }
    }

    int literal = 0;

    for (auto it = weights.begin(); it != weights.end(); it++) {
        if (values[abs(it->first)] != TermValue::Undefined)
            continue;

        if (literal == 0 || it->second > weights[literal])
            literal = it->first;
    }

    return literal;
}

// литерал по стратегии Up
int ConjunctiveNormalForm::GetUpLiteral() const {
    int literal = 0;

    for (auto it = up.begin(); it != up.end(); it++) {
        if (GetLiteralValue(it->first) != TermValue::Undefined)
            continue;

        if (literal == 0 || it->second > up.at(literal))
            literal = it->first;
    }

    return literal;
}

// выбор литерала для разветвления
int ConjunctiveNormalForm::GetDecisionLiteral(DecisionStrategy strategy) const {
    if (strategy == DecisionStrategy::First)
        return GetFirstUndefinedLiteral();

    if (strategy == DecisionStrategy::Random)
        return GetRandomUndefinedLiteral();

    if (strategy == DecisionStrategy::Max)
        return GetMaxOccurencesLiteral();

    if (strategy == DecisionStrategy::Moms)
        return GetMomsOccurencesLiteral();

    if (strategy == DecisionStrategy::Weighted)
        return GetWeightedLiteral();

    if (strategy == DecisionStrategy::Up)
        return GetUpLiteral();

    return GetFirstUndefinedLiteral();
}

// откат
void ConjunctiveNormalForm::RollBack(std::stack<int> &assignments, std::stack<Assignment> &decisions) {
    do {
        // удаляем все присваивания, выполненные на последнем разделении
        while (assignments.top() != decisions.top().literal) {
            values[abs(assignments.top())] = TermValue::Undefined;
            assignments.pop();
        }

        Assignment &decision = decisions.top();

        if (decision.isFirst) { // сли это была первая ветвь
            decision.isFirst = false;
            values[abs(decision.literal)] = decision.value == TermValue::True ? TermValue::False : TermValue::True; // заменяем на противоположное;
            return;
        }

        // иначе попробовали оба варианта
        values[abs(decision.literal)] = TermValue::Undefined; // сбрасываем переменную
        assignments.pop(); // извлекаем присваивание
        decisions.pop(); // извлекаем выбор
    } while (decisions.size());
}

// разветвление
void ConjunctiveNormalForm::Decision(std::stack<int> &assignments, std::stack<Assignment> &decisions, DecisionStrategy strategy) {
    int literal = GetDecisionLiteral(strategy);
    TermValue value = literal > 0 ? TermValue::True : TermValue::False;

    decisions.push({ literal, true, value });
    assignments.push(literal);
    values[abs(literal)] = value;
}

// алгоритм DPLL
bool ConjunctiveNormalForm::DPLL(DecisionStrategy strategy) {
    std::stack<int> assignments;
    std::stack<Assignment> decisions;

    while (true) {
        UnitPropagation(assignments); // распространяем единичные литералы

        if (IsConflict()) { // если конфликт
            RollBack(assignments, decisions); // откатываемся

            if (decisions.size() == 0) // если выбора больше нет
                return false; // то невыполнима

            continue; // пытаемся посмотреть дальше
        }

        if (IsSolve()) // если решение
            return true; // то выполнима

        Decision(assignments, decisions, strategy); // разветвляемся
    }
}