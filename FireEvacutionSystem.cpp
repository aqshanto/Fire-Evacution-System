#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <map>

// Use the standard namespace to avoid prefixing with std::
using namespace std;

// A pair to store {neighbor_node, edge_weight}
using Edge = pair<int, int>;
// A pair to store {distance, node_id} for the priority queue
using PqElement = pair<int, int>;

// Define a constant for infinity
const int INF = numeric_limits<int>::max();

// Struct to hold the results from a single Dijkstra run
struct DijkstraResult {
    vector<int> dist;
    vector<int> parent;
};

// Dijkstra's algorithm function
DijkstraResult dijkstra(const vector<vector<Edge>>& graph, int src) {
    int num_nodes = graph.size();
    DijkstraResult result;
    result.dist.assign(num_nodes, INF);
    result.parent.assign(num_nodes, -1);

    priority_queue<PqElement, vector<PqElement>, greater<PqElement>> pq;

    if (src >= num_nodes || src < 0) {
        return result;
    }

    result.dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        int u = pq.top().second;
        int d = pq.top().first;
        pq.pop();

        if (d > result.dist[u]) {
            continue;
        }

        for (const auto& edge : graph[u]) {
            int v = edge.first;
            int weight = edge.second;

            if (result.dist[u] != INF && result.dist[u] + weight < result.dist[v]) {
                result.dist[v] = result.dist[u] + weight;
                result.parent[v] = u;
                pq.push({result.dist[v], v});
            }
        }
    }
    return result;
}

