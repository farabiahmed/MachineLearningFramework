import pygame
from pygame.locals import *

pygame.init()

gameDisplay = pygame.display.set_mode((400,400))

pygame.display.set_caption('Multiagent Gridworld')

clock  = pygame.time.Clock()

crashed = False

agentImg = pygame.image.load('agent.png')
agentImg = pygame.transform.scale(agentImg, (50, 50))

def agent(x,y):

    gameDisplay.blit(agentImg,(x,y))

while not crashed:

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            crashed = True
            print("Good Bye")
        #print(event)


    gameDisplay.fill((255,255,255))
    agent(40, 40)

    pygame.display.update()
    clock.tick(60)

pygame.quit()
quit()