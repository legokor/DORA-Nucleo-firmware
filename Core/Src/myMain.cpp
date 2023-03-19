/*
 * main.cpp
 *
 *  Created on: Mar 12, 2023
 *      Author: dkiovics
 */

#include "main.h"

#include "motor.h"
#include "uart.h"

#include <string.h>
#include <stdlib.h>


Uart console;

Motor motor1;
Motor motor2;
Motor motor3;


extern "C"{

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	console.handleReceiveCplt(huart);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	console.handleTransmitCplt(huart);
}

}


extern "C"{

void myMain(){
	console.init(SERIAL, 1000, 1000);
	motor1.init(MOT1_TIM, MOT1_EN_CH, 1024, MOT1_DIR_GPIO_Port, MOT1_DIR_Pin, false);
	motor3.init(MOT23_TIM, MOT3_EN_CH, 1024, MOT3_DIR_GPIO_Port, MOT3_DIR_Pin, false);
	motor2.init(MOT23_TIM, MOT2_EN_CH, 1024, MOT2_DIR_GPIO_Port, MOT2_DIR_Pin, false);

	console.transmit("Na csaaaaa\n\r");

	while(1){
		char tmp[1001];

		bool ok = console.receive(tmp);

		if(ok){

			if(tmp[0] == 'f'){
			  int num = atoi(strchr(tmp, ' ') + 1);
			  char* pwmC = strchr(strchr(tmp, ' ') + 1, ' ') + 1;
			  int pwm = atoi(pwmC);

			  console.transmit("PWM: %d   MOTOR: %d\n\r", pwm, num);

			  switch(num){
			  case 1: motor1.setSpeed(pwm); break;
			  case 2: motor2.setSpeed(pwm); break;
			  case 3: motor3.setSpeed(pwm); break;
			  }
		  }


		  if(tmp[0] == 's'){
			  console.transmit("STOP\n\r");
			  motor1.setSpeed(0);
			  motor2.setSpeed(0);
			  motor3.setSpeed(0);
		  }

		HAL_Delay(10);
	}
	}
}


}

