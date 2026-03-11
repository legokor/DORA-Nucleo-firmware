/*
 * replyHandlers.cpp
 *
 *  Created on: Mar 5, 2025
 *      Author: dkiovics
 */

#include "comm/replyHandlers.h"
#include "comm/streamers.h"
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

void StreamSettingsRequestReply::init(IStream** streams, size_t streamCount) {
    RequestReply::init(0x02);
    this->streams = streams;
    this->streamCount = streamCount;
}

bool StreamSettingsRequestReply::process(const uint8_t* request,
                                         size_t requestSize,
                                         uint8_t* reply,
                                         size_t& replySize) {
    if (requestSize != sizeof(uint8_t) + sizeof(uint32_t))
        return false;

    uint8_t streamID = request[0];

    uint32_t updateInterval;
    std::memcpy(&updateInterval, request + sizeof(uint8_t), sizeof(uint32_t));

    for (size_t i = 0; i < streamCount; ++i) {
        if (streams[i]->getID() == streamID) {
            if (updateInterval == 0) {
                streams[i]->enable(false);
                break;
            }
            streams[i]->enable(true);
            streams[i]->setUpdateInterval(updateInterval);
            break;
        }
    }

    replySize = 0;
    return true;
}
