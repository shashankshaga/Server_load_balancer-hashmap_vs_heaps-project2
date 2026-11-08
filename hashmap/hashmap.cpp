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

//  I put my own twist on this
int HashMap::hash(int key) {

    unsigned int val = key * 2654435761u;
    return val % table.size();
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

    if (count * 4 > table.size() * 3) {
        rehash();
    }
}


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


int HashMap::size() {
    return count;
}

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

