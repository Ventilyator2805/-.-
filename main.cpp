#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace std;

struct Expense {
    string name;
    double amount;
    set<string> excluded;
};

static string trim(const string& s) {
    size_t left = s.find_first_not_of(" \t\r\n");
    if (left == string::npos) return "";
    size_t right = s.find_last_not_of(" \t\r\n");
    return s.substr(left, right - left + 1);
}

vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        token = trim(token);
        if (!token.empty()) tokens.push_back(token);
    }
    return tokens;
}

void solve() {
    ifstream input("input.txt");
    ofstream output("output.txt");

    if (!input.is_open()) {
        cerr << "File not found!" << endl;
        return;
    }

    string line;
    vector<string> participants;
    vector<Expense> expenses;

    if (!getline(input, line)) return;
    line = trim(line);

    istringstream iss(line);
    int count = 0;
    iss >> count;

    string name;
    while (iss >> name) {
        participants.push_back(name);
    }

    while (getline(input, line)) {
        if (line.find_first_not_of(" \t\r\n") == string::npos) continue;

        Expense exp{};
        exp.amount = 0.0;

        size_t slash_pos = line.find('/');

        if (slash_pos != string::npos) {
            string before_slash = trim(line.substr(0, slash_pos));
            string after_slash = trim(line.substr(slash_pos + 1));

            size_t colon_pos = before_slash.find(':');
            if (colon_pos == string::npos) {
                istringstream line_stream(before_slash);
                line_stream >> exp.name >> exp.amount;
            } else {
                exp.name = trim(before_slash.substr(0, colon_pos));
                string amount_str = trim(before_slash.substr(colon_pos + 1));
                exp.amount = stod(amount_str);
            }

            vector<string> excluded_names = split(after_slash, ',');
            for (const string& excluded_name : excluded_names) {
                exp.excluded.insert(excluded_name);
            }
        } else {
            size_t colon_pos = line.find(':');
            if (colon_pos != string::npos) {
                exp.name = trim(line.substr(0, colon_pos));
                string amount_str = trim(line.substr(colon_pos + 1));
                exp.amount = stod(amount_str);
            } else {
                istringstream line_stream(line);
                line_stream >> exp.name >> exp.amount;
            }
        }

        expenses.push_back(exp);
    }

    input.close();

    map<string, double> total_expenses;
    map<string, double> should_pay;
    map<string, double> balance;

    for (const string& p : participants) {
        total_expenses[p] = 0.0;
        should_pay[p] = 0.0;
        balance[p] = 0.0;
    }

    for (const Expense& exp : expenses) {
        total_expenses[exp.name] += exp.amount;
    }

    for (const Expense& exp : expenses) {
        vector<string> included;
        for (const string& p : participants) {
            if (exp.excluded.find(p) == exp.excluded.end()) {
                included.push_back(p);
            }
        }

        if (included.empty()) continue;

        double share = exp.amount / included.size();
        for (const string& p : included) {
            should_pay[p] += share;
        }
    }

    output << fixed << setprecision(1);

    for (const string& p : participants) {
        output << p << " " << total_expenses[p] << " (из них " << should_pay[p] << ")" << endl;
        balance[p] = total_expenses[p] - should_pay[p];
    }

    vector<pair<string, double>> creditors;
    vector<pair<string, double>> debtors;

    for (const string& p : participants) {
        if (balance[p] > 1e-9) {
            creditors.push_back({p, balance[p]});
        } else if (balance[p] < -1e-9) {
            debtors.push_back({p, -balance[p]});
        }
    }

    sort(creditors.begin(), creditors.end(),
         [](const pair<string, double>& a, const pair<string, double>& b) {
             return a.second > b.second;
         });

    sort(debtors.begin(), debtors.end(),
         [](const pair<string, double>& a, const pair<string, double>& b) {
             return a.second > b.second;
         });

    size_t i = 0, j = 0;
    while (i < debtors.size() && j < creditors.size()) {
        double amount = min(debtors[i].second, creditors[j].second);

        if (amount > 1e-9) {
            output << debtors[i].first << " " << amount << " " << creditors[j].first << endl;
        }

        debtors[i].second -= amount;
        creditors[j].second -= amount;

        if (debtors[i].second < 1e-9) ++i;
        if (creditors[j].second < 1e-9) ++j;
    }

    output.close();
}

int main() {
    solve();
    return 0;
}
