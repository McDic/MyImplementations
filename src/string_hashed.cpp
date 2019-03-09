/*
	Author: McDic
	Description: String comparing using simple hash with multiple modulos.
*/

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <utility>

typedef long long int lld;

// 1e8, 9e7, 8e7, 7e7, 6e7, 5e7-th prime numbers
const std::vector<lld> keys = {2038074743LL, 1824261409LL, 1611623773LL};//, 1400305337LL, 1190494759LL, 982451653LL};

// Fast power with memorization
std::map<std::pair<lld, std::pair<lld, lld>>, lld> savePower;
lld power(int a, int p, lld r){
	std::pair<lld, std::pair<lld, lld>> key = std::make_pair(a, std::make_pair(p, r));
	if(savePower.find(key) != savePower.end()) return savePower[key];
	else if(p==0) savePower[key] = 1;
	else{
		lld half = power(a, p/2, r);
		if(p%2 == 0) savePower[key] = half*half%r;
		else savePower[key] = half*half%r*a%r;
	}
	return savePower[key];
}

// Calculate partial hash
lld partialHash(int lbound, int rbound, lld R, const std::vector<lld> &hashvec){
	lld hashL = (lbound > 0 ? hashvec[lbound-1]:0), hashR = hashvec[rbound];
	lld newHash = (hashR - hashL * power(128, rbound-lbound+1, R) % R) % R;
	if(newHash < 0) newHash += R;
	return newHash;
}

int main(void){
	
	std::string origin, target;
	std::getline(std::cin, origin);
	std::getline(std::cin, target);
	
	// Store hash value
	std::vector<lld> hashes[keys.size()]; // hashes[h][i] = hash(origin[0~i]) mod key[h] 
	lld targetHashes[keys.size()] = {0, }; // targetHashes[h] = hash(target) mod keys[h]
	
	// Hash pre-calculation
	for(int h=0; h<keys.size(); h++){
		
		// hash(origin[0~i]) mod keys[h]
		lld base = 0;
		for(int i=0; i<origin.length(); i++){
			base = (base*128%keys[h] + origin[i]) % keys[h];
			hashes[h].push_back(base);
		}
		
		// hash(target) mod keys[h]
		targetHashes[h] = 0;
		for(int i=0; i<target.length(); i++) targetHashes[h] = (targetHashes[h]*128%keys[h] + target[i]) % keys[h];
	}
	
	// Calculate
	std::vector<int> occurences;	//printf("0 <= origin.length() - target.length() = %d -> %d\n", origin.length() - target.length(), 0 <= (int)(origin.length() - target.length()));
	for(int head=0; head <= (int)origin.length() - (int)target.length(); head++){
		bool allSame = true;
		for(int h=0; h<keys.size(); h++){ // hash(target) == hash(origin[head~head+len(target)-1) mod keys[h] ?
			lld partialHashed = partialHash(head, head+target.length()-1, keys[h], hashes[h]);
			if(partialHashed != targetHashes[h]){
				allSame = false;
				break;
			}
		}
		if(allSame)	occurences.push_back(head);
	}
	
	printf("%d\n", occurences.size());
	for(auto index: occurences) printf("%d ", index+1);
	
}
