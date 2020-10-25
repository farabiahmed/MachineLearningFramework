import numpy as np

arr = np.array([[-7,3,-4],[-5,6,-1]])

print(arr)


min = arr[:,1:].min(axis=1)
print(min)

sum = np.sum(arr.min(axis=1))
print(sum)




arr = np.array([[-7],[5]])

print(arr)


min = arr[:,0:].min(axis=1)
print(min)

sum = np.sum(arr[:,0:].min(axis=1))
print(sum)



import datetime

x = datetime.datetime.now()
print(x.strftime("%Y%m%d_%H%M%S"))