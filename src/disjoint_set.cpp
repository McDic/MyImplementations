#include <stdio.h>
#include <vector>

class disjointset{
public:
	
	// Attributes
	std::vector<int> parent, rank; // Parent[i] = Parent of i in group, Rank[i] = Rank of i
	
	// Constructor
	disjointset(int vertices){
		parent = std::vector<int>(vertices+1, 0);
		rank = std::vector<int>(vertices+1, 1);
		for(int i=0; i<parent.size(); i++) parent[i] = i;
	}
	
	// Find root of this vertex
	int root(int v){
		if(v == parent[v]) return v;
		parent[v] = root(parent[v]);
		return parent[v];
	}
	
	// Union two sets
	void merge(int v1, int v2){
		int root1 = root(v1); 
		int root2 = root(v2);
		if(root1 == root2) return;
		else if(rank[root1] > rank[root2]){ // Merge root2 into root1
			parent[root2] = root1;
			rank[root1]++;
		}
		else{
			parent[root1] = root2;
			rank[root2]++;
		}
	}
	
	// Return current groups
	std::vector<std::vector<int>> currentstatus(){
		std::vector<std::vector<int>> groups(parent.size());
		for(int i=1; i<parent.size(); i++) groups[root(i)].push_back(i);
		std::vector<std::vector<int>> result;
		for(std::vector<int> &it: groups) if(!it.empty()) result.push_back(it);
		return result;
	}
	
};

int main(void){
	
	disjointset DS = disjointset(10);
	DS.merge(1, 2);
	DS.merge(2, 3);
	DS.merge(1, 6);
	DS.merge(7, 8);
	DS.merge(1, 8);
	DS.merge(4, 5);
	DS.merge(9, 10);
	DS.merge(4, 10);
	int counter = 0;
	for(auto it: DS.currentstatus()){
		printf("Group #%d: ", ++counter);
		for(auto num: it) printf("%d, ", num);
		printf("\n");
	}
	
	return 0;
}
