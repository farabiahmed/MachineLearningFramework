import numpy as np

def myfunc(val):
    if val>5:
        return True
    else:
        val =10
    
A = np.array([1,2,3,4,5,6,7,8,9,10, 11, 12], dtype=np.float)

window_sz = 3
#B[:len(B)/window_sz*window_sz].reshape(-1,window_sz).mean(1)

B = [sum(A[i:i+window_sz])/window_sz if i+window_sz < len(A) else np.sum(A[i:])/(A.size - i) for i in np.arange(0,len(A),window_sz)]

C = np.arange(0,len(A),window_sz)

D = A[0:0+window_sz];

E = A[::window_sz]
print(A)
print(B)
print(C)
print(D)
print(E)

print(myfunc(8))
print(myfunc(3))


a = 5
b= 6
t = (a,b)
print(t)

a=9

print(t)

n = np.array([a,b])

dict = {}
dict[n.tobytes()] = 5
dict[n.tobytes()] = 25
dict[n.tobytes()] = 35

print(dict)