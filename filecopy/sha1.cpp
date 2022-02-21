// 
//            sha1tstFIXED
//
//     Author: Noah Mendelsohn
//
//     Test programming showing use of computation of 
//     sha1 hash function.
//
//     NOTE: problems were discovered using the incremental
//     version of the computation with SHA1_Update. This 
//     version, which computes the entire checksum at once,
//     seems to be reliable (if less flexible).
//
//     Note: this must be linked with the g++ -lssl directive. 
//


#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <openssl/sha.h>

using namespace std;

// Generate the SHA1 of the given file.
// When calling it, must remember to release the SHA1 array.
const unsigned char* getSHA1(const char *argv)
{
	ifstream *t;
	stringstream *buffer;

	unsigned char* obuf = new unsigned char[20];

	string res;

	t = new ifstream(argv);
	buffer = new stringstream;
	*buffer << t->rdbuf();
	SHA1((const unsigned char *)buffer->str().c_str(), 
			(buffer->str()).length(), obuf);
	delete t;
	delete buffer;
	return obuf;
}

// int main(int argc, char* argv[]) {
// 	const char * file = "nastyfiletest.cpp";

// 	auto res = getSHA1(file);
// 	cout << res << endl;
// 	return 0;
// }

