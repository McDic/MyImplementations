// Standard libraries
#include <stdio.h>
#include <vector>
#include <queue>
#include <functional>

// Typedef
typedef long long int lld;

// Segment Tree with lazy propagation.
class SegTree{

    // ====================================================================================
    // Properties

    // Internal structure and flags
    private:
    typedef std::pair<int, int> pii;
    int level;
    std::vector<std::vector<bool>> willPropagate;

    // Features
    private:
    std::vector<std::vector<lld>> num, sum;

    // ====================================================================================
    // Constructors

    // Constructor
    public: SegTree(int interval_length){

        // Set basic properties
        this->level = 1;
        while((1 << (this->level)) < interval_length) this->level++;
        this->willPropagate = this->makeVector<bool>(this->level, false);

        // Initialize features
        this->__initialize_features();
    }
    // Initialize feature vectors.
    private: void __initialize_features(){
        this->num = this->sum = this->makeVector<lld>(this->level, 0);
    }

    // ====================================================================================
    // Helper functions

    // Make 2D vector with fit size.
    private: template<typename v> static std::vector<std::vector<v>> makeVector(int level, v defaultValue){
        std::vector<std::vector<v>> result = std::vector<std::vector<v>>();
        while(level >= 0) result.push_back(std::vector<v>(1 << (level--), defaultValue));
        return result;
    }

    // Lookup indices generator. Return [(level, index), ...].
    // 0 1 2 3 4 5 6 7
    // 0 . 1 . 2 . 3 .
    // 0 . . . 1 . . .
    // 0 . . . . . . .
    private: static std::vector<std::pair<int, int>> generateLookup(int left, int right){
        std::vector<std::pair<int, int>> result;
        int level = 0;
        while(left <= right){
            bool same = (left == right);
            if(left & 1) // left % 2 == 1
                result.push_back({level, left++});
            if(!(right & 1)) // right % 2 == 0
                result.push_back({level, right--});
            if(same) break;
            left >>= 1, right >>= 1, level++;
        } return result;
    }

    public: void representation(){
        for(int level = 0; level <= this->level; level++){
            printf("Level %2d: ", level);
            for(int i=0; i<willPropagate[level].size(); i++){
                printf(" [%3lld, %3lld], ", num[level][i], sum[level][i]);
            } printf("\n");
        }
    }

    // ====================================================================================
    // Core logic stuffs of lazy propagation

    // Set willPropagate flag for given lookup and its parents.
    private: void setFlag(std::vector<pii> &lookup){
        for(auto info: lookup){
            for(int level = info.first, index = info.second;
                level <= this->level && !this->willPropagate[level][index];
                level++, index >>= 1){
                this->willPropagate[level][index] = true;
            }
        }
    }

    // Total propagation.
    private: void totalPropagate(int level, int index){
        if(level > 0 && this->willPropagate[level][index]){ // Go for marked nodes only
            this->singlePropagate(level, index);
            this->totalPropagate(level-1, index << 1);
            this->totalPropagate(level-1, index << 1 | 1);
        }
        this->willPropagate[level][index] = false;
        this->singleRefresh(level, index);
    }

    // ====================================================================================
    // Update and search based on lazy propagation

    // Update function with lazy propagation.
    public: void update(int left, int right, lld value){
        std::vector<pii> lookup = this->generateLookup(left, right);
        for(pii info: lookup){
            int level = info.first, index = info.second;
            this->singleUpdate(level, index, value);
        }
        this->setFlag(lookup);
        this->totalPropagate(this->level, 0);
    }

    // Search integrated feature in given range.
    public: lld search(int left, int right){
        auto lookup = this->generateLookup(left, right);
        this->setFlag(lookup);
        this->totalPropagate(this->level, 0);
        
        // Actual feature integration
        lld result = 0;
        for(pii info: lookup){
            int level = info.first, index = info.second;
            result += this->sum[level][index];
        } return result;
    }

    // ====================================================================================
    // Feature related

    // Single node update, used in update method.
    private: void singleUpdate(int level, int index, lld value){
        this->num[level][index] += value;
    }

    // Single propagation used in totalPropagate. Implement feature modification only.
    private: void singlePropagate(int level, int index){
        this->num[level-1][index << 1] += this->num[level][index];
        this->num[level-1][index << 1 | 1] += this->num[level][index];
        this->num[level][index] = 0;
    }

    // Refresh current node's status. Implement feature modification only.
    private: void singleRefresh(int level, int index){
        { // Common refresh
            this->sum[level][index] = this->num[level][index] << level;
        }
        if(level > 0){ // Non-leaf refresh
            this->sum[level][index] += 
                this->sum[level-1][index << 1] + this->sum[level-1][index << 1 | 1];
        }
        else{ // Leaf refresh

        }
    }

};

// Main
int main(int argc, char **argv){
#ifdef __McDic__ // Local testing I/O
    freopen("IO/input.txt", "r", stdin);
    freopen("IO/output.txt", "w", stdout);
#endif

    int n, m, k; scanf("%d %d %d", &n, &m, &k);
    std::vector<lld> num(n);
    for(int i=0; i<n; i++) scanf("%lld", &num[i]);

    SegTree *root = new SegTree(n);
    for(int i=0; i<n; i++) root->update(i, i, num[i]);
    //root->representation();

    for(int q=0; q<m+k; q++){
        //printf("%d-th update\n", q+1);
        int command, left, right; scanf("%d %d %d", &command, &left, &right);
        left--, right--;
        if(command == 1){
            lld val; scanf("%lld", &val);
            root->update(left, right, val);
        }
        else{
            printf("%lld\n", root->search(left, right));
        }
        //root->representation();
        //for(int i=0; i<100; i++) printf("="); printf("\n\n");
    }
    return 0;
}