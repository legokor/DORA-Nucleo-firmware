/*
 * vbat.cpp
 *
 *  Created on: Aug 6, 2023
 *      Author: dkiovics
 */

#include "vbat.h"


void Vbat::init(ADC_HandleTypeDef* hadc, float stepsPerVolt){
	this->hadc = hadc;
	this->stepsPerVolt = stepsPerVolt;
	initCplt = true;
}

void Vbat::handlePeriodElapse(){
	if(!initCplt)
		return;
	periodCounter++;
	if(periodCounter == periodCount){
		periodCounter = 0;
		HAL_ADC_Start_IT(hadc);
	}
}

void Vbat::handleConversionCplt(ADC_HandleTypeDef* hadc){
	if(!initCplt)
		return;
	voltage = HAL_ADC_GetValue(hadc) * rollingAverage + voltage * (1 - rollingAverage);
}

float Vbat::getVoltage(){
	return voltage / stepsPerVolt;
}
