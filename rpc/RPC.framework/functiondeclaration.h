// --------------------------------------------------------------
//
//                        functiondeclaration.h
//
//        Author: Noah Mendelsohn
//
//        Classes:
//
//        FunctionDeclaration: Represents a function declaration 
//
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------

#ifndef ___FUNCTION_DECLARATION_H_INCLUDED__  
#define ___FUNCTION_DECLARATION_H_INCLUDED__  


#include "declaration.h"
#include "arg_or_member_declaration.h"
#include "declarations.h"

using namespace std;



//--------------------------------------------------------
//                 FunctionDeclaration
//--------------------------------------------------------

class FunctionDeclaration : public Declaration {
 private:

 protected:
  TypeDeclaration *returnType;       // pointer to return type
  ArgumentVector arguments; // vector of pointers to arguments

  // parse one argument. Return true iff it's the last one
  virtual bool parseOneArgument(TokenizedDeclaration& td, 
				int &argTokenIndex, const int numTokens);
 
 public:
  FunctionDeclaration(Declarations* declp);       // constructor
  virtual ~FunctionDeclaration() {};       // destructor

  virtual void initialize(TokenizedDeclaration& td, int& nextTokenIndex, const int numTokens);  

  virtual ArgumentVector& getArgumentVector() {return arguments;};

  // Check first few tokens to see if they look like a function signature
  static bool isPossibleFunctionDeclaration(TokenizedDeclaration& td, const int firstToken, const int numTokens);

  virtual TypeDeclaration* getReturnType() {return returnType;}; // pointer to function return type


};

#endif
