#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/types.h>

pthread_mutex_t mutex;
pthread_cond_t condition;
int count = 0;

void* writing_t()
{

	while(1)
	{
		sleep(2);
		pthread_mutex_lock(&mutex);
		count++;	
		pthread_cond_broadcast(&condition);//pthread_cond_signal нагляднее иллюстрирует систему очередей, правда
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void* reading_t()
{
	while(1)
	{
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&condition, &mutex);
		printf("Tid: %ld, count: %i\n", pthread_self(), count);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	return NULL;
}
	
int main()
{
	pthread_t writer, readers[10];
	
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condition, NULL);
	
	pthread_create(&writer, NULL, writing_t, NULL);
	for (int i = 0; i < 10; ++i)
		pthread_create(&readers[i], NULL, reading_t, NULL);
	for (int i = 0; i < 10; ++i)
		pthread_join(readers[i], NULL);
	
	pthread_join(writer, NULL);
	
	pthread_cond_destroy(&condition);
	pthread_mutex_destroy(&mutex);
	return 0;
}
