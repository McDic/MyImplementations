/*
	Author: McDic
	Description: n-dimensional dynamic segment tree with lazy propagation.
*/

#include <stdio.h>
#include <vector>

// typedef
typedef long long int lld;

// Update Directions
const int UD_DIRECT      = 0xAAAAAA01;
const int UD_LEFTONLY    = 0xAAAAAA02;
const int UD_RIGHTONLY   = 0xAAAAAA03;
const int UD_BOTHPARTIAL = 0xAAAAAA04;
const int UD_ERROR       = 0xAAAAAA00;

// Reminder
const lld R = 1000 * 1000 * 1000 + 7;

// Describe indices
std::vector<lld> maxSizesByDimensions;

// n-dimensional segment tree. (n>=1)
class node{
public:
	
	// Attributes
	lld left, right; // Coverage range in n-th dimension
	int dimension; // Dimension
	lld val, sum; // Node's value
	node *parent, *lc, *rc; // Parent and childs
	node *lower_dimension; // Lower dimension nodes
	
	// Constructor
	node(int dimension, lld left, lld right, node *parent){
		
		// Set attributes
		this->dimension = dimension;
		this->left = left, this->right = right;
		this->val = 0, this->sum = 0;
		this->parent = parent;
		
		// Lower dimension tree setting
		if(dimension > 0) // Complex dimension; Need to make lower_dimension
			lower_dimension = new node(dimension-1, 1, maxSizesByDimensions[dimension-1], NULL);
		else if(dimension == 0) // Primitive dimension
			lower_dimension = NULL;
		else{ // Invalid dimension
			printf("Invalid dimension %d detected.\n", dimension);
			throw;
		}
		
		// Set this node as leaf first; New nodes will be created later.
		this->lc = NULL, this->rc = NULL;
	}
	
	// Update Direction
	bool isLeaf(){return lc == NULL && rc == NULL;} // Is this node leaf?
	bool isIn(int i){return left <= i && i <= right;} // Is given index fit in this node's range?
	int updateDirection(lld lbound, lld rbound){ // Describe update direction
		lld mid = (left + right) >> 1;
		if(!(left <= lbound && lbound <= rbound && rbound <= right)) return UD_ERROR; // Error
		else if(left == lbound && rbound == right) return UD_DIRECT; // Direct update
		else if(lbound <= mid && rbound <= mid) return UD_LEFTONLY; // Left-only update
		else if(mid < lbound && mid < rbound) return UD_RIGHTONLY; // Right-only update
		else return UD_BOTHPARTIAL; // lbound <= mid < rbound; Both-partial update
	}
	
	// Add value in range
	void insert(const std::vector<std::pair<lld, lld>> &ranges, const lld &value){
		lld lbound = ranges[dimension].first, rbound = ranges[dimension].second;
		int dir = updateDirection(lbound, rbound);
		if(dir == -1) throw;
		else if(dir == 0){
			
		}
	}
};

int main(void){
	
	return 0;
}
