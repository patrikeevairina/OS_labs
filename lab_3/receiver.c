#include <sys/types.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

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
        if ((shm_id = shmget(key, sizeof(struct tp), 0666)) < 0)
        {
                perror("shmget");
                return 1;
        }

        struct tp* shm_ptr;
        if ((shm_ptr = shmat(shm_id, NULL, 0)) == (void*)-1)
        {
                perror("shmat");
                return -1;
        }

        time_t buf_time = shm_ptr->time;
        pid_t buf_pid = shm_ptr->pid;
        time_t t = time(NULL);
        printf("RECEIVER\ntime: %s\npid: %d\nSENDER\ntime: %spid: %d\n", ctime(&buf_time), buf_pid, ctime(&t), getpid());
        sleep(1);
        shmdt(shm_ptr);
}
