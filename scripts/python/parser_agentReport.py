from parse import parse
import pandas as pd
import numpy as np
import os
import sys #for commandline args
from datetime import datetime
import os.path
from os.path import basename
from os.path import splitext
import getopt
import matplotlib.pyplot as plt
import matplotlib
from matplotlib import rcParams
rcParams.update({'figure.autolayout': True})

# Usage:
# python3.4 scripts/python/parser_agentReport.py

# If you want to skip some records try this
# python3.4 scripts/python/parser_agentReport.py 10

# Get The Newest Folder form given path
path = "log"
inputFolder = ""
upperlimitGiven = None
lowerlimitGiven = None
multiplierGiven = None
titles = None
lowerlimit = 0
upperlimit = 0
band = False;
skiplineGiven = None
myfigsize = (7, 3)
xscale_log = False;
uncertainty = False;

print(sys.argv)

try:
   opts, args = getopt.getopt(sys.argv[1:],"xhbci:s:l:u:m:o:t:")
   print("Opts: ", opts)
   print("Args: ", args)
except getopt.GetoptError:
   print ('Check input params')
   sys.exit(2)
for opt, arg in opts:
    if opt == '-h':
        print ('parser_agent.py -i <inputfolder>')
        print ('parser_agent.py -i <inputfolder> -s <skipline>')
        sys.exit()
    elif opt in ("-i", "--inputfolder"):
        inputFolder = arg
    elif opt in ("-s", "--skip"):
        skiplineGiven = [int(i) for i in arg.split(',')]
        print("skiplineGiven: ", skiplineGiven)
    elif opt in ("-u", "--upper"):
        upperlimitGiven = [int(i) for i in arg.split(',')]
        print("upperlimitGiven: ", upperlimitGiven)
    elif opt in ("-l", "--lower"):
        lowerlimitGiven = [int(i) for i in arg.split(',')]
        print("lowerlimitGiven: ", lowerlimitGiven)
    elif opt in ("-m", "--multiplier"):
        multiplierGiven = [float(i) for i in arg.split(',')]
        print("multiplierGiven: ", multiplierGiven)
    elif opt in ("-o", "--offset"):
        offsetGiven = [float(i) for i in arg.split(',')]
        print("offsetGiven: ", offsetGiven)
    elif opt in ("-b", "--band"): # Create uncertainty band
        band = True
    elif opt in ("-x", "--xlog"): # Logarithmic
        xscale_log = True
    elif opt in ("-c", "--certainty"):  # Disable uncertainty
        uncertainty = True
    elif opt in ("-t", "--title"):
        titles = arg.split(',')
        print("titles: ", titles)

print('ARGV      :', sys.argv[1:])
print('OPTIONS   :', opts)

if len(inputFolder) < 1:
    print ('Getting newest simulation folder...')
    
    # Get subdirectories
    subdirectories = os.listdir(path)

    # Sort the list to find newest one
    subdirectories.sort(key=lambda date: datetime.strptime(date, "%Y%m%d_%H%M%S"))

    print("Subdirectories:")
    print(subdirectories)

    # Get the newest directory
    inputFolder = subdirectories[-1]

if not os.path.exists(path + '/' + inputFolder):
    print (path)
    print (inputFolder)
    print ('Check input params, file not found!')
    sys.exit(2)
    skiplineGiven = None

print("Current Simulation To Process:", path + '/' + inputFolder)

with open(path + '/' + inputFolder + '/' + "arguments.txt", "a") as myfile:
    myfile.write("New config:\n")
    myfile.write("###########\n")
    myfile.write(str(opts)+"\n")
    myfile.write(str(args)+"\n")
    myfile.write("\n")
    myfile.write("\n")


# 'agentReport_15.csv', 'agentReport_1.csv', 'agentReport_7.csv',....
modelFiles = [filename for filename in os.listdir(path + '/' + inputFolder) if filename.startswith("agentReport_") and filename.endswith(".csv")]

if len(modelFiles) == 0:
    modelFiles = ["agentReport.csv"]
else:
    modelFiles.sort(key=lambda str: int(splitext(basename(str))[0].split('_')[1]))
    
