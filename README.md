# Description
C++ program to convert epsilon NFA to equivalent NFA & DFA.

# Program input
a file consists on a quintuple for a specifc e-nfa, the file structure as follows:
- 1st line ==> Q (set of all posible states seprated by spaces).
- 2nd line ==> language symbols.
- 3rd line ==> initial state or start state.
- 4th line ==> F (set of all final states or accepted states seprated by spaces).
- the rest is the transition function ==> [state] [input symbol] [next state].
