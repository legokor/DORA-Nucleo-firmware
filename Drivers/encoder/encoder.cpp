/*
 * encoder.cpp
 *
 *  Created on: Mar 20, 2023
 *      Author: dkiovics
 */

#include "encoder.h"


void Encoder::init(TIM_HandleTypeDef* encoderTimer, uint32_t captureChannelA, HAL_TIM_ActiveChannel activeChannelA, uint32_t timerPeriod, uint32_t timerFrequency,
			GPIO_TypeDef* portA, uint16_t pinA, GPIO_TypeDef* portB, uint16_t pinB, uint16_t maxCountsPerSecond, bool reversed){
	this->encoderTimer = encoderTimer;
	this->captureChannelA = captureChannelA;
	this->activeChannelA = activeChannelA;
	this->timerPeriod = timerPeriod;
	this->timerFrequency = timerFrequency;
	this->portA = portA;
	this->pinA = pinA;
	this->portB = portB;
	this->pinB = pinB;
	this->maxCountsPerSecond = maxCountsPerSecond;
	this->reversed = reversed;
	this->periodDivCps = (float)timerFrequency / maxCountsPerSecond * 100;

	this->absolutePosition = 0;
	this->lastCaptureTimerValue = 0;
	this->maxOverflowCount = (float)timerFrequency / timerPeriod * 0.015f + 1; 	//0.015s needs to pass without encoder reading to declare the motor as stopped
	this->stopped = true;
	this->firstReading = true;
	this->lastInterval = 0;
	this->overflowCounter = 0;

	HAL_TIM_Base_Start_IT(encoderTimer);
	HAL_TIM_IC_Start_IT(encoderTimer, captureChannelA);

	this->ok = true;
}

void Encoder::handleTimerOverflow(TIM_HandleTypeDef* htim){
	if(htim != encoderTimer || !ok)
		return;
	overflowCounter++;
	if(overflowCounter > maxOverflowCount){
		overflowCounter = 0;
		stopped = true;
		firstReading = true;
	}
}

void Encoder::handleInputCapture(TIM_HandleTypeDef* htim){
	if(htim != encoderTimer || htim->Channel != activeChannelA || !ok)
		return;

	uint32_t timerValue = HAL_TIM_ReadCapturedValue(htim, captureChannelA);
	uint32_t overflowCount = overflowCounter;
	overflowCounter = 0;

	GPIO_PinState A = HAL_GPIO_ReadPin(portA, pinA);
	GPIO_PinState B = HAL_GPIO_ReadPin(portB, pinB);
	int32_t dir;
	if(A != B){
		if(reversed){
			absolutePosition--;
			dir = -1;
		}else{
			absolutePosition++;
			dir = +1;
		}
	}else{
		if(reversed){
			absolutePosition++;
			dir = +1;
		}else{
			absolutePosition--;
			dir = -1;
		}
	}

	if(firstReading){
		firstReading = false;
		lastCaptureTimerValue = timerValue;
	}else{
		stopped = false;
		uint32_t elapsed = overflowCount * timerPeriod + timerValue - lastCaptureTimerValue;
		lastInterval = elapsed * dir;
		lastCaptureTimerValue = timerValue;
	}
}

uint32_t Encoder::getPosition(){
	return absolutePosition;
}

float Encoder::getSpeed(){
	if(stopped)
		return 0;
	return periodDivCps / lastInterval;
}



