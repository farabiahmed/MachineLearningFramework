from Representation import Representation
import os
import os.path
import numpy as np
import tensorflow as tf
from DeepQNetwork_PrioritizedReplay_Target_LearnerThread_Hybrid import DeepQNetwork_PrioritizedReplay_Target_LearnerThread_Hybrid
#from GeneticAlgorithmForDroneDelivery import GeneticAlgorithmForDroneDelivery
import GeneticAlgorithmForDroneDelivery as GA
import deliveryDroneHelper as droneHelper
import datetime

class GeneticDistributer(Representation):

    def __init__(self, 
                 gridsize=5,
                 actionspaceperagent=5,
                 numberofagent=2,
                 hidden_unit=[12,12], #for learning
                 learning_rate=0.1, #for learning
                 batch_size=32, #for learning
                 trainpass=25, #for learning
                 experiencebuffer=128, #for learning
                 modelId = "noid",
                 logfolder = "",
                 agent_model = "models/model_0_20181224_091433.h5",
                 state_dim = 7,
                 algo_mode = "search"
                 ):
        print('GeneticDistributer has just been called.')

        self.numberofagent = numberofagent
        self.state_dim = state_dim
        self.size_of_action_space = actionspaceperagent**numberofagent
        self.actionspaceforagent = actionspaceperagent
        self.algo_mode = algo_mode

        self.representation = DeepQNetwork_PrioritizedReplay_Target_LearnerThread_Hybrid   (
                gridsize            = gridsize,
                actionspaceperagent = actionspaceperagent,
                numberofagent       = 1,
                hidden_unit         = hidden_unit,
                learning_rate       = learning_rate,
                batch_size          = batch_size,
                trainpass           = trainpass,
                experiencebuffer    = experiencebuffer,
                statePreprocessType = 'Vector',
                modelId             = modelId,
                logfolder           = logfolder,
                agent_model         = agent_model
                )

        droneHelper.Init(self.numberofagent, self.state_dim)

    def Get_Action_Index(self, action):

        sizeOfAction = action.shape[0]
        temp = 0

        for i in np.arange(sizeOfAction):
            temp = temp + action[i] * (self.actionspaceforagent**(sizeOfAction-i-1))

        return temp

    def Get_Greedy_Pair(self,state):
        action = np.zeros(self.numberofagent)
        agent_model_outputs = []
        sumValue = 0
        start_whole = datetime.datetime.now()
        for i in range(self.numberofagent):
            stateMini = state[self.state_dim*i:self.state_dim*i+self.state_dim]
            arg, argValue = self.representation.Get_Greedy_Pair(stateMini)
            agent_model_outputs.append((arg, argValue))
            action[i] = arg
            sumValue += argValue
            
        # print("agent_model_outputs: ", agent_model_outputs)
        combinedValue = sumValue / self.numberofagent
        action_index = self.Get_Action_Index(action)
        finish_whole = datetime.datetime.now()
        #print(finish_whole-start_whole)

        #arg,valmax
        return action_index, combinedValue

    def Get_Greedy_Pairs(self,state):
        action = np.zeros(self.numberofagent)
        agent_model_outputs = []
        start_whole = datetime.datetime.now()
        for i in range(self.numberofagent):
            stateMini = state[self.state_dim*i:self.state_dim*i+self.state_dim]
            arg, argValue = self.representation.Get_Greedy_Pair(stateMini)
            agent_model_outputs.append((arg, argValue))
            
        # print("agent_model_outputs: ", agent_model_outputs)
        finish_whole = datetime.datetime.now()
        #print(finish_whole-start_whole)

        #arg,valmax
        return agent_model_outputs       

    def Get_Value(self,state,action):
        agent_model_outputs = []
        for i in range(self.numberofagent):
            stateMini = state[self.state_dim*i:self.state_dim*i+self.state_dim]
            value = self.representation.Get_Value(stateMini, action[i])
            agent_model_outputs.append(value)           
            sumValue += value
        combinedValue = sumValue / self.numberofagent
        return combinedValue

    def Set_Value(self,state,action,value):
        self.representation.Set_Value(state, action, float(value))
        return

    def Set_Values(self,state,action,value):

        for i in range(self.numberofagent):
            stateMini = state[self.state_dim*i:self.state_dim*i+self.state_dim]
            self.representation.Set_Value(stateMini, np.array([action[i]]), float(value[i]))

        return

    def Add_Experience(self,state,action,nextstate,reward,status):
        raise NotImplementedError()

    def Save_Model(self):
        pass

    # i = 0
    isDone = True

    def Initial_State(self,state, value):                
        
        #print("Initial_State -> state: ", state, " value: ", value, " isdone: ", self.isDone)

        if self.algo_mode=="static":
            self.isDone = True
            newState = state
        elif self.algo_mode=="random":
            self.isDone = True
            newState = droneHelper.RandomPacketSwitch(state.tolist())
        elif self.algo_mode=="search":
            if self.isDone:
                droneHelper.Init(self.numberofagent, self.state_dim)
            
            self.isDone, newState = droneHelper.Process(state.tolist(), value)
        elif self.algo_mode=="genetic":
            self.isDone = True
            newState = state
        else:
            self.isDone = True
            newState = state


                # print('Initial_State: ', state, " Fitness:", value)

                #GA = GeneticAlgorithmForDroneDelivery()
                # GA.init()

                #self.i+=1
                #self.isDone = False
                #if self.i>0:
                #   self.isDone = True
                #   self.i = 0
                
                # print(GA.test())

        #print("Initial_State -> isDone: ", self.isDone, " newState: ", newState)
        
        return newState, self.isDone