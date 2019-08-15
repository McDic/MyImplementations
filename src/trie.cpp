/*
	Author: McDic
	Description: Trie and Aho-Corasick algorithm.
*/

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <set>
#include <map>
#include <utility>

// Typedef
typedef long long int lld;
const lld MOD = 1000 * 1000 * 1000 + 7;

// Trie class
static unsigned int TrieID = 0; // Global ID counter
class Trie{
	
	// Base Trie attributes
	public:
	char thischar; // Char of current location. -1 for root.
	int count, endpoint; // Number of words pass here, Number of endpoints at current location
	unsigned int id; // ID of this trie
	Trie *parent; // Parent of this trie. NULL for root.
	std::map<char, Trie*> childs; // Childs of this trie.
	
	// Aho-Corasick attributes
	protected:
	std::vector<Trie*> matching_links; // Success links.
	Trie* failure_link; // Failure link.
	std::vector<Trie*> reverse_failure_links; // Reversed edge of failure links.

	// Trie constructor
	public: Trie(char thischar, Trie *parent){

		// Base attributes
		this->thischar = thischar;
		this->count = 0, this->endpoint = 0;
		this->id = TrieID++;
		this->parent = parent;
		this->childs.clear();

		// Aho-corasick attributes
		this->matching_links.clear();
		this->failure_link = NULL;
		this->reverse_failure_links.clear();
	}
	
	// Does this node has child with given char?
	public: bool hasChild(char nextchar){ return childs.find(nextchar) != childs.end();}

	// Return child with given char
	public: Trie* child(char nextchar){ return hasChild(nextchar) ? childs[nextchar] : NULL;}
	
	// Extend a character. Set addEnd(default false) to true to count endpoint.
	protected: void extend(char nextchar, bool addEnd = false){
		if(!this->hasChild(nextchar)) this->childs[nextchar] = new Trie(nextchar, this);
		this->child(nextchar)->count++;
		if(addEnd) this->child(nextchar)->endpoint++;
	}
	// Extend a word.
	public: void extend(std::string &word, int index = 0){
		if((int)word.length() <= index) return;
		this->extend(word[index], (int)word.length()-1 == index); // Count endpoint only for last index
		this->child(word[index])->extend(word, index+1);
	}
	
	// Find the child AFTER this node using given word. Return NULL if not found.
	// If empty string given, return self.
	public: Trie* find(const std::string &word){
		Trie *now = this;
		for(int i=0; i<(int)word.length() && now != NULL; i++) now = now->child(word[i]);
		return now;
	}

	// Build Aho-Corasick attributes.
	public: static void __build_aho_corasick(Trie *root){
		if(root->parent != NULL) throw "undefined"; // Only root can build it
		
		// Building failure link
		root->failure_link = root; // Root's failure link is root
		std::queue<Trie*> trie_queue;
		for(auto child: root->childs) if(child.second != NULL) trie_queue.push(child.second);
		while(!trie_queue.empty()){
			Trie* now = trie_queue.front(); trie_queue.pop();
			now->failure_link = now->parent->failure_link;
			now->reverse_failure_links.clear(); // Since this is BFS from root, reverse_failure_links is never modified before
			now->matching_links.clear(); // Clear matching points
			if(now->parent == root) now->failure_link = root;
			else while(true){
				if(now->failure_link->hasChild(now->thischar)){
					now->failure_link = now->failure_link->child(now->thischar);
					break;
				}
				else if(now->failure_link == root) break; // Can't go deeper anymore(root reached)
				else now->failure_link = now->failure_link->failure_link; // Go deeper
			} now->failure_link->reverse_failure_links.push_back(now); // Add reversed edge
			for(auto child: now->childs) if(child.second != NULL) trie_queue.push(child.second);
		}

		// Building matching links
		std::vector<Trie*> succeed_matches;
		root->__build_aho_corasick_success_links(succeed_matches);
	}
	// Build matching links using traversal of reverse_failure_links.
	private: void __build_aho_corasick_success_links(std::vector<Trie*> &succeed_matches){
		if(this->endpoint > 0) succeed_matches.push_back(this); // If this node is endpoint, then include match points into succeed_matches..
		this->matching_links = succeed_matches;
		for(auto further: this->reverse_failure_links) further->__build_aho_corasick_success_links(succeed_matches);
		if(this->endpoint > 0) succeed_matches.pop_back(); // Exclude
	}
	// Match patterns with Aho-Corasick algorithm
	public: static std::vector<std::vector<Trie*>> AhoCorasick(Trie* root, const std::string &sentence){
		if(root->parent != NULL) throw "undefined"; // Only root can build it

		std::vector<std::vector<Trie*>> matchResult; // Matching result
		Trie* now = root;
		for(char c: sentence){
			while(true){
				if(now->hasChild(c)){
					now = now->child(c);
					break;
				}
				else if(now == root) break; // Can't go further
				else now = now->failure_link;
			}
			matchResult.push_back(now->matching_links);
		} return matchResult;
	}
	// Match patterns with Aho-Corasick algorithm. Return only if match is found.
	public: static bool AhoCorasickBool(Trie *root, const std::string &sentence){
		if(root->parent != NULL) throw "undefined";

		Trie *now = root;
		for(char c: sentence){
			while(true){
				if(now->hasChild(c)){
					now = now->child(c);
					break;
				}
				else if(now == root) break; // Can't go further
				else now = now->failure_link;
			}
			if(!now->matching_links.empty()) return true;
		} return false;
	}

	// Print for debugging
	public: void print(int tabLevel = 0, const char *spaces = "    "){
		for(int i=0; i<tabLevel; i++) printf("%s", spaces);
		printf("Trie id %d, char %c, count %d, endpoint %d, failure link id %d, reverse failure <", 
			id, (thischar == -1 ? '$' : thischar), count, endpoint, failure_link->id);
		for(auto reversed_failure: reverse_failure_links) printf("%d, ", reversed_failure->id); 
		printf(">, matching links <"); for(auto matching_link: matching_links) printf("%d, ", matching_link->id); 
		printf(">\n");
		for(auto child: childs) child.second->print(tabLevel+1, spaces);
	}

	// Recursive memory deallocation
	public: static void terminate(Trie *root){
		for(auto child: root->childs) if(child.second != NULL) terminate(child.second);
		delete root;
	}
};

// Main
int main(void){

	//freopen("input.txt", "r", stdin);
	//freopen("output.txt", "w", stdout);

	int wordnum; std::cin >> wordnum;
	Trie *root = new Trie(-1, NULL);
	for(int i=0; i<wordnum; i++){
		std::string word; std::cin >> word;
		root->extend(word, 0);
	} Trie::__build_aho_corasick(root);

	int query; std::cin >> query;
	for(int q=0; q<query; q++){
		std::string sentence; std::cin >> sentence;
		bool result = root->AhoCorasickBool(root, sentence);
		if(result) std::cout << "YES\n";
		else std::cout << "NO\n";
	} Trie::terminate(root);
}
