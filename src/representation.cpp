/*
    Author: McDic
    Description: Various representation of variables for debugging.
*/

#include <iostream>
#include <string>
#include <vector>

// Binary representation of given integer.
std::string binaryRepresentation(int x, int maxBase = 32){
    std::string answer;
    for(int i=maxBase-1; i>=0; i--) answer += (((1<<i) & x) ? '1':'0');
    return answer;
}
std::string binaryRepresentation(long long int x, int maxBase = 64){
    std::string answer;
    for(int i=maxBase-1; i>=0; i--) answer += (((1LL<<i) & x) ? '1':'0');
    return answer;
}

int main(void){

    std::cout << binaryRepresentation(17, 10);
    while(1);
    return 0;

}