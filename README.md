# DPLL
## Features
* No pure literal assign
* Only unit propagation
* No recursive, uses decisions stack
* Different decision heuristics

## Build
* For building dpll application run `make dpll`
* For building perofrmance test run `make test` and than `./test`

## Usage:
`./dpll [path/to/cnf/file] [strategy]`

## Decision strategies:
* `first` - get first undefined literal (selected by defalt)
* `random` - get random undefined literal
* `max` - get literal with max occurencies in clauses
* `moms` - get literal with max occurencies in clauses with minimal size
* `weighted` - get literal with max weighted sum (score of l = 2^-|clause with l|)
* `up` - get literal with max up value (up in unit propagation)


## Performance of DPLL SAT solver (time in ms)
|  cnf \ strategy |     max |    moms |weighted |      up |   first |  random |
|        :-:      |     :-: |     :-: |     :-: |     :-: |     :-: |     :-: |
|      data/sat20 |   **0.126** |   0.134 |   **0.126** |   0.126 |   0.128 |    0.13 |
|      data/sat50 |    **1.03** |    1.12 |    **1.03** |    2.41 |    2.97 |     3.6 |
|    data/unsat50 |    **1.85** |    2.02 |    1.87 |    5.57 |    8.94 |    9.67 |
|      data/sat75 |    5.55 |    6.21 |    **5.53** |    23.1 |    68.4 |    65.8 |
|    data/unsat75 |    **12.3** |    13.4 |    **12.3** |    64.4 |     150 |     172 |
|     data/sat100 |    **27.9** |    30.8 |    28.2 |     205 |     897 |     933 |
|   data/unsat100 |    71.3 |    76.1 |    **70.1** |     651 | 2570 | 2810 |
|     data/hanoi4 |   47123 | 5770912 | 15910293 |       ? |   72238 | 84937 |


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
