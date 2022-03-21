// --------------------------------------------------------------
//
//                        Declaration.cpp
//
//        Author: Noah Mendelsohn
//
//        Classes:
//
//        FunctionDeclaration: Represents a parsed declaration of a function
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------

#include "functiondeclaration.h"     

//
//          Constructor
//
//   This version is only for user-defined types.
//   Built ins use simple constructor
//
//   Should be called only when you know that 
//   the tokens are to be parsed as a function declaration
//
//   Call initialize immediately after the constructor,
//   and then be sure to add to the declarations map when done
//

FunctionDeclaration::FunctionDeclaration(Declarations* declp) :
  Declaration(declp) 
{

  // all the work of building this will be done in the intialize call

}




//
//          Initialize Function Declaration
//
//
//   Parse a function argument from the tokens
//   starting at nextTokenIndex.
//
//   Format of the arguments we handle for now is
//   just <typename> <variable name>
//   The variable name we just store as the declaration name
//
//   Postconditions: 
//
//       1) this function declaration is fully constructed
//
//       2) reference parameter nextToken is updated to point
//          to closing paren
//

void FunctionDeclaration::initialize(TokenizedDeclaration& td, int& nextTokenIndex,
				const int numTokens)
{
  //
  // Make sure token string long enough, and has paren in 3rd slot
  //
  if (!isPossibleFunctionDeclaration(td, nextTokenIndex, numTokens)) {
    stringstream ss;
    ss << "FunctionDeclaration::initialize: token string does not appear to be a function declaration: " << td.str() ;
    throw C150Exception(ss.str());
  }


  //
  // Make sure first token is a type. If so, remember it as the
  // function return type
  //
  //  Specifically, make sure the token is text (not delimiter, etc.)
  //  and that it's in the map of type names
  //
  IDLToken *typeTokenp = td[nextTokenIndex];
  string typeName = typeTokenp->getText();
  if (typeTokenp->getTokenType() != IDLToken::textToken ||
      !declarationsp->typeExists(typeName) ) {
    stringstream ss;
    ss << "FunctionDeclaration::initialize: \"" << typeTokenp->getText() << 
      "\" is not a declared type name" << endl << "Declaration causing error is: " << td.str();
    throw C150Exception(ss.str());
  }

  returnType = declarationsp->types[typeName];

  //
  // Make sure second argument is a name and is not a keyword
  // Set it as the name of this argument
  //

  IDLToken *nameTokenp = td[nextTokenIndex+1];
  if (nameTokenp->getTokenType() != IDLToken::textToken ||
      nameTokenp->isKeyword() ) {
    stringstream ss;
    ss << "FunctionDeclaration::initialize: \"" << nameTokenp->getText() << 
      "\" is not a legal variable name (might be a reserved word, delimiter, etc.)"  << endl << "Declaration causing error is: " << td.str();
    throw C150Exception(ss.str());
  }

  setName(nameTokenp->getText());

  //
  // Loop parsing arguments to the  function
  //

  nextTokenIndex+=3; // skip "retType functionname ("

  IDLToken *nextTokenp;
  while(true) {
    if (nextTokenIndex >  numTokens) {

      stringstream ss;
      ss << "FunctionDeclaration::initialize: declararing function \"" << getName() << 
	"\"incomplete argument list declaration. Reached end of list without close parenthesis" << endl << "Declaration causing error is: " << td.str();
      throw C150Exception(ss.str());
    }

    // 
    // Unless we're looking at delimiter (probably close paren)
    // parse an argument
    //
    // note: parseOneArgument updates nextTokenIndex to be after
    // terminating comma or paren...returns true iff close paren
    // parsed
    //
    //

    nextTokenp = td[nextTokenIndex];
    if (nextTokenp->getTokenType() == IDLToken::delimiter || 
	parseOneArgument(td, nextTokenIndex, numTokens))
      break;
  }

  //
  // Argument parser tends to go past ) specifically in case
  // where there are arguments and the last one (correctly) is not followed
  // by comma
  //
  if (nextTokenIndex > numTokens-2)
    nextTokenIndex = numTokens-2;     // point to last token

  //
  // Make sure the last argument was not followed by comma
  //
  nextTokenp = td[nextTokenIndex++];   // should be )
  string shouldBeParen = nextTokenp->getText();
  if (shouldBeParen == ",") {

      stringstream ss;
      ss << "FunctionDeclaration::initialize: declararing function \"" << getName() << 
	"\" extra comma at end for argument list" << endl << "Declaration causing error is: " << td.str();
      throw C150Exception(ss.str());
  }

  //
  // Following is probably redundant -- make sure we got )
  //
  if (shouldBeParen != ")") {

      stringstream ss;
      ss << "FunctionDeclaration::initialize: declararing function \"" << getName() << 
	"\" didn't find ) or ;  at end of declaration" << endl << "Declaration causing error is: " << td.str();
      throw C150Exception(ss.str());
  }

  //
  // Make sure we got semicolon right after end
  //
  nextTokenp = td[nextTokenIndex++];   // should be ;
  string shouldBeSemicolon = nextTokenp->getText();
  if (shouldBeSemicolon != ";") {

      stringstream ss;
      ss << "FunctionDeclaration::initialize: declararing function \"" << getName() << 
	"\" did not find semicolon (;) after function declaration" << endl << "Declaration causing error is: " << td.str();
      throw C150Exception(ss.str());
  }

  //
  // Only way to get here is if we parsed the close paren. We leave nextArgIndex 
  // pointer to that (not past it!)
  //

  return;
}



