/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

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

#define SERIAL_UART_IR USART2_IRQn
#define SERIAL_UART &huart2
#define CONTROL_UART_IR USART1_IRQn
#define CONTROL_UART &huart1
#define GPS_UART_IR UART4_IRQn
#define GPS_UART &huart4

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

void myMain();

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define BATTERY_Pin GPIO_PIN_0
#define BATTERY_GPIO_Port GPIOC
#define ENC2_A_Pin GPIO_PIN_0
#define ENC2_A_GPIO_Port GPIOA
#define ENC2_B_Pin GPIO_PIN_1
#define ENC2_B_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define ENC1_A_Pin GPIO_PIN_6
#define ENC1_A_GPIO_Port GPIOA
#define MOT3_EN_Pin GPIO_PIN_2
#define MOT3_EN_GPIO_Port GPIOB
#define LCD_SCL_Pin GPIO_PIN_10
#define LCD_SCL_GPIO_Port GPIOB
#define MOT1_EN_Pin GPIO_PIN_14
#define MOT1_EN_GPIO_Port GPIOB
#define ENC3_A_Pin GPIO_PIN_6
#define ENC3_A_GPIO_Port GPIOC
#define ENC3_B_Pin GPIO_PIN_7
#define ENC3_B_GPIO_Port GPIOC
#define MOT1_DIR_Pin GPIO_PIN_8
#define MOT1_DIR_GPIO_Port GPIOA
#define MOT3_DIR_Pin GPIO_PIN_11
#define MOT3_DIR_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define GPS_TX_Pin GPIO_PIN_10
#define GPS_TX_GPIO_Port GPIOC
#define GPS_RX_Pin GPIO_PIN_11
#define GPS_RX_GPIO_Port GPIOC
#define LCD_SDA_Pin GPIO_PIN_12
#define LCD_SDA_GPIO_Port GPIOC
#define GPS_RST_Pin GPIO_PIN_2
#define GPS_RST_GPIO_Port GPIOD
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define ENC1_B_Pin GPIO_PIN_5
#define ENC1_B_GPIO_Port GPIOB
#define IMU_SCL_Pin GPIO_PIN_6
#define IMU_SCL_GPIO_Port GPIOB
#define IMU_SDA_Pin GPIO_PIN_7
#define IMU_SDA_GPIO_Port GPIOB
#define MOT2_DIR_Pin GPIO_PIN_8
#define MOT2_DIR_GPIO_Port GPIOB
#define MOT2_EN_Pin GPIO_PIN_9
#define MOT2_EN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
