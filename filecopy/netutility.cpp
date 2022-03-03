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

using namespace std;

namespace C150NETWORK {
    const int secLen = 400;
    // send a message
    // return the response
    Packet arrayToPacket(vector<char>& recBuffer) {
        cout << "RecBuffer is " ;
        cout << "RecBuffer len is: " << recBuffer.size() << endl;
        char lenBuffer[4];  // used to buffer all the 4-byte length

        // get the message type
        memcpy(lenBuffer, recBuffer.data(), 4);
        int messageType;
        sscanf(lenBuffer, "%d", &messageType);

        cout << "extract message type is " << messageType << endl;

        // get the length of filename
        memcpy(lenBuffer, recBuffer.data() + 4, 4);
        int filenameLen;
        sscanf(lenBuffer, "%d", &filenameLen);
        cout << "extract filename len: " << filenameLen << endl;

        // read filename
        char* cFilename = new char[filenameLen];
        memcpy(cFilename, recBuffer.data() + 8, filenameLen);
        string filename(cFilename);

        cout << "extract filename: " << filename << endl;
        delete[] cFilename;

        // read packet_id
        memcpy(lenBuffer, recBuffer.data() + 8 + filenameLen, 4);
        int packetID;
        cout << "extract packetID: " << packetID << endl;
        sscanf(lenBuffer, "%d", &packetID);

        // read carryload length
        memcpy(lenBuffer, recBuffer.data() + 12 + filenameLen, 4);
        int carryloadLen;
        cout << "extract carryload len: " << carryloadLen << endl;
        sscanf(lenBuffer, "%d", &carryloadLen);

        // read carryload 
        vector<char> carryload(recBuffer.data() + 16 + filenameLen, recBuffer.data() + 16 + filenameLen + carryloadLen);
        cout << "extract carryload: " << carryload.data() << endl;

        // pack data into 
        Packet packet = make_tuple(messageType, filenameLen, filename, packetID, carryloadLen, carryload);

        // release the memory
        // delete[] recBuffer;
        return packet;
    }

