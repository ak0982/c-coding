#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define MAX_MSG_TYPES 10

typedef enum {
    MSG_TYPE_INIT,
    MSG_TYPE_PROCESS_ORDER,
    MSG_TYPE_SHUTDOWN,
    MSG_TYPE_SWITCHON
} msg_type_t;

typedef struct {
    msg_type_t type;
    void *data;
} message_t;

// Message handlers array
typedef void (*msg_handler_t)(void *);
msg_handler_t handlers[MAX_MSG_TYPES];

// Registration function
void register_handler(msg_type_t type, msg_handler_t handler) {
    if (type < MAX_MSG_TYPES) {
        handlers[type] = handler;
    }
}

// Dispatch function
void dispatch_message(message_t *msg) {
    if (handlers[msg->type]) {
        handlers[msg->type](msg->data);
    } else {
        printf("No handler registered for message type %d\n", msg->type);
    }
}

// Example handler functions
void init_handler(void *data) {
    printf("INIT handler: %s\n", (char *)data);
}

void process_order_handler(void *data) {
    printf("Processing order: %s\n", (char *)data);
}

void shutdown_handler(void *data) {
    printf("Shutdown handler invoked.\n");
}

// Worker thread function
void *worker_thread(void *arg) {
    message_t *msg = (message_t *)arg;
    dispatch_message(msg);
    return NULL;
}

int main() {
    // Register handlers
    register_handler(MSG_TYPE_INIT, init_handler);
    register_handler(MSG_TYPE_PROCESS_ORDER, process_order_handler);
    register_handler(MSG_TYPE_SHUTDOWN, shutdown_handler);

    // Example messages
    message_t init_msg = {MSG_TYPE_INIT, "Initialize system"};
    message_t order_msg = {MSG_TYPE_PROCESS_ORDER, "Order #123"};
    message_t shutdown_msg = {MSG_TYPE_SHUTDOWN, NULL};
    message_t switchon_msg = {MSG_TYPE_SWITCHON, NULL};

    // Create threads for each message
    pthread_t threads[4];
    pthread_create(&threads[0], NULL, worker_thread, &init_msg);
    pthread_create(&threads[1], NULL, worker_thread, &order_msg);
    pthread_create(&threads[2], NULL, worker_thread, &shutdown_msg);
    pthread_create(&threads[3], NULL, worker_thread, &switchon_msg);

    // Join threads
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

