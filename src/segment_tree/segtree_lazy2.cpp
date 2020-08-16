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
    class Node{
        
        // Properties
        protected:
        
            // Built-ins
            int lcover, rcover;
            Node *parent, *lchild, *rchild;

            // Features
            std::queue<lld> delayedOperations; // std::queue<feature> your_operation
            lld num, sum;

        // Constructor of segtree node.
        public: Node(int L, int R, Node* parent){

            // Basic initialization
            this->lcover = L; this->rcover = R;
            this->parent = parent;
            if(this->lcover == this->rcover){
                this->lchild = NULL;
                this->rchild = NULL;
            }
            else{
                int mid = (L+R)/2;
                this->lchild = new Node(L, mid, this);
                this->rchild = new Node(mid+1, R, this);
            }
            while(!this->delayedOperations.empty()) this->delayedOperations.pop();

            // Feature initialization. For leaf it's real, otherwise it's virtual.
            this->num = 0, this->sum = 0;
        }

        // Is this node leaf?
        protected: bool isLeaf(){return this->lchild == NULL && this->rchild == NULL;}

        // Is given index covered by this node?
        protected: bool isIn(int index){return this->lcover <= index && index <= this->rcover;}
        
        // Determine direction of lower propagation with given range
        protected: int updateDirection(int lbound, int rbound){
            if(this->lcover == lbound && this->rcover == rbound) return 0; // Direct update
            else if(this->isLeaf() || lbound > rbound) return -1; // Error; lbound > rbound or Non-direct update at leaf node
            else if(this->lchild->isIn(lbound) && this->lchild->isIn(rbound)) return 1; // Left child only
            else if(this->rchild->isIn(lbound) && this->rchild->isIn(rbound)) return 2; // Right child only
            else if(this->lchild->isIn(lbound) && this->rchild->isIn(rbound)) return 3; // Both childs update
            else return -1; // Error; out of range, etc
        }

        // IMPLEMENT HERE: Apply single operation VIRTUALLY on interval.
        protected: void applySingleOperation(lld operation){
            this->num += operation;
            this->sum += operation * (this->rcover - this->lcover + 1);
            debugprintf("Now [%d, %d] has num = %lld, sum = %lld\n", 
                lcover, rcover, num, sum);
        }

        // MODIFY HERE: Lazily propagate to child nodes.
        protected: void lazyPropagation(){
            const bool isThisLeaf = this->isLeaf();
            const bool isLeftLeaf = isThisLeaf || this->lchild->isLeaf();
            const bool isRightLeaf = isThisLeaf || this->rchild->isLeaf();
            while(!this->delayedOperations.empty()){
                lld operation = this->delayedOperations.front();
                this->delayedOperations.pop();
                if(isThisLeaf) this->applySingleOperation(operation); // If leaf then apply immediately
                else{ // Push operation to child nodes
                    debugprintf("Propagating operation %lld from [%d, %d] to [%d, %d] and [%d, %d]\n",
                        operation, lcover, rcover, 
                        lchild->lcover, lchild->rcover, rchild->lcover, rchild->rcover);
                    this->lchild->delayedOperations.push(operation);
                    if(!isLeftLeaf) this->lchild->applySingleOperation(operation);
                    this->rchild->delayedOperations.push(operation);
                    if(!isRightLeaf) this->rchild->applySingleOperation(operation);
                }
            }
            this->refresh();
        }

        // IMPLEMENT HERE: Refreshing the node.
        protected: void refresh(){
            if(this->isLeaf()){
                this->sum = this->num;
            }
            else{
                this->num = 0;
                this->sum = this->lchild->sum + this->rchild->sum;
            }
        }

        // MODIFY HERE: Update on query.
        public: void update(int lbound, int rbound, lld operation){
            const int dir = this->updateDirection(lbound, rbound);
            switch(dir){
                case 0:
                    this->delayedOperations.push(operation);
                    this->lazyPropagation();
                    break;
                case 1:
                    this->lazyPropagation();
                    this->lchild->update(lbound, rbound, operation);
                    break;
                case 2:
                    this->lazyPropagation();
                    this->rchild->update(lbound, rbound, operation);
                    break;
                case 3:
                    this->lazyPropagation();
                    this->lchild->update(lbound, this->lchild->rcover, operation);
                    this->rchild->update(this->rchild->lcover, rbound, operation);
                    break;
                default:
                    debugprintf("Unknown direction %d on update(%d, %d, %lld)\n", 
                        dir, lbound, rbound, operation);
                    fflush(stdout); throw;
            }
            this->refresh();
        }

        // MODIFY HERE; Get query result between left bound and right bound.
        public: lld search(int lbound, int rbound){
            const int dir = this->updateDirection(lbound, rbound);
            debugprintf("Looking search(%d, %d) on [%d, %d]: dir = %d\n", 
                lbound, rbound, lcover, rcover, dir);
            this->lazyPropagation();
            this->refresh();
            switch(dir){
                case 0: return this->sum;
                case 1: return this->lchild->search(lbound, rbound);
                case 2: return this->rchild->search(lbound, rbound);
                case 3: return 
                    this->lchild->search(lbound, this->lchild->rcover) + 
                    this->rchild->search(this->rchild->lcover, rbound);
                default:
                    debugprintf("Unknown direction %d on search(%d, %d) at [%d, %d]\n", 
                        dir, lbound, rbound, this->lcover, this->rcover); 
                    fflush(stdout); throw;
            }
        }

        public: void print(int tablevel){
            for(int i=0; i<tablevel; i++) debugprintf("\t");
            debugprintf("[%d, %d]: ", this->lcover, this->rcover);
            debugprintf("num %lld, sum %lld, ", this->num, this->sum);
            debugprintf("%d delayed operations\n", (int)this->delayedOperations.size());
            if(!this->isLeaf()){
                this->lchild->print(tablevel + 1);
                this->rchild->print(tablevel + 1);
            }
        }

        // Recursive destructor
        public: void terminate(){
            if(!this->isLeaf()){
                this->lchild->terminate();
                this->rchild->terminate();
            } delete this;
        }

    };

    void solve(){
        
        int n, m, k; scanf("%d %d %d", &n, &m, &k);
        std::vector<lld> init(n);
        for(int i=0; i<n; i++) scanf("%lld", &init[i]);

        Node *root = new Node(1, n, NULL);
        for(int i=0; i<n; i++) root->update(i+1, i+1, init[i]);
        //root->print(0);

        for(int q=0; q<m+k; q++){
            debugprintf("Doing %d-th query:\n", q+1);
            int opcode, left, right; scanf("%d %d %d", &opcode, &left, &right);
            if(opcode == 1){
                lld amount; scanf("%lld", &amount);
                root->update(left, right, amount);
                //root->print(0);
            }
            else{
                printf("%lld\n", root->search(left, right));
                //root->print(0);
            }
            debugprintf("======\n");
        }

        root->terminate();
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