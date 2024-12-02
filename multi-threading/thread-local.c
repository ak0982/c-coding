#include <pthread.h>
#include <stdio.h>

__thread int thread_var = 0;  // Thread-local variable

void* thread_func(void* arg) {
    thread_var++;  // Each thread has its own instance of thread_var
    printf("Thread ID: %ld, thread_var: %d\n", pthread_self(), thread_var);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, thread_func, NULL);
    pthread_create(&t2, NULL, thread_func, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}

