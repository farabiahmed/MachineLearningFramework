import numpy as np


# Command Parser
def command_parser(str):

    # Initialize returns
    state = []
    action = []
    nextstate = []
    reward = 0
    value = 0
    status = 0
    commandtype = ''

    # Split the string by commas
    words = str.split(",")

    # loop thorough each element then catch
    # appropriate item
    for word in words:

        if word == 'setvalue':
            commandtype = word
            continue

        elif word == 'state':
            commandtype = word
            continue

        elif word == 'action':
            commandtype = word
            continue

        elif word == 'nextstate':
            commandtype = word
            continue

        elif word == 'reward':
            commandtype = word
            continue

        elif word == 'status':
            commandtype = word
            continue

        elif word == 'value':
            commandtype = word
            continue

        elif commandtype == 'state':
            state.append(int(word))

        elif commandtype == 'action':
            action.append(int(word))

        elif commandtype == 'nextstate':
            nextstate.append(int(word))

        elif commandtype == 'value' or commandtype == 'reward':
            value = float(word)

        elif commandtype == 'status':
            status = int(word)

    # Return in this order:
    # command, state, action, value
    return words[0],np.array(state),np.array(action),value, np.array(nextstate),status