print("modelFiles", modelFiles)

f2 = plt.figure(1,figsize=myfigsize)
ax2 = f2.add_subplot(111)

index = 0
color_cycle = plt.rcParams['axes.prop_cycle'].by_key()['color'] #different colors for each agent

for filename in modelFiles:
    ###################################
    # Report #i
    ###################################

    reportNumber = int(parse('agentReport_{}.csv', filename)[0])

    mycolor = color_cycle[reportNumber]
    print("Color: ", mycolor)

    index = index + 1

    # Inform The User
    targetLog = 'log/'+inputFolder+'/'+filename
    print("REPORT #", index);
    print("Related File:", targetLog)
    
    # Get related file and read via pandas
    df = pd.read_csv(targetLog,header=None,)
    arr = df.values
    
    # Loop through each data and scatter
    records = arr.shape[0]
    if upperlimitGiven is not None:
        if upperlimitGiven[index-1] == 0 : upperlimit = records
        elif upperlimitGiven[index-1] > records: upperlimit = records
        else : upperlimit=upperlimitGiven[index-1]
    else:
        upperlimit = records

    if lowerlimitGiven is not None:
        if lowerlimitGiven[index-1] > upperlimit : lowerlimit = 0
        else : lowerlimit = lowerlimitGiven[index-1]
    else:
        lowerlimit = 0

    if multiplierGiven is None:
        multiplier = 1
    else:
        multiplier = multiplierGiven[index-1]

    if offsetGiven is None:
        offset = 0
    else:
        offset = offsetGiven[index - 1]

    print('Total Records ',records)
    print('Lower Limit ', lowerlimit)
    print('Upper Limit ', upperlimit)
    print('multiplier ', multiplier)
    print('offset ', offset)

    arr[:, 1:]+=offset

    records = upperlimit - lowerlimit

    if skiplineGiven is None:
        if records>100 :
            skipline = (records // 100)
        else :
            skipline = 1
    else:
        skipline = skiplineGiven[index-1]

    print('Skipline:',skipline)
    indexlist = np.arange(lowerlimit,upperlimit,skipline)

    lowerlimitOffset = 0 #arr[indexlist[0],0]
    print("lowerlimitOffset", lowerlimitOffset)
    
    f1 = plt.figure(0, figsize=myfigsize)
    ax1 = f1.add_subplot(111)

    if uncertainty:
        for xe in indexlist:
            for ye in arr[xe,1:]:
                plt.scatter(arr[xe,0]-lowerlimitOffset, ye,color=mycolor, alpha=0.1)
                ax2.scatter(arr[xe, 0]-lowerlimitOffset, ye, color=mycolor, alpha=0.1)


    print('Arr:', arr)

    playedGameForEachEpisode = len(arr[0])-1
    print('Played Game for each episode', playedGameForEachEpisode)
    # Plot using matplotlib
    # Draw mean values

    avgArr = [np.sum(arr[i:i + skipline, 1:]) / skipline / playedGameForEachEpisode * multiplier
              if i + skipline < upperlimit
              else np.sum(arr[i:upperlimit,1:]) * multiplier / (upperlimit - i) / playedGameForEachEpisode
              for i in np.arange(lowerlimit, upperlimit, skipline)]

    #ax1.plot(arr[::skipline,0],np.mean(arr[::skipline,1:], axis=1))
    ax1.plot(arr[lowerlimit:upperlimit:skipline,0]-lowerlimitOffset, avgArr, color=mycolor)

    if band :
        avgArr_min = [ np.sum(arr[i:i + skipline, 1:].min(axis=1)) / skipline * multiplier
                        if i + skipline < upperlimit
                        else np.sum(arr[i:upperlimit, 1:].min(axis=1)) * multiplier / (upperlimit - i)
                        for i in np.arange(lowerlimit, upperlimit, skipline)]

        avgArr_max = [ np.sum(arr[i:i + skipline, 1:].max(axis=1)) / skipline * multiplier
                        if i + skipline < upperlimit
                        else np.sum(arr[i:upperlimit, 1:].max(axis=1)) * multiplier / (upperlimit - i)
                        for i in np.arange(lowerlimit, upperlimit, skipline)]

        print('Band is on')
        print(' avgArr_min', len(avgArr_min), " Contents: ", avgArr_min)
        print(' avgArr_max', len(avgArr_max), " Contents: ", avgArr_max)
        print(" avgArr", len(arr[::skipline,0]))

        # It is too wide to draw.
        ax1.fill_between(arr[lowerlimit:upperlimit:skipline,0]-lowerlimitOffset, avgArr_min, avgArr_max, color=mycolor, alpha=0.1)
        ax2.fill_between(arr[lowerlimit:upperlimit:skipline,0]-lowerlimitOffset, avgArr_min, avgArr_max, color=mycolor, alpha=0.1)


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
    plt.savefig('log/'+inputFolder+"/"+splitext(basename(filename))[0]+".svg", format="svg", dpi=250)
    plt.savefig('log/' + inputFolder + "/" + splitext(basename(filename))[0] + ".png", format="png", dpi=250)
    plt.close(f1)


    plt.figure(1,figsize=myfigsize)
    # skipline = 1
    #ax2.plot(range(lowerlimit,upperlimit,skipline),avgArr, color=mycolor, label = 'Model #'+ str(index))
    currentLabel = ('Model #' + str(index)) if titles is None else titles[index-1]
    ax2.plot(arr[lowerlimit:upperlimit:skipline,0]-lowerlimitOffset, avgArr, color=mycolor, label=currentLabel)

if xscale_log :
    ax2.set_xscale("log")
    #plt.xscale('log')
    #plt.xlim(xmin=1)  # this line

plt.ylim(ymax=1)  # this line
#plt.xlim(xmin=10000)  # this line
plt.figure(1,figsize=myfigsize)
plt.legend()
plt.title('Performance Of The Models')
#plt.xlabel('Game Number')
plt.xlabel('Number of Bellman Update')
plt.ylabel('Cumulative Reward')
plt.savefig('log/'+inputFolder+"/"+"combined"+".svg", format="svg", figsize=myfigsize)
plt.savefig('log/'+inputFolder+"/"+"combined"+".png", format="png", dpi=250)
plt.close(f2)

"""
###################################
# Report #2
###################################

reportfilename = 'log/'+inputFolder+'/agentReport2.csv'

if os.path.isfile(reportfilename):

    # Get related file and read via pandas
    df = pd.read_csv(reportfilename)
    arr = df.values

    # Inform The User
    print("REPORT Type#2");
    print("Related File:", inputFolder,"/agentReport2.csv")

    # Loop through each data and scatter
    records = arr.shape[0]
    if records>100 :
        skipline = (records // 100)
    else :
        skipline = 1
    print('Total Records',records)

    plt.figure(2)
    # Draw Moves vs Game
    plt.plot(arr[::skipline,0],arr[::skipline,1],color='LimeGreen')
    plt.title('Performance Of The Agent')
    plt.xlabel('Game #')
    plt.ylabel('Player Moves')
    plt.savefig('log/'+inputFolder+"/agentReport_gamevsmove.svg", format="svg")

    plt.figure(3)
    plt.plot(arr[::skipline,0],arr[::skipline,2],color='OrangeRed')
    plt.title('Exploration-Exploitation Parameter')
    plt.xlabel('Game #')
    plt.ylabel('E-Greedy Epsilon')
    plt.savefig('log/'+inputFolder+"/agentReport_epsilon.svg", format="svg")
    plt.close()


    
    plt.figure(4)
    avgArr = [np.sum(arr[i:i + skipline, 4]) / skipline if i + skipline < upperlimit else np.sum(arr[i:upperlimit,4]) / (upperlimit - i) for i in
         np.arange(lowerlimit, upperlimit, skipline)]
    plt.plot(arr[lowerlimit:upperlimit:skipline,0], avgArr)
    plt.title('Network Error')
    plt.xlabel('Game #')
    plt.ylabel('Error')
    plt.savefig('log/'+inputFolder+"/agentReport_error.svg", format="svg")
    plt.savefig('log/'+inputFolder+"/agentReport_error.png", format="png", dpi=250)
    plt.close()
    
"""