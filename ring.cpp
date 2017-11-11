#include <iostream>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;

#define dbg(x) cout << #x << ": " << x << endl


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

  int myid, predecessor, sucessor;
	string ipSucessor, ipPredecessor, currentNode, nextNode; //ip's

  while (true) {
    if (pol.poll()) {
      if (pol.has_input(s_client)) {

      }
      if (pol.has_input(s_server)) {

      }
    }
  }

	return 0;
}
