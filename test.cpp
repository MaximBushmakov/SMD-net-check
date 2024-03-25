#include <iostream>
#include <fstream>

#include "alg.cpp"

using namespace std;

const int test_num = 7;
int main() {
    ifstream file;
    for (int test_i = 0; test_i < test_num; ++test_i) {
        file.open("./tests/" + to_string(test_i), std::ios::in);
        
        string line;
        getline(file, line);
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