/*=============================================================================
    McDic's Competitive Programming Template / Requires C++11 or later.
    Last edited: 2020-08-18 15:10 UTC+9
=============================================================================*/

// Custom define
#ifdef __McDic__ // Local testing
#define debugprintf(f_, ...) fprintf(stderr, f_, ##__VA_ARGS__)
#define debugflush() fflush(stdout),fflush(stderr)
#else // Submission version
#define debugprintf(f_, ...) NULL
#define debugflush() NULL
#endif
//#define raise(f_, ...) {debugprintf(f_, ##__VA_ARGS__); exit(1000);}
//#define raiseif(condition, f_, ...) if(condition) raise(f_, ##__VA_ARGS__)

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

// Random
#include <random>
std::mt19937_64 mersenne_twister(std::chrono::steady_clock::now().time_since_epoch().count());

/*=============================================================================
    Implement your own code below. 
    To modify number of test cases, please look at Main function.
=============================================================================*/

namespace McDicCP{

	namespace SCC{

		typedef std::vector<std::vector<int>> graphtype;

        // DFS Phase 1: Sort the order of vertices
		void DFS1(std::vector<bool> &visited, std::vector<int> &stack, 
                  const graphtype &graph, int now){
            visited[now] = true;
            for(int neighbor: graph[now]) if(!visited[neighbor])
                DFS1(visited, stack, graph, neighbor);
            stack.push_back(now);
		}

        // DFS Phase 2: Search inverse vertices
		void DFS2(std::vector<bool> &visited, std::vector<int> &group,
                  const graphtype &graph_t, int now){
            group.push_back(now);
            visited[now] = true;
            for(int neighbor: graph_t[now]) if(!visited[neighbor])
                DFS2(visited, group, graph_t, neighbor);
		}

		// Perform SCC, where each vector implies group of vertices.
		std::vector<std::vector<int>> SCC(
                int n, const graphtype &graph, const graphtype &graph_t){

            // DFS Step 1
            std::vector<bool> visited(n, false);
            std::vector<int> stack;
			for(int i=0; i<n; i++) if(!visited[i]){
                DFS1(visited, stack, graph, i);
            }

            // DFS Step 2
            visited = std::vector<bool>(n, false);
            std::reverse(stack.begin(), stack.end());
            std::vector<std::vector<int>> groups;
            for(int root: stack){
                if(visited[root]) continue;
                groups.push_back({});
                DFS2(visited, groups.back(), graph_t, root);
            }

            return groups;
		}

		// Transpose given graph.
		graphtype transpose(int n, const graphtype &graph){
			graphtype result(n);
			for(int now=0; now<n; now++) for(int next: graph[now])
				result[next].push_back(now);
			return result;
		}
	}

    // Main solver function
    void solve(){
        
        // Enter graph
        int v, e; scanf("%d %d", &v, &e);
        SCC::graphtype edges(v);
        while(e--){
            int v1, v2; scanf("%d %d", &v1, &v2);
            v1--, v2--;
            edges[v1].push_back(v2);
        }

        // SCC
        SCC::graphtype edges_t = SCC::transpose(v, edges);
        std::vector<std::vector<int>> scc = SCC::SCC(v, edges, edges_t);

        // Sort SCC
        for(size_t i=0; i<scc.size(); i++) std::sort(scc[i].begin(), scc[i].end());
        std::sort(scc.begin(), scc.end(), 
            [](const std::vector<int> &v1, const std::vector<int> &v2) -> bool{
                return v1[0] < v2[0];
            });

        // Print
        printf("%d\n", (int)scc.size());
        for(size_t i=0; i<scc.size(); i++){
            std::sort(scc[i].begin(), scc[i].end());
            for(int num: scc[i]) printf("%d ", num+1);
            printf("-1\n");
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
    for(int t=1; t<=testcases; t++) McDicCP::solve();

#ifdef __McDic__ // Local testing
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = endTime - startedTime;
    debugprintf("\n"); for(int i=0; i<80; i++) debugprintf("="); debugprintf("\n");
    debugprintf("Total %.8lf ms used.\n", duration.count());
#endif
    return 0;
}