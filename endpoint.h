#ifndef __endpoint__
#define __endpoint__

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <strings.h>    // Required for bzero()
#include <string.h>
#include <iostream>
#include <unistd.h>     // Required for read() / write()
#include <errno.h>      // Required for error handling
#include <mutex>

#include "data.h"

using namespace std;

enum recieve_code { error, transfering, complete };

class endpoint {
    protected:
        int port, client_len, socket_fd, newsocket_fd;
        struct sockaddr_in server_addr, client_addr;
        int hello_size = 6;
        mutex *mmutex;
        bool status;

    public:
        endpoint(int port);
        ~endpoint();

        bool isReady();

        static bool _send(int to_socket, void *package, int size);

        static bool _recieve(int from_socket, void *package, int size);

        static int __recieve(int from_socket, void *package, int size);

        static bool _connect(int new_socket, struct sockaddr_in new_addr);
};

#endif