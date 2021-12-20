#include <sys/types.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/sem.h>

#define MEM_FILE "tmp"

typedef struct tp
{
	time_t time;
	pid_t pid;
}tp;

struct sembuf lock = {0, -1, 0}, unlock = {0,1,0};

int main()
{
	key_t key;
	if ((key = ftok(MEM_FILE, 5)) == -1)
	{
		perror("ftok");
		return(1);
	}
	
	int shm_id;
	if ((shm_id = shmget(key, sizeof (tp), 0666)) < 0)
	{
		perror("shmget");
		return(1);
	}

	tp* shm_ptr;
	if ((shm_ptr = shmat(shm_id, NULL, 0)) == (void *) -1)
	{
		perror("shmat");
		return(1);
	}

	int sem_id;	
	if ((sem_id = semget(key, 1, IPC_CREAT | 0666)) < 0) 
	{
		perror("semget");
		return(1);
	}
	
	semop(sem_id, &lock, 1);
	time_t t = time(NULL);
	printf("SENDER \ntime: %spid: %d\nRECEIVER \ntime: %spid: %d", ctime(&shm_ptr->time), shm_ptr->pid, ctime(&t), getpid());
	semop(sem_id, &unlock, 1);
	shmdt(shm_ptr);
	shmctl(sh_id, IPC_RMID,NULL);
	return 0;
}
