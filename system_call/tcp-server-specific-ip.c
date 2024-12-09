#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    char *ack_msg = "Message received\n";

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind to the specific IP address 10.0.2.15
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    // Set the specific IP address
    if (inet_pton(AF_INET, "10.0.2.15", &address.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 3. Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on 10.0.2.15:%d\n", PORT);

    // 4. Accept client connections
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_socket < 0) {
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 5. Handle multiple messages from the client
    while (1) {
        memset(buffer, 0, sizeof(buffer));  // Clear the buffer
        int bytes_received = recv(new_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            perror("recv failed or client disconnected");
            break;
        }
        printf("Received: %s\n", buffer);
        
        // Send acknowledgment
        send(new_socket, ack_msg, strlen(ack_msg), 0);
    }

    // 6. Close sockets
    close(new_socket);
    close(server_fd);
    return 0;
}

