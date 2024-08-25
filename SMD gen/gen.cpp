// this file's main function generates all SMD EN systems of size up to max_size
// generated nets are graphs that are:
//      bipartite (Petri nets)
//      without self-loops
//      state machine decomposable (check by another program)
// program gets no input and stores output in text file as list of graphs

#include <vector>
#include <unordered_set>
#include <set>
#include <string>

// for next_permutation function
#include <algorithm> 

// for writing to file
#include <fstream>

// for time measurement
#include <chrono>

// for debug
#include <iostream>

using namespace std;

#define forx(x, beg, end) for(int x = beg; x < end; ++x) 
#define fori(n) for(int i = 0; i < n; ++i) 

// stores and modifies SMD EN systems
// methods used for SMD checking are private
class Net {

    private:
        // graph as matrix
        // graph[i][j] = 1 if there is an arc from i to j
        // as far as net is SMD EN:
        //      diagonal elements are zeros
        //      if graph[i][j] != 0 then graph[j][i] == 0
        vector<vector<int>> graph;

        // size of net
        // may differ from graph.size()
        // 
        int size;
        
        // true if ith vertex is place, false if it is transition
        vector<bool> places;

        // for each vertex if it is place it contains copies of all seq components from cover
        vector<set<vector<int>>> cover;

        int smd_dif;
    
    public:

    // method for Net (int max_size) constructor
    private:

    // change maximum size of net
    void resize_net(int size) {
        graph.resize(size);
        fori(size) {
            graph[i].resize(size);
        }
        places.resize(size);
        cover.resize(size);
    }

    public:

    Net () {
        size = 0;
        places = {};
        graph = {{}};
        cover = {};
    }

    // base constructor
    // create empty net, fix maximum size
    Net (int max_size, int smd_dif_base = 0) {
        resize_net(max_size);
        size = 0;
        smd_dif = smd_dif_base;
    }

    // copy constructor
    Net (Net* net) {
        resize_net(net->graph.size());
        graph = net->graph;
        size = net->size;
        places = net->places;
        cover = net->cover;
        smd_dif = net->smd_dif;
    }

    // redirect net[i][j] to graph[i][j]
    // getter for graph
    const vector<int>& operator[](int v) const {
        return graph[v];
    }

    // comparison operator for nets
    bool operator==(const Net& other) const {
        if (other.size != size) {
            return false;
        }
        if (!equal(other.places.begin(), other.places.begin() + size, places.begin())) {
            return false;
        }
        fori(size) {
            if (!equal(other.graph[i].begin(), other.graph[i].begin() + size, graph[i].begin())) {
                return false;
            }
        }
        return true;
    }

    // return all children of vertex v
    // input: vertex index
    // output: vector of indexes of child vertices
    vector<int> next(int v) const {
        vector<int> res;
        fori (size) {
            if (graph[v][i]) {
                res.push_back(i);
            }
        }
        return res;
    }

    // return all parents of vertex v
    // input: vertex index
    // output: vector of indexes of parent vertices
    vector<int> prev(int v) const {
        vector<int> res;
        fori (size) {
            if (graph[i][v]) {
                res.push_back(i);
            }
        }
        return res;
    }

    // getter for size of net
    size_t get_size() const noexcept {
        return size;
    }

    // redirect net.is_place(i) to is_place[i]
    // getter for is_place
    bool is_place(int i) const {
        return places[i];
    }

    // return list of places' indices
    // in order from small to large
    vector<int> places_list() const {
        vector<int> res;
        fori(size) {
            if (places[i]) {
                res.push_back(i);
            }
        }
        return res;
    }

    // return list of transitions' indices
    // in order from small to large
    vector<int> transitions_list() const {
        vector<int> res;
        fori(size) {
            if (!places[i]) {
                res.push_back(i);
            }
        }
        return res;
    }
    
    // return number of places
    int places_num() const {
        int res = 0;
        fori(size) {
            res += places[i];
        }
        return res;
    }

    // return number of transitions
    int transitions_num() const {
        int res = 0;
        fori(size) {
            res += 1 - places[i];
        }
        return res;
    }

