/*
 * motor.h
 *
 *  Created on: Mar 12, 2023
 *      Author: dkiovics
 */

#ifndef MOTOR_MOTOR_H_
#define MOTOR_MOTOR_H_

#include "stm32f4xx_hal.h"
#include "encoder.h"



/*
 * A class that controls the power for a single motor with PWM
 */
class Motor {
public:
	/*
	 * Initializes the object
	 */
	void init(TIM_HandleTypeDef* pwmTimer, uint32_t pwmChannel, uint16_t timerPeriod, uint32_t timerFrequency, GPIO_TypeDef* dirPort, uint16_t dirPin,
			bool reversed, bool speedControlEnabled = false, Encoder* encoder = nullptr, float P = 0, float I = 0, float D = 0);

	/*
	 * Call when the motor's timer has overflown (if speed control is enabled)
	 */
	void handleTimerOverflow(TIM_HandleTypeDef* htim);

	/*
	 * Sets the power and direction of the motor (if speed control is disabled)
	 * power - a value between -(timerPeriod-1) and (timerPeriod-1)
	 */
	void setPowerPWM(int power);

	/*
	 * Sets the speed of the motor (if speed control is enabled)
	 * speed - a value between -100 and 100
	 */
	void setSpeed(float speed);

private:
	TIM_HandleTypeDef* pwmTimer;
	uint32_t pwmChannel;
	uint32_t timerPeriod;
	GPIO_TypeDef* dirPort;
	uint16_t dirPin;
	bool reversed;
	bool speedControlEnabled;
	Encoder* encoder;
	float P, I, D;
	float dt;
	volatile float targetSpeed;
	volatile float errorIntegral;
	volatile float lastError;
	volatile int controlCounter;
};




#endif /* MOTOR_MOTOR_H_ */
