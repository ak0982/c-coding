#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <event2/event.h>
#include <event2/util.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define PORT 8080

// Client structure to store client information
struct client {
    int fd;
    struct event *read_event;
};

// Global variables to hold clients
struct client *clients[MAX_CLIENTS];
int num_clients = 0;

// Callback function to handle client messages
void client_callback(evutil_socket_t client_fd, short events, void *arg) {
    char buffer[1024];
    int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    
    if (bytes_read <= 0) {
        // Handle client disconnect (close and remove client)
        close(client_fd);
        struct client *cli = (struct client *)arg;
        event_free(cli->read_event);
        free(cli);
        return;
    }

    buffer[bytes_read] = '\0';  // Null-terminate the message
    printf("Broadcasting message: %s", buffer);  // Debug: print the message being broadcast

    // Broadcast the message to all clients except the sender
    for (int i = 0; i < num_clients; ++i) {
        if (clients[i]->fd != client_fd) {
   	    printf("Broadcasting message to this client fd :%d: \n", clients[i]->fd);  // Debug: print the message being broadcast
            int bytes_sent = write(clients[i]->fd, buffer, bytes_read);
            if (bytes_sent < 0) {
                perror("Error sending message to client");
            }
        }
    }
}


// Callback function to accept new client connections
void accept_callback(evutil_socket_t listener, short events, void *arg) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    int client_fd = accept(listener, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("Failed to accept client");
        return;
    }

    printf("New client connected: %s, client_fd is :%d: \n", inet_ntoa(client_addr.sin_addr), client_fd);

    if (num_clients >= MAX_CLIENTS) {
        printf("Maximum clients reached. Closing connection.\n");
        close(client_fd);
        return;
    }

    // Add the new client
    struct client *new_client = (struct client *)malloc(sizeof(struct client));
    new_client->fd = client_fd;

    // Create an event for the new client to handle incoming data
    new_client->read_event = event_new((struct event_base *)arg, client_fd, EV_READ | EV_PERSIST, client_callback, new_client);
    event_add(new_client->read_event, NULL);

    // Store the client
    clients[num_clients++] = new_client;
}
// Main function to set up the server and run the event loop
int main() {
    struct event_base *base = event_base_new();
    if (!base) {
        perror("Could not initialize event base");
        return 1;
    }

    // Create the server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Could not create socket");
        event_base_free(base);
        return 1;
    }

    // Bind the socket to an address and port
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        event_base_free(base);
        return 1;
    }

    // Start listening for incoming connections
    if (listen(server_fd, 10) == -1) {
        perror("Listen failed");
        close(server_fd);
        event_base_free(base);
        return 1;
    }

    // Create an event to accept new client connections
    struct event *accept_event = event_new(base, server_fd, EV_READ | EV_PERSIST, accept_callback, base);
    event_add(accept_event, NULL);

    // Start the event dispatch loop
    printf("Server started on port %d...\n", PORT);
    event_base_dispatch(base);

    // Cleanup
    close(server_fd);
    event_free(accept_event);
    event_base_free(base);

    return 0;
}

