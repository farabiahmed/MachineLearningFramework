import numpy as np
from DeepActorCritic_PrioritizedReplay_tflearn import DeepActorCritic_PrioritizedReplay_tflearn
from Representation import Representation


rep = DeepActorCritic_PrioritizedReplay_tflearn        (gridsize            = 5,
                                                        actionspaceperagent = 5,
                                                        numberofagent       = 1)    


state = np.array([2,4])
print(state)
arg, val = rep.Get_Greedy_Pair(state)
print(arg)
print(val)                                                