#include <iostream>
#include <zmqpp/zmqpp.hpp>
#include <string>

using namespace std;
using namespace zmqpp;

class node {
private:
	int myid;
	int predecessor;
	int sucessor;
	string ipSucessor, ipPredecessor; //ip's
	string currentNode, nextNode; //ip's
	context ctx;
	// socket s_server(ctx, socket_type::rep); //Listening
	// socket s_client(ctx, socket_type::req); //Asking
	// poller pol;
public:
	node(string server_port, string client_port){
		/*s_server.bind(server_port);
		s_client.connect(client_port);
		cout << "Server listening on " << server_port << endl;
		cout << "Client connected on " << client_port << endl;
		pol.add(s_server);
		pol.add(s_client);
		*/
	}
};
