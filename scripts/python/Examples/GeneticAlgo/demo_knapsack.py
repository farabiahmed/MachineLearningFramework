import genetic_algorithm

things = [
    Thing('Laptop', 500, 2200),
    Thing('Headphones', 150, 160),
    Thing('Coffee Mug', 60, 350),
    Thing('Notepad', 40, 333),
    Thing('Water Bottle', 30, 192),
]

more_things = [
    Thing('Mints', 5, 25),
    Thing('Socks', 10, 38),
    Thing('Tissues', 15, 80),
    Thing('Phone', 500, 200),
    Thing('Baseball Cap', 100, 70),
] + things

# Function to generate new solutions
def generate_population(size: int, genome_length: int) -> Population:
    return [generate_genome(genome_length) for _ in range(size)]


# Fitness function
def fitness(genome: Genome, things: [Thing], weight_limit: int) -> int:
    if len(genome) != len(things):
        raise ValueError("genome and things must be of the same length")

    weight = 0
    value = 0

    for i, thing in enumerate(things):
        if(genome[i] == 1):
            weight += thing.weight
            value += thing.value

            if weight > weight_limit:
                return 0
    return value

def genome_to_things(genome: Genome, things: [Thing]) -> [Thing]:
    result = []
    weight = 0
    for i, thing in enumerate(things):
        if genome[i] == 1:
            result+=[thing.name]
            weight+=thing.weight

    return result, weight
    
start = time.time()
population, generations = run_evaluation(
    populate_func=partial(
        generate_population, size=10, genome_length=len(more_things)
    ),
    fitness_func=partial(
        fitness, things=more_things, weight_limit=3000
    ),
    fitness_limit=1310, #740
    generation_limit=100
)
end = time.time()

print(f"number of generations: {generations}")
print(f"time: {end - start}")
print(f"best solution: {genome_to_things(population[0], more_things)}")
