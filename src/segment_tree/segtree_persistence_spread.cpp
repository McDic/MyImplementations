// Custom define
#ifdef __McDic__ // Local testing
#define debugprintf(f_, ...) printf(f_, ##__VA_ARGS__)
#else // Submission version
#define debugprintf(f_, ...) -1
#endif
#define raise(f_, ...) {debugprintf(f_, ##__VA_ARGS__); fflush(stdout); exit(1000);}
#define raiseif(condition, f_, ...) if(condition) raise(f_, ##__VA_ARGS__)

// Standard libraries
#include <stdio.h>
#include <iostream>
#include <vector>

typedef long long int lld;

// Persistent Segment Tree with Destructured Feature Spreading.
class PersistLazyNode{ public:

// Attributes ////////////////////////////////////////

    // Node range attributes
    int lRange, rRange;
    PersistLazyNode *lChild, *rChild, *parent;
    bool deleteFlag;

    // Features
    lld added; // equivalently added amount under this node
    lld total; // total sum under this node

// Methods ////////////////////////////////////////

    // Construction
    PersistLazyNode(int l, int r, 
      PersistLazyNode *parent = NULL,
      PersistLazyNode *lOldChild = NULL, 
      PersistLazyNode *rOldChild = NULL){

        // Base initialization
        this->lRange = l, this->rRange = r;
        raiseif(l > r, "What the hell is going on? l = %d, r = %d.", l, r);
        this->deleteFlag = false;

        // Feature initialization
        this->added = 0;
        this->total = 0;

        // Tree connection
        this->parent = parent;
        if(l < r){
            int mid = (l+r)/2;
            this->lChild = lOldChild == NULL ? new PersistLazyNode(l, mid, this, NULL, NULL) : lOldChild;
            this->rChild = rOldChild == NULL ? new PersistLazyNode(mid+1, r, this, NULL, NULL) : rOldChild;
        }
        else this->lChild = this->rChild = NULL;
    }

    // Refresh this node's feature.
    void singleRefresh(){
        this->total = (this->rRange - this->lRange + 1) * added;
        if(this->lChild != NULL) this->total += this->lChild->total;
        if(this->rChild != NULL) this->total += this->rChild->total;
    }

    // Make new updated version performed under [l, r] in this node(L - R).
    // Ensure l <= r.
    PersistLazyNode* update(int l, int r, int add){
        //debugprintf("ok, let's go (%d, %d) update on (%d, %d) node.\n",
        //    l, r, this->lRange, this->rRange);
        raiseif(!(this->lRange <= l && l <= r && r <= this->rRange), 
            "Range invalid: Tried (%d, %d) update on (%d, %d) node.",
            l, r, this->lRange, this->rRange);
        
        PersistLazyNode *newNode;
        if(this->lRange == l && this->rRange == r){ // Full update.
            newNode = new PersistLazyNode(this->lRange, this->rRange, 
                this->parent, this->lChild, this->rChild);
            newNode->added = this->added + add; // direct feature assignment
        }
        else if(l <= this->lChild->rRange && r <= this->lChild->rRange){
            PersistLazyNode *newLchild = this->lChild->update(l, r, add);
            newNode = new PersistLazyNode(this->lRange, this->rRange,
                this->parent, newLchild, this->rChild);
            newNode->added = this->added;
        }
        else if(this->rChild->lRange <= l && this->rChild->lRange <= r){
            PersistLazyNode *newRchild = this->rChild->update(l, r, add);
            newNode = new PersistLazyNode(this->lRange, this->rRange,
                this->parent, this->lChild, newRchild);
            newNode->added = this->added;
        }
        else{ // Partial update on non-leaf node.
            PersistLazyNode *newLchild = NULL, *newRchild = NULL;
            
            if(l <= this->lChild->rRange)
                newLchild = this->lChild->update(l, this->lChild->rRange, add);
            else newLchild = this->lChild;
            if(this->rChild->lRange <= r)
                newRchild = this->rChild->update(this->rChild->lRange, r, add);
            else newRchild = this->rChild;

            newNode = new PersistLazyNode(this->lRange, this->rRange, 
                this->parent, newLchild, newRchild);
            newNode->added = this->added; // direct feature assignment
        }
        newNode->singleRefresh();
        return newNode;
    }

    // Search integrated features on range [l, r].
    lld search(int l, int r){
        raiseif(!(this->lRange <= l && l <= r && r <= this->rRange), 
            "Range invalid: Tried (%d, %d) search on (%d, %d) node.",
            l, r, this->lRange, this->rRange);


        // Full fit
        if(this->lRange == l && this->rRange == r) return this->total;

        // Partial fit
        lld feature = this->added * (r-l+1);
        if(l <= this->lChild->rRange && r <= this->lChild->rRange)
            return feature + this->lChild->search(l, r);
        else if(this->rChild->lRange <= l && this->rChild->lRange <= r)
            return feature + this->rChild->search(l, r);
        else{
            return feature + this->lChild->search(l, this->lChild->rRange)
                + this->rChild->search(this->rChild->lRange, r);
        }
    }

    // Memory destruction
    void setDeleteFlag(std::vector<PersistLazyNode*> *trashlist = NULL){
        if(!this->deleteFlag){
            if(trashlist != NULL) trashlist->push_back(this);
            this->deleteFlag = true;
            if(this->lChild != NULL) this->lChild->setDeleteFlag(trashlist);
            if(this->rChild != NULL) this->rChild->setDeleteFlag(trashlist);
        }
    }

    // Debug
    void print(int tabLevel = 0, const char *tabs = "  "){
        
        // Base prefix
        for(int i=0; i<tabLevel; i++) debugprintf("%s", tabs);
        debugprintf("Node [%d, %d]: ", this->lRange, this->rRange);
        
        // Features
        debugprintf("added %lld, total %lld\n", this->added, this->total);

        // Child propagation
        if(this->lChild != NULL) this->lChild->print(tabLevel+1, tabs);
        if(this->rChild != NULL) this->rChild->print(tabLevel+1, tabs);
    }
};

// Main
int main(int argc, char **argv){
#ifdef __McDic__ // Local testing
    freopen("IO/input.txt", "r", stdin);
    printf("------------------------------------------------\n");
    printf(" McDic's C++17 file execution for CP\n");
    printf("------------------------------------------------\n");
    freopen("IO/output.txt", "w", stdout);
#endif



    return 0;
}