# Makefile for COMP 117 RPC Framework
#
#    Copyright 2012 - Noah Mendelsohn
#
#
# Useful targets:
#
#
#    c150idsrpc.a   - comp150ids RPC library
#
#    clean       - clean out all compiled object and executable files
#    all         - (default target) make sure everything's compiled
#

# Do all C++ compies with g++
CPP = g++
AR = ar r
CPPFLAGS = -g -Wall -Werror -I$(C150LIB) 


# Where the COMP 150 shared utilities live, including c150ids.a and userports.csv
# Note that environment variable COMP117 must be set for this to work!

C150LIB = $(COMP117)/files/c150Utils/
C150AR = $(C150LIB)c150ids.a
C150IDSRPC = $(COMP117)/files/RPC.framework/
C150IDSRPCAR = $(C150IDSRPC)c150idsrpc.a

LDFLAGS = 
INCLUDES = $(C150LIB)c150streamsocket.h $(C150LIB)c150network.h $(C150LIB)c150exceptions.h $(C150LIB)c150debug.h $(C150LIB)c150utility.h IDLToken.h tokenizeddeclarations.h  tokenizeddeclaration.h declarations.h declaration.h functiondeclaration.h typedeclaration.h arg_or_member_declaration.h 

all: c150idsrpc.a

c150idsrpc.a: tokenizeddeclarations.o IDLToken.o tokenizeddeclaration.o declaration.o declarations.o functiondeclaration.o arg_or_member_declaration.o typedeclaration.o  $(C150AR) $(INCLUDES) Makefile
	$(AR) c150idsrpc.a tokenizeddeclarations.o IDLToken.o tokenizeddeclaration.o declaration.o declarations.o functiondeclaration.o arg_or_member_declaration.o typedeclaration.o 
	chmod 644 c150idsrpc.a

########################################################################
#
#                   Housekeeping
#
########################################################################


# make .o from .cpp

generate_stub: generate_stub.o $(INCLUDES) $(C150AR) $(C150IDSRPCAR)
	$(CPP) -o generate_stub generate_stub.o $(CPPFLAGS) $(C150AR) $(C150IDSRPCAR)

%.o:%.cpp  $(INCLUDES)
	$(CPP) -c  $(CPPFLAGS) $< 


# clean up everything we build dynamically (probably missing .cpps from .idl)
clean:
	 rm -f  *.o c150idsrpc.a

