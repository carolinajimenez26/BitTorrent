from graphviz import Digraph
import pygame
import sys
import zmq
import thread
import os.path

def createGraph(info, window, clock, fileName, flag):
    dot = Digraph(comment='BitTorrent', format='jpg')

    if (flag):
        info = info.split("\n")
        for i in info:
            if (i):
                subs = i.split("->")[0]
                myNode = subs.split(" ")[2]
                print("myNode ", myNode)
                dot.node(myNode)

                i = i.replace(subs + "->", "")
                neigh = i.split(".")[0]
                neigh = neigh.split(",")[1]
                neigh = neigh.split("sucessorId: ")[1]
                print ("neigh ", neigh)
                dot.edge(myNode, neigh)
    else:
        pass

    dot.render(fileName, view=False)

    background = pygame.image.load(fileName + ".jpg")
    size = background.get_rect().size
    background = background.convert()
    window = pygame.display.set_mode(size, 0, 32)
    window.blit(background,(0,0))
    pygame.display.update()
    pygame.display.flip()
    clock.tick(60)

def main():
    # ----------------Zmq----------------------------
    context = zmq.Context()
    socket = context.socket(zmq.SUB)
    socket.setsockopt(zmq.SUBSCRIBE, "")
    endPoint = "tcp://localhost:5564"
    print("Connecting to " + endPoint)
    socket.connect(endPoint)

    fingerSocket = context.socket(zmq.SUB)
    fingerSocket.setsockopt(zmq.SUBSCRIBE, "")
    endPoint = "tcp://localhost:5574"
    print("Connecting to " + endPoint)
    fingerSocket.connect(endPoint)

    poller = zmq.Poller()
    poller.register(socket, zmq.POLLIN)
    poller.register(fingerSocket, zmq.POLLIN)

    # -------------------Pygame-----------------------

    pygame.init()
    clock = pygame.time.Clock()
    pygame.mouse.set_visible(True)
    pygame.display.set_caption("BitTorrent")
    window = pygame.display.set_mode((500,500), 0, 32)
    fingerWindow = pygame.display.set_mode((500,500), 0, 32)

    while(True):
        # -------------------Pygame-----------------------
        events = pygame.event.get()

        for event in events:
            if event.type == pygame.QUIT:
                break
        # ----------------Zmq----------------------------
        socks = dict(poller.poll())

        if socket in socks and socks[socket] == zmq.POLLIN:
            message = socket.recv()
            print "Recieved %s" % message
            createGraph(message, window, clock, 'graph.gv', True)

        if fingerSocket in socks and socks[fingerSocket] == zmq.POLLIN:
            message = socket.recv()
            print "Recieved %s" % message
            createGraph(message, fingerWindo, clock, 'finger-graph.gv', False)

    pygame.display.quit()


if __name__ == "__main__":
    main()
