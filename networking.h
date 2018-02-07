#ifndef __networking__
#define __networking__

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <strings.h>    // Required for bzero()
#include <iostream>
#include <unistd.h>     // Required for read() / write()

#include "server.h"
#include "client.h"

using namespace std;

enum Mode { server_mode, client_mode };

class Networking {
    protected:
        server *ser;
        client *cli;
        
        Mode mode;

    public:
        Networking(Mode m);
        ~Networking();

        void send(void *packet, int size);
        void recieve(void *packet);
        void add_recieve_pool();
};

#endif