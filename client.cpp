#include "client.h"

client::client(const char *hostname, int sock_port) : endpoint(sock_port) {
    server = gethostbyname(hostname);

    if(server == NULL) {
        cout << "No such host" << endl;
        return;
    }

    // Setup address and port for the server
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(this->port);

    if(!connect()) {
        cout << "Failed to connect to the server" << endl;
        return;
    }

    if(!recieve(&client_id, sizeof(client_id))) {
        cout << "Failed to obtain client id" << endl;
        return;
    }

    /* Test */
    for(int i=0; i<5; i++) {
    char buff[1];
    if(!recieve(buff, sizeof(buff))) {
        cout << "Failed to obtain letter" << endl;
        return;
    } else {
        cout << "client # " << client_id << ": " << buff << endl;
    }
    }
    /* End of Test */

    cout << "Issued client id: " << client_id << endl;
    this->status = true;
}

client::~client() {
}

bool client::send(void *package, int size) {
    return endpoint::_send(socket_fd, package, size);
    
}

bool client::recieve(void *package, int size) {
    return endpoint::_recieve(socket_fd, package, size);
}

bool client::connect() {
    return endpoint::_connect(socket_fd, server_addr);
}