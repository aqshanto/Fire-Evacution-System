// --- LIBRARIES ---
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <map>

// Use the standard namespace to avoid prefixing with std::
using namespace std;

// --- 1. FILL IN YOUR CREDENTIALS ---
char auth[] = "YOUR_BLYNK_AUTH_TOKEN";
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

// --- SYSTEM CONFIGURATION ---
const int NUM_NODES = 42; // We use 1-41, so size is 42
const int FIRE_PENALTY = 100000;
BlynkTimer timer;

// --- DATA TYPES FOR GRAPH AND DIJKSTRA ---
using Edge = pair<int, int>;
using PqElement = pair<int, int>;
const int INF = numeric_limits<int>::max();

struct DijkstraResult {
    vector<int> dist;
    vector<int> parent;
};

// --- GRAPH DEFINITION (STORED IN FLASH MEMORY) ---
// This is an edge list, which is easier to store in PROGMEM.
struct FlashEdge {
    uint8_t u;
    uint8_t v;
    uint16_t weight;
};

const FlashEdge graph_edges[] PROGMEM = {
    {1, 2, 125}, {1, 33, 112}, {2, 3, 125}, {3, 4, 142}, {3, 8, 114},
    {4, 5, 112}, {4, 7, 114}, {5, 6, 114}, {5, 32, 150}, {6, 7, 112},
    {6, 9, 135}, {7, 8, 142}, {7, 34, 100}, {8, 11, 135}, {8, 15, 100},
    {9, 10, 64}, {11, 12, 100}, {12, 13, 35}, {12, 35, 100}, {13, 14, 30},
    {14, 18, 143}, {15, 16, 141}, {15, 36, 220}, {16, 17, 128}, {17, 21, 101},
    {17, 24, 23}, {18, 19, 10}, {18, 20, 12}, {20, 21, 99}, {20, 22, 23},
    {21, 37, 60}, {22, 23, 99}, {22, 25, 40}, {23, 24, 101}, {23, 38, 75},
    {24, 28, 196}, {25, 26, 272}, {26, 27, 55}, {27, 40, 70}, {28, 29, 28},
    {28, 30, 103}, {28, 39, 230}, {30, 31, 45}, {31, 41, 135}, {32, 33, 150},
    {34, 35, 175}, {36, 37, 250}, {38, 39, 250}, {40, 41, 150}
};
const int NUM_EDGES = sizeof(graph_edges) / sizeof(FlashEdge);

// --- GLOBAL VARIABLES ---
vector<int> door_nodes = {32, 33, 34, 35, 36, 37, 38, 39, 40, 41};
vector<int> exit_nodes = {5, 10, 19, 29};
vector<int> current_fire_locations;

// Map your 10 sensor pins to the 10 nodes they monitor
const int sensorPins[] = {34, 35, 32, 33, 25, 26, 27, 14, 12, 13}; // Example GPIO pins
const int monitoredNodes[] = {32, 33, 34, 35, 36, 37, 38, 39, 40, 41}; // Nodes monitored by sensors
const int SENSOR_COUNT = 10;
const int SENSOR_THRESHOLD = 3000; // Threshold for analog sensor reading

// --- CORE LOGIC FUNCTIONS ---
DijkstraResult dijkstra(const vector<vector<Edge>>& graph, int src) {
    // ... (Dijkstra function from previous code, no changes)
    int num_nodes = graph.size(); DijkstraResult result; result.dist.assign(num_nodes, INF); result.parent.assign(num_nodes, -1); priority_queue<PqElement, vector<PqElement>, greater<PqElement>> pq; if (src >= num_nodes || src < 0) return result; result.dist[src] = 0; pq.push({0, src}); while (!pq.empty()) { int u = pq.top().second; int d = pq.top().first; pq.pop(); if (d > result.dist[u]) continue; for (const auto& edge : graph[u]) { int v = edge.first; int weight = edge.second; if (result.dist[u] != INF && result.dist[u] + weight < result.dist[v]) { result.dist[v] = result.dist[u] + weight; result.parent[v] = u; pq.push({result.dist[v], v}); }}} return result;
}

