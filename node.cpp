#include <iostream>
#include <vector>
using namespace std;

#define dbg(x) cout << #x << ": " << x << endl

class Node {

private:
	int id;
	string ip, port;
  vector<pair<int,string>> fingerTable; // id, ip + port

public:
  Node() {}

  Node(string _ip, string _port, int _id) {
		ip = _ip;
		port = _port;
		id = _id;
	}

	string getEndPoint() {
		return "tcp://" + ip + ":" + port;
	}

  void setIp(string _ip) {
    ip = _ip;
  }

  int getId() {
    return id;
  }

  void setId(int _id) {
    id = _id;
  }

  string getIp() {
    return ip;
  }

  string getPort() {
    return port;
  }

  void setPort(string _port) {
    port = _port;
  }

  void resizeFingerTable(int n) {
    fingerTable.resize(n);
  }

  void insertInFingerTable(int _id, string _ip, string _port) {
    string connection = _ip + _port;
    fingerTable.push_back(make_pair(_id, connection));
  }

  void print() {
    cout << "----------------" << endl;
    dbg(id); dbg(ip); dbg(port);
    cout << "----------------" << endl;
  }
};
