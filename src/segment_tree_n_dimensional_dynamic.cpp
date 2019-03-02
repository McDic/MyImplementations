/*
	Author: McDic
	Description: n-dimensional dynamic segment tree with lazy propagation.
*/

#include <stdio.h>
#include <vector>
#include <string>
#include <utility>

// typedef
typedef long long int lld;
typedef std::pair<lld, lld> pll;

// [EDIT THIS] Customized feature example =============================================================================
// Feature should identify the single node, so it can be fit in n-dimensional segment tree.
// The minimal qualification for implementing method are implemented in this class.
class feature{
private:
	
	// Attributes
	static const lld R = 1000 * 1000 * 1000 + 7;
	lld left, right, sizeFactor; // Range
	lld val, sum; // Represented value

public:
	
	// Constructor; Should be initialized by only range bound and dimension
	feature(lld lbound, lld rbound, int dimension, const std::vector<pll> *rangeByDimensions){
		left = lbound, right = rbound;
		val = 0, sum = 0;
		sizeFactor = (rbound - lbound + 1) % R;
		for(int i=0; i<dimension; i++){
			sizeFactor *= (rangeByDimensions->at(i).second - rangeByDimensions->at(i).first + 1) % R;
			sizeFactor %= R;
		}
	}
	
	// Current feature := Update(Current, val)
	// Note that this method necessarily don't need to update cumulative feature.
	void query(const std::string &queryType, void *arg){
		if(queryType == "add"){
			val += *(lld *)arg; val %= R;
		}
		else{
			printf("Given query type is not supported");
			throw;
		}
	}
	
	// Current feature := Integrated(Current, LeftChild, RightChild, LowerDimension);
	// Note that feature pointers can be NULL.
	void integrate(feature *leftchild, feature *rightchild, feature *lower_dimension){
		sum = val * sizeFactor % R;
		if(leftchild != NULL) sum += leftchild->sum, sum %= R;
		if(rightchild != NULL) sum += rightchild->sum, sum %= R;
		if(lower_dimension != NULL) sum += lower_dimension->sum * ((right-left+1) % R), sum %= R;
	}
};

// n-dimensional segment tree with customized feature. Written by @McDic (github).
template <class featureClass> class node{
private:
	
	// Base attributes; Used to operate segment tree
	std::vector<pll> *rangeByDimensions; // Max range by dimensions. {(min0, max0), (min1, max1), ...}
	lld left, right; // Coverage range in n-th dimension
	int dimension; // Dimension
	node *parent, *lc, *rc; // Parent and childs
	node *lower_dimension; // Lower dimension nodes
	bool willPropagate; // Used to lazy propagation
	
	// Update Directions
	static const char UD_DIRECT = 0xF0, UD_LEFT = 0xF1, UD_RIGHT = 0xF2, UD_BOTH = 0xF3, UD_ERROR = 0xFF;
	
	// Custom feature; Should have implemented constructor, update, integration method
	featureClass feature;

	// Create child
	void createChild(bool isLeft){
		lld mid = (left+right)/2;
		if(isLeft && lc == NULL) lc = new node(dimension, left, mid, this, rangeByDimensions);
		else if(!isLeft && rc == NULL) rc = new node(dimension, mid+1, right, this, rangeByDimensions);
	}
	
	// Upper propagation
	void upperPropagation(){
		
		// Skip if child node is planning to propagate
		if(lc != NULL && lc->willPropagate) return;
		else if(rc != NULL && rc->willPropagate) return;
		else if(lower_dimension != NULL && lower_dimension->willPropagate) return;
		willPropagate = false;
		
		// Upper propagate
		if(parent != NULL) parent->upperPropagation();
	}
	
	// Recursive update function; Use lbound and rbound for current dimension and use ranges for lower dimension
	void privateUpdate(const std::string &queryType, void *newFeature, 
	                   lld lbound, lld rbound, const std::vector<pll> &ranges){
		char dir = updateDirection(lbound, rbound);
		if(dir == UD_ERROR) throw; // Error
		else if(dir == UD_DIRECT){ // Direct update
			if(dimension == 0){ // Primitive dimension reached; Update directly
				feature.query(queryType, newFeature);
				upperPropagation();
			}
			else lower_dimension->privateUpdate(queryType, newFeature, // Go deeper
			     ranges[dimension-1].first, ranges[dimension-1].second, ranges);
		}
		else if(dir == UD_LEFT){ // Left only
			createChild(true); lc->willPropagate = true; 
			lc->privateUpdate(queryType, newFeature, lbound, rbound, ranges);
		}
		else if(dir == UD_RIGHT){ // Right only
			createChild(false); rc->willPropagate = true; 
			rc->privateUpdate(query, newFeature, lbound, rbound, ranges);
		}
		else{ // Both partial
			createChild(true), lc->willPropagate = true;
			createChild(false), rc->willPropagate = true;
			lc->privateUpdate(query, newFeature, lbound, lc->right, ranges);
			rc->privateUpdate(query, newFeature, rc->left, rbound, ranges);
		}
	}

public:
	
	// Constructor
	node(int dimension, lld left, lld right, node *parent, std::vector<pll> *rangeByDimensions){
		
		// Set base attributes
		this->rangeByDimensions = rangeByDimensions;
		this->dimension = dimension;
		this->left = left, this->right = right;
		this->parent = parent;
		lc = NULL, rc = NULL; // New child nodes will be created later.
		willPropagate = false;
		feature = featureClass(left, right, dimension, rangeByDimensions);
		
		// Lower dimension tree setting
		if(dimension > 0) // Complex dimension; Need to make lower_dimension
			lower_dimension = new node(dimension-1, rangeByDimensions->at(dimension-1).first, 
			                           rangeByDimensions->at(dimension-1).second, this, rangeByDimensions);
		else if(dimension == 0) lower_dimension = NULL; // Primitive dimension
		else{ // Invalid dimension
			printf("Invalid dimension %d detected.\n", dimension);
			throw;
		}
	}
	
	// Destructor
	~node(){
		
		// Delete link between parent and this
		if(parent != NULL){
			if(parent->lc == this) parent->lc = NULL;
			else if(parent->rc == this) parent->rc = NULL;
			else if(parent->lower_dimension == this) parent->lower_dimension = NULL;
		}
		
		// Recursive deletion
		if(lc != NULL) delete lc;
		if(rc != NULL) delete rc;
		if(lower_dimension != NULL) delete lower_dimension;
	}
	
	// Find direction
	char updateDirection(lld lbound, lld rbound){ // Describe update direction
		lld mid = (left + right) >> 1;
		if(!(left <= lbound && lbound <= rbound && rbound <= right)) return UD_ERROR; // Error
		else if(left == lbound && rbound == right) return UD_DIRECT; // Direct update
		else if(lbound <= mid && rbound <= mid) return UD_LEFT; // Left-only update
		else if(mid < lbound && mid < rbound) return UD_RIGHT; // Right-only update
		else return UD_BOTH; // lbound <= mid < rbound; Both-partial update
	}
	
	// Public update
	void query(const std::string &queryType, void *newFeature, std::vector<pll> ranges){
		if(dimension >= ranges.size()) throw; // Invalid ranges provided
		willPropagate = true;
		privateUpdate(queryType, newFeature, ranges[dimension].first, ranges[dimension].second, ranges);
	}
};

int main(void){
	
	return 0;
}
