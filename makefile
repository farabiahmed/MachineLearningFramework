# Makefile v1.3
#
# TODO: - Add dependency to files. Currently all .o files must be created to link.
#
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
#    $@  :    $<
#          |_______________________|
#                     $^
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


CFLAGS 	:= -g -O0 -std=c++11 -Wall
LIB 	:= -pthread -lpython2.7
INC 	:= -I include -I /usr/include/python2.7/ 

SRCDIR			:= src
SRCEXT 			:= cpp
BUILDDIR		:= build
BINDIR			:= bin
SOURCES 		:= $(shell find $(SRCDIR)/* -type f -name *.$(SRCEXT))				# src/Representations/Representation.cpp src/Representations/TabularStateActionPair.cpp src/main.cpp
OBJECTS 		:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o)) 	# build/Representations/Representation.o build/Representations/TabularStateActionPair.o build/main.o
TARGET 			:= main

UNITTESTDIR 	:= unittest
UNITTESTSRC 	:= $(shell find $(UNITTESTDIR)/* -type f -name *.$(SRCEXT))			# unittest/Vector_Permutation.cpp unittest/matplotlib_unittest.cpp ...
UNITTESTBINS 	:= $(notdir $(basename $(UNITTESTSRC))) 							# Vector_Permutation matplotlib_unittest ...
UNITTESTOBJS	:= $(UNITTESTSRC:.cpp=.o) 											# unittest/Vector_Permutation.o unittest/matplotlib_unittest.o ...

# All Target
all: $(TARGET) $(addprefix $(UNITTESTDIR)/,$(UNITTESTBINS))

# Get the variables to debug
verbose print:
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
	@echo "Linking....." $@;	
	@mkdir -p $(BINDIR)
	@$(CC) $^ -o $(BINDIR)/$@ $(LIB)

# Main Compilation
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo "Compiling..." $@;	
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INC) -c -o $@ $<


# Unittest Linkage
# Ex: 
# unittest/String_unittest : build/unittest/String_unittest.o
$(addprefix $(UNITTESTDIR)/,$(UNITTESTBINS)) : % : %.o $(OBJECTS) 
	@echo "Linking....." $@;	
	@mkdir -p $(BINDIR)
	@$(eval TMP := build/$@.o)		
	@$(CC) $(filter-out build/main.o, $(OBJECTS)) $(TMP) -o $(BINDIR)/$(notdir $@) $(LIB) 

# Unittest Compilation
# Ex:
# $(BUILDDIR)/unittest/String_unittest.o: unittest/String_unittest.cpp
$(UNITTESTOBJS) : $(UNITTESTSRC)
	@echo "Compiling..." $@;	
	@mkdir -p $(BUILDDIR)/$(UNITTESTDIR)
	@$(eval TMP := $(@:.o=.$(SRCEXT) ))										# Change .o with .cpp  Ex: String_unittest.o -> String_unittest.cpp
	@$(CC) $(CFLAGS) $(INC) -c $(TMP) -o $(BUILDDIR)/$@

# Testing Make
test: $(filter-out build/main.o, $(OBJECTS))
	@for f in 1 2 3 4; do \
	    echo $$f ; \
	done

# Clean
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