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

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

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
#define ENC3_A_Pin GPIO_PIN_6
#define ENC3_A_GPIO_Port GPIOC
#define ENC3_B_Pin GPIO_PIN_7
#define ENC3_B_GPIO_Port GPIOC
#define MOT1_EN_Pin GPIO_PIN_9
#define MOT1_EN_GPIO_Port GPIOA
#define MOT1_DIR_Pin GPIO_PIN_10
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
