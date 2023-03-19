/*
 * encoder.h
 *
 *  Created on: Mar 19, 2023
 *      Author: dkiovics
 */

#ifndef ENCODER_ENCODER_H_
#define ENCODER_ENCODER_H_

#include "stm32f4xx_hal.h"


class Encoder {
public:
	/*
	 * Initializes the encoder object
	 */
	void init(TIM_HandleTypeDef* encoderTimer, uint32_t captureChannelA, HAL_TIM_ActiveChannel activeChannelA, uint16_t timerPeriod, uint32_t countsPerSecond,
			GPIO_TypeDef* portA, uint16_t pinA, GPIO_TypeDef* portB, uint16_t pinB, uint16_t maxCountsPerSecond);

	/*
	 * Call whenever a timer overflows
	 */
	void handleTimerOverflow(TIM_HandleTypeDef* htim);

	/*
	 * Call whenever a timer capture event occurs
	 */
	void handleInputCapture(TIM_HandleTypeDef* htim);

	/*
	 * Returns the encoder absolute position
	 */
	uint32_t getPosition();

	/*
	 * Returns the encoder relative speed scaled to -100 - 100 (from -maxCountsPerSecond - maxCountsPerSecond)
	 */
	float getSpeed();

private:
	TIM_HandleTypeDef* encoderTimer;
	uint32_t captureChannelA;
	HAL_TIM_ActiveChannel activeChannelA;
	uint16_t timerPeriod;
	uint32_t countsPerSecond;
	GPIO_TypeDef* portA;
	uint16_t pinA;
	GPIO_TypeDef* portB;
	uint16_t pinB;
	uint16_t maxCountsPerSecond;

	uint32_t absolutePosition;

	uint16_t maxOverflowCount;
	uint16_t overflowCounter;
	uint16_t lastCaptureTimerValue;
	uint32_t lastInterval;
	bool stopped;

};


#endif /* ENCODER_ENCODER_H_ */
