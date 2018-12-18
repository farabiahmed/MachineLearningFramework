
import os
import os.path
import numpy as np
import tensorflow as tf
from Memory.Memory_SumTree import Memory_SumTree
from Representation import Representation


class DeepCorrection_base(Representation):

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

        self.Gamma = gamma
        self.batchsize = batch_size
        self.trainPass = trainpass
        self.hidden_unit = hidden_unit
        self.learningrate = learning_rate
        self.convolutionLayer = convolutionLayer

        self.size_of_input_units = 3 * numberofagent; # (x,y,a) for each agent
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

        if os.path.isfile("models/model_0.h5"):
            print("###############################")
            print("Existing model loaded.......")
            print("###############################")

            for i in range(self.numberofagent):
                print("\nLoading model for agent #" + i)
                print("###############################")
                self.model[i] = load_model("models/model_0.h5")
                self.model[i].summary()

        # save the TensorFlow graph:
        self.graph = tf.get_default_graph()



        # Create Correction Model With Tensorflow
        model_correction_input  = tf.placeholder(tf.float32, [None, self.size_of_input_units])   # input state+action
        model_correction_label = tf.placeholder(tf.float32, [None, 1])                          # label y

        # neural network layers
        model_correction_layers = []

        model_correction_layers.append(tf.layers.dense(model_correction_input, hidden_unit[0], tf.nn.tanh))  # input layer

        for i in range(1, len(hidden_unit)):
            model_correction_layers.append(tf.layers.dense(model_correction_layers[i-1], hidden_unit[1], tf.nn.tanh))  # hidden layer

        model_correction_layers.append(tf.layers.dense(model_correction_layers[len(hidden_unit)-1], 1, tf.nn.relu))  # output layer, 1, only Q value

        BLA BLA BAL

        self.Save_Model()

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
            hypothesis = self.model.predict(input)

        values = np.asarray(hypothesis).reshape(self.output_unit)

        return values


    def Get_Action_Index(self, action):
        sizeOfAction = action.shape[0]
        temp = 0

        for i in np.arange(sizeOfAction):
            temp = temp + action[i] * (self.actionspaceforagent**(sizeOfAction-i-1))

        return temp;


    def Set_Value(self,state,action,value):

        return;

    def Add_Experience(self,state,action,nextstate,reward,status):

        # WORKING
        arg_Qmax, Qmax = self.Get_Greedy_Pair(nextstate)
        QValue = reward + self.Gamma * Qmax

        self.Set_Value(state,action,QValue)

    def Save_Model(self):
        with self.graph.as_default():

            if not os.path.exists("log/"+self.logfolder):
                os.makedirs("log/"+self.logfolder)

            self.model_correction.save("log/" + self.logfolder + "/model_correction_" + self.modelId + ".h5")
            self.model_correction.save_weights("log/"+self.logfolder+"/model_correction_weight_"+self.modelId+".h5")

            print("###############################")
            print("Model saved: " + "log/" + self.logfolder)
            print("###############################")

    def __del__(self):
        self.Save_Model()
        print('Representation object died.')
