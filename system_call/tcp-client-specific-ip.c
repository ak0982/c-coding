#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // 1. Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Connect to the server at 10.0.2.15
    if (inet_pton(AF_INET, "10.0.2.15", &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 3. Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at 10.0.2.15:%d\n", PORT);

    // 4. Send multiple messages
    while (1) {
        // Get message input from user
        printf("Enter message to send (type 'stop' to disconnect): ");
        fgets(buffer, sizeof(buffer), stdin);

        // Send the message to the server
        send(sockfd, buffer, strlen(buffer), 0);
        printf("Sent: %s\n", buffer);

        // If the user types "stop", disconnect from the server
        if (strcmp(buffer, "stop\n") == 0) {
            printf("Disconnecting from server...\n");
            break;
        }

        // Receive acknowledgment from server
        int bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';  // Null terminate the received message
            printf("Received from server: %s\n", buffer);
        }
    }

    // 5. Close socket
    close(sockfd);
    return 0;
}

