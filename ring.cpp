#include <iostream>
#include <zmqpp/zmqpp.hpp>
#include "lib/zhelpers.hpp"
#include <string>
#include <thread>

using namespace std;
using namespace zmqpp;

#define dbg(x) cout << #x << ": " << x << endl

void messageToSubscriber(string &message){
	//---------- preparing publisher
	//  Prepare our context and publisher
    zmq::context_t context(1);
    zmq::socket_t publisher(context, ZMQ_PUB);
    publisher.bind("tcp://*:5563");

	while(true){	
		s_sendmore (publisher, "A");
        s_send (publisher, message);
        sleep(1);
    
	}
}

int main(int argc, char** argv)
{
	if(argc != 3){
		cout << "Missing arguments" << endl;
		return -1;
	}

	//--------------- Declaration of variables and sockets -----------------
	int myid;
	int predecessor;
	int sucessor;
	string ipSucessor, ipPredecessor; //ip's
	string currentNode, nextNode; //ip's
	context ctx;
	socket s_server(ctx, socket_type::rep); //Listening
	socket s_client(ctx, socket_type::req); //Asking
	poller pol;
	string toSusbcriber = "Hello World!";
	thread t1(messageToSubscriber, ref(toSusbcriber));
	// ---------------------------------------------------------------------

	//------------------ Initialization of sockets and poller ---------------
	string a(argv[1]);
	string b(argv[2]);
	string server_port = "tcp://*:555" + a;
	string client_port = "tcp://localhost:555" + b;
	s_server.bind(server_port);
	s_client.connect(client_port);
	cout << "Server listening on " << server_port << endl;
	cout << "Client connected on " << client_port << endl;
	pol.add(s_server);
	pol.add(s_client);
	// --------------------------------------------------------------------
	while(true){
		cin >> toSusbcriber;
	}
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
