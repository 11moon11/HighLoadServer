#ifndef __server__
#define __server__

#include <pthread.h>
#include <vector>

#include "endpoint.h"

typedef void * (*THREADFUNCPTR)(void *);

struct client_info {
    int c_len;
    int client_fd;
    struct sockaddr_in client_addr;
};

struct send_struct {
    void *packet;
    client_info *recipient;
};

class server : public endpoint {
    protected:
        vector<client_info *> clients;
        pthread_t listen_thread;
        int connections;
        bool active;

    private:
        static void *connection_listen(server *ptrThis);

        /**
         * @param send_instuctions - recipient's information
         * Thread ready implementation of send
        */
        static void *__send(send_struct *send_s);

        /**
         * @param recieve_s - sender's information
         * Thread ready implementation of recieve
        */
        static void *__recieve(send_struct *recieve_s);

        /**
         * @param pool - pool of users that will be handeled in this thread
         * Loops through the pool and checks weather there is a message from this user, if there is -
         *      will create another thread that will recieve the message and pass it to the handler function
        */
        static void recieve_therad_pool(vector<client_info *> *pool, mutex *mtx, bool *active);

    public:
        server(int sock_port, int max_connections = 5);
        ~server();

        void start();
        void stop();

        static void send(client_info *client_id, void *packet);
        static void recieve(client_info *client_id, void *packet);

        /**
         * Adds pool to the priorty queue that will equalize number of users per pool
        */
        void add_recieve_pool();
};

#endif