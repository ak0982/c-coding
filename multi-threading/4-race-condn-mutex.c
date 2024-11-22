#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>

int x = 0;
pthread_mutex_t mutex;
void *fun() {

	for(int i =0;i<100000000;i++) {
		pthread_mutex_lock(&mutex);
		x++;
		pthread_mutex_unlock(&mutex);
	}


}


int main(int argc , char *argv[]) {
	
	pthread_t t1, t2, t3, t4;
	pthread_mutex_init(&mutex, NULL);
	if(pthread_create(&t1, NULL, &fun, NULL)) {
		return 1;
	}
	if(pthread_create(&t2, NULL, &fun, NULL)) {
		return 2;
	}
	if(pthread_create(&t3, NULL, &fun, NULL)) {
		return 2;
	}
	if(pthread_create(&t4, NULL, &fun, NULL)) {
		return 2;
	}
	if(pthread_join(t1, NULL) != 0) return 3;
	if(pthread_join(t2, NULL) != 0) return 4;
	if(pthread_join(t3, NULL) != 0) return 3;
	if(pthread_join(t4, NULL) != 0) return 4;
	pthread_mutex_destroy(&mutex);	
	printf("Value of x: %d\n", x);
	return 0;



}

