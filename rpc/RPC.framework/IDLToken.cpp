// --------------------------------------------------------------
//
//                        IDLToken.cpp
//
//        Author: Noah Mendelsohn
//
//        Syntax:
//
//               idltokenizer idlfile1 [idlfile...]*
//  
//        This program reads each file named on the command line
//        and calls the idl tokenizer on a stream tied that file.
//        The results are then formatted on stdoutput for demonstration
//        purposes.
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------


#include "IDLToken.h"
#include "tokenizeddeclarations.h"


using namespace std;

// ************************************************************************
//                       CONSTRUCTOR
// ************************************************************************


//
//  Constructor for IDLToken base class. Because string has not
//  been parsed at this point, we let derived class set text
//  and type code once known.
//

IDLToken::IDLToken() {
  text = "";                  // Temporary...will be overridden by
                              // ...derived class constructor
};



// ************************************************************************
//      STATIC MEMBERS OF IDLToken class
// ************************************************************************

  // map from state enumeration to description strings
  // this array is used only to format debugging messages
  // MUST be kept in sync with the enum in the header file

  const char* IDLToken::tokenTypeNames[] = {
    "textToken",
    "delimiter",
    "integer",
    "undefined"
  };


// ************************************************************************
//      STATIC Methods OF IDLToken class
// ************************************************************************

//
//  Return the type code for a supplied token starting character
//
//  Token validation is done later when type-specific constructor called.
//
//  Assumptions (for now):
//
//       delimiters are single character
//       any token starting with an integer is numeric and
//       has already been parsed to be numeric only
//
//
IDLToken::typeEnum IDLToken::typeOfToken(const char c) {

  if (c >= '0' && c <= '9') 
    return IDLToken::integer;

  // If next char is delimiter, we'll parse as that
  switch (c) {
  case '{':
  case '}':
  case '[':
  case ']':
  case '(':
  case ')':
  case ',':
  case ';':
    return IDLToken::delimiter;
  };

  // otherwise, it might be a keyword, but
  // it's definitely a textToken

  return IDLToken::textToken;

}

// ************************************************************************
//      DERIVED CLASS: IDLTextToken 
//
//      Represents a text token that may or may not be a keyword            
// ************************************************************************

//
// Constructor
//
IDLTextToken::IDLTextToken(istream& is) {
  stringstream newToken;
  int i;

  //
  // Gather until we hit delimiter or white space
  //
  char peekedChar;
  while (!is.eof() && isTokenChar(peekedChar = is.peek())) {
    newToken << ((char)is.get());                 // advance past the char we peeked
  }

  //
  // Token is in the stringstream, save it in our object
  //
  text = newToken.str();  

  //
  // For text tokens, we also need to determine whether it's a keyword
  //
  isAKeyWord = false;

  for(i=0; i<numberOfKeywords; i++) {
    if (text == keywords[i]) {
      isAKeyWord = true;
      break;
    }
  }

}


//
// isTokenChar
//
//   True if this is a legal character in a token
//
//   NEEDSWORK: for now, we accept anything that's not
//   whitespace or delimiter
//
bool IDLTextToken::isTokenChar(const char c) {
  bool retval = true;
  if (TokenizedDeclarations::isWhiteSpaceChar(c))
    retval = false;
  else if (typeOfToken(c) == delimiter)
    retval = false;
  return retval;
};


// - - - - - - - - - - - - - - - - - - - - - - - - - 
//      STATIC MEMBERS OF IDLTextToken class
// - - - - - - - - - - - - - - - - - - - - - - - - - 

// list of keywords that aren't allowed
// as function, member or user-defined type names


  const char* IDLTextToken::keywords[] = {
    "void",
    "struct",
    "int",
    "string",
  };

const int IDLTextToken::numberOfKeywords = (sizeof(IDLTextToken::keywords) /  sizeof(char *));

// ************************************************************************
//      DERIVED CLASS: IDLIntegerToken 
//
//      Represents a text token that may or may not be a keyword            
// ************************************************************************

//
// Constructor
//
IDLIntegerToken::IDLIntegerToken(istream& is) {
  stringstream newToken;
  //
  // Gather until we hit delimiter or white space
  //
  char peekedChar;


  while (!is.eof() && isTokenChar(peekedChar = is.peek())) {
    newToken << ((char)is.get());                 // advance past the char we peeked
  }

  //
  // Token is in the stringstream, save it in our object
  // and note its value
  //
  text = newToken.str();
  value = atoi(text.c_str());
}


//
// isTokenChar
//
//   True if this is a legal character in an integer token
//
//
bool IDLIntegerToken::isTokenChar(const char c) {
  return (c>='0') && (c<='9');
};



// ************************************************************************
//      DERIVED CLASS: IDLDelimiterToken 
//
//      Represents a text token that may or may not be a keyword            
// ************************************************************************

//
// Constructor
//
IDLDelimiterToken::IDLDelimiterToken(istream& is) {
  stringstream newToken;

  //
  // Gather until we hit delimiter or white space
  //
  char peekedChar;
  if (is.eof()) {
    throw C150Exception("IDLDelimiterToken::IDLDelimiterToken: Internal error...called at eof");
  }

  if (isTokenChar(peekedChar = is.peek())) {
    throw C150Exception("IDLDelimiterToken::IDLDelimiterToken: Internal error...called pointing at non-delimiter char");
  }

  //
  // Get the 1 char delimiter
  //
  newToken << ((char)is.get());                 // advance past the char we peeked

  //
  // One character Token is in the stringstream, save it in our object
  //
  text = newToken.str();  
}


//
// isTokenChar
//
//   True if this is a legal character in an integer token
//
//   NEEDSWORK: for now, we accept anything that's not
//   whitespace or delimiter
//
bool IDLDelimiterToken::isTokenChar(const char c) {
  bool retval = true;
  if (TokenizedDeclarations::isWhiteSpaceChar(c))
    retval = false;
  else if (typeOfToken(c) == delimiter)
    retval = false;
  return retval;
};


