/*
	Author: McDic
	Convex Hull implementation using CCW and monotone.
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

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
	
	// Absolute value
	coord absSquare(){ return x*x + y*y;} // abs square
	coord distanceSquare(Point2D another){ return (this->operator -(another)).absSquare();}
	
	// CCW: Clockwise (-), Colinear 0, CounterClockwise (+)
	coord CCW(Point2D previous, Point2D next){
		Point2D first = (*this) - previous, second = next - previous;
		// z = a1b2 - a2b1
		//printf("first = (%d, %d), second = (%d, %d)\n", first.x, first.y, second.x, second.y);
		coord z = first.x * second.y - first.y * second.x;
		return z;
	}

	// Return true if this is on first 180' angle based on center/axis. Otherwise false.
    bool onFirst(Point2D center, Point2D axis){
        coord ccw_value = center.CCW(axis, *this);
        if(ccw_value != 0) return ccw_value < 0;
        else return this->operator == (axis); // axis -> true, antiaxis -> false
    }
};

// Counter-clockwise comparing: Return p1<p2 based on center, axis.
template<typename coord> bool CCW_Compare(
Point2D<coord> p1, Point2D<coord> p2, Point2D<coord> center, Point2D<coord> axis){
    // Check angle range of p1 and p2
    bool p1plane = p1.onFirst(center, axis), p2plane = p2.onFirst(center, axis);
    if(p1plane == p2plane) return center.CCW(p1, p2) < 0; // p2 is counter-clockwise later.
    else return p1plane;
}

// Convex Hull - Returned in clockwise order.
template <typename coord> std::vector<Point2D<coord>> ConvexHull(
	std::vector<Point2D<coord>> vertices, bool strict){
	
	// Constant
	const long double epsilon = 1e-6; // Epsilon used to determine sign of CCW

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
				clockwise.push_back(vertices[i]);
				cw_indices.push_back(i);
				visited[i] = true;
				break;
			}
			else{ // Pop latest vertex
				if(cw_indices.back() == (int)vertices.size() - 1) return false; // Can't pop latest vertex
				clockwise.pop_back();
				visited[cw_indices.back()] = false;
				cw_indices.pop_back();
			}
		} return true;
	};
	
	// Go monotone and return
	for(int i=0; i<(int)vertices.size(); i++) if(!visited[i]) put(i);
	for(int i=(int)vertices.size()-1; i>=0; i--) if(!visited[i]) put(i);
	if(put(0)) clockwise.pop_back(), cw_indices.pop_back(); // Make it clean
	return clockwise;
}

int main(void){
	
	typedef long long int lld;
	int n; std::cin >> n;
	std::vector<Point2D<lld>> vertices;
	for(int i=0; i<n; i++){
		lld x, y; std::cin >> x >> y;
		vertices.push_back(Point2D<lld>(x, y));
	}
	std::vector<Point2D<lld>> convexhull = ConvexHull<lld>(vertices, false);
	printf("%d\n", convexhull.size());
	return 0;
}
