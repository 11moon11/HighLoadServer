#include "endpoint.h"

endpoint::endpoint(int sock_port) {
    status = false;
    port = sock_port;
    client_len = sizeof(client_addr);
    socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if(socket_fd < 0) {
        cout << "Coud not initialize socket :(" << endl;
        return;
    }

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
}

endpoint::~endpoint() {
    close(socket_fd);
    close(newsocket_fd);
}

bool endpoint::isReady() {
    return status;
}

bool endpoint::_send(int to_socket, void *package) {
    while(write(to_socket, package, sizeof(package)) < 0) {
        if(errno == EINPROGRESS) { // Operation in progress
            usleep(100000);
            continue;
        }

        // If execution reaches here - something is wrong
        cout << "Error writing to server" << endl;
        cout << strerror(errno) << endl;
        return false;
    }

    return true;
}

bool endpoint::_recieve(int from_socket, void *package, int size) {
    while (read(from_socket, package, size) < 0) {
        if(errno == EINPROGRESS) { // Operation in progress
            usleep(100000);
            continue;
        }

        // If execution reaches here - something is wrong
        cout << "Error recieving data from server" << endl;
        cout << strerror(errno) << endl;
        return false;
    }

    return true;
}

int endpoint::__recieve(int from_socket, void *package, int size) {
    if(read(from_socket, package, size) < 0) {
        if(errno != EINPROGRESS) // Some error happened
            return error;

        return transfering;
    }

    return complete;
}

bool endpoint::_connect(int new_socket, struct sockaddr_in new_addr) {
    while (connect(new_socket, (struct sockaddr *)&new_addr, sizeof(new_addr)) < 0) {
        if(errno == EINPROGRESS) { // Operation in progress
            usleep(100000);
            continue;
        }

        cout << "Client failed to connect to host" << endl;
        cout << strerror(errno) << endl;
        return false;
    }

    return true;
}