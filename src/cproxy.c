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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Include threading libraries */
#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread>
#endif


// Sample custom data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
typedef struct payload {
    int bytes;
    char *msg;
} DATA, *PAYLOAD;


/*
 * Main function
 */
int main()
{
    const char* s = getenv("MAKE_HOME");
    SOCKET consumer = get_udp_listener_socket("127.0.0.1", "53");
    SOCKET producer = connect_to_target_host("127.0.0.1", "80");
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);

    char reads[MAX_PACKET_SIZE];

    while(TRUE)
    {
        int bytes_received = recvfrom(consumer, reads, sizeof(reads),0,(struct sockaddr*) &client_address, &client_len);
#if defined(_WIN32)
        DWORD   thread_id;
        HANDLE  thread;
        PAYLOAD data;
        data = (PAYLOAD) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(DATA));
        data->bytes = bytes_received;
        data->msg = &reads;

        thread = CreateThread(
                NULL,                   // default security attributes
                0,                      // use default stack size
                win32_send_to_eventhub,       // thread function name
                data,          // argument to thread function
                0,                      // use default creation flags
                &thread_id);   // returns the thread identifier

        printf("Thread created with ID: %d\n", thread_id);
#else

//    add threading code for POSX
#endif
//        printf("Received (%d bytes): %.*s\n", data->bytes, data->bytes, data->data);
    }
}


/*!
 *
 * @param lpdata
 * @return
 */
DWORD WINAPI win32_send_to_eventhub(LPVOID lpdata)
{
    PAYLOAD data;
    data= (PAYLOAD)lpdata;
    printf("Received (%d bytes): %.*s\n", data->bytes, data->bytes, data->msg);
    return 0;
}

///**
// * Main function to start the udp server.
//
// * @return Return 0 on completion.
// */
//int main(int argc, char *argv[])
//{
//    char *port;
//    char *address;
//    int proxy = 1;
//    char *proxy_address;
//    char *proxy_port;
//
//    address = "127.0.0.1";
//    port = "53";
//    proxy_address = "127.0.0.1";
//    proxy_port = "53914";



//    printf("Starting UPD server. IS_PROXY %d\t Address: %s\t port: %s\n", proxy, address, port);
//
//    int status;
//    if(proxy == 1)
//        status = start_udp_tcp_proxy_server(address, port, proxy_address, proxy_port);
//    else
//        status = start_udp_server(address, port);
//
//    if(status == 1)
//    {
//        fprintf(stderr, "start_udp_server() failed.\n");
//        return 1;
//    }
//    return 0;
//}