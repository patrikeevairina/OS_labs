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

	pipe(fd);
	switch (pid = fork())
	{
	case -1:
	{
		printf("Error\n");
		return -1;
	}
	case 0:
	{
		printf("Child: reading from pipe\n");
		close(fd[1]);
		
		read(fd[0], &pid, sizeof(pid_t));
		read(fd[0], &t, sizeof(time_t));
		close(fd[0]);
	
		sleep(5);
		printf("Parent: pid = %d\t time %s\n", pid, ctime(&t));
		time(&t);
		printf("Child: time %s\n", ctime(&t));
	}
	break;
	default:
	{
		printf("Parent: writing to pipe\n");
		close(fd[0]);
		
		write(fd[1], &pid, sizeof(pid_t));
		time(&t);
		write(fd[1], &t, sizeof(time_t));
		close(fd[1]);
		printf("Parent: waiting\n");
		wait(NULL);
	}
	}
	
	return 0;
}
