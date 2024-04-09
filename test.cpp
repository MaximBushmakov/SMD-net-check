#include "alg_base.cpp"
#include "alg_base_del.cpp"
#include "alg_optimized.cpp"
#include "alg_optimized_del.cpp"

#include <fstream>

using namespace std;
using std::cout;

using alg_optimized_del::Node;

int test_num = 14;
int test_begin = 10;
int test_end = 15;

int main() {
    ifstream file;
    if (test_num != -1) {
        test_begin = test_num;
        test_end = test_num + 1;
    }
    for (int test_i = test_begin; test_i < test_end; ++test_i) {
        file.open("./tests/bad_cycles/" + to_string(test_i), std::ios::in);
        
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
        cout << line << endl;
        int ans_correct = stoi(line);

        file.close();

        int ans = alg(net_size, net);

        if (ans == -2) {
            cout << "Test " + to_string(test_i) + ": Incorrect test" << endl;
            break;
        }

        if (ans_correct == 1 && ans == -1 || ans_correct == 0 && ans >= 0) {
            cout << "Test " + to_string(test_i) + ": Success" << endl;
        } else {
            cout << "Test " + to_string(test_i) + ": Fail" << endl;
            cout << "Correct: " + to_string(ans_correct) << endl;
            cout << "Given: " + to_string(ans) << endl;
            break;
        }
    }
}

// int main() {
//     string line;
//     getline(cin, line);
//     int net_size = stoi(line);
//     vector<Node> net (net_size);
//     fori(net_size) {
//         net[i] = Node{i, true, {}, {}};
//     }

//     istringstream line_stream;
//     fori(net_size) {
//         getline(cin, line);
//         line_stream = istringstream(line);
        
//         int j;
//         line_stream >> j;
//         net[i].is_place = j;
//         while (line_stream >> j) {
//             net[i].next.push_back(j);
//             net[j].prev.push_back(i);
//         }
//     }

//     int  ans = alg(net_size, net);

//     if (ans == -1) {
//         cout << "It's SMD" << endl;
//     } else {
//         cout << "Place " << ans << " is not a part of any component" << endl;
//     }
    
// }