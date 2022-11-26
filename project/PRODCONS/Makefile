#   Makefile - Steven J Zeil, Old Dominion University
#              June 1996
#
#  This is a "self-building" makefile for use in any project where the
#  gcc/g++ compilers are to be used to build a single program.
#
#  What does it do for you?  It keeps track of the #include dependencies
#  as your program evolves.  If you add or delete an #include in your
#  source code, you don't need to alter this makefile.  It will
#  note the change and automatically produce your program with the minimal
#  amount of re-compilation.
#
#
#  Copy this file into your project directory.
#  Your directory should contain all the .c, .C, .cc, or .cpp files needed
#  for your program.
#
#  Edit your copy of this Makefile, supplying the appropriate values for
#  the CFLAGS, CPPFLAGS, TARGET, OBJS, LINK, and LFLAGS variables.
#
#  The command
#     make
#  will actually compile and build your program.
#  (The first time you use this Makefile in any directory, you may get
#  a warning about a missing "make.dep" file.  Ignore this message. This 
#  missing file will be generated for you.)
#
#  The command
#     make clean
#  will "clean up" your directory by deleting the executable program and
#  the various by-products of the compilation process.  Your source code
#  will be left unharmed.
#
#
########################################################################
# Macro definitions for "standard" C and C++ compilations
#
# Edit the next 6 definitions. After that, "make" will
#   build the program.
# 
#  Define special compilation flags for C++ compilation. These may change when
#  we're done testing and debugging.
CPPFLAGS=-g -std=c++11 -Wall
# For more run-time error checking, use the following instead
# CPPFLAGS=-g -std=c++11 -Wall -D_GLIBCXX_DEBUG
# For still more error-checking, on Linux systems, use:
# CPPFLAGS=-g -std=c++11 -Wall -fsanitize=address -D_GLIBCXX_DEBUG
#
#  Define special compilation flags for C compilation. These may change when
#  we're done testing and debugging.
CFLAGS=-fdiagnostics-color=always -g 
 
#  What is the name of the program you want to create?  (See below for notes
#     on using this makefile to generate multiple programs.)
# TARGET=main

#  List the object code files to be produced by compilation. Normally this 
#  list will include one ".o" file for each C++ file (with names ending in 
#  ".cpp", ".cc" or ".C"), and/or each C file (with names ending in ".c").
#  Do NOT list .h files. For example, if you are building a program from 
#  source files foo.c, foo.h, bar.cpp, baz.cc, and bam.h you would use
#      OBJS1=foo.o bar.o baz.o
OBJS=sender.o receiver.o

#  What program should be used to link this program? If the program is
#  even partly C++, use g++.  If it is entirely C, use gcc.
# LINK=g++ $(CPPFLAGS)
LINK=gcc $(CFLAGS)

#  Define special linkage flags.  Usually, these are used to include
#  special libraries of code, e.g., -lm to add the library of mathematical
#  routines such as sqrt, sin, cos, etc.
LFLAGS=-lm

#  In most cases, you should not change anything below this line.
#
#  The following is "boilerplate" to set up the standard compilation
#  commands:
#
.SUFFIXES:
.SUFFIXES: .d .o .h .c .cc .C .cpp
.c.o: ; $(CC) $(CFLAGS) -MMD -c $*.c
.cc.o: ; $(CPP) $(CPPFLAGS) -MMD -c $*.cc 
.C.o: ; $(CPP) $(CPPFLAGS) -MMD -c $*.C
.cpp.o: ; $(CPP) $(CPPFLAGS) -MMD -c $*.cpp

CC=gcc
CPP=g++

%.d: %.c
	touch $@
%.d: %.cc
	touch $@
%.d: %.C
	touch $@
%.d: %.cpp
	touch $@

DEPENDENCIES = $(OBJS:.o=.d)

# 
# Targets:
# 
all: sender receiver

$(TARGET): $(TARGET).cpp $(OBJS)
	$(LINK) $(FLAGS) -o $(TARGET) $(TARGET).cpp $(OBJS) $(LFLAGS)

debug: $(TARGET).cpp $(OBJS)
	$(LINK) $(FLAGS) -DDEBUG -o $(TARGET) $(TARGET).cpp $(OBJS) $(LFLAGS)

# $(TARGET).cpp: $(OBJS)
# 	$(LINK) $(FLAGS) -o $(TARGET) $(TARGET).cpp $(OBJS) $(LFLAGS)

clean:
	-rm -f $(TARGET) $(OBJS) $(DEPENDENCIES) *Test make.dep


make.dep: $(DEPENDENCIES)
	-cat $(DEPENDENCIES) > make.dep

include make.dep


