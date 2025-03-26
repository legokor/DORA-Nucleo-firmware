/*
 * drive.h
 *
 *  Created on: Mar 5, 2025
 *      Author: dkiovics
 */

#ifndef DRIVE_DRIVE_H_
#define DRIVE_DRIVE_H_

#include <cstdint>
#include "linear/matrix.hpp"
#include "linear/vector.hpp"

class Drive {
private:
    bool driveEnabled = false;
    lin::Matrix driveMatrix;
    lin::Matrix driveMatrixInv;

public:
    void init();

    void enableDrive(bool enable);

    void setDriveSpeed(lin::Vector speed);

    lin::Vector getActualDriveSpeed();
};

#endif /* DRIVE_DRIVE_H_ */