    vector<char> sendMessage(C150DgmSocket* sock, int messageType, const string fileName, int packetID, int carryloadLen, const char* fileBuffer, const int isClient) {
        if (sock == nullptr) {
            cerr << "Error DgmSocket:" << "  errno=" << strerror(errno) << endl;
            exit(16);
        }

        if ((messageType & (messageType - 1)) != 0) {
            cerr << "Wrong message type: " << messageType << "  errno = " << strerror(errno) << endl;
            exit(32);
        } 

        char buffer[512];
        char recBuffer[512];  // buffer is used to send message, recBuffer is used to receive response
        int fileNameLen = fileName.size();
	cout << "sendMessage, filenameLen: " << fileNameLen << endl;
        const char * cFileName = fileName.c_str();  // c-style string, easier to copy into array

        memcpy(buffer, &messageType, sizeof(int));
        memcpy(buffer + sizeof(int), &fileNameLen, sizeof(int));
        memcpy(buffer + sizeof(int) * 2, cFileName, fileNameLen);
        memcpy(buffer + sizeof(int) * 2 + fileNameLen, &packetID, sizeof(int));
        memcpy(buffer + sizeof(int) * 3 + fileNameLen, &carryloadLen, sizeof(int));
        memcpy(buffer + sizeof(int) * 4 + fileNameLen, fileBuffer, carryloadLen);
        buffer[sizeof(int) * 4 + fileNameLen + carryloadLen] = '\0';
        cout << "Write buffer size " << sizeof(buffer) << endl;

        try {
            c150debug->printf(C150APPLICATION, "Send Message: %s. Try time 0.", buffer);
            sock->write(buffer, sizeof(buffer));

            sock->read(recBuffer, sizeof(recBuffer));
            
            int retryCnt = 0;
            // if called in client, it must ensure the server has received this packet
            if (isClient == 1) {
                while (sock->timedout()) {
                    ++retryCnt;
                    c150debug->printf(C150APPLICATION, "Send Message: %s. Try time %d.", buffer, retryCnt);
                    sock->write(buffer, sizeof(buffer));
                    sock->read(recBuffer, sizeof(recBuffer));
                }
            } else {  // otherwise, do it best to ensure the client could receive
                for (int i = 0; i < 4; i++) {
                    sock->write(buffer, sizeof(buffer));
                }
            }
           
            c150debug->printf(C150APPLICATION, "Receive Message: %s. Try time(s). %d", recBuffer, retryCnt + 1);
        } catch (C150Exception& e) {
            // Write to debug log
            c150debug->printf(C150ALWAYSLOG,"Caught C150NetworkException: %s\n",
                        e.formattedExplanation().c_str());
            // In case we're logging to a file, write to the console too
            cerr << fileName << ": caught C150NetworkException: " << e.formattedExplanation()\
                        << endl;
        }
        vector<char> recData(recBuffer, recBuffer + 512);
        // }
        return recData;
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
            cout << "receive message len " << len << endl;

            vector<char> recData(recBuffer, recBuffer + 512);
            
            return arrayToPacket(recData);

        }  catch (C150NetworkException& e) {
            throw e;
        }
    }

    // bool check packet
    bool checkCarryload(Packet& pack, const char* carryLoad) {
        vector<char>& packCarryload = get<5>(pack);
        return strcmp(packCarryload.data(), carryLoad) == 0;
    }

    // The next to do is the 6 steps of sending a file
    // The returned value is the status code
    // if not 0, call the function again to send the file
    int sendFileBySock(C150DgmSocket* sock, string filename, const char* fileBuffer) {
        // step 1: compute the packets of the data
        // const int maxAvailableSpace = 512 - 16 - 1 - filename.size();
        // const int secLen = maxAvailableSpace > 400 ? 400 : maxAvailableSpace;

        const int fileBufferLen = sizeof(fileBuffer);
        int packets = fileBufferLen / secLen;
        if (fileBufferLen % secLen != 1) {
            ++packets;
        }
        try {
            char cPacket[8];
            memcpy(cPacket, &packets, sizeof(int));
            memcpy(cPacket + sizeof(int), &fileBufferLen, sizeof(int));
            vector<char> response = sendMessage(sock, 1, filename, 0, 4, cPacket, 1);
            Packet responsePacket = arrayToPacket(response);

            while (!checkCarryload(responsePacket, cPacket)) {
                response = sendMessage(sock, 1, filename, 0, 4, cPacket, 1);
                responsePacket = arrayToPacket(response);          
            }
            // step 2: send the fileBuffer
            int leftLen = fileBufferLen;
            int sendLen = secLen;
            const char* fileCopier = fileBuffer;
            for (int i = 1; i <= packets; i++) {
                leftLen -= secLen;
                sendLen = (leftLen < secLen) ? leftLen : secLen;
                response = sendMessage(sock, 4, filename, i, sendLen, fileCopier, 1);
                responsePacket = arrayToPacket(response);

                while (!checkCarryload(responsePacket, cPacket)) {
                    response = sendMessage(sock, 4, filename, i, sendLen, cPacket, 1);

                    responsePacket = arrayToPacket();  
                }
                fileCopier += sendLen;
            }

            // step 3: send the checksum
            unsigned char checksum[20];
            SHA1((const unsigned char *) fileBuffer, fileBufferLen, checksum);
            response = sendMessage(sock, 16, filename, packets + 1, 20, (const char *)checksum, 1);
            responsePacket = arrayToPacket(response);

            int breakTime = 0;  // If the checksum does not match after 3 times
            do {
                if (breakTime == 3) {
                    return -1;
                }
                ++breakTime;
                response = sendMessage(sock, 1, filename, 0, 20, (const char *) checksum, 1);
                responsePacket = arrayToPacket(response);          
            } while (!checkCarryload(responsePacket, (char *)checksum));
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

    // if any error occurs, throw 
    Packet receiveFileBySock(C150DgmSocket* sock, map<string, char*>& fileQueue, Packet& prevPack) {
        Packet header = receiveMessage(sock);
        int messageType = get<0>(header);
        string filename = get<2>(header);
        int packetID = get<3>(header);
        int carryloadLen = get<4>(header);
        vector<char> carry = get<5>(header);

        int prevMessageType = get<0>(prevPack);
        string prevFilename = get<2>(prevPack);
        int prevPacketID = get<3>(prevPack);

        vector<char> resp;

        if (messageType == prevMessageType && prevFilename == filename && packetID == prevPacketID) {
            return header;
        }

        if (messageType == 1) {
            int packets, bufferLen;
            char cPackets[4], cBufferLen[4];
            strncpy(cPackets, carry.data(), 4);
            strncpy(cBufferLen, carry.data() + 4, 4);
            sscanf(cPackets, "%d", packets);
            sscanf(cBufferLen, "%d", bufferLen);

            char* fileBuffer = new char[bufferLen];
            fileQueue[filename] = fileBuffer;
            resp = sendMessage(sock, messageType << 1, filename, packetID, carryloadLen, carry.data(), 0);
        }

        else if (messageType == 4) {
            char * fileBuffer = fileQueue[filename];

            const char * carryload = get<5>(header).data();
            memcpy(fileBuffer + packetID * secLen, carryload, carryloadLen);
            resp = sendMessage(sock, messageType << 1, filename, packetID, carryloadLen, carry.data(), 0);
        }

        else if (messageType == 16) {
            unsigned char checksum[20];
            const char * fileBuffer = fileQueue[filename];
            SHA1((const unsigned char *) fileBuffer, strlen(fileBuffer), checksum);
            
            const char * carryload = get<5>(header).data();
            int isSame = strcmp(carryload, (const char *) checksum);
            resp = sendMessage(sock, messageType << 1, filename, packetID, carryloadLen, (const char *)checksum, 0);
        }

        return header;
    }
}
