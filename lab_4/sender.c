#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define MEM_FILE "tmp"

typedef struct tp
{
	time_t time;
	pid_t pid;
} tp;

struct sembuf sem_open = {0,1,0}, sem_lock = {0,-1,0};

int main()
{
	key_t key;
	if ((key = ftok(MEM_FILE, 5)) == -1)
	{
		perror("ftok");
		return 1;
	}

	int shm_id;
	if ((shm_id = shmget(key, sizeof(tp), IPC_CREAT|0666)) < 0)
	{
		perror("shmget");
		return -1;
	}
	
	int sem_id;
	if ((sem_id = semget(key, 1, IPC_CREAT|0666)) == -1)
	{
		perror("semget");
		return 2;
	}
	
	tp* shm_ptr;
	if ((shm_ptr = shmat(shm_id, NULL, 0)) == (void*)-1)
	{
		perror("shmat");
		return 4;
	}

	semop(sem_id, &sem_open, 1);
	if (semctl(sem_id, 0, GETVAL) == 0)
	{
		printf("\nTrying to open second sender\n");
		return 5;
	}
	
	while(1)
	{
		semop(sem_id, &sem_lock, 1);
		tp buf;
		buf.time = time(NULL);
		buf.pid = getpid();
		*shm_ptr = buf;
		semop(sem_id, &sem_open, 1);
		sleep(3);
		shmdt(shm_ptr);
	}
	return 0;
}
