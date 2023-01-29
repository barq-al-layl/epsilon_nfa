#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>
#include <set>
#include <map>

#define INPUT_FILE_NAME "e_nfa.txt"
#define EPSILON '-'

using namespace std;

class DFA {
    set<char> symbols;
    set<string> states, finalStates;
    map<pair<string, char>, string> transitions;
    string initialState;

public:
    DFA(
            const set<char> &symbols,
            const set<string> &states,
            const set<string> &finalStates,
            const map<pair<string, char>, string> &transitions,
            string initialState
    ) :
            symbols(symbols),
            states(states),
            finalStates(finalStates),
            transitions(transitions),
            initialState(std::move(initialState)) {}

    void showQuintuple() {
        cout << "-------- DFA Quintuple --------\n";
        cout << "Q ==> { ";
        for (const string &state: states) {
            cout << state << ' ';
        }
        cout << "}\n";
        cout << "initial state ==> " << initialState << '\n';
        cout << "final states ==> { ";
        for (const string &state: finalStates) {
            cout << state << ' ';
        }
        cout << "}\n";
        cout << "transition function ==>\n";
        for (auto [key, value]: transitions) {
            cout << key.first << "\t\t" << key.second << "\t->\t" << value << '\n';
        }
        cout << "-------------------------------\n";
    }
};

class NFA {
    set<char> symbols;
    set<string> states, finalStates;
    map<pair<string, char>, set<string>> transitions;
    string initialState;

public:
    NFA(
            const set<char> &symbols,
            const set<string> &states,
            const set<string> &finalStates,
            const map<pair<string, char>, set<string>> &transitions,
            string initialState
    ) :
            symbols(symbols),
            states(states),
            finalStates(finalStates),
            transitions(transitions),
            initialState(std::move(initialState)) {}

    DFA toDFA() {
        set<string> dfaStates, dfaFinalStates;
        map<pair<string, char>, string> dfaTransitions;
        string state;
        dfaStates.insert(states.begin(), states.end());
        for (auto [key, value]: transitions) {
            if (value.size() < 2) continue;
            state = "";
            for (const string &str: value)
                state += str;
            dfaStates.insert(state);
        }
        for (const string &fs: finalStates) {
            for (const string &ds: dfaStates) {
                if (ds.contains(fs))
                    dfaFinalStates.insert(ds);
            }
        }
        pair<string, char> k1, k2;
        for (char symbol: symbols) {
            for (const string &dfaState: dfaStates) {
                k1 = make_pair(dfaState, symbol);
                state = "";
                for (const string &str: transitions[k1]) {
                    state += str;
                }
                if (state.empty()) {
                    for (auto [k, values]: transitions) {
                        if (k.second != symbol) continue;
                        for (const string &value: values) {
                            k2 = make_pair(value, symbol);
                            for (const string &s: transitions[k2]) {
                                if (state.contains(s)) continue;
                                state += s;
                            }
                        }
                    }
                }
                dfaTransitions[k1] = state;
            }
        }
        return DFA{
                symbols, dfaStates, dfaFinalStates,
                dfaTransitions, initialState
        };
    }

    void showQuintuple() {
        cout << "-------- NFA Quintuple --------\n";
        cout << "Q ==> { ";
        for (const string &state: states) {
            cout << state << ' ';
        }
        cout << "}\n";
        cout << "initial state ==> " << initialState << '\n';
        cout << "final states ==> { ";
        for (const string &state: finalStates) {
            cout << state << ' ';
        }
        cout << "}\n";
        cout << "transition function ==>\n";
        for (auto [key, value]: transitions) {
            cout << key.first << "\t" << key.second << "\t->\t";
            for (const string &v: value) {
                cout << v << ' ';
            }
            cout << '\n';
        }
        cout << "-------------------------------\n";
    }
};

class EpsilonNfa {
    set<string> states;
    set<char> symbols;
    string initialState;
    set<string> finalStates;
    map<pair<string, char>, set<string>> transitions;

