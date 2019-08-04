/*
	Author: McDic
	Convex Hull implementation using CCW and monotone.
*/

#include <iostream>
#include <vector>
#include <algorithm>

// Epsilon
const long double epsilon = 1e-6;

// Point class
template <typename coord> class Point2D{
public:
	
	// Attributes
	coord x, y;
	
	// Constructor
	Point2D(coord x_, coord y_){ x = x_, y = y_;}
	
	// Arithmetic Operation
	Point2D operator + (Point2D another){ return Point2D(x + another.x, y + another.y);}
	Point2D operator - (Point2D another){ return Point2D(x - another.x, y - another.y);}
	coord operator * (Point2D another){ return x * another.x + y * another.y;}
	Point2D operator * (coord factor){ return Point2D(x * factor, y * factor);}
	
	// Comparing
	bool operator < (const Point2D &another) const{
		if(x != another.x) return x < another.x;
		else return y < another.y;
	}
	bool operator == (const Point2D &another) const{ return x == another.x && y == another.y;}
	bool operator != (const Point2D &another) const{ return !(this->operator ==(another));}
	
	// CCW: Clockwise (-), Colinear 0, CounterClockwise (+)
	coord CCW(Point2D previous, Point2D next){
		Point2D first = (*this) - previous, second = next - previous;
		// z = a1b2 - a2b1
		//printf("first = (%d, %d), second = (%d, %d)\n", first.x, first.y, second.x, second.y);
		coord z = first.x * second.y - first.y * second.x;
		return z;
	}
};

// Convex Hull - Returned in clockwise order.
template <typename coord> std::vector<Point2D<coord>> ConvexHull(
	std::vector<Point2D<coord>> vertices, bool strict, bool debug = false){
	
	// Sort
	std::sort(vertices.begin(), vertices.end());
	
	// Attributes
	std::vector<bool> visited(vertices.size(), false); // To avoid revisiting
	std::vector<int> cw_indices; // Indices of convex hull
	std::vector<Point2D<coord>> clockwise; // Convex hull points ordered in clockwise order
	
	// Putting function
	auto put = [&](int i){
		while(true){
			int us = clockwise.size();
			bool go = false;
			if(us-2 < 0) go = true; // If there are 0 or 1 vertex then just put
			else{
				coord CCW = clockwise[us-1].CCW(clockwise[us-2], vertices[i]);
				if(CCW < 0) go = true;
				else if(!strict && -epsilon < CCW && CCW < epsilon) go = true;
			}
			if(go){ // Include current vertex
				if(debug) std::cout << "Pushed " << i << "-th vertex (" << vertices[i].x << ", " << vertices[i].y << ")\n";
				clockwise.push_back(vertices[i]);
				cw_indices.push_back(i);
				visited[i] = true;
				break;
			}
			else{ // Pop latest vertex
				if(cw_indices.back() == vertices.size() - 1){
					if(debug) std::cout << "You can't pop last vertex!! Popping cancelled.\n";
					return false;
				}
				if(debug) std::cout << "Popped " << cw_indices.back() << "-th vertex (" << clockwise.back().x << ", " << clockwise.back().y << ") by " << i << "-th vertex (" << vertices[i].x << " ," << vertices[i].y << ")\n";
				clockwise.pop_back();
				visited[cw_indices.back()] = false;
				cw_indices.pop_back();
			}
		} return true;
	};
	
	// Go monotone
	for(int i=0; i<vertices.size(); i++) if(!visited[i]) put(i);
	for(int i=(int)vertices.size()-1; i>=0; i--) if(!visited[i]) put(i);
	bool putted = put(0); 
	if(putted) clockwise.pop_back(), cw_indices.pop_back(); // Make it clean
	
	// Result
	if(debug){
		std::cout << "Convex hull result:\n";
		for(auto point: clockwise) std::cout << "  (" << point.x << ", " << point.y << ")\n";
	}
	return clockwise;
}

int main(void){
	
	//Point2D<int> a(2, 1), b(2, 0), c(1, 1);
	//printf("CCW %d\n", b.CCW(a, c));
	//return 0;
	
	typedef long long int lld;
	int n; std::cin >> n;
	std::vector<Point2D<lld>> vertices;
	for(int i=0; i<n; i++){
		lld x, y; std::cin >> x >> y;
		vertices.push_back(Point2D<lld>(x, y));
	}
	std::vector<Point2D<lld>> convexhull = ConvexHull<lld>(vertices, false, true);
	printf("%d\n", convexhull.size());
	return 0;
}
