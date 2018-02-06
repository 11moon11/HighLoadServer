#include "networking.h"

Networking::Networking(Mode m) {
    mode = m;

    if(m == server_mode) {
        ser = new server(1234);
        ser->start();
        ser->add_recieve_pool();
    } else {
        cli = new client("127.0.0.1", 1234);
    }
}

Networking::~Networking() {
    if(ser != NULL) {
        ser->stop();
    }

    delete ser;
    delete cli;
}