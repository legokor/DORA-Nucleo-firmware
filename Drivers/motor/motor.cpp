/*
 * motor.cpp
 *
 *  Created on: Mar 13, 2023
 *      Author: dkiovics
 */

#include "motor.h"
#include <math.h>


void Motor::init(TIM_HandleTypeDef* pwmTimer, uint32_t pwmChannel, bool chN, uint16_t timerPeriod, uint16_t maxPWM, uint32_t timerFrequency, GPIO_TypeDef* dirPort, uint16_t dirPin,
		bool reversed, bool speedControlEnabled, Encoder* encoder, float P, float I, float D){
	this->pwmTimer = pwmTimer;
	this->pwmChannel = pwmChannel;
	this->timerPeriod = timerPeriod;
	this->reversed = reversed;
	this->dirPort = dirPort;
	this->dirPin = dirPin;
	this->P = P;
	this->I = I;
	this->D = D;
	this->speedControlEnabled = speedControlEnabled;
	this->encoder = encoder;
	this->targetSpeed = 0;
	this->errorIntegral = 0;
	this->lastError = 0;
	this->dt = 1.0f * 100 / (timerFrequency / timerPeriod);
	this->controlCounter = 0;
	this->maxPWM = maxPWM;
	this->chN = chN;

	if(speedControlEnabled)
		HAL_TIM_Base_Start_IT(pwmTimer);

	if(chN)
		HAL_TIMEx_PWMN_Start(pwmTimer, pwmChannel);
	else
		HAL_TIM_PWM_Start(pwmTimer, pwmChannel);

	this->ok = true;
}

void Motor::setPowerPWM(int power){

	HAL_GPIO_WritePin(dirPort, dirPin, (power > 0) xor reversed ? GPIO_PIN_SET : GPIO_PIN_RESET);

	uint32_t setValue = std::abs(power);

	if(setValue > timerPeriod - 1)
		setValue = timerPeriod - 1;
	if(setValue > maxPWM)
		setValue = maxPWM;

	switch (pwmChannel) {
		case TIM_CHANNEL_1 : pwmTimer->Instance->CCR1 = setValue; break;
		case TIM_CHANNEL_2 : pwmTimer->Instance->CCR2 = setValue; break;
		case TIM_CHANNEL_3 : pwmTimer->Instance->CCR3 = setValue; break;
		case TIM_CHANNEL_4 : pwmTimer->Instance->CCR4 = setValue; break;
	}
}

void Motor::setSpeed(float speed){
	if(speed > 100)
		speed = 100;
	if(speed < -100)
		speed = -100;
	if(speedControlEnabled){
		targetSpeed = speed;
	}else{
		setPowerPWM((timerPeriod - 1) / 100.0f * speed);
	}
}

void Motor::handleTimerOverflow(TIM_HandleTypeDef* htim){
	if(htim != pwmTimer || !speedControlEnabled || !ok)
		return;
	controlCounter++;
	if(controlCounter < 100)
		return;
	controlCounter = 0;
	float speed = encoder->getSpeed();
	float error = targetSpeed - speed;

	errorIntegral += error * dt * I;
	if(errorIntegral > 100)
		errorIntegral = 100;
	if(errorIntegral < -100)
		errorIntegral = -100;

	float power = error * P + errorIntegral + (error - lastError) / dt * D;
	lastError = error;
	setPowerPWM(power * 0.01f * timerPeriod);
}


