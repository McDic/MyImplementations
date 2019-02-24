/*
	Author: McDic
	Description: Implementation of SoE(Sieve of Eratosthenes).
*/

#include <stdio.h>
#include <assert.h>
#include <vector>

// Attributes
std::vector<bool> isPrime;
std::vector<int> primes;

int main(void){
	
	// Get input
	printf("Input upper bound to calculate all primes: ");
	int n; scanf("%d", &n);
	assert(2<=n);
	
	// Sieve of Eratosthenes
	primes.clear();
	isPrime = std::vector<bool>(n+1, true);
	isPrime[0] = false; isPrime[1] = false;
	for(int i=2; i<=n; i++){
		if(isPrime[i]){
			for(int j=2*i; j<=n; j+=i) isPrime[j] = false;
			primes.push_back(i);
		}
	}
	
	// Show result
	printf("Primes under %d: ", n); 
	for(auto p: primes) printf("%d ", p);
	printf("\n");
	
	return 0;
}
