#include <iostream>
#include "lib/node.hh"
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;

#define dbg(x) cout << #x << ": " << x << endl

// class node
// {
// private:
// 	int myid;
// 	int predecessor;
// 	int sucessor;
// 	string ipSucessor, ipPredecessor; //ip's
// 	string currentNode, nextNode; //ip's
// 	context ctx;
// 	socket s_server(ctx, socket_type::rep); //Listening
// 	socket s_client(ctx, socket_type::req); //Asking
// 	poller pol;
// public:
// 	node(string server_port, string client_port){
// 		s_server.bind(server_port);
// 		s_client.connect(client_port);
// 		cout << "Server listening on " << server_port << endl;
// 		cout << "Client connected on " << client_port << endl;
// 		pol.add(s_server);
// 		pol.add(s_client);
//
// 	}
// };

int main(int argc, char** argv)
{

	if(argc != 3){
		cout << "Missing arguments" << endl;
		return -1;
	}
	string a(argv[1]);
	string b(argv[2]);
	string server_port = "tcp://*:555" + a;
	string client_port = "tcp://localhost:555" + b;
	// node n(server_port, client_port);

	return 0;
}
