/*
 * drive.cpp
 *
 *  Created on: Mar 5, 2025
 *      Author: dkiovics
 */

#include "drive.h"
#include <algorithm>
#include <cmath>
#include "instance/robotInstance.h"

void Drive::init() {
    setDriveSpeed(lin::Vector(0));
    driveMatrix = lin::Matrix(lin::Vector(0.33f, -0.58f, -0.33f), lin::Vector(0.33f, 0.58f, -0.33f),
                              lin::Vector(-0.67f, 0, -0.33f));

    driveMatrixInv = lin::Matrix(lin::Vector(0.5f, 0.5f, -1.0f), lin::Vector(-0.8621f, 0.8621f, 0),
                                 lin::Vector(-1.015, -1.015, -1.0f));
}

void Drive::enableDrive(bool enable) {
    driveEnabled = enable;
    if (!driveEnabled) {
        setDriveSpeed(lin::Vector(0));
    }
}

void Drive::setDriveSpeed(lin::Vector speed) {
    if (!driveEnabled) {
        return;
    }

    // float m1 = +0.33f * speed.x - 0.58f * speed.y - 0.33f * speed.z;
    // float m2 = +0.33f * speed.x + 0.58f * speed.y - 0.33f * speed.z;
    // float m3 = -0.67f * speed.x - 0.33f * speed.z;

    lin::Vector m = speed * driveMatrix;

    float maxAbs = std::max(std::max(std::abs(m.x), std::abs(m.y)), std::abs(m.z));
    if (maxAbs > 1) {
        m = m / maxAbs;
    }

    robotInstance.mot1.setSpeed(m.x * 100);
    robotInstance.mot2.setSpeed(m.y * 100);
    robotInstance.mot3.setSpeed(m.z * 100);
}

lin::Vector Drive::getActualDriveSpeed() {
    lin::Vector motorSpeed(robotInstance.enc1.getSpeed(), robotInstance.enc2.getSpeed(), robotInstance.enc3.getSpeed());

    return motorSpeed * driveMatrixInv * 0.01f;
}
