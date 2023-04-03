/*
 * motor.h
 *
 *  Created on: Mar 12, 2023
 *      Author: dkiovics
 */

#ifndef MOTOR_MOTOR_H_
#define MOTOR_MOTOR_H_

#include "stm32f4xx_hal.h"



/*
 * A class that controls the power for a single motor with PWM
 */
class Motor {
public:
	/*
	 * Initializes the object
	 */
	void init(TIM_HandleTypeDef* pwmTimer, uint32_t pwmChannel, uint16_t timerPeriod, GPIO_TypeDef* dirPort, uint16_t dirPin, bool reversed);

	/*
	 * Sets the power and direction of the motor
	 * power - a value between -(timerPeriod-1) and (timerPeriod-1)
	 */
	void setSpeed(int power);

private:
	TIM_HandleTypeDef* pwmTimer;
	uint32_t pwmChannel;
	uint32_t timerPeriod;
	GPIO_TypeDef* dirPort;
	uint16_t dirPin;
	bool reversed;
};




#endif /* MOTOR_MOTOR_H_ */
