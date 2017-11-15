#include <iostream>
#include <zmqpp/zmqpp.hpp>
#include "lib/zhelpers.hpp"

using namespace std;
using namespace zmqpp;

#define dbg(x) cout << #x << ": " << x << endl

int main(int argc, char** argv)
{
	context ctx;
	string server_port = "tcp://localhost:5550";
	socket subscriber(ctx, socket_type::sub); //Listening
	subscriber.connect(server_port);
	subscriber.setsockopt(ZMQ_SUBSCRIBE, "B", 1);
	cout << "Server Listening on port 5550" << endl;
	while(true){
		// Read envelope with address
		string address = s_recv(subscriber);
		// Read message contents
		string contents = s_recv(subscriber);

		cout << "[" << address << "]" << contents << endl;
	}
	return 0;
}