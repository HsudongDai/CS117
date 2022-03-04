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
    const int secLen = 200;
    vector<char> intToCharArray(int a);
    int charArrayToInt(char * ptr);
    // send a message
    // return the response
    Packet arrayToPacket(vector<char>& recBuffer) {

        // cout << "RecBuffer is " ;
        cout << "RecBuffer len is: " << recBuffer.size() << endl;
        char lenBuffer[4];  // used to buffer all the 4-byte length

        // get the message type
        memcpy(lenBuffer, recBuffer.data(), 4);
        int messageType = charArrayToInt(lenBuffer);

        // cout << "extract message type is " << messageType << endl;

        // get the length of filename
        memcpy(lenBuffer, recBuffer.data() + 4, 4);
        int filenameLen = charArrayToInt(lenBuffer);
        // cout << "extract filename len: " << filenameLen << endl;

        // read filename
        char cFilename[filenameLen];
        memcpy(cFilename, recBuffer.data() + 8, filenameLen);
        string filename(cFilename, cFilename + filenameLen);

        // cout << "extract filename: " << filename << endl;

        // read packet_id
        memcpy(lenBuffer, recBuffer.data() + 8 + filenameLen, 4);
        int packetID = charArrayToInt(lenBuffer);
        // cout << "extract packetID: " << packetID << endl;

        // read carryload length
        memcpy(lenBuffer, recBuffer.data() + 12 + filenameLen, 4);
        int carryloadLen = charArrayToInt(lenBuffer);
        //  cout << "extract carryload len: " << carryloadLen << endl;
        if (carryloadLen < 0) {
        // carryloadLen = recBuffer.size() - 12 - filenameLen;
              carryloadLen =secLen;
        }
        // read carryload 
        vector<char> carryload(recBuffer.data() + 16 + filenameLen, recBuffer.data() + 16 + filenameLen + carryloadLen);
        // cout << "extract carryload: " << carryload.size() << endl;

        // pack data into 
        Packet packet = make_tuple(messageType, filenameLen, filename, packetID, carryloadLen, carryload);

        // release the memory
        // delete[] recBuffer;
        return packet;
    }

    int sendMessage(C150DgmSocket* sock, int messageType, const string fileName, int packetID, int carryloadLen, const char* fileBuffer, const int isClient) {
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
	    //cout << "sendMessage, filenameLen: " << fileNameLen << endl;
        const char * cFileName = fileName.c_str();  // c-style string, easier to copy into array

        memcpy(buffer, intToCharArray(messageType).data(), sizeof(int));
        memcpy(buffer + sizeof(int), intToCharArray(fileNameLen).data(), sizeof(int));
        memcpy(buffer + sizeof(int) * 2, cFileName, fileNameLen);
        memcpy(buffer + sizeof(int) * 2 + fileNameLen, intToCharArray(packetID).data(), sizeof(int));
        memcpy(buffer + sizeof(int) * 3 + fileNameLen, intToCharArray(carryloadLen).data(), sizeof(int));
        memcpy(buffer + sizeof(int) * 4 + fileNameLen, fileBuffer, carryloadLen);
        buffer[sizeof(int) * 4 + fileNameLen + carryloadLen] = '\0';
     //   cout << "Write buffer" << buffer  + sizeof(int) * 4 + fileNameLen << endl;        
//cout << "Write buffer size " << sizeof(buffer) << endl;

        try {
            c150debug->printf(C150APPLICATION, "Send Message: %s. Try time 0.", buffer);
            sock->write(buffer, sizeof(buffer));
            
            // int retryCnt = 0;
            // if called in client, it must ensure the server has received this packet
            // if (isClient == 1) {
            //     sock->read(recBuffer, sizeof(recBuffer));
            //     while (sock->timedout()) {
            //         ++retryCnt;
            //         c150debug->printf(C150APPLICATION, "Send Message: %s. Try time %d.", buffer, retryCnt);
            //         sock->write(buffer, sizeof(buffer));
            //         // sock->read(recBuffer, sizeof(recBuffer));
            //     }
            // } else {  // otherwise, do it best to ensure the client could receive
            for (int i = 0; i < 4; i++) {
                sock->write(buffer, sizeof(buffer));
            }
           
//            c150debug->printf(C150APPLICATION, "Receive Message: %s. Try time(s). %d", recBuffer, retryCnt + 1);

        } catch (C150Exception& e) {
            // Write to debug log
            c150debug->printf(C150ALWAYSLOG,"Caught C150NetworkException: %s\n",
                        e.formattedExplanation().c_str());
            // In case we're logging to a file, write to the console too
            cerr << fileName << ": caught C150NetworkException: " << e.formattedExplanation()\
                        << endl;
        }
        // vector<char> recData(recBuffer, recBuffer + 512);
        // }
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
            char* cPacket = new char[4];
            memcpy(cPacket, intToCharArray(packets).data(), sizeof(int));
            // memcpy(cPacket + sizeof(int), intToCharArray(fileBufferLen).data(), sizeof(int));
            // delete[] cPacket;
            int status = sendMessage(sock, 1, filename, 0, sizeof(cPacket), cPacket, 1);
            // Packet responsePacket = ;

            // while (!checkCarryload(responsePacket, cPacket)) {
            //     response = sendMessage(sock, 1, filename, 0, 4, cPacket, 1);
            //     responsePacket = arrayToPacket(response);          
            // }
            // cout << "step 1 done" << endl;
            // step 2: send the fileBuffer
            const char* fileCopier = fileBuffer.data();

            int index = 0;
            int packetCount = 0;
            while (index < fileBufferLen) {
                if (index + secLen < fileBufferLen) {
                    status = sendMessage(sock, 4, filename, packetCount, secLen, fileCopier, 1);

           //         cout << "send content " << filename << " packet count: " << packetCount << endl;
                    index += secLen;
                    fileCopier += secLen;
                } else {
                    status = sendMessage(sock, 4, filename, packetCount, fileBufferLen - index, fileCopier, 1);
                    
                    cout << "left len: " << fileBufferLen - index << endl;
	            index = fileBufferLen;
                    
             //       cout << "send content " << filename << " packet count: " << packetCount << endl;
                }
                //cout << "Current index is " << index << endl;
                ++packetCount;
            }  // step 3: send the checksum
            unsigned char checksum[20];
            SHA1((const unsigned char *) fileBuffer.data(), fileBufferLen, checksum);
            status = sendMessage(sock, 16, filename, packets + 1, 20, (const char *)checksum, 1);
            // responsePacket = arrayToPacket(response);

            // int breakTime = 0;  // If the checksum does not match after 3 times
            // do {
            //     if (breakTime == 3) {
            //         return -1;
            //     }
            //     ++breakTime;
            //     response = sendMessage(sock, 1, filename, 0, 20, (const char *) checksum, 1);
            //     responsePacket = arrayToPacket(response);          
            // } while (!checkCarryload(responsePacket, (char *)checksum));
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
    Packet receiveFileBySock(C150DgmSocket* sock, map<string, vector<char>>& fileQueue, Packet& prevPack) {
        Packet header = receiveMessage(sock);
        cout << "Packet received" << endl;

        int messageType = get<0>(header);
        string filename = get<2>(header);
        int packetID = get<3>(header);
        int carryloadLen = get<4>(header);
        vector<char> carry = get<5>(header);    

        int prevMessageType = get<0>(prevPack);
        string prevFilename = get<2>(prevPack);
        int prevPacketID = get<3>(prevPack);

        int resp;

        if (messageType == prevMessageType && prevFilename == filename && packetID == prevPacketID) {
            return header;
        }
        cout << messageType << " " << filename << " " << packetID << " " << carryloadLen << " " << carry.data() << endl;

        if (messageType == 1) {
            int packets;
<<<<<<< HEAD
            // unsigned bufferLen;
            // char cPackets[4], cBufferLen[4];
=======
            int bufferLen;
            char cPackets[4], cBufferLen[4];
>>>>>>> 5226d241df809ccf8dc1bbd5780263faba4ca953
            memcpy(cPackets, carry.data(), 4);
            // memcpy(cBufferLen, carry.data() + 4, 4);
            packets = charArrayToInt(cPackets);
            // bufferLen = charArrayToInt(cBufferLen);
            cout << "Packets: " << packets << endl;
            // cout << "BufferLen: " << bufferLen << endl;
            
<<<<<<< HEAD
            vector<char> fileBuffer(packets * secLen);
            fileQueue[filename] = fileBuffer;
=======
            if (packets < 0 || bufferLen > 10000000) {
                return prevPack;
            }
   
            if (fileQueue.count(filename) == 0) {
		vector<char> fileBuffer(packets * secLen);
                fileQueue[filename] = fileBuffer;
            }
>>>>>>> 5226d241df809ccf8dc1bbd5780263faba4ca953
            resp = sendMessage(sock, messageType << 1, filename, packetID, carryloadLen, carry.data(), 0);
        }

        else if (messageType == 4) {
            vector<char>& data = fileQueue[filename];
            cout << "Is data null: " << (data.size()) << endl;

            const char * carryload = get<5>(header).data();
            string arrived(carryload, carryload + carry.size());
            cout << "arrived: " << arrived << endl;
            
            for (int i = 0; i < carry.size(); i++) {
                data[packetID * secLen + i] = carry[i];
            }
            resp = sendMessage(sock, messageType << 1, filename, packetID, carryloadLen, carry.data(), 0);
        }

        else if (messageType == 16) {
            unsigned char checksum[20];
            const char * fileBuffer = fileQueue[filename].data();
            SHA1((const unsigned char *) fileBuffer, strlen(fileBuffer), checksum);
            
            const char * carryload = get<5>(header).data();
            int isSame = strcmp(carryload, (const char *) checksum);
            resp = sendMessage(sock, messageType << 1, filename, packetID, carryloadLen, (const char *)checksum, 0);
        }

        return header;
    }

    vector<char> intToCharArray(int a) {
        vector<char> arr(4, 0);
        arr[0] = a & 0xFF;
        arr[1] = (a >> 8) & 0xFF;
        arr[2] = (a >> 16) & 0xFF;
        arr[3] = (a >> 24) & 0xFF;

        return arr;
    }

    int charArrayToInt(char * ptr) {
        int a = 0;
        a |= *ptr;
        a |= (*(ptr + 1)) << 8;
        a |= (*(ptr + 2)) << 16;
        a |= (*(ptr + 3)) << 24;

        return a;
    }
}
