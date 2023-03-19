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
#include <iostream>


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


void controlMotor(float x, float y, float w){
	float m1 = -0.33f * x + 0.58f * y + 0.33f * w;
	float m2 = -0.33f * x - 0.58f * y + 0.33f * w;
	float m3 = 0.67f * x + 0.33f * w;

	float mult = 1 / std::max(std::max(m1, m2), m3);

	m1 *= mult;
	m2 *= mult;
	m3 *= mult;

	motor1.setSpeed(m1 * 1023);
	motor2.setSpeed(m2 * 1023);
	motor3.setSpeed(m3 * 1023);
}


extern "C"{

void myMain(){
	console.init(SERIAL, 1000, 1000);
	motor1.init(MOT1_TIM, MOT1_EN_CH, 1024, MOT1_DIR_GPIO_Port, MOT1_DIR_Pin, false);
	motor3.init(MOT23_TIM, MOT3_EN_CH, 1024, MOT3_DIR_GPIO_Port, MOT3_DIR_Pin, false);
	motor2.init(MOT23_TIM, MOT2_EN_CH, 1024, MOT2_DIR_GPIO_Port, MOT2_DIR_Pin, false);

	console.transmit("Na csaaaaa\n\r");


	float vx = 0, vy = 0, w = 0;

	while(1){
		char tmp[1001];

		bool ok = console.receive(tmp);

		if(ok){
			char cmd1[50];
			char* cmd2 = strchr(tmp, ' ');
			strncpy(cmd1, tmp, (size_t)(cmd2 - tmp));

			float speed = atof(cmd2);

			if(strcmp(cmd1, "M1")==0){
				console.transmit("M1 power: %f\n", speed);
				motor1.setSpeed(power);
			}
			else if(strcmp(cmd1, "M2")==0){
				console.transmit("M2 power: %f\n", speed);
				motor2.setSpeed(power);
			}
			else if(strcmp(cmd1, "M3")==0){
				console.transmit("M3 power: %f\n", speed);
				motor3.setSpeed(power);
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

