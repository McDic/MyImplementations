/*
	Author: McDic
	Description: Fast calculation of Euler function.
*/

#include <stdio.h>
#include <vector>

// Constants
typedef long long int lld;
const int limit = 30000005;

// Attributes
std::vector<bool> isPrime(limit, true);
std::vector<lld> primes;

// euler(x) = count(for i in range(1, x+1) if coprime(i,x))
lld euler(lld x){
	
	// Make copy
	lld x_copy = x;
	
	// Calculate prime divisors
	std::vector<lld> prime_divisors;
	for(auto p: primes){
		if(x <= 1) break;
		else if(x < limit && isPrime[x]){ // isPrime -> prime
			prime_divisors.push_back(x);
			break;
		}
		else if(x >= limit && (lld)p*p > x){ // Cannot find divisors -> prime
			prime_divisors.push_back(x);
			break;
		}
		else if(x%p == 0){
			prime_divisors.push_back(p);
			while(x%p == 0) x /= p;
		}
	}
	
	// Calculate with bitwise
	lld s = 0;
	for(lld it=0; it<(1LL<<prime_divisors.size()); it++){
		int included = 0;
		lld mul = 1;
		for(int digit=0; digit<prime_divisors.size(); digit++){
			if(it&(1LL<<digit)){
				included++;
				mul *= prime_divisors[digit];
			}
		}
		if(included%2 == 0) s += x_copy / mul;
		else s -= x_copy / mul;
	} return s;
}

int main(void){
	
	// Prime pre-calculation
	printf("Please wait few seconds until prime pre-calculation is completed.\n");
	isPrime[0] = false; isPrime[1] = false;
	for(int i=2; i<limit; i++){
		if(isPrime[i]){
			primes.push_back(i);
			for(int j=2*i; j<limit; j+=i) isPrime[j] = false;
		}
	}
	
	// Process queries
	while(true){
		printf("Input number to calculate euler function(0 to exit): ");
		lld num; scanf("%lld", &num);
		if(num==0) break;
		else if(1 <= num && num <= (lld)(limit-1)*(limit-1)) printf("Euler(%lld) = %lld\n", num, euler(num));
		else if(0 > num) printf("Negative number given, please try again.\n");
		else printf("Given number is too large(bigger than %lld), please try again.\n", (lld)(limit-1)*(limit-1));
	}
	
	return 0;
}
