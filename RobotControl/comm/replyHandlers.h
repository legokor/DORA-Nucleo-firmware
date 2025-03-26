/*
 * replyHandlers.h
 *
 *  Created on: Mar 5, 2025
 *      Author: dkiovics
 */

#ifndef COMM_REPLYHANDLERS_H_
#define COMM_REPLYHANDLERS_H_

#include <cstdint>
#include "comm/commServer.h"

class Drive;

namespace comm {

class RequestReply {
protected:
    uint8_t ID;

    void init(uint8_t ID);

public:
    uint8_t getRequestID() const;
    virtual bool process(const uint8_t* request, size_t requestSize, uint8_t* reply, size_t& replySize) = 0;
};

class MotorSpeedRequestReply : public RequestReply {
public:
    void init(Drive* drive);

    bool process(const uint8_t* request, size_t requestSize, uint8_t* reply, size_t& replySize) override;

private:
    Drive* drive;
};

} // namespace comm

#endif /* COMM_REPLYHANDLERS_H_ */
