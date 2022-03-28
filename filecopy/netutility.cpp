// Written by the author himself
// execute the basic functions for file-transmitting
// as a library that can be linked to other files
// Author: Xudong Dai
// Date: 2022-02-26
// execute the function of a file-receiving server

#include <tuple>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <openssl/sha.h>
#include "c150dgmsocket.h"
#include "c150nastydgmsocket.h"
#include "c150debug.h"
#include "c150grading.h"
#include "netutility.hpp"
#include "time.h"

using namespace std;

namespace C150NETWORK {
    
    // max length of data section is 400
    // each packet could contain 512 packets at most,
    // 16 bytes reserved for 4 lengths, and we need to keep enough space for long filenames
    const int secLen = 400;

    // provide the function signature 
    vector<unsigned char> intToCharArray(int a);
    int charArrayToInt(unsigned char * ptr);
    // send a message
    // return the response
    Packet arrayToPacket(vector<char>& recBuffer) {
        unsigned char lenBuffer[4];
        // get the message type
        memcpy(lenBuffer, recBuffer.data(), 4);
        int messageType = charArrayToInt(lenBuffer);

        // get the length of filename
        memcpy(lenBuffer, recBuffer.data() + 4, 4);
        int filenameLen = charArrayToInt(lenBuffer);

        // read filename
        char cFilename[filenameLen];
        memcpy(cFilename, recBuffer.data() + 8, filenameLen);
        string filename(cFilename, cFilename + filenameLen);

        // read packet_id
        memcpy(lenBuffer, recBuffer.data() + 8 + filenameLen, 4);
        int packetID = charArrayToInt(lenBuffer);

        // read carryload length
        memcpy(lenBuffer, recBuffer.data() + 12 + filenameLen, 4);
        int carryloadLen = charArrayToInt(lenBuffer);
        if (carryloadLen < 0) {
              carryloadLen =secLen;
        }
        // read carryload 
        vector<char> carryload(recBuffer.data() + 16 + filenameLen, recBuffer.data() + 16 + filenameLen + carryloadLen);
        // cout << "extract carryload: " << carryload.size() << endl;

        // pack data into 
        Packet packet = make_tuple(messageType, filenameLen, filename, packetID, carryloadLen, carryload);

        // release the memory
        return packet;
    }

    int sendMessage(C150DgmSocket* sock, int messageType, const string fileName, int packetID, int carryloadLen, const char* fileBuffer, const int isClient) {
        if (sock == nullptr) {
            cerr << "Error DgmSocket:" << "  errno=" << strerror(errno) << endl;
            exit(16);
        }

        // make sure every messsage type is the power of 2
        if ((messageType & (messageType - 1)) != 0) {
            cerr << "Wrong message type: " << messageType << "  errno = " << strerror(errno) << endl;
            exit(32);
        } 

        char buffer[512];   // buffer is used to send message
        int fileNameLen = fileName.size();
        const char * cFileName = fileName.c_str();  // c-style string, easier to copy into array
        
        // copy data into buffer for the convenience of sending
        memcpy(buffer, intToCharArray(messageType).data(), sizeof(int));
        memcpy(buffer + sizeof(int), intToCharArray(fileNameLen).data(), sizeof(int));
        memcpy(buffer + sizeof(int) * 2, cFileName, fileNameLen);
        memcpy(buffer + sizeof(int) * 2 + fileNameLen, intToCharArray(packetID).data(), sizeof(int));
        memcpy(buffer + sizeof(int) * 3 + fileNameLen, intToCharArray(carryloadLen).data(), sizeof(int));
        memcpy(buffer + sizeof(int) * 4 + fileNameLen, fileBuffer, carryloadLen);
        buffer[sizeof(int) * 4 + fileNameLen + carryloadLen] = '\0';

        try {
            // for each sending step, wait for 10μs against network nastiness
            struct timespec req = {0, 10000}, rem;
            c150debug->printf(C150APPLICATION, "Send Message: %s. Try time 0.", buffer);

            // and send each packet for 5 times
            for (int i = 0; i < 5; i++) {
                sock->write(buffer, sizeof(buffer));
                nanosleep(&req, &rem);
            }

        } catch (C150Exception& e) {
            // Write to debug log
            c150debug->printf(C150ALWAYSLOG,"Caught C150NetworkException: %s\n",
                        e.formattedExplanation().c_str());
            // In case we're logging to a file, write to the console too
            cerr << fileName << ": caught C150NetworkException: " << e.formattedExplanation()\
                        << endl;
        }
        // if everything goes well, return 0
        return 0;
    }

