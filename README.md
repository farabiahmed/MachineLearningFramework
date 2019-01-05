# Machine Learning Framework
This project is started to make a warm and practicable introduction to 
various machine learning algorithms to develop next generation Multi-Agent 
UAV projects. Currently main focus is on the Deep-Q-Networks and 
Dynamic Programming applications.

## Getting Started


### Prerequisites
In order to utilize most of this project, currently you need to make those
softwares ready on your system. 
```
g++ (sudo apt install g++)
make (sudo apt install make)
Python3 (sudo apt install python3-dev python3-pip)
matplotlib (pip3 install -U matplotlib)
tensorflow (pip3 install --upgrade tensorflow)
pandas (pip3 install pandas)
python3-tk (sudo apt-get install python3-tk)
openCL
```
### Checking Installations of Python
```
pip3 list
```
### Installation of PyCharm Python IDE
```
sudo snap install pycharm-community --classic
```
### Java
```
sudo add-apt-repository ppa:webupd8team/java
sudo apt-get update
sudo apt-get install oracle-java8-installer
sudo apt-get install oracle-java8-set-default
java -version
```

## Deployment
### Installing
```
git clone https://github.com/farabiahmed/MachineLearningFramework.git
cd MachineLearningFramework
```
### Building
All operations including building, running, post-analysis, etc, should be operated from the root of the project:
```
cd /path/to/MachineLearningFramework
```

In order to build main of the project:
```
make main
```

In order to build an unittest from unittest folder:
```
make unittest/SmartVector_unittest
```
### Running an executable
To start new grid search:
```
./bin/main -CONFIG config/gridsearch_rendezvous.cfg
```
To continue from where you left off:
```
./bin/main -CONFIG config/gridsearch_rendezvous.cfg -TIME_STAMP 20180527_133325
```
To conduct single simulation:
```
./bin/RLFramework --CONFIG config/config_rendezvous_3x3_2agent.cfg --ENVIRONMENT rendezvous --AGENT trajectory-based-value-iteration --REPRESENTATION RepresentationUDP
```
To run tensorflow supported network:
```
python3 scripts/python/main.py 
```
To test other unittest examples
```
./bin/SmartVector_unittest
```

### Post Processing
This python script will produce all figures in '''log''' folder with related date.  
```
python3 scripts/python/parser_agentReport.py 
python3 scripts/python/parser_agentReport.py -i 20181228_230617 -s 250
```

## Developped Modules
* Agents : Some popular algorithms to solve MDPs.
  * Qiteration
  * TrajectoryBasedValueIteration
* Environments : Some domains in varying complexity to evaluate algorithms.
  * Gridworld
  * Blocksworld
* Representations : Various type of representation methods to represent value functions.
  * TabularStateActionPair
  * FunctionApproximatorBinaryBasis
  * FunctionApproximatorGaussianRadialBasis
  * FunctionApproximatorNeuralNetwork
* NeuralNets : It is a basis for Deep-Q-Networks.
* ProbabilityDistributions : Basic classes to take samples.
* Communication : It allows us to communicate with other tools including python and UAV-Simulator.
* Miscellaneous : Other supporting tools.
  * CommandLineParser : It helps to parse commandline argument (./bin/main --environment Gridworld)
  * ConfigParser : It parses .csv files located in config folder.
  * Convert : It helps to convert strings to various complex types including (vector<SmartVector>)
  * SmartVector : It plays a key role and mainly represents states and actions.
  * UserControl : It helps to interact human to stop everlasting iterations without killing the program. By that way you can still get end of program reports.


## Recommended Tools to Utilize
- Basic writing and formatting syntax: Markdown, Prose.io
- Text Editor: Vim, SublimeText, Atom
- Compiler: G++
- Debugger: GDB
- Visual Debugger: Eclipse
- UML Drawings: graphviz, doxygen-gui
- Plotting: matplotlib
- Memory Leak Check: valgrind
- Data Manipulation: Python, Numpy, Scikit


## Useful snippets:

### Get rid of backup files in subfolders:
  find . -type f -name '*~' -delete

### Installing documentation tools
  sudo apt-get install graphviz
  sudo apt-get install doxygen-gui

### Saving terminal output via colors to html
  ./bin/Blocksworld_unittest --color=always | aha  > output.html

### Valgrind
  valgrind --leak-check=full --show-reachable=yes --track-origins=yes -v ./bin/main
* --track-origins: to find uninitialized data
* --show-reachable: shows where the first leaked memory was allocated


## Folder Structure

 ```html
Project-Name  
| 
├──── bin 
│ 
├──── build 
│ 
├──── src  
|   | 
|   ├──── ModuleA 
|   |   |
|   |   ├──── handler.cpp 
|   |   | 
|   |   ├──── converter.cpp 
|   |  
|   ├──── ModuleB  
|   |   |
|   |   ├──── helper.cpp
|   |   |
|   |   ├──── builder.cpp
|   |   
|   ├──── main.cpp  
│
├──── include
|   | 
|   ├──── ModuleA
|   |   |
|   |   ├──── handler.hpp
|   |   |
|   |   ├──── converter.hpp
|   |   
|   ├──── ModuleB
|   |   |
|   |   ├──── helper.hpp
|   |   |
|   |   ├──── builder.hpp
|   |   
|   ├──── main.hpp
│
├──── lib
│
├──── unittest
│
├──── config
│
├──── doc
│
├──── scripts
│
├──── makefile
│
├──── README.md
│
├──── .git
```

- bin: For all executables (applications). The output of linking process can be found here like main application and other test executables.

- build: This folder contains all object files, and can be removed by make clean.

- src: Source files that the application will use must be stored here. Generally .cpp and .c.

- include: The folder that stores all necessary third-party header files and that do not exist under /usr/local/include.Generally .hpp and .h.

- lib: All other binaries like static and shared libraries (.so or .dll) must be stored here.

- config: Configuratior files to make programs parametric.

- unittest: All test code files that you write to test modules, classes, etc. Not the real application

- doc: The folder that stores documentation of project. Text files, uml diagrams and doxygen output etc. 

- scripts: The files you need to run to automating some processes. Especially post-processing python scripts are located here. 

- makefile: Shell commands to automate compiling and linking of source codes and objects.

- README.md: Markdown styled main introduction file

- .git: git repository folder


## Makefile

http://hiltmon.com/blog/2013/07/03/a-simple-c-plus-plus-project-structure/

- g++ -c *.cpp #compiling
- g++ *.o -o program.bin #linking 


### Common Macros
- CC: The name of the compiler

- DEBUG: The debugging flag. This is -g in both g++ and cxx. The purpose of the flag is to include debugging information into the executable, so that you can use utilities such as gdb to debug the code.

- LFLAGS: The flags used in linking. As it turns out, you don't need any special flags for linking. The option listed is "-Wall" which tells the compiler to print all warnings. But that's fine. We can use that.

- CFLAGS: The flags used in compiling and creating object files. This includes both "-Wall" and "-c". The "-c" option is needed to create object files, i.e. .o files.

## Authors
* **Farabi Ahmed TARHAN** - [LinkedIn](https://www.linkedin.com/in/farabi-ahmed-tarhan)