    vector<set<vector<int>>> get_cover() const {
        return cover;
    }

    // change graph according to new enumeration of vertices
    // input:
    //      permutation for list of vertices
    //      flag for changing anything other than graph
    // output:
    //      change graph (change basis for matrix) and places
    //      optional: change cover
    void renumerate(vector<int> indices, bool change_all = false) {
        // change places
        vector<bool> places_prev (places);
        fori(size) {
            places[i] = places_prev[indices[i]];
        }

        vector<vector<int>> graph_prev (graph);
        // change rows of graph
        fori(size) {
            graph[i] = graph_prev[indices[i]];
        }

        graph_prev = graph;
        // change columns of graph
        forx(r, 0, size) {
            forx(c, 0, size) {
                graph[r][c] = graph_prev[r][indices[c]];
            }
        }

        if (change_all) {

            // change cover
            vector<set<vector<int>>> cover_prev (size);
            fori (size) {
                for (vector<int> cov: cover[i]) {
                    cover_prev[i].insert(cov);
                }
            }
            fori (size) {
                cover.clear();
                for (vector<int> cov: cover_prev[i]) {
                    forx(p, 0, cov.size()) {
                        cov[p] = indices[cov[p]];
                    }
                    cover[i].insert(cov);
                }
            }
        } else {
            cover.clear();
        }
    }

    // reverse all arcs in net
    void transpose() {
        vector<vector<int>> graph_prev (size);
        fori(size) {
            graph_prev[i] = graph[i];
        }
        forx (r, 0, size) {
            forx(c, 0, size) {
                graph[r][c] = graph_prev[c][r];
            }
        }
    }

    // methods used in add_transition
    private:

    // check if net give by graph is bipartite with sides given by in_place
    // output: success
    bool check_bipartiteness() const {
        forx (cur, 0, size) {
            forx (next, 0, size) {
                if (graph[cur][next] && !(places[cur] ^ places[next])) {
                    return false;
                }
            }
        }
        return true;
    }

    // return cartesian product of vectors from input vector
    // input: vector of vectors (k vectors of sizes n1, ..., nk)
    // output: vector of vectors (n1 * ... * nk vectors of size k)
    static vector<vector<int>> gen_prod(vector<vector<int>> input) {
        int n = input.size();

        vector<int> prod;
        prod.resize(n);
        fori (n) {
            prod[i] = input[i][0];
        };

        vector<int> next_pos (n, 0);

        vector<vector<int>> res {};
        res.push_back(prod);
        
        int i = n;

        while (true) {
            --i;
            while (prod[i] == input[i].back()) {
                --i;
                if (i < 0) {
                    return res;
                }
            }

            ++next_pos[i];
            prod[i] = input[i][next_pos[i]];
            ++i;

            while (i < n) {
                prod[i] = input[i][0];
                next_pos[i] = 0;
                ++i;
            }

            res.push_back(prod);
        }

        return res;
    }

    // check if place p has neighbours that are visited
    // neighbour is child of parent or parent of child (for graph)
    // input:
    //      p, index of place
    //      vis, boolean vector of size equal to net size for visited vertices
    // output: success if there are neighbours
    bool check_neighbours(int p, vector<bool> vis) const {
        for (int prev_transition: prev(p)) {
            int neighbours = 0;
            for (int neighbour: next(prev_transition)) {
                neighbours += vis[neighbour];
            }
            if (neighbours > 1) {
                return false;
            }
        }
        for (int next_transition: next(p)) {
            int neighbours = 0;
            for (int neighbour: prev(next_transition)) {
                neighbours += vis[neighbour];
            }
            if (neighbours > 1) {
                return false;
            }
        }
        return true;
    }

