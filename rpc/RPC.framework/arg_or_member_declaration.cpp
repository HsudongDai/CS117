// --------------------------------------------------------------
//
//                        arg_or_member_declaration.cpp
//
//        Author: Noah Mendelsohn
//
//        Classes:
//
//        Arg_or_Member_Declaration: Represents a parsed declaration of 
//                   function argument or struct member
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------



#include "arg_or_member_declaration.h"     


//
//          Initialize
//
//   Parse a function argument or struct member from the tokens
//   starting at nextTokenIndex
//
//   Side effect: nextTokenIndex updated to point to (presumed) delimiter
//   following the argument declaration...existence of delimiter not checked
//
//   Format of the arguments we handle for now is
//   just <typename> <variable name>
//   The argument/member name we store as the declaration name
//
//   WARNING: The nextTokenIndex is actually updated in parseType
//
//   LATER (not implemented yet):
//
//   As side-effect, create/lookup types for arrays e.g. int membername[10] would
//   create int[10]
//

void Arg_or_Member_Declaration::initialize(TokenizedDeclaration& td, int& nextTokenIndex,
					   const int numTokens)
{

  //

  //
  // Make sure there are at least two more tokens
  //
  if (numTokens < nextTokenIndex +2)
    throw C150Exception("Arg_or_Member_Declaration::initialize: incomplete argument declaration");

  //
  // Make sure second argument is a name and is not a keyword
  // Set it as the name of this argument
  //
  // NOTE: We do not bump the token pointer here. Because of array syntax,
  // the type declaration effectively straddles the name. We handle the
  // name first here, then let parseType bump nextTokenIndex
  //

  IDLToken *nameTokenp = td[nextTokenIndex+1];
  if (nameTokenp->getTokenType() != IDLToken::textToken ||
      nameTokenp->isKeyword() ) {
    stringstream ss;
    ss << "Arg_or_Member_Declaration::initialize: \"" << nameTokenp->getText() << 
      "\" is not a legal variable name (might be a reserved word, delimiter, etc.)";
    throw C150Exception(ss.str());
  }

  setName(nameTokenp->getText());

  //
  // Parse the type, creating as a side effect a new type
  // declaration as necessary if array syntax is used.
  //
  // nextTokenIndex is updated as side-effect to point to 
  // delimiter past the argument declaration (if any...not checked)
  //
  
  type = parseType(td, nextTokenIndex, numTokens);

  initialized = true;

  return;

};


// 
// parseType
//
// Helper routine for initialize to parse the type of an argument or member
//					       
// Side effects:
//
//   1) If array syntax is used, and similar arrays are not already in the
//      type table, the type is added with a name like int[10][4]
//
//      FIXED: 23 November 2012 The abvoe is an array of 10 items, each
//      with 4 ints, not 4 items with 10 each.
//
//   2) nextTokenIndex is advanced to the token (if any) following the declaration
//      Note: that means skipping the type name, member name and any array [x]
//      syntax
//
// Return value:
//   Pointer to the type declaration, which may or may not be newly created
//

TypeDeclaration* Arg_or_Member_Declaration::parseType(TokenizedDeclaration& td, int& nextTokenIndex, const int numTokens) {
  //
  // Make sure first argument is a type. If so, remember it
  //
  //  Specifically, make sure the token is text (not delimiter, etc.)
  //  and that it's in the map of type names
  //
  //  Bump nextTokenIndex past this token
  //
  IDLToken *typeTokenp = td[nextTokenIndex];
  if (typeTokenp->getTokenType() != IDLToken::textToken ||
      !declarationsp->typeExists(typeTokenp->getText())) {
    stringstream ss;
    ss << "Arg_or_Member_Declaration::initialize: \"" << typeTokenp->getText() << 
      "\" is not a declared type name";
    throw C150Exception(ss.str());
  }
  
  //
  // No void function arguments or members
  //

  if (typeTokenp -> getText() == "void") {
    stringstream ss;
    ss << "Arg_or_Member_Declaration::initialize: \"" << typeTokenp->getText() << 
      "\" is not allowed as function argument or struct member type";
    throw C150Exception(ss.str());
  }

  //
  // Remember the type...this may get updated later if it's an array
  //
  TypeDeclaration* typeDeclarationp = declarationsp->types[typeTokenp->getText()];

  //
  // Bump token index past type and arg/member name
  //
  nextTokenIndex += 2;

  //
  //  Parse array syntax like
  //
  //  int membername[10][20]
  //
  //  We'll want to process the bounds in reverse order (20 before 20), so
  //  we put them on a stack and then pop them
  //
  stack<unsigned int> boundStack;
  while (nextTokenIndex+3 < numTokens &&   // enough tokens for [ ... ]
	 td[nextTokenIndex]->getText() == "[") {
    boundStack.push(parseArrayDecl(td, nextTokenIndex, numTokens));
    
  }

  while (!boundStack.empty())
  {
    typeDeclarationp=constructArrayDecl(typeDeclarationp,boundStack.top());
    boundStack.pop();
  }  

  //
  // Return type object
  //

  return typeDeclarationp;

}

