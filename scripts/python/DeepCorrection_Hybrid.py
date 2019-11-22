
import os
import os.path
import numpy as np
import tensorflow as tf
import tensorflow.contrib.slim as slim
from Memory.Memory_SumTree import Memory_SumTree
from Representation import Representation
from keras.layers import *
from keras.models import load_model
import _thread
from threading import Thread, Lock
import time

class DeepCorrection_Hybrid(Representation):

    def __init__(self,gridsize=5,actionspaceperagent=5,numberofagent=2,
                 hidden_unit=[12,12],learning_rate=0.1,
                 batch_size=32,trainpass=25,experiencebuffer=128,
                 train_period=1,
                 gamma = 0.99,
                 model_reset_counter=32,
                 fusion_model = "MAX_SUM", # MAX_SUM or MAX_MIN
                 correction_model_type="MULTIPLE_OUT",  # MULTIPLE_OUT, SINGLE_OUT
                 modelId = "noid",
                 logfolder = "",
                 agent_model = "models/model_0_20181224_091433.h5",
                 state_dim = 7):

        self.state_dim = 7
        self.update_target_interval = 10000
        self.trainingepochtotal = 0
        self.flag_continue = True
        self.dict = {}
        self.dict_agents = {}
        self.dict_getvalue = {}
        self.dict_getgreedy = {}
        self.dict_actionindex = {}
        
        self.model_agent_file = agent_model #low quality
        self.experiencebuffersize = experiencebuffer
        
        #self.model_agent_file = "models/model_0_20181217_173349.h5" # high quality
        self.Gamma = gamma
        self.batchsize = batch_size
        self.trainPass = trainpass
        self.hidden_unit = hidden_unit
        self.learningrate = learning_rate
        self.fusion_model = fusion_model
        self.correction_model_type = correction_model_type
        self.gridsize = gridsize
        self.size_of_action_space = actionspaceperagent**numberofagent
        self.action_count_per_agent = actionspaceperagent
        self.memory = Memory_SumTree(experiencebuffer)

        self.fresh_experience_counter = 0
        self.actionspaceforagent = actionspaceperagent
        self.numberofagent = numberofagent
        self.trainingepochtotal = 0
        self.experience_counter = 0
        
        self.train_period = train_period # After how many new experience we will run fitting/training.
        self.counter_experience = 0 # A counter to hold how many tuple experienced
        self.counter_modelReset = model_reset_counter
        self.modelId = modelId
        self.logfolder = logfolder
        
        if (self.correction_model_type == "MULTIPLE_OUT"):
            self.output_unit = actionspaceperagent**numberofagent
            self.size_of_input_units = self.state_dim * numberofagent  # (x,y,f,packet,delivery) for each agent
        else:
            self.output_unit = 1
            self.size_of_input_units = (self.state_dim+1) * numberofagent  # (x,y,f,packet,delivery,a) for each agent

        self.actions = self.Get_Action_List()

        #self.graph = tf.reset_default_graph()
        #self.graph = tf.Graph()
        #with self.graph.as_default():

        # Construct model and encapsulating all ops into scopes, making
        # Tensorboard's Graph visualization more convenient
        # for example: with tf.name_scope('Loss'):

        self.graph_agent = []
        self.sess_agent = []
        self.model = [] # model holder for the agents
        if os.path.isfile(self.model_agent_file):
            print("###############################")
            print("Existing agent models are getting loaded...")
            print("###############################")

            for i in range(self.numberofagent):
                print("\nLoading model for agent #", '%d' % (i))
                print("###############################")

                self.graph_agent.append(tf.Graph())
                self.sess_agent.append(tf.Session(graph=self.graph_agent[i]))
                with self.graph_agent[i].as_default(), self.sess_agent[i].as_default():
                    with tf.name_scope('Model%d' % i):
                        model = load_model(self.model_agent_file)
                        self.model.append(model)
                        self.model[i].summary()

        # Load model
        #with tf.Session() as sess:
             # Restore variables from disk.
                        #    saver.restore(sess, "log/" + self.logfolder + "/model_correction_" + self.modelId + ".ckpt")
                        #print("###############################")
                        #print("Model restored.")
                        #print("###############################")

        print("Agent Model Loading is completed.")
        # save the TensorFlow graph:


        self.graph_correction = tf.Graph()
        self.session_correction = tf.Session(graph=self.graph_correction)
        self.session_target = tf.Session(graph=self.graph_correction)
        
        with self.graph_correction.as_default():
            with self.session_correction.as_default():
                with tf.name_scope('Model_Correction'):
                    # Create Correction Model With Tensorflow
                    self.model_correction_input  = tf.placeholder(tf.float32, [None, self.size_of_input_units], name="model_correction_input")   # input state+action
                    self.model_correction_label = tf.placeholder(tf.float32, [None, self.output_unit], name="model_correction_label")                          # label y
    
                    # neural network layers
                    self.model_correction_layers = []
    
                    # First Layer
                    self.model_correction_layers.append(tf.layers.dense(self.model_correction_input, hidden_unit[0], tf.nn.tanh))  # input layer
    
                    # Hidden Layers
                    for i in range(1, len(hidden_unit)):
                        self.model_correction_layers.append(tf.layers.dense(self.model_correction_layers[i-1], hidden_unit[i], tf.nn.tanh))  # hidden layer
    
                    # Output Layer
                    self.model_correction_layers.append(
                            tf.layers.dense
                            (
                                self.model_correction_layers[len(hidden_unit)-1], 
                                self.output_unit, 
                                activation=self.my_leaky_relu
                            )
                        )  # output layer, 1, only Q value
    
                with tf.name_scope('Model_Correction_Optimizer'):
                    # Minimize error
                    self.model_correction_cost = tf.reduce_mean(tf.squared_difference(self.model_correction_label, self.model_correction_layers[-1]))
    
                    # Optimizer embedded in API
                    model_correction_optimizer = tf.train.GradientDescentOptimizer(self.learningrate)
    
                    # Add minimizer as trainer
                    self.model_correction_train = model_correction_optimizer.minimize(self.model_correction_cost)
    
                # Create a summary to monitor cost tensor
                tf.summary.scalar("Model_Correction_Optimizer", self.model_correction_cost)
    
                # Test Variables
                with tf.name_scope('Test_Variables'):
                    test_variable = tf.Variable(42, name='foo')
                    test_assign = test_variable.assign(13)
    
    
                # Add an op to initialize the variables.
                model_correction_initialize = tf.group(tf.global_variables_initializer(), tf.local_variables_initializer())
                #model_correction_initialize_g = tf.global_variables_initializer()
                #model_correction_initialize_a = tf.initialize_all_variables()
                #model_correction_initialize_l = tf.local_variables_initializer()
    
    
                # Add ops to save and restore all the variables.
                self.saver = tf.train.Saver()
    
                # Inform the user about model
                self.tf_model_summary()
    
                # Intialize the Session
                self.session_correction.run(model_correction_initialize)
                
                if os.path.isfile("log/" + self.logfolder + "/model_correction_" + self.modelId + ".ckpt"):
                    print("###############################")
                    print("Existing model is being loaded.......")
                    print("###############################")
                    tf.train.Saver.restore(self.session_correction, "log/" + self.logfolder + "/model_correction_" + self.modelId + ".ckpt")
                    print("###############################")
                    print("Existing model params are loaded.......")
                    print("###############################")
                    
                print("###############################")
                print("TEST RESULTS:")
                print("###############################")
                print(self.session_correction.run(test_variable))
                print(self.session_correction.run(test_assign))
                print("")

        self.session_target = self.session_correction
        
        print("###############################")
        print("ACTION SPACE:")
        print("###############################")
        print(self.actions)
        print("")

        print("###############################")
        print("LIST OF DEVICES:")
        print("###############################")
        devices = self.session_correction.list_devices()
        for d in devices:
            print(d.name)
        print("")

        print("\n###############################")
        print("Initialization is completed.")
        print("###############################")
        print("")
        
        #_thread.start_new_thread(self.Learner, ())
        self._thread = Thread(target = self.Learner, args=())
        self._thread.start()
        #self.Save_Model()
        
    def Update_target(self):
        print('Updating Target Network')
        
        self.session_target = self.session_correction

        self.Save_Model()
        self.dict.clear()
        self.dict_getgreedy.clear()
        self.dict_getvalue.clear()
        
    def tf_model_summary(self):
        model_vars = tf.trainable_variables()
        slim.model_analyzer.analyze_vars(model_vars, print_info=True)

    def Convert_State_To_Input(self,state, action):
        if (self.correction_model_type == "MULTIPLE_OUT"):
            return state
        return np.concatenate((state,action))

    def my_leaky_relu(self, x):
        #return tf.nn.leaky_relu(x, alpha=0.3) #requires tf 1.4
        alpha = 0.3
        return tf.nn.relu(x) - alpha * tf.nn.relu(-x)

    def Get_Greedy_Pair(self,state):
        if state.tobytes() in self.dict_getgreedy:
            (arg, valmax) = self.dict_getgreedy[state.tobytes()]
            return arg, valmax
        
        if len(self.dict_getgreedy) > self.experiencebuffersize and len(self.dict_getgreedy)%100 is 0:
            print("Dictionary GetGreedy: ", len(self.dict_getgreedy))
                
        
        agent_model_outputs = [] #2D list, agentId and Outputs(array) of each Agent.
        for i in range(self.numberofagent):
            agent_model_outputs.append(self.ForwardPass_AgentModel(i,state[self.state_dim*i:self.state_dim*i+self.state_dim]))

        if (self.correction_model_type == "MULTIPLE_OUT"):
            input = self.Convert_State_To_Input(state, self.actions[0]);
            correction_model_predicts = self.ForwardPass_CorrectionModel(input)

        values = []
        for action in self.actions:

            #if (self.correction_model_type == "SINGLE_OUT"):
            #    input = self.Convert_State_To_Input(state, action);
            #    correction_model_predicts = self.ForwardPass_CorrectionModel(input)

            agent_model_predicts = [] #1D list, holds Q value of each agent for a given action
            for i in range(self.numberofagent):
                agent_model_predicts.append(agent_model_outputs[i][action[i]])

            #out=0
            out = self.Fusion_Models(np.array(agent_model_predicts))

            #if (self.correction_model_type == "SINGLE_OUT"):
            #    correction_model_predict = correction_model_predicts
            #    value = out + correction_model_predict
            #else:
            action_index = self.Get_Action_Index(action)
            #action_index=0
            value = out + correction_model_predicts[action_index]

            values.append(value)

        values = np.array(values)

        # Get the maximums
        arg = values.argmax()
        valmax = values.max()
        
        if len(self.dict_getgreedy) < self.experiencebuffersize:
            self.dict_getgreedy[state.tobytes()] = (arg, valmax)
        
        return arg,valmax

    def Get_Value(self,state,action):
        input = self.Convert_State_To_Input(state, action);

        if input.tobytes() in self.dict_getvalue:
            ret = self.dict_getvalue[input.tobytes()]
            return ret
        
        if len(self.dict_getvalue) > self.experiencebuffersize and len(self.dict_getvalue)%100 is 0:
            print("Dictionary Getvalue: ", len(self.dict_getvalue))
                

        agent_model_predicts = []

        for i in range(self.numberofagent):
            values = self.ForwardPass_AgentModel(i,state[self.state_dim*i:self.state_dim*i+self.state_dim])
            agent_model_predicts.append(values[action[i]])
            #if (self.correction_model_type == "MULTIPLE_OUT"):
            #    agent_model_predicts.append(values)
            #else:
            #    agent_model_predicts.append(values[action[i]])


        out = self.Fusion_Models(
            np.array(agent_model_predicts),
        )

        correction_model_predicts = self.ForwardPass_CorrectionModel(input)

        if (self.correction_model_type == "MULTIPLE_OUT"):
            action_index = self.Get_Action_Index(action)
            #out = out + correction_model_predicts[action_index]

            self.correction_model_predicts = correction_model_predicts
            correction_model_predicts[action_index] += out
            self.fusioned_model_predicts = out
            #self.combined_model_predicts = correction_model_predicts
            ret = correction_model_predicts[action_index]
        else:
            correction_model_predict = correction_model_predicts
            ret = out + correction_model_predict
        
        if len(self.dict_getvalue) < self.experiencebuffersize:
            self.dict_getvalue[input.tobytes()] = ret
            
        return ret

    def Fusion_Models(self, agent_outputs):

        # max-sum
        if (self.fusion_model == "MAX_SUM"):
            # if (self.correction_model_type == "MULTIPLE_OUT"):
            #     sum = agent_outputs[0]
            #     for i in range(1, len(agent_outputs)) :
            #         sum = sum + agent_outputs[i]
            #     return sum
            # else:
            #     return agent_outputs.sum()
            return agent_outputs.sum()

        # max-min
        elif(self.fusion_model == "MAX_MIN"):
            return agent_outputs.min()

        else:
            raise NotImplementedError()

    def ForwardPass_AgentModel(self,agent_id, input):

        # Form Input Values
        # input = np.reshape(input,(1,input.shape[0]))
        input = np.reshape(input, (1, input.shape[0]))
        
        if input.tobytes() in self.dict_agents:
            return self.dict_agents[input.tobytes()]
        else:
            # Prediction of the model
            with self.graph_agent[agent_id].as_default(), self.sess_agent[agent_id].as_default():
                prediction = self.model[agent_id].predict(input)
    
            values = np.asarray(prediction).reshape(self.action_count_per_agent)
            
            # Dont add more to dictionary if maximum limit reached. experiencebuffersizex3
            if len(self.dict_agents) < self.experiencebuffersize * 3:
                self.dict_agents[input.tobytes()] = values 
            
                if len(self.dict_agents) > self.experiencebuffersize  * 3 and len(self.dict_agents)%100 is 0:
                    print("Dictionary Agents: ", len(self.dict_agents))
                
            return values

    def ForwardPass_CorrectionModel(self,input):
        input = np.expand_dims(input,axis=0)
        
        if input.tobytes() in self.dict:
            return self.dict[input.tobytes()]
        else:
            with self.graph_correction.as_default():
                with self.session_target.as_default():
                    prediction = self.session_target.run(self.model_correction_layers[-1], feed_dict={
                                                                                            self.model_correction_input: input
                                                                                            })            
            if len(self.dict) > self.experiencebuffersize and len(self.dict)%100 is 0:
                print("Dictionary: ", len(self.dict))
                
            if (self.correction_model_type == "SINGLE_OUT"):
                
                self.dict[input.tobytes()] = prediction[0][0]
                return prediction[0][0]
            else:
                self.dict[input.tobytes()] = prediction[0] 
                return prediction[0]

    def Get_Action_Index(self, action):
        
        return self.dict_actionindex[action.tobytes()]
        
        sizeOfAction = action.shape[0]
        temp = 0

        for i in np.arange(sizeOfAction):
            temp = temp + action[i] * (self.actionspaceforagent**(sizeOfAction-i-1))

        return temp;

    def Get_Action_List(self):
        
        self.dict_actionindex = {}
        
        actions = []
        for i in range(self.size_of_action_space):

            action = []
            for j in range(self.numberofagent):
                numberOfFieldsToPass = self.action_count_per_agent ** (self.numberofagent - j - 1);
                action.append ( int((i / numberOfFieldsToPass) % self.action_count_per_agent) )

            actions.append(action)
            self.dict_actionindex[np.array(action).tobytes()] = i
            
        return np.array(actions)

    def Set_Value(self,state,action,value):

        # Update label
        network_out = self.Get_Value(state, action)

        if (self.correction_model_type == "SINGLE_OUT"):
            label = network_out
        else:
            action_index = self.Get_Action_Index(action)
            #self.fusioned_model_predicts
            #self.combined_model_predicts
            self.correction_model_predicts[action_index] = value - self.fusioned_model_predicts
            label = self.correction_model_predicts

        error = abs(network_out - value)

        # Append new sample to Memory of Experiences
        # Don't worry about its size, since it is a queue
        self.memory.add(error,(state, action, label))
        
        # To be able to stop learner thread if there is no more experience
        if self.experience_counter < 10: #self.experiencebuffersize:
            self.experience_counter+=1
        
        #self.Learn()
        
        return self.trainingepochtotal
    
    def Learn(self) :
        #if self.fresh_experience_counter == self.batchsize :
        if self.memory.length() >= self.batchsize and self.experience_counter>0:

            self.trainingepochtotal += self.trainPass
            # print('Training Epoch:', self.trainingepochtotal)

            if not self.trainingepochtotal % self.update_target_interval:
                self.Save_Model()

            # Get Unique Samples from memory as much as batchsize
            minibatch = self.memory.sample(self.batchsize)

            states = []
            actions = []
            values = []
            for i in np.arange(len(minibatch)):
                idx, (s, a, v) = minibatch[i]
                states.append(s)
                actions.append(a)
                values.append(v)

            inputs = []
            for i in range(len(states)):
                input = self.Convert_State_To_Input(states[i], actions[i])
                inputs.append(input)

            inputs = np.array(inputs)
            values = np.array(values)
            values = np.reshape(values, (self.batchsize, self.output_unit))
            
            with tf.device('/gpu:0'):
                with self.graph_correction.as_default():
                    with self.session_correction.as_default(): 
                        for i in range(self.trainPass):
                            cost, _  = self.session_correction.run([self.model_correction_cost, 
                                                                    self.model_correction_train],
                                                                            feed_dict={
                                                                                self.model_correction_input: inputs,
                                                                                self.model_correction_label: values
                                                                            })
            
            if not self.trainingepochtotal % self.update_target_interval:
                self.Update_target()
                
            # To be able to stop learner thread if there is no more experience
            self.experience_counter -= 1
        else:
            #print("Sleeping Learner Thread")
            time.sleep(0.1)
            
    # Learner Thread Run Function
    def Learner(self):

        while self.flag_continue:
            self.Learn()

        print("Thread Learner stopped.")
        
    def Add_Experience(self,state,action,nextstate,reward,status):

        # WORKING
        arg_Qmax, Qmax = self.Get_Greedy_Pair(nextstate)
        QValue = reward + self.Gamma * Qmax

        self.Set_Value(state,action,QValue)

    def Save_Model(self):

        if not os.path.exists("log/"+self.logfolder):
            os.makedirs("log/"+self.logfolder)

        with self.graph_correction.as_default():
            with self.session_correction.as_default():
                # Save the variables to disk.
                save_path = self.saver.save(self.session_correction, "log/" + self.logfolder + "/model_correction_" + self.modelId + ".ckpt")

        print("###############################")
        print("Model saved in path: %s" % save_path)
        print("log/" + self.logfolder + "/model_correction_" + self.modelId + ".ckpt")
        print("###############################")
        print("")

    def __del__(self):
        self.Save_Model()

        # Close the session
        self.sess.close()

        print('Representation object died.')
