// Language: cpp
// Path: rpc/RPC.samples/generate_stub.cpp
// Compare this snippet from rpc/RPC.samples/simplefunction.proxy.cpp:

#include "rpcstubhelper.h"
#include "c150debug.h"
#include <string>
#include "base64.hpp"

using namespace C150NETWORK;
using namespace std;

#include "lotsofstuff.idl"

void getFunctionNamefromStream(char *buffer, unsigned int bufSize);
string readFromStream();

// ======================================================================
//                             STUBS
//
//    Each of these is generated to match the signature of the 
//    function for which it is a stub. The names are prefixed with "__"
//    to keep them distinct from the actual routines, which we call!
//
//    Note that when functions take arguments, these are the routines
//    that will read them from the input stream. These routines should
//    be generated dynamically from your rpcgen program (and so should the
//    code above).
//
// ======================================================================

string string64_to_string(string *val) {
  return base64_encode(*val);
}

string string64_to_int(int *val) {
  return base64_encode(to_string(*val));
}

string string64_to_float(float *val) {
  return base64_encode(to_string(*val));
}

void parse_string(string *value, string arg) {
  *value = arg;
}

void parse_int(int *value, string arg) {
  *value = stoi(arg);
}

void parse_float(float *value, string arg) {
  *value = stof(arg);
}

string string64_to_MT(MT *val);
string string64_to_Person(Person *val);
string string64_to_StructWithArrays(StructWithArrays *val);
string string64_to_ThreePeople(ThreePeople *val);
string string64_to_Person_10_(Person val[10]);
string string64_to_int_100_(int val[100]);
string string64_to_int_10_(int val[10]);
string string64_to_int_10__100_(int val[10][100]);
string string64_to_int_15_(int val[15]);
string string64_to_int_24_(int val[24]);
string string64_to_int_24__15_(int val[24][15]);
string string64_to_int_4_(int val[4]);
string string64_to_int_4__10_(int val[4][10]);
string string64_to_int_4__10__100_(int val[4][10][100]);
string string64_to_rectangle_200_(rectangle val[200]);
string string64_to_rectangle(rectangle *val);
string string64_to_s(s *val);

void parse_MT(MT *val, string arg);
void parse_Person(Person *val, string arg);
void parse_StructWithArrays(StructWithArrays *val, string arg);
void parse_ThreePeople(ThreePeople *val, string arg);
void parse_Person_10_(Person val[10], string arg);
void parse_int_100_(int val[100], string arg);
void parse_int_10_(int val[10], string arg);
void parse_int_10__100_(int val[10][100], string arg);
void parse_int_15_(int val[15], string arg);
void parse_int_24_(int val[24], string arg);
void parse_int_24__15_(int val[24][15], string arg);
void parse_int_4_(int val[4], string arg);
void parse_int_4__10_(int val[4][10], string arg);
void parse_int_4__10__100_(int val[4][10][100], string arg);
void parse_rectangle_200_(rectangle val[200], string arg);
void parse_rectangle(rectangle *val, string arg);
void parse_s(s *val, string arg);

string string64_to_MT(MT *val) {
  stringstream ss;
  return base64_encode(ss.str());
}

string string64_to_Person(Person *val) {
  stringstream ss;
  ss << string64_to_string(&((*val).firstname)) << ' ';
  ss << string64_to_string(&((*val).lastname)) << ' ';
  ss << string64_to_int(&((*val).age)) << ' ';
  return base64_encode(ss.str());
}

string string64_to_StructWithArrays(StructWithArrays *val) {
  stringstream ss;
  ss << string64_to_int(&((*val).aNumber)) << ' ';
  ss << string64_to_Person_10_((*val).people) << ' ';
  return base64_encode(ss.str());
}

string string64_to_ThreePeople(ThreePeople *val) {
  stringstream ss;
  ss << string64_to_Person(&((*val).p1)) << ' ';
  ss << string64_to_Person(&((*val).p2)) << ' ';
  ss << string64_to_Person(&((*val).p3)) << ' ';
  return base64_encode(ss.str());
}

string string64_to_Person_10_(Person val[10]) {
  stringstream ss;
  for(int i = 0; i < 10; i++) {
    ss << string64_to_Person(&(val[i])) << ' ';
  }
  return base64_encode(ss.str());
}

