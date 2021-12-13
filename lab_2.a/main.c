#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <wait.h>

int main()
{
	int fd[2];
	pid_t pid, child_pid;
	time_t t;

	if (pid = fork())
	{
		printf("Parent: writing to pipe\n");

		write(fd[1], &child_pid, sizeof(pid_t));
		time(&t);
		write(fd[1], &t, sizeof(time_t));
		
		printf("Parent: waiting\n");
		wait(NULL);
		printf("Parent exiting\n");
	}
	else
	{
		printf("Child: reading from pipe\n");
		
		read(fd[0], &pid, sizeof(pid_t));
		read(fd[0], &t, sizeof(time_t));
		sleep(1);
		
		printf("Parent: pid %d \ttime %s", pid, ctime(&t));
		time(&t);
		printf("Child: pid %d \ttime %s", child_pid, ctime(&t));
        
		printf("Child exiting\n");
	}
	close(fd[0]);
	close(fd[1]);
	return 0;
}
