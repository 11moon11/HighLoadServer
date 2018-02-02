#include <iostream>
#include "networking.h"

using namespace std;

int main() {
    Networking *s = new Networking(server_mode);
    usleep(1000000);
    Networking *c = new Networking(client_mode);

    int num = -1;
    do {
        cout << "0) exit" << endl;
        cout << "Enter a number: ";
        cin >> num;

    } while(num != 0);

    delete c;
    delete s;
}