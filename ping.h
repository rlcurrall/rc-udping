#include "queue.h"

#ifndef PING_H
#define PING_H

typedef struct PingOptions
{
    char *remote_host;
    int remote_port;
    int delay;
    int num_pings;
    Queue *queue;
} PingOptions;

int start_ping(PingOptions *options);
int parse_ping_options(int argc, char *argv[], PingOptions *options);

#endif /* PING_H */