
#include <vector>
using namespace std;

class Node {
    public:
        int key;
        int value;
        Node* next;
        Node(int k, int v) : key(k), value(v), next(nullptr) {}
};

class HashMap {

    private:
        vector<Node*> table;
        int count;
        // This is the actual algorithm that creates the hash function
        int hash(int key);

        void rehash(); // Helps resize the table when more than 75% full

    public: 
        HashMap();
        ~HashMap();
        void put(int key, int value);
        int get(int key);
        void remove(int key);
        int min(); // This 
        int size(); // Options but simple to use
        vector<vector<int>> values(); // This is just to help the box visualization
};