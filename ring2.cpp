#include <iostream>
#include <random>
#include <sstream>
#include <zmqpp/zmqpp.hpp>
#include <thread>
#include <chrono>
#include <math.h>
#include "lib/node.cpp"
#include <string>

using namespace std;
using namespace zmqpp;

#define NumberOfBits 5 // numbers between 0 and 2^5-1 = 31
#define dbg(x) cout << #x << ": " << x << endl

const int range_from = 0, range_to = 31;

void messageToTerminal(socket &s_terminal, string text) {
  message m;
  m << text;
  s_terminal.send(m);
  cout << "Sent to susbscriber" << endl;
}

bool inTheRange(int left, int right, int i) {
  dbg(left); dbg(right); dbg(i);
  if ((i > right and i <= range_to) or (i >= range_from and i < left)) return true;
  return false;
}

pair<int, string> findSucessor(int id, Node me, Node sucessor, Node predecessor) {
  cout << "findSucessor" << endl;
  string endPoint = "";
  dbg(me.getId()); dbg(sucessor.getId()); dbg(predecessor.getId());
  dbg((me.getId() > sucessor.getId()));
  dbg(inTheRange(sucessor.getId(), me.getId(), id));
  if (id == me.getId()) { // somebody is looking for my sucessor
    cout << "somebody is looking for me" << endl;
    endPoint = me.getIp() + ":" + me.getPort();
    return make_pair(me.getId(), endPoint);
  } else if (predecessor.getId() == id) {
    cout << "Returned " << predecessor.getId() << endl;
    endPoint = predecessor.getIp() + ":" + predecessor.getPort();
    return make_pair(predecessor.getId(), endPoint);
  } else if (me.getId() > id and id > predecessor.getId()) { // I am your sucessor
    cout << "I am your sucessor " << me.getId() << endl;
    endPoint = me.getIp() + ":" + me.getPort();
    return make_pair(me.getId(), endPoint);
  } else if (me.getId() > sucessor.getId() and
      inTheRange(sucessor.getId(), me.getId(), id)) { // in the end of the range
    cout << "In the end of the range" << endl;
    endPoint = sucessor.getIp() + ":" + sucessor.getPort();
    return make_pair(sucessor.getId(), endPoint);
  } else if (sucessor.getId() > id and id > me.getId()) { // "My sucessor is your sucessor"
    cout << "My sucessor is your sucessor" << endl;
    endPoint = sucessor.getIp() + ":" + sucessor.getPort();
    return make_pair(sucessor.getId(), endPoint);
  } else { // take a look in my fingerTable
    pair<int, string> new_sucessor = me.findSucessor(id); // id, endPoint
    dbg(new_sucessor.first); dbg(new_sucessor.second);

    if (new_sucessor.first < 0) { // Gonna ask my sucessor
      endPoint = sucessor.getEndPoint();
    } else { // Gona ask the nearest sucessor
      endPoint = "tcp://" + new_sucessor.second;
    }

    message m, n;
    string ans, new_id, new_endPoint;

    context ctx;
    socket s(ctx, socket_type::req); // Asking
    s.connect(endPoint);
    cout << "Connecting to " << endPoint << endl;

    m << "Looking for sucessor of"
      << toString(id);
    s.send(m);
    cout << "Sent: Looking for sucessor of " << toString(id) << endl;
    cout << "Asked to " << new_sucessor.first << " -> " << endPoint << endl;
    s.receive(n); // "Found the sucessor. It is "
    n >> ans >> new_id >> new_endPoint;
    dbg(ans); dbg(new_id); dbg(new_endPoint);
    s.disconnect(endPoint); // Thanks, bye
    return make_pair(toInt(new_id), new_endPoint);
  }
}

void updateFingerTable(Node &me, Node &sucessor, bool &flag, Node &predecessor) {
  // chrono::seconds interval(30); // 10 seconds
  // while (true) {
    if (flag) {
      cout << "updateFingerTable" << endl;
      me.clearFingerTable();
      me.insertInFingerTable(sucessor.getId(), sucessor.getIp(), sucessor.getPort());
      int bit = 1;
      for (int i = 0; i < NumberOfBits; i++) {
        dbg(bit);
        int needed = (me.getId() + bit) % range_to;
        bit = bit << 1;
        dbg(needed);
        if (needed != sucessor.getId()) {
          pair<int, string> new_sucessor = findSucessor(needed, me, sucessor, predecessor);
          vector<string> splitted = split(new_sucessor.second, ':');
          dbg(splitted[0]); dbg(splitted[1]);
          me.insertInFingerTable(new_sucessor.first, splitted[0], splitted[1]);
        }
      }
      me.showFingerTable();
      // this_thread::sleep_for(interval);
    }
  // }
}

