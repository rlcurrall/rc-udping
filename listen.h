#ifndef LISTEN_H
#define LISTEN_H

typedef struct ListenOptions
{
    int port;
} ListenOptions;

int parse_listen_options(int argc, char *argv[], ListenOptions *options);
int start_listen(ListenOptions *options);

#endif /* LISTEN_H */
