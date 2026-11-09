#include <iostream>
#include <vector>
#include <cmath>

class Heap{

    private:
        int curr_size = 0;
        int curr_idx = 0;
        std::vector<long long> heap_array;

        void heapify_up(int i){
            if (i==0){return;}
            int parent = (i - 1) / 2;
            if (parent < 0){return;}
            if (heap_array[parent] > heap_array[i]){
                long long t = heap_array[parent];
                heap_array[parent] = heap_array[i];
                heap_array[i] = t;
                heapify_up(parent);
            }
        }
        void heapify_down(int i){
            if (i == curr_size-1){return;}
            int left = 2 * i + 1;
            int right = 2 * i + 2;

            if (left < curr_size && right < curr_size){

                long long min_val = std::min(heap_array[left], heap_array[right]);
                int min_idx = min_val == heap_array[left] ? left : right;

                if (heap_array[min_idx] < heap_array[i]){
                    long long t = heap_array[min_idx];
                    heap_array[min_idx] = heap_array[i];
                    heap_array[i] = t;
                    heapify_down(min_idx);
                }
                else {return;}

            }
            else if (left < curr_size){

                long long v = heap_array[left];
                if (v < heap_array[i]){
                    heap_array[left] = heap_array[i];
                    heap_array[i] = v;
                    heapify_down(left);
                }
                else {return;}

            }
            else if (right < curr_size){

                long long v = heap_array[right];
                if (v < heap_array[i]){
                    heap_array[right] = heap_array[i];
                    heap_array[i] = v;
                    heapify_down(right);
                }
                else {return;}

            }
            return;

        }

    public:
        Heap(){}
        void insert(long long v){
            heap_array.push_back(v);
            curr_size++;
            curr_idx = curr_size - 1;
            heapify_up(curr_idx);
        }
        long long extract_min(){
            if (heap_array.empty()){
                return -1;
            }
            long long min = heap_array[0];
            long long last = heap_array[curr_size - 1];
            heap_array[0] = last;
            heap_array.pop_back();
            curr_size--;
            heapify_down(0);
            curr_idx = curr_size - 1;
            return min;
        }

        long long find_by_id(int id, long long id_space){
            for (int i = 0; i < curr_size; i++){
                if (heap_array[i] % id_space == id){
                    return heap_array[i];
                }
            }
            return -1;
        }
        void printHeap(){
            for (int i = 0; i < curr_size; i++){
                std::cout << heap_array[i] << " ";
            }
        }

};