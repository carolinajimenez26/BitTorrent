#include <iostream>
#include <random>
#include <sstream>
#include <zmqpp/zmqpp.hpp>
#include <thread>
#include "node.cpp"

using namespace std;
using namespace zmqpp;

#define NumberOfBits 5 // numbers between 0 and 2^5-1 = 31
#define dbg(x) cout << #x << ": " << x << endl

const int range_from = 0, range_to = 30;

string toSusbcriber = "";

void messageToSubscriber(string &text) {
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

void outOfTheRing(socket &s_client, Node predecessor, Node sucessor, Node me) {

  message m, n;

  if (sucessor.getEndPoint() == me.getEndPoint()
      and predecessor.getEndPoint() == me.getEndPoint()) { // connected to myself

    toSusbcriber = "out:" + me.getEndPoint();
    cout << "---------------------- Good bye baby ----------------------" << endl;
    exit(1);
    
  } else {

    m << "I'm going out, this is your new predecessor"
      << toString(predecessor.getId())
      << predecessor.getIp()
      << predecessor.getPort();
    s_client.send(m);
    s_client.receive(n); // Ok
    if (sucessor.getEndPoint() != predecessor.getEndPoint()) {
        cout << "Disconnecting from " << sucessor.getEndPoint() << endl;
        s_client.disconnect(sucessor.getEndPoint());
        cout << "Connecting to " << predecessor.getEndPoint() << endl;
        s_client.connect(predecessor.getEndPoint());
    }
    m << "I'm going out, this is your new sucessor"
      << toString(sucessor.getId())
      << sucessor.getIp()
      << sucessor.getPort();
    s_client.send(m);
    s_client.receive(n); // Ok
    cout << "Disconnecting from " << predecessor.getEndPoint() << endl;
    s_client.disconnect(predecessor.getEndPoint());

    toSusbcriber = "out:" + me.getEndPoint();
    cout << "---------------------- Good bye baby ----------------------" << endl;
    exit(1);
  }
}

void printInformation(Node me, Node sucessor, Node predecessor) {
  cout << "Me: " << endl;
  me.print();
  cout << "Sucessor: " << endl;
  sucessor.print();
  cout << "Predecessor: " << endl;
  predecessor.print();
}

void enterToTheRing(Node me, Node sucessor, Node predecessor, bool &flag) {
  flag = true;
  cout << "-------Entered to the ring!!-------" << endl;
  printInformation(me, sucessor, predecessor);
  cout << "---------------------------" << endl;
  dbg(me.getEndPoint());
  toSusbcriber = me.getEndPoint();
}

void ask(socket &s_client, Node &me, Node &predecessor, Node &sucessor, bool &flag) {

  context ctx;
  socket s(ctx, socket_type::req);
  s.connect(sucessor.getEndPoint());

  while (true) {
    if (flag) { // enteredToRing
      message m;
      string op;
      cout << "*************************" << endl;
      cout << "Enter an option" << endl;
      cout << "1 - Exit" << endl;
      cout << "*************************" << endl;
      cin >> op;
      if (op == "1" or op == "Exit") {
        outOfTheRing(s_client, predecessor, sucessor, me);
      }
    }
  }
}

string findSucessor(int id, Node me, Node sucessor) {
  return "";
}

void updateFingerTable(Node &me) {

}

void updatePredecessor(Node &predecessor, int id, string ip, string port) {
  predecessor.setId(id);
  predecessor.setIp(ip);
  predecessor.setPort(port);
}

void updateSucessor(Node &sucessor, int id, string ip, string port) {
  sucessor.setId(id);
  sucessor.setIp(ip);
  sucessor.setPort(port);
}

int main(int argc, char** argv) {

  if(argc != 6){
      cout << "Usage: \"<local ip>\" \"<local port>\" \"<remote ip>\" \"<remote port>\"" << endl;
      return 1;
  }

	Node me("*", argv[2], toInt(argv[5]));
  Node sucessor(argv[3], argv[4], -1);
  Node predecessor(argv[3], argv[4], -1);

  me.resizeFingerTable(NumberOfBits);

  context ctx;
	socket s_server(ctx, socket_type::rep); //Listening
	socket s_client(ctx, socket_type::req); //Asking

  s_server.bind(me.getEndPoint());
  cout << "Server listening on " << me.getEndPoint() << endl;
  me.setIp("localhost");
  s_client.connect(sucessor.getEndPoint());
  cout << "Client connected to " << sucessor.getEndPoint() << endl;

  poller pol;
  pol.add(s_server);
  pol.add(s_client);

  bool id_flag = false, enteredToRing = false, baseCase = false;

  thread t1(messageToSubscriber, ref(toSusbcriber));
  thread t2(ask, ref(s_client), ref(me), ref(predecessor), ref(sucessor),
            ref(enteredToRing));

  message m;
  m << "Want to join. This is my information "
    << toString(me.getId())
    << me.getEndPoint();
  s_client.send(m);

  while (true) {
    if (pol.poll()) {
      if (!enteredToRing) {
        if (pol.has_input(s_client)) {
          message m;
          string ans;
          s_client.receive(m);
          m >> ans;
          cout << "Receiving from server -> " << ans << endl;

        }
      }
      if (pol.has_input(s_server)) {
        message m;
        s_server.receive(m);
        string ans;
        m >> ans;
        cout << "Receiving from client -> " << ans << endl;

        if (ans == "send me your information") {
          string information = "I am " + toString(me.getId()) +
                     "-> predecessorId: " + toString(predecessor.getId()) +
                     ", sucessorId: " + toString(sucessor.getId()) +
                     ". Connected to: " + sucessor.getEndPoint() +
                     ". Listening on: " + me.getEndPoint();
          message n;
          n << information;
          s_server.send(n);
        }

        if (ans == "Want to join. This is my information ") {
          string id, endPoint;
          m >> id >> endPoint;
          dbg(id); dbg(endPoint);
          if (endPoint == predecessor.getEndPoint()
              and endPoint == sucessor.getEndPoint()) baseCase = true;
          if (baseCase) {
            sucessor.setId(toInt(id));
            predecessor.setId(toInt(id));
            enterToTheRing(me, sucessor, predecessor, enteredToRing);
            message n;
            n << "";
            s_server.send(n);
          } else {
            string sucessorInformation = findSucessor(toInt(id), me, sucessor);
          }
        }

        if (ans == "I'm going out, this is your new predecessor") {
          string id, ip, port;
          m >> id >> ip >> port;
          dbg(id); dbg(ip); dbg(port);
          updatePredecessor(predecessor, toInt(id), ip, port);
          message n;
          n << "Ok";
          s_server.send(n);
        }

        if (ans == "I'm going out, this is your new sucessor") {
          string id, ip, port;
          m >> id >> ip >> port;
          dbg(id); dbg(ip); dbg(port);
          s_client.disconnect(sucessor.getEndPoint());
          updateSucessor(sucessor, toInt(id), ip, port);
          s_client.connect(sucessor.getEndPoint());
          message n;
          n << "Ok";
          s_server.send(n);
          printInformation(me, sucessor, predecessor);
        }
      }
    }
  }

  return 0;
}
