/*=============================================================================
    McDic's Competitive Programming Template / Requires C++11 or later.
    Last edited: 2020-08-16 15:13 UTC+9
=============================================================================*/

// Custom define
#ifdef __McDic__ // Local testing
#define debugprintf(f_, ...) printf(f_, ##__VA_ARGS__)
#else // Submission version
#define debugprintf(f_, ...) -1
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
    Implement your own code below. To modify number of test cases, 
    please look at Main function.
=============================================================================*/

namespace McDicCP{

    typedef long long int lld;

    // Segment Tree with lazy propagation.
    class SegmentTree{
        
        // Properties
        protected:
        
            // Built-ins
            int size, offset, limit; // Size of segment tree
            bool *temporaryCheck;

            // Features; 1 is the root index of internal array.
            // For any non-leaf index, feature is represented as "virtual".
            int *feature_xor, *feature_seglength, *delayedOperations;

        // Constructor of segtree node, supporting [0, 1, 2, ..., size-1].
        public: SegmentTree(int size){
            
            // Built-in initialization
            this->size = size;
            this->offset = size; // Leaves are offset, offset+1, ..., offset+size-1
            this->limit = this->size + this->offset;
            temporaryCheck = new bool[limit];
            for(int i=0; i<limit; i++) temporaryCheck[i] = false;

            // Feature initialization
            this->feature_xor = new int[limit];            
            this->feature_seglength = new int[limit];
            this->delayedOperations = new int[limit];
            for(int i=0; i<limit; i++){
                this->feature_xor[i] = 0;
                this->delayedOperations[i] = 0;
            }
            for(int i = limit-1; i >= this->offset; i--)
                this->feature_seglength[i] = 1;
            for(int i = this->offset - 1; i > 0; i--){
                this->feature_seglength[i] = 
                    this->feature_seglength[i << 1] +
                    this->feature_seglength[i << 1 | 1];
            }
        }

        // Is given index leaf?
        protected: bool isLeaf(int index){
            return this->offset <= index && index < 2*this->offset;
        }

        // Generate lookup for [lbound, rbound] range. Assume 2*L > R.
        protected: static std::vector<int> generateLookup(int lbound, int rbound){
            if(rbound >= 2 * lbound){
                debugprintf("2*L (%d) <= R (%d) in generateLookup.\n", 2 * lbound, rbound);
                fflush(stdout); throw;
            }
            std::vector<int> result, rtemp;
            for(rbound++; lbound < rbound; lbound >>= 1, rbound >>= 1){
                if(lbound & 1) result.push_back(lbound++);
                if(rbound & 1) rtemp.push_back(--rbound);
            }
            while(!rtemp.empty()){
                result.push_back(rtemp.back());
                rtemp.pop_back();
            }
            return result;
        }

        // [IMPLEMENT HERE] Apply single operation on single index.
        protected: inline void stashSingleOperation(int index, int operation){
            this->feature_xor[index] ^= operation * (this->feature_seglength[index] % 2);
            if(!this->isLeaf(index)) this->delayedOperations[index] ^= operation;
        }

        // [IMPLEMENT HERE] Lazily propagate and refresh.
        protected: inline void refresh(int index){
            //debugprintf("  Refreshing node %d\n", index);
            if(!this->isLeaf(index)){
                this->stashSingleOperation(index << 1, this->delayedOperations[index]);
                this->stashSingleOperation(index << 1 | 1, this->delayedOperations[index]);
                this->delayedOperations[index] = 0;
                this->feature_xor[index] = 
                    this->feature_xor[index << 1] ^ this->feature_xor[index << 1 | 1];
            }
        }

        // Propagate all delayed operations on indices' ancestors and refresh in upper direction again.
        protected: std::vector<int> guidePropagateDirections(const std::vector<int> &lookUp, bool goingUp){
            for(int index: lookUp){
                this->temporaryCheck[index] = true;
                for(int i2 = index >> 1; i2 > 0; i2 >>= 1){
                    if(this->temporaryCheck[i2]) break;
                    this->temporaryCheck[i2] = true;
                }
            }
            std::vector<int> result;
            if(goingUp){ // Going up from lower
                for(int index: lookUp){
                    for(int i2 = index; i2 > 0; i2 >>= 1){
                        if(!this->isLeaf(i2) &&
                          (this->temporaryCheck[i2 << 1] || this->temporaryCheck[i2 << 1 | 1]))
                            break;
                        this->temporaryCheck[i2] = false;
                        result.push_back(i2);
                    }
                }
            }
            else{ // Going down from root
                std::vector<int> stack = {1};
                while(!stack.empty()){
                    int index = stack.back(); stack.pop_back();
                    if(index >= this->limit) continue;
                    this->temporaryCheck[index] = false;
                    result.push_back(index);
                    if(this->temporaryCheck[index << 1]) stack.push_back(index << 1);
                    if(this->temporaryCheck[index << 1 | 1]) stack.push_back(index << 1 | 1);
                }
            }
            return result;
        }

