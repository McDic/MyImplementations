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

#include <complex>

namespace McDicCP{

    typedef std::complex<double> codo;
    void fft(std::vector<codo> &poly, bool invert){
        
        // Resize
        int n = (int)poly.size(), log = 1;
        while((1<<log) < n) log++;
        n = (1<<log); poly.resize(n);

        // Index swap
        auto reverse_bit = [&log](int index) -> int{
            int s = 0;
            for(int b=0; b<log; b++) 
                if(index & (1<<b)) s |= (1<<(log-1-b));
            return s;
        };
        for(int i=0; i<n; i++){
            int j = reverse_bit(i);
            if(i < j) std::swap(poly[i], poly[j]);
        }
        /*
        for (int i = 1, j = 0; i < n; i++) {
            int bit = n >> 1;
            for (; j & bit; bit >>= 1) j ^= bit;
            j ^= bit;
            if (i < j) std::swap(a[i], a[j]);
        }
        */

        // Calculate from small length
        const int inversefactor = (invert ? -2: 2);
        for(int l=2; l<=n; l<<=1){
            const double angle = inversefactor * (double)M_PI / l;
            const codo wfactor(cos(angle), sin(angle));
            for(int offset=0; offset<n; offset+=l){
                codo w = 1;
                for(int i=0; i<l/2; i++){
                    codo front = poly[offset+i], 
                         back  = poly[offset+i+l/2] * w;
                    poly[offset+i]     = front + back;
                    poly[offset+i+l/2] = front - back;
                    w *= wfactor;
                }
            }
        }
        if(invert) for(int i=0; i<n; i++) poly[i] /= n;
    }

    template <class t> std::vector<t> multiply(
        const std::vector<t> &a, const std::vector<t> &b){

        // Create temporary vector
        std::vector<codo> poly1(a.begin(), a.end()),
                          poly2(b.begin(), b.end());
        size_t n=1; 
        while(n < a.size() + b.size()) n <<= 1;
        poly1.resize(n), poly2.resize(n);

        // FT -> Convolution -> IFT
        fft(poly1, false); fft(poly2, false);
        for(size_t i=0; i<n; i++) poly1[i] *= poly2[i];
        fft(poly1, true);

        // Return
        std::vector<t> result(n);
        for(size_t i=0; i<n; i++) result[i] = round(poly1[i].real());
        return result;
    }

    // Main solver function
    void solve(){

        int n; scanf("%d", &n);
        std::vector<int> a(n), b(n);
        for(int i=0; i<n; i++) scanf("%d", &a[i]);
        for(int i=0; i<n; i++) scanf("%d", &b[n-1-i]);
        std::vector<int> multiplied = multiply<int>(a, b);
        multiplied.push_back(0);
        int max = -1;
        for(int i=0; i<n; i++) max = std::max(max, 
            multiplied[i] + multiplied[i+n]);
        printf("%d\n", max);
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