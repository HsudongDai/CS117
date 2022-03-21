// --------------------------------------------------------------
//
//                        tokenizeddeclarations.cpp
//
//        Author: Noah Mendelsohn
//
//        Class: TokenizedDeclarations
//
//        The constructor tokenizes an entire IDL file,
//        creating a vector with one entry per declaration.
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------



// #include "c150debug.h"     
// #include "c150utility.h"
#include <fstream>
#include <iostream>
// #include <unistd.h>
#include "c150exceptions.h"
#include "tokenizeddeclarations.h"

using namespace std;



// --------------------------------------------------------------
//                       Constructor
// --------------------------------------------------------------

TokenizedDeclarations::TokenizedDeclarations(istream& is) {
  char firstChar;
  int curlyDepth = 0;              // curly braces open
  TokenizedDeclaration *declp = NULL;
  IDLToken *tokenp;
  IDLDelimiterToken *delimp = NULL;   // set only if it's a delimiter
string DEBUGSTRING;


  //
  // skipWhitespace gives us the first character
  // of each token, which is enough to determine the type to
  // construct.
  //
  while(!is.eof()) {
    tokenp = NULL;
    delimp = NULL;   // set only if it's a delimiter

    firstChar = skipWhiteSpace(is);
    //
    // If skipping did not take us to eof
    //
    if (!is.eof()) {
      switch (IDLToken::typeOfToken(firstChar)) {
      case IDLToken::textToken:
	tokenp = new IDLTextToken(is);
DEBUGSTRING = "TEXTTOKEN";
	break;
      case IDLToken::delimiter:
	delimp = new IDLDelimiterToken(is);
	tokenp = delimp;
DEBUGSTRING = "DELIMTOKEN";
	break;
      case IDLToken::integer:
	tokenp = new IDLIntegerToken(is);
DEBUGSTRING = "INTEGETOKEN";
	break;
      default:
	throw C150Exception("TokenizedDeclarations::TokenizedDeclarations Internal error: unexpected return value from typeOfToken");
      };
      
      //
      // If this is a curly brace, then increment or decrement
      // the curly brace nesting count. We do this because
      // inside a struct {...; ...;}; the semis do not
      // terminate the statement
      //
      if (delimp != NULL && (delimp -> getText()[0]) == '{') {
	curlyDepth++;
      }

      //
      // If this is a curly brace, then increment or decrement
      // the curly brace nesting count. We do this because
      // inside a struct {...; ...;}; the semis do not
      // terminate the statement
      //
      if (delimp != NULL && (delimp -> getText()[0]) == '}') {
	curlyDepth--;
	if (curlyDepth < 0) {
	  throw C150Exception("TokenizedDeclarations::TokenizedDeclarations: unmatched '}' character");

	}
      }

      //
      // If this is a statement end delimiter (;) then
      // we take all the tokens we have and push them
      // on to the declaration vector as a new
      // declaration.
      //
      // We ignore semicolons inside open curly brace 
      //
      if (curlyDepth == 0 && delimp != NULL && (delimp -> getText()[0]) == ';') {
	// If we've collected a declaration, then
	// remember it, without the delimiter
	if (declp != NULL) {
    	  declp -> push_back(delimp);  // put the ; on the end of each statement
	  vec.push_back(*declp);

	  // NEEDSWORK: Not clear why deleting vector
	  // Destroys referents of all pointers!
	  delete declp;
	  declp = NULL;
	} else {
	  cerr << "TokenizedDeclarations::TokenizedDeclarations: ignoring extra semicolon (;)" << endl;
	}

      //
      // This is not a statement end delimiter .. push a copy of the token
      // onto the end of the declaration vector
      //
      } else {
	if (declp == NULL)            // Start new declaration if necessary
	  declp = new TokenizedDeclaration();
	declp -> push_back(tokenp);
      }

      //
      // We pushed a pointer, but let's remember to start 
      // with a new one next time!
      //
      tokenp = NULL;
      delimp = NULL;
      // 
      // EOF reached...make sure statement terminated
      //
      } else {
      if (declp != NULL) {
	throw C150Exception("TokenizedDeclarations::TokenizedDeclarations Input file did not end with semicolon...unterminated declaration");
      }
    }
  }

  return;                 // all done
}


// --------------------------------------------------------------
//                isWhiteSpaceChar
//
//
//        Tests a char to see whether it's whitespace
//
// --------------------------------------------------------------

bool TokenizedDeclarations::isWhiteSpaceChar(const char c) {

  bool retval = false;

  // The switch may take a bit of space, but should be fast
  switch (c) {
  case ' ':
  case '\r':
  case '\n':
  case '\t':
    retval = true;
  };

  return retval;
}

// --------------------------------------------------------------
//                skipWhiteSpace
//
//      Advances the supplied stream past any white space.
//  
//      Return conditions:
//
//         If we're at a point where the remainder of the stream
//         consists of zero or more whitespace, leave stream
//         at eof and return a blank.
//      
//         Otherwise, leave the stream positioned so that 
//         the next getc will return a non-whitespace character,
//         and return that character (I.e. the char remains
//         in the input buffer)
//
// --------------------------------------------------------------

char TokenizedDeclarations::skipWhiteSpace(istream& is) {

  char peekedChar = ' ';               // in case we hit eof

  //
  // Skip whitespace. Leave stream at next non-whitespace
  // char, and also collect that in peekedChar
  //
  while (!is.eof() && isWhiteSpaceChar(peekedChar = is.peek()))
    is.ignore();                 // advance past the char we peeked

  return peekedChar;
}

// --------------------------------------------------------------
//                       Destructor
// --------------------------------------------------------------

TokenizedDeclarations::~TokenizedDeclarations() {
}
