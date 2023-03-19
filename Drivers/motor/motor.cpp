/*
 * motor.cpp
 *
 *  Created on: Mar 13, 2023
 *      Author: dkiovics
 */

#include "motor.h"
#include <math.h>


void Motor::init(TIM_HandleTypeDef* pwmTimer, uint32_t pwmChannel, uint16_t timerPeriod, GPIO_TypeDef* dirPort, uint16_t dirPin, bool reversed){
	this->pwmTimer = pwmTimer;
	this->pwmChannel = pwmChannel;
	this->timerPeriod = timerPeriod;
	this->reversed = reversed;
	this->dirPort = dirPort;
	this->dirPin = dirPin;

	HAL_TIM_PWM_Start_IT(pwmTimer, pwmChannel);
}

void Motor::setSpeed(int power){
	if(power < -(timerPeriod-1))
		power = -(timerPeriod-1);
	if(power > (timerPeriod-1))
		power = (timerPeriod-1);

	HAL_GPIO_WritePin(dirPort, dirPin, (power > 0) xor reversed ? GPIO_PIN_SET : GPIO_PIN_RESET);

	uint32_t setValue = std::abs(power) * timerPeriod / 255;

	switch (pwmChannel) {
		case TIM_CHANNEL_1 : pwmTimer->Instance->CCR1 = setValue; break;
		case TIM_CHANNEL_2 : pwmTimer->Instance->CCR2 = setValue; break;
		case TIM_CHANNEL_3 : pwmTimer->Instance->CCR3 = setValue; break;
		case TIM_CHANNEL_4 : pwmTimer->Instance->CCR4 = setValue; break;
	}
}


