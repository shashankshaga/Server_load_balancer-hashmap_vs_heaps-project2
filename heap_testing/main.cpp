#include "heap.h"

int main(){

    Heap heap;
    heap.insert(4);
    heap.insert(3);
    heap.insert(2);
    heap.insert(1);

    heap.printHeap();

    return 0;

}