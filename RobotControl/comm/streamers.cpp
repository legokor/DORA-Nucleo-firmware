/*
 * streamers.cpp
 *
 *  Created on: Mar 26, 2025
 *      Author: dkiovics
 */

#include "comm/streamers.h"
#include "comm/commServer.h"
#include "drive/drive.h"
#include "instance/robotInstance.h"

using namespace comm;

void IStream::init(uint8_t ID, CommServer* commServer, uint32_t updateInterval) {
    this->ID = ID;
    this->commServer = commServer;
    this->updateInterval = updateInterval;
}

uint8_t IStream::getID() const {
    return ID;
}

void IStream::enable(bool enable) {
    enabled = enable;
}

bool IStream::isUpdateNeeded() {
    if (enabled) {
        uint32_t now = HAL_GetTick();
        if (now - lastUpdate >= updateInterval) {
            lastUpdate = now;
            return true;
        }
    } else {
        lastUpdate = HAL_GetTick();
    }
    return false;
}

void DriveSpeedStream::init(CommServer* commServer, Drive* drive, uint32_t updateInterval) {
    IStream::init(0x02, commServer, updateInterval);
    this->drive = drive;
}

void DriveSpeedStream::update() {
    if (isUpdateNeeded()) {
        lin::Vector speed = drive->getActualDriveSpeed();
        uint8_t* buffer = commServer->getStreamDataWriteBuffer(getID());
        std::memcpy(buffer, &speed, sizeof(lin::Vector));
        commServer->sendStreamData(sizeof(lin::Vector));
    }
}

void StatusStream::init(CommServer* commServer, uint32_t updateInterval) {
    IStream::init(0x03, commServer, updateInterval);
}

void StatusStream::update() {
    if (isUpdateNeeded()) {
        uint8_t* buffer = commServer->getStreamDataWriteBuffer(getID());
        float vbat = robotInstance.vbat.getVoltage();
        std::memcpy(buffer, &vbat, sizeof(float));
        commServer->sendStreamData(sizeof(float));

        robotInstance.lcd.printf(1, 3, "Vbat: %.2f", vbat);
    }
}
