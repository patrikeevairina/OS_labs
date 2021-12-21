#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/types.h>

int count = 0;
pthread_rwlock_t rw_lock;

void* writing_t()
{
	while(1)
	{
		sleep(1);
		pthread_rwlock_wrlock(&rw_lock);
		count++;	
		pthread_rwlock_unlock(&rw_lock);
	}
	return NULL;
}

void* reading_t()
{
	while(1)
	{
		pthread_rwlock_rdlock(&rw_lock);
		printf("Tid: %ld, count: %i\n", pthread_self(), count);
		pthread_rwlock_unlock(&rw_lock);
		sleep(1);
	}
	return NULL;
}
	
int main()
{
	pthread_t writer, readers[10];
	
	pthread_create(&writer, NULL, writing_t, NULL);
	for (int i = 0; i < 10; ++i)
		pthread_create(&readers[i], NULL, reading_t, NULL);
	for (int i = 0; i < 10; ++i)
		pthread_join(readers[i], NULL);
	
	pthread_join(writer, NULL);
	
	pthread_rwlock_destroy(&rw_lock);
	return 0;
}