        // [MODIFY HERE] Edit given range with operation. lbound, rbound must be in [0, 1, ..., n-1].
        public: void update(int lbound, int rbound, int operation){
            //debugprintf("Updating [lbound %d, rbound %d, op %d]..\n", lbound, rbound, operation);
            std::vector<int> lookUp = this->generateLookup(lbound + this->offset, rbound + this->offset);
            for(int index: this->guidePropagateDirections(lookUp, false)) this->refresh(index);
            for(int index: lookUp) this->stashSingleOperation(index, operation);
            for(int index: this->guidePropagateDirections(lookUp, true)) this->refresh(index);
        }

        // [MODIFY/IMPLEMENT HERE] Search given range.
        public: int search(int lbound, int rbound){
            int answer = 0;
            std::vector<int> lookUp = this->generateLookup(lbound + this->offset, rbound + this->offset);
            for(int index: this->guidePropagateDirections(lookUp, false)) this->refresh(index);
            for(int index: lookUp) answer ^= this->feature_xor[index];
            return answer;
        }

        // [MODIFY HERE] Print segment tree status.
        public: void print(int index = 1, int tablevel = 0){
            for(int i=0; i<tablevel; i++) debugprintf("\t");
            debugprintf("Node #%d", index);
            if(this->isLeaf(index)) debugprintf("(Leaf index %d)", index - this->offset);
            debugprintf(": ");
            debugprintf("[xor %d, delayed %d, seglen %d]\n", 
                this->feature_xor[index],
                this->isLeaf(index) ? 0 : this->delayedOperations[index],
                this->feature_seglength[index]);
            if((index << 1) < this->offset + size) this->print(index << 1, tablevel + 1);
            if((index << 1 | 1) < this->offset + size) this->print(index << 1 | 1, tablevel + 1);
        }

        // Destructor
        ~SegmentTree(){

            // Built-ins
            delete[] this->temporaryCheck;

            // Features
            delete[] this->feature_xor; 
            delete[] this->feature_seglength;
            delete[] this->delayedOperations;
        }

    };

    void solve(){
        
        int n, m; scanf("%d", &n);
        std::vector<int> init(n);
        for(int i=0; i<n; i++) scanf("%d", &init[i]);
        debugprintf("Inputted completed.\n"); fflush(stdout);
        std::vector<int> brute = init;

        SegmentTree SegTree(n);
        for(int i=0; i<n; i++) SegTree.update(i, i, init[i]);
        //debugprintf("Initialization completed.\n"); fflush(stdout);
        //SegTree.print();
        //debugprintf("=====================================================\n");

        scanf("%d", &m);
        for(int q=0; q<m; q++){
            int opcode, left, right; scanf("%d %d %d", &opcode, &left, &right);
            //debugprintf("Doing %d-th query: opcode %d, left %d, right %d\n", 
            //    q+1, opcode, left, right);
            if(opcode == 1){
                int amount; scanf("%d", &amount);
                //debugprintf("Query amount = %d\n", amount);
                SegTree.update(left, right, amount);
                //for(int i=left; i<=right; i++) brute[i] ^= amount;
            }
            else{
                int result = SegTree.search(left, right);
                printf("%d\n", result);
                /*int bruteresult = 0;
                for(int i=left; i<=right; i++) bruteresult ^= brute[i];
                debugprintf("(Brute = %d)\n", bruteresult);
                if(result != bruteresult){
                    debugprintf("Wrong answer found at %d-th query:\n", q+1);
                    SegTree.print();
                    debugprintf("ERROR\n");
                    return;
                }*/
            }
            //SegTree.print();
            //debugprintf("brute = ["); for(int num: brute) debugprintf("%d, ", num); debugprintf("]\n");
            //debugprintf("=====================================================\n");
        }
        debugprintf("All finished, ok.\n");
        fflush(stdout);
    }

}

/*=============================================================================
    Main function
=============================================================================*/

// Main
int main(int argc, char **argv){
#ifdef __McDic__ // Local testing
    freopen("IO/input.txt", "r", stdin);
    printf("================================================\n");
    printf(" McDic's C++17 file execution for CP\n");
    printf("================================================\n");
    freopen("IO/output.txt", "w", stdout);
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