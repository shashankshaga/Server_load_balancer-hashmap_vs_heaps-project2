#include <iostream>
#include <vector>

class Heap{

    private:
        int curr_size = 0;
        int curr_idx = 0;
        std::vector<int> heap_array;

        void heapify_up(int i){
            if (i==0){return;}
            int parent = (i - 1) / 2;
            if (parent < 0){return;}
            if (heap_array[parent] > heap_array[i]){
                int t = heap_array[parent];
                heap_array[parent] = heap_array[i];
                heap_array[i] = t;
                heapify_up(parent);
            }
        }

    public:
        Heap(){}
        void insert(int v){
            heap_array.push_back(v);
            curr_size++;
            curr_idx = curr_size - 1;
            heapify_up(curr_idx);
        }
        void printHeap(){
            for (int i = 0; i < curr_size; i++){
                std::cout << heap_array[i] << " ";
            }
        }

};