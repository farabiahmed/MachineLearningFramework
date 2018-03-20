import Memory, random
from collections import deque

class Memory_UniformRandom:

    def __init__(self, capacity):
        self.memory = deque(maxlen=capacity)

    def add(self, sample):
        self.memory.add(sample)

    def sample(self, n):
        minibatch = random.sample(list(self.memory), n)
        return minibatch

    def update(self, data):
        raise NotImplementedError()

    def length(self):
        return len(self.memory)