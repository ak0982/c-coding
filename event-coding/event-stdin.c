#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <event2/event.h>
#include <time.h>

// Callback for stdin event
void stdin_callback(evutil_socket_t fd, short events, void *arg) {
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), stdin)) {
        printf("Read from stdin: %s", buffer);
    }
}

// Callback for timer event
void timer_callback(evutil_socket_t fd, short events, void *arg) {
    time_t now = time(NULL);
    printf("Timer event: Current time is %s", ctime(&now));
}

int main() {
    struct event_base *base;
    struct event *stdin_event, *timer_event;
    struct timeval timer_interval = {2, 0};  // Timer interval: 2 seconds

    // Initialize the event base
    base = event_base_new();
    if (!base) {
        perror("Could not initialize event base");
        return 1;
    }

    // Create an event for stdin (file descriptor 0)
    stdin_event = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, stdin_callback, NULL);
    if (!stdin_event) {
        perror("Could not create stdin event");
        event_base_free(base);
        return 1;
    }
    event_add(stdin_event, NULL); // Add stdin event to the event base

    // Create a timer event
    timer_event = event_new(base, -1, EV_PERSIST, timer_callback, NULL);
    if (!timer_event) {
        perror("Could not create timer event");
        event_free(stdin_event);
        event_base_free(base);
        return 1;
    }
    event_add(timer_event, &timer_interval); // Add timer event with interval

    // Run the event loop
    printf("Starting event loop...\n");
    event_base_dispatch(base);

    // Cleanup
    event_free(stdin_event);
    event_free(timer_event);
    event_base_free(base);

    return 0;
}

