/*
 * replyHandlers.cpp
 *
 *  Created on: Mar 5, 2025
 *      Author: dkiovics
 */

#include "comm/replyHandlers.h"
#include "drive/drive.h"

using namespace comm;

void RequestReply::init(uint8_t ID) {
    this->ID = ID;
}

uint8_t RequestReply::getRequestID() const {
    return ID;
}

void MotorSpeedRequestReply::init(Drive* drive) {
    RequestReply::init(0x01);
    this->drive = drive;
}

bool MotorSpeedRequestReply::process(const uint8_t* request, size_t requestSize, uint8_t* reply, size_t& replySize) {
    if (requestSize != sizeof(lin::Vector))
        return false;

    lin::Vector speed;
    std::memcpy(&speed, request, sizeof(lin::Vector));
    drive->setDriveSpeed(speed);

    replySize = 0;
    return true;
}
