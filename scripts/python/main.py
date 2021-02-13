import socket
import _thread
import time
import datetime as dt
import numpy as np
import tensorflow
from Representation_Keras_MultiAgent_TensorInput import Representation_Keras_MultiAgent_TensorInput
from DeepQNetwork import DeepQNetwork
from DeepQNetwork_PrioritizedReplay import DeepQNetwork_PrioritizedReplay
from DeepQNetwork_PrioritizedReplay_Target import DeepQNetwork_PrioritizedReplay_Target
from DeepQNetwork_PrioritizedReplay_Target_LearnerThread import DeepQNetwork_PrioritizedReplay_Target_LearnerThread
from DeepQNetwork_PrioritizedReplay_Target_LearnerThread_Hybrid import DeepQNetwork_PrioritizedReplay_Target_LearnerThread_Hybrid
from DeepCorrection_base import DeepCorrection_base
from DeepCorrection_Hybrid import DeepCorrection_Hybrid
from Representation_Empty import Representation_Empty
from DeepActorCritic_PrioritizedReplay import DeepActorCritic_PrioritizedReplay
#from DeepActorCritic_PrioritizedReplay_tflearn import DeepActorCritic_PrioritizedReplay_tflearn
from GeneticDistributer import GeneticDistributer
from Representation import Representation
from command_parser import command_parser, config_parser
import signal
import sys


#rep = Representation_Tabular([4,12,12])
#rep = Representation_Tensorflow(4,0.1)
# rep = Representation_Tensorflow_Batch(inputs=3,
#                                       hidden_unit=4,
#                                       learning_rate=0.1,
#                                       batch_size=9*4,
#                                       trainpass=5000)

print('tensorflow: %s' % tensorflow.__version__)

def strToValue(str):
    if len(str.split(";")) > 1 :
        try:
            return [int(i) for i in str.split(";")]
        except ValueError:
            return [float(i) for i in str.split(";")]
    else:
        try:
            return int(str)
        except ValueError:
            return float(str)

