/*
 * robot.h
 *
 *  Created on: Jan 3, 2025
 *      Author: dkiovics
 */

#ifndef INSTANCE_ROBOTINSTANCE_H_
#define INSTANCE_ROBOTINSTANCE_H_

#include "encoder/encoder.h"
#include "lcd/lcd.h"
#include "motor/motor.h"
#include "mpu9250/mpu9250.h"
#include "uart/binaryUart.h"
#include "vbat/vbat.h"

struct RobotInstance {
    Encoder enc1, enc2, enc3;
    Motor mot1, mot2, mot3;
    Lcd lcd;
    // Mpu9250 imu;
    comm::BinaryUartImpl<256, 512, 512> jetsonUart;

    Vbat vbat;

    volatile bool initCplt = false;
};

extern RobotInstance robotInstance;

#endif /* INSTANCE_ROBOTINSTANCE_H_ */
