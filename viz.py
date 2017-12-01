from graphviz import Digraph
import pygame
import sys
import zmq
import thread

def createGraph(info):
    dot = Digraph(comment='BitTorrent', format='png')

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

def printWindow():
    pygame.init()
    clock = pygame.time.Clock()
    pygame.mouse.set_visible(True)

    while(True):
        mouse_pos = pygame.mouse.get_pos()
        events = pygame.event.get()

        for event in events:
            if event.type == pygame.QUIT:
                sys.exit()

        background = pygame.image.load('graph.gv.png')
        window = pygame.display.set_mode(background.get_rect().size, 0, 32)
        background = background.convert()
        window.blit(background,(0,0))
        pygame.display.flip()
        pygame.display.update()
        clock.tick(60)

    pygame.display.quit()

def main():
    context = zmq.Context()
    socket = context.socket(zmq.SUB)
    socket.setsockopt(zmq.SUBSCRIBE, "")
    endPoint = "tcp://localhost:5564"
    print("Connecting to " + endPoint)
    socket.connect(endPoint)
    thread.start_new_thread(printWindow, ())

    while(True):
        message = socket.recv()
        print "Recieved %s" % message
        createGraph(message)


if __name__ == "__main__":
    main()