    // that recursive algorithm for finding seq component
    pair<bool, vector<bool>> find_sequential_component_rec(int p, vector<bool> vis) const {
        if (vis[p]) {
            for (int prev_v : prev(p)) {
                int neighbour = 0;
                for (int prev_next: next(prev_v)) {
                    neighbour += vis[prev_next];
                }
                if (neighbour > 1) {
                    return {false, vis};
                }
            }

            for (int next_v: next(p)) {
                int neighbour = 0;
                for (int next_prev: prev(next_v)) {
                    neighbour += vis[next_prev];
                }
                if (neighbour > 1) {
                    return {false, vis};
                }
            }

            return {true, vis};
        }

        vector<vector<int>> next;
        for (int next_transition : this->next(p)) {
            vector<int> next_next = this->next(next_transition);
            next.emplace_back(next_next.begin(), next_next.end());
        }
        for (int prev_transition : prev(p)) {
            vector<int> prev_prev = prev(prev_transition);
            next.emplace_back(prev_prev.begin(), prev_prev.end());
        }

        vector<vector<int>> next_prod = gen_prod(next);
        
        vector<bool> vis_prev (vis);

        for (vector<int> p_next : next_prod) {
            bool success = true;
            vis[p] = true;
            for (int p_i : p_next) {
                auto res = find_sequential_component_rec(p_i, vis);
                if (res.first) {
                    vis = res.second;
                } else {
                    success = false;
                    vis = vis_prev;
                    break;
                }
            }
            if (success && find_sequential_component_rec(p, vis).first) {
                return {true, vis};
            }
        }
        return {false, vis};
    }

    // check if there is sequential component that contains given place
    // input: index of place p
    // output: success
    pair<bool, vector<int>> find_sequential_component(int p) const {
        // isolated vertex
        if (next(p).empty() && prev(p).empty()) {
            return {true, {p}};
        }

        vector<bool> vis (size, false);

        auto res = find_sequential_component_rec(p, vis);

        if (!res.first) {
            return {false, {}};
        }

        // collect indices of visited vertices in vector
        vector<int> res_list;
        fori(size) {
            if (res.second[i]) {
                res_list.push_back(i);
            }
        }

        return {true, res_list};

    }

    // check if net given by graph is smd
    // supposed that net without last transition is smd
    // output:
    //      success flag
    //      on success change cover to fit new net
    bool check_smd() {
        vector<set<vector<int>>> cover_prev (size);
        forx (i, 0, size) {
            for (vector<int> cov: cover[i]) {
                cover_prev[i].insert(cov);   
            }
        }

        int smd_dif_cur = 0;

        vector<bool> vis (size, true);
        fori(size - 1) {
            if (!places[i]) {
                continue;
            }

            vector<vector<int>> del_list;
            for (vector<int> cov: cover[i]) {
                bool to_del = false;
                for (int place: cov) {
                    if (graph[size - 1][place] || graph[place][size - 1]) {
                        to_del = true;
                        break;
                    }
                }
                if (to_del) {
                    for (int place: cov) {
                        del_list.push_back(cov);
                    }
                }
            }

            for (vector<int> cov: del_list) {
                for (int v: cov) {
                    cover[v].erase(cov);
                }
            }

            if (cover[i].empty()) {
                vis[i] = false;
            }
        }

        cover[size - 1] = {{}};

        forx (start_place, 0, size) {
            if (!places[start_place] || vis[start_place]) {
                continue;
            }
            pair<bool, vector<int>> res = find_sequential_component(start_place);
            if (!res.first) {
                ++smd_dif_cur;
                if (smd_dif_cur > smd_dif) {
                    fori (size) {
                        cover[i] = set<vector<int>> (cover_prev[i]);
                    }
                    return false;
                }
            }

            for (int place : res.second) {
                vis[place] = true;
                cover[place].insert(res.second);
            }
        }

        return true;
    }

    public:

