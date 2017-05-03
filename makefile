#
# TODO: - Add more comments
#		- Edit clean to remove unittest outs
#		- Edit unittest section more subtly
#

# make       -> compile the shared library "libfoo.so"
# make clean -> remove the library file and all object files (.o)
# make all   -> clean and compile
#
# Useful Notes:
# Comments doesn't start with tab.
# The "@" sign at the begining of the message suppresses messages. "@command"
# 
# Quick Guide
#
# A Common Rule Structure:
# target : prequisite1 prequisite2
#
# Dictionary:
# dependency = prequisite
#
# Automatic Variables:
# $@ output variable (file name of the target)
# $< first input variable (name of the first prequisite)
# $^ all prequisites seperated by spaces.
#
# Functions for string substitution and analysis:
#
# - $(patsubst pattern,replacement,text)
# 		$(patsubst %.c,%.o,x.c.c bar.c)
#		output: x.c.o bar.o
#
# - $(basename names…) -> Remove suffix
#		$(basename src/foo.c src-1.0/bar hacks)
#		output: src/foo src-1.0/bar hacks
#
# - $(notdir names…) -> Remove directory part of string
#		$(notdir src/foo.c hacks)
#		output: foo.c hacks
#
#
CC 	:= g++ # This is the main compiler
# LD 	:= g++

SRCDIR			:= src
BUILDDIR		:= build
BINDIR			:= bin
TARGET 			:= bin/main

SRCEXT 			:= cpp
SOURCES 		:= $(shell find $(SRCDIR)/* -type f -name *.$(SRCEXT))


UNITTESTDIR 	:= unittest
UNITTESTSRC 	:= $(shell find $(UNITTESTDIR)/* -type f -name *.$(SRCEXT))
UNITTESTBINS 	:= $(notdir $(basename $(UNITTESTSRC)))

# src/Letters/A.cpp
# src/main.cpp


OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS 	:= -g -O0 -std=c++11 -Wall
LIB 	:= -pthread -lpython2.7
INC 	:= -I include -I /usr/include/python2.7/ 

# All Target
all: $(TARGET) unittest

# Print Internal Variables for Debug
verbose: 
	@echo "CC:\t"		$(CC)
	@echo "CFLAGS:\t" 	$(CFLAGS)
	@echo "LIB: \t"		$(LIB)
	@echo "INC: \t"		$(INC)
	@echo "TARGET: " 	$(TARGET)
	@echo "SRCEXT: " 	$(SRCEXT)
	@echo "UNITTESTBINS: " 	$(UNITTESTBINS)
	@echo "UNITTESTSRC: "	$(UNITESTSRC)	
	@echo "SOURCES: " 		$(SOURCES)
	@echo "OBJECTS: " 		$(OBJECTS)

# Main Linkage
$(TARGET): $(OBJECTS)
	@echo "Linking..." $@;	
	@mkdir -p $(BINDIR)
	@$(CC) $^ -o $(TARGET) $(LIB)


# Main Compilation
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo "Compiling..." $@;	
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Tests
#tester:  $(filter-out build/main.o, $(OBJECTS))
#	@mkdir -p $(dir $(BINDIR))	
#	$(CC) $(CFLAGS) $^ unittest/SmartVector_unittest.cpp $(INC) $(LIB) -o $(BINDIR)/SmartVector_unittest
#	$(CC) $(CFLAGS) $^ unittest/Probabilities_unittest.cpp $(INC) $(LIB) -o bin/Probabilities_unittest
#	$(CC) $(CFLAGS) $^ unittest/Hastable_unittest.cpp $(INC) $(LIB) -o bin/Hastable_unittest
#	$(CC) $(CFLAGS) $^ unittest/String_unittest.cpp $(INC) $(LIB) -o bin/String_unittest
#	$(CC) $(CFLAGS) $^ unittest/ConfigParser_unittest.cpp $(INC) $(LIB) -o bin/ConfigParser_unittest
#	$(CC) $(CFLAGS) $^ unittest/NeuralNet_unittest.cpp $(INC) $(LIB) -o bin/NeuralNet_unittest
#	$(CC) $(CFLAGS) $^ unittest/matplotlib_unittest.cpp $(INC) $(LIB) -o bin/matplotlib_unittest
	
unittest: $(filter-out build/main.o, $(OBJECTS))
	@mkdir -p $(dir $(BINDIR))	
#	Example: $(CC) $(CFLAGS) $^ unittest/matplotlib_unittest.cpp $(INC) $(LIB) -o bin/matplotlib_unittest
#	$(foreach f,$(UNITTESTBINS), $(CC) $(CFLAGS) $^ $(UNITTESTDIR)/$(addsuffix .cpp,$(f)) $(INC) $(LIB) -o $(BINDIR)/$(f);)
	@for f in $(UNITTESTBINS) ; do \
	    echo "Linking: " $$f ; \
	    $(CC) $(CFLAGS) $^ $(UNITTESTDIR)/$(addsuffix .cpp,$$f) $(INC) $(LIB) -o $(BINDIR)/$$f; \
	done

test: $(filter-out build/main.o, $(OBJECTS))
	@for f in 1 2 3 4; do \
	    echo $$f ; \
	done
	
clean:
	@echo " Cleaning...";
	@$(RM) -r -v $(BUILDDIR) $(TARGET) $(BINDIR)
	@mkdir -p $(BINDIR)	
	@mkdir -p $(BUILDDIR)
	@echo " Done.";
	
	
# Rebuild
rebuild: clean all

# What is PHONY?
# Make assumes its target is a file, and this makes writing Makefiles relatively easy
# Special targets are called phony and you can explicitly tell Make they're not associated with files, e.g.,
# .PHONY: clean
# clean:
# 	rm -rf *.o
# Now make clean will run as expected even if you do have a file named clean.

.PHONY: clean
