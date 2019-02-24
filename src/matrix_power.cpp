/*
	Author: McDic
	Description: Matrix multiplication and fast power
*/

#include <stdio.h>
#include <assert.h>
#include <vector>

// Shorter name
typedef long long int lld;
lld R = 1000 * 1000 * 1000 + 7;

// Matrix class
class squarematrix{
public:
	
	// Attributes
	std::vector<std::vector<lld>> mat;
	
	// Constructors
	squarematrix(int size, bool shouldI = true){ // Elementary matrix constructor
		this->mat = std::vector<std::vector<lld>>(size);
		for(int i=0; i<size; i++){
			this->mat[i] = std::vector<lld>(size, 0);
			if(shouldI) this->mat[i][i] = 1; // shouldI = true -> I, false -> ZERO
		}
	}
	squarematrix(const std::vector<std::vector<lld>> &anothermat){ // Vector copy constructor; Must be square
		this->mat = anothermat;
		for(int i=0; i<this->mat.size(); i++) // Size assertion
			assert(this->mat.size() == this->mat[i].size());
	}
	
	// Multiplication
	squarematrix operator *(const squarematrix &another){
		assert(this->mat.size() == another.mat.size());
		int size = this->mat.size();
		squarematrix result = squarematrix(size, false);
		for(int row=0; row<size; row++){
			for(int col=0; col<size; col++){
				for(int k=0; k<size; k++){
					result.mat[row][col] += this->mat[row][k] * another.mat[k][col];
					result.mat[row][col] %= R;
				}
			}
		}
		return result;
	}
	
	// Power
	squarematrix power(lld p){ // this ^ p % R
		if(p==0) return squarematrix(this->mat.size());
		squarematrix half = this->power(p>>1);
		if(p%2==0) return half*half;
		else return half*half*(*this);
	}
};

int main(void){
	
	// Get input
	printf("Input the size of matrix: ");
	int n; scanf("%d", &n);
	
	// Input by row
	squarematrix mat = squarematrix(n);
	for(int i=0; i<n; i++){
		printf("Input %d numbers for %d-th row: ", n, i+1);
		for(int j=0; j<n; j++) scanf("%lld", &mat.mat[i][j]);
	}
	
	// Get power
	printf("Input the power you want: ");
	lld p; scanf("%lld", &p); assert(p >= 0);
	squarematrix powered = mat.power(p);
	printf("Powered result:\n");
	for(int i=0; i<n; i++){
		printf("  "); 
		for(int j=0; j<n; j++) printf("%10lld ", powered.mat[i][j]);
		printf("\n");
	}
	return 0;
}
