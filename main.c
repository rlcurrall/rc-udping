#include <stdio.h>
#include <string.h>

#include "lib/queue.h"
#include "lib/ping.h"
#include "lib/listen.h"

void print_usage();

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "ping") == 0)
    {
        Queue queue;
        initQueue(&queue);
        struct PingOptions ping_options = {
            .remote_host = "127.0.0.1",
            .remote_port = 5000,
            .delay = 0,
            .num_pings = 10,
            .queue = &queue};
        if (parse_ping_options(argc, argv, &ping_options) != 0)
        {
            return 1;
        }

        return start_ping(&ping_options);
    }
    else if (strcmp(argv[1], "listen") == 0)
    {
        struct ListenOptions listen_options = {.port = 5000};
        if (parse_listen_options(argc, argv, &listen_options) != 0)
        {
            return 1;
        }

        return start_listen(&listen_options);
    }
    else
    {
        print_usage();
        return 1;
    }

    return 0;
}

void print_usage()
{
    printf("Usage: program_name <ping|listen> [options]\n");
    printf("Options:\n");
    printf("  ping:\n");
    printf("    -r <remote host>   remote hostname or IP address\n");
    printf("    -p <remote port>   remote port\n");
    printf("    -d <delay>         delay between pings in seconds\n");
    printf("    -n <num pings>     number of pings to send\n");
    printf("  listen:\n");
    printf("    -p <port>          port to listen on\n");
}
