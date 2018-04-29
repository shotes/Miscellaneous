#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

class KernelSemaphore{
	int semid;
public:
	KernelSemaphore (int value, int count)
	{
		key_t key = ftok ("a.txt", count);
		semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);  // why IPC_CREAT, beyond the scope
		if (semid < 0){
            semid = semget(key, 1, IPC_CREAT | 0666);  // why IPC_CREAT, beyond the scope
		}
        else{
            struct sembuf sb = {0,value,0};
            if (semop(semid, &sb, 1) == -1) {
                exit(-1); /* error, check errno */
            }
        }
		// initialize the KernelSemaphore value to an initial value = value
		// initially it was 0 meaning LOCKED
	}

	void P(){
		struct sembuf sb = {0, -1, 0};
		if (semop(semid, &sb, 1) == -1) {
			perror("semop");
			exit(1);
		}
	}

	void V(){
		struct sembuf sb = {0, 1, 0};
		if (semop(semid, &sb, 1) == -1) {
			perror("semop");
			exit(1);
		}
	}

	~KernelSemaphore (){
		//union semun arg={0};
		if (semctl(semid, 0, IPC_RMID, 0) == -1) {
			perror("semctl");
			exit(1);
		}
	}
};