string string64_to_int_100_(int val[100]) {
  stringstream ss;
  for(int i = 0; i < 100; i++) {
    ss << string64_to_int(&(val[i])) << ' ';
  }
  return base64_encode(ss.str());
}

string string64_to_int_10_(int val[10]) {
  stringstream ss;
  for(int i = 0; i < 10; i++) {
    ss << string64_to_int(&(val[i])) << ' ';
  }
  return base64_encode(ss.str());
}

string string64_to_int_10__100_(int val[10][100]) {
  stringstream ss;
  for(int i = 0; i < 10; i++) {
    ss << string64_to_int_100_(val[i]) << ' ';
  }
  return base64_encode(ss.str());
}

string string64_to_int_15_(int val[15]) {
  stringstream ss;
  for(int i = 0; i < 15; i++) {
    ss << string64_to_int(&(val[i])) << ' ';
  }
  return base64_encode(ss.str());
}

string string64_to_int_24_(int val[24]) {
  stringstream ss;
  for(int i = 0; i < 24; i++) {
    ss << string64_to_int(&(val[i])) << ' ';
  }
  return base64_encode(ss.str());
}

string string64_to_int_24__15_(int val[24][15]) {
  stringstream ss;
  for(int i = 0; i < 24; i++) {
    ss << string64_to_int_15_(val[i]) << ' ';
  }
  return base64_encode(ss.str());
}

string string64_to_int_4_(int val[4]) {
  stringstream ss;
  for(int i = 0; i < 4; i++) {
    ss << string64_to_int(&(val[i])) << ' ';
  }
  return base64_encode(ss.str());
}

string string64_to_int_4__10_(int val[4][10]) {
  stringstream ss;
  for(int i = 0; i < 4; i++) {
    ss << string64_to_int_10_(val[i]) << ' ';
  }
  return base64_encode(ss.str());
}

string string64_to_int_4__10__100_(int val[4][10][100]) {
  stringstream ss;
  for(int i = 0; i < 4; i++) {
    ss << string64_to_int_10__100_(val[i]) << ' ';
  }
  return base64_encode(ss.str());
}

string string64_to_rectangle_200_(rectangle val[200]) {
  stringstream ss;
  for(int i = 0; i < 200; i++) {
    ss << string64_to_rectangle(&(val[i])) << ' ';
  }
  return base64_encode(ss.str());
}

string string64_to_rectangle(rectangle *val) {
  stringstream ss;
  ss << string64_to_int(&((*val).x)) << ' ';
  ss << string64_to_int(&((*val).y)) << ' ';
  return base64_encode(ss.str());
}

string string64_to_s(s *val) {
  stringstream ss;
  ss << string64_to_int_4_((*val).m1) << ' ';
  ss << string64_to_int_4__10_((*val).m2) << ' ';
  ss << string64_to_int_4__10__100_((*val).m3) << ' ';
  return base64_encode(ss.str());
}


void parse_MT(MT *val, string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
}

void parse_Person(Person *val, string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
  args >> arg64;
   parse_string(&((*val).firstname), base64_decode(arg64));
  args >> arg64;
   parse_string(&((*val).lastname), base64_decode(arg64));
  args >> arg64;
   parse_int(&((*val).age), base64_decode(arg64));
}

void parse_StructWithArrays(StructWithArrays *val, string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
  args >> arg64;
   parse_int(&((*val).aNumber), base64_decode(arg64));
  args >> arg64;
  parse_Person_10_((*val).people, base64_decode(arg64));
}

void parse_ThreePeople(ThreePeople *val, string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
  args >> arg64;
   parse_Person(&((*val).p1), base64_decode(arg64));
  args >> arg64;
   parse_Person(&((*val).p2), base64_decode(arg64));
  args >> arg64;
   parse_Person(&((*val).p3), base64_decode(arg64));
}

void parse_Person_10_(Person val[10], string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
  for(int i = 0; i < 10; i++) {
    args >> arg64;
    parse_Person(&(val[i]), base64_decode(arg64));
  }
}

void parse_int_100_(int val[100], string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
  for(int i = 0; i < 100; i++) {
    args >> arg64;
    parse_int(&(val[i]), base64_decode(arg64));
  }
}

void parse_int_10_(int val[10], string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
  for(int i = 0; i < 10; i++) {
    args >> arg64;
    parse_int(&(val[i]), base64_decode(arg64));
  }
}

