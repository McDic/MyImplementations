/*=============================================================================
    McDic's Competitive Programming Template / Requires C++11 or later.
    Last edited: 2020-08-18 15:10 UTC+9
=============================================================================*/

// Custom define
#ifdef __McDic__ // Local testing
#define debugprintf(f_, ...) fprintf(stderr, f_, ##__VA_ARGS__)
#define debugflush() fflush(stdout);fflush(stderr)
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
    Implement your own code below. To modify number of test cases, 
    please look at Main function.
=============================================================================*/

namespace McDicCP{

    typedef long long int lld;

    // Implementation of vector with fixed max size.
    template<class t> class FixedVector{ public:

        // Built-ins
        int size, maxSize;
        t *objects = NULL;

        // Constructor
        FixedVector(){this->resizeMax(0);}
        FixedVector(int maxSize){this->resizeMax(maxSize);}
        FixedVector(int maxSize, int size, t defaultValue){this->resizeMax(maxSize, size, defaultValue);}

        // Access operator
        t& operator[] (int index){
            return this->objects[index];
        }

        // Resize
        void resizeMax(int maxSize){
            this->~FixedVector();
            this->maxSize = maxSize;
            this->objects = (this->maxSize > 0 ? new t[this->maxSize] : NULL);
            this->size = 0;
        }
        void resizeMax(int maxSize, int size, t defaultValue){
            this->resizeMax(maxSize);
            this->size = size;
            for(int i=0; i<size; i++) this->objects[i] = defaultValue;
        }

        // Clear all internal contents.
        void popAll(){this->size = 0;}

        // Stack operations
        void push(t obj){this->objects[(this->size)++] = obj;}
        t pop(){return this->objects[--(this->size)];}

        // Destructor
        ~FixedVector(){
            if(this->objects != NULL) delete[] this->objects;
        }
    };

    // Segment Tree with lazy propagation.
    class SegmentTree{
        
        // Properties
        protected:

            // =======================================================================        
            // Built-ins
            int size, offset, limit; // Size of segment tree
            FixedVector<bool> lazyCheck;
            static const int maxLogs = 500;
            FixedVector<int> tempVector, lookUp, ancestors; // Used for propagations.

            // =======================================================================
            // Features; 1 is the root index of internal array.
            // For any non-leaf index, feature is represented as "virtual".
            FixedVector<int> feature_xor, delayedOperations;
            FixedVector<bool> feature_seglength_isOdd;

        // Constructor of segtree node, supporting [0, 1, 2, ..., size-1].
        public: SegmentTree(int size){
            
            // Built-in constants
            this->size = size;
            this->offset = size; // Leaves are offset, offset+1, ..., offset+size-1
            this->limit = this->size + this->offset;

            // Built-in arrays used for internals
            this->lazyCheck.resizeMax(this->limit, this->limit, false);
            this->tempVector.resizeMax(maxLogs);
            this->lookUp.resizeMax(maxLogs);
            this->ancestors.resizeMax(maxLogs);

            // Feature initialization
            this->feature_xor.resizeMax(this->limit, this->limit, 0);            
            this->feature_seglength_isOdd.resizeMax(this->limit, this->limit, true);
            this->delayedOperations.resizeMax(this->limit, this->limit, 0);
            for(int i = this->offset - 1; i > 0; i--){
                this->feature_seglength_isOdd[i] = 
                    (this->feature_seglength_isOdd[i << 1] ^ this->feature_seglength_isOdd[i << 1 | 1]);
            }
        }

        // Is this leaf?
        protected: inline bool isLeaf(int index){return this->offset <= index && index < this->limit;}

        // Generate lookup for [lbound, rbound] range, in left to right order. Assumes 2*L > R.
        protected: void generateLookup(int lbound, int rbound){
            /*if(rbound >= 2 * lbound){
                debugprintf("2*L (%d) <= R (%d) in generateLookup.\n", 2 * lbound, rbound);
                debugflush(); throw;
            }*/
            this->tempVector.popAll();
            this->lookUp.popAll();
            for(rbound++; lbound < rbound; lbound >>= 1, rbound >>= 1){
                if(lbound & 1) this->lookUp.push(lbound++);
                if(rbound & 1) this->tempVector.push(--rbound);
            }
            while(this->tempVector.size > 0) this->lookUp.push(this->tempVector.pop());
        }

        // [IMPLEMENT HERE] Apply single operation on single index.
        protected: inline void stashSingleOperation(int index, int operation){
            if(this->feature_seglength_isOdd[index]) this->feature_xor[index] ^= operation;
            if(!this->isLeaf(index)) this->delayedOperations[index] ^= operation;
        }

