// --------------------------------------------------------------
//
//                        declaration.h
//
//        Author: Noah Mendelsohn
//
//        Classes:
//
//        Declaration: Represents a parsed declaration of a type or function
//
//        NOTE: Constructors remember declarations global state but
//        do not add to it. Initialize functions do.    
//
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------

#ifndef ___DECLARATION_H_INCLUDED__  
#define ___DECLARATION_H_INCLUDED__  


#include <iostream>
#include <vector>
#include "declarations.h"
#include "tokenizeddeclaration.h"
#include "c150exceptions.h"
#include "IDLToken.h"

using namespace std;

class Declarations;

//--------------------------------------------------------
//                 Declaration (Base Class)
//--------------------------------------------------------

class Declaration {
 protected:
  bool initialized;
  string name;         // name of the type, function, etc.

  // NEEDSWORK, do we need Declarationsp to be retained? Think not. but for now
  // we do
  Declarations* declarationsp; 
                            // Lists of all decls - this pointer
                            // not used for arguments, which are
                            // scoped to function



 
 public:
  // constructors
  // (Note: arguments don't go in the declarations table)
  Declaration();  // for arguments
  Declaration(Declarations* declp); 
  Declaration(Declarations* declp, string nm); 

  virtual ~Declaration();       // destructor

  // call to construct from token stream, returns index of next token
  // Only some declarations support this form of initialize
  //
  virtual int initialize(TokenizedDeclaration& td, const int firstToken, const int numTokens) {
    throw C150Exception("Initialize(td, firsttoken) called on declaration that does not support it");
  }

  // all declarations have names
  virtual string getName() {return name;};
  virtual void setName(string& s) {name = s;};
  
};



#endif
