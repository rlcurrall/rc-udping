#include <arpa/inet.h>
#include <getopt.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "listen.h"

#define BUFFER_SIZE 1024

int parse_listen_options(int argc, char *argv[], ListenOptions* options)
{
    int opt;

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                options->port = atoi(optarg);
                break;
            default:
                return 1;
        }
    }

    return 0;
}

int start_listen(ListenOptions* options) {
    return 0;
}