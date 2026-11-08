#include "heap.h"
#include <iostream>
using namespace std;

int main(){

    Heap heap;
    heap.insert(4);
    heap.insert(3);
    heap.insert(2);
    heap.insert(1);

    //heap.printHeap();

    int v = heap.extract_min();

    heap.printHeap();
    cout << "\n";

    v = heap.extract_min();

    heap.printHeap();
    cout << "\n";

    v = heap.extract_min();

    heap.printHeap();
    cout << "\n";

    return 0;

}