def init_model(config):

    if config["DEEP_ALGO_TYPE"] ==  "DeepQNetwork_PrioritizedReplay":
        rep = DeepQNetwork_PrioritizedReplay                   (gridsize            = strToValue(config["NUMBER_OF_ROWS"]),
                                                                actionspaceperagent = 5,
                                                                numberofagent       = strToValue(config["NUMBER_OF_AGENTS"]),
                                                                #hidden_unit=[256, 512, 256],
                                                                hidden_unit         = strToValue(config["HIDDEN_LAYERS"]),
                                                                learning_rate       = strToValue(config["ETA_LEARNING_RATE"]),
                                                                batch_size          = strToValue(config["BATCH_SIZE"]),
                                                                trainpass           = strToValue(config["TRAINING_PASS_PER_BATCH"]),
                                                                experiencebuffer    = strToValue(config["EXPERIENCE_REPLAY_BUFFER"]),
                                                                statePreprocessType = 'Vector',
                                                                convolutionLayer    = False,
                                                                modelId             = config["MODEL_ID"],
                                                                logfolder           = config["TIME_STAMP"],
                                                                )

    elif config["DEEP_ALGO_TYPE"] ==  "DeepQNetwork_PrioritizedReplay_Target":
        rep = DeepQNetwork_PrioritizedReplay_Target            (gridsize            = strToValue(config["NUMBER_OF_ROWS"]),
                                                                actionspaceperagent = 5,
                                                                numberofagent       = strToValue(config["NUMBER_OF_AGENTS"]),
                                                                #hidden_unit=[256, 512, 256],
                                                                hidden_unit         = strToValue(config["HIDDEN_LAYERS"]),
                                                                learning_rate       = strToValue(config["ETA_LEARNING_RATE"]),
                                                                batch_size          = strToValue(config["BATCH_SIZE"]),
                                                                trainpass           = strToValue(config["TRAINING_PASS_PER_BATCH"]),
                                                                experiencebuffer    = strToValue(config["EXPERIENCE_REPLAY_BUFFER"]),
                                                                statePreprocessType = 'Vector',
                                                                convolutionLayer    = False,
                                                                modelId             = config["MODEL_ID"],
                                                                logfolder           = config["TIME_STAMP"],
                                                                )

    elif config["DEEP_ALGO_TYPE"] ==  "DeepQNetwork_PrioritizedReplay_Target_LearnerThread":
        rep = DeepQNetwork_PrioritizedReplay_Target_LearnerThread   (gridsize            = strToValue(config["NUMBER_OF_ROWS"]),
                                                                actionspaceperagent = 5,
                                                                numberofagent       = strToValue(config["NUMBER_OF_AGENTS"]),
                                                                #hidden_unit=[256, 512, 256],
                                                                hidden_unit         = strToValue(config["HIDDEN_LAYERS"]),
                                                                learning_rate       = strToValue(config["ETA_LEARNING_RATE"]),
                                                                batch_size          = strToValue(config["BATCH_SIZE"]),
                                                                trainpass           = strToValue(config["TRAINING_PASS_PER_BATCH"]),
                                                                experiencebuffer    = strToValue(config["EXPERIENCE_REPLAY_BUFFER"]),
                                                                statePreprocessType = 'Vector',
                                                                convolutionLayer    = False,
                                                                modelId             = config["MODEL_ID"],
                                                                logfolder           = config["TIME_STAMP"],
                                                                )
    elif config["DEEP_ALGO_TYPE"] ==  "DeepQNetwork_PrioritizedReplay_Target_LearnerThread_Hybrid":
        rep = DeepQNetwork_PrioritizedReplay_Target_LearnerThread_Hybrid   (gridsize            = strToValue(config["NUMBER_OF_ROWS"]),
                                                                actionspaceperagent = 5,
                                                                numberofagent       = strToValue(config["NUMBER_OF_AGENTS"]),
                                                                #hidden_unit=[256, 512, 256],
                                                                hidden_unit         = strToValue(config["HIDDEN_LAYERS"]),
                                                                learning_rate       = strToValue(config["ETA_LEARNING_RATE"]),
                                                                batch_size          = strToValue(config["BATCH_SIZE"]),
                                                                trainpass           = strToValue(config["TRAINING_PASS_PER_BATCH"]),
                                                                experiencebuffer    = strToValue(config["EXPERIENCE_REPLAY_BUFFER"]),
                                                                statePreprocessType = 'Vector',
                                                                convolutionLayer    = False,
                                                                modelId             = config["MODEL_ID"],
                                                                logfolder           = config["TIME_STAMP"],
                                                                )

