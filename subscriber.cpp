#include "lib/zhelpers.hpp"
#include <iostream>
#include <zmqpp/zmqpp.hpp>
#include <vector>

using namespace std;
using namespace zmqpp;

void askInformation(vector<string> &ips, socket &s_client){

    cout << "-----------------------------------------------" << endl;

    message m, n;
    string ip, ans;
    for (int i = 0; i < ips.size(); i++){
        ip = ips[i];
        s_client.connect(ip);
        m << "send me your information";
        s_client.send(m);
        s_client.receive(n);
        n >> ans;
        cout << ans << endl;
        s_client.disconnect(ip);
    }
    cout << "-----------------------------------------------" << endl;
}

int main () {
    context ctx;
    socket s_server(ctx, socket_type::rep); //Listening
    socket s_client(ctx, socket_type::req); //Asking
    s_server.bind("tcp://*:5563");
    vector<string> ips;
    poller pol;
    pol.add(s_server);
    message m;

    while(true){
        if (pol.poll()) {
            if (pol.has_input(s_server)){        
                s_server.receive(m);
                string text;
                m >> text;
                ips.push_back(text);
                cout << "Entered " << text << endl;
                m << "";
                s_server.send(m);
                askInformation(ips, s_client);
            }
        }
    }

    return 0;
}
