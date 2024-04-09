#include "alg_base.cpp"
#include "alg_base_del.cpp"
#include "alg_optimized.cpp"
#include "alg_optimized_del.cpp"

#include <fstream>
#include <chrono>
#include <cmath>

using namespace std;
using std::cout;

// alg_base
int upper = 3;
vector<pair<string, int>> tests = {
    // {"manual", 9},
    {"cycles", 12},
    // {"good_cycles", upper},
    // {"bad_cycles", upper},
    // {"philosophers_simple", upper},
    // {"philosophers", upper}
};

// // alg_optimized
// int upper = 14;
// vector<pair<string, int>> tests = {
//     // {"manual", 9},
//     {"cycles", 20},
//     {"good_cycles", 16},
//     {"bad_cycles", upper},
//     {"philosophers_simple", upper},
//     {"philosophers", upper}
// };

using alg_base::Node;

int main() {
    ofstream output ("./output_optimized_2.csv");
    for (auto test : tests) {
        string test_name = test.first;
        int test_num = test.second;
        output << test_name << ",";

        for (int test_i = 1; test_i < test_num + 1; ++test_i) {
            ifstream file;
            file.open("./tests/" + test_name + "/" + to_string(test_i), std::ios::in);
            
            string line;
            getline(file, line);
            cout << line << endl;
            int net_size = stoi(line);
            vector<Node> net (net_size);
            fori(net_size) {
                net[i] = Node{i, true, {}, {}};
            }

            istringstream line_stream;
            fori(net_size) {
                getline(file, line);
                line_stream = istringstream(line);
                
                int j;
                line_stream >> j;
                net[i].is_place = j;
                while (line_stream >> j) {
                    net[i].next.emplace(j);
                    net[j].prev.emplace(i);
                }
            }

            getline(file, line);
            int ans_correct = stoi(line);

            file.close();

            auto start = chrono::steady_clock::now();

            fori(10) int ans = alg(net_size, net);

            output << std::log2((float)(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() / 10) << ",";

        }

        output << "\n";

    }

    output.close();
}