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

    typedef long long int lld;
    const lld inf = 1LL << 50;

    class SparseTable{
        private: // Properties 
        int n;
        std::vector<std::vector<lld>> feature_min, feature_max;

        // Constructor
        public: SparseTable(const std::vector<lld> &base){
            // Resize into n = 2**k
            n = (int)base.size(); int log = 0;
            while((1<<log) < (int)base.size()) log++;
            log++;
            
            // Feature initialization
            feature_min.clear(); feature_min.resize(log, std::vector<lld>(n, inf));
            feature_min[0] = base;
            feature_max.clear(); feature_max.resize(log, std::vector<lld>(n, -inf));
            feature_max[0] = base;
            initialize();
        }

        // Initialize base feature.
        private: void initialize(){
            // feature[level][offset] = operation(base[offset : offset + 2 ** level])
            for(int log=1; log < (int)feature_min.size(); log++){
                int halflen = 1 << (log-1);
                for(int offset=0; offset + halflen < n; offset++){
                    feature_min[log][offset] = std::min(feature_min[log-1][offset], feature_min[log-1][offset + halflen]);
                    feature_max[log][offset] = std::max(feature_max[log-1][offset], feature_max[log-1][offset + halflen]);
                }
            }
        }

        // Return min[left:right], max[left:right]
        public: std::pair<lld, lld> query(int left, int right){
            if(left >= right) return {inf, -inf};
            int log = 0;
            while((1 << log) < right - left) log++; 
            if(log) log--;
            lld min = std::min(feature_min[log][left], feature_min[log][right - (1<<log)]);
            lld max = std::max(feature_max[log][left], feature_max[log][right - (1<<log)]);
            return {min, max};
        }
    };

    // Sort and remove all duplicated elements
    template <class t> void uniquesort(std::vector<t> &vec){
        std::sort(vec.begin(), vec.end());
        auto it = std::unique(vec.begin(), vec.end());
        vec.erase(it, vec.end());
    }

    // Main solver function
    void solve(){
        
        int n; scanf("%d", &n);
        std::vector<std::pair<lld, lld>> points;
        for(int i=0; i<n; i++){
            lld x, y; scanf("%lld %lld", &x, &y);
            points.push_back({x << 1, y << 1});
        }
        uniquesort<std::pair<lld, lld>>(points);
        n = (int)points.size();

        std::vector<lld> xs, ys;
        for(int i=0; i<n; i++){
            xs.push_back(points[i].first);
            ys.push_back(points[i].second);
        }
        SparseTable table(ys);

        lld lowbound = 0, highbound = 1LL << 35;
        while(lowbound < highbound){
            
            lld maxdist = (lowbound + highbound) >> 1; // = max dist
            bool possible = false;

            int except_front = 0, except_back = 0; // include point[front:back]
            for(int i=0; i<(int)xs.size() && !possible; i++){
                lld x_center = xs[i] + maxdist;
                while(except_back < n && 
                      points[except_back].first <= x_center + maxdist) except_back++;
                while(except_front < except_back &&
                      points[except_front].first < x_center - maxdist) except_front++;

                if(except_front == 0 && except_back == n) possible = true;
                else{
                    auto frontqueried = table.query(0, except_front), backqueried = table.query(except_back, n);
                    lld ymin = std::min(frontqueried.first , backqueried.first);
                    lld ymax = std::max(frontqueried.second, backqueried.second);
                    if(ymax - ymin <= 2 * maxdist) possible = true;
                }
            }

            if(possible) highbound = maxdist;
            else lowbound = maxdist + 1;
        }
        printf("%.1lf", (double)lowbound / 2.0);
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