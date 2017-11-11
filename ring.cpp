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

	if(argc != 5){
		cout << "Usage: \"<local ip>\" \"<local port>\" \"<remote ip>\" \"<remote port>\"" << endl;
		return 1;
	}

	string myIp(argv[1]), myPort(argv[2]), ipSucessor(argv[3]), portSucessor(argv[4]);
	string ipPrevious = "", portPrevious = "";

	string tcp = "tcp://";
	string server_endPoint = tcp + myIp + ":" + myPort; // e.g: "tcp://*:5555";
	string client_endPoint = tcp + ipSucessor + ":" + portSucessor; // e.g: "tcp://localhost:5555";
	string previous_endPoint = tcp + ipPrevious + ":" + portPrevious;

  context ctx;
	socket s_server(ctx, socket_type::rep); //Listening
	socket s_client(ctx, socket_type::req); //Asking

  s_server.bind(server_endPoint);
	cout << "Server listening on " << server_endPoint << endl;
  s_client.connect(client_endPoint);
	cout << "Client connected on " << client_endPoint << endl;

  poller pol;
  pol.add(s_server);
  pol.add(s_client);

  int myId = getRandom(), sucessorId, predecessorId;
	string ipLookingFor = "";
	dbg(myId);

	int i = 0;

	bool id_flag = false;

	message m;
	m << "What's your ID?";
	s_client.send(m);

  while (true) {
		dbg(i);
    if (pol.poll()) {

			if (pol.has_input(s_client)) {

				message m, n;
				string ans, server_predecessor_id, server_id;
        s_client.receive(m);
        m >> ans;
				cout << "Receiving from server -> " << ans << endl;

				if (ans == "My ID is") {
					m >> server_id >> server_predecessor_id;
					dbg(server_id);
					dbg(server_predecessor_id);

					if (myId < toInt(server_id) and myId > toInt(server_predecessor_id)) {
						// connect between server_predecessor_id and server_id
						predecessorId = toInt(server_predecessor_id);
						sucessorId = toInt(server_id);
						n << "Now I am your predecessor" << toString(myId);
						s_client.send(n);
						previous_endPoint = tcp + ipPrevious + ":" + portPrevious;
						s_client.connect(previous_endPoint);
						message l;
						l << "Now I am your sucessor" << myIp << myPort;
						s_client.send(l);
						s_client.connect(client_endPoint);

					} else { // keep going through the ring
						n << "What's your sucessor IP and PORT";
						s_client.send(n);
						id_flag = true;
					}
				}
				if (ans == "My sucessor IP and PORT is") {
					ipPrevious = ipSucessor;
					portPrevious = portSucessor;
					m >> ipSucessor;
					m >> portSucessor;
					cout << ipSucessor << ":" << portSucessor << endl;
					// connect
					client_endPoint = tcp + ipSucessor + ":" + portSucessor;
					s_client.connect(client_endPoint);
					id_flag = false;
				}

				if (!id_flag) {
					n << "What's your ID?";
					s_client.send(n);
				}
      }

      if (pol.has_input(s_server)) {

				string ans;
				message m, n;
        s_server.receive(m);
				m >> ans;
				cout << "Receiving from client -> " << ans << endl;

				if (ans == "What's your ID?") {
					n << "My ID is" << toString(myId) << toString(predecessorId);
					s_server.send(n);
				}
				if (ans == "What's your sucessor IP and PORT") {
					n << "My sucessor IP and PORT is" << ipSucessor << portSucessor;
					s_server.send(n);
				}
				if (ans == "Now I am your predecessor") {
					string ans2;
					m >> ans2;
					predecessorId = toInt(ans2);
				}
				if (ans == "Now I am your sucessor") {
					m >> ipSucessor >> portSucessor;
					client_endPoint = tcp + ipSucessor + ":" + portSucessor;
					s_client.connect(client_endPoint);
				}
      }
    }
		i++;
		if (i == 20) break;
  }

	return 0;
}
