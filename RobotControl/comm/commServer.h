/*
 * commServer.h
 *
 *  Created on: Jan 3, 2025
 *      Author: dkiovics
 */

#ifndef COMM_COMMSERVER_H_
#define COMM_COMMSERVER_H_

#include <cstdint>
#include "uart/binaryUart.h"

namespace comm {

// The max request payload size
constexpr size_t MAX_REQUEST_SIZE = 256;
// The max reply payload size
constexpr size_t MAX_REPLY_SIZE = 256;

class RequestReply;

class CommServer {
private:
    constexpr static size_t REPLY_BUFFER_SIZE = MAX_REPLY_SIZE + 3;
    constexpr static size_t REQUEST_BUFFER_SIZE = MAX_REQUEST_SIZE + 3;

    IBinaryUart* uart;
    RequestReply** rrTypes;
    size_t rrTypesCnt;

    uint8_t requestBuffer[MAX_REQUEST_SIZE];
    uint8_t replyBuffer[MAX_REPLY_SIZE];

    uint8_t calculateChecksum(const uint8_t* data, size_t size) const;

public:
    void init(IBinaryUart* uart, RequestReply** rrTypes, size_t rrTypesCnt);

    void setRequestReplyTypes(RequestReply** rrTypes, size_t rrTypesCnt);

    void processRequests();

    uint8_t* getStreamDataWriteBuffer(uint8_t streamID);
    void sendStreamData(size_t size);
};

} // namespace comm

#endif /* COMM_COMMSERVER_H_ */
