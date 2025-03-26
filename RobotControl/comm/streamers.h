/*
 * streamers.h
 *
 *  Created on: Mar 26, 2025
 *      Author: dkiovics
 */

#ifndef COMM_STREAMERS_H_
#define COMM_STREAMERS_H_

#include <cstdint>

class Drive;

namespace comm {

class CommServer;

class IStream {
private:
    bool enabled = false;
    uint32_t lastUpdate = 0;
    uint32_t updateInterval = 0;

protected:
    uint8_t ID;
    CommServer* commServer;

    bool isUpdateNeeded();
    void init(uint8_t ID, CommServer* commServer, uint32_t updateInterval);

public:
    uint8_t getID() const;
    void enable(bool enable);
    virtual void update() = 0;
};

class DriveSpeedStream : public IStream {
private:
    Drive* drive;

public:
    void init(CommServer* commServer, Drive* drive, uint32_t updateInterval);

    void update() override;
};

class StatusStream : public IStream {
public:
    void init(CommServer* commServer, uint32_t updateInterval);

    void update() override;
};

} // namespace comm

#endif /* COMM_STREAMERS_H_ */
