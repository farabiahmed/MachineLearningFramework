import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os
from datetime import datetime

# Usage:
# python3.4 scripts/python/parser_agentReport.py 


# Get The Newest Folder form given path
path = "log"
# Get subdirectories
subdirectories = os.listdir(path)
# Sort the list to find newest one
subdirectories.sort(key=lambda date: datetime.strptime(date, "%Y%m%d_%H%M%S"))
# Get the newest directory
freshfolder = subdirectories[-1]

# Get related file and read via pandas
df = pd.read_csv('log/'+freshfolder+'/agentReport.csv')
arr = df.values

# Inform The User
print("Related File:", freshfolder,"/agentReport.csv")

# Loop through each data and scatter
iteration = arr.shape[0]
for xe in np.arange(iteration):
    for ye in arr[xe,:]:
        plt.scatter(xe+1,ye,color='purple', alpha=0.1)

plt.plot(np.arange(iteration)+1,np.mean(arr, axis=1))
plt.title('Performance Of The Agent')
plt.xlabel('Number of Iteration')
plt.ylabel('Cumulative Reward')
plt.show()

