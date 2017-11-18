# Bit Torrent

The objective is to connect all nodes in a ring, all of them connected only
to another by a socket (*client_endPoint*) and listening in another (*server_endPoint*).
The nodes must organize themselves in order, so we can have something like this:

... -> 15 -> 11 -> 12 -> 13 -> 14 -> ... 

### To compile

```
g++ -std=c++11 ring.cpp -lzmq -lzmqpp -lpthread -o ring.out
g++ -std=c++11 subscriber.cpp -lzmq -lzmqpp -o subscriber.out

```

### To run  

```
./subscriber.out
./ring.out "<local ip>" "<local port>" "<remote ip>" "<remote port>" <node identifier (number)>

```

E.g:

```
./ring.out "*" "5552" "localhost" "5553" 11
./ring.out "*" "5552" "localhost" "5553" 12
./ring.out "*" "5552" "localhost" "5553" 10

```
