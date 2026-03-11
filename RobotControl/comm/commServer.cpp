/*
 * commServer.cpp
 *
 *  Created on: Jan 13, 2025
 *      Author: dkiovics
 */

#include "comm/commServer.h"
#include "comm/replyHandlers.h"

using namespace comm;

uint8_t CommServer::calculateChecksum(const uint8_t* data, size_t size) const {
    uint8_t checksum = 0;
    for (size_t i = 0; i < size; i++)
        checksum += data[i];
    return checksum;
}

void CommServer::init(IBinaryUart* uart, RequestReply** rrTypes, size_t rrTypesCnt) {
    this->uart = uart;
    this->rrTypes = rrTypes;
    this->rrTypesCnt = rrTypesCnt;
}

void CommServer::setRequestReplyTypes(RequestReply** rrTypes, size_t rrTypesCnt) {
    this->rrTypes = rrTypes;
    this->rrTypesCnt = rrTypesCnt;
}

void CommServer::processRequests() {
    size_t cnt = 0;
    uint32_t status = uart->processIncomingData(requestBuffer, cnt);
    if (status & status::RX_DATA_PRESENT) {
        if (cnt < 4)
            return;
        uint8_t ID = requestBuffer[0] & 0x3f;
        uint8_t type = (requestBuffer[0] & 0xc0) >> 6;
        uint8_t sequenceNumber = requestBuffer[1];
        uint8_t checksum = requestBuffer[cnt - 1];
        if (checksum != calculateChecksum(requestBuffer, cnt - 1))
            return;

        if (type == 0) // request
        {
            for (size_t i = 0; i < rrTypesCnt; i++) {
                if (rrTypes[i]->getRequestID() == ID) {
                    replyBuffer[0] = ID | 0x40;
                    replyBuffer[1] = sequenceNumber;
                    size_t replySize = 0;
                    if (rrTypes[i]->process(requestBuffer + 2, cnt - 3, replyBuffer + 2, replySize)) {
                        replySize++; // add the ID byte
                        replySize++; // add the sequence number byte
                        replyBuffer[replySize] = calculateChecksum(replyBuffer, replySize);
                        replySize++; // add the checksum byte
                        uart->writeData(replyBuffer, replySize);
                    }
                    break;
                }
            }
        }
    }
}

uint8_t* CommServer::getStreamDataWriteBuffer(uint8_t streamID) {
    replyBuffer[0] = streamID | 0x80;
    return replyBuffer + 1;
}

void CommServer::sendStreamData(size_t size) {
    size++; // add the ID byte
    replyBuffer[size] = calculateChecksum(replyBuffer, size);
    size++; // add the checksum byte
    uart->writeData(replyBuffer, size);
}
