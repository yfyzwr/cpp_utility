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

	/*int shmid = 0;
	prcinfo* shmaddr = NULL;

	shmid = shmget(IPC_PRIVATE, sizeof(prcinfo), IPC_CREAT|0600);
	if ( shmid < 0 )
	{
      		perror("get shm  ipc_id error") ;
       		return -1 ;
	}

	shmaddr = (prcinfo *) shmat(shmid, NULL, 0 ) ;
        if ( (int)shmaddr == -1 )
        {
		perror("shmat addr error");
		return -1;
	}

	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&(shmaddr->f_lock), &attr);
	*/
	while(!isStop)
	{
		if(iProcNum > 0)
		{
			tempPid = fork();
			if(tempPid > 0)
			{
				aPid[--iProcNum] = tempPid;
				printf("this is parent process, create child process[%d]\n", aPid[iProcNum]);

				shlock.lock();
				shlock.unlock();
			}
			else if(tempPid == 0)
			{
				//printf("fork pid[%d] --- iProcNum[%d]\n", getpid(), iProcNum);

				/*struct sigaction hup_act;
    				memset(&hup_act, 0, sizeof(hup_act));
    				hup_act.sa_handler = sighup_hander;
    				sigemptyset(&hup_act.sa_mask);
				//hup_act.sa_flags = SA_SIGINFO;
				//hup_act.sa_sigaction = sighup_hander_else;:
    				//sigaction(SIGHUP, &hup_act_else, 0);
				sigaction(SIGHUP, &hup_act, 0);*/

				shlock.local_addr_map();

				sleep(3);


                shlock.lock();
                shlock.unlock();

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
