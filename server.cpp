#include "server.h"

server::server(int sock_port, int max_connections) : endpoint(sock_port) {
    pq_recieve = new vector<vector<client_info *> *>();
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(this->port);
    server_addr.sin_family = AF_INET;
    connections = 0;
    active = false;
    
    if(bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cout << "Coud not bind socket :(" << endl;
        return;
    }

    add_recieve_pool();
    listen(socket_fd, max_connections);
    this->status = true;
}

server::~server() {
    stop();

    for(unsigned int i=0; i<pq_recieve->size(); i++) {
        vector<client_info *> * tmp = pq_recieve->back();
        pq_recieve->pop_back();
        delete tmp;
    }
    delete pq_recieve;

    for(unsigned int i=0; i<clients.size(); i++) {
        close(clients[i]->client_fd);
        delete clients[i];
    }

    // Wait for all threads to calm down
    usleep(1000000);
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
            equalize_pq(This->pq_recieve);
        } while(new_fd == -1 && This->active);

        if(This->active == false) {
            break;
        }

        client_info *new_info = new client_info();
        new_info->c_len = c_len;
        new_info->client_fd = new_fd;
        new_info->client_addr = new_addr;

        int conn_num = This->clients.size() + 1;
        send(new_info, &conn_num, sizeof(int));

        /* Test */
        char buff[256];
        strcpy(buff, "Hello there!");
        cout << "server: " << buff << endl;
        send(new_info, (void *)buff, strlen(buff));
        /* End of Test */

        This->mmutex->lock();
        cout << "Client #" << conn_num << " connected!" << endl;
        This->clients.push_back(new_info);
        This->pq_recieve->back()->push_back(new_info);
        This->mmutex->unlock();
    }

    return nullptr;
}

void server::recieve_therad_pool(recieve_pool_struct *rev_info) {
    pthread_detach(pthread_self());

    if(*rev_info->act == 1 || *rev_info->act == 0) {
    } else {
        cout << "*rev_info->act = " << *rev_info->act << endl;
    }

    while(*rev_info->act) {
        rev_info->mtx->lock();
        unsigned int limit = rev_info->pool->size();
        rev_info->mtx->unlock();

        for(unsigned int i=0; i<limit; i++) {
            rev_info->mtx->lock();
            client_info *user = rev_info->pool->at(i);
            rev_info->mtx->unlock();
            if(user == nullptr)
                continue;
            
            int number;
            int ret = endpoint::__recieve(user->client_fd, &number, sizeof(number));
            if(ret == error) {
                rev_info->mtx->lock();
                cout << strerror(errno) << endl;
                rev_info->mtx->unlock();
                continue;
            } else if(ret == transfering) {
                // Start recieve thread

            }
        }

        usleep(100000);
    }
}

void server::add_recieve_pool() {
    vector<client_info *> *pool = new vector<client_info *>();
    pq_recieve->push_back(pool);

    recieve_pool_struct *rec_info = new recieve_pool_struct();
    rec_info->act = &active;
    rec_info->pool = pool;
    rec_info->mtx = mmutex;

    pthread_t recieve_thread;
    pthread_create(&recieve_thread, NULL, (THREADFUNCPTR)recieve_therad_pool, rec_info);
}

// TODO: optimize with priority queue
void server::equalize_pq(vector<vector<client_info *> *> *pq) {
    if(pq->size() < 2)
        return;

    int pos_min = -1;
    int pos_max = -1;
    int min = INT_MAX;
    int max = INT_MIN;

    for(int i=0; i<(int)pq->size(); i++) {
        int pool_size = pq->at(i)->size();

        if(pool_size < min) {
            min = pool_size;
            pos_min = i;
        }
        if(pool_size > max) {
            max = pool_size;
            pos_max = i;
        }
    }

    if(pos_min != -1 && pos_max != -1 && pos_min != pos_max) {
        if((max - min) > 1) {
            cout << "Moving 1 client from pool #" << pos_max << " to pool #" << pos_min << endl;
            pq->at(pos_min)->push_back(pq->at(pos_max)->back());
            pq->at(pos_max)->pop_back();
        }
    }
}

void server::send(client_info *client_id, void *packet, int size) {
    send_struct *send_info = new send_struct();
    send_info->recipient = client_id;
    send_info->packet = packet;
    send_info->size = size;
    pthread_t send_thread;

    pthread_create(&send_thread, NULL, (THREADFUNCPTR)__send, send_info);
}

void server::broadcast(void *packet, int size) {
    for(client_info *ci : clients) {
        send(ci, packet, size);
    }
}

void *server::__send(send_struct *send_s) {
    pthread_detach(pthread_self());

    endpoint::_send(send_s->recipient->client_fd, send_s->packet, send_s->size);

    return nullptr;
}