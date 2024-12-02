#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>  // Include this for bufferevent functions and constants

#define MAX_THREADS 4
#define SERVER_PORT 9000

// Thread context structure to store the event base for each thread
struct thread_context {
    struct event_base *event_base;
};

// Thread-local storage for the current thread's context
__thread struct thread_context current_thread_ctx;

// Function to get the current thread's context
struct thread_context* get_thread_context() {
    return &current_thread_ctx;
}

// Callback to handle data received from the client
void read_cb(struct bufferevent *bev, void *ctx) {
    char buffer[256];
    int n = bufferevent_read(bev, buffer, sizeof(buffer) - 1);
    buffer[n] = '\0';  // Null-terminate the string
    printf("Received message: %s\n", buffer);

    // Send the same message back to the client (echo)
    bufferevent_write(bev, buffer, n);
}

// Callback to handle events (errors, EOF, etc.) on the bufferevent
void event_cb(struct bufferevent *bev, short events, void *ctx) {
    if (events & BEV_EVENT_ERROR) {
        perror("Error on bufferevent");
    }
    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        bufferevent_free(bev); // Free the bufferevent when done
    }
}

// Callback for accepting new client connections
void accept_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void *arg) {
    struct thread_context *thread_ctx = get_thread_context();

    if (!thread_ctx || !thread_ctx->event_base) {
        fprintf(stderr, "Error: Invalid thread context\n");
        close(fd);
        return;
    }

    // Create a new bufferevent for this connection
    struct bufferevent *bev = bufferevent_socket_new(thread_ctx->event_base, fd, BEV_OPT_CLOSE_ON_FREE);
    if (!bev) {
        fprintf(stderr, "Error creating bufferevent\n");
        close(fd);
        return;
    }

    // Set callback functions for reading and events
    bufferevent_setcb(bev, read_cb, NULL, event_cb, thread_ctx);
    bufferevent_enable(bev, EV_READ | EV_WRITE); // Enable read and write events
}

// Thread function to start an event loop for each thread
void* thread_func(void *arg) {

    printf("Thread context address: %p\n", (void*)&current_thread_ctx);

    struct event_base *base = event_base_new();
    if (!base) {
        perror("Error initializing event base");
        return NULL;
    }

    // Set the current thread's event base
    current_thread_ctx.event_base = base;

    // Create a listener for new incoming connections
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SERVER_PORT);
    sin.sin_addr.s_addr = INADDR_ANY;

    struct evconnlistener *listener = evconnlistener_new_bind(base, accept_cb, NULL,
                                                              LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
                                                              -1, (struct sockaddr*)&sin, sizeof(sin));
    if (!listener) {
        perror("Error creating listener");
        event_base_free(base);
        return NULL;
    }

    printf("Worker thread started with event base: %p\n", base);
    event_base_dispatch(base);  // Start the event loop

    // Cleanup after event loop ends
    evconnlistener_free(listener);
    event_base_free(base);
    return NULL;
}

int main() {
    pthread_t threads[MAX_THREADS];

    // Create worker threads to handle the event loops
    for (int i = 0; i < MAX_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, thread_func, NULL) != 0) {
            perror("Error creating thread");
            return 1;
        }
    }

    // Wait for all worker threads to finish
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Chat server stopped.\n");
    return 0;
}

