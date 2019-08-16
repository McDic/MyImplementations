/*
    Author: McDic
    Description: Vectorized implementation of Segment Tree with lazy propagation.
*/

#include <iostream>
#include <vector>

// Segment Tree node.
class SegTree{

    // Base attributes
    public:
    int size;
    //std::vector<std::vector<bool>> willPropagate; // Lazy propagation

    // Feature attributes
    public:
    std::vector<std::vector<long long int>> num, sum;

    // Constructor
    public: SegTree(int size){

        // Base initialization
        this->size = size; // This segtree is describing interval [0, size)
        //__init_intervals<bool>(willPropagate, false); // Initialize willPropagation

        // Feature initialization
        __init_intervals<long long int>(num, 0);
        __init_intervals<long long int>(sum, 0);
    }
    // Initialize 2d vectors for size. 
    // [level][index] describes interval [index<<level, (index+1)<<level).
    private: template <typename feature> void __init_intervals(std::vector<std::vector<feature>> &feature_vector, feature default_value){
        feature_vector.clear();
        for(int level=0; (1<<level) <= size; level++){
            feature_vector.push_back(std::vector<feature>());
            for(int start=0; start + (1<<level) <= size; start += (1<<level))
                feature_vector.back().push_back(default_value);
        }
    }

    // Log level decomposition for [left, right]. 
    // Return values are {(start, level), ...}, where each element describes [start, start + 2^level).
    public: static std::vector<std::pair<int, int>> logLevelDecomposition(long long int left, long long int right, bool oneBased = false){
        if(oneBased) left--, right--; // Increase later
        if(left > right || left < 0) throw "undefined";
        std::vector<std::pair<int, int>> answer;
        while(left <= right){
            int lowestBit;
            for(lowestBit = 0; left + (1 << (lowestBit+1)) - 1 <= right && lowestBit < 25; lowestBit++)
                if(left & (1LL << lowestBit)) break; // Out of left's max coverage range
            answer.push_back({left + (int)oneBased, lowestBit});
            left += (1 << lowestBit);
        } return answer;
    }
    // Matching indices for log level decomposited interval. 
    // Input is [start, start + 2^level), Output is [level index, start index].
    public: std::pair<int, int> matchingIndices(int start, int level){
        //printf("Matching indices start = %d, level = %d, end = %d, size = %d\n", start, level, start + (1<<level), size);
        if(start % (1<<level) != 0) printf("OMG1\n"), throw "undefined"; // Level should be fit
        else if(start + (1<<level) - 1 >= size) printf("OMG2\n"), throw "undefined"; // Interval should be fit in size
        else return {level, start >> level};
    }

    // Single feature refresh
    protected: void singleRefresh(int level, int start_index){
        if(level == 0) sum[level][start_index] = num[level][start_index];
        else sum[level][start_index] = 
            num[level][start_index] * (1LL<<level) + 
            sum[level-1][start_index << 1] + 
            sum[level-1][(start_index << 1) | 1];
        //printf("Refreshed level = %d, start_index = %d\n", level, start_index);
    }
    // Single feature update
    protected: void singleUpdate(int level, int start_index, long long int add){
        num[level][start_index] += add;
        //printf("Updated level = %d, start_index = %d\n", level, start_index);
    }
    // Range feature update
    public: void rangeUpdate(int left, int right, long long int add){
        auto decompositedIntervals = SegTree::logLevelDecomposition(left, right);
        for(auto interval: decompositedIntervals){
            std::pair<int, int> indices = matchingIndices(interval.first, interval.second);
            int level = indices.first, startindex = indices.second;
            singleUpdate(level, startindex, add);
            while(level < (int)num.size() && startindex < (int)num[level].size()){
                //printf("Let's go refresh level %d, start_index %d\n", level, startindex);
                singleRefresh(level, startindex);
                level++; startindex >>= 1;
            }
        }
    }
    // Range search
    public: long long int rangeSearch(int left, int right){
        auto decompositedIntervals = SegTree::logLevelDecomposition(left, right);
        long long int answer = 0;
        for(auto interval: decompositedIntervals){
            //printf("Given interval start = %d, level = %d\n", (int)interval.first, (int)interval.second); fflush(stdout);
            std::pair<int, int> indices = matchingIndices(interval.first, interval.second);
            answer += sum[indices.first][indices.second];
        } return answer;
    }

    // Debug printing
    public: void print(){
        printf("=============================================\n");
        printf("Segment tree representation\n");
        for(int level=(int)num.size()-1; level>=0; level--){
            printf("Level %d:\n", level);
            for(int index=0; index<(int)num[level].size(); index++)
                printf("  [level %d, offset %d] -> Range [%d, %d): num %lld, sum %lld\n", 
                    level, index, index << level, (index+1) << level,
                    num[level][index], sum[level][index]
                );
        }
    }
};

int main(void){

    //freopen("VScode/IO/input.txt", "r", stdin);
    //freopen("VScode/IO/vectorized_segment_tree.in", "r", stdin);
    //freopen("VScode/IO/output.txt", "w", stdout);

    // Get input and construct segment tree
    int n, m, k; scanf("%d %d %d", &n, &m, &k);
    SegTree segment(n+1);
    for(int i=1; i<=n; i++){
        long long int num; scanf("%lld", &num);
        segment.rangeUpdate(i, i, num);
    } //segment.print();

    // Process queries
    for(int q=0; q<m+k; q++){
        //printf("=============================================\n");
        //printf("%d-th query:", q+1);
        int querytype, left, right; 
        scanf("%d %d %d", &querytype, &left, &right);
        if(querytype == 1){
            long long int add; scanf("%lld", &add);
            //printf(" [%d, %d], += %lld\n", left, right, add);
            segment.rangeUpdate(left, right, add);
            //segment.print();
        }
        else{
            //printf("compute [%d, %d]\n", left, right);
            printf("%lld\n", segment.rangeSearch(left, right));
        }
    }

    return 0;
}