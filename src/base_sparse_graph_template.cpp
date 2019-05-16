/*
	Author: McDic
	Description: Base undirected sparse graph template.
*/

#include <stdio.h>
#include <vector>
#include <queue>
#include <utility>
#include <algorithm>

// Typedef
typedef std::pair<int, int> PII;
typedef std::vector<PII> VPII;

// Graph class.
class Graph{
public:
	
	// Attributes
	int vertices;
	std::vector<VPII> edges; // edges[start] = [(end, weight), ...]
	std::vector<bool> visited;
	
	// Constructor
	Graph(int vertices){
		this->vertices = vertices;
		edges.resize(vertices+1);
		visited.resize(vertices+1, false);
	}
	
	// Add edges
	void addEdge(int start, int end, int weight = 1, bool bidirectional = true){
		if(start < 1 || end < 1 || vertices < start || vertices < end){
			printf("Invalid edge(%d - %d) given - index out of range(%d - %d)\n", 
				start, end, 1, vertices); throw;
		}
		edges[start].push_back({end, weight});
		if(bidirectional) edges[end].push_back({start, weight});
	}
	
	// Sort edges in given order.
	void edgeSort(__compare compare){
		for(int i=1; i<=vertices; i++) std::sort(edges[i].begin(), edges[i].end(), compare);
	}
	
	// DFS, BFS. Return history - [(vertex id, degree), ...]
	// EdgeDerived: IF true, visit vertex multiple times if that vertex is connected by multiple edges.
	VPII DFS(int start, bool edgeDerived = false){
		visited = std::vector<bool>(edges.size(), false);
		VPII stack, history; stack.push_back({start, 0});
		while(!stack.empty()){
			PII nowinfo = stack.back(); stack.pop_back();
			int now = nowinfo.first, nowdegree = nowinfo.second;
			if(!visited[now]){ // If not visited yet, check it as visited, push to history, and go further
				visited[now] = true;
				history.push_back(nowinfo);
				for(PII next: edges[now]) stack.push_back({next.first, nowdegree+1});
			}
			else if(edgeDerived) history.push_back(nowinfo);
		} return history;
	}
	VPII BFS(int start, bool edgeDerived = false){
		visited = std::vector<bool>(edges.size(), false);
		VPII history;
		std::queue<PII> que; que.push({start, 0});
		while(!que.empty()){
			PII nowinfo = que.front(); que.pop();
			int now = nowinfo.first, nowdegree = nowinfo.second;
			if(!visited[now]){ // If 
				visited[now] = true;
				history.push_back(nowinfo);
				for(PII next: edges[now]) que.push({next.first, nowdegree+1});
			}
			else if(edgeDerived) history.push_back(nowinfo);
		} return history;
	}
	
	// Get SCC using tarjan's algorithm.
	std::vector<std::vector<int>> SCC(){
		
	}
	
};

int main(void){
	
	// Attributes
	printf("Input number of vertices and edges: ");
	int v, e; scanf("%d %d", &v, &e);
	Graph graph(v);
	
	// Input edges
	for(int i=0; i<e; i++){
		printf("Input %d-th edge: ", i+1);
		int v1, v2; scanf("%d %d", &v1, &v2); // v1 -> v2 directed
		graph.addEdge(v1, v2);
	}
	
	printf("DFS"); for(auto it: graph.DFS(1)) printf(" -> %d", it); printf("\n");
	printf("BFS"); for(auto it: graph.BFS(1)) printf(" -> %d", it); printf("\n");
	
	return 0;
}
