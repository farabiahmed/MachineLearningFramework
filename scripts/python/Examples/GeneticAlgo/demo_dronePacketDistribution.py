import time
from genetic_algorithm import Genome, Population, init, process
from collections import namedtuple
from functools import partial
from custom_random import choices
import random
import math  # for factorial
from typing import Tuple
from random import randint, randrange, random
from itertools import chain 

numberofagent = 15
packageIds = []
factorialTable = [] # holds all factorials for efficiency

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

# Genetic representation of a solution
def generate_genome(length: int) -> Genome:

    global packageIds, numberofagent, factorialTable
    randomPermutationIndex = randint(0, factorialTable[numberofagent-1])
    packetOrder = getNthPermutation(randomPermutationIndex, packageIds.copy(), [])

    return packetOrder

# Function to generate new solutions
def generate_population(size: int, genome_length: int) -> Population:
    return [generate_genome(genome_length) for _ in range(size)]

# Fitness function
def fitness(genome: Genome) -> int:    
    target_genome = list(range(len(genome)))
    diff = [abs(a - b) for a, b in zip(genome, target_genome)]
    fitness = -sum(diff)
    return fitness

def ordered_crossover(mum: Genome, dad: Genome) -> Tuple[Genome, Genome]:
    """Implements ordered crossover"""

    size = len(mum)

    # Choose random start/end position for crossover
    alice, bob = [-1] * size, [-1] * size
    start, end = sorted([randrange(size) for _ in range(2)])
    # print("")
    # print(mum, dad)
    # print(start, end)
    
    # Replicate mum's sequence for alice, dad's sequence for bob
    for i in range(start, end + 1):
        alice[i] = mum[i]
        bob[i] = dad[i]

    # # Fill the remaining position with the other parents' entries
    current_dad_position, current_mum_position = 0, 0
    #
    for i in chain(range(start), range(end + 1, size)):
    #
         while dad[current_dad_position] in alice:
             current_dad_position += 1
    #
         while mum[current_mum_position] in bob:
             current_mum_position += 1
    #
         alice[i] = dad[current_dad_position]
         bob[i] = mum[current_mum_position]
    #
    # print(alice, bob)
    # # Return twins
    return alice, bob

def calculateFitness(population:Population):
    return [fitness(p) for p in population]

factorialTable = [math.factorial(i) for i in range(numberofagent+1)]
packageIds = list(range(numberofagent))

start = time.time()
population = init(
    _populate_func=partial(
        generate_population, size=10, genome_length=numberofagent
    ),
    _crossover_func=ordered_crossover,
    _fitness_limit=-1,
    _generation_limit=20,
    _representation_type="permutation"
)

isDone = False
 
while isDone == False:
    fitnessvals = calculateFitness(population)
    population, generations, isDone = process(fitnessvals)

end = time.time()

print("number of generations: " + str(generations))
print("time: " + str(end - start))
[print(str(population[i]) + " " + str(fitness(population[i]))) for i in range(len(population))]