//
// isPossibleFunctionDeclaration
//
//   Do some simple syntax checks to see if this looks like a function declaration
//
bool FunctionDeclaration::isPossibleFunctionDeclaration(TokenizedDeclaration& td, const int firstTokenIndex, const int numTokens) {

  //
  // Make sure there are at least five more tokens, for minimal definition:
  // returnType name ( ), so that's four 
  //
  if (numTokens < firstTokenIndex + 4)
    return false;

  //
  // Make sure third argument is a "(" delimiter
  //
  //
  IDLToken *parenTokenp = td[firstTokenIndex+2];
  string paren = parenTokenp->getText();
  if (parenTokenp->getTokenType() != IDLToken::delimiter ||
      !(parenTokenp->getText() == "(") ) 
    return false;

  //
  // Looks like it might be a function
  //
  return true;

};


//
// parseOneArgument
//
//   Parse one argument 
//
//    NOTE: this updates argTokenIndex to point past the terminating delimiter!
//
//    Return value: 
//
//       false: terminated by comma, try another argument
//       true:  terminated by ), we're done with function
//
//    NEEDSWORK: should be shared with struct member parsing!
//
bool
FunctionDeclaration::parseOneArgument(TokenizedDeclaration& td, int &argTokenIndex, const int numTokens)
{


  IDLToken *delimTokenp = NULL;
  string delimText;
  bool retval = false;                // guess there will be more arguments
  Arg_or_Member_Declaration *newArgp = new Arg_or_Member_Declaration(declarationsp);

  
  // parse the next argument
  newArgp -> initialize(td, argTokenIndex, numTokens);
  
  // add to list of arguments for this function
  arguments.push_back(newArgp);
  
  //
  // Make sure we aren't off end of declatation without close paren
  // and then see whether there's another argument to go.
  //
  if (argTokenIndex >= numTokens) {
    stringstream ss;
    ss << "FunctionDeclaration::parseOneArgument: declararing function \"" << getName() << 
      "\"incomplete argument list declaration. Reached end of list without close parenthesis" << endl << "Declaration causing error is: " << td.str();
    throw C150Exception(ss.str());
  }
  
  //
  // The next token should be a delimiter, either a comma or close
  // paren. Do NOT bump pointer past that one yet.
  //
  delimTokenp = td[argTokenIndex];
  
  if (delimTokenp -> getTokenType() != IDLToken::delimiter) {
    stringstream ss;
    ss << "FunctionDeclaration::parseOneArgument: declararing function \"" << getName() << 
      "\"argument declaration not followed by delimiter" << endl << "Declaration causing error is: " << td.str();
    throw C150Exception(ss.str());
  }
  


  
  //
  // When the next token to parse is a close paren
  // we're done parsing the argument list
  //
  delimText = delimTokenp -> getText();

  if (delimText == ")")
    retval = true;
  
  //
  // We've already bumped to look at next token postion,
  // but we still need to make sure this one is a comma
  //
  else if (delimText!= ",") {
    stringstream ss;
    ss << "FunctionDeclaration::parseOneArgument: declararing function \"" << getName() << 
      "\" argument declaration not followed by comma or parenthesis" << endl << "Declaration causing error is: " << td.str();
    throw C150Exception(ss.str());
  }

  argTokenIndex++;           // skip the comma or )

  return retval;

}

