/*
 * robot.h
 *
 *  Created on: Jan 3, 2025
 *      Author: dkiovics
 */

#ifndef ROBOTINSTANCE_H_
#define ROBOTINSTANCE_H_

#include "encoder.h"
#include "lcd.h"
#include "motor.h"
#include "mpu9250.h"
#include "binaryUart.h"
#include "vbat.h"


struct RobotInstance
{
	Encoder* enc1, enc2, enc3;
	Motor* mot1, mot2, mot3;
	Lcd* lcd;
	Mpu9250* imu;
	comm::IBinaryUart* binUart;
	Vbat* vbat;

	volatile bool initCplt = false;
};

extern RobotInstance robotInstance;


#endif /* ROBOTINSTANCE_H_ */
