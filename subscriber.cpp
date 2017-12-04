#include <iostream>
#include <zmqpp/zmqpp.hpp>
#include <vector>
#include <unordered_map>
#include <sstream>

using namespace std;
using namespace zmqpp;

#define dbg(x) cout << #x << ": " << x << endl

vector<string> split(string s, char tok) { // split a string by a token especified
  istringstream ss(s);
  string token;
  vector<string> v;

  while(getline(ss, token, tok)) {
    v.push_back(token);
  }

  return v;
}

void askInformation(vector<string> &ips, socket &s_client, socket &s_publisher){

    cout << "-----------------------------------------------" << endl;

    message m, n;
    string ip, ans, msg;
    for (int i = 0; i < ips.size(); i++){
        ip = ips[i];
        s_client.connect(ip);
        m << "send me your information";
        s_client.send(m);
        s_client.receive(n);
        n >> ans;
        msg += ans;
        msg += "\n";
        cout << ans << endl;
        s_client.disconnect(ip);
    }
    cout << "-----------------------------------------------" << endl;

    message subs_m;
    subs_m << msg;
    s_publisher.send(subs_m);
    cout << "Sending to publisher " << endl;
}

int main () {
    context ctx;
    socket s_server(ctx, socket_type::rep); //Listening
    socket s_client(ctx, socket_type::req); //Asking
    socket s_publisher(ctx, socket_type::pub);

    string endPoint = "tcp://*:5563";
    string publisherEndPoint = "tcp://*:5564";

    s_server.bind(endPoint);
    cout << "Server Listening on " << endPoint << endl;
    s_publisher.bind(publisherEndPoint);
    cout << "Publisher endPoint " << publisherEndPoint << endl;

    vector<string> ips;
    poller pol;
    pol.add(s_server);
    message m;
    vector<string> splitted;

    while(true){
        if (pol.poll()) {
            if (pol.has_input(s_server)){
                s_server.receive(m);
                string text;
                m >> text;
                cout << "TEXT: " << text << endl;
                splitted = split(text, ':');
                dbg(splitted[0]);
                if (splitted[0] == "showFingerTable") {
                  s_publisher.send(m);
                  cout << "Sending to publisher: showFingerTable" << endl;
                }
                if (splitted[0] == "out"){
                    string ip_aux = splitted[1] + ":" + splitted[2] + ":" + splitted[3];
                    cout << "out " << ip_aux << endl;
                    for(int i = 0; i < ips.size(); i++) {
                        if (ips[i] == ip_aux) {
                            cout << "erase: " << ips[i] << endl;
                            ips.erase(ips.begin() + i);
                        }
                    }
                    m << "";
                    s_server.send(m);
                    askInformation(ips, s_client, s_publisher);
                } else {
                    ips.push_back(text);
                    cout << "Entered " << text << endl;
                    m << "";
                    s_server.send(m);
                    askInformation(ips, s_client, s_publisher);
                }
            }
        }
    }

    return 0;
}
