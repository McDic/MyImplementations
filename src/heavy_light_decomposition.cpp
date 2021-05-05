/*=============================================================================
    McDic's Competitive Programming Template / Requires C++11 or later.
    Last edited: 2021-03-29 23:49 UTC+9
=============================================================================*/

// Custom define
#ifdef __McDic__ // Local testing
#define debugprintf(f_, ...) fprintf(stderr, f_, ##__VA_ARGS__)
#define debugflush() fflush(stdout),fflush(stderr)
#else // Submission version
#define debugprintf(f_, ...) NULL
#define debugflush() NULL
#endif

// Standard libraries
#include <stdio.h>
#include <iostream>
#include <chrono> // For template clock
#include <vector>
#include <queue>
#include <algorithm>
#include <functional>
#include <string>
#include <map>
#include <set>
#include <utility>
#include <assert.h>

// Random
#include <random>
std::mt19937_64 mersenne_twister(std::chrono::steady_clock::now().time_since_epoch().count());

/*=============================================================================
    Implement your own code below. 
    To modify number of test cases, please look at Main function.
=============================================================================*/

namespace McDicCP{

    typedef long long int lld;

    template<typename S> class MonoidBase{ public:
        MonoidBase(){};
        static S identity(){ return 0;};
        static S op(S a, S b){ return 0;};
    };

    template<typename S> class MonoidSum: MonoidBase<S>{ public:
        static S identity(){ return 0;}
        static S op(S a, S b){ return a+b;}
    };

    template<typename S> class MonoidMax: MonoidBase<S>{ public:
        static S identity(){ return -1;}
        static S op(S a, S b){ return a >= b ? a : b;}
    };

    template<typename S, class Monoid> class SegmentTree{ public:

        int size;
        std::vector<S> arr;

        SegmentTree(int size){
            this->size = size;
            arr = std::vector<S>(size << 1, Monoid::identity());
        }

        void set(int location, S value){
            location += size;
            arr[location] = value;
            while(location > 1){
                location >>= 1;
                arr[location] = Monoid::op(arr[location << 1], arr[location << 1 | 1]);
            }
        }

        S get(int left, int right){
            S left_answer = Monoid::identity(), right_answer = Monoid::identity();
            for(left += size, right += size+1; left < right; left >>= 1, right >>= 1){
                if(left & 1) left_answer = Monoid::op(left_answer, arr[left++]);
                if(right & 1) right_answer = Monoid::op(arr[--right], right_answer);
            }
            return Monoid::op(left_answer, right_answer);
        }
    };

    template<typename S, class Monoid, class Segment> class HLD{ public:

        // Graph info
        int n, root;
        std::vector<int> parents, degrees;
        static const int bitlimit = 30;
        std::vector<int> powerparents[bitlimit];
        std::vector<std::vector<int>> childs;

        // HLD info
        std::vector<int> size_of_subtree;
        std::vector<int> in_group_indices, group_indices;
        std::vector<std::vector<int>> groups;

        // HLD - Addons
        std::vector<Segment> segments;

        // Constructor
        HLD(const int n, const int root, const std::vector<std::vector<int>> &edges){
            initialize_graph(n, root, edges);
            initialize_lca();
            initialize_hld();
        }

        // Initialize Graph
        void initialize_graph(const int n, const int root, const std::vector<std::vector<int>> &edges){
            this->n = n;
            this->root = root;
            this->parents = std::vector<int>(n, -1);
            this->childs = std::vector<std::vector<int>>(n);
            this->degrees = std::vector<int>(n, -1);

            std::queue<std::pair<int, int>> q;
            q.push({-1, root});
            while(!q.empty()){
                auto [parent, now] = q.front(); q.pop();
                this->parents[now] = parent;
                this->degrees[now] = (parent == -1 ? 0 : this->degrees[parent] + 1);
                if(parent != -1) this->childs[parent].push_back(now);
                for(int next: edges[now]){
                    if(next == parent) continue;
                    q.push({now, next});
                }
            }
            this->parents[root] = root;
        }

        // Initialize LCA
        void initialize_lca(){
            this->powerparents[0] = this->parents;
            for(int b=1; b<bitlimit; b++){
                this->powerparents[b] = std::vector<int>(n, -1);
                for(int i=0; i<this->n; i++)
                    this->powerparents[b][i] = this->powerparents[b-1][this->powerparents[b-1][i]];
            }
        }

        // Initialize HLD
        void initialize_hld(){
            
            int depth = 0;
            
            // Size of subtree
            auto recur1 = [&](int now, auto recur) -> int{
                depth++;
                size_of_subtree[now] = 1;
                for(int child: this->childs[now])
                    size_of_subtree[now] += recur(child, recur);
                depth--;
                return size_of_subtree[now];
            };
            size_of_subtree = std::vector<int>(n, -1);
            recur1(this->root, recur1);

            // Groups
            auto recur2 = [&](int now, size_t group_index, auto recur) -> void{
                this->groups[group_index].push_back(now);
                this->group_indices[now] = group_index;
                this->in_group_indices[now] = this->groups[group_index].size() - 1;
                if(this->childs[now].empty()) return;
                
                int max_subtree_size = -1, max_subtree_child = -1;
                for(int child: this->childs[now]){
                    if(max_subtree_size < size_of_subtree[child]){
                        max_subtree_size = size_of_subtree[child];
                        max_subtree_child = child;
                    }
                }

                recur(max_subtree_child, group_index, recur);
                for(int child: this->childs[now]){
                    if(child == max_subtree_child) continue;
                    this->groups.push_back({});
                    recur(child, this->groups.size() - 1, recur);
                }
            };
            this->group_indices = std::vector<int>(this->n, -1);
            this->in_group_indices = std::vector<int>(this->n, -1);
            this->groups = std::vector<std::vector<int>>(1, std::vector<int>());
            recur2(this->root, 0, recur2);

            for(size_t i=0; i<this->groups.size(); i++)
                this->segments.push_back(Segment(this->groups[i].size()));
        }

        int LCA(int v1, int v2){

            int degree_difference = this->degrees[v1] - this->degrees[v2];
            if(degree_difference < 0){
                int temp = v1;
                v1 = v2;
                v2 = temp;
                degree_difference *= -1;    
            }

            for(int b=0; b<bitlimit; b++)
                if(degree_difference & (1<<b)) v1 = this->powerparents[b][v1];

            int b = bitlimit - 1;
            while(v1 != v2 && b >= 0){
                if(this->powerparents[b][v1] != this->powerparents[b][v2])
                    v1 = this->powerparents[b][v1],
                    v2 = this->powerparents[b][v2];
                b--;
            }
            if(v1 != v2) v1 = this->parents[v1], v2 = this->parents[v2];
            return v1;
        }

        int hld_grouppeak(int v){
            return groups[group_indices[v]][0];
        }
        int hld_groupnext(int v){
            if(in_group_indices[v] + 1 >= groups[group_indices[v]].size()) return -1;
            else return groups[group_indices[v]][in_group_indices[v] + 1];
        }

        std::tuple<int, int, int> information(int v1, int v2){
            assert(group_indices[v1] == group_indices[v2]);
        }

        S get_of_group(int v1, int v2){
            assert(group_indices[v1] == group_indices[v2]);
            int group_index_start = in_group_indices[v1], group_index_end = in_group_indices[v2], group_number = group_indices[v1];
            return get_of_group(group_number, group_index_start, group_index_end);
        }
        S get_of_group(int group_number, int group_index_start, int group_index_end){
            if(group_index_start > group_index_end)
                std::swap(group_index_start, group_index_end);
            return segments[group_number].get(group_index_start, group_index_end);
        }

        void set(int v, S value){
            segments[group_indices[v]].set(in_group_indices[v], value);
        }

        S get(int v1, int v2, bool vertexmode = true){

            int v_lca = LCA(v1, v2);

            S left_answer = Monoid::identity(), right_answer = Monoid::identity();
            while(group_indices[v1] != group_indices[v_lca]){
                int pog = hld_grouppeak(v1);
                left_answer = Monoid::op(left_answer, get_of_group(v1, pog));
                v1 = this->parents[pog];
            }
            if(v1 != v_lca) left_answer = Monoid::op(left_answer, get_of_group(v1, hld_groupnext(v_lca)));

            if(vertexmode) left_answer = Monoid::op(left_answer, get_of_group(v_lca, v_lca));

            while(this->group_indices[v2] != this->group_indices[v_lca]){
                int pog = hld_grouppeak(v2);
                right_answer = Monoid::op(get_of_group(pog, v2), right_answer);
                v2 = this->parents[pog];
            }
            if(v2 != v_lca) right_answer = Monoid::op(get_of_group(hld_groupnext(v_lca), v2), right_answer);

            return Monoid::op(left_answer, right_answer);
        }
    };

