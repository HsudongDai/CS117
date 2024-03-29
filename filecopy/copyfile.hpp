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
#include <cstring>                // for strerro
#include <iostream>               // for cout
#include <fstream>                // for input files
#include <vector>

namespace C150NETWORK {
    // for the preliminary submission
    string makeFileName(string dir, string name);
    void copyFile(string sourceDir, string fileName, string targetDir, int nastiness);
    int32_t checkCopyPaths(char * srcDir, char * tgrDir);

    // interfaces of file I/O
    // regardless of file nastiness
    vector<char> safeReadFile(string sourceDir, string fileName, int nastiness);
    void safeWriteFile(string targetDir, string fileName, vector<char>& buffer, int nastiness);

    // provide a declaration in the header file
    // the program would check whether it is a valid directory
    void checkDirectory(char *dirname);
}

#endif
