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

// Abstract base of feature.
template <class rangeClass> class baseFeature{
public:
	
	rangeClass left, right;
	
	// Constructor
	baseFeature(rangeClass lbound, rangeClass rbound){
		
	}
	
	// Partial feature generated from current feature.
	feature()
	
};

// [EDIT THIS] Customized feature example =============================================================================
// Feature should identify the single node, so it can be fit in n-dimensional segment tree.
// The minimal qualification for implementing method are implemented in this class.
class feature: public baseFeature{
private:
	
	// Global constant
	static const lld R = 1000 * 1000 * 1000 + 7;
	
	// Attributes inherited from node
	std::vector<pll> *rangeByDimensions; // Range by dimensions
	lld left, right, sizeFactor;
	int dimension;

public:
	
	// Actually represented value
	lld val, sum;
	
	// [Necessary] Constructor; Should be initialized by only range bound and dimension
	feature(){} // Zero construct should not do anything
	feature(lld lbound, lld rbound, int dimension, std::vector<pll> *rangeByDimensions){
		this->dimension = dimension;
		left = lbound, right = rbound;
		val = 0, sum = 0;
		sizeFactor = (rbound - lbound + 1) % R;
		this->rangeByDimensions = rangeByDimensions;
		for(int i=0; i<dimension; i++){
			sizeFactor *= (rangeByDimensions->at(i).second - rangeByDimensions->at(i).first + 1) % R;
			sizeFactor %= R;
		}
	}
	
	// [Necessary] Partial feature generated from this feature.
	feature partial(lld lbound, lld rbound){
		if(left <= lbound && lbound <= rbound && rbound <= right){
			feature newFeature(lbound, rbound, dimension, rangeByDimensions);
			newFeature.val = val;
			newFeature.integrate(NULL, NULL, NULL);
			return newFeature;
		} else throw;
	}
	
	// [Necessary] Current feature := Update(Current, arg)
	// Note that this method necessarily don't need to update cumulative feature.
	// arg is void* because in general purpose, the type of argument for query is not specified.
	void query(const std::string &queryType, void *arg){
		if(queryType == "add"){
			val += *(lld *)arg; val %= R;
		}
		else{
			printf("Given query type is not supported");
			throw;
		}
	}
	
	// [Necessary] Current feature := Integrated(Current, LeftChild, RightChild, LowerDimension);
	// Note that feature pointers can be NULL.
	void integrate(feature *leftchild, feature *rightchild, feature *lower_dimension){
		sum = val * sizeFactor % R;
		if(leftchild != NULL) sum += leftchild->sum, sum %= R;
		if(rightchild != NULL) sum += rightchild->sum, sum %= R;
		if(lower_dimension != NULL) sum += lower_dimension->sum * ((right-left+1) % R), sum %= R;
	}
	
	// Debug-purposed; Must be 1-lined ends without '\n'
	// If you don't want to implement this method then just make empty function.
	void debugPrint(){ 
		printf("Val = %lld, Sum = %lld, Sizefactor = %lld", val, sum, sizeFactor);
	}
};

