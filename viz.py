from graphviz import Digraph
import pygame
import sys
import zmq

def createGraph():

    dot = Digraph(comment='BitTorrent', format='png')

    dot.node('A', 'A')
    dot.node('B', 'B')
    dot.node('L', 'L')

    dot.edges(['AB', 'AL'])
    dot.edge('B', 'L', constraint='false')

    dot.render('graph.gv', view=False)

def printWindow():
    pygame.init()
    background = pygame.image.load('graph.gv.png')
    # window = pygame.display.set_mode((WIDTH, HEIGHT))
    window = pygame.display.set_mode(background.get_rect().size, 0, 32)
    background = background.convert()
    clock = pygame.time.Clock()

    window.blit(background,(0,0))

    pygame.mouse.set_visible(True)

    pygame.display.flip()

    while(True):
        mouse_pos = pygame.mouse.get_pos()
        events = pygame.event.get()

        for event in events:
            if event.type == pygame.QUIT:
                sys.exit()

        window.blit(background,(0,0))
        pygame.display.flip()
        clock.tick(60)

    pygame.display.quit()

def askInformation(ips, s_client):
    pass

def main():
    context = zmq.Context()
    socket = context.socket(zmq.SUB)
    socket.setsockopt(zmq.SUBSCRIBE, "")
    endPoint = "tcp://localhost:5564"
    print("Connecting to " + endPoint)
    socket.connect(endPoint)

    while(True):
        message = socket.recv()
        print "Recieved %s" % message


if __name__ == "__main__":
    main()
