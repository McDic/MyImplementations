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

    // Namespace for Suffix Array.
    namespace SuffixArray{

        typedef std::pair<std::pair<int, int>, int> PI3;

        // $ character, and all alphabets
        const std::string ALP = "$abcdefghijklmnopqrstuvwxyz";

        // Inner sorting with looking at offset characters.
        // Sort result [idxleft, idxright) where 
        // looking first $maxoffset characters only.
        void _innerSort(
                const std::string &s, std::vector<int> &SA, 
                std::vector<int> &group, int offset){

            int n = (int)s.length();
            std::vector<int> gcount(group.back() + 1, 0),
                             SA_inverse(SA.size());
            for(int i=0; i<n; i++) SA_inverse[SA[i]] = i;
            for(int g: group) gcount[g]++;
            std::vector<PI3> new_gid(n);
            for(int i=0; i<n; i++) 
                new_gid[i] = {{group[i], group[SA_inverse[(SA[i] + offset) % n]]}, SA[i]};

            // Back
            {
                std::vector<int> tempgcount = gcount;
                int nonceidx = 0, nonceg = 0;
                std::vector<std::vector<int>> idxs(group.back() + 1);
                for(int i=0; i<n; i++) idxs[new_gid[i].first.second].push_back(i);
                std::vector<int> idxs_flat;
                for(size_t i=0; i<idxs.size(); i++)
                    for(int idx: idxs[i]) idxs_flat.push_back(idx);
                std::vector<PI3> new_new_gid = new_gid;
                for(size_t i=0; i<idxs_flat.size(); i++)
                    new_new_gid[i] = new_gid[idxs_flat[i]];
                new_gid = new_new_gid;
            }
            // Front
            {
                std::vector<int> tempgcount = gcount;
                int nonceidx = 0, nonceg = 0;
                std::vector<std::vector<int>> idxs(group.back() + 1);
                for(int i=0; i<n; i++) idxs[new_gid[i].first.first].push_back(i);
                std::vector<int> idxs_flat;
                for(size_t i=0; i<idxs.size(); i++)
                    for(int idx: idxs[i]) idxs_flat.push_back(idx);
                std::vector<PI3> new_new_gid = new_gid;
                for(size_t i=0; i<idxs_flat.size(); i++)
                    new_new_gid[i] = new_gid[idxs_flat[i]];
                new_gid = new_new_gid;
            }

            for(int i=0; i<n; i++) SA[i] = new_gid[i].second;

            // Regroup
            group[0] = 0;
            for(int i=1; i<n; i++){
                group[i] = group[i-1];
                if(new_gid[i-1].first != new_gid[i].first) group[i]++;
            }
        }

        void _innerNaiveSort(
                const std::string &s, std::vector<int> &SA,
                int idxleft, int idxright, int minoffset, int maxoffset){
            int n = (int)s.length();
            std::vector<std::pair<std::string, int>> huh;
            for(int i=idxleft; i<idxright; i++){
                huh.push_back({"", SA[i]});
                for(int offset=minoffset; offset<maxoffset; offset++) 
                    huh.back().first += s[(offset + SA[i]) % n];
            }
            std::sort(huh.begin(), huh.end());
            for(int i=idxleft; i<idxright; i++) SA[i] = huh[i - idxleft].second;
        }

        // Check if cyclic suffix from i1 and i2 are same.
        bool isSame(const std::string &s, std::vector<int> &SA, int i1, int i2, int length){
            int n = (int)s.length();
            for(int offset = 0; offset < length; offset++)
                if(s[(SA[i1] + offset) % n] != s[(SA[i2] + offset) % n]) return false;
            return true;
        }

        // Construct suffix array of given string. SA[i] = i-th suffix in sorted order
        std::vector<int> ConstructSA(std::string s){
            s += '$';
            int n = (int)s.length(); // Length of string
            
            // Base case construction
            std::vector<int> result(n, 0), group(n, 0);
            int nonce = 0, nonce_g = 0;
            for(char c: ALP){
                bool detected = false;
                for(int i=0; i<n; i++) if(s[i] == c){
                    result[nonce] = i;
                    detected = true;
                    group[nonce] = nonce_g;
                    nonce++;
                }
                if(detected) nonce_g++;
            }

            // Sort
            for(int offset=1; ; offset <<= 1){
                _innerSort(s, result, group, offset);
                if(group.back() == (int)group.size() - 1) break;
            }
            result.erase(result.begin());
            return result;
        }

        // Construct LCP based on SA. LCP[x] = Longest Common Prefix Length of x-th and (x+1)-th suffix
        std::vector<int> ConstructLCP(const std::string &s, const std::vector<int> &SA){
            int n = (int)SA.size();
            std::vector<int> SAinverse(n, 0); // Inverse of suffix array
            for(int i=0; i<n; i++) SAinverse[SA[i]] = i;

            std::vector<int> result(n-1, 0);
            // banana: a -> ana -> anana -> banana -> na -> nana
            // ... => ana vs anana => na vs nana => a vs ana (by position)
            for(int i1 = 0, offset = 0; i1 < n; i1++, offset = std::max(0, offset-1)){
                if(SAinverse[i1] == n-1) continue; // This position is the last suffix
                for(int i2 = SA[SAinverse[i1] + 1]; s[i1 + offset] == s[i2 + offset]; offset++);
                result[SAinverse[i1]] = offset;
            } return result;
        }
    }

    // Main solver function
    void solve(){
        
        std::string s; std::cin >> s;
        std::vector<int> SA = SuffixArray::ConstructSA(s);
        std::vector<int> LCP = SuffixArray::ConstructLCP(s, SA);
        for(size_t i=0; i<SA.size(); i++) printf("%d%c", SA[i]+1, i+1 == SA.size() ? '\n' : ' ');
        printf("x"); for(int num: LCP) printf(" %d", num); printf("\n");
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
    // std::cin >> testcases;
    for(int t=1; t<=testcases; t++) McDicCP::solve();

#ifdef __McDic__ // Local testing
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = endTime - startedTime;
    debugprintf("\n"); for(int i=0; i<80; i++) debugprintf("="); debugprintf("\n");
    debugprintf("Total %.8lf ms used.\n", duration.count());
#endif
    return 0;
}