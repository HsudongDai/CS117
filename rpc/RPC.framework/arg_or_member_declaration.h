// --------------------------------------------------------------
//
//                        arg_or_member_declaration.h
//
//        Author: Noah Mendelsohn
//
//        Classes:
//
//        Declaration: represents a function argument or structure member 
//
//        NOTE: Constructors remember declarations global state but
//        do not add to it. Initialize functions do.    
//
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------

#ifndef ___ARGUMENT_DECLARATION_H_INCLUDED__  
#define ___ARGUMENT_DECLARATION_H_INCLUDED__  


#include "declaration.h"
#include "typedeclaration.h"
#include "IDLToken.h"
#include <stack>

using namespace std;


//--------------------------------------------------------
//                 Arg_or_Member_Declaration
//
//     Note: argument declarations don't inherit from
//     declaration class. Live in arguments vector of function
//     only.
//
//--------------------------------------------------------


class Arg_or_Member_Declaration : public Declaration {
 private:

 protected:
  TypeDeclaration* type;      // pointer to the type for this argument

 
 public:
  Arg_or_Member_Declaration(Declarations* declp) :  Declaration(declp)  {}; // constructor
  virtual ~Arg_or_Member_Declaration() {};       // destructor
  
  // 
  // Parses tokens to create a function argument or struct member
  // declaration. Does not add it to the list for the containing
  // function/struct. 
  //
  // itemDelim will be , for arg lists and ; for structs
  // listDelim will ne ) for arg lists and } for structs
  //
  // NEEDSWORK: check for duplicate names?
  //
  virtual void initialize(TokenizedDeclaration& td, int& nextTokenIndex, const int numTokens);

  // 
  // parseType
  //
  // Helper routine for initialize to parse the type of an argument or member
  //					       
  // Side effects:
  //
  //   1) If array syntax is used, and similar arrays are not already in the
  //      type table, the type is added with a name like int[10][4]
  //
  //   2) nextTokenIndex is advanced to the token (if any) following the declaration
  //
  // Return value:
  //   Pointer to the type declaration, which may or may not be newly created
  //
  virtual TypeDeclaration* parseType(TokenizedDeclaration& td, int& nextTokenIndex, const int numTokens);

  virtual TypeDeclaration* getType() {return type;};

  //
  //  parseArrayDecl
  //
  //    parses one [..] construct returning the bounds
  //    parses one [..] construct with side effects as described above.
  //
  unsigned int parseArrayDecl(TokenizedDeclaration& td, int& nextTokenIndex, const int numTokens);

  //
  //  constructArrayDecl
  //
  //    Constructs a new array type, with the specified bound
  //
  TypeDeclaration* constructArrayDecl(TypeDeclaration* memberType, unsigned int bound);

  //
  // to_string_stream: formats the declaration for debugging output
  //
  void to_string_stream(stringstream& ss);
};

//
typedef vector<Arg_or_Member_Declaration*> ArgumentVector;

  


#endif
