
import os.path
import random
from time import sleep

import numpy as np
import tensorflow as tf
from keras.models import Sequential
from keras.layers import *
from keras.optimizers import Adam
import tflearn
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

        ########################################
        # Prepare Inputs
        ########################################
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
        
        
        self.action_bound = 1
        self.s_dim = self.size_of_input_units
        self.a_dim = actionspaceperagent**numberofagent
        
        
        
        ########################################
        # Build Actor Model
        ########################################

        self.actor_inputs = tflearn.input_data(shape=[None, self.s_dim])
        net = tflearn.fully_connected(self.actor_inputs, 400)
        net = tflearn.layers.normalization.batch_normalization(net)
        net = tflearn.activations.relu(net)
        net = tflearn.fully_connected(net, 300)
        net = tflearn.layers.normalization.batch_normalization(net)
        net = tflearn.activations.relu(net)
        # Final layer weights are init to Uniform[-3e-3, 3e-3]
        w_init = tflearn.initializations.uniform(minval=-0.003, maxval=0.003)
        self.actor_out = tflearn.fully_connected(
            net, self.a_dim, activation='tanh', weights_init=w_init)
        # Scale output to -action_bound to action_bound
        self.actor_scaled_out = tf.multiply(self.actor_out, self.action_bound)
        #return inputs, out, scaled_out

        self.actor_network_params = tf.trainable_variables()
    
        ########################################
        # Build Critic Model
        ########################################
        
        self.critic_inputs = tflearn.input_data(shape=[None, self.s_dim])
        self.critic_action = tflearn.input_data(shape=[None, self.a_dim])
        net = tflearn.fully_connected(self.critic_inputs, 400)
        net = tflearn.layers.normalization.batch_normalization(net)
        net = tflearn.activations.relu(net)

        # Add the action tensor in the 2nd hidden layer
        # Use two temp layers to get the corresponding weights and biases
        t1 = tflearn.fully_connected(net, 300)
        t2 = tflearn.fully_connected(self.critic_action, 300)

        net = tflearn.activation(
            tf.matmul(net, t1.W) + tf.matmul(action, t2.W) + t2.b, activation='relu')

        # linear layer connected to 1 output representing Q(s,a)
        # Weights are init to Uniform[-3e-3, 3e-3]
        w_init = tflearn.initializations.uniform(minval=-0.003, maxval=0.003)
        self.critic_out = tflearn.fully_connected(net, 1, weights_init=w_init)        
        
        self.critic_network_params = tf.trainable_variables()
        
        
        ########################################
        # Gradients for Actor Model
        ########################################
        # This gradient will be provided by the critic network
        self.action_gradient = tf.placeholder(tf.float32, [None, self.a_dim])
        
        # Combine the gradients, dividing by the batch size to 
        # account for the fact that the gradients are summed over the 
        # batch by tf.gradients 
        self.unnormalized_actor_gradients = tf.gradients(
            self.actor_scaled_out, self.network_params, -self.action_gradient)
        self.actor_gradients = list(map(lambda x: tf.div(x, self.batch_size), self.unnormalized_actor_gradients))
        
        # Optimization Op
        self.actor_optimize = tf.train.AdamOptimizer(self.actor_learning_rate).\
            apply_gradients(zip(self.actor_gradients, self.actor_network_params))
            
            
        ########################################
        # Gradients for Critic Model
        ########################################   
                 
        # Network target (y_i)
        # Obtained from the target networks
        self.predicted_q_value = tf.placeholder(tf.float32, [None, 1])
        
        # Define loss and optimization Op
        self.critic_loss = tflearn.mean_square(self.predicted_q_value, self.critic_out)
        self.critic_optimize = tf.train.AdamOptimizer(self.critic_learning_rate).minimize(self.critic_loss)
        
        # Get the gradient of the net w.r.t. the action
        self.action_grads = tf.gradients(self.critic_out, self.critic_action)
            
            
            
            
        
#         ########################################
#         self.model_actor = Sequential()
#         
#         self.model_actor.add(Dense(self.actor_hidden_unit[0], activation='relu', input_dim = self.size_of_input_units))
# 
#         for i in range(1, len(actor_hidden_unit)):
#             self.model_actor.add(Dense(self.actor_hidden_unit[i], activation='relu',kernel_initializer='he_uniform'))
#         self.model_actor.add(Dense(self.output_unit, activation='softmax', kernel_initializer='he_uniform'))
# 
#         # Compile model
#         self.model_actor.compile(loss='categorical_crossentropy', optimizer=Adam(lr=self.actorlearningrate))
#         #######################################
# 
#         ########################################
#         # Build Critic Model
#         ########################################
#         self.model_critic = Sequential()
#         
#         self.model_critic.add(Dense(self.critic_hidden_unit[0], activation='tanh', input_dim = self.size_of_input_units ))
# 
#         for i in range(1, len(critic_hidden_unit)):
#             self.model_critic.add(Dense(self.critic_hidden_unit[i], activation='tanh'))
#         self.model_critic.add(Dense(1, activation=LeakyReLU(0.3)))
# 
#         self.model_critic.compile(loss='mse', optimizer=Adam(lr=self.criticlearningrate))
#         #######################################
# 
#         
#         #save the TensorFlow graph:
#         self.graph = tf.get_default_graph()
# 
#         self.model_actor.summary()
#         print("Model Output Shape")
#         print(self.model_actor.output_shape[1])
#         sleep(1)
#         
#         self.model_critic.summary()
#         print("Model Output Shape")
#         print(self.model_critic.output_shape[1])


        #save the TensorFlow graph:
        self.graph = tf.get_default_graph()

        sleep(1)

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
