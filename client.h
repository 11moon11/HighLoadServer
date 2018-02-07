#ifndef __client__
#define __client__

#include <netdb.h>

#include "endpoint.h"

class client : public endpoint {
    protected:
        struct hostent *server;
        int client_id;

    public:
        client(const char *hostname, int sock_port);
        ~client();

        /**
         * @param package - pointer to data we are sending
         * 
         * @return Returns true if transfer of file was successful, false otherwise
        */
        bool send(void *package, int size);

        /**
         * @param package - pointer to where to write the data
         * @param size - how many bytes to write
         * 
         * @return Returns true if operation was successful, false otherwise
        */
        bool recieve(void *package, int size);

        /**
         * @return Returns true if connection to the server was successful, false otherwise
        */
        bool connect();
};

#endif