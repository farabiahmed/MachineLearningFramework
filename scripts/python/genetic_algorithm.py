import numpy as np
from matplotlib.pyplot import *
from random import randint, randrange, random
from typing import List, Callable, Tuple
from functools import partial
from custom_random import choices

# Types
Genome = List[int]
Population = List[Genome]
FitnessValues = List[float]
Representation_Type = str

# Seperation of concerns for a better architecture, sperate the problem and algorithm, generalize the work done here.
FitnessFunc = Callable[[Genome], float]  # gets the genome and returns the fitness value to make the correct choice.
PopulateFunc = Callable[[], Population] # takes nothing but spits out new solutions
SelectionFunc = Callable[[Population, FitnessValues], Tuple[Genome, Genome]] # takes population and fitnessfunc to select two solutions to be the parents of our next solution
CrossoverFunc = Callable[[Genome, Genome],Tuple[Genome, Genome]]
MutationFunc = Callable[[Genome], Genome]


population = Population
populate_func = PopulateFunc
fitness_func = FitnessFunc
fitness_limit = int
crossover_func = CrossoverFunc
selection_func = SelectionFunc 
mutation_func = MutationFunc
generation_limit = int
current_generation = 0


# Selection Function: a select function to select the solutions to generate the next generation.
# the selected solutions will be parents of the next generation
# solutions with the higher fitness should be more likely be chosen for the reproduction.
def selection_pair(population: Population, fitnessvals: FitnessValues) -> Population:    
    return choices(
        population, 
        weights=fitnessvals,
        k = 2
    )   


# mutation
def mutation(genome: Genome, num: int=1, probability: float = 0.5) -> Genome:
    global Representation_Type

    if Representation_Type == "binary": 
        for _ in range(num):
            index = randrange(len(genome))
            genome[index] = genome[index] if random() > probability else abs(genome[index]-1)
    elif Representation_Type == "permutation":
        for _ in range(num):
            if random() > probability:
                index = randrange(len(genome))
                value1 = genome[index]
                genome[index], genome[value1] = genome[value1], genome[index]
    return genome

def sortPopulation(population:Population, fitnessvals: FitnessValues) -> Population:
    sorted_zipped = sorted(zip(fitnessvals, population), reverse=True)      
    fitnessvals, population = list(zip(*sorted_zipped))
    population = list(population)
    return population, fitnessvals

def init(
    _populate_func: PopulateFunc,
    _fitness_limit: int,
    _crossover_func: CrossoverFunc,
    _selection_func: SelectionFunc = selection_pair,    
    _mutation_func: MutationFunc = mutation,
    _generation_limit: int = 100,
    _representation_type: str = "binary" #binary, order
) -> Population:
    
    global Representation_Type
    global populate_func
    global fitness_limit
    global crossover_func
    global selection_func
    global mutation_func
    global generation_limit
    global current_generation
    global population

    populate_func     =_populate_func 
    fitness_limit    =_fitness_limit
    crossover_func    =_crossover_func
    selection_func    =_selection_func
    mutation_func    =_mutation_func
    generation_limit    =_generation_limit


    Representation_Type = _representation_type

    population = populate_func()

    current_generation = 0

    return population

def process(fitnessvals) -> Tuple[Population, int, bool]:

    global Representation_Type
    global populate_func
    global fitness_func
    global fitness_limit
    global crossover_func
    global selection_func
    global mutation_func
    global generation_limit
    global current_generation
    global population

    if current_generation < generation_limit:
        
        current_generation+=1

        # fitnessvals = calculateFitness(population, fitness_func)
     
        population, fitnessvals = sortPopulation(population, fitnessvals)
        
        if fitnessvals[0] >= fitness_limit:
            return population, current_generation, True

        next_generation = population[0:2]

        for j in range(int(len(population)/2)-1):
            parents = selection_pair(population, fitnessvals)
            offspring_a, offspring_b = crossover_func(parents[0], parents[1])
            offspring_a = mutation_func(offspring_a)
            offspring_b = mutation_func(offspring_b)
            next_generation += [offspring_a, offspring_b]

        population = next_generation

        if current_generation > 18:
            print("Whatta")
        
    else:
        population, fitnessvals = sortPopulation(population, fitnessvals)
        return population, current_generation, True

    return population, current_generation, False
