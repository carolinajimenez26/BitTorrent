#include <iostream>
#include <random>
#include <sstream>
#include <zmqpp/zmqpp.hpp>
#include "lib/zhelpers.hpp"
#include <thread>

using namespace std;
using namespace zmqpp;

#define dbg(x) cout << #x << ": " << x << endl

const int range_from = 0, range_to = 30;

string toSusbcriber = "";

void messageToSubscriber(string &text){
	context ctx;
	socket s_susbscriber(ctx, socket_type::req); //Asking
	s_susbscriber.connect("tcp://localhost:5563");
	while(true) {
		if (text != "") {
			message m;
			m << text;
			s_susbscriber.send(m);
			s_susbscriber.receive(m);
			text = "";
		}
	}
}

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

bool inTheRange(int left, int right, int i) {
	if ((i > left and i <= range_to) or (i >= range_from and i < right)) return true;
	return false;
}

void enterToTheRing(int &myId, int &predecessorId, int &sucessorId, string &client_endPoint, bool &flag, string &server_endPoint) {
	flag = true;
	if (predecessorId == -1) predecessorId = sucessorId;
	cout << "-------Entered to the ring!!-------" << endl;
	dbg(sucessorId);
	dbg(predecessorId);
	dbg(client_endPoint);
	cout << "---------------------------" << endl;

	toSusbcriber = server_endPoint;
}