// n-dimensional segment tree with customized feature. Written by @McDic (github).
template <class featureClass, class rangeClass> class node{
	// featureClass: Describe your own feature.
	// rangeClass: Covering range. Supporting integer-like types. 
	//             If you use real number type for this, it may lead program to undefined behavior.
	
private:
	
	// Base attributes; Used to operate segment tree
	typedef std::pair<rangeClass, rangeClass> prr;
	std::vector<std::pair<rangeClass, rangeClass>> *rangeByDimensions; // Max range by dimensions. {(min0, max0), (min1, max1), ...}
	rangeClass left, right; // Coverage range in n-th dimension
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
		rangeClass mid = (left+right)/2;
		if(isLeft && lc == NULL) lc = new node(dimension, left, mid, this, rangeByDimensions);
		else if(!isLeft && rc == NULL) rc = new node(dimension, mid+1, right, this, rangeByDimensions);
	}
	
	// Upper propagation
	void upperPropagation(){
		
		// Skip if child node is planning to propagate
		if(lc != NULL && lc->willPropagate) return;
		else if(rc != NULL && rc->willPropagate) return;
		else if(lower_dimension != NULL && lower_dimension->willPropagate) return;
		
		// Integrate
		willPropagate = false;
		featureClass *leftFeature = NULL, *rightFeature = NULL, *lowerDimFeature = NULL;
		if(lc != NULL) leftFeature = &(lc->feature);
		if(rc != NULL) rightFeature = &(rc->feature);
		if(lower_dimension != NULL) lowerDimFeature = &(lower_dimension->feature);
		feature.integrate(leftFeature, rightFeature, lowerDimFeature);
		
		// Upper propagate
		if(parent != NULL) parent->upperPropagation();
	}
	
	// Recursive update function; Use lbound and rbound for current dimension and use ranges for lower dimension
	void privateUpdate(const std::string &queryType, void *newFeature, 
	                   rangeClass lbound, rangeClass rbound, const std::vector<prr> &ranges){
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
			rc->privateUpdate(queryType, newFeature, lbound, rbound, ranges);
		}
		else{ // Both partial
			createChild(true), lc->willPropagate = true;
			createChild(false), rc->willPropagate = true;
			lc->privateUpdate(queryType, newFeature, lbound, lc->right, ranges);
			rc->privateUpdate(queryType, newFeature, rc->left, rbound, ranges);
		}
	}
	
	// Search integrated feature
	featureClass privateSearch(rangeClass lbound, rangeClass rbound, const std::vector<prr> &ranges){
		char dir = updateDirection(lbound, rbound);
		if(dir == UD_ERROR) throw;
		else if(dir == UD_DIRECT){ // Direct return
			if(dimension > 0) return lower_dimension->privateSearch(ranges[dimension-1].first, ranges[dimension-1].second, ranges);
			else return feature;
		}
		else if(dir == UD_LEFT) return lc->privateSearch(lbound, rbound, ranges);
		else if(dir == UD_RIGHT) return rc->privateSearch(lbound, rbound, ranges);
		else{ // UD_BOTH
			featureClass searchedFeature = feature.partial(lbound, rbound),
			             leftFeature = lc->privateSearch(lbound, lc->right, ranges),
						 rightFeature = rc->privateSearch(rc->left, rbound, ranges);
			//printf("Searched feature: [%lld, %lld] %lld, %lld\n", lbound, rbound, searchedFeature.val, searchedFeature.sum);
			//printf("Left     feature: [%lld, %lld] %lld, %lld\n", lbound, lc->right, leftFeature.val, leftFeature.sum);
			//printf("Right    feature: [%lld, %lld] %lld, %lld\n", rc->left, rbound, rightFeature.val, rightFeature.sum);
			searchedFeature.integrate(&leftFeature, &rightFeature, NULL);
			//printf("Integrated: [%lld, %lld], %lld, %lld\n", lbound, rbound, searchedFeature.val, searchedFeature.sum);
			//printf("====================================\n");
			return searchedFeature;
		}
	}

public:
	
	// Constructor
	node(int dimension, rangeClass left, rangeClass right, node *parent, std::vector<prr> *rangeByDimensions){
		
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
	char updateDirection(rangeClass lbound, rangeClass rbound){ // Describe update direction
		rangeClass mid = (left + right) / 2;
		if(!(left <= lbound && lbound <= rbound && rbound <= right)) return UD_ERROR; // Error
		else if(left == lbound && rbound == right) return UD_DIRECT; // Direct update
		else if(lbound <= mid && rbound <= mid) return UD_LEFT; // Left-only update
		else if(mid < lbound && mid < rbound) return UD_RIGHT; // Right-only update
		else return UD_BOTH; // lbound <= mid < rbound; Both-partial update
	}
	
	// Public update
	void query(const std::string &queryType, void *newFeature, std::vector<prr> ranges){
		if(dimension >= ranges.size()) throw; // Invalid ranges provided
		willPropagate = true;
		privateUpdate(queryType, newFeature, ranges[dimension].first, ranges[dimension].second, ranges);
	}
	
	// Public search
	featureClass search(std::vector<prr> ranges){
		return privateSearch(ranges[dimension].first, ranges[dimension].second, ranges);
	}
	
	// Debug purpose
	void debugPrint(int tabLevel){ 
		for(int i=0; i<tabLevel-1; i++) printf("|   ");
		if(tabLevel > 0) printf("+-- ");
		printf("Node dimension %d, coverage [%lld, %lld]: (", dimension, (lld)left, (lld)right);
		feature.debugPrint();
		printf(")\n");
		if(lc != NULL) lc->debugPrint(tabLevel+1);
		if(rc != NULL) rc->debugPrint(tabLevel+1);
	}
};

int main(void){
	
	printf("Size of int: %dB\n", sizeof(int));
	printf("Size of feature and node: %dB, %dB\n", sizeof(feature), sizeof(node<feature, lld>));
	
	lld n, m; scanf("%lld %lld", &n, &m);
	printf("Dense usage estimate: at least ~%lld MB\n", (sizeof(node<feature, lld>) * 2LL * n) >> 20);
	std::vector<pll> ranges = {{1, n}};
	node<feature, lld> root(0, 1, n, NULL, &ranges);
	
	for(int i=0; i<n; i++){
		lld to_add; scanf("%lld", &to_add);
		root.query("add", (void *)(&to_add), {{i+1, i+1}});
	}
	for(int q=0; q<m; q++){
		int command; lld left, right;
		scanf("%d %lld %lld", &command, &left, &right);
		if(command == 1){
			lld val; scanf("%lld", &val);
			root.query("add", (void *)&val, {{left, right}});
			//root.debugPrint(0);
		}
		else if(command == 2){
			feature result = root.search({{left, right}});
			printf("%lld\n", result.sum);
		}
	}
	
	return 0;
}
