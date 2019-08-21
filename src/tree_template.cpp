/*
    Author: McDic
    Description: Tree BFS/DFS, DP, etc template.
*/

// Standard libraries
#include <iostream>
#include <vector>
#include <utility>

// Tree node.
template <typename weight_t> class Tree{
public:

    // Constants
    typedef std::vector<std::vector<std::pair<int, weight_t>>> wedges_t; // Weighted edges type

    // Base attributes
    int vertices, root;
    std::vector<int> parent, degree; // Parent[i] = i's parent, Degree[i] = i's degree
    wedges_t edges, childs; // Edges is unrooted edges. Childs[i] = {i's child: weight}
    std::vector<weight_t> distances; // Distance from root. In unweighted tree, this is degree.

    // LCA
    std::vector<std::vector<int>> levelparent; // levelparent[i][level] = i's (2^level)-th parent

    // HLD
    std::vector<int> myChain, topChain; // Chain of vertex i, Top of myChain[i]
    std::vector<std::vector<int>> chains;

    // DP
    // -----

    // Constructors
    public: Tree(const std::vector<std::vector<int>> original_edges, int root__ = 0){ // Unweighted edges
        
        // Set properties
        vertices = original_edges.size();
        edges = wedges_t(vertices);
        for(int v=0; v<vertices; v++) for(int next: original_edges[v]) edges[v].push_back({next, (weight_t)1});

        // Reroot
        reroot(root__);
    }
    public: Tree(const wedges_t &original_edges, int root__ = 0){ // Weighted edges

        // Set properties
        vertices = edges.size();
        edges = original_edges;

        // Reroot
        reroot(root__);
    }

    // Rerooting
    public: void reroot(int root__){
        root = root__; // Set new root
        parent = degree = std::vector<int>(vertices, -1); // Reset parents and degree
        childs = std::vector<std::vector<int>>(vertices); // Reset childs
        distances = std::vector<weight_t>(vertices, 0); // Reset distances
        std::vector<bool> visited(vertices, false); // Temp vector to check visited
        parent[root] = root, degree[root] = 0; // Parent of root is root
        __construct_base_info(root, visited); // DFS
    }

    // DFS-based base information construction
    protected: void __construct_base_info(int now, std::vector<bool> &visited){
        
        // Visited = true
        visited[now] = true;

        // DFS
        for(auto nextinfo: edges[now]){
            int next = nextinfo.first;
            weight_t weight = nextinfo.second;
            if(next == parent[now]) continue;
            else if(visited[next]){ // Revisiting error - should not be happened
                std::cout << "Visited vertex " << next << "again - is this really tree?\n";
                throw "undefined";
            }

            // Set attributes and go next step
            parent[next] = now;
            degree[next] = degree[now] + 1;
            childs[now].push_back(std::pair<int, weight_t>(next, weight));
            distances[next] = distances[now] + weight;
            __construct_base_info(next, visited);
        }
    }

    // LCA base info construction
    public: void __construct_lca_info(int maxlevel){
        levelparent = std::vector<std::vector<int>>(vertices, std::vector<int>(maxlevel+1, -1));
        for(int v=0; v<vertices; v++) levelparent[v][0] = parent[v];
        for(int level=1; level<=maxlevel; level++) for(int v=0; v<vertices; v++) 
            levelparent[v][level] = levelparent[levelparent[v][level-1]][level-1];
    }
    // Get level-th parent of now
    public: int get_further_parent(int now, int level){
        for(int b=0; b<levelparent[0].size(); b++) if(level & (1<<b)) now = levelparent[now][level];
        return now;
    }
    // Get LCA(Lowest common ancestor) of v1 and v2
    public: int lca(int v1, int v2){
        if(degree[v1] > degree[v2]) v1 = get_further_parent(v1, degree[v1] - degree[v2]);
        else if(degree[v1] < degree[v2]) v2 = get_further_parent(v2, degree[v2] - degree[v1]);
        while(v1 != v2){
            int b=0;
            while(b < levelparent[0].size()){
                if(levelparent[v1][b] == levelparent[v2][b]) break;
                b++;
            }
            v1 = levelparent[v1][b == 0 ? 0 : b-1];
            v2 = levelparent[v2][b == 0 ? 0 : b-1];
        } return v1;
    }

    // Distance calculation
    /*public: std::vector<weight_t> get_distances(int start){
        std::vector<weight_t> distance(vertices, 0);
        std::vector<bool> visited(vertices, false); visited[start] = true;
        std::vector<std::pair<std::pair<int, int>, weight_t>> stack; // [(now, prev), ...]
        for(auto neighbor: edges[start]) stack.push_back({{neighbor.first, start}, neighbor.second});
        while(!stack.empty()){
            auto top = stack.back(); stack.pop_back();
            int now = top.first.first, prev = top.first.second;
            if(visited[now]) continue;
            visited[now] = true;
            distance[now] = distance[prev] + top.second;
            for(auto nextinfo: edges[now]){
                int next = nextinfo.first;
                weight_t nextweight = nextinfo.second;
                if(visited[next]) continue;
                stack.push_back({{next, now}, nextweight});
            }
        } return distance;
    }

    // Internal integrated DFS tool
    private: void __internal_DFS(int now, int &root, std::vector<bool> &visited, 
        std::vector<int> &previous, std::vector<weight_t> &distance);
        visited[now] = true;
        for(auto nextinfo: edges[now]){
            int next = nextinfo.first;
            weight_t weight = nextinfo.second;
            if(visited[next]) continue;
            distance[next] = distance[now] + weight;
            previous[next] = now;
            __internal_DFS(next, root, visited, previous, distance);
        }
    }*/

    //
    public: std::vector<int> diameter(){

    }

};

int main(void){

    int v, root, q; std::cin >> v >> root >> q;
    std::vector<std::vector<int>> edges(v);
    for(int i=1; i<v; i++){
        int start, end; std::cin >> start >> end;
        edges[start].push_back(end);
        edges[end].push_back(start);
    }
    Tree<int> tree(edges, root);
    tree.__construct_lca_info(10);
    for(int i=0; i<q; i++){
        
    }

    return 0;
}