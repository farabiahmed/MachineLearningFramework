# Online Python compiler (interpreter) to run Python online.
# Write Python 3 code in this online editor and run it.
import numpy as np
import itertools 
from typing import List, Callable, Tuple
import operator
from Examples.GeneticAlgo.custom_random import choices

numberofagent = 0
state_dim = 7
dictStateFitness = {}
packagePermutations = []

def SwitchPackages(state, packageOrder):
        #print("state:"+str(state))
    
    # 1. Get the packages
    packages = []
    for i in range(numberofagent):
        packages.append(state[state_dim*i+3:state_dim*i+7])
    #print("Packages:"+str(packages))
    
    # 2. Switch the packages upon given parameter
    newState = state
    for i in range(numberofagent):
        newState[state_dim*i+3:state_dim*i+7] = packages[packageOrder[i]]
    return newState

def RandomPacketSwitch(state):
    return SwitchPackages(state, choices(packagePermutations)[0])

def GetFitnessValue(state):
    fitness = state[3]
    return fitness;
    
def Init(param_numberofagent, param_state_dim):

    global numberofagent, state_dim, dictStateFitness, packagePermutations

    numberofagent =  param_numberofagent
    state_dim = param_state_dim

    packageIds = list(range(numberofagent))
    #print("Packages: " + str(packageIds)) #Packages: [0, 1, 2]
    
    # Get all permutations 
    packagePermutations = list(itertools.permutations(packageIds))
        #print("packagePermutations:")
        #print([str(x) for x in packagePermutations])
        # ['(0, 1, 2)', '(0, 2, 1)', '(1, 0, 2)', '(1, 2, 0)', '(2, 0, 1)', '(2, 1, 0)']
    
    # initialize dictionary to store key-value of state-fitness
    dictStateFitness = {}


def Process(state, fitnessValue):

    global dictStateFitness

    #state = np.array([11,12,13,14,15,16,17,21,22,23,24,25,26,27, 31,32,33,34,35,36,37])
    #print("State: " + str(state)) # State: [11 12 13 14 15 16 17 21 22 23 24 25 26 27 31 32 33 34 35 36 37]
    
    # import initial value
    dictStateFitness[tuple(state)] = fitnessValue

    # remove first element
    packagePermutations.pop(0)

    if len(packagePermutations)==0:
        done=True

        print("Dictionary:", dictStateFitness)

        # https://www.w3resource.com/python-exercises/dictionary/python-data-type-dictionary-exercise-1.php
        dictStateFitness_sorted = sorted(dictStateFitness.items(), key=operator.itemgetter(1),reverse=True)
        print('Dictionary in descending order by value : ',dictStateFitness_sorted)

        state = dictStateFitness_sorted[0][0]
        fitness = dictStateFitness_sorted[0][1]
        print("The chosen state:", state, " fitness:", fitness)
                
    else:
        done=False
        state = SwitchPackages(state, packagePermutations[0])

    return done, state


#for x in packagePermutations:
#    newState = SwitchPackages(state.tolist(), x)
#    print("Perm:" + str(x) + " New State:"+str(newState))
#    dictStateFitness[tuple(newState)] = GetFitnessValue(newState)
#    print("Dictionary length:", len(dictStateFitness))

    

