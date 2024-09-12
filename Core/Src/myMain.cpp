/*
 * main.cpp
 *
 *  Created on: Mar 12, 2023
 *      Author: dkiovics
 */

#include "main.h"

#include "encoder.h"
#include "lcd.h"
#include "motor.h"
#include "uart.h"
#include "vbat.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Lcd lcd;
Vbat vbat;
Uart uartJetson, uartEsp;
Encoder enc1, enc2, enc3;
Motor mot1, mot2, mot3;

extern "C" {

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
    uartJetson.handleReceiveCplt(huart);
    uartEsp.handleReceiveCplt(huart);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart) {
    uartJetson.handleTransmitCplt(huart);
    uartEsp.handleTransmitCplt(huart);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    enc1.handleTimerOverflow(htim);
    enc2.handleTimerOverflow(htim);
    enc3.handleTimerOverflow(htim);

    mot1.handleTimerOverflow(htim);
    mot2.handleTimerOverflow(htim);
    mot3.handleTimerOverflow(htim);

    if (htim == PERIOD_TIM) {
        lcd.handlePeriodElapsed();
        vbat.handlePeriodElapse();
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim) {
    enc1.handleInputCapture(htim);
    enc2.handleInputCapture(htim);
    enc3.handleInputCapture(htim);
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef* i2c) {
    lcd.handleTransmitCplt(i2c);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    vbat.handleConversionCplt(hadc);
}

/**
 * Compute motor speed values for the given [...] values.
 */
void drive(float x, float y, float w) {
    float m1 = +0.33f * x - 0.58f * y - 0.33f * w;
    float m2 = +0.33f * x + 0.58f * y - 0.33f * w;
    float m3 = -0.67f * x - 0.33f * w;

    mot1.setSpeed(m1 * 100);
    mot2.setSpeed(m2 * 100);
    mot3.setSpeed(m3 * 100);
}

void myMain() {
    HAL_TIM_Base_Start_IT(PERIOD_TIM);

    uartJetson.init(SERIAL_UART, SERIAL_UART_IR, 1000, 1000);
    uartEsp.init(CONTROL_UART, CONTROL_UART_IR, 1000, 1000);

    enc1.init(ENC1_TIM, ENC1_CHANNEL, ENC1_ACTIVE_CHANNEL, 65536, 22500000, ENC1_A_GPIO_Port, ENC1_A_Pin,
              ENC1_B_GPIO_Port, ENC1_B_Pin, 2500, false);
    enc2.init(ENC2_TIM, ENC2_CHANNEL, ENC2_ACTIVE_CHANNEL, 65536, 22500000, ENC2_A_GPIO_Port, ENC2_A_Pin,
              ENC2_B_GPIO_Port, ENC2_B_Pin, 2500, false);
    enc3.init(ENC3_TIM, ENC3_CHANNEL, ENC3_ACTIVE_CHANNEL, 65536, 22500000, ENC3_A_GPIO_Port, ENC3_A_Pin,
              ENC3_B_GPIO_Port, ENC3_B_Pin, 2500, true);

    mot1.init(MOT1_TIM, MOT1_EN_CH, true, 1024, 1000, 18000000, MOT1_DIR_GPIO_Port, MOT1_DIR_Pin, false, true, &enc1,
              1.5f, 40.0f, 0.0f);
    mot3.init(MOT23_TIM, MOT3_EN_CH, false, 1024, 1000, 18000000, MOT3_DIR_GPIO_Port, MOT3_DIR_Pin, false, true, &enc3,
              1.5f, 40.0f, 0.0f);
    mot2.init(MOT23_TIM, MOT2_EN_CH, false, 1024, 1000, 18000000, MOT2_DIR_GPIO_Port, MOT2_DIR_Pin, false, true, &enc2,
              1.5f, 40.0f, 0.0f);

    vbat.init(BAT_ADC, 205);

    lcd.init(LCD_I2C, LCD_I2C_IR, 0x4e, 200);
    lcd.printf(0, 0, "Init complete");
    lcd.enableBacklight(true);

    // TODO: main loop
}
}
