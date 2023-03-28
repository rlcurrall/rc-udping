#include <pthread.h>
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "ping.h"

void *producer(void *options);
void *consumer(void *arg);
void handler(int signal_number);

pthread_t sender, receiver;
sig_atomic_t doneFlag = 0;

int parse_ping_options(int argc, char *argv[], PingOptions* options) {
    int opt;

    while ((opt = getopt(argc, argv, "r:p:d:n:")) != -1) {
        switch (opt) {
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

int start_ping(PingOptions* options) {
    srand(time(NULL));
    signal(SIGTERM, handler);

    // Create the sender thread
    if (pthread_create(&sender, NULL, producer, (void*) options) != 0) {
        printf("Error creating sender thread\n");
        return 1;
    }

    // Create the receiver thread
    if (pthread_create(&receiver, NULL, consumer, (void*) options) != 0) {
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
    struct PingOptions *ping_options = (struct PingOptions*) options;

    for (int i = 0; i < ping_options->num_pings; i++) {
        int num = rand() % 100;
        enqueue(ping_options->queue, num);
    }

    if (pthread_kill(receiver, SIGTERM) != 0) {
        printf("Error sending signal to receiver thread\n");
        exit(1);
    }

    pthread_exit(NULL);
}

void *consumer(void *options)
{
    struct PingOptions *ping_options = (struct PingOptions*) options;

    int count = 0;
    int sum = 0;
    int max = 0;
    int min = 100;

    while (!doneFlag || !isEmpty(ping_options->queue)) {
        if (!isEmpty(ping_options->queue)) {
            int num = dequeue(ping_options->queue);

            count++;
            sum += num;
            if (num > max) {
                max = num;
            }
            if (num < min) {
                min = num;
            }
        }
    }

    printf("Count: %d\n", count);
    printf("Sum: %d\n", sum);
    printf("Max: %d\n", max);
    printf("Min: %d\n", min);
    printf("Average: %f\n\n", (float)sum / count);

    pthread_exit(NULL);
}

void handler(int signal_number) {
    doneFlag = 1;
}
