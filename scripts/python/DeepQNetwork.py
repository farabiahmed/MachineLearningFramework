
import os.path
import random

import numpy as np
import tensorflow as tf
from keras.layers import Dense
from keras.layers.advanced_activations import LeakyReLU
from keras.models import Sequential
from Memory import Memory_SumTree
from Memory import Memory_UniformRandom

from Representation import Representation


class DeepQNetwork(Representation):

    def __init__(self,gridsize=5,actionspaceperagent=5,numberofagent=2,
                 hidden_unit=[12,12],learning_rate=0.1,
                 batch_size=32,trainpass=25,experiencebuffer=128,
                 train_period=1,
                 gamma = 0.99,
                 model_reset_counter=32,
                 memory_type = "SumTree"):

        self.Gamma = gamma
        self.batchsize = batch_size
        self.trainPass = trainpass
        self.hidden_unit = hidden_unit
        self.learningrate = learning_rate
        self.size_of_input_units = gridsize * gridsize * numberofagent
        self.gridsize = gridsize

        if memory_type=="Uniform":
            self.memory = Memory_UniformRandom(experiencebuffer)
        elif memory_type == "SumTree":
            self.memory = Memory_SumTree(experiencebuffer)

        self.fresh_experience_counter = 0
        self.actionspaceforagent = actionspaceperagent
        self.numberofagent = numberofagent
        self.output_unit = actionspaceperagent**numberofagent
        self.trainingepochtotal = 0
        self.train_period = train_period # After how many new experience we will run fitting/training.
        self.counter_experience = 0 # A counter to hold how many tuple experienced
        self.counter_modelReset = model_reset_counter

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
        self.modelFreezed = self.model;
        self.modelFreezed.summary()

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
        input = np.reshape(input,(1,input.shape[0]))

        # Prediction of the model
        with self.graph.as_default():
            hypothesis = self.modelFreezed.predict(input)

        values = np.asarray(hypothesis).reshape(self.output_unit)

        return values

    def ForwardPassQ2(self,input):

        # Form Input Values
        input = np.reshape(input,(1,input.shape[0]))

        # Prediction of the model
        with self.graph.as_default():
            hypothesis = self.model.predict(input)

        values = np.asarray(hypothesis).reshape(self.output_unit)

        return values

    def Get_Greedy_PairQ2(self,state):
        # Backward compability: Preprocess state input if needed.
        if np.size(state) != self.size_of_input_units:
            values = self.ForwardPassQ2(self.Convert_State_To_Input(state))
        else:
            values = self.ForwardPassQ2(state)

        # Get the maximums
        arg = values.argmax()
        valmax = values.max()

        return arg,valmax

    def Set_Value(self,state,action,value):

        # Preprocess State
        inputVal = self.Convert_State_To_Input(state)

        # Update label
        values = self.ForwardPass(inputVal)
        index = self.Get_Action_Index(action)
        values[index] = value

        # Append new sample to Memory of Experiences
        # Don't worry about its size, since it is a queue
        self.memory.add((inputVal, values))

        #if self.fresh_experience_counter == self.batchsize :
        if len(self.memory) >= self.batchsize :

            self.trainingepochtotal += self.trainPass
            # print('Training Epoch:', self.trainingepochtotal)

            # Get Unique Samples from memory as much as batchsize
            minibatch = random.sample(list(self.memory), self.batchsize)

            for i in np.arange(len(minibatch)):
                X, Y = minibatch[i]
                self.batchSamplesX = np.vstack((self.batchSamplesX, X))
                self.batchSamplesY = np.vstack((self.batchSamplesY, Y))

            with self.graph.as_default():
                self.model.fit(self.batchSamplesX, self.batchSamplesY, epochs=self.trainPass, batch_size= self.batchsize, verbose=0)

            # Reset the batch
            self.batchSamplesX = np.array([], dtype=np.float).reshape(0, self.size_of_input_units)
            self.batchSamplesY = np.array([], dtype=np.float).reshape(0, self.output_unit)

    def Get_Action_Index(self, action):
        sizeOfAction = action.shape[0]
        temp = 0

        for i in np.arange(sizeOfAction):
            temp = temp + action[i] * (self.actionspaceforagent**(sizeOfAction-i-1))

        return temp;

    def Add_Experience(self,state,action,nextstate,reward,status):

        self.counter_experience += 1

        if self.counter_experience % self.counter_modelReset == 0:
            self.modelFreezed = self.model

        # WORKING
        arg_Qmax, Qmax = self.Get_Greedy_Pair(nextstate)
        QValue = reward + self.Gamma * Qmax

        self.Set_Value(state,action,QValue)

        # WORKING
        # state = self.Convert_State_To_Input(state)
        # nextstate = self.Convert_State_To_Input(nextstate)
        # action = self.Get_Action_Index(action)
        # values=0
        # self.memory.append((state,action,reward,nextstate,status,values))
        #
        # if len(self.memory) >= self.batchsize:
        #
        #     self.trainingepochtotal += self.trainPass
        #
        #     minibatch = self.Memory_Sample_UniformRandom()
        #
        #     for state, action, reward, nextstate, status, values in minibatch :
        #
        #         if status==1 :
        #             value = reward
        #         else :
        #             # arg_Qmax, Qmax = self.Get_Greedy_Pair(nextstate)
        #
        #             arg_Q2max, Q2max = self.Get_Greedy_PairQ2(nextstate)
        #
        #             Q_Update = self.ForwardPass(nextstate)[arg_Q2max]
        #
        #             value = reward + self.Gamma * Q_Update
        #
        #         values = self.ForwardPass(state)
        #         values[action] = value
        #
        #         self.batchSamplesX = np.vstack((self.batchSamplesX, state))
        #         self.batchSamplesY = np.vstack((self.batchSamplesY, values))
        #
        #     with self.graph.as_default():
        #         self.model.fit(self.batchSamplesX, self.batchSamplesY, epochs=self.trainPass,
        #                        batch_size=self.batchsize, verbose=0)
        #
        #     # Reset the batch
        #     self.batchSamplesX = np.array([], dtype=np.float).reshape(0, self.size_of_input_units)
        #     self.batchSamplesY = np.array([], dtype=np.float).reshape(0, self.output_unit)


        # NOT WORKING
        # time_Full = time.time()
        # self.memory.append((state,action,reward,nextstate,status))
        #
        # # if self.fresh_experience_counter == self.batchsize :
        # if len(self.memory) >= self.batchsize:
        #
        #     self.trainingepochtotal += self.trainPass
        #
        #
        #     # Get Unique Samples from memory as much as batchsize
        #     minibatch = random.sample(list(self.memory), self.batchsize)
        #
        #     time_Loop = time.time()
        #
        #     for i in np.arange(len(minibatch)):
        #         state, action, reward, nextstate, status = minibatch[i]
        #
        #         arg_Qmax, Qmax = self.Get_Greedy_Pair(nextstate)
        #         QValue = reward + self.Gamma * Qmax
        #
        #         processed_state = self.Convert_State_To_Input(state)
        #         values = self.ForwardPass(processed_state)
        #
        #         index = self.Get_Action_Index(action)
        #         values[index] = QValue
        #
        #         self.batchSamplesX = np.vstack((self.batchSamplesX, processed_state))
        #         self.batchSamplesY = np.vstack((self.batchSamplesY, values))
        #
        #     print("For Loop: %s seconds" % (time.time() - time_Loop))  # 0.12662458419799805 seconds
        #
        #     time_Fit = time.time()
        #     with self.graph.as_default():
        #         self.model.fit(self.batchSamplesX, self.batchSamplesY, epochs=self.trainPass, batch_size=self.batchsize,
        #                        verbose=0)
        #     print("Get_Fit: %s seconds" % (time.time() - time_Fit))  # 0.12662458419799805 seconds
        #
        #     # Reset the batch
        #     self.batchSamplesX = np.array([], dtype=np.float).reshape(0, self.size_of_input_units)
        #     self.batchSamplesY = np.array([], dtype=np.float).reshape(0, self.output_unit)
        #
        # print("Get_Full: %s seconds" % (time.time() - time_Full))  # 0.12662458419799805 seconds


            # NOT WORKING
        # Transform our states to new form then store them.
        # state = self.Convert_State_To_Input(state)
        # nextstate = self.Convert_State_To_Input (nextstate)


        # self.memory.append((state,action,reward,nextstate,status))
        # self.counter_experience+=1
        #
        # if len(self.memory) >= self.batchsize and self.counter_experience % self.train_period == 0 :
        #     self.Network_Train()

    def Network_Train(self):
        print("Network Train Called..")

        # NOT WORKING #2
        # minibatch = self.Memory_Sample_UniformRandom()
        # for i in range(self.batchsize):
        #
        #     state, action, reward, nextstate, status = minibatch[i]
        #
        #     action_id = self.Get_Action_Index(action)
        #
        #     Qtarget = self.ForwardPass(self.Convert_State_To_Input(state))
        #
        #     if status == 1:
        #         Qtarget[action_id] = reward
        #     else:
        #         arg_Qmax, Qmax = self.Get_Greedy_Pair(nextstate)
        #         Qtarget[action_id] =  reward + self.Gamma * Qmax
        #
        #     self.batchSamplesX = np.vstack((self.batchSamplesX, self.Convert_State_To_Input(state)))
        #     self.batchSamplesY = np.vstack((self.batchSamplesY, Qtarget))
        #
        # with self.graph.as_default():
        #     self.model.fit(self.batchSamplesX, self.batchSamplesY, epochs=self.trainPass, batch_size=self.batchsize,verbose=0)
        #
        # # Reset the batch
        # self.batchSamplesX = np.array([], dtype=np.float).reshape(0, self.size_of_input_units)
        # self.batchSamplesY = np.array([], dtype=np.float).reshape(0, self.output_unit)



        # NOT WORKING #1
        # minibatch = self.Memory_Sample_UniformRandom()
        # #states, actions, rewards, nextstates, stats = zip(*minibatch)
        #
        # states          = np.vstack(np.array(minibatch)[:, 0])
        # actions         = np.vstack(np.array(minibatch)[:, 1])
        # rewards         = np.vstack(np.array(minibatch)[:, 2])
        # nextstates      = np.vstack(np.array(minibatch)[:, 3])
        # stats           = np.vstack(np.array(minibatch)[:, 4])
        #
        # action_ids = np.zeros(shape=(self.batchsize, 1))
        # for i in range(self.batchsize):
        #     action_ids[i, 0] = self.Get_Action_Index(actions[i])
        #
        # # Prediction of the model
        # with self.graph.as_default():
        #
        #     # Predictions
        #     Qpredicted   = self.model.predict(nextstates)
        #     Qtarget      = self.model.predict(states)
        #
        #     for i in range(self.batchsize):
        #
        #         action_id = int(action_ids[i, 0])
        #
        #         if stats[i] == 1:
        #             Qtarget[i, action_id] = rewards[i, 0]
        #         else:
        #             arg_Qmax = np.argmax(Qpredicted[i, :])
        #             Qmax = np.max(Qpredicted[i, :])
        #             reward = rewards[i, 0]
        #
        #             Qtarget[i, action_id] =  reward + self.Gamma * Qmax
        #
        #     self.model.fit(states, Qtarget, epochs=self.trainPass, batch_size=self.batchsize,
        #                    verbose=0)

    def Memory_Sample_UniformRandom(self):
        return random.sample(list(self.memory),self.batchsize)

    def Memory_Sample_PrioritizationProportional(self):
        raise NotImplementedError()

    def Memory_Sample_PrioritizationRankBased(self):
        raise NotImplementedError()

    def Save_Model(self):
        with self.graph.as_default():
            self.model.save_weights("modelOutput.h5")
            print("###############################")
            print("Model saved.......")
            print("###############################")

    def __del__(self):
        print('Representation object died.')
