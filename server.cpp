#include "server.h"

server::server(int sock_port, int max_connections) : endpoint(sock_port) {
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(this->port);
    server_addr.sin_family = AF_INET;
    connections = 0;
    active = false;
    
    if(bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cout << "Coud not bind socket :(" << endl;
        return;
    }

    listen(socket_fd, max_connections);
    this->status = true;
}

server::~server() {
    for(unsigned int i=0; i<clients.size(); i++) {
        close(clients[i]->client_fd);
        delete clients[i];
    }
}

void server::start() {
    if(status) {
        active = true;
        pthread_create(&listen_thread, NULL, (THREADFUNCPTR)connection_listen, this);
    }
}

void server::stop() {
    if(status) {
        active = false;
        pthread_join(listen_thread, NULL);
    }
}

/**
 *  Thread pool implementation for 'connection accept'
*/
void *server::connection_listen(server *This) {
    while(This->active) {
        struct sockaddr_in new_addr;
        int new_fd = -1;
        int c_len;

        do { // So we don't waste resources while no one is trying to connect
            usleep(100000); // Sleep for 100 ms
            new_fd = accept(This->socket_fd, (struct sockaddr *)&new_addr, (socklen_t *)&c_len);
        } while(new_fd == -1 && This->active);

        if(This->active == false) {
            break;
        }

        client_info *new_info = new client_info();
        new_info->c_len = c_len;
        new_info->client_fd = new_fd;
        new_info->client_addr = new_addr;

        int conn_num = This->clients.size() + 1;
        send(new_info, &conn_num);

        This->mmutex.lock();
        cout << "Client #" << conn_num << " connected!" << endl;
        This->clients.push_back(new_info);
        This->mmutex.unlock();
    }

    return NULL;
}

void server::recieve_therad_pool(vector<client_info *> *pool, mutex *mtx, bool *act) {
    while(*act) {
        mtx->lock();
        unsigned int limit = pool->size();
        mtx->unlock();

        for(unsigned int i=0; i<limit; i++) {
            mtx->lock();
            client_info *user = pool->at(i);
            mtx->unlock();
            if(user == nullptr)
                continue;
            
            int number;
            int ret = endpoint::__recieve(user->client_fd, &number, sizeof(number));
            if(ret == error) {
                mtx->lock();
                cout << strerror(errno) << endl;
                mtx->unlock();
                continue;
            } else if(ret == transfering) {
                // Start recieve thread

            }
        }
    }
}

void server::add_recieve_pool() {
    vector<client_info *> *pool = new vector<client_info *>();
}

void server::equalize_pq(vector<vector<client_info *> *> pq) {
    if(pq.size() == 1)
        return;

    int pos_min = -1;
    int pos_max = -1;
    unsigned int min = INT_MAX;
    unsigned int max = INT_MIN;

    for(unsigned int i=0; i<pq[i]->size(); i++) {
        if(pq[i]->size() < min) {
            min = pq[i]->size();
            pos_min = i;
        } else if(pq[i]->size() > max) {
            max = pq[i]->size();
            pos_max = i;
        }
    }

    if((max - min) > 1) {
        pq[pos_min]->push_back(pq[pos_max]->back());
        pq[pos_max]->pop_back();
    }
}

void server::send(client_info *client_id, void *packet) {
    send_struct *send_info = new send_struct();
    send_info->recipient = client_id;
    send_info->packet = packet;
    pthread_t send_thread;

    pthread_create(&send_thread, NULL, (THREADFUNCPTR)__send, send_info);
}

void *server::__send(send_struct *send_s) {
    pthread_detach(pthread_self());

    endpoint::_send(send_s->recipient->client_fd, send_s->packet);

    return NULL;
}