/*
	Author: McDic
	Description: Implementation of Knuth-Morris-Pratt ALgorithm. 
	             This is crazy idea. I respect Dr.K, Dr.M, and Dr.P.
*/

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

// Prefix == Suffix? Time complexity = O(len + sum(log(??))) ~= O(len).
std::vector<int> presuf(const std::string &line){
	
	std::vector<int> result(line.length(), 0); // result[i] = longest length of 'prefix == suffix' in result[0~i].
	int fork = 0;
	for(int head=1; head < line.length(); head++){
		// s[0~head] = [A][B][A]C (ABA = s[0 ~ head-1], A = s[0 ~ result[head-1] - 1], fork = len(A) = result[head-1], C = s[head])
		// We need to see if AB[0] == AC because no longer cases are possible.
		// If B[0] == C then result[head] = ++fork. (Set AB)
		// Otherwise do 'fork = result[fork-1]' to decompose A into XYX where len(X) is the biggest possible.
		//           thus we will see [X][Y][X][B][X][Y][X]C recursively to compare XY[0] and XC.
		//           Since AB[0] != AC, XYXB[0] != XYXC. Also, since XY != YX, XC will be the longest possible(or smaller again).
		while(fork > 0 && line[head] != line[fork]) fork = result[fork-1];
		if(line[head] == line[fork]){
			fork++;
			result[head] = fork;
		}
		else result[head] = 0;
		// printf("result[head=%d] = %d, fork = %d\n", head, result[head], fork);
	} return result;
}

// Find locations of occurence of target in origin using KMP
std::vector<int> find(const std::string &origin, const std::string &target){

	// Define
	std::vector<int> results, presuf_target = presuf(target);
	int head = 0, offset = 0;
	
	// Currently origin[head ~ head+offset-1] == target[0 ~ offset-1]
	while(head < origin.length()){
		
		// Break it
		if(head+offset >= origin.length()) break;
		
		// If origin[head+offset] == target[offset]
		//    offset++ to compare next element.
		else if(origin[head+offset] == target[offset]){ // Expand
			offset++;
			if(offset == target.length()){ 
				// Case found; Add current head into result and compare B vs T instead of BCB vs T because we find that BCB == T.
				results.push_back(head);
				head += offset - presuf_target[offset-1];
				offset = presuf_target[offset-1];
			}
		}
		
		// Else: Currently ABCBX, Change comparing BX vs T instead of BCBX vs T because we found that BCBX != T.
		else if(offset == 0) head++;
		else{
			head += offset - presuf_target[offset-1];
			offset = presuf_target[offset-1];
		}
	} return results;
}

int main(void){
	
	std::string origin, need;
	std::cout << "Please enter your origin string: "; std::getline(std::cin, origin);
	std::cout << "Please enter your target string: "; std::getline(std::cin, need);
	std::vector<int> found = find(origin, need);
	printf("Total %d occurences found.\nIndex: ", found.size());
	for(auto index: found) printf("%d ", index); printf("\n");
	return 0;
}
