// --------------------------------------------------------------
//
//                        IDLToken.h
//
//        Author: Noah Mendelsohn
//
//        Represents single token from an IDL file.
//        Subclasses are used for particular types.
//
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------


#ifndef __IDLTOKEN_H_INCLUDED__  
#define __IDLTOKEN_H_INCLUDED__  

#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>               // for atoi()
#include "c150exceptions.h"

using namespace C150NETWORK;

// ************************************************************************
//      BASE CLASS: IDLToken             
// ************************************************************************

class IDLToken {
 private:

 protected:
  string text;

 public:

  IDLToken();       // constructor: note that individual constructors
                    // of derived types set the actual text based
                    // on their own parsing rules
  virtual ~IDLToken() {}; // virtual destructor

  // possible type names of a C150DgmSocket

  static const char* tokenTypeNames[];

  enum typeEnum {
    textToken,                        // includes keywords
    delimiter,
    integer,
    undefined
  } tokenTypeCode;


  virtual typeEnum getTokenType() {return undefined;};   // returns the type of the token
  virtual const char *getTokenTypeName() {return tokenTypeNames[getTokenType()];};   // returns the type of the token
   string& getText() {return text;}; // The actual token chars

  //
  //  Methods for particular types.  When used in the wrong 
  //  ones, will throw a C150Exception
  //

  virtual int getIntegerValue() {throw C150Exception("IDLToken::getIntegerValue called on non-integer token type");};
  virtual bool isTokenChar(const char c) {throw C150Exception("IDLToken::isTokenChar called on IDLToken base type");};
  virtual bool isKeyword() {throw C150Exception("IDLToken::isKeyword on non-text token type");};

  static typeEnum typeOfToken(const char c);   // determine token type from its first character
};

// ************************************************************************
//      typedef TokenVector
//
//      This is a vector of POINTERS to tokens, so virtual method
//      dispatch will work.
// ************************************************************************

typedef std::vector<IDLToken *> TokenVector;


// ************************************************************************
//      DERIVED CLASS: IDLTextToken 
//
//      Represents a text token that may or may not be a keyword            
// ************************************************************************

class IDLTextToken : public IDLToken {
 private:

 protected:
  bool isAKeyWord;

 public:
  IDLTextToken(istream& is);             // constructor - must set isAKeyWord
  bool isTokenChar(const char c);   // check if this char can be in a token

  static const int numberOfKeywords;
  static const char *keywords[];    // Array of all tokens recognized as keywords


  virtual typeEnum getTokenType() {return textToken;};   // returns the type of the token
  virtual bool isKeyword() {return isAKeyWord;};
};

// ************************************************************************
//      DERIVED CLASS: IDLIntegerToken 
//
//      Represents a text token that may or may not be a keyword            
// ************************************************************************

class IDLIntegerToken : public IDLToken {
 private:
  bool isTokenChar(const char c);   // check if this char can be in a token
  int value;

 protected:
  bool isAKeyWord;

 public:

  IDLIntegerToken(istream& is);             // constructor - must set isAKeyWord
  virtual typeEnum getTokenType() {return integer;};   // returns the type of the token
  virtual int getIntegerValue() {return value;};

};

// ************************************************************************
//      DERIVED CLASS: IDLDelimiterToken 
//
//      Represents a text token that is a delimiter
// ************************************************************************

class IDLDelimiterToken : public IDLToken {
 private:
  bool isTokenChar(const char c);   // check if this char can be in a token

 protected:
  bool isAKeyWord;

 public:

  IDLDelimiterToken(istream& is);             // constructor - must set isAKeyWord
  virtual typeEnum getTokenType() {return delimiter;};   // returns the type of the token
};


#endif
