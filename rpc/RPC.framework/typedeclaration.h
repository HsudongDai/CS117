// --------------------------------------------------------------
//
//                        typedeclaration.h
//
//        Author: Noah Mendelsohn
//
//        Classes:
//
//        TypeDeclaration : Reperesents a type usable in a function or struct
//
//        NOTE: Constructors remember declarations global state but
//        do not add to it. Initialize functions do.    
//
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------

#ifndef ___TYPE_DECLARATION_H_INCLUDED__  
#define ___TYPE_DECLARATION_H_INCLUDED__  


#include <iostream>
#include <vector>
#include <string>
#include "declaration.h"
#include "declarations.h"
#include "tokenizeddeclaration.h"
#include "arg_or_member_declaration.h"
#include "c150exceptions.h"
#include "IDLToken.h"

using namespace std;

class Declarations;

class Arg_or_Member_Declaration;

//--------------------------------------------------------
//                 TypeDeclaration
//--------------------------------------------------------

class TypeDeclaration : public Declaration {
 private:

 protected:
  bool isArrayType;
  bool isStructType;

  TypeDeclaration* arrayMemberType;

  unsigned int arrayBound;

  vector<Arg_or_Member_Declaration *> structMembers;

 public:

  //
  // Friends
  //
  friend class Arg_or_Member_Declaration;  // creates array types

  // Constructors
  TypeDeclaration() {};  // uninitialized
  TypeDeclaration(Declarations* declp, string nm) : Declaration(declp,nm), 
    isArrayType(false), isStructType(false), arrayBound(0) {};
  TypeDeclaration(Declarations* declp, TokenizedDeclaration& td, int& nextTokenIndex, const int numTokens);
  ~TypeDeclaration();       // destructor

  
  //
  // True iff this type is an array/struct respectively
  //
  virtual bool isArray() {return isArrayType;};
  virtual bool isStruct() {return isStructType;};

  //
  // Struct members
  //
  vector<Arg_or_Member_Declaration *>& getStructMembers() {return structMembers;};


  //
  // Type of each element, and number of elements
  //
  virtual TypeDeclaration* getArrayMemberType() {return arrayMemberType;};
  virtual unsigned int getArrayBound() {return arrayBound;};

  // to_string_stream: formats the declaration for debugging output
  //
  void to_string_stream(stringstream& ss);

  //
  // Static test whether token stream is possible struct
  //
  static bool isPossibleStructDeclaration(TokenizedDeclaration& td, const int firstToken, const int numTokens);
  
  // parse one member. Return true iff it's the last one
  virtual bool parseOneMember(TokenizedDeclaration& td, 
				int &argTokenIndex, const int numTokens);


};




#endif
