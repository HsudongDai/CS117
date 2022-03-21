// --------------------------------------------------------------
//
//                        tokenizeddeclaration.h
//
//        Author: Noah Mendelsohn
//
//        Represents an IDL declaration
//         as a C++ Vector of Tokens
//
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------

#ifndef __TOKENIZED_DECLARATION_H_INCLUDED__  
#define __TOKENIZED_DECLARATION_H_INCLUDED__  


// #include "c150debug.h"     
// #include "c150utility.h"
// #include <fstream>
#include <iostream>
#include <vector>
// #include <cstdio>
//#include <unistd.h>
#include "c150exceptions.h"
#include "IDLToken.h"



class TokenizedDeclaration {
 private:
  TokenVector vec;                // the vector with the tokens

 
 public:
  TokenizedDeclaration();       // constructor
  virtual ~TokenizedDeclaration();       // destructor
  
  virtual TokenVector& getTokenVector() { return vec;};
                                  // return C++ vector of token pointers
  //
  // Add a token to the declaration
  //
  virtual void push_back(IDLToken *t) {vec.push_back(t);};

  //
  // Make it appear a bit like a vector
  //
  IDLToken* operator[] (const int index) {return vec[index];};
  size_t size() {return vec.size();};

  //
  // String dump for debugging output
  //
  virtual string str();
};

#endif
