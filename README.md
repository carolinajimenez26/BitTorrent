# Bit Torrent

Implementation of the *Chord Algorithm*, which consists on connect multiples nodes in a ring network, the connections are handle by sockets (*client_endPoint* and *server_endPoint*). The nodes have a finger-table, which is used to save multiple successors of a node, to make the algorithm faster than the linear implementation.
The nodes must organize themselves in order, so we can have something like this:

... -> 15 -> 11 -> 12 -> 13 -> 14 -> ...

`ring.cpp` is the linear implementation without a finger-table (only saves the predecessor and successor). 

`ring2.cpp` is the implementation with the finger-table.

[ZMQ](http://zeromq.org/) was used to handle all the connections.

### To compile

```
g++ -std=c++11 ring2.cpp -lzmq -lzmqpp -lpthread -o ring.out
g++ -std=c++11 subscriber.cpp -lzmq -lzmqpp -o subscriber.out

```

### To run  

In this order:

```
python viz.py
./subscriber.out
./ring.out "<local ip>" "<local port>" "<remote ip>" "<remote port>" <node identifier (number)>

```

E.g In ring execution:

```
./ring.out "*" "5552" "localhost" "5553" 11
./ring.out "*" "5552" "localhost" "5553" 12
./ring.out "*" "5552" "localhost" "5553" 10

```
