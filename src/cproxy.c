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
int main()
{
    printf("test\n");
    const char* s = getenv("PATH");
    printf("PATH :%s\n",(s!=NULL)? s : "getenv returned NULL");
    printf("end test\n");
}

/**
 * Main function to start the udp server.
 * @return Return 0 on completion.
 */
int main(int argc, char *argv[])
{
    char *port;
    char *address;
    int proxy = 1;
    char *proxy_address;
    char *proxy_port;

    address = "127.0.0.1";
    port = "53";
    proxy_address = "127.0.0.1";
    proxy_port = "53914";



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
}