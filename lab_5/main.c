#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/types.h>

pthread_mutex_t mutex;
int count = 0;

_Noreturn void* writing_t()
{
	while(1)
	{
		pthread_mutex_lock(&mutex);
		count++;
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
}

_Noreturn void* reading_t()
{
	while(1)
	{
		pthread_mutex_lock(&mutex);
		printf("Tid: %ld, count: %i\n", pthread_self(), count);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
}
	
int main()
{
	pthread_t writer, readers[10];
	
	pthread_mutex_init(&mutex, NULL);

	pthread_create(&writer, NULL, writing_t, NULL);
	for (int i = 0; i < 10; ++i)
		pthread_create(&readers[i], NULL, reading_t, NULL);
	for (int i = 0; i < 10; ++i)
		pthread_join(readers[i], NULL);
	
	pthread_mutex_destroy(&mutex);
	return 0;
}