    void epsilonClosure(const string &state, set<string> &closure) {
        closure.insert(state);
        pair<string, char> key = make_pair(state, EPSILON);
        if (!transitions.contains(key))
            return;
        for (const string &x: transitions[key]) {
            closure.insert(x);
            epsilonClosure(x, closure);
        }
    }

    set<string> statesAtInput(const string &state, char input) {
        set<string> atEpsilon;
        epsilonClosure(state, atEpsilon);
        atEpsilon.insert(state);
        pair<string, char> key;
        set<string> atInput;
        for (const string &epsilon: atEpsilon) {
            key = make_pair(epsilon, input);
            for (const string &x: transitions[key]) {
                atInput.insert(x);
            }
        }
        set<string> result;
        for (const string &x: atInput) {
            epsilonClosure(x, result);
            result.insert(x);
        }
        return result;
    }

public:
    EpsilonNfa() {
        ifstream file(INPUT_FILE_NAME);
        string line, x;

        // reading set of states.
        getline(file, line);
        istringstream stringStream(line);
        while (stringStream >> x) {
            states.insert(x);
        }

        // reading input symbols.
        getline(file, line);
        for (char symbol: line) {
            symbols.insert(symbol);
        }

        // reading the initial state.
        getline(file, line);
        initialState = line;

        // reading set of final states (accepted states).
        getline(file, line);
        stringStream = istringstream(line);
        while (stringStream >> x) {
            finalStates.insert(x);
        }

        // reading transition function.
        string currentState, nextState;
        char inputSymbol;
        pair<string, char> key;
        while (getline(file, line)) {
            stringStream = istringstream(line);
            stringStream >> currentState >> inputSymbol;
            key = make_pair(currentState, inputSymbol);
            while (stringStream >> nextState) {
                transitions[key].insert(nextState);
            }
        }
    }

    NFA toNFA() {
        pair<string, char> key;
        map<pair<string, char>, set<string>> nfaTransitions;
        set<string> nfaFinalStates, closures, temp, temp2;
        for (const string &state: states) {
            closures = set<string>();
            epsilonClosure(state, closures);
            for (const string &finaleState: finalStates) {
                if (closures.contains(finaleState)) {
                    nfaFinalStates.insert(finaleState);
                }
            }
            for (const string &closure: closures) {
                for (char symbol: symbols) {
                    key = make_pair(closure, symbol);
                    temp = statesAtInput(closure, symbol);
                    for (const string &x: temp) {
                        temp2 = set<string>();
                        epsilonClosure(x, temp2);
                        nfaTransitions[key].insert(temp2.begin(), temp2.end());
                    }
                }
            }
        }
        return NFA{
                symbols, states, nfaFinalStates,
                nfaTransitions, initialState
        };
    }

    void showQuintuple() {
        cout << "------- E-NFA Quintuple -------\n";
        cout << "Q ==> { ";
        for (const string &state: states) {
            cout << state << ' ';
        }
        cout << "}\n";
        cout << "initial state ==> " << initialState << '\n';
        cout << "final states ==> { ";
        for (const string &state: finalStates) {
            cout << state << ' ';
        }
        cout << "}\n";
        cout << "transition function ==>\n";
        for (auto [key, value]: transitions) {
            cout << key.first << "\t" << key.second << "\t->\t";
            for (const string &v: value) {
                cout << v << ' ';
            }
            cout << '\n';
        }
        cout << "-------------------------------\n";
    }

    void epsilonClosures() {
        cout << "--------- E Closures ----------\n";
        set<string> closure;
        for (const string &state: states) {
            closure = set<string>();
            epsilonClosure(state, closure);
            cout << state << "\t==>\t\t";
            for (const string &x: closure)
                cout << x << ' ';
            cout << '\n';
        }
        cout << "-------------------------------\n";
    };
};

int main() {
    auto epsilonNfa = EpsilonNfa();
    epsilonNfa.showQuintuple();
    auto nfa = epsilonNfa.toNFA();
    nfa.showQuintuple();
    auto dfa = nfa.toDFA();
    dfa.showQuintuple();
    epsilonNfa.epsilonClosures();
}