String getPathString(const vector<int>& parent, int start_node) {
    vector<int> path;
    for (int at = start_node; at != -1; at = parent[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());

    String pathStr = "";
    for (size_t i = 0; i < path.size(); ++i) {
        pathStr += String(path[i]);
        if (i < path.size() - 1) {
            pathStr += " -> ";
        }
    }
    return pathStr;
}

bool isFireNode(int node, const vector<int>& fire_locations) {
    for (int fire_node : fire_locations) {
        if (node == fire_node) return true;
    }
    return false;
}

void calculateAndSendPaths(const vector<int>& fire_locations) {
    Blynk.virtualWrite(V0, "New fire state detected! Recalculating all paths...\n");

    // 1. Create a temporary graph in RAM from the PROGMEM data
    vector<vector<Edge>> temp_graph(NUM_NODES);
    for (int i = 0; i < NUM_EDGES; ++i) {
        FlashEdge edge;
        memcpy_P(&edge, &graph_edges[i], sizeof(FlashEdge));
        temp_graph[edge.u].push_back({edge.v, edge.weight});
        temp_graph[edge.v].push_back({edge.u, edge.weight});
    }

    // 2. Apply the fire penalty
    for (int i = 0; i < NUM_NODES; ++i) {
        for (auto& edge : temp_graph[i]) {
            if (isFireNode(i, fire_locations) || isFireNode(edge.first, fire_locations)) {
                edge.second += FIRE_PENALTY;
            }
        }
    }

    // 3. Pre-calculate shortest paths from each exit
    map<int, DijkstraResult> exit_calculations;
    for (int exit_node : exit_nodes) {
        exit_calculations[exit_node] = dijkstra(temp_graph, exit_node);
    }

    // 4. For each door, find the best exit and send data to Blynk
    for (int i = 0; i < door_nodes.size(); ++i) {
        int door_node = door_nodes[i];
        int min_dist = INF;
        int nearest_exit = -1;

        for (int exit_node : exit_nodes) {
            int current_dist = exit_calculations[exit_node].dist[door_node];
            if (current_dist < min_dist) {
                min_dist = current_dist;
                nearest_exit = exit_node;
            }
        }

        String blynk_output = "";
        if (nearest_exit != -1) {
            String path = getPathString(exit_calculations[nearest_exit].parent, door_node);
            if (min_dist >= FIRE_PENALTY) {
                blynk_output = "DANGER: " + path;
            } else {
                blynk_output = "SAFE: " + path;
            }
        } else {
            blynk_output = "TRAPPED!";
        }
        // Send to Virtual Pins V1, V2, ..., V10
        Blynk.virtualWrite(i + 1, blynk_output);
    }
    Blynk.virtualWrite(V0, "Evacuation plan updated.\nMonitoring sensors...\n");
}

void checkSensorsAndRun() {
    vector<int> detected_fires;
    for (int i = 0; i < SENSOR_COUNT; i++) {
        int sensorValue = analogRead(sensorPins[i]);
        if (sensorValue > SENSOR_THRESHOLD) { // If fire is detected
            detected_fires.push_back(monitoredNodes[i]);
        }
    }

    sort(detected_fires.begin(), detected_fires.end());
    
    if (detected_fires != current_fire_locations) {
        current_fire_locations = detected_fires; // Update state
        String fire_msg = "FIRE AT NODES: ";
        for(int loc : current_fire_locations) fire_msg += String(loc) + " ";
        Blynk.virtualWrite(V0, fire_msg + "\n");
        
        calculateAndSendPaths(current_fire_locations);
    }
}

// --- SETUP AND LOOP ---
void setup() {
    Serial.begin(115200);
    Blynk.begin(auth, ssid, pass);

    for (int i = 0; i < SENSOR_COUNT; i++) {
        pinMode(sensorPins[i], INPUT);
    }
    
    Blynk.virtualWrite(V0, "System Initialized. Calculating default paths...\n");
    calculateAndSendPaths({}); // Initial run with no fires
    
    // Set up a timer to check sensors every 2 seconds
    timer.setInterval(2000L, checkSensorsAndRun);
}

void loop() {
    Blynk.run();
    timer.run();
}