int main(int argc, char** argv) {

	if(argc != 6){
		cout << "Usage: \"<local ip>\" \"<local port>\" \"<remote ip>\" \"<remote port>\"" << endl;
		return 1;
	}
	
	string myIp(argv[1]), myPort(argv[2]), ipSucessor(argv[3]), portSucessor(argv[4]);
	string ipPredecessor = ipSucessor, portPredecessor = portSucessor;

	string tcp = "tcp://";
	string server_endPoint = tcp + myIp + ":" + myPort; // e.g: "tcp://*:5555";
	string client_endPoint = tcp + ipSucessor + ":" + portSucessor; // e.g: "tcp://localhost:5555";
	string previous_endPoint = tcp + ipPredecessor + ":" + portPredecessor;

  context ctx;
	socket s_server(ctx, socket_type::rep); //Listening
	socket s_client(ctx, socket_type::req); //Asking

  s_server.bind(server_endPoint);
  myIp = "localhost";
  server_endPoint = tcp + myIp + ":" + myPort; 
	cout << "Server listening on " << server_endPoint << endl;
  s_client.connect(client_endPoint);
	cout << "Client connected to " << client_endPoint << endl;

  poller pol;
  pol.add(s_server);
  pol.add(s_client);

  int myId = toInt(argv[5]), sucessorId = -1, predecessorId = -1;

	dbg(myId);

  	thread t1(messageToSubscriber, ref(toSusbcriber));

	int i = 0;

	bool id_flag = false, enteredToRing = false;

	message m;
	m << "What's your ID?";
	s_client.send(m);

  while (true) {
		dbg(i);
	if (pol.poll()) {

			if (pol.has_input(s_client)) {

				message m, n;
				string ans, server_predecessor_id, s_sucessorId, s_ipSucessor, s_portSucessor,
							 server_predecessor_ip, server_predecessor_port, server_predecessor_endPoint;
		s_client.receive(m);
		m >> ans;
				cout << "Receiving from server -> " << ans << endl;

				if (!enteredToRing) {

					if (ans == "My ID is") {
						m >> s_sucessorId >> server_predecessor_id;
						sucessorId = toInt(s_sucessorId);
						dbg(sucessorId);
						dbg(server_predecessor_id);

						if (myId < sucessorId and myId > toInt(server_predecessor_id)) {
							// connect between predecessorId and server_id
							predecessorId = toInt(server_predecessor_id);
							n << "Now I am your predecessor" << toString(myId) << myIp << myPort;
							s_client.send(n);
							continue;
						} else {

							if ((toInt(server_predecessor_id) > sucessorId) and (toInt(server_predecessor_id) != myId)) { // in the end of the range
								cout << "Entraaa" << endl;
								cout << "In the range? " << inTheRange(toInt(server_predecessor_id),sucessorId, myId) << endl;

								if (inTheRange(toInt(server_predecessor_id), sucessorId, myId)) {
									// connect between predecessorId and server_id
									predecessorId = toInt(server_predecessor_id);
									n << "Now I am your predecessor" << toString(myId) << myIp << myPort;
									s_client.send(n);
									continue;
								}

							} else { // keep going through the ring
								cout << "Elseeee" << endl;
								n << "What's your sucessor IP and PORT";
								s_client.send(n);
								id_flag = true;

							}

						}
					}
					if (ans == "My sucessor IP and PORT is") {
						m >> s_ipSucessor;
						m >> s_portSucessor;
						if (s_portSucessor != myPort and s_portSucessor != portSucessor) {// if (s_ipSucessor != myIp) {
							ipPredecessor = ipSucessor;
							portPredecessor = portSucessor;
							ipSucessor = s_ipSucessor;
							portSucessor = s_portSucessor;
							cout << ipSucessor << ":" << portSucessor << endl;
							// connect
							s_client.disconnect(client_endPoint);
							client_endPoint = tcp + ipSucessor + ":" + portSucessor;
							s_client.connect(client_endPoint);
							id_flag = false;
						} else {
							enterToTheRing(myId ,predecessorId, sucessorId, client_endPoint, enteredToRing, server_endPoint);
							continue;
						}
					}
					if (ans == "Now you are my predecessor") {
						m >> server_predecessor_ip >> server_predecessor_port;
						server_predecessor_endPoint = tcp + server_predecessor_ip + ":" + server_predecessor_port;
						dbg(server_predecessor_endPoint);
						dbg(client_endPoint);
						if (server_predecessor_endPoint != client_endPoint and server_predecessor_port != portPredecessor) { // if (server_predecessor_endPoint != client_endPoint)
							s_client.disconnect(client_endPoint);
							s_client.connect(server_predecessor_endPoint);
						}
						message l;
						l << "Now I am your sucessor" << myIp << myPort << toString(myId);
						s_client.send(l);
						cout << "Sended!" << endl;
						if (server_predecessor_endPoint != client_endPoint and server_predecessor_port != portPredecessor) { // if (server_predecessor_endPoint != client_endPoint)
							s_client.disconnect(server_predecessor_endPoint);
							s_client.connect(client_endPoint);
						}
						enterToTheRing(myId ,predecessorId, sucessorId, client_endPoint, enteredToRing, server_endPoint);
						continue;
					}

					if (!id_flag) {
						n << "What's your ID?";
						s_client.send(n);
					}
				}

	  }

	  if (pol.has_input(s_server)) {

				string ans, c_ipSucessor, c_portSucessor, c_id;
				message m, n;
		s_server.receive(m);
				m >> ans;
				cout << "Receiving from client -> " << ans << endl;

				if (ans == "send me your information"){
					string information = "I am " + toString(myId) + "-> predecessorId: " + toString(predecessorId) + ", sucessorId: " + toString(sucessorId);
					n << information;
					s_server.send(n);
				}

				if (ans == "What's your ID?") {
					n << "My ID is" << toString(myId) << toString(predecessorId);
					s_server.send(n);
				}
				if (ans == "What's your sucessor IP and PORT") {
					n << "My sucessor IP and PORT is" << ipSucessor << portSucessor;
					s_server.send(n);
				}
				if (ans == "Now I am your predecessor") {
					string aux_IpPredecessor, aux_portPredecessor;
					m >> c_id >> aux_IpPredecessor >> aux_portPredecessor;
					predecessorId = toInt(c_id);
					dbg(predecessorId);
					dbg(portPredecessor);
					n << "Now you are my predecessor" << ipPredecessor << portPredecessor;
					s_server.send(n);
					ipPredecessor = aux_IpPredecessor;
					portPredecessor = aux_portPredecessor;
					dbg(predecessorId);
					dbg(portPredecessor);
				}
				if (ans == "Now I am your sucessor") {
					m >> c_ipSucessor >> c_portSucessor >> c_id;
					n << "Ok";
					// dbg(c_ipSucessor); dbg(c_portSucessor); dbg(c_id);
					string new_endPoint = tcp + ipSucessor + ":" + portSucessor;
					s_server.send(n);
					if (c_portSucessor != portSucessor and new_endPoint != client_endPoint) {// if (c_ipSucessor != ipSucessor) {
						ipSucessor = c_ipSucessor;
						portSucessor = c_portSucessor;
						sucessorId = toInt(c_id);
						cout << "Disconnecting from " << client_endPoint << endl;
						s_client.disconnect(client_endPoint);
						client_endPoint = new_endPoint;
						cout << "Connecting to " << client_endPoint << endl;
						s_client.connect(client_endPoint);
					}
				}
	  }
	}
		i++;
		if (i == 100) break;
  }

	return 0;
}
