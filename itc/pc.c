#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>

#define QUEUE_NAME "/order_queue"
#define MAX_MSG_SIZE 8192

// Producer: Simulates receiving customer orders
void producer() {
    mqd_t mq;
    char order[1024];
    
    // Open the queue for sending messages
    mq = mq_open(QUEUE_NAME, O_WRONLY | O_CREAT, 0644, NULL);
    if (mq == -1) {
        perror("Producer: Failed to open message queue");
        exit(1);
    }

    // Send multiple orders to the queue
    for (int i = 0; i < 5; i++) {
        sprintf(order, "Order #%d: Customer wants 5 items", i + 1);
        if (mq_send(mq, order, strlen(order) + 1, 0) == -1) {
            perror("Producer: Failed to send message");
            exit(1);
        }
        printf("Producer: Sent %s\n", order);
        sleep(1);
    }

    mq_close(mq); // Close the queue after sending
}

// Consumer: Simulates processing customer orders
void consumer() {
    mqd_t mq;
    char order[MAX_MSG_SIZE];

    // Open the queue for receiving messages
    mq = mq_open(QUEUE_NAME, O_RDONLY);
    if (mq == -1) {
        perror("Consumer: Failed to open message queue");
        exit(1);
    }

    // Receive and process orders from the queue
    while (1) {
        ssize_t bytes_read = mq_receive(mq, order, MAX_MSG_SIZE, NULL);
        if (bytes_read >= 0) {
            printf("Consumer: Processing %s\n", order);
            sleep(2); // Simulate processing time
        } else {
            perror("Consumer: Failed to receive message");
            exit(1);
        }
    }

    mq_close(mq); // Close the queue after receiving
}

int main() {
    // Create a separate process for producer and consumer
    pid_t pid = fork();
    if (pid == 0) {
        consumer(); // Child process runs the consumer
    } else if (pid > 0) {
        producer(); // Parent process runs the producer
        wait(NULL); // Wait for the consumer process to finish
    } else {
        perror("Failed to fork process");
        exit(1);
    }

    // Clean up the message queue
    mq_unlink(QUEUE_NAME);
    return 0;
}

