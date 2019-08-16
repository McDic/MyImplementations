/*
    Author: McDic
    Description: Log level decomposition of range.
*/

#include <iostream>
#include <vector>
#include <utility>

// Log level decomposition for [left, right]. 
// Return values are {(start, level), ...}, where each element describes [start, start + 2^level).
std::vector<std::pair<long long int, int>> logLevelDecomposition(long long int left, long long int right, bool oneBased = false){
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

int main(void){

    freopen("VScode/IO/output.txt", "w", stdout);
    auto result = logLevelDecomposition(16, 30);
    for(auto interval: result) printf("[%lld, %lld] (d = %lld)\n", 
        interval.first, interval.first + (1LL << interval.second) - 1, 1LL << interval.second);
    return 0;
}