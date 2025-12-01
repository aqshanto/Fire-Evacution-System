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
void printPath(const vector<int>& parent, int end_node) {
    vector<int> path;
    // Backtrack from the end_node to the start
    for (int at = end_node; at != -1; at = parent[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());

    if (path.size() <= 1 && parent[end_node] == -1) {
         // Should not happen if a valid path exists, but safety check
         return; 
    }

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
    // Highest node ID is 119, so we need size 120 (0 to 119)
    int num_nodes = 120; 
    vector<vector<Edge>> original_graph(num_nodes);
    
    // --- Populating the graph with your NEW data ---
    original_graph[41].push_back({42, 150}); original_graph[41].push_back({101, 112});
    
    original_graph[42].push_back({41, 150}); original_graph[42].push_back({103, 120});
    
    original_graph[43].push_back({44, 125}); original_graph[43].push_back({105, 100});
    
    original_graph[44].push_back({43, 125}); original_graph[44].push_back({107, 100});
    
    original_graph[45].push_back({46, 250}); original_graph[45].push_back({106, 220});
    
    original_graph[46].push_back({45, 250}); original_graph[46].push_back({112, 50});
    
    original_graph[47].push_back({48, 250}); original_graph[47].push_back({115, 75}); original_graph[47].push_back({118, 70});
    
    original_graph[48].push_back({47, 250}); original_graph[48].push_back({117, 150});
    
    original_graph[49].push_back({50, 150}); original_graph[49].push_back({117, 200});
    
    original_graph[50].push_back({49, 150}); original_graph[50].push_back({119, 70});
    
    original_graph[101].push_back({41, 112}); original_graph[101].push_back({102, 250});
    
    original_graph[102].push_back({101, 250}); original_graph[102].push_back({103, 245}); original_graph[102].push_back({106, 114});
    
    original_graph[103].push_back({42, 120}); 
    original_graph[103].push_back({51, 20}); // One-way to 51 (Stair/Exit)
    original_graph[103].push_back({102, 245}); original_graph[103].push_back({104, 114});
    
    original_graph[104].push_back({103, 114}); original_graph[104].push_back({105, 100}); original_graph[104].push_back({111, 200});
    
    original_graph[105].push_back({43, 100}); original_graph[105].push_back({104, 100}); original_graph[105].push_back({106, 145});
    
    original_graph[106].push_back({45, 220}); original_graph[106].push_back({102, 114}); original_graph[106].push_back({105, 145}); original_graph[106].push_back({107, 235}); original_graph[106].push_back({113, 370});
    
    original_graph[107].push_back({44, 100}); original_graph[107].push_back({106, 235}); original_graph[107].push_back({108, 35});
    
    original_graph[108].push_back({107, 35}); original_graph[108].push_back({109, 175});
    
    original_graph[109].push_back({53, 10}); // One-way to 53 (Stair/Exit)
    original_graph[109].push_back({108, 175}); original_graph[109].push_back({110, 20});
    
    original_graph[110].push_back({109, 20}); original_graph[110].push_back({112, 100}); original_graph[110].push_back({116, 65});
    
    original_graph[111].push_back({52, 20}); // One-way to 52 (Stair/Exit)
    original_graph[111].push_back({104, 200});
    
    original_graph[112].push_back({46, 50}); original_graph[112].push_back({110, 100}); original_graph[112].push_back({113, 100});
    
    original_graph[113].push_back({106, 370}); original_graph[113].push_back({112, 100}); original_graph[113].push_back({114, 65});
    
    original_graph[114].push_back({113, 65}); original_graph[114].push_back({115, 130}); original_graph[114].push_back({117, 200});
    
    original_graph[115].push_back({47, 75}); original_graph[115].push_back({114, 130}); original_graph[115].push_back({116, 70});
    
    original_graph[116].push_back({110, 65}); original_graph[116].push_back({115, 70}); original_graph[116].push_back({118, 40});
    
    original_graph[117].push_back({48, 150}); original_graph[117].push_back({49, 200}); 
    original_graph[117].push_back({54, 30}); // One-way to 54 (Stair/Exit)
    original_graph[117].push_back({114, 200});
    
    original_graph[118].push_back({47, 70}); original_graph[118].push_back({116, 40}); original_graph[118].push_back({119, 327});
    
    original_graph[119].push_back({50, 70}); original_graph[119].push_back({118, 327});

    // --- Define the doors and exits based on your request ---
    vector<int> door_nodes = {41, 42, 43, 44, 45, 46, 47, 48, 49, 50};
    vector<int> exit_nodes = {51, 52, 53, 54};
    
    vector<int> fire_locations;
    int num_fires;
    const int FIRE_PENALTY = 100000;
    
    cout << "--- Door-to-Exit Evacuation Plan (Multiple Fires) ---" << endl;
    cout << "How many fire locations are there? ";
    cin >> num_fires;
    for (int i = 0; i < num_fires; ++i) {
        int fire_node;
        cout << "Enter fire location #" << i + 1 << ": ";
        cin >> fire_node;
        fire_locations.push_back(fire_node);
    }
    
    cout << "\nFIRE DETECTED AT NODES: ";
    for(int loc : fire_locations) cout << loc << " ";
    cout << "\nGenerating safest routes..." << endl;

    // Apply penalty for all fire locations
    vector<vector<Edge>> temp_graph = original_graph;
    for (int i = 0; i < num_nodes; ++i) {
        for (auto& edge : temp_graph[i]) {
            if (isFireNode(i, fire_locations) || isFireNode(edge.first, fire_locations)) {
                edge.second += FIRE_PENALTY;
            }
        }
    }
    
    // --- Loop through each DOOR and find the nearest EXIT ---
    // (We calculate from Door -> Exit because some paths to exits are one-way)
    for (int door_node : door_nodes) {
        // Run Dijkstra starting from this specific door
        DijkstraResult result = dijkstra(temp_graph, door_node);
        
        int min_dist = INF;
        int nearest_exit = -1;
        
        // Check distance to all possible exits
        for (int exit_node : exit_nodes) {
            int current_dist = result.dist[exit_node];
            if (current_dist < min_dist) {
                min_dist = current_dist;
                nearest_exit = exit_node;
            }
        }
        
        cout << "\n------------------------------------" << endl;
        
        if (nearest_exit != -1) {
            cout << "Start: Door " << door_node << " -> Nearest Exit: " << nearest_exit;

            // Check if the path is dangerous
            if (min_dist >= FIRE_PENALTY) {
                cout << "\n\n   *** WARNING: THIS IS A DANGEROUS PATH OF LAST RESORT ***\n"
                     << "   *** The route passes through a fire-affected zone. ***\n";
                cout << "\n   Actual travel distance: " << min_dist - FIRE_PENALTY 
                     << " (Total cost with penalty: " << min_dist << ")\n";
            } else {
                cout << " | Cost: " << min_dist;
            }
            
            cout << " | Path: ";
            // Use the parent array from the Dijkstra run to print path
            printPath(result.parent, nearest_exit);
            cout << endl;

        } else {
            cout << "Door " << door_node << " is TRAPPED and cannot reach any exit." << endl;
        }
    }

    return 0;
}