#include "loadbalancer.h"
#include "hashmap.h"

#include <algorithm>
#include <chrono> // time measurement
#include <iostream>

using namespace std;

int lookups = 0;
long long statusSum = 0;

// This grabs the details of each request made
struct Step {
    int requestId;
    int arrival;
    int processing;
    int server;
    int start;
    int finish;
    int wait;
};

int main(int argc, char* argv[]) {

    vector<Step> trace; // This stores the requests, so the frontend can replace the animation
    int workloadId = 2;
    int numServers = 200;
    int requestLimit = 10000000;
    int animation = 30;
    // without this its causing an overflow
    long long totalWait = 0;
    int longestWait = 0;

    // grabs the context from the server.js
    for (int i = 1; i + 1 < argc; i += 2) {
        string flag = argv[i];
        int value = atoi(argv[i + 1]);
        if (flag == "--servers") numServers = value;
        if (flag == "--workload") workloadId = value;
        if (flag == "--limit") requestLimit = value;
        if (flag == "--animation") animation = value;
        if (flag == "--lookups") lookups = value;
    }
    // loads the requests from the database
    vector<Request> requests = loadRequests(workloadId, requestLimit);

    if (requests.empty()) {
        cerr << "Run the generator first BRUH\n" << endl;
        return 1;
    }

    HashMap servers;
    for (int i = 0; i < numServers; ++i) {
        servers.put(i, 0); 
    }

    vector<vector<int>> layout = servers.values();
    vector<int> handled(numServers, 0);
    // We officially start the simulation timer here
    auto startTimer = chrono::steady_clock::now();

    for (int i = 0; i < requests.size(); ++i) {

        Request req = requests[i];
        int server = servers.min();
        int freeAt = servers.get(server); // checks which one is open

        int start = max(req.arrivalTime, freeAt);
        int finish = start + req.processingTime;
        servers.put(server, finish); // updates the server's next free time

        int wait = start - req.arrivalTime;
        totalWait += wait;
        longestWait = max(longestWait, wait);
        handled[server]++;

        // This just helps the animation happen for the first few requests
        if ( i < animation) {
            trace.push_back({req.id, req.arrivalTime, req.processingTime, server, start, finish, wait});
        }

        for (int k = 0; k < lookups; k++
        ) {
            int id = (i * 31 + k * 17) % numServers;
            statusSum += servers.get(id);
        }
    }

    auto endTimer = chrono::steady_clock::now();
    auto elapsed = chrono::duration<double, milli>(endTimer - startTimer).count();

    // Server.js converts this to json and sends this to the frontend
    cout << "{";
    cout << "\"servers\": " << numServers << ", ";
    cout << "\"routingMs\": " << elapsed << ", ";
    cout << "\"avgWait\": " << (double)totalWait / requests.size() << ", ";
    cout << "\"maxWait\": " << longestWait << ", ";
    cout << "\"lookups\": " << lookups << ", ";
    cout << "\"statusSum\": " << statusSum << ", ";
    cout << "\"trace\": [";
    //replay the trace for the frontend animation
    for (int i = 0; i < (int)trace.size(); i++) {
        cout << (i ? "," : "") << "{\"server\": " << trace[i].server << "}";
        
    }
    cout << "]}\n";
    return 0;
}