//
//    Called for each occurrence of a [..] array declaration 
//    for a struct member or argument
//
//    Precondition: nextTokenIndex identifies [, at least 3 tokens available
//
//    Postcondition: nextTokenIndex past ]
//
//    Returns: bounds for the array.
//
//    We want to process the bounds in reverse order. This routine is
//    typically called in a loop to gather the bounds. Then constructArrayDecl
//    is used to iteratively create the actual typeDeclaration objects
//
unsigned int Arg_or_Member_Declaration::parseArrayDecl(TokenizedDeclaration& td, int& nextTokenIndex, const int numTokens) {

  
  //
  // Make sure third token is ]
  //
  IDLToken *closeBracketTokenp = td[nextTokenIndex+2];
  if (closeBracketTokenp->getTokenType() != IDLToken::delimiter ||
      closeBracketTokenp->getText() != "]") {
    stringstream ss;
    ss << "Arg_or_Member_Declaration::parseArrayDecl: array declaration missing \"]\"" <<       endl << "  " << td.str();
    throw C150Exception(ss.str());
  }
  //
  // Make sure next token is an integer 
  //
  IDLToken *arrayCountTokenp = td[nextTokenIndex+1];
  if (arrayCountTokenp->getTokenType() != IDLToken::integer) {
    stringstream ss;
    ss << "Arg_or_Member_Declaration::parseArrayDecl: array bound is not an integer" <<       endl << "  " << td.str();
    throw C150Exception(ss.str());
  }

  nextTokenIndex+=3;

  return (unsigned int) arrayCountTokenp -> getIntegerValue();
}

//
//    Called for each occurrence of a [..] array declaration 
//    for a struct member or argument
//
//    Precondition: nextTokenIndex identifies [, at least 3 tokens available
//
//    Postcondition: nextTokenIndex past ]
//
//    Returns: pointer to type declaration for the array type.
//
//    If the type did not already exist, then add it to the type
//    declarations table
//
TypeDeclaration* Arg_or_Member_Declaration::constructArrayDecl(TypeDeclaration* memberTypep, unsigned int bound) {
  //
  // Compute type name and see if type exists
  //

  stringstream arrayTypeNameStream;

  //
  // For arrays, names are __BaseName[xx][yy]
  //
  //  
  arrayTypeNameStream << "__";         
  if (!memberTypep->isArray()) {
    arrayTypeNameStream << memberTypep->getName() << "[" << bound << "]";
  } else {                                  // multidimensional array
    // member type name looks like __memtype[nn][mm]
    // we want __memtype[bound][nn][mm]
    string baseName= memberTypep->getName();
    size_t bracketpos = baseName.find('[');  // past end of type
    string atomicTypeName = baseName.substr(2,bracketpos-2); // between __ and [
    arrayTypeNameStream << atomicTypeName << "[" << bound << "]" 
				       << baseName.substr(bracketpos) ;
  }

  string arrayTypeName = arrayTypeNameStream.str();

  //
  // If someone's already made an array just like this, use it!
  //
  TypeDeclaration* returnType;

  if (declarationsp->typeExists(arrayTypeName)) {
    returnType = declarationsp->types[arrayTypeName];
  } else {
    returnType = new TypeDeclaration(declarationsp, arrayTypeName);
    //
    // Note: we are a friend of the TypeDeclaration class, and
    // so can set its private members
    //
    returnType -> isArrayType = true;
    returnType -> arrayMemberType = memberTypep;
    returnType -> arrayBound = bound;
    declarationsp->types[arrayTypeName] = returnType;
  }

  //
  // Return the pointer to type object...it may or may not
  // have been newly constructed here. Advance parse pointer
  // past the array declaration.
  //
  
  return returnType;
}

