/*
	Author: McDic
	Description: Big integer implementation.
*/

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

// Big integer class
class bigInt{
private:
	
	// ----------------------------------------------------------------------------------
	// Attributes
	
	bool sign; // + or -. Zero is considered as positive.
	std::vector<int> digits; // Each digit stores 10^4.
	const static int onesize = 10000;
	
	// ----------------------------------------------------------------------------------
	// Helper functions
	
	// Remove unnecessary zero
	void cleanBack(){
		while(!digits.empty() && digits.back() == 0) digits.pop_back();
		if(digits.empty()) sign = true;
	}
	
public:
	
	// ----------------------------------------------------------------------------------
	// Constructors
	
	// Constructor by int
	bigInt(int x = 0){*this = bigInt((long long int)x);}
	bigInt(long long int x){
		if(x>=0) sign = true;
		else x *= -1, sign = false;
		digits.clear();
		while(x>0){
			digits.push_back(x % onesize);
			x /= onesize;
		} cleanBack();
	}
	
	// Constructor by string
	bigInt(const char* &line){*this = bigInt(std::string(line));}
	bigInt(std::string line){
		if(line[0] == '+' || ('0' <= line[0] && line[0] <= '9')) sign = true;
		else sign = false;
		digits.clear();
		while(!line.empty() && line != "+" && line != "-"){
			int num = 0, scale = 1;
			for(int i=0; i<4 && !line.empty(); i++){
				if(line == "+" || line == "-") break;
				else if(line.back() < '0' || '9' < line.back()) throw "Non-number found";
				num += (line.back() - '0') * scale;
				scale *= 10;
				line.pop_back();
			}
			digits.push_back(num);
		} cleanBack();
	}
	
	// Constructor by bigInt
	bigInt(const bigInt &another){
		digits = another.digits;
		cleanBack();
	}

	// ----------------------------------------------------------------------------------
	// String representation
	std::string str(){
		if(digits.empty()) return "0";
		std::string result = std::to_string(digits.back());
		for(int i=(int)digits.size()-2; i >= 0; i--){
			for(int j = onesize/10; j>0; j/=10) result += (digits[i] / j % 10) + '0';
		} return (sign ? '+':'-') + result;
	}
	
	// ----------------------------------------------------------------------------------
	// Comparing operators: It can be optimized even more, but for convenience of study I don't implemented it.
	
	bool operator == (const bigInt &another){ // this == another ?
		if(digits.empty() && another.digits.empty()) return true;
		else if(sign != another.sign || digits.size() != another.digits.size()) return false;
		for(int i=0; i<digits.size(); i++) if(digits[i] != another.digits[i]) return false;
		return true;
	}
	bool operator != (const bigInt &another){return !(this->operator ==(another));}
	bool operator > (const bigInt &another){ // this > another ?
		if(sign != another.sign) return sign; // this < 0 <= another (sign = false) || another < 0 <= this (sign = true)
		else if(digits.size() > another.digits.size()) return sign; // abs(this) > abs(another)
		else if(digits.size() < another.digits.size()) return !sign; // abs(this) < abs(another)
		for(int i=(int)digits.size()-1; i>=0; i--){
			if(digits[i] > another.digits[i]) return sign; // abs(this) > abs(another)
			else if(digits[i] < another.digits[i]) return !sign; // abs(another) > abs(this)
		} return false; // Absolute same
	}
	bool operator >= (const bigInt &another){return this->operator ==(another) || this->operator >(another);}
	bool operator < (const bigInt &another){return !(this->operator >=(another));}
	bool operator <= (const bigInt &another){return !(this->operator >(another));}
	
	// ----------------------------------------------------------------------------------
	// Arithmetic operator
	
	bigInt operator -() const { // return -x
		bigInt result(*this); result.sign = !(result.sign);
		return result;
	}
	bigInt operator +() const {return *this;} // return +a (= a)
	
	bigInt operator +(const bigInt &another){ // return this + another
		if(sign != another.sign) return this->operator -(-another);
		bigInt result(0);
		int maxdigitsize = (digits.size() > another.digits.size() ? digits.size() : another.digits.size());
		result.digits = std::vector<int>(0, maxdigitsize + 1);
		for(int i=0; i < maxdigitsize; i++){
			if(i < digits.size()) result.digits[i] += digits[i];
			if(i < another.digits.size()) result.digits[i] += another.digits[i];
			result[i+1] += result[i] / onesize;
			result[i] %= onesize;
		} result.cleanBack();
		return result;
	}
	bigInt operator -(const bigInt &another){ // return this - another
		if(sign != another.sign) return this->operator +(-another);
	}
};

int main(void){
	
	bigInt a("1725543529834737");
	bigInt b("941515007841");
	std::cout << a.str() << " == " << b.str() << " = " << (a==b ? "TRUE":"FALSE") << std::endl;
	std::cout << a.str() << " > " << b.str() << " = " << (a>b ? "TRUE":"FALSE") << std::endl;
	std::cout << a.str() << " < " << b.str() << " = " << (a<b ? "TRUE":"FALSE") << std::endl;
	return 0;
}
