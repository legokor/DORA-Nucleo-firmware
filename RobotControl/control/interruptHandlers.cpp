/*
 * interruptHandlers.cpp
 *
 *  Created on: Mar 26, 2025
 *      Author: dkiovics
 */

#include <cstdint>
#include "instance/config.h"
#include "instance/robotInstance.h"
#include "main.h"

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart) {
    robotInstance.jetsonUart.txCpltInterruptCb(huart);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    robotInstance.enc1.handleTimerOverflow(htim);
    robotInstance.enc2.handleTimerOverflow(htim);
    robotInstance.enc3.handleTimerOverflow(htim);

    robotInstance.mot1.handleTimerOverflow(htim);
    robotInstance.mot2.handleTimerOverflow(htim);
    robotInstance.mot3.handleTimerOverflow(htim);

    if (htim == PERIOD_TIM) {
        robotInstance.lcd.handlePeriodElapsed();
        robotInstance.vbat.handlePeriodElapse();
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim) {
    robotInstance.enc1.handleInputCapture(htim);
    robotInstance.enc2.handleInputCapture(htim);
    robotInstance.enc3.handleInputCapture(htim);
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef* i2c) {
    robotInstance.lcd.handleTransmitCplt(i2c);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    robotInstance.vbat.handleConversionCplt(hadc);
}
