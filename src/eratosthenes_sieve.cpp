/*
	Author: McDic
	Description: Implementation of SoE(Sieve of Eratosthenes).
*/

#include <stdio.h>
#include <assert.h>
#include <vector>

// Attributes
typedef long long int lld;
std::vector<bool> isPrime;
std::vector<int> primes;

int main(void){
	
	// Get input
	printf("Input upper bound to calculate all primes: ");
	int n = 20000000; //scanf("%d", &n);
	assert(2<=n);
	
	// Sieve of Eratosthenes
	primes.clear();
	isPrime = std::vector<bool>(n+1, true);
	isPrime[0] = false; isPrime[1] = false;
	for(int i=2; i<=n; i++){
		if(isPrime[i]) primes.push_back(i);
		for(int j=0; j<primes.size() && (lld)i*primes[j] <= (lld)n; j++){
			isPrime[i*primes[j]] = false;
			if(i % primes[j] == 0) break;
		}
	}
	
	// Show result
	printf("Total %d primes under %d (showing only first 1000th): ", primes.size(), n); 
	for(int i=0; i<1000 && i<primes.size(); i++) printf("%d ", primes[i]);
	printf("\n");
	
	return 0;
}
