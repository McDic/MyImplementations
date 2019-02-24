/*
	Author: McDic
	Description: Implementation of dijkstra algorithm for undirected graph.
*/

#include <stdio.h>
#include <vector>
#include <queue>
#include <map>
#include <functional>
#include <utility>

// Constants
typedef long long int lld;

// Attributes
std::vector<std::map<int, lld>> edges;

int main(void){
	
	// Get input
	printf("Input number of vertices and edges and starting point: ");
	int v, e, start; scanf("%d %d %d", &v, &e, &start);
	edges = std::vector<std::map<int, lld>>(v+1);
	for(int i=0; i<e; i++){
		int v1, v2; lld dist;
		printf("Input %d-th edge (v1, v2, dist): ", i+1); 
		scanf("%d %d %lld", &v1, &v2, &dist);
		edges[v1][v2] = dist;
		edges[v2][v1] = dist;
	}
	
	// Construct PQ
	lld inf_dist = -1LL ^ (1LL<<63);
	std::vector<lld> dist(v+1, inf_dist); dist[start] = 0;
	typedef std::pair<lld, int> pli;
	std::priority_queue<pli, std::vector<pli>, std::greater<pli>> pq;
	pq.push({0, start});
	
	// Dijkstra
	while(!pq.empty()){
		pli now_info = pq.top(); pq.pop();
		int now = now_info.second;
		if(dist[now] > now_info.first) continue;
		for(auto next_info: edges[now]){
			int next = next_info.first, singledist = next_info.second;
			if(dist[next] > dist[now] + singledist){
				dist[next] = dist[now] + singledist;
				pq.push({dist[next], next});
			}
		}
	}
	
	// Print
	for(int i=1; i<=v; i++) printf("Distance between #%d ~ #%d: %lld\n", start, i, dist[i]);
	
	return 0;
}
