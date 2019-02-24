/*
	Author: McDic
	Description: Calculation of power operation with divide and conquer.
*/

#include <stdio.h>

// Shorter name
typedef long long int lld;

// Power calculation with divide and conquer
lld power(lld a, lld n, lld r){ // a^n % r
	if(n==0) return 1LL;
	lld half = power(a, n>>1, r); // a^(n/2) % r
	if(n%2==0) return half*half%r;
	else return half*half%r*a%r;
}

int main(void){
	
	// Get input and process queries
	printf("Input number of queries: ");
	int q; scanf("%d", &q);
	while(q--){
		printf("Input a, n, and r to calculate a^n%%r: ");
		lld a, n, r; scanf("%lld %lld %lld", &a, &n, &r);
		printf("%lld ^ %lld %% %lld = %lld\n", a, n, r, power(a, n, r));
	}
	return 0;
}
