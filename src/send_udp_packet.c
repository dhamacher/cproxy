//
// Created by dahamacher on 4/2/2021.
//
#include "cproxy.h"

int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        printf("Usage: udp_send <ip> <port> <string or file as payload>");
    }

    int batch_size;
    char *port;
    char *ip;
    char *message;

    ip = argv[1];
    port = argv[2];
    message = argv[3];
    batch_size = atoi(argv[4]);

    for (int i = 0; i <= batch_size; i++)
    {
        send_udp_packet(ip, port,message, batch_size);
    }
}
