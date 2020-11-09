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

    typedef unsigned long long int lld;

    lld pow(const lld a, lld x, const lld mod){
        if(x==0) return 1;
        lld half = pow(a, x>>1, mod);
        if(x&1) return half * half % mod * a % mod;
        else return half * half % mod;
    }

    // https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test
    // Return true if (maybe) prime, false if composite.
    bool MillerRabinPrimality(lld n){

        lld r = 0, d = n-1; // n = 2**r * d + 1
        while(!(d&1)) r++, d>>=1;
        debugprintf("MillerRabin: n = %lld, r = %lld, d = %lld\n", n, r, d);

        lld a_candidates[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
        for(lld a: a_candidates){
            if(n == a) return true;
            lld x = pow(a%n, d, n);
            if(x==1) continue;
            for(int i=1; i<r && x != n-1; i++) x = x * x % n;
            if(x == n-1) continue;
            else return false; // composite
        }
        return true; // Maybe prime
    }

    // Main solver function
    void solve(){

        int n; std::cin >> n;
        int ans = 0;
        while(n--){
            lld s; std::cin >> s;
            //if(MillerRabinCPAlgo::MillerRabin(2*s+1)){
            if(MillerRabinPrimality(2*s+1)){
                ans++;
                debugprintf("Candidate: %lld\n", s);
            }
        }
        printf("%d\n", ans);
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