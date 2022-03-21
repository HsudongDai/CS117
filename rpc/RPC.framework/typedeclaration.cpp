// --------------------------------------------------------------
//
//                        TypeDeclaration.cpp
//
//        Author: Noah Mendelsohn
//
//        Classes:
//
//        TypeDeclaration: Represents a parsed declaration of a type 
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------



#include "typedeclaration.h"     

//
// constructor called when the first token says struct
//

TypeDeclaration::TypeDeclaration(Declarations* declp, TokenizedDeclaration& td, int& nextTokenIndex, const int numTokens) : Declaration(declp), 
    isArrayType(false), isStructType(false), arrayBound(0){

  //
  // our caller checked that first token is struct
  //

  //
  // Make sure second token is a name and is not a keyword
  // Set it as the name of this structure type
  //

  IDLToken *nameTokenp = td[nextTokenIndex+1];
  if (nameTokenp->getTokenType() != IDLToken::textToken ||
      nameTokenp->isKeyword() ) {
    stringstream ss;
    ss << "TypeDeclaration::TypeDeclaration: \"" << nameTokenp->getText() << 
      "\" is not a structure variable name (might be a reserved word, delimiter, etc.)"  << endl << "Declaration causing error is: " << td.str();
    throw C150Exception(ss.str());
  }

  //
  // Remember the structure name
  //

  setName(nameTokenp->getText());

  //
  // Make sure third argument is a "{" delimiter
  //
  //
  IDLToken *parenTokenp = td[nextTokenIndex+2];
  string paren = parenTokenp->getText();
  if (parenTokenp->getTokenType() != IDLToken::delimiter ||
      !(parenTokenp->getText() == "{") ) {
    stringstream ss;
    ss << "TypeDeclaration::TypeDeclaration: declaration for struct \"" << nameTokenp->getText() << 
      "\" is missing opening \"{\" for member list"  << endl << "Declaration causing error is: " << td.str();
    throw C150Exception(ss.str());
  }

  //
  // Loop parsing members of the struct
  //

  nextTokenIndex+=3; // skip "struct name {"

  IDLToken *nextTokenp;

  while(true) {
    if (nextTokenIndex >  numTokens) {

      stringstream ss;
      ss << "TypeDeclaration::TypeDeclaration: declararing struct \"" << getName() << 
	"\"incomplete member list declaration. Reached end of list without closing curly brace" << endl << "Declaration causing error is: " << td.str();
      throw C150Exception(ss.str());
    }

    // 
    // Unless we're looking at delimiter (probably curly brace)
    // parse an member
    //
    // note: parseOneMember updates nextTokenIndex to be after
    // terminating comma or paren...returns true iff close paren
    // parsed
    //
    //

    nextTokenp = td[nextTokenIndex];
    if (nextTokenp->getTokenType() == IDLToken::delimiter || 
	parseOneMember(td, nextTokenIndex, numTokens))
      break;
  }

  //
  // Argument parser tends to go past } specifically in case
  //
  if (nextTokenIndex > numTokens-2)
    nextTokenIndex = numTokens-2;     // point to last token

  //
  // Make sure the last argument was not followed by comma
  //
  nextTokenp = td[nextTokenIndex++];   // should be )
  string shouldBeCurly = nextTokenp->getText();


  //
  // Following is probably redundant -- make sure we got )
  //
  if (shouldBeCurly != "}") {

      stringstream ss;
      ss << "TypeDeclaration::initialize: declararing function \"" << getName() << 
	"\" didn't find } or ;  at end of declaration" << endl << "Declaration causing error is: " << td.str();
      throw C150Exception(ss.str());
  }

  //
  // Make sure we got semicolon right after end
  //
  nextTokenp = td[nextTokenIndex++];   // should be ;
  string shouldBeSemicolon = nextTokenp->getText();
  if (shouldBeSemicolon != ";") {

      stringstream ss;
      ss << "TypeDeclaration::initialize: declararing function \"" << getName() << 
	"\" did not find semicolon (;) after function declaration" << endl << "Declaration causing error is: " << td.str();
      throw C150Exception(ss.str());
  }

  //
  // Only way to get here is if we parsed the closing curly brace. 
  // We leave nextTokenindex 
  // pointer to that (not past it!)
  //

  isStructType = true;

  return;
}

