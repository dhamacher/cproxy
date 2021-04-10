/**
* MIT License
*
* Copyright (c) 2021 Daniel Hamacher
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "cproxy.h"
#include <ctype.h>
#include <time.h>
#if defined(_WIN32)
#include <conio.h>
#endif


/**
 * Function to send UDP packets.
 * @param address The address or ip to send the packets to.
 * @param port The port to send the packet to.
 * @param message The message toi include as the pyaload in this packet.
 * @param batch_size The number of messages to send for this batch.
 * @return Return 0 on success and 1 for errors.
 */
int send_udp_packet(char *address, char *port, char *message, int batch_size) {
#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) 
    {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
#endif
    printf("Configuring remote address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    struct addrinfo *peer_address;
    if (getaddrinfo(address, port, &hints, &peer_address))
    {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    printf("Remote address is: ");
    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
    address_buffer, sizeof(address_buffer),
    service_buffer, sizeof(service_buffer),
    NI_NUMERICHOST | NI_NUMERICSERV);
    printf("%s %s\n", address_buffer, service_buffer);
    printf("Creating socket...\n");
    SOCKET socket_peer;
    socket_peer = socket(peer_address->ai_family,
    peer_address->ai_socktype, peer_address->ai_protocol);

    if (!ISVALIDSOCKET(socket_peer)) 
    {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    int batch = batch_size;
    while (batch >= 0)
    {
        int bytes_sent = sendto(socket_peer, message, strlen(message), 0, peer_address->ai_addr, peer_address->ai_addrlen);
        printf("Batch: %d\tSent %d bytes.\n", batch, bytes_sent);
        batch--;
    }
    freeaddrinfo(peer_address);
    CLOSESOCKET(socket_peer);
#if defined(_WIN32)
    WSACleanup();
#endif
    printf("Finished.\n");
    return 0;
}


/**
 * Function to start an UDP server at the specified address and port.
 * @param address The address to bind the server to.
 * @param port The port to listen for UDP connection.
 * @return Returns 1 if an error occurs.
 */
int start_udp_server(char *address, char *port)
{
#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d))
    {
        fprintf(stderr, "Failed to initailize.\n");
        return 1;
    }
#endif
    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(address, port, &hints, &bind_address);
    printf("Creating socket...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
                         bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen))
    {
        fprintf(stderr, "sock() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    printf("Binding socket to local address...\n");
    if(bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
    {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_address);

    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    SOCKET max_socket = socket_listen;

    printf("Waiting for connections...\n");

    while(1)
    {
        /* Read for incoming UDP packets. */
        fd_set reads;
        reads = master;
        if(select(max_socket+1, &reads, 0,0,0) < 0)
        {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        if (FD_ISSET(socket_listen, &reads))
        {
            /* Receive UDP packets. */
            struct sockaddr_storage client_address;
            socklen_t client_len = sizeof(client_address);

            char read[1024];
            int bytes_received = recvfrom(socket_listen, read, 1024, 0,(struct sockaddr*)&client_address, &client_len);
            if (bytes_received < 1)
            {
                fprintf(stderr, "Connection closed. (%d)\n", GETSOCKETERRNO());
                return 1;
            }

            /* Get current date and time and print the received packet content. */
            time_t now;
            struct tm *timeinfo;
            time(&now);
            timeinfo = localtime(&now);
            printf("UDP Packet Received at %s:\n%s\n", asctime(timeinfo), read);

            /* Use if you want to send a response. */
            sendto(socket_listen, "Packet received...\n", bytes_received, 0, (struct sockaddr*)&client_address, client_len);
        } //if FD_ISSET
    }//while(1)

    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);

#if defined(_WIN32)
    WSACleanup();
#endif

    printf("Finished.\n");
    return 0;
}


/**
 * Function used to start a proxy server that listens for UDP packets and redirects them another server using TCP.
 * @param address The local address to bind the server to.
 * @param port The local port to bind the server to.
 * @param proxy_address The proxy address to redirect the packets.
 * @param proxy_port The proxy port to redirect the packects to.
 * @return Returns 1 on errors and 0 on success.
 */
int start_udp_tcp_proxy_server(char *address, char *port, char *proxy_address, char *proxy_port)
{
#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d))
    {
        fprintf(stderr, "Failed to initailize.\n");
        return 1;
    }
#endif
    /* Used for UDP server */
    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    /* Configure local address to listen to. */
    struct addrinfo *bind_address;
    getaddrinfo(address, port, &hints, &bind_address);
    printf("Creating socket...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
                           bind_address->ai_socktype, bind_address->ai_protocol);

    /* Used to proxy UDP over TCP */
    printf("Configuring remote address...\n");
    struct addrinfo proxy;
    memset(&proxy, 0, sizeof(proxy));
    proxy.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;

    if (getaddrinfo(proxy_address, proxy_port, &proxy, &peer_address)) {
        fprintf(stderr, "getaddrinfo() failed with address: %s and port: %s. (%d)\n", proxy_address, proxy_port,
                GETSOCKETERRNO());
        return 1;
    }

    /* Configure proxy address */
    printf("Proxy address is: ");
    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
                address_buffer, sizeof(address_buffer),
                service_buffer, sizeof(service_buffer),
                NI_NUMERICHOST);
    printf("%s %s\n", address_buffer, service_buffer);

    printf("Creating socket...\n");
    SOCKET socket_peer;
    socket_peer = socket(peer_address->ai_family,
                         peer_address->ai_socktype, peer_address->ai_protocol);

    freeaddrinfo(peer_address);

    printf("Proxy connected.\n");

    /* Check if the sockets are valid */
    if (!ISVALIDSOCKET(socket_peer)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    if (!ISVALIDSOCKET(socket_listen))
    {
        fprintf(stderr, "sock() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    /* Bind UDP listening address */
    printf("Binding socket to local address...\n");
    if(bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
    {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_address);

    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    SOCKET max_socket = socket_listen;

    printf("Waiting for connections...\n");

    while(1)
    {
        /* Read for incoming UDP packets. */
        fd_set reads;
        reads = master;
        if(select(max_socket+1, &reads, 0,0,0) < 0)
        {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        if (FD_ISSET(socket_listen, &reads))
        {
            /* Receive UDP packets. */
            struct sockaddr_storage client_address;
            socklen_t client_len = sizeof(client_address);

            char read[1024];
            int bytes_received = recvfrom(socket_listen, read, 1024, 0,(struct sockaddr*)&client_address, &client_len);
            if (bytes_received < 1)
            {
                fprintf(stderr, "Connection closed. (%d)\n", GETSOCKETERRNO());
                return 1;
            }

            /* Get current date and time and print the received packet content. */
            time_t now;
            struct tm *timeinfo;
            time(&now);
            timeinfo = localtime(&now);
            printf("UDP Packet Received at %s", asctime(timeinfo));


            /* Connect to proxy */
            socket_peer = socket(peer_address->ai_family,
                                 peer_address->ai_socktype, peer_address->ai_protocol);

            printf("Connecting to proxy...\n");
            if (connect(socket_peer,
                        peer_address->ai_addr, peer_address->ai_addrlen)) {
                fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
                return 1;
            }

            /* Proxy the packet over TCP */
            int bytes_sent;
            time(&now);
            bytes_sent = send(socket_peer, read, strlen(read), 0);
            timeinfo = localtime(&now);
            printf("Send %d bytes over TCP at %s", bytes_sent, asctime(timeinfo));

            /* Close socket */
            CLOSESOCKET(socket_peer);


        } //if FD_ISSET
    }//while(1)

    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);

#if defined(_WIN32)
    WSACleanup();
#endif

    printf("Finished.\n");
    return 0;
}
