// --------------------------------------------------------------
//
//                        TokenizedDeclaration.cpp
//
//        Author: Noah Mendelsohn
//
//       Class: TokenizedDeclaration
//
//       Represents a declaration as a vector of tokens
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------

#include "tokenizeddeclaration.h"



// --------------------------------------------------------------
//                       Constructor
// --------------------------------------------------------------

TokenizedDeclaration::TokenizedDeclaration() {



}



// --------------------------------------------------------------
//                       Destructor
// --------------------------------------------------------------

TokenizedDeclaration::~TokenizedDeclaration() {


}


// --------------------------------------------------------------
//                       str
//
//    Put the tokens in a string for debugging output
// --------------------------------------------------------------

string TokenizedDeclaration::str() {
  unsigned int j;
  stringstream ss;
  for (j=0; j < size(); j++) {
    IDLToken *tokenp = vec[j];
    ss  << tokenp->getText() << ' ' ;
  }
  return ss.str();

}

