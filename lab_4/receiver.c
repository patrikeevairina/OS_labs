#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MEM_FILE "tmp"

struct sembuf sem_lock = {0,-1,0}, sem_open = {0,1,0};

typedef struct tp
{
	time_t time;
	pid_t pid;
}tp;

int main()
{
	key_t key;
	if ((key = ftok(MEM_FILE, 5)) == -1)
        {
                perror("ftok");
                return -1;
        }

        int shm_id;
        if ((shm_id = shmget(key, sizeof(tp), 0666)) < 0)
        {
                perror("shmget");
                return 1;
        }

	int sem_id;
	if ((sem_id = semget(key, 1, 0666)) == -1)
	{
		perror("semget");
		return 4;
	}

        struct tp* shm_ptr;
        if ((shm_ptr = shmat(shm_id, NULL, 0)) == (void*)-1)
        {
                perror("shmat");
                return -1;
        }
	
	semop(sem_id, &sem_lock, 1);
	tp buf = {shm_ptr->time, shm_ptr->pid};
	time_t t = time(NULL);
	printf("RECEIVER\ntime: %spid: %d\nSENDER\ntime: %spid: %d\n", ctime(&t), getpid(), ctime(&buf.time), buf.pid);

	sleep(1);
	semop(sem_id, &sem_open, 1);
	
	shmdt(shm_ptr);
	return 0;
}
