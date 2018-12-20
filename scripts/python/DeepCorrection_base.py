
import os
import os.path
import numpy as np
import tensorflow as tf
import tensorflow.contrib.slim as slim
from Memory.Memory_SumTree import Memory_SumTree
from Representation import Representation
from keras.layers import *
from keras.models import load_model


class DeepCorrection_base(Representation):

    def __init__(self,gridsize=5,actionspaceperagent=5,numberofagent=2,
                 hidden_unit=[12,12],learning_rate=0.1,
                 batch_size=32,trainpass=25,experiencebuffer=128,
                 train_period=1,
                 gamma = 0.99,
                 model_reset_counter=32,
                 fusion_model = "MAX_SUM", # MAX_SUM or MAX_MIN
                 modelId = "noid",
                 logfolder = ""):

        self.Gamma = gamma
        self.batchsize = batch_size
        self.trainPass = trainpass
        self.hidden_unit = hidden_unit
        self.learningrate = learning_rate
        self.fusion_model = fusion_model
        self.size_of_input_units = 3 * numberofagent; # (x,y,a) for each agent
        self.gridsize = gridsize
        self.size_of_action_space = actionspaceperagent**numberofagent
        self.action_count_per_agent = actionspaceperagent
        self.memory = Memory_SumTree(experiencebuffer)

        self.fresh_experience_counter = 0
        self.actionspaceforagent = actionspaceperagent
        self.numberofagent = numberofagent
        self.output_unit = 1 #actionspaceperagent**numberofagent
        self.trainingepochtotal = 0
        self.train_period = train_period # After how many new experience we will run fitting/training.
        self.counter_experience = 0 # A counter to hold how many tuple experienced
        self.counter_modelReset = model_reset_counter
        self.modelId = modelId
        self.logfolder = logfolder

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
        if os.path.isfile("models/model_0.h5"):
            print("###############################")
            print("Existing models are getting loaded...")
            print("###############################")

            for i in range(self.numberofagent):
                print("\nLoading model for agent #", '%d' % (i))
                print("###############################")

                self.graph_agent.append(tf.Graph())
                self.sess_agent.append(tf.Session(graph=self.graph_agent[i]))
                with self.graph_agent[i].as_default(), self.sess_agent[i].as_default():
                    with tf.name_scope('Model%d' % i):
                        model = load_model("models/model_0.h5")
                        self.model.append(model)
                        self.model[i].summary()
        print("Loading is completed.")

        # Load model
        # with tf.Session() as sess:
        #     # Restore variables from disk.
        #     saver.restore(sess, "/tmp/model.ckpt")
        #     print("Model restored.")
        # save the TensorFlow graph:


        self.graph_correction = tf.Graph()
        with self.graph_correction.as_default():
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
                    self.model_correction_layers.append(tf.layers.dense(self.model_correction_layers[i-1], hidden_unit[1], tf.nn.tanh))  # hidden layer

                # Output Layer
                self.model_correction_layers.append(tf.layers.dense(self.model_correction_layers[len(hidden_unit)-1], self.output_unit, tf.nn.relu))  # output layer, 1, only Q value

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
            model_correction_initialize_g = tf.global_variables_initializer()
            model_correction_initialize_a = tf.initialize_all_variables()
            model_correction_initialize_l = tf.local_variables_initializer()


            # Add ops to save and restore all the variables.
            self.saver = tf.train.Saver()

            # Inform the user about model
            self.tf_model_summary()

            # Start the Session
            self.sess = tf.Session(graph=self.graph_correction)
            #self.sess = tf.Session()
            # with tf.Session() as sess:
            #     sess.run(model_correction_initialize)
            #     print(sess.run(test_variable))

            # Intialize the Session
            self.sess.run(model_correction_initialize)

            print("###############################")
            print("TEST RESULTS:")
            print("###############################")
            print(self.sess.run(test_variable))
            print(self.sess.run(test_assign))
            print("")

        print("###############################")
        print("ACTION SPACE:")
        print("###############################")
        print(self.actions)
        print("")

        print("###############################")
        print("LIST OF DEVICES:")
        print("###############################")
        devices = self.sess.list_devices()
        for d in devices:
            print(d.name)
        print("")

        print("\n###############################")
        print("Initialization is completed.")
        print("###############################")
        print("")

        self.Save_Model()

    def tf_model_summary(self):
        model_vars = tf.trainable_variables()
        slim.model_analyzer.analyze_vars(model_vars, print_info=True)

    def Convert_State_To_Input(self,state, action):

        return np.concatenate((state,action))

    def Get_Greedy_Pair(self,state):

        agent_model_outputs = [] #2D list, agentId and Outputs(array) of each Agent.
        for i in range(self.numberofagent):
            agent_model_outputs.append(self.ForwardPass_AgentModel(i,state[2*i:2*i+3]))

        values = []
        for action in self.actions:
            input = self.Convert_State_To_Input(state, action);
            action_index = self.Get_Action_Index(action)
            correction_model_predicts = self.ForwardPass_CorrectionModel(input)

            agent_model_predicts = [] #1D list, holds Q value of each agent for a given action
            for i in range(self.numberofagent):
                #print(agent_model_outputs)
                #print("i:%d " % i, "action_index:%d" % action_index)
                temp = agent_model_outputs[i][action_index]
                agent_model_predicts.append(temp)

            out = self.Fusion_Models(
                np.array(agent_model_predicts),
                np.array(correction_model_predicts)
            )

            values.append(out)

        values = np.array(values)

        # Get the maximums
        arg = values.argmax()
        valmax = values.max()

        return arg,valmax

    def Get_Value(self,state,action):

        input = self.Convert_State_To_Input(state, action);
        action_index = self.Get_Action_Index(action)

        agent_model_predicts = []

        for i in range(self.numberofagent):
            values = self.ForwardPass_AgentModel(i,state)
            agent_model_predicts.append(values[action_index])

        correction_model_predicts = self.ForwardPass_CorrectionModel(input)

        out = self.Fusion_Models(
            np.array(agent_model_predicts),
            np.array(correction_model_predicts)
        )

        return out

    def Fusion_Models(self, agent_outputs, correction_output):

        # max-sum
        if (self.fusion_model == "MAX_SUM"):
            temp = agent_outputs.sum()
            temp = temp + correction_output
            return  temp

        # max-min
        elif(self.fusion_model == "MAX_MIN"):
            return agent_outputs.min() + correction_output

        else:
            raise NotImplementedError()

    def ForwardPass_AgentModel(self,agent_id, input):

        # Form Input Values
        # input = np.reshape(input,(1,input.shape[0]))
        input = np.reshape(input, (1, input.shape[0]))

        # Prediction of the model
        with self.graph_agent[agent_id].as_default(), self.sess_agent[agent_id].as_default():
            prediction = self.model[agent_id].predict(input)

        values = np.asarray(prediction).reshape(self.action_count_per_agent)
        return values

    def ForwardPass_CorrectionModel(self,input):

        input = np.expand_dims(input,axis=0)
        with self.graph_correction.as_default():
            prediction = self.sess.run(self.model_correction_layers[-1], feed_dict={
                                                                                    self.model_correction_input: input
                                                                                    })

        #values = np.asarray(prediction).reshape(self.output_unit)
        #return values
        return prediction[0][0]

    def Get_Action_Index(self, action):
        sizeOfAction = action.shape[0]
        temp = 0

        for i in np.arange(sizeOfAction):
            temp = temp + action[i] * (self.actionspaceforagent**(sizeOfAction-i-1))

        return temp;

    def Get_Action_List(self):

        actions = []
        for i in range(self.size_of_action_space):

            action = []
            for j in range(self.numberofagent):
                numberOfFieldsToPass = self.size_of_action_space ** (self.numberofagent - j - 1);
                action.append ( int((i / numberOfFieldsToPass) % self.size_of_action_space) )

            actions.append(action)

        return np.array(actions)

    def Set_Value(self,state,action,value):

        # Update label
        network_out = self.Get_Value(state, action)

        # Calculate error for Prioritized Experience Replay
        error = abs(network_out - value)

        # Append new sample to Memory of Experiences
        # Don't worry about its size, since it is a queue
        self.memory.add(error,(state, action, value))

        #if self.fresh_experience_counter == self.batchsize :
        if self.memory.length() >= self.batchsize :

            self.trainingepochtotal += self.trainPass
            # print('Training Epoch:', self.trainingepochtotal)

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
            values = np.reshape(values, (self.batchsize, 1))

            with self.graph_correction.as_default():
                cost, _  = self.sess.run([self.model_correction_cost, self.model_correction_train],
                                                                    feed_dict={
                                                                        self.model_correction_input: inputs,
                                                                        self.model_correction_label: values
                                                                    })

            print(cost)

        return;

    def Add_Experience(self,state,action,nextstate,reward,status):

        # WORKING
        arg_Qmax, Qmax = self.Get_Greedy_Pair(nextstate)
        QValue = reward + self.Gamma * Qmax

        self.Set_Value(state,action,QValue)

    def Save_Model(self):

        if not os.path.exists("log/"+self.logfolder):
            os.makedirs("log/"+self.logfolder)

        with self.graph_correction.as_default():
            # Save the variables to disk.
            save_path = self.saver.save(self.sess, "log/" + self.logfolder + "/model_correction_" + self.modelId + ".ckpt")

        print("###############################")
        print("Model saved in path: %s" % save_path)
        print("###############################")
        print("")

    def __del__(self):
        self.Save_Model()

        # Close the session
        self.sess.close()

        print('Representation object died.')
