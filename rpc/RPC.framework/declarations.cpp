// --------------------------------------------------------------
//
//                        declarations.cpp
//
//        Author: Noah Mendelsohn
//
//        Class: Declarations
//
//        Represents the root of the parse tree for an IDL file.
//
//        Public data members:
//
//          types:       map from type name to TypeDeclaration*
//          functions:   map from function name to FunctionDeclaration*
//
//        Public member functions:
//
//          typeExists:     returns true if named type exists
//          functionExists: returns true if named function exists
//
//       NEEDSWORK:
//
//          Destructors of various classes used by this framework
//          are not written to free dynamically allocated objects.
//          Valgrind will report numerous examples of unreclaimed memory.
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------



#include <fstream>
#include <iostream>
#include "c150exceptions.h"
#include "declarations.h"
#include "functiondeclaration.h"
#include "typedeclaration.h"

using namespace std;



// --------------------------------------------------------------
//                       Constructor
//
//      Builds the token list by calling the constructor
//      of tokens, which is of type TokenizedDeclarations
// --------------------------------------------------------------

Declarations::Declarations(istream& is) : tokenizedDecls(is) {
  unsigned int i;
  string functionName;

  //
  // Put standardized types into type list.
  //
  types["void"] = new TypeDeclaration(this,"void");
  types["int"] = new TypeDeclaration(this,"int");
  types["string"] = new TypeDeclaration(this,"string");
  types["float"] = new TypeDeclaration(this,"float");

  //
  // Parse all the tokenized declarations
  //
  for (i=0; i<tokenizedDecls.size(); i++) {
    TokenizedDeclaration& td = tokenizedDecls[i];
    //
    // If it's a function
    //
    int nextTokenIndex = 0;
    if (FunctionDeclaration::isPossibleFunctionDeclaration(td, 0, td.size())){
      FunctionDeclaration *fdp = new FunctionDeclaration(this);
      fdp -> initialize(td, nextTokenIndex, td.size());
      string functionName = fdp->getName();
      if (functionExists(functionName)) {
	stringstream ss;
	ss << "Duplicate function name: " << functionName << endl << "  " << td.str() << endl;
	throw C150Exception(ss.str());
      }
      functions[functionName] = fdp;  // register in functions table

    } else if (TypeDeclaration::isPossibleStructDeclaration(td, 0, td.size())){
      TypeDeclaration *typep = new TypeDeclaration(this,td, nextTokenIndex, td.size());
      string typeName = typep->getName();
      if (typeExists(typeName)) {
	stringstream ss;
	ss << "Duplicate type name: " << typeName << endl << "  " << td.str() << endl;
	throw C150Exception(ss.str());
      }
      types[typeName] = typep;  // register in functions table

    } else  {
      stringstream ss;
      ss << "Illegal declaration: " << td.str() << endl;
      throw C150Exception(ss.str());
    }


    // 
    // NEEDSWORK: need to check for missing semicolons at decl ends
    //
//    cerr << td.str() << endl << "Declarations::Declarations:: end of function/struct parse nextTokenIndex=" << nextTokenIndex <<     " td.size()=" << (int) td.size() << " points to " << td[nextTokenIndex]->getText() << ((nextTokenIndex+1 < (int) td.size())?td[nextTokenIndex+1]->getText() : " OFF-END") << endl;

  }
};

// --------------------------------------------------------------
//                      destructor
//
// --------------------------------------------------------------

Declarations::~Declarations() {};

// --------------------------------------------------------------
//                      typeExists
//
//     True iff there's an entry in the type map for this name
//
// --------------------------------------------------------------

bool Declarations::typeExists(string name) {
  return (types.count(name) == 1);
};

// --------------------------------------------------------------
//                      functionExists
//
//     True iff there's an entry in the function map for this name
//
// --------------------------------------------------------------

bool Declarations::functionExists(string name) {
  return (functions.count(name) == 1);
};



// --------------------------------------------------------------
//                      addType
//
//    add a type to the map of types
//
// --------------------------------------------------------------

void Declarations::addType(string name, TypeDeclaration& typeRef) {
    if (typeExists(name)) {
      stringstream ss;
      ss << "Type " << name << " registered more than once in type table";
      throw C150Exception(ss.str());
    }
    types[name] = &typeRef;
  };



// --------------------------------------------------------------
//                      addFunction
//
//    add a function to the map of functions
//
// --------------------------------------------------------------

void Declarations::addFunction(string name, FunctionDeclaration& functionRef) {
    if (functionExists(name)) {
      stringstream ss;
      ss << "Function " << name << " registered more than once in function table";
      throw C150Exception(ss.str());
    }
    functions[name] = &functionRef;
  };





