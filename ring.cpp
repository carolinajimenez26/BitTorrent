#include <iostream>
#include <random>
#include <sstream>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;

#define dbg(x) cout << #x << ": " << x << endl

const int range_from = 0, range_to = 30;

int getRandom() {
	random_device rand_dev;
	mt19937 generator(rand_dev());
	uniform_int_distribution<int> distr(range_from, range_to);

	return distr(generator);
}

string toString(int n) {
  stringstream ss;
  ss << n;
  string out;
  ss >> out;
  return out;
}

int toInt(string s) {
  stringstream ss;
  ss << s;
  int out;
  ss >> out;
  return out;
}

int main(int argc, char** argv) {

	if(argc != 3){
		cout << "Missing arguments" << endl;
		return 1;
	}
	string a(argv[1]);
	string b(argv[2]);
	string server_port = "tcp://*:555" + a;
	string client_port = "tcp://localhost:555" + b;

  context ctx;
	socket s_server(ctx, socket_type::rep); //Listening
	socket s_client(ctx, socket_type::req); //Asking

  s_server.bind(server_port);
  s_client.connect(client_port);
  cout << "Server listening on " << server_port << endl;
  cout << "Client connected on " << client_port << endl;

  poller pol;
  pol.add(s_server);
  pol.add(s_client);

  int myId = getRandom(), predecessorId;
	dbg(myId);

	int i = 0;

	string ipSucessor, ipPredecessor, currentNode, nextNode; //ip's

	message m;
	m << "What's your ID?";
	s_client.send(m);

  while (true) {
    if (pol.poll()) {
      if (pol.has_input(s_client)) {
				message m, n;
				string ans, id;
        s_client.receive(m);
        m >> ans;
				cout << "Receiving from server -> " << ans << endl;

				if (ans == "My ID is") {
					m >> id;
					cout << " " << id << endl;
				}

				n << "What's your ID?";
				s_client.send(n);
      }
      if (pol.has_input(s_server)) {
				string ans;
				message m, n;
        s_server.receive(m);
				m >> ans;
				cout << "Receiving from client -> " << ans << endl;

				if (ans == "What's your ID?") {
					n << "My ID is" << toString(myId);
					s_server.send(n);
				} else {
					n << "Error";
					s_server.send(n);
				}

      }
    }
		i++;
		if (i == 6) break;
  }

	return 0;
}
