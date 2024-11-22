#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>

int x = 0;
void *fun() {

	for(int i =0;i<1000;i++) {
		
		x++;
	}

	printf("Value is %d\n", x);

}


int main(int argc , char *argv[]) {
	
	pthread_t t1, t2;

	if(pthread_create(&t1, NULL, &fun, NULL)) {
		return 1;
	}
	if(pthread_create(&t2, NULL, &fun, NULL)) {
		return 2;
	}
	if(pthread_join(t1, NULL) != 0) return 3;
	if(pthread_join(t2, NULL) != 0) return 4;
	
	return 0;



}