void parse_int_10__100_(int val[10][100], string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
  for(int i = 0; i < 10; i++) {
    args >> arg64;
    parse_int_100_(val[i], base64_decode(arg64));
  }
}

void parse_int_15_(int val[15], string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
  for(int i = 0; i < 15; i++) {
    args >> arg64;
    parse_int(&(val[i]), base64_decode(arg64));
  }
}

void parse_int_24_(int val[24], string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
  for(int i = 0; i < 24; i++) {
    args >> arg64;
    parse_int(&(val[i]), base64_decode(arg64));
  }
}

void parse_int_24__15_(int val[24][15], string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
  for(int i = 0; i < 24; i++) {
    args >> arg64;
    parse_int_15_(val[i], base64_decode(arg64));
  }
}

void parse_int_4_(int val[4], string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
  for(int i = 0; i < 4; i++) {
    args >> arg64;
    parse_int(&(val[i]), base64_decode(arg64));
  }
}

void parse_int_4__10_(int val[4][10], string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
  for(int i = 0; i < 4; i++) {
    args >> arg64;
    parse_int_10_(val[i], base64_decode(arg64));
  }
}

void parse_int_4__10__100_(int val[4][10][100], string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
  for(int i = 0; i < 4; i++) {
    args >> arg64;
    parse_int_10__100_(val[i], base64_decode(arg64));
  }
}

void parse_rectangle_200_(rectangle val[200], string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
  for(int i = 0; i < 200; i++) {
    args >> arg64;
    parse_rectangle(&(val[i]), base64_decode(arg64));
  }
}

void parse_rectangle(rectangle *val, string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
  args >> arg64;
   parse_int(&((*val).x), base64_decode(arg64));
  args >> arg64;
   parse_int(&((*val).y), base64_decode(arg64));
}

void parse_s(s *val, string arg) {
  stringstream args;
  string arg64;
  args.str(arg);
  args >> arg64;
  parse_int_4_((*val).m1, base64_decode(arg64));
  args >> arg64;
  parse_int_4__10_((*val).m2, base64_decode(arg64));
  args >> arg64;
  parse_int_4__10__100_((*val).m3, base64_decode(arg64));
}

void __area(string args64) {
  c150debug->printf(C150RPCDEBUG, "called areawith %s", args64.c_str());
  *GRADING << "stub: called __areawith base64string of " << args64 << endl;
  stringstream args;
  string arg64;
  args.str(base64_decode(args64));
  *GRADING << "stub: parsing arg - r" << endl;
  rectangle r;
  args >> arg64;
  parse_rectangle(&r, base64_decode(arg64));
  string resp = "area";
  c150debug->printf(C150RPCDEBUG,"stub: invoking area()");
  int retval = area(r);
  resp = resp + ' ' + string64_to_int(&retval);
  c150debug->printf(C150RPCDEBUG,"stub: area() has returned");
  *GRADING << "stub: function area returned. Response to client - " << resp << endl;
  RPCSTUBSOCKET->write(resp.c_str(), strlen(resp.c_str()) + 1);
}

void __findPerson(string args64) {
  c150debug->printf(C150RPCDEBUG, "called findPersonwith %s", args64.c_str());
  *GRADING << "stub: called __findPersonwith base64string of " << args64 << endl;
  stringstream args;
  string arg64;
  args.str(base64_decode(args64));
  *GRADING << "stub: parsing arg - tp" << endl;
  ThreePeople tp;
  args >> arg64;
  parse_ThreePeople(&tp, base64_decode(arg64));
  string resp = "findPerson";
  c150debug->printf(C150RPCDEBUG,"stub: invoking findPerson()");
  Person retval = findPerson(tp);
  resp = resp + ' ' + string64_to_Person(&retval);
  c150debug->printf(C150RPCDEBUG,"stub: findPerson() has returned");
  *GRADING << "stub: function findPerson returned. Response to client - " << resp << endl;
  RPCSTUBSOCKET->write(resp.c_str(), strlen(resp.c_str()) + 1);
}