#   elif config["DEEP_ALGO_TYPE"] ==  "DeepActorCritic_PrioritizedReplay":
#       rep = DeepActorCritic_PrioritizedReplay_tflearn        (gridsize            = strToValue(config["NUMBER_OF_ROWS"]),
#                                                               actionspaceperagent = 5,
#                                                               numberofagent       = strToValue(config["NUMBER_OF_AGENTS"]),
#                                                               actor_hidden_unit   = strToValue(config["HIDDEN_LAYERS"]),
#                                                               critic_hidden_unit  = strToValue(config["CRITIC_HIDDEN_LAYERS"]),
#                                                               actor_learning_rate  = 0.05,
#                                                               critic_learning_rate = strToValue(config["ETA_LEARNING_RATE"]),
#                                                               batch_size          = strToValue(config["BATCH_SIZE"]),
#                                                               trainpass           = strToValue(config["TRAINING_PASS_PER_BATCH"]),
#                                                               experiencebuffer    = strToValue(config["EXPERIENCE_REPLAY_BUFFER"]),
#                                                               statePreprocessType = 'Vector',
#                                                               convolutionLayer    = False,
#                                                               modelId             = config["MODEL_ID"],
#                                                               logfolder           = config["TIME_STAMP"],
#                                                               )


    elif config["DEEP_ALGO_TYPE"] ==  "DeepCorrection":
        rep = DeepCorrection_base                               (gridsize            = strToValue(config["NUMBER_OF_ROWS"]),
                                                                actionspaceperagent = 5,
                                                                numberofagent       = strToValue(config["NUMBER_OF_AGENTS"]),
                                                                #hidden_unit=[256, 512, 256],
                                                                hidden_unit         = strToValue(config["HIDDEN_LAYERS"]),
                                                                learning_rate       = strToValue(config["ETA_LEARNING_RATE"]),
                                                                batch_size          = strToValue(config["BATCH_SIZE"]),
                                                                trainpass           = strToValue(config["TRAINING_PASS_PER_BATCH"]),
                                                                experiencebuffer    = strToValue(config["EXPERIENCE_REPLAY_BUFFER"]),
                                                                fusion_model        = config["FUSION_MODEL"],
                                                                correction_model_type = config["CORRECTION_MODEL_TYPE"],
                                                                modelId             = config["MODEL_ID"],
                                                                logfolder           = config["TIME_STAMP"],
                                                                agent_model         = config["AGENT_MODEL"],
                                                                )
    elif config["DEEP_ALGO_TYPE"] ==  "DeepCorrectionHybrid":
        rep = DeepCorrection_Hybrid                               (gridsize            = strToValue(config["NUMBER_OF_ROWS"]),
                                                                actionspaceperagent = 5,
                                                                numberofagent       = strToValue(config["NUMBER_OF_AGENTS"]),
                                                                #hidden_unit=[256, 512, 256],
                                                                hidden_unit         = strToValue(config["HIDDEN_LAYERS"]),
                                                                learning_rate       = strToValue(config["ETA_LEARNING_RATE"]),
                                                                batch_size          = strToValue(config["BATCH_SIZE"]),
                                                                trainpass           = strToValue(config["TRAINING_PASS_PER_BATCH"]),
                                                                experiencebuffer    = strToValue(config["EXPERIENCE_REPLAY_BUFFER"]),
                                                                fusion_model        = config["FUSION_MODEL"],
                                                                correction_model_type = config["CORRECTION_MODEL_TYPE"],
                                                                modelId             = config["MODEL_ID"],
                                                                logfolder           = config["TIME_STAMP"],
                                                                agent_model         = config["AGENT_MODEL"],
                                                                )
    elif config["DEEP_ALGO_TYPE"] ==  "GeneticDistributer":
        rep = GeneticDistributer                               (gridsize            = strToValue(config["NUMBER_OF_ROWS"]),
                                                                actionspaceperagent = 5,
                                                                numberofagent       = strToValue(config["NUMBER_OF_AGENTS"]),                                                                
                                                                modelId             = config["MODEL_ID"],
                                                                logfolder           = config["TIME_STAMP"],
                                                                agent_model         = config["AGENT_MODEL"],
                                                                algo_mode           = config["ALGO_MODE"],
                                                                )

    elif config["DEEP_ALGO_TYPE"] ==  "Representation_Empty":
        rep = Representation_Empty()


    return rep

# rep = Representation_Keras_MultiAgent_TensorInput      (gridsize=3,
#                                                         actionspaceperagent=5,
#                                                         numberofagent=2,
#                                                         hidden_unit=[25,25],
#                                                         learning_rate=0.1,
#                                                         batch_size=32,
#                                                         trainpass=25,
#                                                         experiencebuffer=128)

# rep = Representation_Tensorflow_ExperienceReplay_TypeB(statedimperagent=2,
#                                                         actionspaceperagent=5,
#                                                         numberofagent=2,
#                                                         hidden_unit=[12,6],
#                                                         learning_rate=0.1,
#                                                         batch_size=1,
#                                                         trainpass=1,
#                                                         experiencebuffer=1)



# rep = Representation_Keras_MultiAgent                   (statedimperagent=2,
#                                                         actionspaceperagent=5,
#                                                         numberofagent=3,
#                                                         hidden_unit=[128,256],
#                                                         learning_rate=0.1,
#                                                         batch_size=32,
#                                                         trainpass=10,
#               end                                          experiencebuffer=64)


# rep = Representation_Tensorflow_ExperienceReplay(       inputs=6,
#                                                         actionspaceperagent=5,
#                                                         numberofagent=2,
#                                                         hidden_unit=[12,6],
#                                                         learning_rate=0.1,
#                                                         batch_size=10,
#                                                         trainpass=1,
#                                                         experiencebuffer=20
#                                                         )

HOSTTX   ='127.0.0.1'
PORTRX = 5001
PORTTX = 4001

send_command_type = ''
send_ok = False
flag_continue = True

total_command_persec = 0
total_getval_persec = 0
total_setval_persec = 0
total_getgreedy_persec = 0
total_train_persec = 0
total_train = 0
total_train_old = 0

