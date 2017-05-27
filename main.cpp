#include <iostream>
#include <unistd.h>

#include "include/block_queue.h"

using namespace std;

class TTT{
public:
    int a;
};

template <typename T>
int mycmp(T a, T b)
{
    return a > b;
}

void* customer(void* arg) {

    int task = -1;
    block_queue<int>* bq = (block_queue<int>*)arg;

    for(int i = 0; i < 100; ++i) {
        bq->get(task);
        cout << "customer get : " << task << endl;
        usleep(100);
    }
}

void* producer(void* arg) {
    block_queue<int>* bq = (block_queue<int>*)arg;

    for(int i = 0; i < 1000; ++i) {
        bq->put(i);
       cout << "producer put :" << i << endl;
    }
}

int main(int argc, char** argv) {
    /*
    pthread_t tid[2];

    block_queue<int>* bq = new block_queue<int>(500);

    pthread_create(tid, NULL, producer, (void*)bq);
    pthread_create(tid+1, NULL, customer, (void*)bq);

    pthread_join(*tid, NULL);
    pthread_join(*(tid+1), NULL);

    cout << "the number of bq is : " << bq->size() << endl;

    delete bq;*/

    TTT a, b;
    a.a = 1;
    b.a = 2;

    try {
        mycmp(a, b);
    }
    catch(exception e){
            cout << e.what() << endl;
    }
}