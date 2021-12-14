#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_NAME "fifo_file"

int main()
{
	pid_t pid;
	time_t t;

	switch (pid = fork())
	{
	case -1:
	{
		perror("fork");
		return -1;
	}
	case 0:
	{
		printf("Child: reading\n");
		sleep(1);
		int fr;	
		fr = open(FIFO_NAME, O_RDONLY);
		read(fr, &pid, sizeof(pid_t));
		read(fr, &t, sizeof(time_t));
		if (close(fr) == -1)
		{
			perror("close fr");
			return 1;
		}
		
		sleep(5);
	
		printf("Parent: pid = %d\t time %s\n", pid, ctime(&t));
		time(&t);
		printf("Child: time = %s\n", ctime(&t));
	}
	break;
	default:
	{
		printf("Parent: writing\n");
		if (mkfifo(FIFO_NAME, 0777))
		{
			perror("mkfifo");
			return 1;
		}
		int fw;
		fw = open(FIFO_NAME, O_WRONLY);
		time(&t);
		write(fw, &pid, sizeof(pid_t));
		write(fw, &t, sizeof(time_t));
		if (close(fw) == -1)
		{
			perror("close fw");
			return -1;
		}
		printf("Parent: waiting\n");
		wait(NULL);
	}	
	}
	remove(FIFO_NAME);
	return 0;
}
