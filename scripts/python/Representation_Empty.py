import os
import os.path
import random

from Representation import Representation

class Representation_Empty(Representation):

    def __init__(self):

        print("###############################")
        print("Representation Empty")
        print("###############################")

    def Get_Greedy_Pair(self,state):
        return 0,0

    def Get_Value(self,state,action):
        return 0

    def Set_Value(self,state,action,value):
        return 0

    def Add_Experience(self,state,action,nextstate,reward,status):
        pass
    
    def Save_Model(self):
        pass
    
    def __del__(self):
        print('Representation object died.')
