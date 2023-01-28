#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <map>

#define INPUT_FILE_NAME "e_nfa.txt"

using namespace std;

class EpsilonNfa {
    set<string> states;
    set<char> symbols;
    string initialState;
    set<string> finalStates;
    map<pair<string, char>, set<string>> transitions;
    string currentState;

private:
    void readFile() {
        ifstream file(INPUT_FILE_NAME);
        string line, temp;

        // reading set of states.
        getline(file, line);
        istringstream stringStream(line);
        while (stringStream >> temp) {
            states.insert(temp);
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
        while (stringStream >> temp) {
            finalStates.insert(temp);
        }

        // reading transition function.
        string nextState;
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

public:
    EpsilonNfa() {
        readFile();
    }

    set<string> getEpsilonClosure(const string &state, char input) {
        set<string> atEpsilon;
        atEpsilon = epsilon(state, atEpsilon);
        atEpsilon.insert(state);
        pair<string, char> key;
        set<string> atInput;
        cout << "----- Epsilon -----" << endl;
        for (const string &str1: atEpsilon) {
            key = make_pair(str1, input);
            for (const string &str2: transitions[key]) {
                cout << str2 << endl;
                atInput.insert(str2);
            }
        }
        cout << "----- ------ -----" << endl;

        set<string> result;
        cout << "----- Input -----" << endl;
        for (const string &str: atInput) {
            cout << str << endl;
            result = epsilon(str, result);
            result.insert(str);
        }
        cout << "----- ----- -----" << endl;
        return result;
    }

    set<string> epsilon(const string &state, set<string> &epsilonStates) {
        pair<string, char> key = make_pair(state, '-');
        if (transitions.find(key) == transitions.end())
            return epsilonStates;
        for (const string &s: transitions[key]) {
            epsilonStates.insert(s);
            return epsilon(s, epsilonStates);
        }
        return epsilonStates;
    }

    void showQuintuple() {
        cout << "Q: ";
        for (const string &state: states) {
            cout << state << ' ';
        }
        cout << "\ninitial state = " << initialState << '\n';
        cout << "final states: ";
        for (const string &state: finalStates) {
            cout << state << ' ';
        }
        cout << "\ntransition function:\n";
        for (auto [key, value]: transitions) {
            cout << key.first << ", " << key.second << " -> ";
            for (auto v: value) {
                cout << v << ' ';
            }
            cout << endl;
        }
    }
};

int main() {
    auto fa = EpsilonNfa();
    fa.showQuintuple();
    set<string> s = fa.getEpsilonClosure("C", '1');
    cout << "--- "<< s.size() << " ---\n";
    for (const auto &state: s) {
        cout << state << endl;
    }
}
