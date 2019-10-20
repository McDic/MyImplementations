/*
	Author: McDic
	Description: Implementation of static segment tree without lazy propagation.
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
	node *childLeft, *childRight, *parent; // Parent and child nodes
	
	// Features
	lld num; // Node's covering number and integrated features under this node

	// Constructor
	node(int rangeLeft, int rangeRight, node *parent){
		
		// Setting input attributes
		this->rangeLeft = rangeLeft; this->rangeRight = rangeRight;
		this->parent = parent;
		
		// Feature initialization
		this->num = 0;

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
		if(!this->isLeaf())	this->num = this->childLeft->num + this->childRight->num;
	}
	
	// Non-lazy upper propagation
    void upperPropagation(){
        this->singleUpdate();
        if(this->parent != NULL) this->parent->upperPropagation();
	}
	
	// Change value for given range
	void update(lld val, int targetIndex){
		int dir = this->updateDirection(targetIndex, targetIndex);
        if(dir == 0) {
			this->num = val; // Direct update
			this->upperPropagation(); // Upper propagation
		}
		else if(dir == 1){
			this->childLeft->update(val, targetIndex); // Left update
		}
		else if(dir == 2){
			this->childRight->update(val, targetIndex); // Right update
        }
        else throw "undefined behaviour";
	}
	
	// Find sum for given lbound and rbound
	lld search(int lbound, int rbound){
		int dir = this->updateDirection(lbound, rbound);
        if(dir == 0){ // Direct found
			//printf("Found fit (%lld, %lld): sum %lld\n", lbound, rbound, this->sum);
			return this->num;
		}
		else if(dir == 1) return this->childLeft->search(lbound, rbound); // Left search
		else if(dir == 2) return this->childRight->search(lbound, rbound); // Right search
		else if(dir == 3){ // Both search
			return    this->childLeft->search(lbound, this->childLeft->rangeRight)
					+ this->childRight->search(this->childRight->rangeLeft, rbound);
		}
		else throw "undefined";
	}
	
	// Print information
	void print(int tabLevel){
		for(int i=0; i<tabLevel-1; i++) printf("|   ");
		if(tabLevel > 0) printf("+-- ");
		printf("Node covering [%d, %d]: num %lld\n", 
			this->rangeLeft, this->rangeRight, this->num);
		if(!this->isLeaf()){
			this->childLeft->print(tabLevel+1);
			this->childRight->print(tabLevel+1);
		}
	}
};

int main(void){
#ifdef __McDic__ // Local testing I/O
    freopen("VScode/IO/input.txt", "r", stdin);
    freopen("VScode/IO/output.txt", "w", stdout);
#endif

	// Get input
	int n, m, k; scanf("%d %d %d", &n, &m, &k);
	std::vector<lld> nums;
	for(int i=0; i<n; i++){
		lld temp; scanf("%lld", &temp);
		nums.push_back(temp);
	}
	
	// Initialize segment tree
	node *root = new node(1, n, NULL);
	for(int i=0; i<n; i++) root->update(nums[i], i+1);
	//printf("Tree initialized.\n\n"); root->print(0);
	
	// Query processing
	for(int q=0; q<m+k; q++){
		int command; scanf("%d", &command);
		if(command == 0){ // Print and exit
			printf("\n");
			root->print(0);
			break;
		}

        int b, c; scanf("%d %d", &b, &c);
        if(command == 1){
            root->update(c, b);
        }
        else{
            printf("%lld\n", root->search(b, c));
        }
	}
	
	return 0;
}
