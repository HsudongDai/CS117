#ifndef __SHA1_H_INCLUDED__
#define __SHA1_H_INCLUDED__

#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <openssl/sha.h>

using namespace std;

namespace C150NETWORK {
    const unsigned char* getSHA1(string file);
}

#endif