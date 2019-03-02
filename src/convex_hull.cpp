/*
	Author: McDic
	Description: Convex hull implementation using Graham scan.
*/

#include <stdio.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <utility>

typedef std::pair<int, int> pii;
typedef long long int lld;

// Calculate angle
double angle(const pii point){
	if(point.first == 0){
		if(point.second > 0) return M_PI / 2.0;
		else if(point.second < 0) return M_PI / -2.0;
		else return -1e100;
	}
	else return atan2(point.second, point.first);
}

// Compare vertices in monotonic angle
bool compare_angle(const pii &p1, const pii &p2){return angle(p1) < angle(p2);}

// Calculate z coordinate of (p0p1) X (p1p2)
lld z_by_cross(pii p0, pii p1, pii p2){
	lld ax = (lld)(p1.first) - p0.first, ay = (lld)(p1.second) - p0.second,
	    bx = (lld)(p2.first) - p1.first, by = (lld)(p2.second) - p1.second;
	return ax*by - ay*bx;
}

int main(void){
	
	// Get input and sort by monotonic angle (anti-clockwise)
	std::vector<pii> vertices;
	int n; scanf("%d", &n);
	for(int i=0; i<n; i++){
		char trash;
		int x, y; scanf("%d %d", &x, &y);
		vertices.push_back({x, y});
	} 
	
	// Sort by X/Y, pick endpoint, and re-sort by angle
	std::sort(vertices.begin(), vertices.end());
	pii base = vertices.front();
	for(int i=0; i<vertices.size(); i++){
		vertices[i].first -= base.first;
		vertices[i].second -= base.second;
	}
	std::sort(vertices.begin(), vertices.end(), compare_angle);
	
	// Create convex stack
	std::vector<pii> convex_stack;
	for(int i=0; i<n; i++){
		pii now = vertices[i];
		while(true){ // Compare now - prev - prevprev
			if(convex_stack.size() < 2){
				convex_stack.push_back(now);
				break;
			}
			pii prev = convex_stack[convex_stack.size()-1], prevprev = convex_stack[convex_stack.size()-2];
			lld z = z_by_cross(prevprev, prev, now);
			if(z > 0){ // Anti-clockwise since the vertices is sorted by anti-clockwise
				convex_stack.push_back(now);
				break;
			}
			else{
				convex_stack.pop_back();
			}
		}
	}
	
	printf("%d vertices are in convex line\n", convex_stack.size());
	for(int i=0; i<convex_stack.size(); i++) 
		printf("(%d, %d)\n", convex_stack[i].first + base.first, convex_stack[i].second + base.second);
	
	return 0;
}
