#ifndef __IDL_TO_JSON_HPP__
#define __IDL_TO_JSON_HPP__

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "c150exceptions.h"
#include "declarations.h"
#include "functiondeclaration.h"
#include "typedeclaration.h"

using namespace std;

// forward function declarations

void processIDLFile(const char fileName[]);
void indent_it(int ind);
void print_arglist_or_members(vector<Arg_or_Member_Declaration *>& members, 
			      int indent);
void print_type(TypeDeclaration *typep, int indent);
void print_function(FunctionDeclaration *functionp, int indent, 
		    bool writeCommaFunction);
void print_type_of_type(const char * type_of_type);

const int INDENT = 4;            // Amount to indent each block of JSON

#endif