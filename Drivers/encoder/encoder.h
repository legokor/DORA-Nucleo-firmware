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
	void init(TIM_HandleTypeDef* encoderTimer, uint32_t captureChannelA, HAL_TIM_ActiveChannel activeChannelA, uint32_t timerPeriod, uint32_t timerFrequency,
			GPIO_TypeDef* portA, uint16_t pinA, GPIO_TypeDef* portB, uint16_t pinB, uint16_t maxCountsPerSecond, bool reversed);

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
	 * Returns the encoder relative speed scaled to -1 - 1 (from -maxCountsPerSecond - maxCountsPerSecond)
	 */
	float getSpeed();

private:
	TIM_HandleTypeDef* encoderTimer;
	uint32_t captureChannelA;
	HAL_TIM_ActiveChannel activeChannelA;
	uint32_t timerPeriod;
	uint32_t timerFrequency;
	GPIO_TypeDef* portA;
	uint16_t pinA;
	GPIO_TypeDef* portB;
	uint16_t pinB;
	uint16_t maxCountsPerSecond;
	bool reversed;
	float periodDivCps;

	volatile uint32_t absolutePosition;

	uint16_t maxOverflowCount;
	volatile uint16_t overflowCounter;
	volatile uint16_t lastCaptureTimerValue;
	volatile int32_t lastInterval;
	volatile bool stopped;
	volatile bool firstReading;

};


#endif /* ENCODER_ENCODER_H_ */