    // add transition to net, check if new net is SMD EN
    // input:
    //      list of arcs for vertices:
    //          1 if from new to ith
    //          0 if no arc
    //          -1 if from ith to new
    // output:
    //      success
    //      on success this net must contain new transition
    bool add_transition(vector<int> arcs) {

        if (!(find(arcs.begin(), arcs.end(), 1) != arcs.end() &&
            find(arcs.begin(), arcs.end(), -1) != arcs.end())) {
            return false;
        }

        size += 1;
        places[size - 1] = false;
        cover[size - 1] = {{}};
        int place_ind = 0;
        fori(size - 1) {
            if (!places[i]) {
                continue;
            }
            switch (arcs[place_ind++]) {
            case 1:
                graph[i][size - 1] = 0;
                graph[size - 1][i] = 1;
                break;
            case -1:
                graph[i][size - 1] = 1;
                graph[size - 1][i] = 0;
                break;
            default:
                graph[i][size - 1] = 0;
                graph[size - 1][i] = 0;
            }
        }
        graph[size - 1][size - 1] = 0;
        
        if (!(check_bipartiteness() && check_smd())) {
            size -= 1;
            return false;
        }

        return true;
    }

    // add place to net, no arcs to or from new place
    void add_place() {
        size += 1;
        places[size - 1] = true;
        fori(size) {
            graph[i][size - 1] = 0;
            graph[size - 1][i] = 0;
        }
        cover[size - 1] = {{size - 1}};
    }

    // check if any local transformation can be applied to net
    // transformations are:
    //      P -> T -> P = P and T -> P -> T = T
    //      if there are two vertices with equal neighbourhood, one of them can be discarded
    // return true if there are any
    bool check_transformations() const {

        fori(size) {

            // find neighbourhood intersection

            vector<int> prev_list = prev(i);
            vector<int> next_list = next(i);
            
            set<int> prev_next;
            for(int prev_v: prev_list) {
                for (int prev_next_v: next(prev_v)) {
                    if (prev_next_v != i) { 
                        prev_next.insert(prev_next_v);
                    }
                }
            }
            set<int> next_prev;
            for(int next_v: next_list) {
                for (int next_prev_v: prev(next_v)) {
                    if (next_prev_v != i) {
                        next_prev.insert(next_prev_v);
                    }
                }
            }

            vector<int> intersection;
            if (prev_list.empty()) {
                intersection = vector<int>(next_prev.begin(), next_prev.end());
            } else if (next_list.empty()) {
                intersection = vector<int>(prev_next.begin(), prev_next.end());
            } else {
                set_intersection(prev_next.begin(), prev_next.end(), next_prev.begin(), next_prev.end(), back_inserter(intersection));
            }

            // first transformation check
            if (prev(i).size() == 1 &&
                next(i).size() == 1 &&
                intersection.empty()) {
                
                set<int> prev_prev;
                for(int prev_v: prev_list) {
                    for (int prev_prev_v: prev(prev_v)) {
                        prev_prev.insert(prev_prev_v);
                    }
                }
                set<int> next_next;
                for(int next_v: next_list) {
                    for (int next_next_v: next(next_v)) {
                        next_next.insert(next_next_v);
                    }
                }

                vector<int> common;
                set_intersection(prev_prev.begin(), prev_prev.end(), next_next.begin(), next_next.end(), back_inserter(common));

                // there must be no self loops
                if (common.empty()) {
                    return true;
                }
            }

            
            
            for (int v: intersection) {
                vector<int> prev_v_list = prev(v);
                vector<int> next_v_list = next(v);
                if (prev_list.size() == prev_v_list.size() && equal(prev_list.begin(), prev_list.end(), prev_v_list.begin()) &&
                    next_list.size() == next_v_list.size() && equal(next_list.begin(), next_list.end(), next_v_list.begin())) {
                    return true;
                }
            }
            
        }
        return false;
    }

    // check if graph is connected (not strongly)
    bool check_connectivity() const {
        vector<bool> vis (size, false);
        vis[0] = true;
        vector<int> queue;
        queue.push_back(0);
        while (!queue.empty()) {
            int v = queue.back();
            queue.pop_back();
            for (int next_v: next(v)) {
                if (!vis[next_v]) {
                    vis[next_v] = true;
                    queue.push_back(next_v);
                }
            }
            for (int prev_v: prev(v)) {
                if (!vis[prev_v]) {
                    vis[prev_v] = true;
                    queue.push_back(prev_v);
                }
            }
        }
        fori (size) {
            if (!vis[i]) {
                return false;
            }
        }
        return true;
    }
};


