#include "heap.h"

int main(){

    Heap heap;
    heap.insert(3);
    heap.insert(2);
    heap.insert(4);

    heap.printHeap();

    return 0;

}