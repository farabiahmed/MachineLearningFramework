import concurrent.futures
import numpy as np

import keras.backend as K
from keras.layers import Dense
from keras.models import Sequential

import tensorflow as tf
from tensorflow.python.client import device_lib

def get_available_gpus():
    local_device_protos = device_lib.list_local_devices()
    return [x.name for x in local_device_protos if x.device_type == 'GPU']

def get_available_devices():
    local_device_protos = device_lib.list_local_devices()
    return [x.name for x in local_device_protos]


gpus = get_available_gpus()
results = [x for x in gpus]
print('results: ', results) #results:  ['/gpu:0']


devs = get_available_devices()
results = [x for x in devs]
print('results: ', results) #results:  ['/cpu:0', '/gpu:0']