void __func1(string args64) {
  c150debug->printf(C150RPCDEBUG, "called func1with %s", args64.c_str());
  *GRADING << "stub: called __func1with base64string of " << args64 << endl;
  stringstream args;
  string arg64;
  args.str(base64_decode(args64));
  string resp = "func1";
  c150debug->printf(C150RPCDEBUG,"stub: invoking func1()");
  func1();
  c150debug->printf(C150RPCDEBUG,"stub: func1() has returned");
  *GRADING << "stub: function func1 returned. Response to client - " << resp << endl;
  RPCSTUBSOCKET->write(resp.c_str(), strlen(resp.c_str()) + 1);
}

void __func2(string args64) {
  c150debug->printf(C150RPCDEBUG, "called func2with %s", args64.c_str());
  *GRADING << "stub: called __func2with base64string of " << args64 << endl;
  stringstream args;
  string arg64;
  args.str(base64_decode(args64));
  string resp = "func2";
  c150debug->printf(C150RPCDEBUG,"stub: invoking func2()");
  func2();
  c150debug->printf(C150RPCDEBUG,"stub: func2() has returned");
  *GRADING << "stub: function func2 returned. Response to client - " << resp << endl;
  RPCSTUBSOCKET->write(resp.c_str(), strlen(resp.c_str()) + 1);
}

void __func3(string args64) {
  c150debug->printf(C150RPCDEBUG, "called func3with %s", args64.c_str());
  *GRADING << "stub: called __func3with base64string of " << args64 << endl;
  stringstream args;
  string arg64;
  args.str(base64_decode(args64));
  string resp = "func3";
  c150debug->printf(C150RPCDEBUG,"stub: invoking func3()");
  func3();
  c150debug->printf(C150RPCDEBUG,"stub: func3() has returned");
  *GRADING << "stub: function func3 returned. Response to client - " << resp << endl;
  RPCSTUBSOCKET->write(resp.c_str(), strlen(resp.c_str()) + 1);
}

void __multiply(string args64) {
  c150debug->printf(C150RPCDEBUG, "called multiplywith %s", args64.c_str());
  *GRADING << "stub: called __multiplywith base64string of " << args64 << endl;
  stringstream args;
  string arg64;
  args.str(base64_decode(args64));
  *GRADING << "stub: parsing arg - x" << endl;
  float x;
  args >> arg64;
  parse_float(&x, base64_decode(arg64));
  *GRADING << "stub: parsing arg - y" << endl;
  float y;
  args >> arg64;
  parse_float(&y, base64_decode(arg64));
  string resp = "multiply";
  c150debug->printf(C150RPCDEBUG,"stub: invoking multiply()");
  float retval = multiply(x, y);
  resp = resp + ' ' + string64_to_float(&retval);
  c150debug->printf(C150RPCDEBUG,"stub: multiply() has returned");
  *GRADING << "stub: function multiply returned. Response to client - " << resp << endl;
  RPCSTUBSOCKET->write(resp.c_str(), strlen(resp.c_str()) + 1);
}

void __searchRectangles(string args64) {
  c150debug->printf(C150RPCDEBUG, "called searchRectangleswith %s", args64.c_str());
  *GRADING << "stub: called __searchRectangleswith base64string of " << args64 << endl;
  stringstream args;
  string arg64;
  args.str(base64_decode(args64));
  *GRADING << "stub: parsing arg - rects" << endl;
  rectangle rects[200];
  args >> arg64;
  parse_rectangle_200_(rects, base64_decode(arg64));
  string resp = "searchRectangles";
  c150debug->printf(C150RPCDEBUG,"stub: invoking searchRectangles()");
  searchRectangles(rects);
  c150debug->printf(C150RPCDEBUG,"stub: searchRectangles() has returned");
  *GRADING << "stub: function searchRectangles returned. Response to client - " << resp << endl;
  RPCSTUBSOCKET->write(resp.c_str(), strlen(resp.c_str()) + 1);
}

void __showsArraysofArrays(string args64) {
  c150debug->printf(C150RPCDEBUG, "called showsArraysofArrayswith %s", args64.c_str());
  *GRADING << "stub: called __showsArraysofArrayswith base64string of " << args64 << endl;
  stringstream args;
  string arg64;
  args.str(base64_decode(args64));
  *GRADING << "stub: parsing arg - x" << endl;
  int x[24];
  args >> arg64;
  parse_int_24_(x, base64_decode(arg64));
  *GRADING << "stub: parsing arg - y" << endl;
  int y[24][15];
  args >> arg64;
  parse_int_24__15_(y, base64_decode(arg64));
  *GRADING << "stub: parsing arg - z" << endl;
  int z[24][15];
  args >> arg64;
  parse_int_24__15_(z, base64_decode(arg64));
  string resp = "showsArraysofArrays";
  c150debug->printf(C150RPCDEBUG,"stub: invoking showsArraysofArrays()");
  int retval = showsArraysofArrays(x, y, z);
  resp = resp + ' ' + string64_to_int(&retval);
  c150debug->printf(C150RPCDEBUG,"stub: showsArraysofArrays() has returned");
  *GRADING << "stub: function showsArraysofArrays returned. Response to client - " << resp << endl;
  RPCSTUBSOCKET->write(resp.c_str(), strlen(resp.c_str()) + 1);
}