    // receive the data from sock then pack it into a Packet
    // more complex logic is packed
    Packet receiveMessage(C150DgmSocket* sock) {
        try {
            char recBuffer[512];
            int len = 0;

            // read one message from buffer then break
            do {
                len = sock->read(recBuffer, sizeof(recBuffer));
            } while (len == 0);

            // return a vector
            vector<char> recData(recBuffer, recBuffer + 512);

            return arrayToPacket(recData);

        }  catch (C150NetworkException& e) {
            throw e;
        }
    }

    // bool, check whether the carryload itself is same to the packet
    // obsolete
    bool checkCarryload(Packet& pack, const char* carryLoad) {
        vector<char>& packCarryload = get<5>(pack);
        return strcmp(packCarryload.data(), carryLoad) == 0;
    }

    // The next to do is the 3 steps of sending a file
    // The returned value is the status code
    // if not 0, call the function again to send the file
    int sendFileBySock(C150DgmSocket* sock, string filename, vector<char>& fileBuffer) {
        // step 1: compute the packets of the data
        // const int maxAvailableSpace = 512 - 16 - 1 - filename.size();
        // const int secLen = maxAvailableSpace > 400 ? 400 : maxAvailableSpace;

        const int fileBufferLen = fileBuffer.size();
        int packets = fileBufferLen / secLen;
        if (fileBufferLen % secLen != 1) {
            ++packets;
        }
        cout << "Packets: " << packets << endl;
        cout << "FileBufferLen: " << fileBufferLen << endl; 
        try {
            // copy the buffer length into carrt load
            char cPacket[4];
            memcpy(cPacket, intToCharArray(fileBufferLen).data(), sizeof(int));

            int status = sendMessage(sock, 1, filename, 0, sizeof(cPacket), cPacket, 1);
            // step 2: send the fileBuffer
            const char* fileCopier = fileBuffer.data();

            int index = 0;
            int packetCount = 0;

            // the filebuffer would be sent out through each packet
            while (index < fileBufferLen) {
                if (index + secLen < fileBufferLen) {
                    status = sendMessage(sock, 4, filename, packetCount, secLen, fileCopier, 1);

                    index += secLen;
                    fileCopier += secLen;
                } else {  // called while the left length is shorter than a segment
                    status = sendMessage(sock, 4, filename, packetCount, fileBufferLen - index, fileCopier, 1);
                    
                    cout << "left len: " << fileBufferLen - index << endl;
	                index = fileBufferLen;
                }
                // sleep 1 μs for server to receive the packets
                if (packetCount % 10 == 0) {
                    struct timespec req = {0, 1000}, rem;
                    nanosleep(&req, &rem);
                }
                ++packetCount;
            }  
            // step 3: send the checksum
            // first, calculate it locally
            unsigned char checksum[20];
            SHA1((const unsigned char *) fileBuffer.data(), fileBufferLen, checksum);
            status = sendMessage(sock, 16, filename, packets + 1, 20, (const char *)checksum, 1);

            // receive the packet carrying checksum
            Packet header = receiveMessage(sock);
            int messageType = get<0>(header);
            string rcvFilename = get<2>(header);

            // since other confirmation packets are sent simultaneously, it needs to pick out the // 
            // exact packet of checksum
            while (messageType != 32 || filename != rcvFilename) {
                header = receiveMessage(sock);
                messageType = get<0>(header);
                rcvFilename = get<2>(header);
            }

            // sometimes the checksum packet may be dropped out, just go back to send the file again
            if (messageType != 32 || filename != rcvFilename) {
                return -2;
            }

            // pick out the checksum
            vector<char> rcvChecksum = get<5>(header);
            cout << "Received checksum len: " << rcvChecksum.size() << endl;
            cout << "local checksum len: " << sizeof(checksum) << endl;

            // print checksums from both sources
            for (int i = 0; i < 20; i++) {
                printf("%02x", (unsigned char)rcvChecksum[i]);
            }
            bool isSame = true;
            cout << endl;
            for (int i = 0; i < 20; i++) {
                printf("%02x", checksum[i]);
            }
            cout << endl;
            for (int i = 0; i < 20; i++) {
                if ((unsigned char)rcvChecksum[i] != checksum[i]) {
                    isSame = false;
                    break;
                }
            }
            
            // if the checksums mismatch, start from the very beginning
            if (!isSame) {
                return -1;
            }
        } catch (C150Exception& e) {
            // Write to debug log
            c150debug->printf(C150ALWAYSLOG,"Caught C150NetworkException: %s\n",
                        e.formattedExplanation().c_str());
            // In case we're logging to a file, write to the console too
            cerr << filename << ": caught C150NetworkException: " << e.formattedExplanation()\
                        << endl;
        }
        return 0;
    }

