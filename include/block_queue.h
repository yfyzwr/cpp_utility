#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <iostream>
#include <deque>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define SUCCESS         10
#define ERROR_EMPTY     1
#define ERROR_FULL      2
#define ERROR_TIMEOUT   3
#define ERROR_ARGUMENT  4
#define ERROR_VALUE     5
#define ERROR_OTHER     9999

using namespace std;

template <typename T>
class block_queue{

public:
    block_queue(const int max_size){

        this->m_max_size = max_size;
        this->m_task_size = 0;
        this->m_unfinished_tasks = 0;
        this->m_deque = new deque<T>();         // 初始时不分配存储空间

        this->m_mutex = new pthread_mutex_t;
        this->m_not_empty = new pthread_cond_t;
        this->m_not_full = new pthread_cond_t;
        this->m_all_tasks_done = new pthread_cond_t;

        pthread_mutex_init(this->m_mutex, NULL);
        pthread_cond_init(this->m_not_empty, NULL);
        pthread_cond_init(this->m_not_full, NULL);
        pthread_cond_init(this->m_all_tasks_done, NULL);
    }

    ~block_queue(){

        if(this->m_deque != NULL){
            delete this->m_deque;
        }

        pthread_mutex_destroy(this->m_mutex);
        pthread_cond_destroy(this->m_not_empty);
        pthread_cond_destroy(this->m_not_full);
        pthread_cond_destroy(this->m_all_tasks_done);

        delete this->m_mutex;
        delete this->m_not_empty;
        delete this->m_not_full;
        delete this->m_all_tasks_done;
    }

    bool empty(){

        bool result = false;

        pthread_mutex_lock(this->m_mutex);
        result = this->m_task_size == 0;
        pthread_mutex_unlock(this->m_mutex);

        return result;
    }

    bool full(){

        bool result = false;

        pthread_mutex_lock(this->m_mutex);
        result = this->m_task_size == this->m_max_size;
        pthread_mutex_unlock(this->m_mutex);

        return result;
    }

    int clear(){

        pthread_mutex_lock(this->m_mutex);

        this->m_task_size = 0;
        this->m_unfinished_tasks = 0;
        this->m_deque->clear();

        pthread_mutex_unlock(this->m_mutex);

        return SUCCESS;
    }

    int size(){

        int iResult = 0;

        pthread_mutex_lock(this->m_mutex);
        iResult = (int)this->m_deque->size();
        pthread_mutex_unlock(this->m_mutex);

        return iResult;
    }

    int put(const T& task, const bool block = true, const int timeout = -1){

        struct timespec ts = {0, 0};
        struct timeval now = {0, 0};

        gettimeofday(&now, NULL);
        ts.tv_sec = now.tv_sec + timeout;
        ts.tv_nsec = now.tv_usec * 1000;

        pthread_mutex_lock(this->m_mutex);

        if(block == false) {
            if(this->m_task_size == this->m_max_size) {
                pthread_mutex_unlock(this->m_mutex);
                return ERROR_FULL;
            }
        }
        else if(timeout < 0){
            while(this->m_task_size == this->m_max_size) {
                pthread_cond_wait(this->m_not_full, this->m_mutex);
            }
        }
        else{
            while(this->m_task_size == this->m_max_size) {
                int iResult = SUCCESS;
                iResult = pthread_cond_timedwait(this->m_not_full, this->m_mutex, &ts);
                if(iResult == ETIMEDOUT) {
                    pthread_mutex_unlock(this->m_mutex);
                    return ERROR_TIMEOUT;
                }
                else if(iResult != SUCCESS) {
                    pthread_mutex_unlock(this->m_mutex);
                    return ERROR_OTHER;
                }
            }
        }

        this->m_deque->push_back(task);
        this->m_task_size += 1;
        this->m_unfinished_tasks += 1;

        pthread_cond_signal(this->m_not_empty);
        pthread_mutex_unlock(this->m_mutex);

        return SUCCESS;
    }

    int put_nowait(T& task){

        return put(task, false);
    }

    int get(T& task, const bool block = true, const int timeout = -1) {

        struct timespec ts = {0, 0};
        struct timeval now = {0, 0};

        gettimeofday(&now, NULL);
        ts.tv_sec = now.tv_sec + timeout;
        ts.tv_nsec = now.tv_usec * 1000;

        pthread_mutex_lock(this->m_mutex);

        if(block == false){
            if(this->m_task_size == 0) {
                pthread_mutex_unlock(this->m_mutex);
                return ERROR_EMPTY;
            }
        }
        else if(timeout < 0){
            while(this->m_task_size == 0) {
                pthread_cond_wait(this->m_not_empty, this->m_mutex);
            }
        }
        else {
            while(this->m_task_size == 0) {
                int iResult = SUCCESS;
                iResult = pthread_cond_timedwait(this->m_not_empty, this->m_mutex, &ts);
                if(iResult == ETIMEDOUT) {
                    pthread_mutex_unlock(this->m_mutex);
                    return ERROR_TIMEOUT;
                }
                else if(iResult != SUCCESS) {
                    pthread_mutex_unlock(this->m_mutex);
                    return ERROR_OTHER;
                }
            }
        }

        memcpy(&task, &this->m_deque->front(), sizeof(T));
        this->m_deque->pop_front();
        this->m_task_size -= 1;
        pthread_cond_signal(this->m_not_full);
        pthread_mutex_unlock(this->m_mutex);

        return SUCCESS;
    }

    int get_nowait(T& task){

        return get(task, false);
    }

    int task_done(){

        int unfinished = 0;

        pthread_mutex_lock(this->m_mutex);

        unfinished = this->m_unfinished_tasks - 1;
        if(unfinished < 0) {
            pthread_mutex_unlock(this->m_mutex);
            return ERROR_VALUE;
        }
        else if(unfinished == 0) {
            pthread_cond_broadcast(this->m_all_tasks_done);
        }
        this->m_unfinished_tasks = unfinished;

        pthread_mutex_unlock(this->m_mutex);
        return SUCCESS;
    }

    int join(){

        pthread_mutex_lock(this->m_mutex);
        while(this->m_unfinished_tasks != 0) {
            pthread_cond_wait(this->m_all_tasks_done, this->m_mutex);
        }
        pthread_mutex_unlock(this->m_mutex);

        return SUCCESS;
    }

private:
    deque<T>* m_deque = NULL;
    int m_max_size = 0;
    int m_task_size = 0;
    int m_unfinished_tasks = 0;

    pthread_mutex_t* m_mutex = NULL;
    pthread_cond_t* m_not_empty = NULL;
    pthread_cond_t* m_not_full = NULL;
    pthread_cond_t* m_all_tasks_done = NULL;
};

#endif //BLOCK_QUEUE_H
