import matplotlib.pyplot as plt
import tensorflow as tf
from pylab import *
from keras.models import Sequential
from keras.layers import Dense
from keras import optimizers
from threading import Thread, Lock
from time import sleep

def ThreadRun():
    print("Thread started.")
    sleep(10)
    with graph.as_default():
        with session.as_default() as sess:
            plt.plot(data, model.predict(data), 'b', data, y, 'k.')
    print("Thread stopped.")

#Generate dummy data
data = data = linspace(1,2,100).reshape(-1,1)
y = data*5


graph = tf.Graph()
session = tf.Session(graph=graph)

#Define the model
with graph.as_default():
    with session.as_default() as sess:
        model = Sequential()
        model.add(Dense(1, activation = 'linear', input_dim = 1))
        sgd = optimizers.SGD(lr=0.01)
        model.compile(optimizer = sgd, loss = 'mean_squared_error', metrics = ['accuracy'])

_thread = Thread(target=ThreadRun, args=())
_thread.start()

#Use the model
with graph.as_default():
    with session.as_default() as sess:
        for i in range(5):
            data_before = model.predict(data)
            plt.plot(data, data_before, 'r')
            model.fit(data,y,epochs = 1,batch_size = 1)    
    
_thread.join()

    
plt.show()