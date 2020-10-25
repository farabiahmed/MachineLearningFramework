import matplotlib
import matplotlib.pyplot as plt
import numpy as np

# Data for plotting
t = np.arange(0-10, 100000, 10)
s = t * 2

fig, ax = plt.subplots()


ax.plot(t, s)
plt.xscale('log')
#plt.xlim([0, 1000])  # this line
plt.xlim(xmin=0)  # this line

ax.set(xlabel='time (s)', ylabel='voltage (mV)',
       title='About as simple as it gets, folks')
ax.grid()




#fig.savefig("test.png")
plt.show()
