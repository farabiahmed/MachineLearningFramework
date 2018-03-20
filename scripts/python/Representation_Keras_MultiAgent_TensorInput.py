
import tensorflow as tf
import numpy as np
import random
from Representation import Representation
from collections import deque
from keras.models import Sequential
from keras.layers import Dense
from keras.layers.advanced_activations import LeakyReLU
import os.path
import h5py
import time

class Representation_Keras_MultiAgent_TensorInput(Representation):

    def __init__(self,gridsize=5,actionspaceperagent=5,numberofagent=2,hidden_unit=[12,12],learning_rate=0.1,batch_size=10,trainpass=1,experiencebuffer=100):

        self.batchsize = batch_size
        self.trainPass = trainpass
        self.hidden_unit = hidden_unit
        self.learningrate = learning_rate
        self.size_of_input_units = gridsize * gridsize * numberofagent
        self.gridsize = gridsize
        self.memory = deque(maxlen=experiencebuffer)
        self.fresh_experience_counter = 0
        self.actionspaceforagent = actionspaceperagent
        self.numberofagent = numberofagent
        self.output_unit = actionspaceperagent**numberofagent
        self.trainingepochtotal = 0

        # create model
        self.model = Sequential()
        self.model.add(Dense(self.hidden_unit[0], activation='tanh', input_dim = self.size_of_input_units ))
        for i in range(1, len(hidden_unit)):
            self.model.add(Dense(self.hidden_unit[i], activation='tanh'))
        self.model.add(Dense(self.output_unit, activation=LeakyReLU(0.3)))

        # Compile model
        self.model.compile(loss='mse', optimizer='sgd', metrics=['accuracy'])
        self.model._make_predict_function()

        #save the TensorFlow graph:
        self.graph = tf.get_default_graph()

        self.model.summary()

        if os.path.isfile("modeltrained.h5"):
            self.model.load_weights("modeltrained.h5")
            print("###############################")
            print("Existing model loaded.......")
            print("###############################")

        self.model.save_weights("modelinit.h5")

        # Reset the batch
        self.batchSamplesX = np.array([], dtype=np.float).reshape(0, self.size_of_input_units)
        self.batchSamplesY = np.array([], dtype=np.float).reshape(0, self.output_unit)

    def Convert_State_To_Input(self,state):

        # Create zero filled output value
        outValue = np.zeros(shape=(self.numberofagent,self.gridsize,self.gridsize))

        # Convert list to numpy array
        state = np.reshape(state,(1,state.shape[0]))

        # Decompose states
        states = np.split(state[0],self.numberofagent)

        # Fill output tensor with agents position
        for i in range(self.numberofagent) :
            row = int(states[i][0])
            col = int(states[i][1])
            outValue[i,row,col] = 1

        return outValue.flatten()

    def Get_Greedy_Pair(self,state):

        values = self.ForwardPass(self.Convert_State_To_Input(state))

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
        input = np.reshape(input,(1,input.shape[0]))

        # Prediction of the model
        with self.graph.as_default():
            hypothesis = self.model.predict(input)

        values = np.asarray(hypothesis).reshape(self.output_unit)

        return values

    def Set_Value(self,state,action,value):

        #time_Convert_State_To_Input = time.time() 
        inputVal = self.Convert_State_To_Input(state)
        #print("Convert_State_To_Input: %s seconds" % (time.time() - time_Convert_State_To_Input)) # 7.677078247070312e-05 seconds


        #time_ForwardPass = time.time()
        values = self.ForwardPass(inputVal)
        #print("ForwardPass: %s seconds" % (time.time() - time_ForwardPass)) # 0.001142740249633789 seconds

        #time_Get_Action_Index = time.time()
        index = self.Get_Action_Index(action)
        values[index] = value
        #print("Get_Action_Index: %s seconds" % (time.time() - time_Get_Action_Index)) #0.12781143188476562 seconds




        #time_Prepare_Fit = time.time()
        # Rearrange the given input and output to make it appropriate for our NN
        sampleX = inputVal
        sampleY = values
        #np.array([value],dtype=np.float)

        # Fit the model
        #state = np.reshape(state, (1, state.shape[0]))
        #values = np.reshape(values, (1, values.shape[0]))

        # Increase counter to ensure enough new samples are gathered
        # self.fresh_experience_counter+=1

        # Append new sample to Memory of Experiences
        # Don't worry about its size, since it is a queue
        self.memory.append((sampleX, sampleY))

        #if self.fresh_experience_counter == self.batchsize :
        if len(self.memory) >= self.batchsize :

            self.trainingepochtotal += self.trainPass
            #print('Training Epoch:', self.trainingepochtotal)

            # Reset the counter
            # self.fresh_experience_counter=0

            # Get Unique Samples from memory as much as batchsize
            minibatch = random.sample(list(self.memory), self.batchsize)

            for i in np.arange(len(minibatch)):
                X, Y = minibatch[i]
                self.batchSamplesX = np.vstack((self.batchSamplesX, X))
                self.batchSamplesY = np.vstack((self.batchSamplesY, Y))
        
            #time_Fit = time.time()
            with self.graph.as_default():
                self.model.fit(self.batchSamplesX, self.batchSamplesY, epochs=self.trainPass, batch_size= self.batchsize, verbose=0)
            #print("Get_Fit: %s seconds" % (time.time() - time_Fit)) #0.12662458419799805 seconds



            # Reset the batch
            self.batchSamplesX = np.array([], dtype=np.float).reshape(0, self.size_of_input_units)
            self.batchSamplesY = np.array([], dtype=np.float).reshape(0, self.output_unit)
        
        #print("Get_Prepare_Fit: %s seconds" % (time.time() - time_Prepare_Fit)) #0.12781143188476562 seconds



        # # Increase counter to ensure enough new samples are gathered
        # self.fresh_experience_counter+=1
        #
        # # Append new sample to Memory of Experiences
        # # Don't worry about its size, since it is a queue
        # self.memory.append((sampleX, sampleY))
        #
        # if self.fresh_experience_counter == self.batchsize :
        #
        #     # Reset the counter
        #     self.fresh_experience_counter=0
        #
        #     # Get Unique Samples from memory as much as batchsize
        #     minibatch = random.sample(list(self.memory), self.batchsize)
        #
        #     for i in np.arange(len(minibatch)):
        #         X, Y = minibatch[i]
        #         self.batchSamplesX = np.vstack((self.batchSamplesX, X))
        #         self.batchSamplesY = np.vstack((self.batchSamplesY, Y))
        #
        #     print('Training Batch... ')
        #
        #     # Fit the model
        #     self.model.fit(self.batchSamplesX, self.batchSamplesY, epochs=self.trainPass, batch_size=1, verbose=0)
        #
        #     # Reset the batch
        #     self.batchSamplesX = np.array([], dtype=np.float).reshape(0, self.size_of_input_units)
        #     self.batchSamplesY = np.array([], dtype=np.float).reshape(0, self.output_unit)

    #TODO: Modify it for multiagent case
    def Get_Action_Index(self, action):

        #print("action:", action) #action: [1 4 4]
        #print(type(action)) #< class 'numpy.ndarray'>
        #print(action.shape) #(3,)
        #
        # if action.shape[0]==1 :
        #     temp = action[0]
        #
        # elif action.shape[0]==2 :
        #     temp = action[0] * self.actionspaceforagent + action[1]
        #
        # elif action.shape[0]==3 :
        #     temp = action[0] * (self.actionspaceforagent**2) \
        #            + action[1] * (self.actionspaceforagent**1) \
        #            + action[2] * (self.actionspaceforagent**0)

        sizeOfAction = action.shape[0]
        temp = 0

        for i in np.arange(sizeOfAction):
            temp = temp + action[i] * (self.actionspaceforagent**(sizeOfAction-i-1))

        return temp;

    def Save_Model(self):
        with self.graph.as_default():
            self.model.save_weights("modelOutput.h5")
            print("###############################")
            print("Model saved.......")
            print("###############################")

    def Add_Experience(self,state,action,nextstate,reward,status):
        raise NotImplementedError()

    def __del__(self):
        print('Representation object died.')
