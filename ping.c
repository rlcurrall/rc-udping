#include <arpa/inet.h>
#include <getopt.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "ping.h"

void *producer(void *options);
void *consumer(void *arg);
void handler(int signal_number);
void make_ping_request(int sockfd, struct sockaddr_in servaddr);

pthread_t sender, receiver;
sig_atomic_t doneFlag = 0;

int parse_ping_options(int argc, char *argv[], PingOptions *options)
{
    int opt;

    while ((opt = getopt(argc, argv, "r:p:d:n:")) != -1)
    {
        switch (opt)
        {
        case 'r':
            options->remote_host = optarg;
            break;
        case 'p':
            options->remote_port = atoi(optarg);
            break;
        case 'd':
            options->delay = atoi(optarg);
            break;
        case 'n':
            options->num_pings = atoi(optarg);
            break;
        default:
            return 1;
        }
    }

    return 0;
}

int start_ping(PingOptions *options)
{
    srand(time(NULL));
    signal(SIGTERM, handler);

    // Create the sender thread
    if (pthread_create(&sender, NULL, producer, (void *)options) != 0)
    {
        printf("Error creating sender thread\n");
        return 1;
    }

    // Create the receiver thread
    if (pthread_create(&receiver, NULL, consumer, (void *)options) != 0)
    {
        printf("Error creating receiver thread\n");
        return 1;
    }

    // Wait for the threads to finish
    pthread_join(sender, NULL);
    pthread_join(receiver, NULL);

    return 0;
}

void *producer(void *options)
{
    struct PingOptions *ping_options = (struct PingOptions *)options;

    // Create a UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket() failed");
        exit(1);
    }

    // Set the server address and port
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ping_options->remote_host);
    servaddr.sin_port = htons(ping_options->remote_port);

    // Close the socket

    for (int i = 0; i < ping_options->num_pings; i++)
    {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        make_ping_request(sockfd, servaddr);
        clock_gettime(CLOCK_MONOTONIC, &end);
        int elapsed = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
        enqueue(ping_options->queue, elapsed);
    }

    if (pthread_kill(receiver, SIGTERM) != 0)
    {
        printf("Error sending signal to receiver thread\n");
        close(sockfd);
        exit(1);
    }

    close(sockfd);
    pthread_exit(NULL);
}

void make_ping_request(int sockfd, struct sockaddr_in servaddr)
{
    // Send a UDP ping request to the server
    int n = sendto(sockfd, "PING", strlen("PING"), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (n < 0)
    {
        perror("sendto() failed");
        exit(1);
    }

    // Wait for the ping response from the server
    char buf[1024];
    struct sockaddr_in fromaddr;
    socklen_t fromaddrlen = sizeof(fromaddr);

    n = recvfrom(sockfd, buf, 1024, 0, (struct sockaddr *)&fromaddr, &fromaddrlen);
    if (n < 0)
    {
        perror("recvfrom() failed");
        exit(1);
    }

    // Check if the received packet is a ping response
    if (strcmp(buf, "PING") != 0)
    {
        printf("Received packet is not a UDP ping response\n");
    }
}

void *consumer(void *options)
{
    struct PingOptions *ping_options = (struct PingOptions *)options;

    int count = 0;
    int sum = 0;
    int max = 0;
    int min = 100;

    while (!doneFlag || !isEmpty(ping_options->queue))
    {
        if (!isEmpty(ping_options->queue))
        {
            int num = dequeue(ping_options->queue);

            count++;
            sum += num;
            if (num > max)
            {
                max = num;
            }
            if (num < min)
            {
                min = num;
            }
        }
    }

    printf("Statistics\n");
    printf("    Request Count: %d\n", count);
    printf("    Sum (ms):      %f\n", (float) sum / 1000000);
    printf("    Max (ms):      %f\n", (float) max / 1000000);
    printf("    Min (ms):      %f\n", (float) min / 1000000);
    printf("    Average (ms):   %f\n", (float) sum / count / 1000000);

    pthread_exit(NULL);
}

void handler(int signal_number)
{
    doneFlag = 1;
}
