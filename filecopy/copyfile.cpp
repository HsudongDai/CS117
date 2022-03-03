#include "c150nastyfile.h"        // for c150nastyfile & framework
#include <vector>
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

// using namespace C150NETWORK;

// void copyFile(string sourceDir, string fileName, string targetDir, int nastiness); // fwd decl
// bool C150NETWORK::isFile(string fname);
// void C150NETWORK::checkDirectory(char *dirname);
// const char * C150NETWORK::safeReadFile(string sourceDir, string fileName, int nastiness);
// void C150NETWORK::safeWriteFile(string targetDir, string fileName, const char * buffer, int nastiness);

namespace C150NETWORK {
    string makeFileName(string dir, string name) {
        stringstream ss;

        ss << dir;
        // make sure dir name ends in /
        if (dir.substr(dir.length()-1,1) != "/")
            ss << '/';
        ss << name;     // append file name to dir
        return ss.str();  // return dir/name 
    }


    // ------------------------------------------------------
    //
    //                   checkDirectory
    //
    //  Make sure directory exists
    //     
    // ------------------------------------------------------

    void checkDirectory(char *dirname) {
        struct stat statbuf;  
        if (lstat(dirname, &statbuf) != 0) {
        fprintf(stderr,"Error stating supplied source directory %s\n", dirname);
        exit(8);
        }

        if (!S_ISDIR(statbuf.st_mode)) {
        fprintf(stderr,"File %s exists but is not a directory\n", dirname);
        exit(8);
        }
    }


    // ------------------------------------------------------
    //
    //                   isFile
    //
    //  Make sure the supplied file is not a directory or
    //  other non-regular file.
    //     
    // ------------------------------------------------------

    bool isFile(string fname) {
        const char *filename = fname.c_str();
        struct stat statbuf;  
        if (lstat(filename, &statbuf) != 0) {
        fprintf(stderr,"isFile: Error stating supplied source file %s\n", filename);
        return false;
        }

        if (!S_ISREG(statbuf.st_mode)) {
        fprintf(stderr,"isFile: %s exists but is not a regular file\n", filename);
        return false;
        }
        return true;
    }


    // ------------------------------------------------------
    //
    //                   copyFile
    //
    // Copy a single file from sourcdir to target dir
    //
    // ------------------------------------------------------

    void copyFile(string sourceDir, string fileName, string targetDir, int nastiness) {

        //
        //  Misc variables, mostly for return codes
        //
        void *fopenretval;
        size_t len;
        string errorString;
        char *buffer;
        struct stat statbuf;  
        size_t sourceSize;

        //
        // Put together directory and filenames SRC/file TARGET/file
        //
        string sourceName = makeFileName(sourceDir, fileName);
        string targetName = makeFileName(targetDir, fileName);

        //
        // make sure the file we're copying is not a directory
        // 
        if (!isFile(sourceName)) {
            cerr << "Input file " << sourceName << " is a directory or other non-regular file. Skipping" << endl;

            return;
        }

        cout << "Copying " << sourceName << " to " << targetName << endl;

        // - - - - - - - - - - - - - - - - - - - - -
        // LOOK HERE! This demonstrates the 
        // COMP 150 Nasty File interface
        // - - - - - - - - - - - - - - - - - - - - -

        try {

            //
            // Read whole input file 
            //
            if (lstat(sourceName.c_str(), &statbuf) != 0) {
                fprintf(stderr,"copyFile: Error stating supplied source file %s\n", sourceName.c_str());
                exit(20);
            }

            //
            // Make an input buffer large enough for
            // the whole file
            //
            sourceSize = statbuf.st_size;
            buffer = (char *)malloc(sourceSize);

            //
            // Define the wrapped file descriptors
            //
            // All the operations on outputFile are the same
            // ones you get documented by doing "man 3 fread", etc.
            // except that the file descriptor arguments must
            // be left off.
            //
            // Note: the NASTYFILE type is meant to be similar
            //       to the Unix FILE type
            //
            NASTYFILE inputFile(nastiness);      // See c150nastyfile.h for interface
            NASTYFILE outputFile(nastiness);     // NASTYFILE is supposed to
                                                    // remind you of FILE
                                                    //  It's defined as: 
                                                    // typedef C150NastyFile NASTYFILE

            // do an fopen on the input file
            fopenretval = inputFile.fopen(sourceName.c_str(), "rb");  
                                                    // wraps Unix fopen
                                                    // Note rb gives "read, binary"
                                                    // which avoids line end munging

            if (fopenretval == NULL) {
                cerr << "Error opening input file " << sourceName << 
                    " errno=" << strerror(errno) << endl;
                exit(12);
            }


        // 
        // Read the whole file
        //
        len = inputFile.fread(buffer, 1, sourceSize);
        if (len != sourceSize) {
            cerr << "Error reading file " << sourceName << 
                "  errno=" << strerror(errno) << endl;
            exit(16);
        }

        if (inputFile.fclose() != 0 ) {
            cerr << "Error closing input file " << targetName << 
                " errno=" << strerror(errno) << endl;
            exit(16);
        }


        //
        // do an fopen on the output file
        //
        fopenretval = outputFile.fopen(targetName.c_str(), "wb");  
                                        // wraps Unix fopen
                                        // Note wb gives "write, binary"
                                        // which avoids line and munging

        //
        // Write the whole file
        //
        len = outputFile.fwrite(buffer, 1, sourceSize);
        if (len != sourceSize) {
            cerr << "Error writing file " << targetName << 
                "  errno=" << strerror(errno) << endl;
            exit(16);
        }

        if (outputFile.fclose() == 0 ) {
            cout << "Finished writing file " << targetName <<endl;
        } else {
            cerr << "Error closing output file " << targetName << 
                " errno=" << strerror(errno) << endl;
            exit(16);
        }

        //
        // Free the input buffer to avoid memory leaks
        //
        free(buffer);

        //
        // Handle any errors thrown by the file framekwork
        //
        } catch (C150Exception& e) {
            cerr << "nastyfiletest:copyfile(): Caught C150Exception: " << 
            e.formattedExplanation() << endl;
        }      
    }

