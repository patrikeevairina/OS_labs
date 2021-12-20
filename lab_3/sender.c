#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define MEM_FILE "tmp"

struct tp
{
        time_t time;
        pid_t pid;
};

int main()
{
        key_t key;
        if ((key = ftok(MEM_FILE, 5)) == -1)
        {
                perror("ftok");
                return -1;
        }

        int shm_id;
        if ((shm_id = shmget(key, sizeof(struct tp), IPC_CREAT|0666)) < 0)
        {
                perror("shmget");
                return 1;
        }

        struct tp* shm_ptr;
        if ((shm_ptr = shmat(shm_id, NULL, 0)) == (void*)-1)
        {
                perror("shmat");
                return 2;
        }

	struct shmid_ds buf;
	shmctl(shm_id, IPC_STAT, &buf);
	if (buf.shm_nattch > 1)
	{
		printf("Trying to open second server");
		return 2;	
	}
	
        while(1)
        {
                struct tp buf;
                buf.time = time(NULL);
                buf.pid = getpid();
                *shm_ptr = buf;
                sleep(2);
        }
	shmdt(shm_ptr);
}
