from abc import ABCMeta, abstractmethod

class Representation(metaclass=ABCMeta):
    @abstractmethod
    def Get_Greedy_Pair(self,state):
        raise NotImplementedError()

    @abstractmethod
    def Get_Value(self,state,action):
        raise NotImplementedError()

    @abstractmethod
    def Set_Value(self,state,action,value):
        raise NotImplementedError()

    @abstractmethod
    def Add_Experience(self,state,action,nextstate,reward,status):
        raise NotImplementedError()