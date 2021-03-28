import numpy as np
import time
from matplotlib.pyplot import *
from random import randint, randrange, random
from Examples.GeneticAlgo.custom_random import choices
from typing import List, Callable, Tuple
from collections import namedtuple
from functools import partial
import deliveryDronePacketDistributionHelperForFullSearch as droneHelper

def getFitness(state):
    global stateDim, numberofAgents
    targetState = list(range(numberofAgents*stateDim))
    diff = [abs(a - b) for a, b in zip(state, targetState)]
    fitness = -sum(diff)
    return fitness

numberofAgents = 3
stateDim = 7
droneHelper.Init(numberofAgents, stateDim)

# Get perfect state which is ordered already
state = list(range(numberofAgents*stateDim))
# Get a random switch
state = droneHelper.GetRandomSwitchedState(state)

isDone = False
while isDone is not True:
    value = getFitness(state)
    isDone, newState = droneHelper.Process(state, value)
    print(isDone, value, state, newState)
    state = newState

print(newState)
