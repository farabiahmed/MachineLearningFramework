# Online Python compiler (interpreter) to run Python online.
# Write Python 3 code in this online editor and run it.
import numpy as np
import itertools 
from typing import List, Callable, Tuple
import operator
from Examples.GeneticAlgo.custom_random import choices
import math  # for factorial
import random

packageIds = []
numberofagent = 0
state_dim = 7
dictStateFitness = {}
packagePermutations = []
factorialTable = [] # holds all factorials for efficiency
currentPermutationIndex = 1

def getNthPermutation( pIndex, inList, outList=[] ):
    """ get :pIndex: permutation of :inList:
    
    :warn: permutations are sorted in lexicographical order starting from 0, i.e.:
    0 -> [1, 2, 3], 1 -> [1, 3, 2] and so on
    :param int pIndex: given permutation index
    :param list inList: initial list of elements
    :param list outList: result list
    """
    ## permutation index too big
    if pIndex >= factorialTable[len(inList)]: return []
    ## no more elements to use
    if not inList: return outList
    ## make sure eList is sorted
    inList.sort()
    ## get factorial
    f = factorialTable[len(inList)-1]
    index = pIndex // f
    reminder = pIndex % f
    ## add new element to outList
    outList.append( inList[index] )
    ## ...and remove it from inList
    del inList[index]
    ## bail out or call recursively depending of the reminder 
    if not reminder:
        return outList + inList
    else:
        return getNthPermutation( reminder, inList, outList )

def GetRandomSwitchedState(state):
    global packageIds, numberofagent, factorialTable

    randomPermutationIndex = random.randint(0, factorialTable[numberofagent-1])
    packetOrder = getNthPermutation(randomPermutationIndex, packageIds.copy(), [])
    _state = SwitchPackages(state, packetOrder)
    return _state

def SwitchPackages(state, packageOrder):
        #print("state:"+str(state))
    
    # 1. Get the packages
    packages = []
    for i in range(numberofagent):
        packages.append(state[state_dim*i+3:state_dim*i+7])
    #print("Packages:"+str(packages))
    
    # 2. Switch the packages upon given parameter
    newState = state.copy()
    for i in range(numberofagent):
        newState[state_dim*i+3:state_dim*i+7] = packages[packageOrder[i]]
    return newState

def RandomPacketSwitch(state):
    return SwitchPackages(state, choices(packagePermutations)[0])

def Init(param_numberofagent, param_state_dim):

    global numberofagent, state_dim, dictStateFitness, packagePermutations, factorialTable, packageIds, currentPermutationIndex
    
    numberofagent =  param_numberofagent
    state_dim = param_state_dim

    if len(factorialTable) < numberofagent+1:
        factorialTable = [math.factorial(i) for i in range(numberofagent+1)]

    packageIds = list(range(numberofagent))
    #print("Packages: " + str(packageIds)) #Packages: [0, 1, 2]
    
    # Get all permutations 
    # packagePermutations = list(itertools.permutations(packageIds))
    # print("packagePermutations Len(", len(packagePermutations), ") :", packagePermutations)

    # initialize dictionary to store key-value of state-fitness
    dictStateFitness = {}

    currentPermutationIndex = 1

    #print("\nInitialized (DeliveryDroneHelper)...")


def Process(state, fitnessValue):

    global dictStateFitness, currentPermutationIndex, numberofagent, factorialTable

    #state = np.array([11,12,13,14,15,16,17,21,22,23,24,25,26,27, 31,32,33,34,35,36,37])
    #print("State: " + str(state)) # State: [11 12 13 14 15 16 17 21 22 23 24 25 26 27 31 32 33 34 35 36 37]
    
    # import initial value
    dictStateFitness[tuple(state)] = fitnessValue

    #if(fitnessValue>0.9):
    #    packagePermutations.clear()

    #if len(packagePermutations)==0:
    if currentPermutationIndex == int(factorialTable[numberofagent]):
        done=True

        #print("\nDictionary:", dictStateFitness)

        # https://www.w3resource.com/python-exercises/dictionary/python-data-type-dictionary-exercise-1.php
        dictStateFitness_sorted = sorted(dictStateFitness.items(), key=operator.itemgetter(1),reverse=True)
        #print('\nDictionary in descending order by value : ',dictStateFitness_sorted)

        _state = dictStateFitness_sorted[0][0]
        fitness = dictStateFitness_sorted[0][1]
        #print("\nThe chosen state:", _state, " fitness:", fitness)
                
    else:
        done=False
        packetOrder = getNthPermutation(currentPermutationIndex, packageIds.copy(), [])
        _state = SwitchPackages(state, packetOrder)
        currentPermutationIndex+=1

    return done, _state


#for x in packagePermutations:
#    newState = SwitchPackages(state.tolist(), x)
#    print("Perm:" + str(x) + " New State:"+str(newState))
#    dictStateFitness[tuple(newState)] = GetFitnessValue(newState)
#    print("Dictionary length:", len(dictStateFitness))

    

