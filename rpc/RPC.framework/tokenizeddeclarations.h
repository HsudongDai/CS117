// --------------------------------------------------------------
//
//                        tokenizeddeclarations.h
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

#ifndef __TOKENIZED_DECLARATIONS_H_INCLUDED__  
#define __TOKENIZED_DECLARATIONS_H_INCLUDED__  




// #include "c150debug.h"     
// #include "c150utility.h"
#include <fstream>
#include <iostream>
#include <vector>
// #include <cstdio>
//#include <unistd.h>
#include "c150exceptions.h"
#include "tokenizeddeclaration.h"


// ************************************************************************
//      typedef DeclaractionVector
// ************************************************************************

typedef std::vector<TokenizedDeclaration> TokenizedDeclarationVector;



class TokenizedDeclarations {
 private:
  TokenizedDeclarationVector vec;                // One entry per declaration


 
 public:
  TokenizedDeclarations(istream& is);             // constructor
  virtual ~TokenizedDeclarations();               // destructor
  virtual TokenizedDeclarationVector& getDeclarationVector() { return vec;};
                                      // return C++ vector of tokens


  //
  // Make it appear a bit like a vector
  //
  TokenizedDeclaration& operator[] (const int index) {return vec[index];};
  size_t size() {return vec.size();};


  //
  // NEEDSWORK: White space static methods might belong in a
  // different class
  //
  static char skipWhiteSpace(istream& is);   // make sure next get 
                                            // will be for non-whitespace
                                            // may leave at eof
                                            // returns next char
                                            // that will be read
 
  static bool isWhiteSpaceChar(const char c);
                                            // returns true if c is a 
                                            // whitespace char
};

#endif
