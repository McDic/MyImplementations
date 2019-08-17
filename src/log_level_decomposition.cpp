/*
    Author: McDic
    Description: Log level decomposition of range.
*/

#include <iostream>
#include <random>
#include <vector>
#include <utility>
#include <algorithm>

// Log level decomposition for [left, right]. Time complexity is O(log^2(right - left)).
// Return values are {(start, level), ...}, where each element describes [start, start + 2^level).
std::vector<std::pair<long long int, int>> logLevelDecompositionSlow(long long int left, long long int right, bool oneBased = false){
    if(oneBased) left--, right--; // Increase later
    if(left > right || left < 0) throw "undefined";
    std::vector<std::pair<long long int, int>> answer;
    while(left <= right){
        int lowestBit;
        for(lowestBit = 0; left + (1LL << (lowestBit+1)) - 1 <= right && lowestBit < 60; lowestBit++)
            if(left & (1LL << lowestBit)) break; // Out of left's max coverage range
        answer.push_back({left + (int)oneBased, lowestBit});
        left += (1LL << lowestBit);
    } return answer;
}

// Log level decomposition for [left, right]. Time complexity is O(log(right - left)).
// Return values are {(start, level), ...}, where each element describes [start, start + 2^level).
std::vector<std::pair<long long int, int>> logLevelDecomposition(long long int left, long long int right){
    if(left > right || left < 0) throw "undefined";
    std::vector<std::pair<long long int, int>> answer;
    int zoom_level = 0;
    while(left < right){
        if(left % 2 == 1){
            answer.push_back({left << zoom_level, zoom_level});
            left++;
        }
        if(right % 2 == 0){
            answer.push_back({right << zoom_level, zoom_level});
            right--;
        }
        left >>= 1, right >>= 1, zoom_level++;
    } 
    if(left == right) answer.push_back({left << zoom_level, zoom_level}); // Last position
    std::sort(answer.begin(), answer.end());
    return answer;
}

int main(void){

    FILE *out = fopen("VScode/IO/output.txt", "w");
    std::default_random_engine generator;
    std::uniform_int_distribution<long long int> distribution(1, 1LL<<20);

    long long int limit = 5000;
    for(long long int start = 0; start < limit; start++)
    for(long long int end = start; end < limit; end++){
        //if(iteration % 1000 == 0) printf("Iteration %d\n", iteration);
        if(start == end && start % 100 == 0) printf("start = %lld\n", start);
        //long long int start = distribution(generator), end = distribution(generator);
        if(start > end){ long long int temp = start; start = end; end = temp;}
        auto result = logLevelDecomposition(start, end);

        bool validated = true;
        if(result.front().first != start) validated = false;
        else if(result.back().first + (1LL << result.back().second) != end+1) validated = false;
        else{
            for(int i=0; i<(int)result.size()-1; i++){
                if(result[i].first + (1LL << result[i].second) != result[i+1].first){
                    validated = false;
                    break;
                }
            }
        }

        if(!validated){
            fprintf(out, "Counter case found: [%lld, %lld]\n", start, end);
            for(auto interval: result) fprintf(out, "<start = %lld, level = %d>\n", interval.first, interval.second);
            return 0;
        }
    }
    fprintf(out, "No counter case found on brute force for [0, %lld)\n", limit);
    return 0;
}