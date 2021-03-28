import numpy as np
from matplotlib.pyplot import *
from random import randint, randrange, random
from typing import List, Callable, Tuple
from functools import partial
from custom_random import choices

Genome = List[int]
Population = List[Genome]
Representation_Type = str

# Seperation of concerns for a better architecture, sperate the problem and algorithm, generalize the work done here.
FitnessFunc = Callable[[Genome], int]  # gets the genome and returns the fitness value to make the correct choice.
PopulateFunc = Callable[[], Population] # takes nothing but spits out new solutions
SelectionFunc = Callable[[Population, FitnessFunc], Tuple[Genome, Genome]] # takes population and fitnessfunc to select two solutions to be the parents of our next solution
CrossoverFunc = Callable[[Genome, Genome],Tuple[Genome, Genome]]
MutationFunc = Callable[[Genome], Genome]

# Selection Function: a select function to select the solutions to generate the next generation.
# the selected solutions will be parents of the next generation
# solutions with the higher fitness should be more likely be chosen for the reproduction.
def selection_pair(population: Population, fitness_func: FitnessFunc) -> Population:
    
    w = [fitness_func(genome) for genome in population]
    
    return choices(
        population=population,
        weights=w,
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

def run_evaluation(
    populate_func: PopulateFunc,
    fitness_func: FitnessFunc,
    fitness_limit: int,
    crossover_func: CrossoverFunc,
    selection_func: SelectionFunc = selection_pair,    
    mutation_func: MutationFunc = mutation,
    generation_limit: int = 100,
    representation_type: str = "binary" #binary, order
) -> Tuple[Population, int]:
    
    global Representation_Type

    Representation_Type = representation_type

    population = populate_func()

    for i in range(generation_limit):
        population = sorted(
            population,
            key=lambda genome: fitness_func(genome),
            reverse=True
            )

        if fitness_func(population[0]) >= fitness_limit:
            break;

        next_generation = population[0:2]

        for j in range(int(len(population)/2)-1):
            parents = selection_pair(population, fitness_func)
            offspring_a, offspring_b = crossover_func(parents[0], parents[1])
            offspring_a = mutation_func(offspring_a)
            offspring_b = mutation_func(offspring_b)
            next_generation += [offspring_a, offspring_b]

        population = next_generation
    
    population = sorted(
        population,
        key=lambda genome: fitness_func(genome),
        reverse=True
        )

    return population, i