//
// parseOneMember
//
//   Parse one member 
//
//    NOTE: this updates memTokenIndex to point past the terminating delimiter!
//
//    Return value: 
//
//       false: terminated by comma, try another member
//       true:  terminated by ), we're done with function
//
//    NEEDSWORK: should be shared with struct member parsing!
//
bool
TypeDeclaration::parseOneMember(TokenizedDeclaration& td, int &memTokenIndex, const int numTokens)
{


  IDLToken *delimTokenp = NULL;
  string delimText;

  //
  // if we're at a semicolon, then we're done
  //
  //
  // Make sure we aren't off end of declaration without close brace
  // and then see whether there's another member to go.
  //
  if (memTokenIndex >= numTokens) {
    stringstream ss;
    ss << "TypeDeclaration::parseOneMember: declararing struct\"" << getName() << 
      "\" incomplete member list declaration. Reached end of declaration without \"}\"" << endl << "Declaration causing error is: " << td.str();
    throw C150Exception(ss.str());
  }

  //
  // We know there's at least one token to look at
  // if it's '}', we're done. This could also
  // happen for an empty struct
  //
  delimTokenp = td[memTokenIndex];
  
  if ((delimTokenp -> getTokenType() == IDLToken::delimiter) &&
      ( delimTokenp -> getText()) == "}") 
    return true;
  

  //  
  // parse the next member: ignore the returned name
  //
  Arg_or_Member_Declaration *newMemp = new Arg_or_Member_Declaration(declarationsp);
  newMemp -> initialize(td, memTokenIndex, numTokens);
  
  // add to list of members for this struct
  structMembers.push_back(newMemp);
  
  //
  // Make sure we aren't off end of declaration without close brace
  // and then see whether there's another member to go.
  //
  if (memTokenIndex >= numTokens) {
    stringstream ss;
    ss << "TypeDeclaration::parseOneMember: declararing struct\"" << getName() << 
      "\" incomplete member list declaration. Reached end of list without closing curly brace" << endl << "Declaration causing error is: " << td.str();
    throw C150Exception(ss.str());
  }
  
  //
  // The next token should be a delimiter, either a semicolon or close
  // curly brace. Do NOT bump pointer past that one yet.
  //
  delimTokenp = td[memTokenIndex];
  
  if (delimTokenp -> getTokenType() != IDLToken::delimiter) {
    stringstream ss;
    ss << "TypeDeclaration::parseOneMember: declararing struct \"" << getName() << 
      "\" member declaration not followed by delimiter" << endl << "Declaration causing error is: " << td.str();
    throw C150Exception(ss.str());
  }
  
  delimText = delimTokenp -> getText();
  
  //
  // When the next token to parse is a close brace '}'
  // there's a missing semicolon after last decl.
  //
  if (delimText == "}") {
    stringstream ss;
    ss << "TypeDeclaration::parseOneMember: declararing struct \"" << getName() << 
      "\" last member declaration not followed by semicolon" << endl << "Declaration causing error is: " << td.str();
    throw C150Exception(ss.str());
  }

  
  //
  // We've already bumped to look at next token postion,
  // but we still need to make sure this one is a semicolon, which
  // is required after all declarations
  //
  else if (delimText!= ";") {
    stringstream ss;
    ss << "TypeDeclaration::parseOneMember: declararing struct \"" << getName() << 
      "\" member declaration not followed by semicolon or curly brace" << endl << "Declaration causing error is: " << td.str();
    throw C150Exception(ss.str());
  }

  memTokenIndex++;           // skip the comma or )

  return false;              // haven't parsed } yet

}

//
//  Destructor
//
TypeDeclaration::~TypeDeclaration() {};       // destructor

//
// to_string_stream: formats the declaration for debugging output
//
void TypeDeclaration::to_string_stream(stringstream& ss) {
  unsigned int memberNum;
  ss << "Type Name: " << getName();
  if (isStruct()) {
    ss << "  STRUCT";
    //
    // Get a C++ reference to the vector with the argument list
    //
    vector<Arg_or_Member_Declaration *>& members = getStructMembers();
    ss << "  Number of members: " << members.size() << endl << "{";

    //
    // The members are in a vector, not a map, so we
    // access them by position, not name
    //
    for(memberNum=0; memberNum<members.size();memberNum++) {
      Arg_or_Member_Declaration* memp = members[memberNum];
      ss << endl << "   " << memp->getType()->getName() << "  " << memp->getName();
    }

    //
    // Write closing '}'
    //
    ss << endl << "}";
  }

  ss << endl;
}

//
// isPossibleStructDeclaration
//
//   Do some simple syntax checks to see if this looks like a struct declaration
//
bool TypeDeclaration::isPossibleStructDeclaration(TokenizedDeclaration& td, const int firstTokenIndex, const int numTokens) {

  //
  // Check if first token says "struct"
  //
  //
  IDLToken *structTokenp = td[firstTokenIndex];
  if (structTokenp->getTokenType() != IDLToken::textToken ||
      !(structTokenp->getText() == "struct") ) 
    return false;

  //
  // Looks like it might be a struct
  //
  return true;

};
