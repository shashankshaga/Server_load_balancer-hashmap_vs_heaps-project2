
#include <vector>
using namespace std;

// Basically Using this to track requests in the load balancer simulation
struct Request {
    int id;
    int arrivalTime;
    int processingTime;
};
// Helps choose different situations for generating requests based on workload ID   
vector<Request> loadRequests(int workloadId, int limit); // limit the maximum amount of values we want to pull from the database