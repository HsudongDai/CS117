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
RPCGEN = ./rpcgenerate

all: c150idsrpc.a idl_to_json rpcgenerate arithmeticclient arithmeticserver

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
	$(CPP) $(CPPFLAGS) -c utility.cpp -o utility.o
	$(CPP) -o generate_stub utility.o generate_stub.o $(CPPFLAGS) $(C150AR) $(C150IDSRPCAR)

generate_proxy: generate_proxy.o $(INCLUDES) $(C150AR) $(C150IDSRPCAR)
	$(CPP) $(CPPFLAGS) -c utility.cpp -o utility.o
	$(CPP) -o generate_proxy utility.o generate_proxy.o $(CPPFLAGS) $(C150AR) $(C150IDSRPCAR)

# Compile the rpcgenerate program
rpcgenerate: rpcgenerate.o generate_stub.o generate_proxy.o utility.o $(C150AR) $(C150IDSRPCAR) $(INCLUDES)
	$(CPP) $(CPPFLAGS) -c utility.cpp -o utility.o
	$(CPP) -o generate_proxy.o -c generate_proxy.cpp $(CPPFLAGS)
	$(CPP) -o generate_stub.o -c generate_stub.cpp $(CPPFLAGS) 
	$(CPP) -o rpcgenerate.o -c rpcgenerate.cpp $(CPPFLAGS)
	$(CPP) -o rpcgenerate utility.o generate_proxy.o generate_stub.o rpcgenerate.o $(C150AR) $(C150IDSRPCAR) $(CPPFLAGS)

%client: %.o %.proxy.o rpcserver.o rpcproxyhelper.o %client.o %.proxy.o base64.o
	$(CPP) -o $@ $@.o rpcproxyhelper.o $*.proxy.o base64.o $(C150AR) $(C150IDSRPCAR) 

%server: %.o %.stub.o rpcserver.o rpcstubhelper.o %.stub.o base64.o
	$(CPP) -o $@ rpcserver.o $*.stub.o $*.o rpcstubhelper.o base64.o $(C150AR) $(C150IDSRPCAR)

%.proxy.cpp %.stub.cpp:%.idl
	$(RPCGEN) $< 

%.o:%.cpp  $(INCLUDES)
	$(CPP) -c  $(CPPFLAGS) $< 


# clean up everything we build dynamically (probably missing .cpps from .idl)
clean:
	rm -f  *.o c150idsrpc.a rpcgenerate generate_stub generate_proxy
	
