#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "ProcessShareLock.h"
#include "TcpSocket.h"

int main(int argc, char** argv)
{
	const int iProcCnt = 500;
	int iProcNum = iProcCnt;
	int aPid[iProcCnt] = {0};
	bool isStop = false;
	pid_t tempPid = 0;


    ProcessShareLock<int> shlock;
    shlock.init();
    shlock.local_addr_map();
    shlock.set_lock_share();

    TcpSocket tcpsock;
    tcpsock.server_init(22222, 100);
    tcpsock.setnonblocking(tcpsock.get_listen_socket(), true);

	while(!isStop)
	{
		if(iProcNum > 0)
		{
			tempPid = fork();
			if(tempPid > 0)
			{
				aPid[--iProcNum] = tempPid;
				//printf("this is parent process, create child process[%d]\n", aPid[iProcNum]);

				shlock.lock();
				shlock.unlock();
			}
			else if(tempPid == 0)
			{
				sleep(3);
                bool isHaveLock = false;

                tcpsock.epoll_init();

                shlock.local_addr_map();

                while(1)
                {
                    int iRet = shlock.try_lock();
                    if (iRet == ProcessShareLock::InnerErrorCode::ELOCKBUSY) {
                        bool bRet = tcpsock.epoll_del_sock(tcpsock.get_listen_socket());
                        if (bRet == false) {
                            printf("epoll_del_sock error!\n");
                        }

                        isHaveLock = false;
                    } else if (iRet == ProcessShareLock::InnerErrorCode::SUCESS) {
                        tcpsock.epoll_add_sock(tcpsock.get_listen_socket());
                        isHaveLock = true;
                    } else {
                        exit(0);
                    }

                    int EventNum = tcpsock.epoll_wait_get(-1);
                    if(isHaveLock)
                    {
                        for(int i = 0; i <EventNum; ++i)
                        {
                            int iClinetSockFd = 0;
                            tcpsock.accept(iClinetSockFd);
                            tcpsock.setnonblocking(iClinetSockFd, true);
                            tcpsock.epoll_add_sock(iClinetSockFd);
                        }
                    }
                    else
                    {

                    }

                    shlock.unlock();
                }

				//printf("child process pid[%d] end !\n", getpid());

				exit(0);
			}
			else
			{
				printf("fork error!\n");
			}
		}
		else
		{
			isStop = true;
		}
	}

	//for(int i = 0; i < iProcCnt; ++i)
	//{
	//	waitpid(-1, 0, 0);
	//}
	//pthread_mutex_destroy(&(shmaddr->f_lock));
	//pthread_mutexattr_destroy(&attr);
}
