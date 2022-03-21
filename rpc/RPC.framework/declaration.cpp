// --------------------------------------------------------------
//
//                        Declaration.cpp
//
//        Author: Noah Mendelsohn
//
//        Classes:
//
//        Declaration: Represents a parsed declaration of a type or function
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------



#include "declaration.h"     

// NOTHING HERE YET. EVERYTHING IN .H FILE OR SUBCLASSES

  // constructors
  // (Note: arguments don't go in the declarations table)
Declaration::Declaration() :initialized(false), name(""), declarationsp(NULL) {};  // for arguments

Declaration::Declaration(Declarations* declp) :initialized(false), name(""),
    declarationsp(declp) {}; 

Declaration:: Declaration(Declarations* declp, string nm) :initialized(false), name(nm),  declarationsp(declp) {}; 

Declaration::~Declaration() {};       // destructor

