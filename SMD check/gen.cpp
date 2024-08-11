#include <iostream>
#include <fstream>

using namespace std;

// generate complete dining philosophers problem
// generate it for 2^k philosophers, 1 <= k <= n
// there will be 4 * 2^k places and 3 * 2^k transitions
// all computations are local so time will be linear
void gen_philosophers(int n) {
    ofstream file;
    int phils = 1;
    for (int k = 1; k <= n; ++k) {
        phils *= 2;
        file.open("./tests/philosophers/" + to_string(k));
        file << 7 * phils << "\n";

        file << 1 << " " << 7 * (phils - 1) + 3 << " " << 1 << "\n";
        file << 0 << " " << 2 << "\n";
        file << 1 << " " << 3 << "\n";
        file << 0 << " " << 4 << "\n";
        file << 1 << " " << 5 << "\n";
        file << 0 << " " << 0 << " " << 6 << " " << 7 << "\n";
        file << 1 << " " << 1 << "\n";

        for (int i = 1; i < phils - 1; ++i) {
            file << 1 << " " << 7 * (i - 1) + 3 << " " << 7 * i + 1 << "\n";
            file << 0 << " " << 7 * i + 2 << "\n";
            file << 1 << " " << 7 * i + 3 << "\n";
            file << 0 << " " << 7 * i + 4 << "\n";
            file << 1 << " " << 7 * i + 5 << "\n";
            file << 0 << " " << 7 * i + 0 << " " << 7 * i + 6 << " " << 7 * (i + 1) << "\n";
            file << 1 << " " << 7 * i + 1 << "\n";
        }

        file << 1 << " " << 7 * (phils - 2) + 3 << " " << 7 * (phils - 1) + 1 << "\n";
        file << 0 << " " << 7 * (phils - 1) + 2 << "\n";
        file << 1 << " " << 7 * (phils - 1) + 3 << "\n";
        file << 0 << " " << 7 * (phils - 1) + 4 << "\n";
        file << 1 << " " << 7 * (phils - 1) + 5 << "\n";
        file << 0 << " " << 7 * (phils - 1) + 0 << " " << 7 * (phils - 1) + 6 << " " << 0 << "\n";
        file << 1 << " " << 7 * (phils - 1) + 1 << "\n";

        file << 1 << endl;

        file.close();
    }
}

// genereate simplified version of dining philosophers problem
// generate it for 2^k philosophers, 1 <= k <= n
// there will be 2 * 2^k places and 2 * 2^k transitions
// all is local
void gen_philosophers_simple(int n) {
    ofstream file;
    int phils = 1;
    for (int k = 1; k <= n; ++k) {
        phils *= 2;
        file.open("./tests/philosophers_simple/" + to_string(k));
        file << 4 * phils << "\n";

        file << 1 << " " << 4 * (phils - 1) + 1 << " " << 1 << "\n";
        file << 0 << " " << 2 << "\n";
        file << 1 << " " << 3 << "\n";
        file << 0 << " " << 0 << " " << 4 << "\n";

        for (int i = 1; i < phils - 1; ++i) {
            file << 1 << " " << 4 * (i - 1) + 3 << " " << 4 * i + 1 << "\n";
            file << 0 << " " << 4 * i + 2 << "\n";
            file << 1 << " " << 4 * i + 3 << "\n";
            file << 0 << " " << 4 * i + 0 << " " << 4 * (i + 1) << "\n";
        }

        file << 1 << " " << 4 * (phils - 2) + 3 << " " << 4 * (phils - 1) + 1 << "\n";
        file << 0 << " " << 4 * (phils - 1) + 2 << "\n";
        file << 1 << " " << 4 * (phils - 1) + 3 << "\n";
        file << 0 << " " << 4 * (phils - 1) + 0 << " " << " " << 0 << "\n";

        file << 1 << endl;

        file.close();
    }
}

void gen_cycles(int n) {
    ofstream file;
    int len = 1;
    for (int k = 1; k <= n; ++k) {
        len *= 2;
        file.open("./tests/cycles/" + to_string(k));
        file << 2 * len << "\n";

        for (int i = 0; i < len - 1; ++i) {
            file << 1 << " " << 2 * i + 1 << "\n";
            file << 0 << " " << 2 * (i + 1) << "\n";
        }

        file << 1 << " " << 2 * (len - 1) + 1 << "\n";
        file << 0 << " " << 0 << "\n";

        file << 1 << endl;

        file.close();
    }
}

void gen_good_cycles(int n) {
    ofstream file;
    int len = 1;
    for (int k = 1; k <= n; ++k) {
        len *= 2;
        file.open("./tests/good_cycles/" + to_string(k));
        file << 3 * len << "\n";

        file << 1 << " " << 1 << "\n";
        file << 0 << " " << 2 << " " << 3 << "\n";
        file << 1 << " " << 3 * (len - 1) + 1 << "\n";


        for (int i = 1; i < len - 1; ++i) {
            file << 1 << " " << 3 * i + 1 << "\n";
            file << 0 << " " << 3 * i + 2 << " " << 3 * (i + 1) << "\n";
            file << 1 << " " << 3 * (i - 1) + 1 << "\n";
        }

        file << 1 << " " << 3 * (len - 1) + 1 << "\n";
        file << 0 << " " << 3 * (len - 1) + 2 << " " << 0 << "\n";
        file << 1 << " " << 3 * (len - 2) + 1 << "\n";

        file << 1 << endl;

        file.close();
    }
}

void gen_bad_cycles(int n) {
    ofstream file;
    int len = 1;
    for (int k = 1; k <= n; ++k) {
        len *= 2;
        file.open("./tests/bad_cycles/" + to_string(k));
        file << 3 * len << "\n";

        for (int i = 0; i < len - 1; ++i) {
            file << 1 << " " << 2 * i + 1 << "\n";
            file << 0 << " " << 2 * (i + 1) << " " << 2 * len + i << "\n";
        }

        file << 1 << " " << 2 * (len - 1) + 1 << "\n";
        file << 0 << " " << 0 << " " << 3 * len - 1 << "\n";

        file << 1 << " " << 2 * (len - 1) + 1 << "\n";

        for (int i = 1; i < len; ++i) {
            file << 1 << " " << 2 * (i - 1) + 1 << "\n"; 
        }

        file << 1 << endl;

        file.close();
    }
}

int main() {
    // gen_philosophers(17);
    // gen_philosophers_simple(18);
    // gen_cycles(20);
    gen_good_cycles(19);
    gen_bad_cycles(19);
}