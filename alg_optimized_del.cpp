#include <tuple>
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace alg_optimized_del {

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

    struct NodeChoice {
        int place;
        int transition;
        vector<int> next_places;

        explicit NodeChoice(int place, int transition, vector<int> next_places) {
            this->place = place;
            this->transition = transition;
            this->next_places = next_places;
        }
    };

    void change_blocks(vector<Node>& net, int place, vector<int>& block, int f(int)) {
        for (int next_transition : net[place].next) {
            for (int prev_place: net[next_transition].prev) {
                if (prev_place != place) {
                    block[prev_place] = f(block[prev_place]);
                }
            }
        }
        for (int prev_transition : net[place].prev) {
            for (int next_place: net[prev_transition].next) {
                if (next_place != place) {
                    block[next_place] = f(block[next_place]);
                }
            }
        }
    }

    void add_blocks(vector<Node>& net, int place, vector<int>& block) {
        change_blocks(net, place, block, [](int a) {return a + 1;});
    }

    void remove_blocks(vector<Node>& net, int place, vector<int>& block) {
        change_blocks(net, place, block, [](int a) {return a - 1;});
    }

    bool add_neighbourhood(vector<Node>& net, vector<int>& block, vector<bool>& component, int place, vector<NodeChoice>& stack) {
        vector<NodeChoice> stack_appended;
        for (int next_transition : net[place].next) {
            vector<int> correct_places;
            int in_component = 0;
            for (int next_place : net[next_transition].next) {
                if (component[next_place]) {
                    ++in_component;
                } else if (block[next_place] == 0) {
                    correct_places.push_back(next_place);
                }
            }
            if (in_component > 1 || (in_component == 0 && correct_places.empty())) {
                return false;
            }
            if (in_component == 0) {
                stack_appended.push_back(NodeChoice{place, next_transition, correct_places});
            }
        }

        for (int prev_transition : net[place].prev) {
            vector<int> correct_places;
            int in_component = 0;
            for (int prev_place : net[prev_transition].prev) {
                if (component[prev_place]) {
                    ++in_component;
                } else if (block[prev_place] == 0) {
                    correct_places.push_back(prev_place);
                }
            }
            if (in_component > 1 || (in_component == 0 && correct_places.empty())) {
                return false;
            }
            if (in_component == 0) {
                stack_appended.push_back(NodeChoice{place, prev_transition, correct_places});
            }
        }

        stack.insert(stack.end(), stack_appended.begin(), stack_appended.end());
        return true;
    }

    void rollback(vector<Node>& net, vector<NodeChoice>& stack_left, vector<NodeChoice>& stack_right, vector<int>& block, vector<bool>& component) {
        // flag that we're not rolling up (thus going to the end of the previous branch of the current place)
        bool change_branch = stack_right.back().place != (*(stack_right.end() - 2)).next_places.back();

        // put this branch back to stack_left (blocks of the original place are preserved)
        if (change_branch) {
            set<int> next_places;
            if (net[stack_right.back().place].next.count(stack_right.back().transition)) {
                next_places = net[stack_right.back().transition].next;
            } else {
                next_places = net[stack_right.back().transition].prev;
            }
            stack_left.push_back(NodeChoice{stack_right.back().place, stack_right.back().transition, vector<int> (next_places.begin(), next_places.end())});
        }

        // rollback
        stack_right.pop_back();
        vector<int> &cur_places = stack_right.back().next_places;

        // unblock the last place (it will be deleted)
        remove_blocks(net, cur_places.back(), block);
        
        // remove transitions of the current place if rolling up
        if (!change_branch) {
            while (!stack_left.empty() && stack_left.back().place == cur_places.back()) {
                stack_left.pop_back();
            }
            component[cur_places.back()] = false;
        }

        // delete current place and all blocked places (from next choices)
        do {
            cur_places.pop_back();
        } while (!cur_places.empty() && block[cur_places.back()] > 0);
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

            // number of blocks for places
            vector<int> block (net_size, 0);
            add_blocks(net, start_place, block);

            // true if place is a part of the current component
            vector<bool> component (net_size, false);
            component[start_place] = true;

            // left - transitions to add
            // right - transitions already added
            vector<NodeChoice> stack_left, stack_right;
            bool res = add_neighbourhood(net, block, component, start_place, stack_left);
            // res for the first place must be true

            // if it's a self-loop
            if (stack_left.empty()) {
                vis[start_place] = true;
                continue;
            }

            while (!stack_left.empty()) {
                stack_right.push_back(stack_left.back());
                stack_left.pop_back();
                
                vector<int> &cur_places = stack_right.back().next_places;
                res = false;

                while (!res) {
                
                    // remove places with block
                    while (!cur_places.empty() && block[cur_places.back()] > 0) {
                        cur_places.pop_back();
                    }

                    // rollback while no places are left
                    while (stack_right.size() > 1 && cur_places.empty()) {
                        rollback(net, stack_left, stack_right, block, component);
                    }

                    // means it's not SMD
                    if (stack_right.size() == 1 && cur_places.empty()) {
                        return start_place;
                    }

                    // update cur_places
                    cur_places = stack_right.back().next_places;

                    // block current place if needed and add next places
                    if (!net[stack_right.back().place].next.empty() && *(--net[stack_right.back().place].next.end()) == stack_right.back().transition ||
                        !net[stack_right.back().place].prev.empty() && *(--net[stack_right.back().place].prev.end()) == stack_right.back().transition) {
                        add_blocks(net, cur_places.back(), block);
                    }
                    res = add_neighbourhood(net, block, component, cur_places.back(), stack_left);

                    // if res is false remove this place and try another (start while again)
                    if (!res) {
                        cur_places.pop_back();
                    } else {
                        component[cur_places.back()] = true;
                    }
                }
            }

            // add right stack to vis
            while (!stack_right.empty()) {
                int cur_place = stack_right.back().next_places.back();
                stack_right.pop_back();
                vis[cur_place] = true;

                // delete paths to this place
                set<int> next_transitions = net[cur_place].next;
                set<int> prev_transitions = net[cur_place].prev;
                bool can_delete = true;
                for (int next: next_transitions) {
                    if (net[next].prev.size() == 1) {
                        can_delete = false;
                        break;
                    }
                }
                if (!can_delete) {
                    continue;
                }
                for (int prev: prev_transitions) {
                    if (net[prev].next.size() == 1) {
                        can_delete = false;
                    }
                }
                if (!can_delete) {
                    continue;
                }
                
                for (int next: next_transitions) {
                    net[next].prev.erase(cur_place);
                    net[cur_place].next.erase(next);
                }
                for (int prev: prev_transitions) {
                    net[prev].next.erase(cur_place);
                    net[cur_place].prev.erase(prev);
                }

            }
            vis[start_place] = true;
        }

        return -1;
    }
}


