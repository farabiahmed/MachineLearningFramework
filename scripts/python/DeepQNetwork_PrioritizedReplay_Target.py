
import os
import os.path
import random

import numpy as np
import tensorflow as tf
from keras.models import Sequential
from keras.layers.advanced_activations import LeakyReLU
from keras.layers import *
from keras.models import load_model
from keras.models import clone_model
from Memory.Memory_SumTree import Memory_SumTree
from Memory import Memory_UniformRandom

from Representation import Representation


class DeepQNetwork_PrioritizedReplay_Target(Representation):

    def __init__(self,gridsize=5,actionspaceperagent=5,numberofagent=2,
                 hidden_unit=[12,12],learning_rate=0.1,
                 batch_size=32,trainpass=25,experiencebuffer=128,
                 train_period=1,
                 gamma = 0.99,
                 model_reset_counter=32,
                 statePreprocessType = "Tensor",
                 convolutionLayer= False,
                 modelId = "noid",
                 logfolder = ""):

        print("###############################")
        print("DeepQNetwork_PrioritizedReplay_Target")
        print("###############################")

        self.Gamma = gamma
        self.batchsize = batch_size
        self.trainPass = trainpass
        self.hidden_unit = hidden_unit
        self.learningrate = learning_rate
        self.statePreprocessType = statePreprocessType
        self.convolutionLayer = convolutionLayer

        if(statePreprocessType=="Tensor") :
            self.size_of_input_units = gridsize * gridsize * numberofagent
        elif (statePreprocessType=="Vector"):
            self.size_of_input_units = 7 * numberofagent; # (x,y,a) for each agent
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

        self.update_target_interval = 10000

        self.model = None
        self.model_target = None

        print("log/" + logfolder + "/model_0.h5")
        if os.path.isfile("log/"+self.logfolder+"/model_0.h5"):
            print("###############################")
            print("Existing model is being loaded.......")
            print("###############################")
            self.model = load_model("log/" + self.logfolder + "/model_0.h5")
        else :
            print("###############################")
            print("Not Any Existing model found.......")
            print("###############################")


        if self.model is None:

            # create model
            self.model = Sequential()

            if self.convolutionLayer==True:
                self.convolution_input = ( self.numberofagent, self.gridsize, self.gridsize)
                self.model.add(Conv2D(16, (2, 2), strides=(1, 1), activation='relu', input_shape=(self.convolution_input),
                                 data_format='channels_first'))
                self.model.add(Conv2D(32, (2, 2), strides=(1, 1), activation='relu'))
                #self.model.add(Conv2D(64, (3, 3), activation='relu'))
                self.model.add(Flatten())
                self.model.add(Dense(self.hidden_unit[0], activation='tanh'))
            else:
                self.model.add(Dense(self.hidden_unit[0], activation='tanh', input_dim = self.size_of_input_units ))

            for i in range(1, len(hidden_unit)):
                self.model.add(Dense(self.hidden_unit[i], activation='tanh'))
            self.model.add(Dense(self.output_unit, activation='relu'))

            # Compile model
            self.model.compile(loss='mse', optimizer='sgd', metrics=['accuracy'])
            self.model._make_predict_function()

            if os.path.isfile("log/"+self.logfolder+"/model_weight_0.h5"):
                self.model.load_weights("log/"+self.logfolder+"/model_weight_0.h5")
                print("###############################")
                print("Existing model params are loaded.......")
                print("###############################")

        # save the TensorFlow graph:
        self.graph = tf.get_default_graph()

        self.model.summary()

        self.model_target = clone_model(self.model)
        self.Update_target()

        # self.Save_Model()
        self.model.save_weights("log/"+self.logfolder+"/modelinit_"+self.modelId+".h5")

        # Reset the batch
        self.Reset_Batch()

    def Update_target(self):
        print('Updating Target Network')
        model_weights = self.model.get_weights()
        self.model_target.set_weights(model_weights)

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
            values = self.ForwardPass(self.Convert_State_To_Input(state))
        else:
            values = self.ForwardPass(state)

        # Get the maximums
        arg = values.argmax()
        valmax = values.max()

        return arg,valmax

    def Get_Value(self,state,action):

        values = self.ForwardPass(self.Convert_State_To_Input(state))
        index = self.Get_Action_Index(action)
        temp = values[index]

        return temp


    def ForwardPass(self,input):

      # Form Input Values
        if self.convolutionLayer == True:
            input = np.reshape(input, (1, input.shape[0], input.shape[1], input.shape[2]))
        else:
            input = np.reshape(input,(1,input.shape[0]))

        # Prediction of the model
        with self.graph.as_default():
            hypothesis = self.model_target.predict(input)

        values = np.asarray(hypothesis).reshape(self.output_unit)

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

        # Update label
        values = self.ForwardPass(state)
        index = self.Get_Action_Index(action)

        # Calculate error for Prioritized Experience Replay
        error = abs(values[index] - value)

        values[index] = value

        # Append new sample to Memory of Experiences
        # Don't worry about its size, since it is a queue
        self.memory.add(error,(state, values))

        #if self.fresh_experience_counter == self.batchsize :
        if self.memory.length() >= self.batchsize :

            self.trainingepochtotal += self.trainPass
            # print('Training Epoch:', self.trainingepochtotal)

            # Get Unique Samples from memory as much as batchsize
            minibatch = self.memory.sample(self.batchsize)

            batchSamplesX = []
            batchSamplesY = []

            for i in np.arange(len(minibatch)):
                idx, (X, Y) = minibatch[i]
                batchSamplesX.append(X)
                batchSamplesY.append(Y)

            with self.graph.as_default():
                self.model.fit(np.array(batchSamplesX), np.array(batchSamplesY), epochs=self.trainPass, batch_size= self.batchsize, verbose=0)

            if not self.trainingepochtotal % self.update_target_interval:
                self.Update_target()
            # for i in np.arange(len(minibatch)):
            #     idx, (X, Y) = minibatch[i]
            #     self.batchSamplesX = np.vstack((self.batchSamplesX, (X)))
            #     self.batchSamplesY = np.vstack((self.batchSamplesY, Y))
            #
            # with self.graph.as_default():
            #     self.model.fit(self.batchSamplesX, self.batchSamplesY, epochs=self.trainPass, batch_size= self.batchsize, verbose=0)
            # self.Reset_Batch()
            
        return self.trainingepochtotal
    
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

            if not os.path.exists("log/"+self.logfolder):
                os.makedirs("log/"+self.logfolder)

            self.model.save("log/" + self.logfolder + "/model_" + self.modelId + ".h5")
            self.model.save_weights("log/"+self.logfolder+"/model_weight_"+self.modelId+".h5")
            print("###############################")
            print("Model saved: " + "log/" + self.logfolder)
            print("###############################")

    def __del__(self):
        self.Save_Model()
        print('Representation object died.')
