/*
	Author: McDic
	Description: Implementation of O(sqrt(n)) prime expanding.
*/

#include <stdio.h>
#include <assert.h>
#include <vector>

// Constants
typedef long long int lld;

// Attributes
std::vector<bool> isPrime = {false, false, true};
std::vector<lld> primes = {2};

// Expand isPrime
void expand_sqrt(const int limit){
	while(isPrime.size() < limit){
		isPrime.push_back(true);
		lld num = isPrime.size() - 1;
		for(auto p: primes){
			if(p*p > num) break;
			else if(num%p == 0){
				isPrime[num] = false;
				break;
			}
		}
		if(isPrime[num]) primes.push_back(num);
	}
}

int main(void){
	
	// Get input
	printf("Input upper bound to calculate all primes: ");
	int n; scanf("%d", &n); assert(1<n);
	
	// Expand and show result
	expand_sqrt(n);
	printf("Primes under %d: ", n); 
	for(auto p: primes) printf("%lld ", p);
	printf("\n");
	return 0;
}
