#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>

int x = 0;
pthread_mutex_t mutex;
void *fun() {

	for(int i =0;i<1000000;i++) {
		pthread_mutex_lock(&mutex);
		x++;
		pthread_mutex_unlock(&mutex);
	}


}


int main(int argc , char *argv[]) {

	pthread_t t1[4];
	pthread_mutex_init(&mutex, NULL);

	for(int i = 0;i<4;i++) {
		if(pthread_create(t1 + i, NULL, &fun, NULL)) {
			return 1;
		}
		printf("Thread %d is started\n", i);
	}
	for(int i = 0;i<4;i++) {
		if(pthread_join(t1[i], NULL) != 0) return 3;
		printf("Thread %d is completed\n", i);
	}
	pthread_mutex_destroy(&mutex);
	printf("Value of x: %d\n", x);
	return 0;



}