void __sqrt(string args64) {
  c150debug->printf(C150RPCDEBUG, "called sqrtwith %s", args64.c_str());
  *GRADING << "stub: called __sqrtwith base64string of " << args64 << endl;
  stringstream args;
  string arg64;
  args.str(base64_decode(args64));
  *GRADING << "stub: parsing arg - x" << endl;
  int x;
  args >> arg64;
  parse_int(&x, base64_decode(arg64));
  *GRADING << "stub: parsing arg - y" << endl;
  int y;
  args >> arg64;
  parse_int(&y, base64_decode(arg64));
  string resp = "sqrt";
  c150debug->printf(C150RPCDEBUG,"stub: invoking sqrt()");
  int retval = sqrt(x, y);
  resp = resp + ' ' + string64_to_int(&retval);
  c150debug->printf(C150RPCDEBUG,"stub: sqrt() has returned");
  *GRADING << "stub: function sqrt returned. Response to client - " << resp << endl;
  RPCSTUBSOCKET->write(resp.c_str(), strlen(resp.c_str()) + 1);
}

void __takesTwoArrays(string args64) {
  c150debug->printf(C150RPCDEBUG, "called takesTwoArrayswith %s", args64.c_str());
  *GRADING << "stub: called __takesTwoArrayswith base64string of " << args64 << endl;
  stringstream args;
  string arg64;
  args.str(base64_decode(args64));
  *GRADING << "stub: parsing arg - x" << endl;
  int x[24];
  args >> arg64;
  parse_int_24_(x, base64_decode(arg64));
  *GRADING << "stub: parsing arg - y" << endl;
  int y[24];
  args >> arg64;
  parse_int_24_(y, base64_decode(arg64));
  string resp = "takesTwoArrays";
  c150debug->printf(C150RPCDEBUG,"stub: invoking takesTwoArrays()");
  int retval = takesTwoArrays(x, y);
  resp = resp + ' ' + string64_to_int(&retval);
  c150debug->printf(C150RPCDEBUG,"stub: takesTwoArrays() has returned");
  *GRADING << "stub: function takesTwoArrays returned. Response to client - " << resp << endl;
  RPCSTUBSOCKET->write(resp.c_str(), strlen(resp.c_str()) + 1);
}

void __upcase(string args64) {
  c150debug->printf(C150RPCDEBUG, "called upcasewith %s", args64.c_str());
  *GRADING << "stub: called __upcasewith base64string of " << args64 << endl;
  stringstream args;
  string arg64;
  args.str(base64_decode(args64));
  *GRADING << "stub: parsing arg - s" << endl;
  string s;
  args >> arg64;
  parse_string(&s, base64_decode(arg64));
  string resp = "upcase";
  c150debug->printf(C150RPCDEBUG,"stub: invoking upcase()");
  string retval = upcase(s);
  resp = resp + ' ' + string64_to_string(&retval);
  c150debug->printf(C150RPCDEBUG,"stub: upcase() has returned");
  *GRADING << "stub: function upcase returned. Response to client - " << resp << endl;
  RPCSTUBSOCKET->write(resp.c_str(), strlen(resp.c_str()) + 1);
}

// 
//     __badFunction 
//
//   Pseudo-stub for missing functions. 
//
void __badFunction(char *functionName) {
  char doneBuffer[5] = "BAD";
  c150debug->printf(C150RPCDEBUG,"lotsofstuff.stub.cpp: received call for nonexistent function %s()", functionName);
  RPCSTUBSOCKET->write(doneBuffer, 5);
}

// ======================================================================
//                             DISPATCHER
//
//    This is the dispatcher for the stub. It reads the function name
//    from the input stream, and then calls the appropriate routine.
//
// ======================================================================