// hash function for storing nets
// see docs for formula (if docs are ready)
template <>
struct std::hash<Net*> {
    size_t operator () (Net* net) const {
        
        int res = 0;
        int multiplier = 1;
        const int base_multiplier = 7;
        const int place_multiplier = 100;
        const int in_multiplier = 2;
        const int out_multiplier = 3;
        const int mod = 1e6 + 3;

        forx(k, 0, net->get_size()) {
            
            int vertix_value = 0;
            vertix_value += place_multiplier * net->is_place(k);
            fori(net->get_size()) {
                vertix_value += in_multiplier * (*net)[i][k];
            }
            fori(net->get_size()) {
                vertix_value += out_multiplier * (*net)[k][i];
            }

            res += multiplier * vertix_value;
            res %= mod;

            multiplier *= base_multiplier;
            multiplier %= mod;
        }

        return res;
    }
};

// comparison function (KeyEqual) for storing nets in hash table
template<>
struct std::equal_to<Net*> {
    bool operator () (Net* net, Net* other) const {
        return *net == *other;
    }
};

// return graph as string in form of matrix
// if all is true add information about cover and begin/end boundaries
string graph_to_string(Net* net, bool all = false) {
    string res;
    if (all) {
        res += "begin\n\n";
    }
    fori(net->get_size()) {
        res += to_string(int(net->is_place(i)));
        res += " ";
    }
    res += "\n\n";
    forx (r, 0, net->get_size()) {
        forx (c, 0, net->get_size()) {
            res += to_string((*net)[r][c]);
            res += " ";
        }
        res += "\n";
    }
    res += "\n";

    if (all) {
        auto cover = net->get_cover();
        fori (net->places_num()) {
            for (vector<int> v: cover[i]) {
                res += "{";
                for (int p: v) {
                    res += to_string(p);
                    res += " ";
                }
                res += "} ";

            }
            res += "\n";
        }
        res += "\nend\n";
    }
    return res;
}


