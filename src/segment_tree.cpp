/*
	Author: McDic
	Description: Implementation of static segment tree with lazy propagation.
*/

#include <stdio.h>
#include <vector>

typedef long long int lld;
lld R = 1000 * 1000 * 1000 + 7;

void printBar(const int limit = 119){
	for(int i=0; i<limit; i++) printf("-");
	printf("\n");
}

// Represents single node.
class node{
public:

	// Node attributes
	int left, right; // Covering
	lld num, sum, mul; // Node's covering number and integrated features under this node
	bool willPropagate; // Lazy upper propagation
	node *leftchild, *rightchild, *parent; // Parent and child nodes
	
	// Constructor
	node(int left, int right, node *parent){
		
		// Setting input attributes
		this->left = left; this->right = right;
		this->num = 0; this->sum = 0;
		this->parent = parent;
		this->willPropagate = false;
		
		if(left == right){ // Leaf node
			this->leftchild = NULL;
			this->rightchild = NULL;
		}
		else{ // Non-left node
			int mid = (this->left + this->right) / 2;
			this->leftchild = new node(this->left, mid, this);
			this->rightchild = new node(mid+1, this->right, this);
		}
	}
	
	// Is this node leaf?
	bool isLeaf(){return this->leftchild == NULL && this->rightchild == NULL;}
	
	// Is given index covered by this node?
	bool isIn(int index){return (this->left <= index && index <= this->right);}
	
	// Determine direction of lower propagation with given range
	int updateDirection(int lbound, int rbound){
		if(this->left == lbound && this->right == rbound) return 0; // Direct update
		else if(this->isLeaf() || lbound > rbound) return -1; // Error; lbound > rbound or Non-direct update at leaf node
		else if(this->leftchild->isIn(lbound) && this->leftchild->isIn(rbound)) return 1; // Left child only
		else if(this->rightchild->isIn(lbound) && this->rightchild->isIn(rbound)) return 2; // Right child only
		else if(this->leftchild->isIn(lbound) && this->rightchild->isIn(rbound)) return 3; // Both childs update
		else return -1; // Error; out of range, etc
	}
	
	// Single update
	void singleUpdate(){
		this->sum = this->num, this->mul = this->num;
		if(!this->isLeaf())	this->sum = (this->num + this->leftchild->sum + this->rightchild->sum) % R;
	}
	
	// Lazy upper propagation
	void upperPropagation(){
		if(!this->willPropagate) return; // Tried propagation on non-planned node
		else if(!this->isLeaf() && (this->leftchild->willPropagate || this->rightchild->willPropagate)) return; // Child will be propagated
		else{ // Do now; This node is leaf or all child nodes are already updated
			this->willPropagate = false;
			this->singleUpdate();
			if(this->parent != NULL) this->parent->upperPropagation();
		}
	}
	
	// Change value for given range
	void changeValue(lld val, int lbound, int rbound){
		int dir = this->updateDirection(lbound, rbound);
		if(dir == -1) return; // Error occured
		this->willPropagate = true;
		if(dir == 0) this->num = val; // Direct update
		else if(dir == 1) this->leftchild->changeValue(val, lbound, rbound); // Left update
		else if(dir == 2) this->rightchild->changeValue(val, lbound, rbound); // Right update
		else if(dir == 3){ // Both update
			this->leftchild->changeValue(val, lbound, this->leftchild->right);
			this->rightchild->changeValue(val, this->rightchild->left, rbound);
		}
		this->upperPropagation(); // Upper propagation
	}
	
	// Find sum, min, max for given lbound and rbound
	void search(int lbound, int rbound, lld *sum){
		int dir = this->updateDirection(lbound, rbound);
		if(dir == -1) return; // Error occured
		else if(dir == 0){ // Direct found
			*sum += this->sum;
			*sum %= R;
		}
		else if(dir == 1) this->leftchild->search(lbound, rbound, sum); // Left search
		else if(dir == 2) this->rightchild->search(lbound, rbound, sum); // Right search
		else if(dir == 3){ // Both search
			this->leftchild->search(lbound, rbound, sum);
			this->rightchild->search(lbound, rbound, sum);
		}
	}
	
	// Print information
	void print(int tabLevel){
		for(int i=0; i<tabLevel-1; i++) printf("|   ");
		if(tabLevel > 0) printf("+-- ");
		printf("Node covering [%d, %d]: num %lld, sum %lld\n", 
			this->left, this->right, this->num, this->sum);
		if(!this->isLeaf()){
			this->leftchild->print(tabLevel+1);
			this->rightchild->print(tabLevel+1);
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
	for(int i=0; i<n; i++) root->changeValue(nums[i], i+1, i+1);
	printf("Tree initialized.\n\n");
	root->print(0);
	
	// Query processing
	while(true){
		printBar();
		printf("Query type '0' -> Terminate program\n");
		printf("Query type '1 L R X' -> Change value by X with range [L, R]\n");
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
			if(L > R || 1 > L || n <= R) printf("Invalid range detected. Please try again.\n");
			else{
				printf("Value changed to %lld in range [%d, %d].\n\n", x, L, R);
				root->changeValue(x, L, R);
				root->print(0);
			}
		}
		else if(command == 2){ // Search with range
			int L, R; scanf("%d %d", &L, &R);
			if(L > R || 1 > L || n <= R) printf("Invalid range detected. Please try again.\n");
			else{
				lld sum; root->search(L, R, &sum);
				printf("Search result in range [%d, %d]: Sum = %lld\n", L, R, sum);	
			}
		}
		else printf("Invalid query detected. Please type again.\n"); // Invalid query
	}
	
	return 0;
}
