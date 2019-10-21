/*
	Author: McDic
	Description: Implementation of static segment tree with destructed feature spreading.
*/

#include <stdio.h>
#include <vector>

// Constants
typedef long long int lld;
lld R = 1000 * 1000 * 1000 + 7;

// Pretty bar
void printBar(const int limit = 119){
	for(int i=0; i<limit; i++) printf("-");
	printf("\n");
}

// Represents single node.
class node{
public:

	// Node attributes
	int rangeLeft, rangeRight; // Covering
	bool willPropagate; // Lazy upper propagation
	node *childLeft, *childRight, *parent; // Parent and child nodes
	
	// Features
	lld num, sum; // Node's covering number and integrated features under this node

	// Constructor
	node(int rangeLeft, int rangeRight, node *parent){
		
		// Setting input attributes
		this->rangeLeft = rangeLeft; this->rangeRight = rangeRight;
		this->parent = parent;
		this->willPropagate = false;
		
		// Feature initialization
		this->num = 0; this->sum = 0;

		if(rangeLeft == rangeRight){ // Leaf node
			this->childLeft = NULL;
			this->childRight = NULL;
		}
		else{ // Non-leaf node
			int mid = (this->rangeLeft + this->rangeRight) / 2;
			this->childLeft = new node(this->rangeLeft, mid, this);
			this->childRight = new node(mid+1, this->rangeRight, this);
		}
	}
	
	// Is this node leaf?
	bool isLeaf(){return this->childLeft == NULL && this->childRight == NULL;}
	
	// Is given index covered by this node?
	bool isIn(int index){return (this->rangeLeft <= index && index <= this->rangeRight);}
	
	// Determine direction of lower propagation with given range
	int updateDirection(int lbound, int rbound){
		if(this->rangeLeft == lbound && this->rangeRight == rbound) return 0; // Direct update
		else if(this->isLeaf() || lbound > rbound) return -1; // Error; lbound > rbound or Non-direct update at leaf node
		else if(this->childLeft->isIn(lbound) && this->childLeft->isIn(rbound)) return 1; // Left child only
		else if(this->childRight->isIn(lbound) && this->childRight->isIn(rbound)) return 2; // Right child only
		else if(this->childLeft->isIn(lbound) && this->childRight->isIn(rbound)) return 3; // Both childs update
		else return -1; // Error; out of range, etc
	}
	
	// Single update
	void singleUpdate(){
		this->sum = this->num * (this->rangeRight - this->rangeLeft + 1);
		if(!this->isLeaf())	this->sum += this->childLeft->sum + this->childRight->sum;
	}
	
	// Lazy upper propagation
	void upperPropagation(){
		if(!this->willPropagate) return; // Tried propagation on non-planned node
		else if(!this->isLeaf() && (this->childLeft->willPropagate || this->childRight->willPropagate)) return; // Child will be propagated
		else{ // Do now; This node is leaf or all child nodes are already updated
			//printf("Propagating node [%lld, %lld]\n", rangeLeft, rangeRight);
			this->willPropagate = false;
			this->singleUpdate();
			if(this->parent != NULL) this->parent->upperPropagation();
		}
	}
	
	// Change value for given range
	void update(lld val, int lbound, int rbound){
		int dir = this->updateDirection(lbound, rbound);
		if(dir == -1) return; // Error occured
		this->willPropagate = true;
		if(dir == 0) {
			this->num += val; // Direct update
			this->upperPropagation(); // Upper propagation
		}
		else if(dir == 1){
			this->childLeft->willPropagate = true;
			this->childLeft->update(val, lbound, rbound); // Left update
		}
		else if(dir == 2){
			this->childRight->willPropagate = true;
			this->childRight->update(val, lbound, rbound); // Right update
		}
		else if(dir == 3){ // Both update
			this->childLeft->willPropagate = true;
			this->childRight->willPropagate = true;
			this->childLeft->update(val, lbound, this->childLeft->rangeRight);
			this->childRight->update(val, this->childRight->rangeLeft, rbound);
		}
	}
	
	// Find sum for given lbound and rbound
	lld search(int lbound, int rbound){
		int dir = this->updateDirection(lbound, rbound);
		lld thissum = this->sum, thisnum = this->num * (rbound - lbound + 1);
		if(dir == -1) throw "Invalid range"; // Error occured
		else if(dir == 0){ // Direct found
			//printf("Found fit (%lld, %lld): sum %lld\n", lbound, rbound, this->sum);
			return thissum;
		}
		else if(dir == 1) return this->childLeft->search(lbound, rbound) + thisnum; // Left search
		else if(dir == 2) return this->childRight->search(lbound, rbound) + thisnum; // Right search
		else if(dir == 3){ // Both search
			return (this->childLeft->search(lbound, this->childLeft->rangeRight)
					+ this->childRight->search(this->childRight->rangeLeft, rbound) + thisnum) ;
		}
		else throw "undefined";
	}
	
	// Print information
	void print(int tabLevel){
		for(int i=0; i<tabLevel-1; i++) printf("|   ");
		if(tabLevel > 0) printf("+-- ");
		printf("Node covering [%d, %d]: num %lld, sum %lld\n", 
			this->rangeLeft, this->rangeRight, this->num, this->sum);
		if(!this->isLeaf()){
			this->childLeft->print(tabLevel+1);
			this->childRight->print(tabLevel+1);
		}
	}
};

int main(void){
	
	// Get input
	printf("Input number of values: ");
	int n; scanf("%d", &n);
	std::vector<lld> nums;
	for(int i=0; i<n; i++){
		printf("Input %d-th number: ", i+1);
		lld temp; scanf("%lld", &temp);
		nums.push_back(temp);
	}
	
	// Initialize segment tree
	node *root = new node(1, n, NULL);
	for(int i=0; i<n; i++) root->update(nums[i], i+1, i+1);
	printf("Tree initialized.\n\n");
	root->print(0);
	
	// Query processing
	while(true){
		printBar();
		printf("Query type '0' -> Terminate program\n");
		printf("Query type '1 L R X' -> Add value by X with range [L, R]\n");
		printf("Query type '2 L R' -> Find values with range [L, R]\n");
		printf("Please input your query: ");
		int command; scanf("%d", &command);
		if(command == 0){ // Print and exit
			printf("\n");
			root->print(0);
			break;
		}
		else if(command == 1){ // Change value
			int L, R; lld x; scanf("%d %d %lld", &L, &R, &x);
			if(L > R || 1 > L || n < R) printf("Invalid range detected. Please try again.\n");
			else{
				printf("Value added to %lld in range [%d, %d].\n\n", x, L, R);
				root->update(x, L, R);
				root->print(0);
			}
		}
		else if(command == 2){ // Search with range
			int L, R; scanf("%d %d", &L, &R);
			if(L > R || 1 > L || n < R) printf("Invalid range detected. Please try again.\n");
			else{
				lld sum = root->search(L, R);
				printf("Search result in range [%d, %d]: Sum = %lld\n", L, R, sum);	
			}
		}
		else printf("Invalid query detected. Please type again.\n"); // Invalid query
	}
	
	return 0;
}
