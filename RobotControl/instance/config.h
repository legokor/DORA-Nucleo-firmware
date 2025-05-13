/*
 * config.h
 *
 *  Created on: Mar 5, 2025
 *      Author: dkiovics
 */

#ifndef CONTROL_CONFIG_H_
#define CONTROL_CONFIG_H_

#include "stm32f4xx_hal.h"

extern ADC_HandleTypeDef hadc1;

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim8;

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart1;

#define BAT_ADC &hadc1

#define IMU_I2C &hi2c1
#define IMU_I2C_IR I2C1_EV_IRQn
#define LCD_I2C &hi2c2
#define LCD_I2C_IR I2C2_EV_IRQn

#define MOT1_TIM &htim1
#define MOT23_TIM &htim2

#define MOT1_EN_CH TIM_CHANNEL_2
#define MOT2_EN_CH TIM_CHANNEL_2
#define MOT3_EN_CH TIM_CHANNEL_4

#define ENC1_TIM &htim3
#define ENC2_TIM &htim5
#define ENC3_TIM &htim8
#define PERIOD_TIM &htim6

#define ENC1_CHANNEL TIM_CHANNEL_1
#define ENC1_ACTIVE_CHANNEL HAL_TIM_ACTIVE_CHANNEL_1
#define ENC2_CHANNEL TIM_CHANNEL_1
#define ENC2_ACTIVE_CHANNEL HAL_TIM_ACTIVE_CHANNEL_1
#define ENC3_CHANNEL TIM_CHANNEL_1
#define ENC3_ACTIVE_CHANNEL HAL_TIM_ACTIVE_CHANNEL_1

#define STLINK_UART_IR USART2_IRQn
#define STLINK_UART &huart2
#define CONTROL_UART_IR USART1_IRQn
#define CONTROL_UART &huart1
#define GPS_UART_IR UART4_IRQn
#define GPS_UART &huart4

#endif /* CONTROL_CONFIG_H_ */
