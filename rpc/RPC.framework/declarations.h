// --------------------------------------------------------------
//
//                        declarations.h
//
//        Author: Noah Mendelsohn
//
//        Represents an IDL file (typically a simple .h file)
//        as a C++ Vector of objects, each of which represents
//        a single tokenized declaration
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------

#ifndef __DECLARATIONS_H_INCLUDED__  
#define __DECLARATIONS_H_INCLUDED__  




#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include "c150exceptions.h"
#include "declaration.h"
#include "tokenizeddeclaration.h"
#include "tokenizeddeclarations.h"

using namespace std;
using namespace C150NETWORK;

// ************************************************************************
//      typedefs: TypeMap and FunctionMap
// ************************************************************************

class TypeDeclaration;
class FunctionDeclaration;

typedef std::map<std::string,TypeDeclaration*> TypeMap ;
typedef std::map<std::string,FunctionDeclaration*> FunctionMap;



class Declarations {
 public:
   TypeMap types;
   FunctionMap functions;
  
   TokenizedDeclarations tokenizedDecls;                // One entry per declaration


 
 public:
  Declarations(istream& is);                // constructor
  virtual ~Declarations();            // destructor

  // add a type to the map of types
  virtual void addType(string name, TypeDeclaration& typeRef);
  // see if type exists in map
  virtual  bool typeExists(string name);

  // add a function to the map of functions
  virtual void addFunction(string name, FunctionDeclaration& functionRef);

  // see if function exists in map
  virtual  bool functionExists(string name);

};

#endif
