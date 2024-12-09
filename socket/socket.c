#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[4096];

    // 1. Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    // 2. Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80); // HTTP port
    inet_pton(AF_INET, "93.184.216.34", &server_addr.sin_addr); // IP for example.com
//    inet_pton(AF_INET, "104.18.25.140", &server_addr.sin_addr); // IP for example.com

    // 3. Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sockfd);
        return 1;
    }

    // 4. Send an HTTP GET request
    char request[] = "GET / HTTP/1.0\r\nHost: example.com\r\n\r\n";
//    char request[] = "GET / HTTP/1.0\r\nHost: neverssl.com\r\n\r\n";
    send(sockfd, request, strlen(request), 0);

    // 5. Receive the response
    int bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0'; // Null-terminate the response
        printf("Response:\n%s\n", buffer);
    }

    // 6. Close the socket
    close(sockfd);
    return 0;
}

