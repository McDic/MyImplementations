#include <stdio.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <utility>

typedef long long int lld;
const double dbinf = 1e100;
double minangle = 1e100;

template <class T> struct ConvexHull{

	// Typedef and constants
	typedef std::pair<T,T> point;

	// Calculate angle
	static double angle(const point &p){
		if(p.first == 0){
			if(p.second > 0) return M_PI / 2.0;
			else if(p.second < 0) return -M_PI / 2.0;
			else return -dbinf;
		} else return atan2(p.second, p.first);
	}
	
	// Square of abs
	static T absq(const point &p){return p.first * p.first + p.second * p.second;}
	
	// Compare vertices in monotonic angle.
	static bool compare_angle(const point &p1, const point &p2){
		double a1 = angle(p1), a2 = angle(p2);
		if(a1 == a2){
			if(a1 == minangle) return absq(p1) < absq(p2); // If minimum angle, then should check close point first
			else return absq(p1) > absq(p2); // Otherwise should check far point first
		} else return a1 < a2;
	}
	
	// Compute Z of cross dot
	static T z_by_cross(const point &p0, const point &p1, const point &p2){
		T ax = p1.first - p0.first, ay = p1.second - p0.second;
		T bx = p2.first - p1.first, by = p2.second - p1.second;
		return ax*by - ay*bx;
	}
	
	// Calculate Convex Hull based on Graham's scan. Remove parallel vertices if strict.
	static std::vector<point> calculate(std::vector<point> points, bool strict = false, bool debug_msg = false){
		
		// Base sort
		std::sort(points.begin(), points.end());
		point baseoffset = points[0];
		minangle = dbinf;
		for(auto &p: points){
			p.first -= baseoffset.first, p.second -= baseoffset.second;
			double thisangle = angle(p);
			if(minangle > thisangle && thisangle != -dbinf) minangle = thisangle;
		}
		std::sort(points.begin(), points.end(), compare_angle);
		if(debug_msg){
			printf("Minimum angle = %g, baseoffset = (%g, %g)\n", 
			       minangle, (double)baseoffset.first, (double)baseoffset.second);
			printf("Sorted order:\n");
			for(auto p: points) 
				printf("  (%g, %g) angle %g, absq %g\n", 
				       (double)(p.first + baseoffset.first), (double)(p.second + baseoffset.second), 
					   angle(p), (double)absq(p));
		}
		for(auto &p: points) p.first += baseoffset.first, p.second += baseoffset.second;
		
		// Calculate
		std::vector<point> convex;
		for(auto p_now: points){
			if(debug_msg) printf("Looking for (%g, %g):\n", (double)(p_now.first), (double)(p_now.second));
			while(true){
				
				// If convex size is less than 2 then just push
				if(convex.size() < 2){
					if(debug_msg) printf("\tPushing (%g, %g) because not enough convex size\n", 
						(double)(p_now.first), (double)(p_now.second));
					convex.push_back(p_now);
					break;
				}
				
				// Calculate z axis from (p_p - p_pp) and (p_n - p_p) to determine if (p_pp, p_p, p_n) is counter clockwise or not
				point p_prev = convex[convex.size()-1], p_prevprev = convex[convex.size()-2];
				T z = z_by_cross(p_prevprev, p_prev, p_now);
				if((strict && z>0) || (!strict && z>=0)){ // Push if met good condition
					if(debug_msg) printf("\tPushing (%g, %g) because z = %g >%c 0\n", 
						(double)(p_now.first), (double)(p_now.second), (double)z, strict ? 0:'=');
					convex.push_back(p_now);
					break;
				}
				else{
					if(debug_msg) printf("\tPopping (%g, %g) because z = %g <%c 0\n", 
						(double)(p_prev.first), (double)(p_prev.second), (double)z, strict ? '=':0);
					convex.pop_back();
				}
			}
		} 
		
		// If strict, should remove last remaining parallel points
		if(strict){
			while(convex.size() > 3){
				point p_next = convex[0], p_now = convex[convex.size()-1], p_prev = convex[convex.size()-2];
				T z = z_by_cross(p_prev, p_now, p_next);
				if(z>0) break;
				if(debug_msg) printf("Popping (%g, %g) because last point is parallel to first point\n",
			                         (double)(p_now.first), (double)(p_now.second));
				convex.pop_back();
			}
			// Testing
			T z = z_by_cross(convex.back(), convex[0], convex[1]);
			if(z<=0) throw "undefined";
		} return convex;
	}
};

int main(void){
	
	// Get input and sort by monotonic angle (anti-clockwise)
	std::vector<std::pair<lld, lld>> vertices;
	int n; scanf("%d", &n);
	for(int i=0; i<n; i++){
		lld x, y; scanf("%lld %lld", &x, &y);
		vertices.push_back({x, y});
	} 

	std::vector<std::pair<lld, lld>> convexhull = ConvexHull<lld>::calculate(vertices, true, false);
	printf("Total %d vertices in Convex Hull\n", convexhull.size());
	for(auto p: convexhull) printf("(%lld, %lld)\n", p.first, p.second);
	
	return 0;
}
