#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define message types
enum msg_type {
    MSG_TYPE_INIT,
    MSG_TYPE_PROCESS_ORDER,
    MSG_TYPE_SHUTDOWN
};

// Message structure
typedef struct {
    enum msg_type type;
    void *data; // Pointer to additional data
} message_t;

// Define a handler function type
typedef void (*msg_handler_t)(message_t *);

// Handlers for different message types
void init_handler(message_t *msg) {
    printf("Initializing resources...\n");
}

void process_order_handler(message_t *msg) {
    char *order = (char *)msg->data;
    printf("Processing order: %s\n", order);
}

void shutdown_handler(message_t *msg) {
    printf("Shutting down...\n");
}

// Thread-safe message queue (simplified with a static array)
#define MAX_MESSAGES 10
message_t queue[MAX_MESSAGES];
int front = 0, rear = 0;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

// Enqueue a message
void enqueue_message(message_t *msg) {
    pthread_mutex_lock(&queue_mutex);
    if ((rear + 1) % MAX_MESSAGES == front) {
        printf("Queue is full!\n");
    } else {
        queue[rear] = *msg;
        rear = (rear + 1) % MAX_MESSAGES;
    }
    pthread_mutex_unlock(&queue_mutex);
}

// Dequeue a message
int dequeue_message(message_t *msg) {
    pthread_mutex_lock(&queue_mutex);
    if (front == rear) {
        pthread_mutex_unlock(&queue_mutex);
        return 0; // Queue is empty
    }
    *msg = queue[front];
    front = (front + 1) % MAX_MESSAGES;
    pthread_mutex_unlock(&queue_mutex);
    return 1;
}

// Worker thread function
void *worker_thread(void *arg) {
    msg_handler_t *handlers = (msg_handler_t *)arg;
    message_t msg;

    while (1) {
        if (dequeue_message(&msg)) {
            // Dispatch the message to the appropriate handler
            handlers[msg.type](&msg);
            if (msg.type == MSG_TYPE_SHUTDOWN)
                break;
        }
    }
    return NULL;
}

int main() {
    // Register handlers
    msg_handler_t handlers[3] = {init_handler, process_order_handler, shutdown_handler};

    // Start worker thread
    pthread_t thread;
    pthread_create(&thread, NULL, worker_thread, handlers);

    // Send messages
    message_t msg1 = {MSG_TYPE_INIT, NULL};
    enqueue_message(&msg1);

    message_t msg2 = {MSG_TYPE_PROCESS_ORDER, "Order #123"};
    enqueue_message(&msg2);

    message_t msg3 = {MSG_TYPE_SHUTDOWN, NULL};
    enqueue_message(&msg3);

    // Wait for worker thread to complete
    pthread_join(thread, NULL);

    return 0;
}

