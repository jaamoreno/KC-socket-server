#################
# 'make' build parameters
#################
MAKEFILE = makefile

SHELL = /bin/sh

APPDIR = ..
APPINC1 = $(APPDIR)/libs/arqtraza
APPINC2 = $(APPDIR)/libs/tcputil 
APPLIB1 = $(APPDIR)/libs/arqtraza
APPLIB2 = $(APPDIR)/libs/tcputil

#################
# Compiler parameters
#################

COMPILER_FLAGS = -I. -I$(APPINC1) -I$(APPINC2) $(CFLAGS) $(shell mysql_config --cflags)

#################
# Header file parameters 
#################

#    Uncomment this if you want system header files to be expanded
#
# SYSHDRS       =

EXTHDRS	      = arqtraza.h \
                tcputil.h

HDRS	      = supervisor.h

#################
# Loader and options to be used for the final program link step.
#################

LINKER_FLAGS = $(LDFLAGS)

#################
# Library parameters
#################

LIBS          =  -lpthread $(APPLIB1)/libarqtraza.a $(APPLIB2)/libtcputil.a $(shell mysql_config --libs | cut -f1,2 -d' ')

#################
# Program parameters
#################
PROGRAM     = supervisor
SRCS	    = supervisor.c
OBJS	    = supervisor.o

#################
# Target Rules
#################

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) $(MAKEFILE)
		$(CC) $(COMPILER_FLAGS) -o $(OBJS) -c $(SRCS) 
		$(CC) $(LINKER_FLAGS) -o $(PROGRAM) $(OBJS) $(LIBS)
		@echo "done"

clean:
	@rm -f $(OBJS) core

clobber:
	@rm -f $(OBJS) $(PROGRAM) core

echo:
	@echo $(HDRS) $(SRCS)

.c.o:
	@echo "(*)Compilando $*.c..."
	$(CC) $(COMPILER_FLAGS) -c $*.c

