/*
	Author: McDic
	Description: Base undirected sparse graph template.
*/

#include <stdio.h>
#include <vector>

int main(void){
	
	// Attributes
	printf("Input number of vertices and number of edges: ");
	int v, e; scanf("%d %d", &v, &e);
	std::vector<std::vector<int>> edges(v+1);
	
	// Input edges
	for(int i=0; i<e; i++){
		printf("Input %d-th edge's start point number and end point number: ", i+1);
		int v1, v2; scanf("%d %d", &v1, &v2); // v1 -> v2 directed
		edges[v1].push_back(v2);
		edges[v2].push_back(v1);
	}
	return 0;
}
