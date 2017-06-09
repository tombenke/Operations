#include <Operations.h>

const char* MSG_WRONG_ARGUMENTS = "Wrong arguments.";
const char* MSG_UNKNOWN_OPERATION = "UNKNOWN OPERATION";

Operations::Operations(int rePin, int dePin, const char* id, const long unsigned int baudRate, int bufferSize) {
    this->deviceId = id;
    this->rePin = rePin;
    this->dePin = dePin;
    this->baudRate = baudRate;

    // reserve bytes for the readBuffer and the parseBuffer
    this->readBuffer.reserve(bufferSize);
    this->readBuffer = "";
    this->parseBuffer.reserve(bufferSize);
    this->parseBuffer = "";
    this->lineComplete = false;
    this->inTransmission = false;
    this->numCharsReceived = 0L;
}


void Operations::init(OPERATION* operations) {
    this->operations = operations;

    // Initialize the MAX485 line control pins
    pinMode(this->rePin, OUTPUT);
    pinMode(this->dePin, OUTPUT);
 
    digitalWrite(this->rePin, LOW);
    digitalWrite(this->dePin, LOW);

    // initialize the serial port
    Serial.begin(this->baudRate);
}


void Operations::parseRequest(const char* buf) {
    const char* cmdBuf = buf;

    cmdBuf = beginsWith(buf, this->deviceId);
    if (cmdBuf) {

        int i=0;
        while(this->operations[i].cmd != 0L) {
            if (beginsWith(cmdBuf, this->operations[i].cmd)) {
                this->operations[i].cmdFun(buf);
                break;
            }
            i++;
        }

        if (this->operations[i].cmd == 0L) {
            // Could reach the end of operation table without matching
            responseNotOk(buf, MSG_UNKNOWN_OPERATION);
        }
    }
}

void Operations::checkRequest() {
    while (Serial.available()) {

        // get the new byte:
        char inChar = (char)Serial.read();

        // add it to the readBuffer:
        this->readBuffer += inChar;

        // if the incoming character is a newline, set a flag
        // so the main loop can do something about it:
        if (inChar == '\n') {
            this->lineComplete = true;
            break;
        }
    }
  
    // process the string when a newline arrives
    if (this->lineComplete) {
        this->lineComplete = false;
        this->parseBuffer = this->readBuffer;
        // clear the string
        this->readBuffer = "";

        if (this->inTransmission) {
            // Throw away the incoming string, because it is just an echo
            this->inTransmission = false;
        } else {
            this->parseRequest(parseBuffer.c_str());
            this->parseBuffer = "";
        }
    }
}

void Operations::response(const char* buf) {
    this->inTransmission = true;
    digitalWrite(this->dePin, HIGH);
    Serial.write(buf);
    Serial.flush();
    digitalWrite(this->dePin, LOW);
    return;
}

const char* Operations::beginsWith(const char* buf, const char* with) {
    const char* rval = 0L;
    int len = strlen(with);

    if (strncmp(buf, with, len) == 0) rval = &buf[len+1];

    return rval;
}

void Operations::responseOk(const char* orig, const char* resp) {
    char respBuf[64] = "";

    strncat(respBuf, orig, strcspn(orig, "\r\n"));
    if (resp != 0L) {
        strcat(respBuf, " ");
        strcat(respBuf, resp);
    }
    strcat(respBuf, " - OK\n");

    this->response(respBuf);
}

void Operations::responseNotOk(const char* orig, const char* resp) {
    char respBuf[64] = "";

    strncat(respBuf, orig, strcspn(orig, "\r\n"));
    strcat(respBuf, " - NOK");
    if (resp != 0L) {
        strcat(respBuf, " : ");
        strcat(respBuf, resp);
    }
    strcat(respBuf, "\n");
    this->response(respBuf);
}

/*
void Operations::initCommander(int re, int de, const char* id, const long unsigned int baudRate, int bufferSize, OPERATION* ops) {
    rePin = re;
    dePin = de;

    // Initialize the MAX485 line control pins
    pinMode(rePin, OUTPUT);
    pinMode(dePin, OUTPUT);
 
    digitalWrite(rePin, LOW);
    digitalWrite(dePin, LOW);

    // initialize serial
    Serial.begin(baudRate);

    // reserve bytes for the readBuffer and the parseBuffer
    readBuffer.reserve(bufferSize);
    parseBuffer.reserve(bufferSize);
    deviceId = id;
    operations = ops;
}
*/

