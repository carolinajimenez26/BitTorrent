from graphviz import Digraph
import pygame
import sys
import zmq
import thread
import os.path

def createGraph(info, window, clock):
    dot = Digraph(comment='BitTorrent', format='jpg')

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

    dot.render('graph.gv', view=False)

    background = pygame.image.load("graph.gv.jpg")
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

    # -------------------Pygame-----------------------

    pygame.init()
    clock = pygame.time.Clock()
    pygame.mouse.set_visible(True)
    pygame.display.set_caption("BitTorrent")
    window = pygame.display.set_mode((500,500), 0, 32)

    while(True):
        # -------------------Pygame-----------------------
        events = pygame.event.get()

        for event in events:
            if event.type == pygame.QUIT:
                break

        # ----------------Zmq----------------------------
        message = socket.recv()
        print "Recieved %s" % message
        createGraph(message, window, clock)

    pygame.display.quit()


if __name__ == "__main__":
    main()