    int32_t checkCopyPaths(char * srcDir, char * tgrDir) {
        //
        // Check the parameters of function
        //
        if (srcDir != nullptr && !strlen(srcDir)) {
            fprintf(stderr,"The source path is a wrong string: %s", srcDir);
            return false;
        }

        if (tgrDir != nullptr && !strlen(tgrDir)) {
            fprintf(stderr,"The target path is a wrong string: %s", tgrDir);
            return false;
        }

        DIR *SRC;                   // Unix descriptor for open directory
        DIR *TARGET;                // Unix descriptor for target

        //
        // Make sure source and target dirs exist
        //
        checkDirectory(srcDir);
        checkDirectory(tgrDir);

        //
        // Open the source directory
        //
        SRC = opendir(srcDir);
        if (SRC == NULL) {
            fprintf(stderr,"Error opening source directory %s\n", srcDir);     
            return 1;
        }
        closedir(SRC);

        //
        // Open the target directory (we don't really need to do
        // this here, but it will give cleaner error messages than
        // waiting for individual file writes to fail.
        //
        TARGET = opendir(tgrDir);
        if (TARGET == NULL) {
            fprintf(stderr,"Error opening target directory %s \n", tgrDir);     
            return 2;
        } 
        closedir(TARGET);       // we just wanted to be sure it was there
                                // SRC dir remains open for loop below
        return 0;
    }

    vector<char> safeReadFile(string sourceDir, string fileName, int nastiness) {
        void* fopenretval;
        size_t len1, len2;
        string errorString;
        char *buffer1, *buffer2;
        struct stat statbuf;  
        size_t sourceSize;

        string sourceName = makeFileName(sourceDir, fileName);

        if (!isFile(sourceName)) {
            cerr << "Input file " << sourceName << " is a directory or other non-regular file. Skipping" << endl;
            vector<char> nothing;
            return nothing;
        }

        cout << "Copying " << sourceName << endl;

        try {
            //
            // Read whole input file 
            //
            if (lstat(sourceName.c_str(), &statbuf) != 0) {
                fprintf(stderr,"copyFile: Error stating supplied source file %s\n", sourceName.c_str());
                exit(20);
            }

            // Make an input buffer large enough for
            // the whole file
            //
            sourceSize = statbuf.st_size;
            buffer1 = new char[sourceSize];
            buffer2 = new char[sourceSize];

            NASTYFILE inputFile(nastiness); 

            fopenretval = inputFile.fopen(sourceName.c_str(), "rb");  
                                         // wraps Unix fopen
                                         // Note rb gives "read, binary"
                                         // which avoids line end munging
  
            if (fopenretval == NULL) {
                cerr << "Error opening input file " << sourceName << 
                    " errno=" << strerror(errno) << endl;
                exit(12);
            }
            len1 = inputFile.fread(buffer1, 1, sourceSize);
            len2 = inputFile.fread(buffer2, 1, sourceSize);

            bool isSame = strcmp(buffer1, buffer2);

            // make sure the buffer keeps the correct content
            // the lengths must match, hence excludes the exception
            while (len1 != len2 || !isSame) {
                len1 = inputFile.fread(buffer1, 1, sourceSize);
                len2 = inputFile.fread(buffer2, 1, sourceSize);

                isSame = strcmp(buffer1, buffer2);
            }

            // close the inputfile
            if (inputFile.fclose() != 0 ) {
                cerr << "Error closing input file " << sourceName << 
                    " errno=" << strerror(errno) << endl;
                exit(16);
            }
        } catch (C150Exception& e) {
            delete[] buffer1;
            delete[] buffer2;

            cerr << "nastyfiletest:copyfile(): Caught C150Exception: " << 
            e.formattedExplanation() << endl;
        }
        delete[] buffer2;
        cout << "Read File " << fileName << "'s content is: " << buffer1 << endl;
        return vector<char>(buffer1, buffer1 + sourceSize);
    }

    void safeWriteFile(string targetDir, string fileName, const char * buffer, int nastiness) {
        void *fopenretval;
        size_t len1, len2;
        size_t sourceSize = strlen(buffer);
        string errorString;
        struct stat statbuf;  

        string targetName = makeFileName(targetDir, fileName);
        cout << "Writing " << targetName << " to " << targetName << endl;

        try {
            NASTYFILE outputFile(nastiness); 
            fopenretval = outputFile.fopen(targetName.c_str(), "wb");  

            len1 = outputFile.fwrite(buffer, 1, sourceSize);
            len2 = outputFile.fwrite(buffer, 1, sourceSize);

            while (len1 != len2 || len1 != sourceSize || len2 != sourceSize) {
                len1 = outputFile.fwrite(buffer, 1, sourceSize);
                len2 = outputFile.fwrite(buffer, 1, sourceSize);
            }

            if (outputFile.fclose() == 0 ) {
                cout << "Finished writing file " << targetName <<endl;
                } else {
                cerr << "Error closing output file " << targetName << 
                    " errno=" << strerror(errno) << endl;
                exit(16);
            }
            cout << "Write File " << fileName << "'s content is: " << buffer << endl;
            delete[] buffer;
        } catch (C150Exception& e) {
            delete[] buffer;
            cerr << "nastyfiletest:copyfile(): Caught C150Exception: " << 
            e.formattedExplanation() << endl;
        }    
    }
}