// Function to print the path from a start node to an end node
void printPath(const vector<int>& parent, int start_node) {
    vector<int> path;
    for (int at = start_node; at != -1; at = parent[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());

    for (size_t i = 0; i < path.size(); ++i) {
        cout << path[i] << (i == path.size() - 1 ? "" : " -> ");
    }
}

// Helper function to check if a node is in the fire list
bool isFireNode(int node, const vector<int>& fire_locations) {
    for (int fire_node : fire_locations) {
        if (node == fire_node) {
            return true;
        }
    }
    return false;
}


int main() {
    int num_nodes = 42; // Nodes 1-41, so size 42
    vector<vector<Edge>> original_graph(num_nodes);
    
    // --- Populating the graph with your 41 nodes and edges ---
    original_graph[1].push_back({2, 125}); original_graph[1].push_back({33, 112});
    original_graph[2].push_back({1, 125}); original_graph[2].push_back({3, 125});
    original_graph[3].push_back({2, 125}); original_graph[3].push_back({4, 142}); original_graph[3].push_back({8, 114});
    original_graph[4].push_back({3, 142}); original_graph[4].push_back({5, 112}); original_graph[4].push_back({7, 114});
    original_graph[5].push_back({4, 112}); original_graph[5].push_back({6, 114}); original_graph[5].push_back({32, 150});
    original_graph[6].push_back({5, 114}); original_graph[6].push_back({7, 112}); original_graph[6].push_back({9, 135});
    original_graph[7].push_back({6, 112}); original_graph[7].push_back({4, 114}); original_graph[7].push_back({8, 142}); original_graph[7].push_back({34, 100});
    original_graph[8].push_back({3, 114}); original_graph[8].push_back({7, 142}); original_graph[8].push_back({11, 135}); original_graph[8].push_back({15, 100});
    original_graph[9].push_back({6, 135}); original_graph[9].push_back({10, 64});
    original_graph[10].push_back({9, 64});
    original_graph[11].push_back({8, 135}); original_graph[11].push_back({12, 100});
    original_graph[12].push_back({11, 100}); original_graph[12].push_back({13, 35}); original_graph[12].push_back({35, 100});
    original_graph[13].push_back({12, 35}); original_graph[13].push_back({14, 30});
    original_graph[14].push_back({13, 30}); original_graph[14].push_back({18, 143});
    original_graph[15].push_back({8, 100}); original_graph[15].push_back({16, 141}); original_graph[15].push_back({36, 220});
    original_graph[16].push_back({15, 141}); original_graph[16].push_back({17, 128});
    original_graph[17].push_back({16, 128}); original_graph[17].push_back({24, 23}); original_graph[17].push_back({21, 101});
    original_graph[18].push_back({14, 143}); original_graph[18].push_back({19, 10}); original_graph[18].push_back({20, 12});
    original_graph[19].push_back({18, 10});
    original_graph[20].push_back({18, 12}); original_graph[20].push_back({21, 99}); original_graph[20].push_back({22, 23});
    original_graph[21].push_back({17, 101}); original_graph[21].push_back({37, 60}); original_graph[21].push_back({20, 99});
    original_graph[22].push_back({20, 23}); original_graph[22].push_back({23, 99}); original_graph[22].push_back({25, 40});
    original_graph[23].push_back({22, 99}); original_graph[23].push_back({38, 75}); original_graph[23].push_back({24, 101});
    original_graph[24].push_back({17, 23}); original_graph[24].push_back({23, 101}); original_graph[24].push_back({28, 196});
    original_graph[25].push_back({22, 40}); original_graph[25].push_back({26, 272});
    original_graph[26].push_back({25, 272}); original_graph[26].push_back({27, 55});
    original_graph[27].push_back({26, 55}); original_graph[27].push_back({40, 70});
    original_graph[28].push_back({24, 196}); original_graph[28].push_back({29, 28}); original_graph[28].push_back({30, 103}); original_graph[28].push_back({39, 230});
    original_graph[29].push_back({28, 28});
    original_graph[30].push_back({28, 103}); original_graph[30].push_back({31, 45});
    original_graph[31].push_back({30, 45}); original_graph[31].push_back({41, 135});
    original_graph[32].push_back({33, 150}); original_graph[32].push_back({5, 150});
    original_graph[33].push_back({32, 150}); original_graph[33].push_back({1, 112});
    original_graph[34].push_back({7, 100}); original_graph[34].push_back({35, 175});
    original_graph[35].push_back({34, 175}); original_graph[35].push_back({12, 100});
    original_graph[36].push_back({15, 220}); original_graph[36].push_back({37, 250});
    original_graph[37].push_back({36, 250}); original_graph[37].push_back({21, 60});
    original_graph[38].push_back({23, 75}); original_graph[38].push_back({39, 250});
    original_graph[39].push_back({38, 250}); original_graph[39].push_back({28, 230});
    original_graph[40].push_back({27, 70}); original_graph[40].push_back({41, 150});
    original_graph[41].push_back({40, 150}); original_graph[41].push_back({31, 135});


    // --- Define the doors and exits ---
    vector<int> door_nodes = {32, 33, 34, 35, 36, 37, 38, 39, 40, 41};
    vector<int> exit_nodes = {5, 10, 19, 29};
    
    // --- Get multiple fire locations from user ---
    vector<int> fire_locations;
    int num_fires;
    const int FIRE_PENALTY = 100000; // Penalty updated as requested
    
    cout << "--- Door-to-Exit Evacuation Plan (Multiple Fires) ---" << endl;
    cout << "How many fire locations are there? ";
    cin >> num_fires;
    for (int i = 0; i < num_fires; ++i) {
        int fire_node;
        cout << "Enter fire location #" << i + 1 << " (1-41): ";
        cin >> fire_node;
        fire_locations.push_back(fire_node);
    }
    
    cout << "\nFIRE DETECTED AT NODES: ";
    for(int loc : fire_locations) cout << loc << " ";
    cout << "\nGenerating safest routes..." << endl;

    vector<vector<Edge>> temp_graph = original_graph;
    // Apply penalty for all fire locations
    for (int i = 0; i < num_nodes; ++i) {
        for (auto& edge : temp_graph[i]) {
            if (isFireNode(i, fire_locations) || isFireNode(edge.first, fire_locations)) {
                edge.second += FIRE_PENALTY;
            }
        }
    }
    
    // Pre-calculate shortest paths from each exit
    map<int, DijkstraResult> exit_calculations;
    for (int exit_node : exit_nodes) {
        exit_calculations[exit_node] = dijkstra(temp_graph, exit_node);
    }

    // Now, for each door, find the best exit
    for (int door_node : door_nodes) {
        int min_dist = INF;
        int nearest_exit = -1;
        
        for (int exit_node : exit_nodes) {
            int current_dist = exit_calculations[exit_node].dist[door_node];
            if (current_dist < min_dist) {
                min_dist = current_dist;
                nearest_exit = exit_node;
            }
        }
        
        cout << "\n------------------------------------" << endl;
        if (nearest_exit != -1) {
            cout << "Start: Door " << door_node;
            cout << " -> Nearest Exit: " << nearest_exit;
            cout << " | Cost: " << min_dist;
            cout << " | Path: ";
            printPath(exit_calculations[nearest_exit].parent, door_node);
            cout << endl;
        } else {
            cout << "Door " << door_node << " is TRAPPED and cannot reach any exit." << endl;
        }
    }

    return 0;
}