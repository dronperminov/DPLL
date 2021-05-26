#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

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
    Up, // стратегия UP
    AUPC
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
    std::unordered_map<int, std::vector<int>> l2c;

    void SetLiteralsCount(int literalsCount); // обновление количества литералов
    void SetClausesCount(int clausesCount); // обновление количества клауз
    void AddClause(const std::string& line, bool removeDuplicates); // добавление клаузы
    void FillWatchLists(); // заполнение вотчлистов

    bool IsInclude(const std::vector<int> clause1, const std::vector<int> clause2) const; // проверка, что первое множество входит во второе
    void Subsumption(); // удаление клауз, содержащих меньшие клаузы

    TermValue GetLiteralValue(int literal) const; // получение значения литерала
    int GetClauseSize(size_t index) const; // получкение размера клаузы
    bool IsUnitClause(size_t index) const; // является ли клауза единичной
    bool IsRemovedClause(size_t index) const; // удалена ли клауза (есть ли единичные литералы)
    bool IsEmptyClause(size_t index) const; // пустая ли клауза

    int GetUnitLiteral(size_t index) const; // получение литерала из единичной клаузы
    void PropagateLiteral(size_t clause, std::stack<int> &assignments); // распространение константы
    bool UnitPropagation(std::stack<int> &assignments); // распространение констант

    bool IsConflict(int literal) const; // есть ли пустые клаузы

    int GetFirstUndefinedLiteral() const; // первый неопределённый литерал
    int GetRandomUndefinedLiteral() const; // случайный неопределённый литерал
    int GetMaxOccurencesLiteral() const; // литерал с наибольшим числом вхождений
    int GetMomsOccurencesLiteral() const; // литерал с наибольшим числом вхождений в кратчайшие клаузы
    int GetWeightedLiteral() const; // литерал по взвешенной сумме
    int GetUpLiteral() const; // литерал по стратегии Up
    int GetAUPCLiteral() const; // литерал по стратегии AUPC
    int GetDecisionLiteral(DecisionStrategy strategy) const; // выбор литерала для разветвления

    bool RollBack(std::stack<int> &assignments, std::stack<Assignment> &decisions); // откат
    void Decision(std::stack<int> &assignments, std::stack<Assignment> &decisions, DecisionStrategy strategy); // разветвление
public:
    ConjunctiveNormalForm(std::istream &fin, bool removeDuplicates = false, bool subsumption = false);

    void Print() const; // вывод СКНФ
    void PrintTermValues() const; // вывод значений термов

    bool DPLL(DecisionStrategy strategy); // алгоритм DPLL
};

// перевод стратегии в строку
std::string StrategyToString(DecisionStrategy strategy) {
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

    if (strategy == DecisionStrategy::AUPC)
        return "aupc";

    return "";
}

// получение стратегии
DecisionStrategy GetStrategy(const std::string& strategy) {
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

    if (strategy == "aupc")
        return DecisionStrategy::AUPC;

    throw std::string("Invalid strategy name '") + strategy + "'";
}

ConjunctiveNormalForm::ConjunctiveNormalForm(std::istream &fin, bool removeDuplicates, bool subsumption) {
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

        AddClause(line, removeDuplicates);
    }

    if (clauses.size() != clausesCount)
        throw std::string("Invalid file: different clauses count");

    values = std::vector<TermValue>(literalsCount + 1, TermValue::Undefined); // значения литералов не определены

    if (subsumption) {
        Subsumption();
    }

    for (int i = 1; i <= literalsCount; i++)
        up[i] = 0;

    FillWatchLists();
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
void ConjunctiveNormalForm::AddClause(const std::string& line, bool removeDuplicates) {
    std::stringstream ss(line);
    std::vector<int> clause;
    int literal;

    while (ss >> literal && literal != 0) {
        if (literal == 0 || abs(literal) > literalsCount)
            throw std::string("Invalid literal index at line '") + line + "'";

        clause.push_back(literal); // добавляем литералы в клаузу
    }

    if (removeDuplicates) {
        std::sort(clause.begin(), clause.end());

        if (std::find(clauses.begin(), clauses.end(), clause) == clauses.end()) {
            clauses.push_back(clause); // добавляем клаузу
        }
        else {
            clausesCount--;
        }
    }
    else {
        clauses.push_back(clause); // добавляем клаузу
    }
}

