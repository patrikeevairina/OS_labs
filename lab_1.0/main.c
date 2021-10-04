#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void exit_call()
{
	printf("\nThe process %d with parent %d is completed.\n", getpid(), getppid());
}

int main()
{
	pid_t pid;
	atexit(exit_call);
	switch ( pid = fork() )
	{
		case 0:
		{
			printf("\nIt's child\n");
			printf("Pid: %d\n", getpid());
			printf("Ppid %d\n\n", getppid());
			exit(0);
		}	
		default:
		{
			int st;
			printf("\nIt's parent\n");
			printf("Pid: %d\n", getpid());
                        printf("Ppid %d\n", getppid());
			printf("\nWaiting for the child process to terminate\n");
			waitpid(pid, &st, 0);
			printf("Return status of the child process: %d\n", WEXITSTATUS(st));
			
		}
	}
	
	return 0;
}
