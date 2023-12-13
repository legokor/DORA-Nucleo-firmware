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
#include "lcd.h"
#include "ArduinoJson-v6.21.3.h"
#include "vbat.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <sstream>
#include <string>


Uart console;
Uart control;

Motor motor1;
Motor motor2;
Motor motor3;

Encoder encoder1;
Encoder encoder2;
Encoder encoder3;

Lcd lcd;

Vbat vbat;

StaticJsonDocument<1000> outputDataJsonDoc;

StaticJsonDocument<1000> inputDataJsonDoc;


extern "C"{

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	console.handleReceiveCplt(huart);
	control.handleReceiveCplt(huart);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	console.handleTransmitCplt(huart);
	control.handleTransmitCplt(huart);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	encoder1.handleTimerOverflow(htim);
	encoder2.handleTimerOverflow(htim);
	encoder3.handleTimerOverflow(htim);
	motor1.handleTimerOverflow(htim);
	motor2.handleTimerOverflow(htim);
	motor3.handleTimerOverflow(htim);

	if(htim == PERIOD_TIM){
		lcd.handlePeriodElapsed();
		vbat.handlePeriodElapse();
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	encoder1.handleInputCapture(htim);
	encoder2.handleInputCapture(htim);
	encoder3.handleInputCapture(htim);
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *i2c) {
	lcd.handleTransmitCplt(i2c);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	vbat.handleConversionCplt(hadc);
}


void controlMotor(float x, float y, float w) {
	y *= -1;
	x *= -1;
	w *= -1;
	float m1 = -0.33f * x + 0.58f * y + 0.33f * w;
	float m2 = -0.33f * x - 0.58f * y + 0.33f * w;
	float m3 = 0.67f * x + 0.33f * w;
/*
	float max = std::max(std::max(std::fabs(m1), std::fabs(m2)), std::fabs(m3));
	if(max > 0.1){
		float mult = 1 / max;
		m1 *= mult;
		m2 *= mult;
		m3 *= mult;
	}
*/
	motor1.setSpeed(m1 * 100);
	motor2.setSpeed(m2 * 100);
	motor3.setSpeed(m3 * 100);
}


void processConsole();
void processEsp();

void myMain(){
	HAL_TIM_Base_Start_IT(PERIOD_TIM);

	console.init(SERIAL_UART, SERIAL_UART_IR, 1000, 1000);
	control.init(CONTROL_UART, CONTROL_UART_IR, 1000, 1000);

	encoder1.init(ENC1_TIM, ENC1_CHANNEL, ENC1_ACTIVE_CHANNEL, 65536, 22500000, ENC1_A_GPIO_Port, ENC1_A_Pin, ENC1_B_GPIO_Port, ENC1_B_Pin, 2500, false);
	encoder2.init(ENC2_TIM, ENC2_CHANNEL, ENC2_ACTIVE_CHANNEL, 65536, 22500000, ENC2_A_GPIO_Port, ENC2_A_Pin, ENC2_B_GPIO_Port, ENC2_B_Pin, 2500, false);
	encoder3.init(ENC3_TIM, ENC3_CHANNEL, ENC3_ACTIVE_CHANNEL, 65536, 22500000, ENC3_A_GPIO_Port, ENC3_A_Pin, ENC3_B_GPIO_Port, ENC3_B_Pin, 2500, true);
	motor1.init(MOT1_TIM, MOT1_EN_CH, true, 1024, 1000, 18000000, MOT1_DIR_GPIO_Port, MOT1_DIR_Pin, false, true, &encoder1, 1.5f, 40.0f, 0.0f);
	motor3.init(MOT23_TIM, MOT3_EN_CH, false, 1024, 1000, 18000000, MOT3_DIR_GPIO_Port, MOT3_DIR_Pin, false, true, &encoder3, 1.5f, 40.0f, 0.0f);
	motor2.init(MOT23_TIM, MOT2_EN_CH, false, 1024, 1000, 18000000, MOT2_DIR_GPIO_Port, MOT2_DIR_Pin, false, true, &encoder2, 1.5f, 40.0f, 0.0f);

	lcd.init(LCD_I2C, LCD_I2C_IR, 0x4e, 200);

	vbat.init(BAT_ADC, 205);

	lcd.printf(0, 0, "Na csaaa");
	lcd.enableBacklight(true);

	processEsp();
}


void processEsp(){
	int clearCounter = 0;
	int stop = 0;

	while(1){
		HAL_Delay(20);

		clearCounter++;
		if(clearCounter % 10 == 0){
			lcd.printf(0, 0, "Voltage: %.2fV", vbat.getVoltage());
		}
		if(clearCounter==100){
			lcd.printf(1, 0, "                       ");
		}

		char tmp[1001];
		bool ok = control.receive(tmp);

		if(ok){
			DeserializationError error = deserializeJson(inputDataJsonDoc, tmp);
			if(!error){
				if(inputDataJsonDoc["mode"] == "joystick"){
					float x = inputDataJsonDoc["x"];
					float y = inputDataJsonDoc["y"];
					float w = inputDataJsonDoc["w"];
					float speed = inputDataJsonDoc["speed"];
					controlMotor(speed / 100 * x, speed / 100 * y, speed / 100 * w);
				}else{
					float m1 = inputDataJsonDoc["m1"];
					float m2 = inputDataJsonDoc["m2"];
					float m3 = inputDataJsonDoc["m3"];
					motor1.setSpeed(m1);
					motor2.setSpeed(m2);
					motor3.setSpeed(m3);
				}
				stop = 0;
			}
		}

		stop++;
		if(stop == 50)
			controlMotor(0, 0, 0);
	}
}


void processConsole(){
	int clearCounter = 0;

	while(1){
		HAL_Delay(20);

		clearCounter++;
		if(clearCounter % 10 == 0){
			lcd.printf(0, 0, "Voltage: %.2fV", vbat.getVoltage());
		}
		if(clearCounter==100){
			lcd.printf(1, 0, "                       ");
		}

		outputDataJsonDoc.clear();
		outputDataJsonDoc["vbat"] = vbat.getVoltage();
		JsonObject encoders = outputDataJsonDoc.createNestedObject("encoders");
		encoders["M1"] = encoder1.getSpeed();
		encoders["M2"] = encoder2.getSpeed();
		encoders["M3"] = encoder3.getSpeed();


		std::string s;
		serializeJsonPretty(outputDataJsonDoc, s);

		console.transmit(s.c_str());
		//console.transmit("%d\n", HAL_GPIO_ReadPin(ENC2_A_GPIO_Port, ENC2_A_Pin)==GPIO_PIN_SET);

		char tmp[1001];
		bool ok = console.receive(tmp);

		if(ok){
			clearCounter = 0;
			std::stringstream input;
			input << (const char*)tmp;
			std::vector<std::string> commands;
			std::string s;
			while(input >> s){
				commands.push_back(s);
			}

			float a = -10000;
			float b = -10000;
			float c = -10000;
			if(commands.size()>1){
				if(!(std::stringstream(commands[1]) >> a)){
					a = -10000;
				}
			}
			if(commands.size()>2){
				if(!(std::stringstream(commands[2]) >> b)){
					b = -10000;
				}
			}
			if(commands.size()>3){
				if(!(std::stringstream(commands[3]) >> c)){
					c = -10000;
				}
			}

			if(commands.size()>0){
				if(commands[0] == "M"){
					if(a != -10000 && b != -10000){
						lcd.printf(1, 0, "x:%.2f y:%.2f      ", a, b);
						console.transmit("M x: %.2f y: %.2f\r\n", a, b);
						controlMotor(a, b, 0);
					}else{
						lcd.printf(1, 0, "Inv param         ");
						console.transmit("Invalid param(s)!\r\n");
					}
				}
				else if(commands[0] == "M1"){
					if(a != -10000){
						lcd.printf(1,  0, "M1 s: %.2f        ", a);
						console.transmit("M1 speed: %.2f\r\n", a);
						motor1.setSpeed(a);
					}else{
						lcd.printf(1, 0, "Inv param         ");
						console.transmit("Invalid param!\r\n");
					}
				}
				else if(commands[0] == "M2"){
					if(a != -10000){
						lcd.printf(1,  0, "M2 s: %.2f        ", a);
						console.transmit("M2 speed: %.2f\r\n", a);
						motor2.setSpeed(a);
					}else{
						lcd.printf(1, 0, "Inv param         ");
						console.transmit("Invalid param!\r\n");
					}
				}
				else if(commands[0] == "M3"){
					if(a != -10000){
						lcd.printf(1,  0, "M3 s: %.2f        ", a);
						console.transmit("M3 speed: %.2f\r\n", a);
						motor3.setSpeed(a);
					}else{
						lcd.printf(1, 0, "Inv param         ");
						console.transmit("Invalid param!\r\n");
					}
				}
				else if(commands[0] == "N"){
					if(a != -10000){
						lcd.printf(1,  0, "N s: %.2f        ", a);
						console.transmit("N speed: %.2f\r\n", a);
						controlMotor(0, a, 0);
					}else{
						lcd.printf(1, 0, "Inv param         ");
						console.transmit("Invalid param!\r\n");
					}
				}
				else if(commands[0] == "S"){
					if(a != -10000){
						lcd.printf(1,  0, "S s: %.2f        ", a);
						console.transmit("S speed: %.2f\r\n", a);
						controlMotor(0, -a, 0);
					}else{
						lcd.printf(1, 0, "Inv param         ");
						console.transmit("Invalid param!\r\n");
					}
				}
				else if(commands[0] == "E"){
					if(a != -10000){
						lcd.printf(1,  0, "E s: %.2f        ", a);
						console.transmit("E speed: %.2f\r\n", a);
						controlMotor(a, 0, 0);
					}else{
						lcd.printf(1, 0, "Inv param         ");
						console.transmit("Invalid param!\r\n");
					}
				}
				else if(commands[0] == "W"){
					if(a != -10000){
						lcd.printf(1,  0, "W s: %.2f        ", a);
						console.transmit("W speed: %.2f\r\n", a);
						controlMotor(-a, 0, 0);
					}else{
						lcd.printf(1, 0, "Inv param         ");
						console.transmit("Invalid param!\r\n");
					}
				}
				else if(commands[0] == "NE"){
					if(a != -10000){
						lcd.printf(1,  0, "NE s: %.2f        ", a);
						console.transmit("NE speed: %.2f\r\n", a);
						controlMotor(a, a, 0);
					}else{
						lcd.printf(1, 0, "Inv param         ");
						console.transmit("Invalid param!\r\n");
					}
				}
				else if(commands[0] == "NW"){
					if(a != -10000){
						lcd.printf(1,  0, "NW s: %.2f        ", a);
						console.transmit("NW speed: %.2f\r\n", a);
						controlMotor(-a, a, 0);
					}else{
						lcd.printf(1, 0, "Inv param         ");
						console.transmit("Invalid param!\r\n");
					}
				}
				else if(commands[0] == "SE"){
					if(a != -10000){
						lcd.printf(1,  0, "SE s: %.2f        ", a);
						console.transmit("SE speed: %.2f\r\n", a);
						controlMotor(a, -a, 0);
					}else{
						lcd.printf(1, 0, "Inv param         ");
						console.transmit("Invalid param!\r\n");
					}
				}
				else if(commands[0] == "SW"){
					if(a != -10000){
						lcd.printf(1,  0, "SW s: %.2f        ", a);
						console.transmit("SW speed: %.2f\r\n", a);
						controlMotor(-a, -a, 0);
					}else{
						lcd.printf(1, 0, "Inv param         ");
						console.transmit("Invalid param!\r\n");
					}
				}
				else if(commands[0] == "RL"){
					if(a != -10000){
						lcd.printf(1,  0, "RL s: %.2f        ", a);
						console.transmit("RL speed: %.2f\r\n", a);
						controlMotor(0, 0, a);
					}else{
						lcd.printf(1, 0, "Inv param         ");
						console.transmit("Invalid param!\r\n");
					}
				}
				else if(commands[0] == "RR"){
					if(a != -10000){
						lcd.printf(1,  0, "RR s: %.2f        ", a);
						console.transmit("RR speed: %.2f\r\n", a);
						controlMotor(0, 0, -a);
					}else{
						lcd.printf(1, 0, "Inv param         ");
						console.transmit("Invalid param!\r\n");
					}
				}
				else if(commands[0] == "STOP"){
					lcd.printf(1,  0, "Stopped         ", a);
					console.transmit("Stopped\r\n");
					controlMotor(0, 0, 0);
				}
				else{
					lcd.printf(1, 0, "Inv cmd: %s    ", commands[0].c_str());
					console.transmit("Invalid command: %s\r\n", commands[0].c_str());
				}
			}else{
				lcd.printf(1, 0, "Empty command          ");
				console.transmit("Invalid command: empty\r\n");
			}

			HAL_Delay(20);
		}
	}
}


}