// заполнение вотчлистов
void ConjunctiveNormalForm::FillWatchLists() {
    for (int i = 1; i <= literalsCount; i++) {
        l2c[i] = std::vector<int>();
        l2c[-i] = std::vector<int>();
    }

    for (size_t i = 0; i < clauses.size(); i++)
        for (auto j = clauses[i].begin(); j != clauses[i].end(); j++)
            l2c[*j].push_back(i);
}

// проверка, что первое множество входит во второе
bool ConjunctiveNormalForm::IsInclude(const std::vector<int> clause1, const std::vector<int> clause2) const {
    if (clause1.size() > clause2.size())
        return false;

    for (auto it1 = clause1.begin(); it1 != clause1.end(); it1++)
        if (!std::binary_search(clause2.begin(), clause2.end(), *it1))
            return false;

    return true;
}

// удаление клауз, содержащих меньшие клаузы
void ConjunctiveNormalForm::Subsumption() {
    std::unordered_set<size_t> stays;

    for (size_t i = 0; i < clauses.size(); i++)
        stays.insert(i);

    for (size_t i = 0; i < clauses.size(); i++)
        for (size_t j = 0; j < clauses.size(); j++)
            if (i != j && IsInclude(clauses[i], clauses[j]))
                stays.erase(j);

    if (stays.size() == clauses.size())
        return;

    size_t index = 0;

    for (size_t i = 0; i < clauses.size(); i++)
        if (stays.find(i) != stays.end())
            clauses[index++] = clauses[i];

    clauses.resize(index);
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
bool ConjunctiveNormalForm::UnitPropagation(std::stack<int> &assignments) {
    for (size_t i = 0; i < clauses.size(); i++) {
        if (!IsRemovedClause(i) && IsUnitClause(i)) {
            PropagateLiteral(i, assignments);
            return true;
        }
    }

    return false;
}

// есть ли пустые клаузы
bool ConjunctiveNormalForm::IsConflict(int literal) const {
    for (auto i = l2c.at(literal).begin(); i != l2c.at(literal).end(); i++)
        if (IsEmptyClause(*i))
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

// литерал по стратегии AUPC
int ConjunctiveNormalForm::GetAUPCLiteral() const {
    std::unordered_map<int, int> counts;

    for (int i = 1; i <= literalsCount; i++)
        counts[i] = 0;

    for (size_t i = 0; i < clauses.size(); i++) {
        if (IsRemovedClause(i) || GetClauseSize(i) != 2)
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

    if (strategy == DecisionStrategy::AUPC)
        return GetAUPCLiteral();

    return GetFirstUndefinedLiteral();
}

// откат
bool ConjunctiveNormalForm::RollBack(std::stack<int> &assignments, std::stack<Assignment> &decisions) {
    while (decisions.size()) {
        // удаляем все присваивания, выполненные на последнем разделении
        while (assignments.top() != decisions.top().literal) {
            values[abs(assignments.top())] = TermValue::Undefined;
            assignments.pop();
        }

        Assignment &decision = decisions.top();

        if (decision.isFirst) { // сли это была первая ветвь
            decision.isFirst = false;
            values[abs(decision.literal)] = decision.value == TermValue::True ? TermValue::False : TermValue::True; // заменяем на противоположное;
            return true;
        }

        // иначе попробовали оба варианта
        values[abs(decision.literal)] = TermValue::Undefined; // сбрасываем переменную
        assignments.pop(); // извлекаем присваивание
        decisions.pop(); // извлекаем выбор
    }

    return false; // откатываться некуда
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
        if (!UnitPropagation(assignments)) // распространяем единичные литералы
            Decision(assignments, decisions, strategy); // разветвляемся

        if (IsConflict(-assignments.top())) { // если конфликт
            if (!RollBack(assignments, decisions)) // если откатываться стало некуда
                return false; // невыполнима
        }
        else if (assignments.size() == literalsCount) { // если решение
            return true; // то выполнима
        }
    }
}