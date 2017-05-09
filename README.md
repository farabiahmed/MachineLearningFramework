## Used Tools
- Basic writing and formatting syntax: Markdown, Prose.io
- Text Editor: Vim, SublimeText, Eclipse, Atom
- Compiler: G++
- Debugger: GDB
- Visual Debugger: Eclipse
- UML Drawings: graphviz, doxygen-gui
- Plotting: matplotlib
- Memory Leak Check: valgrind 


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
├──── test
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

Any libs that get compiled by the project, third party or any needed in development. Prior to deployment, third party -libraries get moved to /usr/local/lib where they belong, leaving the project clean enough to compile on our Linux deployment servers. I really use this to test different library versions than the standard.

- test: All test code files that you write to test modules, classes, etc. Not the real application

- doc: The folder that stores documentation of project. Text files, uml diagrams and doxygen output etc. 

- scripts: The files you need to run to automating some processes

- makefile: Shell commands to automate compiling and linking of source codes and objects.

- README.md: Markdown styled file

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
