#include <arpa/inet.h>
#include <getopt.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "listen.h"

#define BUFFER_SIZE 1024

int parse_listen_options(int argc, char *argv[], ListenOptions *options)
{
    int opt;

    while ((opt = getopt(argc, argv, "p:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            options->port = atoi(optarg);
            break;
        default:
            return 1;
        }
    }

    return 0;
}

int start_listen(ListenOptions *options)
{
    // Create a UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket() failed");
        return 1;
    }

    // Bind the socket to the specified port
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(options->port);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind() failed");
        return 1;
    }

    printf("UDP ping server listening on port %d...\n", options->port);

    char buf[BUFFER_SIZE];
    struct sockaddr_in cli_addr;
    socklen_t cli_addr_len = sizeof(cli_addr);

    // Listen for incoming UDP packets
    for (;;)
    {
        // Wait for incoming UDP packet
        int n = recvfrom(sockfd, buf, BUFFER_SIZE, 0, (struct sockaddr *)&cli_addr, &cli_addr_len);
        if (n < 0)
        {
            perror("recvfrom() failed");
            continue;
        }

        // Check if the received packet is a ping request
        if (strcmp(buf, "PING") == 0)
        {
            printf("Received ping request from %s:%d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            // Generate a ping response packet and send it back to the client
            n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
            if (n < 0)
            {
                perror("sendto() failed");
                continue;
            }
        }
    }

    // Close the socket
    close(sockfd);

    return 0;
}