    // Main solver function
    void solve(int tnum){
        int n; scanf("%d", &n);
        std::vector<std::vector<int>> edges(n);
        std::vector<std::tuple<int, int, int>> vvw;
        for(int i=1; i<n; i++){
            int v1, v2, w; 
            scanf("%d %d %d", &v1, &v2, &w);
            v1--, v2--;
            edges[v1].push_back(v2);
            edges[v2].push_back(v1);
            vvw.push_back({v1, v2, w});
        }
        
        HLD<int, MonoidMax<int>, SegmentTree<int, MonoidMax<int>>> hld(n, 0, edges);
        for(std::tuple<int, int, int> info: vvw){
            int v1 = std::get<0>(info), v2 = std::get<1>(info), w = std::get<2>(info);
            if(hld.parents[v1] == v2) hld.set(v1, w);
            else hld.set(v2, w);
        }

        /*for(int i=0; i<hld.groups.size(); i++){
            debugprintf("Group #%d: ", i+1);
            for(int v: hld.groups[i]) debugprintf("%d, ", v+1);
            debugprintf("\n");
        } debugprintf("=======\n");*/

        int q; scanf("%d", &q);
        for(int i=0; i<q; i++){
            
            int querytype; scanf("%d", &querytype);
            if(querytype == 1){
                int index, value; scanf("%d %d", &index, &value); index--;
                int v1 = std::get<0>(vvw[index]), v2 = std::get<1>(vvw[index]);
                if(hld.parents[v1] == v2) hld.set(v1, value);
                else hld.set(v2, value);
            }
            else{
                int v1, v2; scanf("%d %d", &v1, &v2);
                v1--, v2--;
                printf("%d\n", hld.get(v1, v2, false));
            }

        }
    }

}

/*=============================================================================
    int main();
=============================================================================*/

// Main
int main(int argc, char **argv){
#ifdef __McDic__ // Local testing
    freopen("IO/input.txt", "r", stdin);
    printf("================================================\n");
    printf(" McDic's C++17 file execution for CP\n");
    printf("================================================\n");
    freopen("IO/output.txt", "w", stdout);
    freopen("IO/debug.txt", "w", stderr);
    auto startedTime = std::chrono::steady_clock::now();
#endif

    int testcases = 1; 
    //std::cin >> testcases;
    for(int t=1; t<=testcases; t++) McDicCP::solve(t);

#ifdef __McDic__ // Local testing
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = endTime - startedTime;
    debugprintf("\n"); for(int i=0; i<80; i++) debugprintf("="); debugprintf("\n");
    debugprintf("Total %.8lf ms used.\n", duration.count());
#endif
    return 0;
}