#include "hashmap.h"
#include <iostream>
using namespace std;


HashMap::HashMap() {
    count = 0;
    table = vector<Node*>(10, nullptr);
}

HashMap::~HashMap() {
    for (auto node : table) {

        while (node) {
            Node* temp = node;
            node = node->next;
            delete temp;
        }
    }
}

//  Instead of doing a typical hash function, I made it reverse the digits of the key in our case the last servers go first
int HashMap::hash(int key) {

    int reverse = 0;
    while (key > 0) {
        reverse = reverse * 10 + key % 10;
        key /= 10;
    }
    return reverse % table.size();

}


void HashMap::put(int key, int value) {

    int index = hash(key);
    Node* current = table[index];

    while (current != nullptr) {
        if (current->key == key) {
            current->value = value;
            return;
        }
        current = current->next;
    }

    Node* newNode = new Node(key, value);
    newNode->next = table[index];
    table[index] = newNode;
    count++;

    // This ensures that the table is resized when more than 75% full
    if (count * 4 > table.size() * 3) {
        rehash();
    }
}

// This checks if the key exists and returns its value, otherwise returns -1
int HashMap::get(int key) {

    int index = hash(key);
    Node* current = table[index];
    while (current != nullptr) {
        if (current->key == key) {
            return current->value;
        }
        current = current->next;
    }
    return -1; 
}

void HashMap::remove(int key) {

    int index = hash(key);
    Node* current = table[index];
    Node* prev = nullptr;
    while (current != nullptr) {
        if (current->key == key) {
            if (prev) {
                prev->next = current->next;
            } else {
                table[index] = current->next;
            }
            delete current;
            count--;
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Just a helper
int HashMap::size() {
    return count;
}

// This finds the lowest key in the hashmap in our case the server with the lowest requests
// Only O(n) operation
int HashMap::min() {

    int minKey = -1;
    int minValue = 0;

    for (auto node : table) {
        while (node) {
            if (minKey == -1 || node->value < minValue || node->value == minValue && node->key < minKey) {
                minKey = node->key;
                minValue = node->value;
            }
            node = node->next;
        }
    }
    return minKey;
}

// resizes the table
void HashMap::rehash() {

    vector<Node*> oldTable = table;
    table = vector<Node*>(oldTable.size() * 2, nullptr);
    count = 0;

    for (auto node : oldTable) {

        while (node) {

            put(node->key, node->value); // reuse instead
            Node* temp = node;
            node = node->next;
            delete temp;
        }
    }
}

// This is for the visualization of the hash table's keys
vector<vector<int>> HashMap::values() {
    vector<vector<int>> result;
    for (auto node : table) {
        vector<int> keys;
        while (node) {
            keys.push_back(node->key);
            node = node->next;
        }
        result.push_back(keys);
    }
    return result;
}

