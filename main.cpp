#include <iostream>
#include "networking.h"

using namespace std;

int main() {
    Networking *s = new Networking(server_mode);
    usleep(1000000);
    client *c1 = new client("127.0.0.1", 1234);
    client *c2 = new client("127.0.0.1", 1234);
    client *c3 = new client("127.0.0.1", 1234);

    //s->send(nullptr, 1);
    //c->recieve(nullptr);

    int num = -1;
    do {
        cout << endl;
        cout << "0) exit" << endl;
        cout << "1) add recieve pool" << endl;
        cout << "Enter a number: ";
        cin >> num;

        if(num == 1) {
            s->add_recieve_pool();
        }

    } while(num != 0);

    delete c1;
    delete c2;
    delete c3;
    delete s;
}