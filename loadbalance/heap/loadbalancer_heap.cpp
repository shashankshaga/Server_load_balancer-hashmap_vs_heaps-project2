#include "loadbalancer.h"
#include "heap.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <string>

using namespace std;

const long long ID_SPACE = 1000000007;

int main(int argc, char* argv[])
{

    vector<int> trace;
    int workloadId = 2;
    int numServers = 200;
    int requestLimit = 100000000;
    int animation = 30;
    int lookups = 0;
    long long statusSum = 0;
    long long totalWait = 0;
    int longestWait = 0;

    for (int i = 1; i + 1 < argc; i += 2) {

        string flag = argv[i];
        int value = atoi(argv[i + 1]);
        if (flag == "--servers") numServers = value;
        if (flag == "--limit") requestLimit = value;
        if (flag == "--animation") animation = value;
        if (flag == "--lookups") lookups = value;
        if (flag == "--workload") workloadId = value;

    }

    vector<Request> requests = loadRequests(workloadId, requestLimit);

    if (requests.empty()) {
        cerr << "No requests loaded." << endl;
        return 1;
    }

    Heap servers;
    for (int id = 0; id < numServers; ++id){
        servers.insert(0 * ID_SPACE + id);
    }

    auto startTimer = chrono::steady_clock::now();

    for (int i = 0; i < (int)requests.size(); ++i) {
        Request req = requests[i];
        // Process the request using the heap-based load balancer

        long long top = servers.extract_min();
        int server = top % ID_SPACE;
        long long freeAt = top / ID_SPACE;

        long long start = max((long long) req.arrivalTime, freeAt);
        long long finish = start + req.processingTime;
        servers.insert(finish * ID_SPACE + server);
        int wait = start - req.arrivalTime;
        totalWait += wait;
        longestWait = max(longestWait, wait);

        if (i < animation) {
            trace.push_back(server);
        }

        for (int k = 0; k < lookups; k++){
            int id = (i * 31 + k * 17) % numServers;
            statusSum += servers.find_by_id(id, ID_SPACE)/ ID_SPACE;
        }
    }

    auto endTimer = chrono::steady_clock::now();
    double elapsed = chrono::duration<double, milli>(endTimer - startTimer).count();
    
    cout << "{\n";
    cout << "  \"structure\": \"heap\",\n";
    cout << "  \"servers\": " << numServers << ",\n";
    cout << "  \"requests\": " << requests.size() << ",\n";
    cout << "  \"workload\": " << workloadId << ",\n";
    cout << "  \"routingMs\": " << elapsed << ",\n";
    cout << "  \"totalWait\": " << totalWait << ",\n";
    cout << "  \"avgWait\": " << (double)totalWait / requests.size() << ",\n";
    cout << "  \"maxWait\": " << longestWait << ",\n";
    cout << "  \"statusSum\": " << statusSum << ",\n";
    cout << "  \"trace\": [";
    for (int i = 0; i < (int)trace.size(); i++) {
        cout << (i ? "," : "") << "{\"server\": " << trace[i] << "}";
    }
    cout << "]\n";
    cout << "}\n";
    return 0;
}