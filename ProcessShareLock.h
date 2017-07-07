#ifndef PROCESSSHARELOCK_H
#define PROCESSSHARELOCK_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "iostream"

using namespace std;

template<typename T>
class ProcessShareLock{

public:

    class InnerErrorCode{

    public:
        const static int SUCESS = 0;
        const static int ECREATESHM = -1;
        const static int EADDRMAP = -2;
        const static int ELOCKBUSY = -3;
        const static int ETRYLOCKERROR = -4;
    };

public:
    int init(){
        this->m_shmid = shmget(IPC_PRIVATE, sizeof(pthread_mutex_t) + sizeof(T), IPC_CREAT|0600);
        if ( this->m_shmid < 0 )
        {
            return InnerErrorCode::ECREATESHM;
        }

        return InnerErrorCode::SUCESS;
    }

    int local_addr_map() {
        this->m_pLock = (pthread_mutex_t*) shmat(this->m_shmid, NULL, 0);
        if ((int)this->m_pLock == -1) {
            return InnerErrorCode::EADDRMAP;
        }

        this->m_pData = (T*)(this->m_pLock + sizeof(T));

        return InnerErrorCode::SUCESS;
    }

    int set_lock_share(){
        pthread_mutexattr_init(&this->m_attr);
        pthread_mutexattr_setpshared(&this->m_attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(this->m_pLock, &this->m_attr);
    };

    int try_lock(){
        int ret = pthread_mutex_trylock(this->m_pLock);
        if(EBUSY == ret){
            return InnerErrorCode::ELOCKBUSY;
        }else if(0 != ret){
            return InnerErrorCode::ETRYLOCKERROR;
        }

        (*this->m_pData)++;
        return InnerErrorCode::SUCESS;
    }

    int lock(){
        int ret = pthread_mutex_lock(this->m_pLock);

        cout << "pid[" << getpid() << "]lock" << endl;
        (*this->m_pData)++;
        cout << "data [" << *(this->m_pData) << "]" << endl;
    }

    int unlock(){
        cout << "pid[" << getpid() << "]unlock" << endl;
        int ret = pthread_mutex_unlock(this->m_pLock);
    }

    int release_source(){
        pthread_mutex_destroy(this->m_pLock);
        pthread_mutexattr_destroy(&this->m_attr);
    }

private:
    int m_shmid;

    pthread_mutexattr_t m_attr;
    pthread_mutex_t* m_pLock;
    T* m_pData;
};

#endif //PROCESSSHARELOCK_H
