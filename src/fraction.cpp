#include <stdio.h>
#include <string>
#include <utility>

// Fraction class
template <typename numtype> class Fraction{
private:
	
	// Numbers
	bool isPositive;
	numtype upnumber, downnumber;

protected:
	
	// GCD for positive numbers
	static numtype gcd(numtype a, numtype b){
		if(a<0) a = -a;
		if(b<0) b = -b;
		if(a==0) return b;
		else if(b==0) return a;
		else return gcd(b, a%b);
	}
	
	// Clean upnumber and downnumber
	void clean(){
		
		// Sign handling
		if(downnumber < 0){
			isPositive = !isPositive;
			downnumber = -downnumber;
		}
		else if(downnumber == 0){
			printf("[Fatal Error] Down is zero\n");
			throw "Down is zero";
		}
		if(upnumber < 0){
			isPositive = !isPositive;
			upnumber = -upnumber;
		}
		else if(upnumber == 0){ // Special case
			isPositive = true;
			upnumber = 0;
			downnumber = 1;
			return;
		}
		
		// Divide by gcd
		numtype thisgcd = gcd(upnumber, downnumber);
		upnumber /= thisgcd;
		downnumber /= thisgcd;
	}
	
public:
	
	// Constructor
	Fraction(numtype up, numtype down){
		upnumber = up, downnumber = down;
		isPositive = true;
		clean();
	}
	
	// Type casting
	std::pair<numtype, numtype> make_pair(){
		return std::make_pair(upnumber * (isPositive ? 1:-1), downnumber);}
	operator double() const{
		return (isPositive ? 1:-1) * (double)upnumber / (double)downnumber;}
	operator long double() const{
		return (isPositive ? 1:-1) * (long double)upnumber / (long double)downnumber;}
	
	// Arithmetic unary operators
	Fraction operator -(){ // - this
		Fraction newfraction = *this;
		newfraction.isPositive = !newfraction.isPositive;
		return newfraction;
	}
	Fraction operator +(){ // + this
		return *this;
	}
	
	// Arithmetic comparisons for long long int
	bool operator == (long long int num){
		return upnumber * (isPositive ? 1LL : -1LL) == downnumber * num; }
	bool operator < (long long int num){
		return upnumber * (isPositive ? 1LL : -1LL) < downnumber * num; }
	bool operator > (long long int num){
		return upnumber * (isPositive ? 1LL : -1LL) > downnumber * num; }
	bool operator != (long long int num){ return !(this->operator == (num));}
	bool operator <= (long long int num){ return !(this->operator > (num));}
	bool operator >= (long long int num){ return !(this->operator < (num));}
	
	// Arithmetic comparisons between fractions
	bool operator == (const Fraction &num){
		
	}
	
	// Arithmetic base binary operators
	Fraction operator +(const Fraction &another){ // this + another
		
	}
	
};

int main(void){
	
	typedef long long int lld;
	Fraction<lld> f1(27, 81);
	auto f1pair = f1.make_pair();
	printf("%lld / %lld\n", f1pair.first, f1pair.second);
	printf("%.10lf\n", (double)f1);
	return 0;
}
