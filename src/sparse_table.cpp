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

    class SparseTable{
        private: // Properties 
        int n;
        std::vector<std::vector<int>> feature_min, feature_max;

        // Constructor
        public: SparseTable(const std::vector<int> &base){
            // Resize into n = 2**k
            n = 1; int log = 0;
            while(n < (int)base.size()) n <<= 1, log++;
            
            // Feature initialization
            feature_min.clear(); feature_min.resize(log, std::vector<int>(n, 0));
            for(int i=0; i<(int)base.size(); i++) feature_min[0][i] = base[i];
            feature_max = feature_min;
            initialize();
        }

        // Initialize base feature.
        protected: void initialize(){
            // feature[level][offset] = operation(base[offset : offset + 2 ** level])
            for(int log=1; (1<<log) <= n; log++){
                int len = 1 << log;
                for(int offset=0; offset + len < n; offset++){
                    feature_min[log][offset] = std::min(feature_min[log-1][offset], feature_min[log-1][offset + (len >> 1)]);
                    feature_max[log][offset] = std::max(feature_max[log-1][offset], feature_max[log-1][offset + (len >> 1)]);
                }
            }
        }

        // Return min[left:right], max[left:right]
        public: std::pair<int, int> query(int left, int right){
            int log = 1;
            while((1 << log) <= right - left) log++; 
            log--;
            int min = std::min(feature_min[log][left], feature_min[log][right - (1<<log)]),
                max = std::max(feature_max[log][left], feature_max[log][right - (1<<log)]);
            return {min, max};
        }
    };

    // Main solver function
    void solve(){
        
        int n, q; scanf("%d %d", &n, &q);
        std::vector<int> num(n);
        for(int i=0; i<n; i++) scanf("%d", &num[i]);
        
        SparseTable table(num);
        for(int i=0; i<q; i++){
            int left, right; scanf("%d %d", &left, &right);
            auto info = table.query(left-1, right);
            printf("%d %d\n", info.first, info.second);
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