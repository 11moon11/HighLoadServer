#include "networking.h"

Networking::Networking(Mode m) {
    mode = m;

    if(m == server_mode) {
        ser = new server(1234);
        ser->start();

        /*
        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);
        if (n < 0) cout << "Error reading from socket :(" << endl;
        else cout << "Message: " << buffer << endl;

        n = write(newsockfd, "I got your message", 18);
        if (n < 0) cout << "Error writing to socket" << endl;
        */
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