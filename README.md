# DPLL
## Features
* No pure literal assign
* Only unit propagation
* No recursive, uses decisions stack
* Different decision heuristics
* Preprocessing (remove duplicate clauses and subsumption)

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

### Flags:
* `-d` - remove duplicate clauses during reading (increase time, false for default)
* `-s` - use subsumption after read (increase time even more, false for default)

## Performance of DPLL SAT solver (time in ms)
| cnf \ strategy |     max |    moms |weighted |      up |   first |  random |
|       :-:      |     :-: |     :-: |     :-: |     :-: |     :-: |     :-: |
|          sat20 |   0.137 |   0.133 |   **0.131** |   0.141 |   0.145 |    0.142 |
|          sat50 |    1.02 | **0.743** |    0.762 |    2.15 |    2.79 |     3.34 |
|        unsat50 |    1.74 |  **1.15** |    1.21 |    5.04 |    8.17 |     8.7 |
|          sat75 |    5.04 |  **2.84** |    3.16 |    23.1 |    59.9 |    55.7 |
|        unsat75 |    11.0 |  **5.27** |    5.67 |    56.9 |     131 |     150 |
|         sat100 |    24.8 |  **8.78** |    10.4 |     176 |     770 |     837 |
|       unsat100 |    60.9 |  **21.3** |    25.0 |     562 | 2212 | 2431 |
|   pigeon-hole6 | **4.6** |    12.2 | **4.6** |     5.1 |     7.9 |     6.0 |
|   pigeon-hole7 | **41.8** |     168 |    42.0 |    68.6 |     111 |    77.4 |
|   pigeon-hole8 | **435** |    2811 |     452 |     935 |    1782 |    1243 |
|   pigeon-hole9 | **5351** | 37501 | 5263 | 14434 | 29918 | 18209 |
|    data/hanoi4 | **34405** | 2208730 |  1763837 |       ? |   55355 | 84937 |


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