// generate nets of size up to max_size
// that are smd for all except smd_dif places
// and write the most important to output_filename
int main() {

    int max_size;
    int smd_dif;
    string output_filename;

    cin >> max_size >> smd_dif >> output_filename;
    cout << endl;

    fstream output (output_filename);

    auto start_time = chrono::steady_clock::now();

    // only left-most variants
    vector<Net*> nets;
    Net* net = new Net(max_size, smd_dif);
    nets.push_back(net);

    // all automorphisms of nets
    unordered_set<Net*> nets_sym;
    nets_sym.insert(net);

    forx(cur_size, 1, max_size) {

        vector<Net*> nets_prev = nets;
        nets.clear();

        Net* net_places = new Net(nets_prev.front());
        net_places->add_place();
        nets.push_back(net_places);
        nets_sym.insert(net_places);

        int nets_num = 0;

        for (Net* net_prev: nets_prev) {
            Net* net_cur = new Net(net_prev);

            // add one transition to every net
            int places_num = net_cur->places_num();
            // each transition has number from 0 to 3^k - 1 including
            // which defines -1/0/1 for each of k places 
            int t_max = 1;
            fori(places_num) {
                t_max *= 3;
            }
            forx(t, 0, t_max) {
                vector<int> arcs (places_num);
                int t_cur = t;
                fori(places_num) {
                    arcs[i] = t_cur % 3 - 1;
                    t_cur /= 3;
                }
                int add_success = net_cur->add_transition(arcs);
                if (add_success) {
                    if (nets_sym.find(net_cur) == nets_sym.end()) {
                        
                        nets.push_back(net_cur);
                        
                        if (net_cur->check_connectivity() && !net_cur->check_transformations()) {
                            output << graph_to_string(net_cur) << std::endl;
                            nets_num += 1;
                        }

                        // add all automorphisms to nets
                        // to ensure that all net are unique up to isomorphisms
                        vector<int>
                            places = net_cur->places_list(),
                            transitions = net_cur->transitions_list();
                        vector<int> indices (net_cur->get_size());

                        // loops are in form of do while
                        bool transition_permutation = true;
                        while (transition_permutation) {
                            bool place_permutation = true;
                            while (place_permutation) {
                                int places_ind = 0,
                                    transitions_ind = 0;
                                fori(net_cur->get_size()) {
                                    if (net_cur->is_place(i)) {
                                        indices[i] = places[places_ind++];
                                    } else {
                                        indices[i] = transitions[transitions_ind++];
                                    }
                                }

                                Net* net_next = new Net(net_cur);
                                net_next->renumerate(indices);
                                nets_sym.insert(net_next);
                                
                                Net* net_next_t = new Net(net_next);
                                net_next_t->transpose();
                                nets_sym.insert(net_next_t);

                                place_permutation = next_permutation(places.begin(), places.end());
                            }
                            transition_permutation = next_permutation(transitions.begin(), transitions.end());
                        }
                    }
                    net_cur = new Net(net_prev);
                }
            }
        }

        cout << nets_num << " " << nets.size() << " " << nets_sym.size() << endl << endl;

        for (Net* net: nets_prev) {
            delete net;
        }
        nets_prev.clear();

        vector<Net*> nets_del (nets_sym.begin(), nets_sym.end());
        nets_sym.clear();
        // seg fault for some reasons
        // for (Net* net: nets_del) {
        //     delete net;
        // }
        nets_del.clear();
    }

    // last cycle

    vector<Net*> nets_prev = nets;
    nets.clear();

    int nets_num = 0;

    for (Net* net_prev: nets_prev) {
        Net* net_cur = new Net(net_prev);

        // add one transition to every net
        int places_num = net_cur->places_num();
        // each transition has number from 0 to 3^k - 1 including
        // which defines -1/0/1 for each of k places 
        int t_max = 1;
        fori(places_num) {
            t_max *= 3;
        }
        forx(t, 0, t_max) {
            vector<int> arcs (places_num);
            int t_cur = t;
            fori(places_num) {
                arcs[i] = t_cur % 3 - 1;
                t_cur /= 3;
            }
            int add_success = net_cur->add_transition(arcs);
            if (add_success) {
                if (net_cur->check_connectivity() && !net_cur->check_transformations() &&
                    nets_sym.find(net_cur) == nets_sym.end()) {
                        
                    nets.push_back(net_cur);
                    output << graph_to_string(net_cur) << std::endl;
                    nets_num += 1;

                    // add all automorphisms to nets
                    // to ensure that all net are unique up to isomorphisms
                    vector<int>
                        places = net_cur->places_list(),
                        transitions = net_cur->transitions_list();
                    vector<int> indices (net_cur->get_size());

                    // loops are in form of do while
                    bool transition_permutation = true;
                    while (transition_permutation) {
                        bool place_permutation = true;
                        while (place_permutation) {
                            int places_ind = 0,
                                transitions_ind = 0;
                            fori(net_cur->get_size()) {
                                if (net_cur->is_place(i)) {
                                    indices[i] = places[places_ind++];
                                } else {
                                    indices[i] = transitions[transitions_ind++];
                                }
                            }
                            Net* net_next = new Net(net_cur);
                            net_next->renumerate(indices);
                            nets_sym.insert(net_next);

                            Net* net_next_t = new Net(net_next);
                            net_next_t->transpose();
                            nets_sym.insert(net_next_t);

                            place_permutation = next_permutation(places.begin(), places.end());
                        }
                        transition_permutation = next_permutation(transitions.begin(), transitions.end());
                    }
                }
                net_cur = new Net(net_prev);
            }
        }
    }

    cout << nets_num << " " << nets.size() << " " << nets_sym.size() << endl << endl;

    auto end_time = chrono::steady_clock::now();

    cout << (float)(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start_time)).count() << endl;

    for (Net* net: nets_prev) {
        delete net;
    }
    nets_prev.clear();

    for (Net* net: nets) {
        delete net;
    }
}