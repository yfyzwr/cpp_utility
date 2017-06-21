#include <iostream>
#include <unistd.h>

//#include "include/block_queue.h"
#include "include/min_heap.h"
#include "include/common_tools.h"
#include "third-party/protobuf/Person.pb.h"

using namespace std;

//void* customer(void* arg) {
//
//    int task = -1;
//    block_queue<int>* bq = (block_queue<int>*)arg;
//
//    for(int i = 0; i < 100; ++i) {
//        bq->get(task);
//        cout << "customer get : " << task << endl;
//        usleep(100);
//    }
//}
//
//void* producer(void* arg) {
//    block_queue<int>* bq = (block_queue<int>*)arg;
//
//    for(int i = 0; i < 1000; ++i) {
//        bq->put(i);
//       cout << "producer put :" << i << endl;
//    }
//}

int main(int argc, char** argv) {

//    pthread_t tid[2];
//
//    block_queue<int>* bq = new block_queue<int>(500);
//
//    pthread_create(tid, NULL, producer, (void*)bq);
//    pthread_create(tid+1, NULL, customer, (void*)bq);
//
//    pthread_join(*tid, NULL);
//    pthread_join(*(tid+1), NULL);
//
//    cout << "the number of bq is : " << bq->size() << endl;
//
//    delete bq;

    min_heap<int> mHeap(10);
    int a = 12;
    mHeap.insert(a);
    a = 4;
    mHeap.insert(a);
    a = 8;
    mHeap.insert(a);
    a = 64;
    mHeap.insert(a);
    a = 34;
    mHeap.insert(a);
    a = 6;
    mHeap.insert(a);
    a = 5;
    mHeap.insert(a);
    a = 9;
    mHeap.insert(a);

    int b;
    mHeap.pop_root(b);

    tutorial::Person person;
    person.set_name("yangfayou");
    person.set_id(1234);
    person.IsInitialized();

    string temp;
    person.SerializeToString(&temp);
    cout<< temp << endl;

    tutorial::Person haha;
    haha.ParseFromString(temp);
    cout<< haha.name() << "-----" << haha.id() << endl;


    return 0;
}