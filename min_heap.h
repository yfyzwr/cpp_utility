#ifndef MIN_HEAP_H
#define MIN_HEAP_H

// 此结构非线程安全的
// 自定义类型需要实现操作符重载
// 需要实现复制构造函数
// 元素从 0 开始，对于位于ｉ的元素，其父节点是（ｉ-1）/2,左子节点是2*1+1,右子节点是2*ｉ+2.

template <typedef T>
class min_heap{

    class min_heap_errno{

    public:
        static const int SUCCESS = 0;
        static const int FULLERROR = 1;
    };

public:
    min_heap(unsigned int capacity){

        this->m_capacity = capacity;
        this->m_cur_size = 0;

        this->m_pHeap = new T[capacity];
    }

    bool is_full(){
        return this->m_cur_size >= this->m_capacity;
    }

    bool is_empty(){
        return this->m_cur_size == 0;
    }

    int built(T* valid_data, unsigned int number){

        for(int i = 0; i < number; ++i){
            this->m_pHeap[i] = valid_data[i];
        }

        this->m_cur_size = number;
        for (int i = (number-1)/2; i > 0; --i) {
            __heap_percolate_down(i);
        }
    }

    int insert(T& element){

        if(is_full() == true){
            return min_heap_errno::FULLERROR;
        }

        // 将新的元素置于最后，然后向上回溯过滤
        this->m_pHeap[this->m_cur_size] = element;    // 需要重载 = 操作符
        __heap_percolate_up(this->m_cur_size);
        this->m_cur_size++;
    }

    ~min_heap(){

        if(this->m_pHeap != NULL){
            delete this->m_pHeap;
        }
    }

private:
    int __heap_percolate_up(unsigned int position){

        T tmp = this->m_pHeap[position];
        unsigned int hole = position;

        for(; this->m_pHeap[hole] < this->m_pHeap[(hole-1)/2]; hole = (hole-1)/2){          //需要重载 < 操作符
            this->m_pHeap[hole] = this->m_pHeap[(hole-1)/2];                                // 当 hole == 0 的之后，会推出循环
        }

        this->m_pHeap[hole] = tmp;

        return  min_heap_errno::SUCCESS;
    }

    int __heap_percolate_down(unsigned int position){

        T tmp = this->m_pHeap[position];
        unsigned int hole = position;

        unsigned  int child = 0;
        for(; (hole*2+1) < this->m_cur_size; hole = child){

            child = hole*2+1;
            if(child < this->m_cur_size - 1 && this->m_pHeap[child] > this->m_pHeap[child + 1]){
                child++;
            }

            if(this->m_pHeap[child] < this->m_pHeap[hole]){
                this->m_pHeap[hole] = this->m_pHeap[child];
            } else{
                break;
            }
        }

        this->m_pHeap[hole] = tmp;

        return  min_heap_errno::SUCCESS;
    }

private:
    int m_capacity = 0;
    int m_cur_size = 0;

    T* m_pHeap = NULL;
};

#endif //MIN_HEAP_H
