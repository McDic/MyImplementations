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
        virtual S identity() = 0;
        virtual S op(S a, S b) = 0;
    };

    template<typename S> class MonoidSum: MonoidBase<S>{ public:
        S identity(){ return 0;}
        S op(S a, S b){ return a+b;}
    };

    template<typename S, class Monoid> class SegmentTree{ public:

        int size;
        std::vector<S> arr;
        Monoid monoid;

        SegmentTree(int size){
            this->size = size;
            monoid = Monoid();
            arr = std::vector<S>(size << 1, monoid.identity());
        }

        void set(int location, S value){
            location += size;
            arr[location] = value;
            while(location > 1){
                location >>= 1;
                arr[location] = monoid.op(arr[location << 1], arr[location << 1 | 1]);
            }
        }

        S get(int left, int right){
            S left_answer = monoid.identity(), right_answer = monoid.identity();
            for(left += size, right += size+1; left < right; left >>= 1, right >>= 1){
                if(left & 1) left_answer = monoid.op(left_answer, arr[left++]);
                if(right & 1) right_answer = monoid.op(arr[--right], right_answer);
            }
            return monoid.op(left_answer, right_answer);
        }
    };

    // Main solver function
    void solve(int tnum){
        
        int n, m, k; std::cin >> n >> m >> k;
        SegmentTree<lld, MonoidSum<lld>> segtree(n);
        for(int i=0; i<n; i++){
            lld num; std::cin >> num;
            segtree.set(i, num);
        }
        for(int i=0; i<m+k; i++){
            int querytype; std::cin >> querytype;
            if(querytype == 1){
                int index; lld num;
                std::cin >> index >> num;
                segtree.set(index-1, num);
            }
            else{
                int start, end;
                std::cin >> start >> end;
                std::cout << segtree.get(start-1, end-1) << '\n';
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