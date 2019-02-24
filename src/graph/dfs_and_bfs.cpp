/*
	Author: McDic
	Description: Implementation of basic DFS and BFS in undirected graph.
*/

#include <stdio.h>
#include <vector>
#include <queue>

// Attributes
int v, e;
std::vector<std::vector<int>> edges;

// DFS with recursive function
void dfs(int index, std::vector<int> &traversal, 
		 std::vector<bool> &checked){
	if(checked[index]) return;
	checked[index] = true;
	traversal.push_back(index);
	for(auto next: edges[index])
		if(!checked[next]) dfs(next, traversal, checked);
}

// BFS with queue
void bfs(int index, std::vector<int> &traversal, 
		 std::vector<bool> &checked){
	std::queue<int> q;
	q.push(index);
	while(!q.empty()){
		int now = q.front(); q.pop();
		if(checked[now]) continue;
		checked[now] = true;
		traversal.push_back(now);
		for(auto next: edges[now])
			if(!checked[next]) q.push(next);
	}	
}

int main(void){
	
	// Get input
	printf("Input number of vertices/edges and starting point number: ");
	int start; scanf("%d %d %d", &v, &e, &start);
	edges = std::vector<std::vector<int>>(v+1);
	for(int i=0; i<e; i++){
		printf("Input %d-th edge's start point number and end point number: ", i+1);
		int v1, v2; scanf("%d %d", &v1, &v2);
		edges[v1].push_back(v2);
		edges[v2].push_back(v1);
	}
	
	std::vector<int> traversal;
	std::vector<bool> checked;
	
	// DFS
	checked = std::vector<bool>(v+1, false);
	dfs(start, traversal, checked);
	printf("DFS result: %d ", traversal[0]); 
	for(int i=1; i<traversal.size(); i++) printf("-> %d", traversal[i]);
	traversal.clear(); printf("\n");
	
	// BFS
	checked = std::vector<bool>(v+1, false);
	bfs(start, traversal, checked);
	printf("BFS result: %d ", traversal[0]);
	for(int i=1; i<traversal.size(); i++) printf("-> %d", traversal[i]);
	traversal.clear(); printf("\n");
	
	return 0;
}

