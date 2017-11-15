

//
//  Pubsub envelope subscriber
//
// Olivier Chamoux <olivier.chamoux@fr.thalesgroup.com>

#include "lib/zhelpers.hpp"
#include <iostream>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;


int main () {
    //  Prepare our context and subscriber
    /*zmq::context_t context(1);
    zmq::socket_t subscriber (context, ZMQ_SUB);
    subscriber.bind("tcp://*:5563");
    subscriber.setsockopt( ZMQ_SUBSCRIBE, "A", 1);

    while (1) {

        //  Read envelope with address
        std::string address = s_recv (subscriber);
        //  Read message contents
        std::string contents = s_recv (subscriber);
        
        std::cout << "[" << address << "] " << contents << std::endl;
    }*/

    context ctx;
    socket s_server(ctx, socket_type::rep); //Listening
    s_server.bind("tcp://*:5563");
    while(true){
        message m;
        s_server.receive(m);
        string text;
        m >> text;
        cout << text << endl;
        m << "";
        s_server.send(m);
    }

    return 0;
}