void outOfTheRing(socket &s_client, socket &s_terminal, Node predecessor,
                  Node sucessor, Node me) {

  message m, n;

  if (sucessor.getEndPoint() == me.getEndPoint()
      and predecessor.getEndPoint() == me.getEndPoint()) { // connected to myself

    string toTerminal = "out:" + me.getEndPoint();
    messageToTerminal(s_terminal, toTerminal);
    cout << "---------------------- Good bye baby ----------------------" << endl;
    exit(1);

  } else {
    dbg(predecessor.getId()); dbg(predecessor.getIp()); dbg(predecessor.getPort());
    m << "I'm going out, this is your new predecessor"
      << toString(predecessor.getId())
      << predecessor.getIp()
      << predecessor.getPort();
    s_client.receive(n); // lost message
    // if (pol.has_input(s_client)) {
    //   message tmp_m;
    //   string tmp;
    //   s_client.receive(tmp_m);
    //   tmp_m >> tmp;
    //   dbg(tmp);
    // }
    s_client.send(m);
    cout << "Sent: I'm going out, this is your new predecessor" << endl;
    s_client.receive(n); // Ok
    cout << "Received confirmation" << endl;
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
    cout << "Sent: I'm going out, this is your new sucessor" << endl;
    s_client.receive(n); // Ok
    cout << "Received confirmation" << endl;
    cout << "Disconnecting from " << predecessor.getEndPoint() << endl;
    s_client.disconnect(predecessor.getEndPoint());

    string toTerminal = "out:" + me.getEndPoint();
    messageToTerminal(s_terminal, toTerminal);
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

void enterToTheRing(Node &me, Node &sucessor, Node &predecessor, bool &flag,
                    socket &s_client, bool baseCase, socket &s_terminal) {
  flag = true;
  cout << "-------Entered to the ring!!-------" << endl;
  printInformation(me, sucessor, predecessor);
  cout << "---------------------------" << endl;
  dbg(me.getEndPoint());
  string toTerminal = me.getEndPoint();
  messageToTerminal(s_terminal, toTerminal);
  if (!baseCase) {
    updateFingerTable(me, sucessor, flag, predecessor);
    message m, n;
    m << "Entered to the Ring" << toString(me.getId());
    s_client.send(m);
    s_client.receive(n);
    cout << "Received ack" << endl;
  }
}

void ask(socket &s_client, socket &s_terminal,Node &me, Node &predecessor,
         Node &sucessor, bool &flag) {

  while (true) {
    if (flag) { // enteredToRing
      string op = "";
      cout << "*************************" << endl;
      cout << "Enter an option" << endl;
      cout << "1 - Exit" << endl;
      cout << "2 - Show fingerTable" << endl;
      cout << "3 - Show Ring" << endl;
      cout << "*************************" << endl;
      cin >> op;
      if (op == "1" or op == "Exit") {
        outOfTheRing(s_client, s_terminal, predecessor, sucessor, me);
      } else if (op == "2" or op == "Show fingerTable"){
        string toTerminal = "showFingerTable:" + me.getFingerTable();
        messageToTerminal(s_terminal, toTerminal);
      } else if (op == "3" or op == "Show Ring"){
        string toTerminal = "ask";
        messageToTerminal(s_terminal, toTerminal);
      } else {
        cout << "Invalid operation" << endl;
      }
    }
  }
}

void updatePredecessor(Node &predecessor, int id, string ip, string port) {
  cout << "updatePredecessor" << endl;
  predecessor.setId(id);
  predecessor.setIp(ip);
  predecessor.setPort(port);
}

void updateSucessor(Node &me, Node &sucessor, int id, string ip, string port) {
  cout << "updateSucessor" << endl;
  me.removeFingerTable(sucessor.getId());
  sucessor.setId(id);
  sucessor.setIp(ip);
  sucessor.setPort(port);
  me.insertInFingerTable(sucessor.getId(), sucessor.getIp(), sucessor.getPort());
}

int main(int argc, char** argv) {

  if(argc != 6){
      cout << "Usage: \"<local ip>\" \"<local port>\" \"<remote ip>\" \"<remote port>\"" << endl;
      return 1;
  }

  Node me("*", argv[2], toInt(argv[5]));
  Node sucessor(argv[3], argv[4], -1);
  Node predecessor(argv[3], argv[4], -1);

  context ctx;

  /*--------------Server--------------------*/
  socket s_server(ctx, socket_type::rep); //Listening
  s_server.bind(me.getEndPoint());
  cout << "Server listening on " << me.getEndPoint() << endl;
  me.setIp("localhost");
  /*-----------------------------------------*/

  /*--------------Client----------------------*/
  socket s_client(ctx, socket_type::req); //Asking
  s_client.connect(sucessor.getEndPoint());
  cout << "Client connected to " << sucessor.getEndPoint() << endl;
  /*------------------------------------------*/

  //----------- terminal ----------------------
  socket s_terminal(ctx, socket_type::req); //Asking
  s_terminal.connect("tcp://localhost:5563");
  cout << "Terminal connected to tcp://localhost:5563" << endl;
  //-------------------------------------------

  poller pol;
  pol.add(s_server);
  pol.add(s_client);
  pol.add(s_terminal);

  bool aux = true, enteredToRing = false, baseCase = false;

  /*---------------Threads---------------------*/
  thread t2(ask,ref(s_client), ref(s_terminal),ref(me), ref(predecessor), ref(sucessor),
            ref(enteredToRing));
  // thread t3(updateFingerTable, ref(me), ref(sucessor), ref(enteredToRing),
            // ref(predecessor));
  /*-------------------------------------------*/

  message m;
  m << "Want to join. This is my information "
    << toString(me.getId())
    << me.getEndPoint();
  s_client.send(m);

  while (true) {
    if (pol.poll()) {
      if (pol.has_input(s_terminal)) {
        cout << "pol.has_input(s_terminal)" << endl;
        message m;
        string ans;
        s_terminal.receive(m);
        m >> ans;
        cout << "Receiving from terminal -> " << ans << endl;
      }
      if (!enteredToRing) {
        if (pol.has_input(s_client)) {
          cout << "pol.has_input(s_client)" << endl;
            message m;
            string ans;
            s_client.receive(m);
            m >> ans;
            cout << "Receiving from server -> " << ans << endl;

            if (ans == "This is your sucessor ") {
              string id, endPoint, ip;
              m >> id >> endPoint;
              dbg(id); dbg(endPoint);
              s_client.disconnect(sucessor.getEndPoint());
              cout << "Disconnecting from " << sucessor.getEndPoint() << endl;
              vector<string> splitted = split(endPoint, ':');
              dbg(splitted[0]);
              dbg(splitted[1]);
              updateSucessor(me, sucessor, toInt(id), splitted[0], splitted[1]);
              s_client.connect(sucessor.getEndPoint());
              cout << "Connecting to " << sucessor.getEndPoint() << endl;
              message n;
              n << "Now I am your predecessor."
                << toString(me.getId())
                << me.getIp()
                << me.getPort();
              s_client.send(n);
            }

            if (ans == "This is your new predecessor") {
              string id, ip, port;
              m >> id >> ip >> port;
              updatePredecessor(predecessor, toInt(id), ip, port);
              s_client.disconnect(sucessor.getEndPoint());
              cout << "Disconnecting from " << sucessor.getEndPoint() << endl;
              s_client.connect(predecessor.getEndPoint());
              cout << "Connecting to " << predecessor.getEndPoint() << endl;
              message n, l;
              n << "Now I am your sucessor"
                << toString(me.getId())
                << me.getIp()
                << me.getPort();
              s_client.send(n);
              cout << "Sent: Now I am your sucessor "
                   << me.getId() << me.getIp() << me.getPort() << endl;
              s_client.receive(l); // Ok
              s_client.disconnect(predecessor.getEndPoint());
              cout << "Disconnecting from " << predecessor.getEndPoint() << endl;
              s_client.connect(sucessor.getEndPoint());
              cout << "Connecting to " << sucessor.getEndPoint() << endl;
              enterToTheRing(me, sucessor, predecessor, enteredToRing, s_client,
                             baseCase, s_terminal);
            }
        }
      } else {
        // if (aux) {
        //   cout << "Removing s_client form poller" << endl;
        //   pol.remove(s_client);
        //   aux = false;
        // }
      }
      if (pol.has_input(s_server)) {
        cout << "pol.has_input(s_server)" << endl;
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
          cout << "Sent my information" << endl;
        }

        if (ans == "Want to join. This is my information ") {
          string id, endPoint;
          m >> id >> endPoint;
          dbg(id); dbg(endPoint);
          if (endPoint == predecessor.getEndPoint()
              and endPoint == sucessor.getEndPoint()) baseCase = true;
          dbg(baseCase);
          if (baseCase) {
            sucessor.setId(toInt(id));
            me.insertInFingerTable(sucessor.getId(), sucessor.getIp(), sucessor.getPort());
            predecessor.setId(toInt(id));
            message n;
            n << "ack";
            s_server.send(n);
            enterToTheRing(me, sucessor, predecessor, enteredToRing, s_client, baseCase, s_terminal);
            baseCase = false;
          } else {
            pair<int, string> sucessorInformation = findSucessor(toInt(id), me, sucessor, predecessor);
            dbg(sucessorInformation.first); dbg(sucessorInformation.second);
            message n;
            n << "This is your sucessor "
              << toString(sucessorInformation.first)
              << sucessorInformation.second;
            s_server.send(n);
            cout << "Sent: This is your sucessor "
                 << sucessorInformation.first << sucessorInformation.second << endl;
          }
        }

        if (ans == "Looking for sucessor of") {
          string id;
          m >> id;
          dbg(id);
          pair<int, string> sucessorInformation = findSucessor(toInt(id), me, sucessor, predecessor);
          message n;
          n << "Found the sucessor. It is "
            << toString(sucessorInformation.first)
            << sucessorInformation.second;
          s_server.send(n);
          cout << "Sent: Found the sucessor. It is "
               << sucessorInformation.first << sucessorInformation.second << endl;
        }

        if (ans == "Now I am your predecessor.") {
          string id, ip, port;
          m >> id >> ip >> port;
          message n;
          n << "This is your new predecessor"
            << toString(predecessor.getId())
            << predecessor.getIp()
            << predecessor.getPort();
          s_server.send(n);
          updatePredecessor(predecessor, toInt(id), ip, port);
        }

        if (ans == "Now I am your sucessor") {
          string id, ip, port;
          m >> id >> ip >> port;
          dbg(id); dbg(ip); dbg(port);
          message n;
          n << "ack"; // Ok
          s_server.send(n);
          cout << "Sent: ack" << endl;
          s_client.disconnect(sucessor.getEndPoint());
          cout << "Disconnecting from " << sucessor.getEndPoint() << endl;
          me.removeFingerTable(sucessor.getId());
          updateSucessor(me, sucessor, toInt(id), ip, port);
          s_client.connect(sucessor.getEndPoint());
          cout << "Connecting to " << sucessor.getEndPoint() << endl;
        }

        if (ans == "Entered to the Ring") {
          string id;
          m >> id;
          dbg(id);
          message n;
          n << "ack";
          s_server.send(n);
          cout << "Ok" << endl;
          if (pol.has_input(s_client)) {
            message tmp_m;
            string tmp;
            s_client.receive(tmp_m);
            tmp_m >> tmp;
            dbg(tmp);
          }
          if (!baseCase and toInt(id) != me.getId()) {
            updateFingerTable(me, sucessor, enteredToRing, predecessor);
            cout << "Replying message" << endl;
            message l, o;
            l << "Entered to the Ring" << id;
            s_client.send(l);
            cout << "Sent" << endl;
            s_client.receive(o);
            cout << "Received answere" << endl;
          }
        }

        if (ans == "I'm going out, this is your new predecessor") {
          string id, ip, port;
          m >> id >> ip >> port;
          dbg(id); dbg(ip); dbg(port);
          updatePredecessor(predecessor, toInt(id), ip, port);
          message n;
          n << "ack";
          s_server.send(n);
          cout << "Sent: ack" << endl;
        }

        if (ans == "I'm going out, this is your new sucessor") {
          string id, ip, port;
          m >> id >> ip >> port;
          dbg(id); dbg(ip); dbg(port);
          s_client.disconnect(sucessor.getEndPoint());
          cout << "Disconnecting from " << sucessor.getEndPoint() << endl;
          me.removeFingerTable(sucessor.getId());
          updateSucessor(me, sucessor, toInt(id), ip, port);
          s_client.connect(sucessor.getEndPoint());
          cout << "Connecting to " << sucessor.getEndPoint() << endl;
          message n;
          n << "ack";
          s_server.send(n);
          cout << "Sent: ack" << endl;
          if (me.getId() != sucessor.getId() and me.getId() != predecessor.getId()) {
            n << "Update fingerTable" << me.getId(); // When it it going to stop
            s_client.send(n);
            cout << "Sent: Update fingerTable " << me.getId() << endl;
            s_client.receive(n);
            cout << "Received confirmation" << endl;
          }
        }

        if (ans == "Update fingerTable") {
          string id;
          m >> id;
          dbg(id);
          message n;
          n << "ack";
          s_server.send(n);
          if (me.getId() != toInt(id) and me.getId() != sucessor.getId() and
              me.getId() != predecessor.getId()) {
            updateFingerTable(me, sucessor, enteredToRing, predecessor);
            cout << "Replying message" << endl;
            message l, o;
            l << "Update fingerTable" << id;
            s_client.send(l);
            cout << "Sent" << endl;
            s_client.receive(o);
            cout << "Received answere" << endl;
          }
        }
      }
    }
  }

  return 0;
}
