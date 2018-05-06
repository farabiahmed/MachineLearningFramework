import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os
import sys #for commandline args
from datetime import datetime
import os.path
from os.path import basename
from os.path import splitext

# Usage:
# python3.4 scripts/python/parser_agentReport.py

# If you want to skip some records try this
# python3.4 scripts/python/parser_agentReport.py 10

# Get The Newest Folder form given path
path = "log"
skipline = 1

print(sys.argv)

if len(sys.argv) == 2:
    skipline = int(sys.argv[1])


# Get subdirectories
subdirectories = os.listdir(path)

# Sort the list to find newest one
subdirectories.sort(key=lambda date: datetime.strptime(date, "%Y%m%d_%H%M%S"))

# Get the newest directory
freshfolder = subdirectories[-1]

# 'agentReport_15.csv', 'agentReport_1.csv', 'agentReport_7.csv',....
modelFiles = [filename for filename in os.listdir(path + '/' + freshfolder) if filename.startswith("agentReport_")]
modelFiles.sort(key=lambda str: int(splitext(basename(str))[0].split('_')[1]))
print(modelFiles)

index = 0
for filename in modelFiles:
    ###################################
    # Report #i
    ###################################
    
    index = index + 1
    
    # Get related file and read via pandas
    df = pd.read_csv('log/'+freshfolder+'/'+filename)
    arr = df.values
    
    # Inform The User
    print("REPORT #", index);
    print("Related File:", freshfolder,"/"+filename)
    
    # Loop through each data and scatter
    records = arr.shape[0]
    print('Total Records',records)
    
    if records>100 :
        skipline = (records // 100)
    else :
        skipline = 1
    
    print('Skipline:',skipline)
    indexlist = np.arange(0,records,skipline)
    
    f1 = plt.figure()
    ax1 = f1.add_subplot(111)
    
    for xe in indexlist:
        for ye in arr[xe,1:]:
            plt.scatter(arr[xe,0],ye,color='purple', alpha=0.1)
    
    # Plot using matplotlib
    
    
    # Draw mean values
    plt.plot(arr[::skipline,0],np.mean(arr[::skipline,1:], axis=1))
    
    # filter_coef = 0.95
    # #filtered = np.zeros([indexlist.shape[0],1])
    # filtered = arr[::skipline,1]
    # for i in indexlist:
    # 	for j in np.arange(1,arr.shape[1]):
    # 		filtered[i] = filtered[i] * filter_coef + (1-filter_coef) * arr[i,j]
    # plt.plot(arr[::skipline,0],filtered)
    
    plt.title('Performance Of The Agent')
    plt.xlabel('Number of Bellman Update')
    plt.ylabel('Cumulative Reward')
    plt.savefig('log/'+freshfolder+"/"+splitext(basename(filename))[0]+".svg", format="svg")


###################################
# Report #2
###################################

reportfilename = 'log/'+freshfolder+'/agentReport2.csv'

if os.path.isfile(reportfilename):

	# Get related file and read via pandas
	df = pd.read_csv(reportfilename)
	arr = df.values

	# Inform The User
	print("REPORT Type#2");
	print("Related File:", freshfolder,"/agentReport2.csv")

	# Loop through each data and scatter
	records = arr.shape[0]
	if records>100 :
		skipline = (records // 100)
	else :
		skipline = 1
	print('Total Records',records)

	f2= plt.figure()
	ax2 = f2.add_subplot(111)

	# Draw Moves vs Game
	plt.plot(arr[::skipline,0],arr[::skipline,1],color='LimeGreen')
	plt.title('Performance Of The Agent')
	plt.xlabel('Game #')
	plt.ylabel('Player Moves')
	plt.savefig('log/'+freshfolder+"/agentReport_gamevsmove.svg", format="svg")



	f3= plt.figure()

	plt.plot(arr[::skipline,0],arr[::skipline,2],color='OrangeRed')
	plt.title('Exploration-Exploitation Parameter')
	plt.xlabel('Game #')
	plt.ylabel('E-Greedy Epsilon')
	plt.savefig('log/'+freshfolder+"/agentReport_epsilon.svg", format="svg")

