/*
    Author: McDic
    Description: Floyd-warshall algorithm.
*/

#include <iostream>
#include <vector>

std::vector<std::vector<bool>> isConnected;
std::vector<std::vector<int>> distances;

// Floyd algorithm
void floyd(int n){

    const int unreachable = 87654321;

	// Initialize
	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			if(i==j) distances[i][j] = 0;
			else if(isConnected[i][j]) distances[i][j] = 1;
            else distances[i][j] = unreachable;
		}
	}
	
	// Floyd
	for(int k=0; k<n; k++)
		for(int i=0; i<n; i++)
			for(int j=0; j<n; j++)
				if(distances[i][j] > distances[i][k] + distances[k][j])
				    distances[i][j] = distances[i][k] + distances[k][j];
}