void dispatchFunction() {
  stringstream ss;
  string name;
  string args;
  ss.str(readFromStream());
  ss >> name;
  ss >> args;
  char* functionNameBuffer = (char*) name.c_str();
  //
  // Read the function name from the stream -- note
  // REPLACE THIS WITH YOUR OWN LOGIC DEPENDING ON THE
  // WIRE FORMAT YOU USE
  //
  if (!RPCSTUBSOCKET-> eof()) {
    if (strcmp(functionNameBuffer, "area") == 0) {
      __area(args);
    } else if (strcmp(functionNameBuffer, "findPerson") == 0) {
      __findPerson(args);
    } else if (strcmp(functionNameBuffer, "func1") == 0) {
      __func1(args);
    } else if (strcmp(functionNameBuffer, "func2") == 0) {
      __func2(args);
    } else if (strcmp(functionNameBuffer, "func3") == 0) {
      __func3(args);
    } else if (strcmp(functionNameBuffer, "multiply") == 0) {
      __multiply(args);
    } else if (strcmp(functionNameBuffer, "searchRectangles") == 0) {
      __searchRectangles(args);
    } else if (strcmp(functionNameBuffer, "showsArraysofArrays") == 0) {
      __showsArraysofArrays(args);
    } else if (strcmp(functionNameBuffer, "sqrt") == 0) {
      __sqrt(args);
    } else if (strcmp(functionNameBuffer, "takesTwoArrays") == 0) {
      __takesTwoArrays(args);
    } else if (strcmp(functionNameBuffer, "upcase") == 0) {
      __upcase(args);
    } else {
      __badFunction(functionNameBuffer);
    }
  }
}

// ======================================================================
//           GET_FUNCTION_NAME_FROM_STREAM
//
//    This routine reads the variable from the input stream.
//
// ======================================================================

void getFunctionNamefromStream(char *buffer, unsigned int bufSize) {
  unsigned int i;
  char* bufp;    // next char to read
  stringstream ss;
  bool readnull;
  ssize_t readlen;             // amount of data read from socket
  //
  // Read a message from the stream.
  // -1 in size below is to leave room for null
  //
  readnull = false;
  bufp = buffer;
  for (i = 0; i < bufSize; i++) {
    readlen = RPCSTUBSOCKET-> read(bufp, 1);  // read a byte
    // check for eof or error
    if (readlen == 0) {
      break;
    }
    // check for null and bump buffer pointer
    if (*bufp++ == '\0') {
      readnull = true;
      break;
    }
  }
  //
  // With TCP streams, we should never get a 0 length read
  // except with timeouts (which we're not setting in pingstreamserver)
  // or EOF
  //
  if (readlen == 0) {
    c150debug->printf(C150RPCDEBUG, "lotsofstuff.stub.cpp: read zero length message, checking EOF");
    if (RPCSTUBSOCKET-> eof()) {
      c150debug->printf(C150RPCDEBUG, "lotsofstuff.stub.cpp: EOF signaled on input");
    } else {
      throw C150Exception("lotsofstuff.stub.cpp: unexpected zero length read without eof");
    }
  }
  //
  // If we didn't get a null, input message was poorly formatted
  //
  else if(!readnull) 
    throw C150Exception("lotsofstuff.stub.cpp: method name not null terminated or too long");


  //
  // Note that eof may be set here for our caller to check
  //
}
string readFromStream() {
    stringstream name;          // name to build
    char bufc;                  // next char to read
    ssize_t readlen;            // amount of data read from socket

    while(1) {
        readlen = RPCSTUBSOCKET-> read(&bufc, 1);  // read a byte
        //
        // With TCP streams, we should never get a 0 length read except with
        // timeouts (which we're not setting in pingstreamserver) or EOF
        //
        if (readlen == 0) { 
            c150debug->printf(C150RPCDEBUG,"stub: read zero length message, checking EOF");
            if (RPCSTUBSOCKET-> eof()) {
                c150debug->printf(C150RPCDEBUG, "stub: EOF signaled on input");
                return name.str();
            } else { 
                throw C150Exception("stub: unexpected zero length read without eof");
            }
        }

        // check for null or space
        if (bufc == '\0') {
            return name.str();
        }
        name << bufc;
    }
    throw C150Exception("readFromString: This should never be thrown.");
}
