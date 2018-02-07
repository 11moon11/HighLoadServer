#include "networking.h"

Networking::Networking(Mode m) {
    mode = m;

    if(m == server_mode) {
        ser = new server(1234);
        ser->start();
    } else {
        cli = new client("127.0.0.1", 1234);
    }
}

Networking::~Networking() {
    if(ser != nullptr) {
        delete ser;
    }

    if(cli != nullptr) {
        delete cli;
    }
}

void Networking::add_recieve_pool() {
    if(ser != nullptr) {
        ser->add_recieve_pool();
    }
}