        // [IMPLEMENT HERE] Lazily propagate and refresh.
        protected: inline void refresh(int index){
            //debugprintf("  Refreshing node %d\n", index);
            // [MODIFY BELOW]
            if(this->isLeaf(index)) return;
            this->stashSingleOperation(index << 1, this->delayedOperations[index]);
            this->stashSingleOperation(index << 1 | 1, this->delayedOperations[index]);
            // [IMPLEMENT BELOW]
            this->delayedOperations[index] = 0;
            this->feature_xor[index] = 
                this->feature_xor[index << 1] ^ this->feature_xor[index << 1 | 1];
        }

        // List all ancestors of lookup in upper direction order(leaf -> root).
        protected: void setAncestors(){
            for(int i=0; i<this->lookUp.size; i++){
                for(int i2 = this->lookUp[i]; i2 > 0; i2 >>= 1){
                    if(this->lazyCheck[i2]) break;
                    this->lazyCheck[i2] = true;
                }
            }
            this->ancestors.popAll();
            for(int i=0; i<lookUp.size; i++){
                for(int i2 = lookUp[i]; i2 > 0; i2 >>= 1){
                    if(!this->isLeaf(i2) && (this->lazyCheck[i2 << 1] || this->lazyCheck[i2 << 1 | 1]))
                        break;
                    this->lazyCheck[i2] = false;
                    this->ancestors.push(i2);
                }
            }
        }

        // [MODIFY HERE] Edit given range with operation. lbound, rbound must be in [0, 1, ..., n-1].
        public: void update(int lbound, int rbound, int operation){
            //debugprintf("Updating [lbound %d, rbound %d, op %d]..\n", lbound, rbound, operation); debugflush();
            this->generateLookup(lbound + this->offset, rbound + this->offset);
            this->setAncestors();
            for(int i = this->ancestors.size; i >= 0; i--) this->refresh(this->ancestors[i]);
            for(int i = 0; i < this->lookUp.size; i++) this->stashSingleOperation(this->lookUp[i], operation);
            for(int i = 0; i < this->ancestors.size; i++) this->refresh(this->ancestors[i]);
        }

        // [MODIFY/IMPLEMENT HERE] Search given range.
        public: int search(int lbound, int rbound){
            int answer = 0;
            this->generateLookup(lbound + this->offset, rbound + this->offset);
            this->setAncestors();
            for(int i = this->ancestors.size; i >= 0; i--) this->refresh(this->ancestors[i]);
            // [IMPLEMENT BELOW] Combine result and feature.
            for(int i = 0; i < this->lookUp.size; i++) answer ^= this->feature_xor[this->lookUp[i]];
            return answer;
        }

        // [MODIFY HERE] Print segment tree status.
        /*public: void print(int index = 1, int tablevel = 0){
            for(int i=0; i<tablevel; i++) debugprintf("\t");
            debugprintf("Node #%d", index);
            if(this->isThisLeaf[index]) debugprintf("(Leaf index %d)", index - this->offset);
            debugprintf(": ");
            debugprintf("[xor %d, delayed %d, seglen %d]\n", 
                this->feature_xor[index],
                this->isThisLeaf[index] ? 0 : this->delayedOperations[index],
                this->feature_seglength_isOdd[index]);
            if((index << 1) < this->offset + size) this->print(index << 1, tablevel + 1);
            if((index << 1 | 1) < this->offset + size) this->print(index << 1 | 1, tablevel + 1);
        }*/
    };

    void solve(){
        
        int n, m; scanf("%d", &n);
        std::vector<int> init(n);
        for(int i=0; i<n; i++) scanf("%d", &init[i]);
        debugprintf("Inputted completed.\n"); fflush(stdout);

        SegmentTree SegTree(n);
        for(int i=0; i<n; i++) SegTree.update(i, i, init[i]);
        debugprintf("Initialization completed.\n"); fflush(stdout);
        //SegTree.print();
        debugprintf("=====================================================\n"); debugflush();

        scanf("%d", &m);
        for(int q=0; q<m; q++){
            int opcode, left, right; scanf("%d %d %d", &opcode, &left, &right);
            if(opcode == 1){
                int amount; scanf("%d", &amount);
                SegTree.update(left, right, amount);
            }
            else{
                int result = SegTree.search(left, right);
                printf("%d\n", result);
            }
            //SegTree.print();
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