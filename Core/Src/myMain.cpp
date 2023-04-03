/*
 * main.cpp
 *
 *  Created on: Mar 12, 2023
 *      Author: dkiovics
 */

#include "main.h"

#include "motor.h"
#include "uart.h"
#include "encoder.h"

#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>


Uart console;

Motor motor1;
Motor motor2;
Motor motor3;

Encoder encoder1;
Encoder encoder2;
Encoder encoder3;


extern "C"{

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	console.handleReceiveCplt(huart);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	console.handleTransmitCplt(huart);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	encoder1.handleTimerOverflow(htim);
	encoder2.handleTimerOverflow(htim);
	encoder3.handleTimerOverflow(htim);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	encoder1.handleInputCapture(htim);
	encoder2.handleInputCapture(htim);
	encoder3.handleInputCapture(htim);
}

}


void controlMotor(float x, float y, float w) {
	y *= -1;
	x *= -1;
	w *= -1;
	float m1 = -0.33f * x + 0.58f * y + 0.33f * w;
	float m2 = -0.33f * x - 0.58f * y + 0.33f * w;
	float m3 = 0.67f * x + 0.33f * w;

	float max = std::max(std::max(std::fabs(m1), std::fabs(m2)), std::fabs(m3));
	if(max > 0.1){
		float mult = 1 / max;
		m1 *= mult;
		m2 *= mult;
		m3 *= mult;
	}

	motor1.setSpeed(m1 * 1024);
	motor2.setSpeed(m2 * 1024);
	motor3.setSpeed(m3 * 1024);
}


extern "C"{

void myMain(){
	console.init(SERIAL, 1000, 1000);
	motor1.init(MOT1_TIM, MOT1_EN_CH, 1024, MOT1_DIR_GPIO_Port, MOT1_DIR_Pin, false);
	motor3.init(MOT23_TIM, MOT3_EN_CH, 1024, MOT3_DIR_GPIO_Port, MOT3_DIR_Pin, false);
	motor2.init(MOT23_TIM, MOT2_EN_CH, 1024, MOT2_DIR_GPIO_Port, MOT2_DIR_Pin, false);

	encoder1.init(ENC1_TIM, ENC1_CHANNEL, ENC1_ACTIVE_CHANNEL, 65536, 22500000, ENC1_A_GPIO_Port, ENC1_A_Pin, ENC1_B_GPIO_Port, ENC1_B_Pin, 3360, false);
	encoder2.init(ENC2_TIM, ENC2_CHANNEL, ENC2_ACTIVE_CHANNEL, 65536, 22500000, ENC2_A_GPIO_Port, ENC2_A_Pin, ENC2_B_GPIO_Port, ENC2_B_Pin, 3360, false);
	encoder3.init(ENC3_TIM, ENC3_CHANNEL, ENC3_ACTIVE_CHANNEL, 65536, 22500000, ENC3_A_GPIO_Port, ENC3_A_Pin, ENC3_B_GPIO_Port, ENC3_B_Pin, 3360, false);

	console.transmit("Na csaaaaa\n");


	float vx = 0, vy = 0, w = 0;

	while(1){
		HAL_Delay(20);

		console.transmit("%.2f %.2f %.2f\n", encoder1.getSpeed(), encoder2.getSpeed(), encoder3.getSpeed());
		//console.transmit("%d\n", HAL_GPIO_ReadPin(ENC2_A_GPIO_Port, ENC2_A_Pin)==GPIO_PIN_SET);

		char tmp[1001];

		bool ok = console.receive(tmp);

		if(ok){
			char cmd1[50];
			char* cmd2 = strchr(tmp, ' ');
			strncpy(cmd1, tmp, (size_t)(cmd2 - tmp));
			cmd1[(size_t)(cmd2 - tmp)] = '\0';

			console.transmit("Received\n");
			HAL_Delay(20);

			float speed = atof(cmd2);

			if(strcmp(cmd1, "M1")==0){
				console.transmit("M1 power: %f\n", speed);
				motor1.setSpeed(speed);
			}
			else if(strcmp(cmd1, "M2")==0){
				console.transmit("M2 power: %f\n", speed);
				motor2.setSpeed(speed);
			}
			else if(strcmp(cmd1, "M3")==0){
				console.transmit("M3 power: %f\n", speed);
				motor3.setSpeed(speed);
			}else{
				if(strcmp(cmd1, "N")==0){
					console.transmit("dir: N   speed: %f\n", speed);
					vy = speed;
					vx = 0;
				}
				else if(strcmp(cmd1, "S")==0){
					console.transmit("dir: S   speed: %f\n", speed);
					vy = -speed;
					vx = 0;
				}
				else if(strcmp(cmd1, "E")==0){
					console.transmit("dir: E   speed: %f\n", speed);
					vy = 0;
					vx = speed;
				}
				else if(strcmp(cmd1, "W")==0){
					console.transmit("dir: W   speed: %f\n", speed);
					vy = 0;
					vx = -speed;
				}
				else if(strcmp(cmd1, "NE")==0){
					console.transmit("dir: NE   speed: %f\n", speed);
					vy = speed;
					vx = speed;
				}
				else if(strcmp(cmd1, "NW")==0){
					console.transmit("dir: NW   speed: %f\n", speed);
					vy = speed;
					vx = -speed;
				}
				else if(strcmp(cmd1, "SE")==0){
					console.transmit("dir: SE   speed: %f\n", speed);
					vy = -speed;
					vx = speed;
				}
				else if(strcmp(cmd1, "SW")==0){
					console.transmit("dir: SW   speed: %f\n", speed);
					vy = -speed;
					vx = -speed;
				}
				else if(strcmp(cmd1, "RL")==0){
					console.transmit("dir: RL   speed: %f\n", speed);
					w = speed;
				}
				else if(strcmp(cmd1, "RR")==0){
					console.transmit("dir: RR   speed: %f\n", speed);
					w = -speed;
				}
				else if(strcmp(cmd1, "STOP")==0){
					console.transmit("stopped\n");
					vx = 0;
					vy = 0;
					w = 0;
				}
				controlMotor(vx, vy, w);
			}
		}


//		if(ok){
//
//			if(tmp[0] == 'f'){
//			  int num = atoi(strchr(tmp, ' ') + 1);
//			  char* pwmC = strchr(strchr(tmp, ' ') + 1, ' ') + 1;
//			  int pwm = atoi(pwmC);
//
//			  console.transmit("PWM: %d   MOTOR: %d\n\r", pwm, num);
//
//			  switch(num){
//			  case 1: motor1.setSpeed(pwm); break;
//			  case 2: motor2.setSpeed(pwm); break;
//			  case 3: motor3.setSpeed(pwm); break;
//			  }
//		  }
//
//
//		  if(tmp[0] == 's'){
//			  console.transmit("STOP\n\r");
//			  motor1.setSpeed(0);
//			  motor2.setSpeed(0);
//			  motor3.setSpeed(0);
//		  }
//
//		HAL_Delay(10);
//	}
	}
}


}

