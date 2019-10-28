import matplotlib.pyplot as plt
from pylab import *
from keras.models import Sequential
from keras.layers import Dense
from keras import optimizers

#Generate dummy data
data = data = linspace(1,2,100).reshape(-1,1)
y = data*5

#Define the model
def baseline_model():
   model = Sequential()
   model.add(Dense(1, activation = 'linear', input_dim = 1))
   sgd = optimizers.SGD(lr=0.2)
   model.compile(optimizer = sgd, loss = 'mean_squared_error', metrics = ['accuracy'])
   return model


#Use the model
regr = baseline_model()
regr.fit(data,y,epochs = 4,batch_size = 1)
plt.plot(data, regr.predict(data), 'b', data,y, 'k.')
plt.show()