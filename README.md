# DPLL
## Features
* No pure literal assign
* Only unit propagation
* No recursive, uses decisions stack
* Different decision heuristics
* Preprocessing (remove duplicate clauses and subsumption)
* Watchlists for effective conflict checking

## Build
* For building dpll application run `make dpll`
* For building perofrmance test run `make test` and than `./test`

## Usage:
`./dpll path/to/cnf/file [strategy] [-d] [-s]`

### Decision strategies:
* `first` - get first undefined literal (selected by defalt)
* `random` - get random undefined literal
* `max` - get literal with max occurencies in clauses
* `moms` - get literal with max occurencies in clauses with minimal size
* `weighted` - get literal with max weighted sum (score of l = 2^-|clause with l|)
* `up` - get literal with max up value (up in unit propagation)
* `aupc` - get literal with max occurencies of clauses by size = 2

### Flags:
* `-d` - remove duplicate clauses during reading (increase time, false for default)
* `-s` - use subsumption after read (increase time even more, false for default)

## Performance of DPLL SAT solver (time in ms)
| cnf \ strategy |       max |     moms |  weighted |      aupc |        up |   first |  random |
|       :-:      |       :-: |      :-: |       :-: |       :-: |       :-: |     :-: |     :-: |
|          sat20 |      0.13 |    0.132 |     0.126 |     0.125 | **0.117** |    0.12 |   0.123 |
|          sat50 |     0.905 |    0.691 |     0.683 | **0.659** |      1.14 |    2.04 |    2.68 |
|        unsat50 |      1.51 | **1.04** |      1.06 |      1.12 |      1.97 |    5.77 |    6.42 |
|          sat75 |       4.5 |     2.65 |      2.77 |  **2.43** |      6.72 |    45.2 |    43.3 |
|        unsat75 |      9.65 | **4.95** |      5.15 |      5.22 |      12.8 |    95.2 |     112 |
|         sat100 |      22.7 | **8.54** |      9.62 |      8.94 |      29.2 |     567 |     614 |
|       unsat100 |        56 | **20.5** |      23.4 |      24.6 |      76.5 |    1661 |    1922 |
|   pigeon-hole6 |         5 |     12.2 |   **4.8** |      10.8 |       6.2 |     8.2 |     7.6 |
|   pigeon-hole7 |      45.8 |      165 |  **44.4** |       143 |      72.2 |     107 |    95.2 |
|   pigeon-hole8 |       466 |     2717 |   **454** |      2352 |      1053 |    1713 |    1383 |
|   pigeon-hole9 |      6081 |    36503 |  **5981** |     31412 |     15730 |   29432 |   22703 |
|         hanoi4 | **35235** |  2091134 |   2001715 |   2243821 |   3421203 |   76612 |       ? |


## Input file format
Solver work with cnf in <a href="https://people.sc.fsu.edu/~jburkardt/data/cnf/cnf.html">DIMACS</a> format:

* The file may begin with comment lines. The first character of each comment line must be a lower case letter "c".
* The comment lines are followed by the "problem" line in format "p" "cnf" "number of literals" "number of clauses".
* The remainder of the file contains lines defining the clauses, one by one.
* A clause is defined by listing the index of each positive literal, and the negative index of each negative literal.
* The definition of a clause may extend beyond a single line of text.
* The definition of a clause is terminated by a final value of "0".
* The file terminates after the last clause is defined.

### Example DIMACS cnf file
```
c cnf with 5 literals and 3 clauses
p cnf 5 3
1 -3 5 0
2 3 -1 0
-4 5 1 0
-3 0
1 -2 3 -4 5 0
0
```
