#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>

// Typedef
typedef long long int lld;
const lld MOD = 1000 * 1000 * 1000 + 7;

// Trie class
static int TrieID = 0; // Global ID counter
class Trie{
public:
	
	// Attributes
	char thischar; // Char of current location
	int count, endpoint; // Number of words pass here, Number of endpoints at current location
	int id; // ID of this trie
	Trie *parent;
	Trie *childs[26]; // abcdef...z
	
	// Trie constructor
	Trie(char thischar, Trie *parent){
		this->thischar = thischar;
		this->count = 0, this->endpoint = 0;
		this->id = TrieID++;
		this->parent = parent;
		for(int i=0; i<26; i++) this->childs[i] = NULL;
	}
	
	// Child related
	bool hasChild(char nextchar){
		return (this->childs[nextchar - 'a'] != NULL);
	}
	Trie* child(char nextchar){
		if(!hasChild(nextchar)) return NULL;
		else return this->childs[nextchar - 'a'];
	}
	
	// Extend
	void extend(char nextchar, bool addEnd = false){
		if(!this->hasChild(nextchar)) this->childs[nextchar - 'a'] = new Trie(nextchar, this);
		this->child(nextchar)->count++;
		if(addEnd) this->child(nextchar)->endpoint++;
	}
	void extend(std::string &word, int index, bool addEnd = true){
		if(word.length() <= index) return;
		this->extend(word[index], addEnd && word.length() == index+1);
		this->childs[word[index] - 'a']->extend(word, index+1, addEnd);
	}
	
	// Debug print
	void print(int tabLevel = 0, const char *spaces = "  "){
		for(int i=0; i<tabLevel; i++) printf("%s", spaces);
		printf("Trie id %d, char %c, count %d, endpoint %d\n", id, (thischar == -1 ? '$' : thischar), count, endpoint);
		for(char c='a'; c<='z'; c++) if(this->hasChild(c)) this->child(c)->print(tabLevel+1, spaces);
	}
};

// Main
int main(void){
	
	// Get input
	int wordnum; std::cin >> wordnum;
	std::string line;
	std::map<std::string, int> words;
	Trie *root = new Trie(-1, NULL);
	for(int i=0; i<wordnum; i++){
		std::string word; std::cin >> word;
		root->extend(word, 0, true);
	} std::cin >> line;
	
	root->print(); printf("===================\n");
	
	// Look
	std::vector<lld> base(line.length() + 1, 0);
	base[0] = 1;
	for(int start = 0; start < line.length(); start++){
		Trie *thistrie = root;
		for(int l = 1; l < 305 && start + l <= line.length(); l++){
			if(!thistrie->hasChild(line[start+l-1])) break;
			else thistrie = thistrie->child(line[start+l-1]);
			base[start + l] += base[start] * (thistrie->count) % MOD;
			base[start + l] %= MOD;
		}
		
		//printf("Start at %d\n", start);
		//for(int i=0; i<=line.length(); i++) printf("Base[%d] = %lld\n", i, base[i]);
		//printf("===================\n");
	}
	printf("%lld\n", base[line.length()]);
	return 0;
}
