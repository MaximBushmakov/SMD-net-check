#include <tuple>
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>


namespace alg_base {

    using namespace std;

    #define fori(n) for (int i = 0; i < n; ++i)


    struct Node {
        int val;
        bool is_place;
        set<int> prev;
        set<int> next;

        explicit Node(int val, bool is_place, set<int> prev, set<int> next) {
            this->val = val;
            this->is_place = is_place;
            this->prev = prev;
            this->next = next;
        }

        explicit Node() {}
    };

    vector<vector<int>> gen_prod(vector<vector<int>> next) {
        int n = next.size();

        vector<int> prod;
        prod.resize(n);
        fori (n) {
            prod[i] = next[i][0];
        };

        vector<int> next_pos (n, 0);

        vector<vector<int>> ans {};
        ans.push_back(prod);
        
        int i = n;

        while (true) {
            --i;
            while (i >= 0 && prod[i] == next[i].back()) {
                --i;
            }

            if (i == -1) {
                break;
            }

            ++next_pos[i];
            prod[i] = next[i][next_pos[i]];
            ++i;

            while (i < n) {
                prod[i] = next[i][0];
                next_pos[i] = 0;
                ++i;
            }

            ans.push_back(prod);
        }

        return ans;
    }


    pair<bool, set<int>> SMD_check(int p, set<int> s, vector<Node>& net) {
        if (s.count(p)) {
            for (int prev_transition : net[p].prev) {
                int neighbour = 0;
                for (int next_place : net[prev_transition].next) {
                    neighbour += s.count(next_place);
                }
                if (neighbour > 1) {
                    return {false, s};
                }
            }

            for (int next_transition : net[p].next) {
                int neighbour = 0;
                for (int prev_place : net[next_transition].prev) {
                    neighbour += s.count(prev_place);
                }
                if (neighbour > 1) {
                    return {false, s};
                }
            }

            return {true, s};
        }

        vector<vector<int>> next;
        for (int next_transition : net[p].next) {
            next.emplace_back(vector<int>(net[next_transition].next.begin(), net[next_transition].next.end()));
        }
        for (int prev_transition : net[p].prev) {
            next.emplace_back(vector<int>(net[prev_transition].prev.begin(), net[prev_transition].prev.end()));
        }

        // isolated vertex
        if (next.empty()) {
            return {true, {p}};
        }

        vector<vector<int>> next_prod = gen_prod(next);
        pair<bool, set<int>> res;

        set<int> s0 = s;

        for (vector<int> p_next : next_prod) {
            bool success = true;
            s.insert(p);
            for (int p_i : p_next) {
                res = SMD_check(p_i, s, net);
                success = res.first;
                if (!success) {
                    break;
                }
                s = res.second;
            }
            if (success) {
                res = SMD_check(p, s, net);
                if (res.first) {
                    return {true, res.second};
                }
            }
            s = s0;
        }
        return {false, s0};

    }

    // return 
    //      index of first place that does not belong to any sequential component
    //      -1 if it's SMD
    //      -2 if it isn't Petri nets (isn't bipartite graph)
    int alg(int net_size, vector<Node>& net) {

        bool is_correct = true;
        fori (net_size) {
            for (int next: net[i].next) {
                if (!(net[next].is_place ^ net[i].is_place)) {
                    is_correct = false;
                    break;
                }
            }
            if (!is_correct) {
                break;
            }
        }
        if (!is_correct) {
            return -2;
        }

        vector<bool> vis (net_size, false);

        for (int start_place = 0; start_place < net_size; ++start_place) {
            if (!net[start_place].is_place || vis[start_place]) {
                continue;
            }
            pair<bool, set<int>> res = SMD_check(start_place, {}, net);
            if (!res.first) {
                return start_place;
            }
            for (int place : res.second) {
                vis[place] = true;
            }
        }

        return -1;
    }
}