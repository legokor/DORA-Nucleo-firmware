/*
 * vbat.h
 *
 *  Created on: Aug 6, 2023
 *      Author: dkiovics
 */

#ifndef VBAT_VBAT_H_
#define VBAT_VBAT_H_


#include "stm32f4xx_hal.h"


class Vbat{
public:
	/*
	 * Initializes the Vbat class
	 */
	void init(ADC_HandleTypeDef* hadc, float stepsPerVolt);

	/*
	 * Call periodically (1-10ms)
	 */
	void handlePeriodElapse();

	/*
	 * Call whenever an adc conversion is complete
	 */
	void handleConversionCplt(ADC_HandleTypeDef* hadc);

	/*
	 * Returns the voltage in V
	 */
	float getVoltage();

private:
	ADC_HandleTypeDef* hadc;
	int periodCounter = 0;
	float voltage = 0;
	float stepsPerVolt;
	volatile bool initCplt = false;

	const int periodCount = 10;
	const float rollingAverage = 0.07;
};


#endif /* VBAT_VBAT_H_ */