rep = None


# UDP Receiver
def read():

    # Initialize parameters
    global flag_continue, send_command_type, send_ok,rep, total_command_persec, total_getgreedy_persec, total_setval_persec, total_getval_persec, rep, total_train

    while flag_continue:
        # Read port when available
        data, addr = s.recvfrom(65530)

        # Convert the byte array to string
        rxstr = data.decode('utf-8')
        #print(rxstr)

        # Check whether it is config or command
        params = rxstr.split(",")


        if params[0] == "command" :
            # Parse Received Command
            command, state, action, value , nextstate, status = command_parser(params[1:])

            if command == 'initialstate':
                #print("params: ", params)
                newState, isDone = rep.Initial_State(state, value)
                
                newStateStr ="["
                for x in newState:
                    newStateStr+=str(x)+" "
                newStateStr = newStateStr[:-1]    
                newStateStr +="]"
                tmp = "OK,initialstate,"+ newStateStr + "," + str(isDone)
                #print(tmp)
                s.sendto(tmp.encode(), (HOSTTX, PORTTX))

            elif command == 'setvalue':
                total_train = rep.Set_Value(state, action, value)
                total_setval_persec += 1
                s.sendto(("OK,setvalue").encode(), (HOSTTX, PORTTX))

            elif command == 'getvalue':
                val = rep.Get_Value(state, action)
                total_getval_persec+=1
                s.sendto(("OK,getvalue,"+ str(val) ).encode(), (HOSTTX, PORTTX))


            elif command == 'getgreedypair':
                #n1 = dt.datetime.now()
                arg, val = rep.Get_Greedy_Pair(state)
                total_getgreedy_persec += 1
                tmp = "OK,getgreedypair," + str(arg) + "," + str(val)
                s.sendto((tmp).encode(), (HOSTTX, PORTTX))
                #n2 = dt.datetime.now()
                #print(((n2-n1).microseconds)/1e3)

            elif command == 'experience':
                rep.Add_Experience(state, action, nextstate, value, status)
                total_setval_persec += 1
                s.sendto(("OK,experience").encode(), (HOSTTX, PORTTX))

            total_command_persec+=1

        elif params[0] == "config" :
            config = config_parser(rxstr[7:].split("|"))
            print("Received Configuration:")
            print(config)

            #if rep != None:
            #    rep.Save_Model()
            if "NOT_INIT" not in config:
                rep = init_model(config)

            s.sendto(("OK,config").encode(), (HOSTTX, PORTTX))

    print("Thread read() stopped.")

# UDP Transmitter
def write():

    # Initialize parameters
    global send_command_type,send_ok,flag_continue

    while flag_continue:

        if send_ok :
            send_ok = False
            s.sendto(("OK," + send_command_type).encode(), (HOSTTX, PORTTX))

        time.sleep(100/1000)

    print("Thread write() stopped.")


# UDP Transmitter
def userinput():

    # Initialize parameters
    global flag_continue

    while flag_continue:
        user_input = input()
        if user_input == "stop":
            rep.Save_Model()
            flag_continue = False
            _thread.exit()
            break
        else:
            print("No User Input.")
            time.sleep(100/1000)

    print("Thread userinput() stopped.")


def signal_handler(sig, frame):
        print('You pressed Ctrl+C!')
        rep.Save_Model()
        flag_continue = False
        _thread.exit()
        sys.exit(0)

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(("", PORTRX))
print ('waiting on port:', PORTRX)
signal.signal(signal.SIGINT, signal_handler)

_thread.start_new_thread(read,())
_thread.start_new_thread(write,())
_thread.start_new_thread(userinput,())

while flag_continue:
    time.sleep(1)

    if total_train:
        total_train_persec = total_train - total_train_old
        total_train_old = total_train

    print('Total Command Per Sec:{:>5} SetVal:{:>5} GetVal:{:>5} GetGreedy:{:>5} TotalTrain:{:>5} '.format(total_command_persec,total_setval_persec,total_getval_persec,total_getgreedy_persec, total_train_persec))
    total_command_persec=0
    total_setval_persec=0
    total_getval_persec=0
    total_train_persec=0
    total_getgreedy_persec=0
