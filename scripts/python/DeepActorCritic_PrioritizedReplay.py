
import os.path
import random
from time import sleep

import numpy as np
import tensorflow as tf
from keras.models import Sequential
from keras.layers import *
from keras.optimizers import Adam

from Memory.Memory_SumTree import Memory_SumTree
from Memory import Memory_UniformRandom

from Representation import Representation


class DeepActorCritic_PrioritizedReplay(Representation):

    def __init__(self,gridsize=5,actionspaceperagent=5,numberofagent=2,
                 actor_hidden_unit=[12,12],
                 critic_hidden_unit=[12,12],
                 actor_learning_rate=0.01,
                 critic_learning_rate=0.1,
                 batch_size=32,trainpass=25,experiencebuffer=128,
                 train_period=1,
                 gamma = 0.99,
                 model_reset_counter=32,
                 statePreprocessType = "Tensor",
                 convolutionLayer= False,
                 modelId = "noid",
                 logfolder = "no_time_stamp"):

        self.Gamma = gamma
        self.batchsize = batch_size
        self.trainPass = trainpass
        self.actor_hidden_unit = actor_hidden_unit
        self.critic_hidden_unit = critic_hidden_unit
        self.actorlearningrate = actor_learning_rate
        self.criticlearningrate = critic_learning_rate
        self.statePreprocessType = statePreprocessType
        self.convolutionLayer = convolutionLayer

        if(statePreprocessType=="Tensor") :
            self.size_of_input_units = gridsize * gridsize * numberofagent
        elif (statePreprocessType=="Vector"):
            self.size_of_input_units = 2 * numberofagent; # (x,y,a) for each agent
        self.gridsize = gridsize

        self.memory = Memory_SumTree(experiencebuffer)

        self.fresh_experience_counter = 0
        self.actionspaceforagent = actionspaceperagent
        self.numberofagent = numberofagent
        self.output_unit = actionspaceperagent**numberofagent
        self.trainingepochtotal = 0
        self.train_period = train_period # After how many new experience we will run fitting/training.
        self.counter_experience = 0 # A counter to hold how many tuple experienced
        self.counter_modelReset = model_reset_counter
        self.modelId = modelId
        self.logfolder = logfolder
        
        # Build Actor Model
        self.model_actor = Sequential()
        
        self.model_actor.add(Dense(self.actor_hidden_unit[0], activation='relu', input_dim = self.size_of_input_units))

        for i in range(1, len(actor_hidden_unit)):
            self.model_actor.add(Dense(self.actor_hidden_unit[i], activation='relu',kernel_initializer='he_uniform'))
        self.model_actor.add(Dense(self.output_unit, activation='softmax', kernel_initializer='he_uniform'))

        # Build Critic Model
        self.model_critic = Sequential()
        
        self.model_critic.add(Dense(self.critic_hidden_unit[0], activation='tanh', input_dim = self.size_of_input_units ))

        for i in range(1, len(critic_hidden_unit)):
            self.model_critic.add(Dense(self.critic_hidden_unit[i], activation='tanh'))
        self.model_critic.add(Dense(1, activation=LeakyReLU(0.3)))
        
        # Compile model
        self.model_actor.compile(loss='categorical_crossentropy', optimizer=Adam(lr=self.actorlearningrate))
        #self.model_actor._make_predict_function()

        self.model_critic.compile(loss='mse', optimizer=Adam(lr=self.criticlearningrate))
        #self.model_critic._make_predict_function()
        
        #save the TensorFlow graph:
        self.graph = tf.get_default_graph()

        self.model_actor.summary()
        print("Model Output Shape")
        print(self.model_actor.output_shape[1])
        sleep(1)
        
        self.model_critic.summary()
        print("Model Output Shape")
        print(self.model_critic.output_shape[1])
        sleep(1)

        if os.path.isfile("log/"+self.logfolder+"/model_actor_trained.h5") and os.path.isfile("log/"+self.logfolder+"/model_critic_trained.h5"):
            self.model_actor.load_weights("log/"+self.logfolder+"/model_actor_trained.h5")
            self.model_critic.load_weights("log/"+self.logfolder+"/model_critic_trained.h5")
            print("###############################")
            print("Existing model loaded.......")
            print("###############################")

        self.model_actor.save_weights("log/"+self.logfolder+"/model_actor_init_"+self.modelId+".h5")
        self.model_critic.save_weights("log/"+self.logfolder+"/model_critic_init_"+self.modelId+".h5")

        # Reset the batch
        self.Reset_Batch()

    def Convert_State_To_Input(self,state):

        if(self.statePreprocessType=="Tensor") :
            # Create zero filled output value
            outValue = np.zeros(shape=(self.numberofagent, self.gridsize, self.gridsize))

            # Convert list to numpy array
            state = np.reshape(state, (1, state.shape[0]))

            # Decompose states
            states = np.split(state[0], self.numberofagent)

            # Fill output tensor with agents position
            for i in range(self.numberofagent):
                row = int(states[i][0])
                col = int(states[i][1])
                outValue[i, row, col] = 1

            if self.convolutionLayer==True:
                return outValue
            else:
                return outValue.flatten()

        elif (self.statePreprocessType=="Vector"):
            return state



    def Get_Greedy_Pair(self,state):
        # Backward compability: Preprocess state input if needed.
        if np.size(state) != self.size_of_input_units:
            values = self.ForwardPass(self.model_actor, self.Convert_State_To_Input(state))
        else:
            values = self.ForwardPass(self.model_actor, state)

        # TODO: Add SoftMax Here
        # Get the maximums
        arg = values.argmax()
        valmax = values.max()

        return arg,valmax

    def Get_Value(self,state,action):

        
        #value = self.ForwardPass(self.model_critic, self.Convert_State_To_Input(state))#TODO
        value = self.ForwardPass(self.model_actor, self.Convert_State_To_Input(state))
        
        #index = self.Get_Action_Index(action)
        #temp = values[index]

        return value[0]


    def ForwardPass(self,model, input):
        # Form Input Values
        if self.convolutionLayer == True:
            input = np.reshape(input, (1, input.shape[0], input.shape[1], input.shape[2]))
        else:
            input = np.reshape(input,(1,input.shape[0]))

        # Prediction of the model
        with self.graph.as_default():
            hypothesis = model.predict(input)

        values = np.asarray(hypothesis).reshape(model.output_shape[1])

        return values


    def Get_Action_Index(self, action):
        sizeOfAction = action.shape[0]
        temp = 0

        for i in np.arange(sizeOfAction):
            temp = temp + action[i] * (self.actionspaceforagent**(sizeOfAction-i-1))

        return temp;


    def Set_Value(self,state,action,value):

        # Preprocess State
        state = self.Convert_State_To_Input(state)
        
        #actor = np.zeros(self.output_unit)
        
        # Update label
        actor = self.ForwardPass(self.model_actor, state)
        index = self.Get_Action_Index(action)

        # Calculate error for Prioritized Experience Replay
        #critic_value = self.Get_Value(state, action)#TODO
        #error = value - critic_value#TODO
        error = actor[index] - value

        # Calculate Label for Actor
        #actor[index] = 1 if error > 0 else 0
        #actor[index] = actor[index] + self.learningrate * error #TODO
        #actor[index] = -error
        actor[index] = value
        
        # Calculate Label for Critic
        #critic = critic_value + self.learningrate * error
        critic = value#TODO
        
        # Append new sample to Memory of Experiences
        # Don't worry about its size, since it is a queue
        self.memory.add(abs(error),(state, actor, critic))

        #if self.fresh_experience_counter == self.batchsize :
        if self.memory.length() >= self.batchsize :

            self.trainingepochtotal += self.trainPass
            # print('Training Epoch:', self.trainingepochtotal)

            # Get Unique Samples from memory as much as batchsize
            minibatch = self.memory.sample(self.batchsize)

            batchSamplesX = [] # states
            batchSamplesY = [] # actor values
            batchSamplesZ = [] # critic

            for i in np.arange(len(minibatch)):
                idx, (X, Y, Z) = minibatch[i]
                batchSamplesX.append(X)
                batchSamplesY.append(Y)
                batchSamplesZ.append(Z)
            
            with tf.device('/gpu:0'):   
                with self.graph.as_default():
                    self.model_actor.fit(np.array(batchSamplesX), np.array(batchSamplesY), epochs=self.trainPass, batch_size= self.batchsize, verbose=0)
                    self.model_critic.fit(np.array(batchSamplesX), np.array(batchSamplesZ), epochs=self.trainPass, batch_size= self.batchsize, verbose=0)

            # for i in np.arange(len(minibatch)):
            #     idx, (X, Y) = minibatch[i]
            #     self.batchSamplesX = np.vstack((self.batchSamplesX, (X)))
            #     self.batchSamplesY = np.vstack((self.batchSamplesY, Y))
            #
            # with self.graph.as_default():
            #     self.model_actor.fit(self.batchSamplesX, self.batchSamplesY, epochs=self.trainPass, batch_size= self.batchsize, verbose=0)
            # self.Reset_Batch()

    def Reset_Batch(self):
        # Reset the batch
        if self.convolutionLayer == True :
            self.batchSamplesX = np.array([], dtype=np.float).reshape(0, self.numberofagent, self.gridsize, self.gridsize)
            self.batchSamplesY = np.array([], dtype=np.float).reshape(0, self.output_unit)
        else:
            self.batchSamplesX = np.array([], dtype=np.float).reshape(0, self.size_of_input_units)
            self.batchSamplesY = np.array([], dtype=np.float).reshape(0, self.output_unit)

    def Add_Experience(self,state,action,nextstate,reward,status):

        # WORKING
        arg_Qmax, Qmax = self.Get_Greedy_Pair(nextstate)
        QValue = reward + self.Gamma * Qmax

        self.Set_Value(state,action,QValue)

    def Save_Model(self):
        with self.graph.as_default():
            self.model_critic.save_weights("log/"+self.logfolder+"/model_critic_Output_"+self.modelId+".h5")
            self.model_actor.save_weights("log/"+self.logfolder+"/model_actor_Output_"+self.modelId+".h5")
            print("###############################")
            print("Model saved.......")
            print("###############################")

    def __del__(self):
        self.Save_Model()
        print('Representation object died.')
