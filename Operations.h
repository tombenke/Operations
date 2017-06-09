/*
 * Operations.h
 * Library to communicate as a slave with its master through the serial RS-485 line
 * Created by Tamás Benke, May 19, 2017.
 * Released into the public domain.
 */

#ifndef _Operations_h
#define _Operations_h

#include <Arduino.h>

typedef struct operation {
    const char* cmd;
    void (*cmdFun)(const char*);
} OPERATION;

class Operations {
    private:
        /** Array which describe the operations incl. their name and the callback function */
        OPERATION* operations;

        /** The unique ID of the device */
        const char* deviceId;

        /** a string to hold incoming data */
        String readBuffer;

        /** a copy of the read buffer to parse */
        String parseBuffer;

        /** true if the readBuffer reached a line-end */
        boolean lineComplete;

        /** true if it is in transmission mode */
        boolean inTransmission;

        long int numCharsReceived;

        int rePin;

        int dePin;

        long unsigned int baudRate;

        void parseRequest(const char* buf);
        void response(const char* buf);
        const char* beginsWith(const char* buf, const char* with);

    public:
        Operations(int rePin, int dePin, const char* id, const long unsigned int baudRate, int bufferSize);
        void init(OPERATION* operations);
        void responseOk(const char* orig, const char* resp);
        void responseNotOk(const char* orig, const char* resp);
        void checkRequest();
};

extern const char* MSG_WRONG_ARGUMENTS;
extern const char* MSG_UNKNOWN_OPERATION;

#endif
