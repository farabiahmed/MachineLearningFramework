
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
                 convolutionLayer= False,
                 modelId = "noid",
                 logfolder = ""):

        self.Gamma = gamma
        self.batchsize = batch_size
        self.trainPass = trainpass
        self.hidden_unit = hidden_unit
        self.learningrate = learning_rate
        self.convolutionLayer = convolutionLayer
        self.fusion_model = fusion_model
        self.size_of_input_units = 3 * numberofagent; # (x,y,a) for each agent
        self.gridsize = gridsize

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

        self.graph = tf.get_default_graph()
        #self.graph = tf.reset_default_graph()
        #self.graph = tf.Graph()
        #with self.graph.as_default():

        # Construct model and encapsulating all ops into scopes, making
        # Tensorboard's Graph visualization more convenient
        # for example: with tf.name_scope('Loss'):

        self.model = [] # model holder for the agents
        if os.path.isfile("models/model_0.h5"):
            print("###############################")
            print("Existing models are getting loaded...")
            print("###############################")

            for i in range(self.numberofagent):
                print("\nLoading model for agent #", '%d' % (i))
                print("###############################")
                with tf.name_scope('Model', '%d' % (i)):
                    self.model.append(load_model("models/model_0.h5"))
                    self.model[i].summary()
        print("Loading is completed.")

        # Load model
        # with tf.Session() as sess:
        #     # Restore variables from disk.
        #     saver.restore(sess, "/tmp/model.ckpt")
        #     print("Model restored.")
        # save the TensorFlow graph:

        with tf.name_scope('Model_Correction'):
            # Create Correction Model With Tensorflow
            self.model_correction_input  = tf.placeholder(tf.float32, [None, self.size_of_input_units])   # input state+action
            self.model_correction_label = tf.placeholder(tf.float32, [None, 1])                          # label y

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
            model_correction_cost = tf.reduce_mean(tf.squared_difference(self.model_correction_label, self.model_correction_layers[-1]))

            # Optimizer embedded in API
            model_correction_optimizer = tf.train.GradientDescentOptimizer(self.learningrate)

            # Add minimizer as trainer
            self.model_correction_train = model_correction_optimizer.minimize(model_correction_cost)

        # Create a summary to monitor cost tensor
        tf.summary.scalar("Model_Correction_Optimizer", model_correction_cost)

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
        self.sess = tf.Session(graph=self.graph)
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
            agent_model_outputs.append(self.ForwardPass_AgentModel(i,state))

        values = np.zeros(1)
        for action in self.actions:
            input = self.Convert_State_To_Input(state, action);
            action_index = self.Get_Action_Index(action)
            correction_model_predicts = self.ForwardPass_CorrectionModel(input)

            agent_model_predicts = [] #1D list, holds Q value of each agent for a given action
            for i in range(self.numberofagent):
                agent_model_predicts.append(agent_model_outputs[i][action_index])

            out = self.Fusion_Models(
                np.array(agent_model_predicts),
                np.array(correction_model_predicts)
            )

            values = np.append(values, out)

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
            return np.sum(agent_outputs) + correction_output

        # max-min
        elif(self.fusion_model == "MAX_MIN"):
            return agent_outputs.min() + correction_output

        else:
            raise NotImplementedError()

    def ForwardPass_AgentModel(self,agent_id, input):

        # Form Input Values
        # input = np.reshape(input,(1,input.shape[0]))

        # Prediction of the model
        with self.graph.as_default():
            prediction = self.model[agent_id].predict(input)

        # values = np.asarray(hypothesis).reshape(self.output_unit)

        return prediction

    def ForwardPass_CorrectionModel(self,input):

        prediction = self.sess.run(self.model_correction_layers[-1], feed_dict={
                                                                        self.model_correction_input: input
                                                                    })

        # values = np.asarray(prediction).reshape(self.output_unit)

        return prediction

    def Get_Action_Index(self, action):
        sizeOfAction = action.shape[0]
        temp = 0

        for i in np.arange(sizeOfAction):
            temp = temp + action[i] * (self.actionspaceforagent**(sizeOfAction-i-1))

        return temp;

    def Set_Value(self,state,action,value):
        #with tf.Session() as sess:
            #sess.run(init_op)

        return;

    def Add_Experience(self,state,action,nextstate,reward,status):

        # WORKING
        arg_Qmax, Qmax = self.Get_Greedy_Pair(nextstate)
        QValue = reward + self.Gamma * Qmax

        self.Set_Value(state,action,QValue)

    def Save_Model(self):

        if not os.path.exists("log/"+self.logfolder):
            os.makedirs("log/"+self.logfolder)

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