    // Implement all the functions of receiving files by the client
    // return the received packet through the short cut to deal with duplicate packets
    Packet receiveFileBySock(C150DgmSocket* sock, map<string, vector<char>>& fileQueue, Packet& prevPack) {
        Packet header = receiveMessage(sock);

        // extract information from the received packet
        int messageType = get<0>(header);
        string filename = get<2>(header);
        int packetID = get<3>(header);
        int carryloadLen = get<4>(header);
        vector<char> carry = get<5>(header);    

        int prevMessageType = get<0>(prevPack);
        string prevFilename = get<2>(prevPack);
        int prevPacketID = get<3>(prevPack);

        // used to save the status code of sending message
        int resp;

        unsigned int bufferLen;

        // if the key information is the same with previous one, return it so that it wouldn't be 
        // processed again
        if (messageType == prevMessageType && prevFilename == filename && packetID == prevPacketID) {
            return header;
        }

        // deal with handshake message
        if (messageType == 1) {
            unsigned int bufferLen;
            unsigned char cBufferLen[4];
            memcpy(cBufferLen, carry.data(), 4);
            bufferLen = charArrayToInt(cBufferLen);
            cout << "BufferLen: " << bufferLen << endl;

            // do not create the buffer twice
            if (fileQueue.count(filename) == 0) {
	        vector<char> fileBuffer(bufferLen);
                fileQueue[filename] = fileBuffer;
            }
            resp = sendMessage(sock, messageType << 1, filename, packetID, carryloadLen, carry.data(), 0);
        }

        // deal with the data packet
        else if (messageType == 4) {
            vector<char>& data = fileQueue[filename];

            cout << filename << " " << packetID << endl; 
            // copy the data into the buffer   
            for (int i = 0; i < carry.size(); i++) {
                data[packetID * secLen + i] = carry[i];
            }

            cout << filename << " " << packetID << endl;
            resp = sendMessage(sock, messageType << 1, filename, packetID, carryloadLen, carry.data(), 0);
        }

        else if (messageType == 16) {
            unsigned char checksum[20];
            vector<char>& fullFileBuffer = fileQueue[filename];

            cout << "new size of rcv packet: " << carry.size() << endl;

            SHA1((const unsigned char *) fullFileBuffer.data(), fullFileBuffer.size(), checksum);

            cout << "rcv data length: " << fullFileBuffer.size() << endl;

            // check the local and received checksums, but the server won't ask the client to 
            // send file again
            int isSame = strcmp(fullFileBuffer.data(), (const char *) checksum);
            resp = sendMessage(sock, messageType << 1, filename, packetID, 20, (const char *)checksum, 0);
        }

        return header;
    }

    // if use memcpy to copy 4-byte int into char array, the byte order would be big-endian
    // different CPU applies different order, X86-64 uses little-endian
    // network protocols all use big endian
    // to cross platforms, must define a tool function to control the transfer
    vector<unsigned char> intToCharArray(int a) {
        vector<unsigned char> arr(4, 0);
        int b = a, c = a, d = a;
        arr[3] = (d & 0xFF000000) >> 24 ;
        arr[2] = (c & 0xFF0000) >> 16;
        arr[1] = (b & 0xFF00) >> 8;
        arr[0] = a & 0xFF;
        return arr;
    }

    int charArrayToInt(unsigned char * ptr) {
        int a = 0;
        a = *ptr;
        a += (*(ptr + 1)) << 8;
        a += (*(ptr + 2)) << 16;
        a += (*(ptr + 3)) << 24;

        return a;
    }
}
