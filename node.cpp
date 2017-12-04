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
		if (_id != id) fingerTable[_id] = _ip + ":" + _port;
  }

	void removeFingerTable(int _id) {
		fingerTable.erase(_id);
	}

	void clearFingerTable() {
		fingerTable.clear();
	}

	void showFingerTable() {
		cout << "------------showFingerTable---------------" << endl;
		for (auto& s : fingerTable) {
			cout << s.first << " -> " << s.second << endl;
		}
		cout << "------------------------------------------" << endl;
	}

	string getFingerTable() {
		string finger = "";
		for (auto& s : fingerTable) {
			finger += s.first + "->" + s.second + "\n";
		}
		return finger;
	}

	pair<int, string> findSucessor(int _id) {
		int size = fingerTable.size(), delta = 0, delta_min = INF, id_min = -1;
		for (auto& s : fingerTable) {
			dbg(_id); dbg(s.first);
			delta = _id - s.first;
			if (delta < delta_min and delta > 0) {
				delta_min = delta;
				id_min = s.first;
			} if (delta < 0) break; // in order, I don't need the greates, only the nearest
			dbg(delta);
			dbg(delta_min);
		}
		if (id_min < 0) return make_pair(id_min, "");
		return make_pair(id_min, fingerTable[id_min]);
	}

  void print() {
    cout << "----------------" << endl;
    dbg(id); dbg(ip); dbg(port);
    cout << "----------------" << endl;
  }
};
