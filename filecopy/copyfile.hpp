#ifndef __COPYFILE_H_INCLUDED__
#define __COPYFILE_H_INCLUDED__

#include "c150nastyfile.h"        // for c150nastyfile & framework
#include "c150grading.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>                // for errno string formatting
#include <cerrno>
#include <cstring>               // for strerro
#include <iostream>               // for cout
#include <fstream>                // for input files

namespace C150NETWORK {
    string makeFileName(string dir, string name);
    void copyFile(string sourceDir, string fileName, string targetDir, int nastiness);
    int32_t checkCopyPaths(const char * srcDir, const char * tgrDir);
}

#endif