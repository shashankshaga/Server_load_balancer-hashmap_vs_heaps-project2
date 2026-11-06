class Heap{

    private:
        const int SIZE = 10000;
        int curr_size = 0;
        int curr_idx = 0;
        int heap_array[SIZE];

        void heapify_up(int i){
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
            heap_array[curr_idx] = v;
            heapify_up(curr_idx);
            curr_size++;
            curr_idx++;
        }
        void printHeap(){
            for (int i = 0; i < curr_size; i++){
                std::cout << heap_array[i] << " ";
            }
        }

};