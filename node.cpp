#include <iostream>
#include <map>
using namespace std;

#define INF numeric_limits<int>::max()
#define dbg(x) cout << #x << ": " << x << endl

class Node {

private:
	int id;
	string ip, port;
  map<int,string> fingerTable; // id, ip + port

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

  void insertInFingerTable(int _id, string _ip, string _port) {
    fingerTable[_id] = _ip + ":" + _port;
  }

	void removeFingerTable(int _id) {
		fingerTable.erase (_id); 
	}

	pair<int, string> findSucessor(int _id, int sucessorId) {
		int size = fingerTable.size(), i = 0, delta = 0, delta_min = INF, id_min = -1, temp = sucessorId;
    /*if (_id < sucessorId)
      return make_pair(sucessorId, fingerTable[sucessorId]);*/
    for (auto& s : fingerTable) {
      dbg(_id); dbg(s.first);
      delta = _id - s.first;

			if (delta < delta_min) {
				delta_min = delta;
				id_min = s.first;
			}
			dbg(delta);
			dbg(delta_min);
			if (delta_min < 0) {
				return make_pair(temp, fingerTable[temp]);
			}
      temp = s.first;
		}
	}

  void print() {
    cout << "----------------" << endl;
    dbg(id); dbg(ip); dbg(port);
    cout << "----------------" << endl;
  }
};
