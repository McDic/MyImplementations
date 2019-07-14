/*
	Author: McDic
	Description: Disjoint set data structure supports union and find.
*/

#include <stdio.h>
#include <vector>

class DisjointSet{
public:
	
	// Attributes
	std::vector<int> parent, rank; // Parent[i] = Parent of i in group, Rank[i] = Rank of i
	
	// Constructor
	DisjointSet(int vertices){
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
	
	// True if v1 and v2 are disjoint, otherwise False
	bool isDisjoint(int v1, int v2){
		return root(v1) != root(v2);
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
	std::vector<std::vector<int>> currentStatus(){
		std::vector<std::vector<int>> groups(parent.size());
		for(int i=1; i<parent.size(); i++) groups[root(i)].push_back(i);
		std::vector<std::vector<int>> result;
		for(std::vector<int> &it: groups) if(!it.empty()) result.push_back(it);
		return result;
	}
};

int main(void){
	
	int n, m; scanf("%d %d", &n, &m); n++;
	DisjointSet sets(n+1);
	for(int i=0; i<m; i++){
		int command, v1, v2; scanf("%d %d %d", &command, &v1, &v2); 
		v1++, v2++;
		if(command == 0) sets.merge(v1, v2);
		else printf(sets.isDisjoint(v1, v2) ? "NO\n":"YES\n");
	}
	
	return 0;
}
