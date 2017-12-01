from graphviz import Digraph
import pygame
import sys

WIDTH = 800
HEIGHT = 600
curdir = "./"

dot = Digraph(comment='BitTorrent', format='png')

dot.node('A', 'A')
dot.node('B', 'B')
dot.node('L', 'L')

dot.edges(['AB', 'AL'])
dot.edge('B', 'L', constraint='false')

dot.render('graph.gv', view=False)

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
