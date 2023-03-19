/*
 * uart.cpp
 *
 *  Created on: Mar 13, 2023
 *      Author: dkiovics
 */

#include "uart.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>



void Uart::init(UART_HandleTypeDef *huart, uint16_t writeBufferLenght, uint16_t readBufferLenght){
	this->huart = huart;
	this->writeBufferLenght = writeBufferLenght;
	this->readBufferLenght = readBufferLenght;
	this->mostRecentNewLinePos = -1;
	this->startOfData = 0;
	this->writePtr = 0;
	this->writePtrOverflow = false;
	this->writeInProgress = false;
	writeBuffer = (char*)malloc(writeBufferLenght);
	readBuffer = (char*)malloc(readBufferLenght);

	HAL_UART_Receive_IT(huart, (uint8_t*)readBuffer, 1);
}


void Uart::handleTransmitCplt(UART_HandleTypeDef *huart){
	if(this->huart == huart)
		writeInProgress = false;
}


bool Uart::transmit(const char *fmt, ...){
	if(writeInProgress)
		return false;

	va_list args;
	va_start(args, fmt);

	int size = vsprintf((char*)writeBuffer, fmt, args);
	if(size<0)
		return false;

	writeInProgress = true;

	HAL_UART_Transmit_IT(huart, (uint8_t*)writeBuffer, size);

	return true;
}


void Uart::handleReceiveCplt(UART_HandleTypeDef *huart){
	if(this->huart != huart)
		return;

	char c = readBuffer[writePtr];

	if(c == '\r'){
		HAL_UART_Receive_IT(huart, (uint8_t*)readBuffer + writePtr, 1);
		return;
	}

	if(readBuffer[writePtr] == '\n'){
		mostRecentNewLinePos = writePtr;
	}

	writePtr++;
	if(writePtr == readBufferLenght){
		writePtrOverflow = true;
		writePtr = 0;
	}

	if(writePtr == mostRecentNewLinePos)
		mostRecentNewLinePos = -1;

	if(writePtr == startOfData){
		startOfData++;
		if(startOfData == readBufferLenght)
			startOfData = 0;
	}

	HAL_UART_Receive_IT(huart, (uint8_t*)readBuffer + writePtr, 1);
}


bool Uart::receive(char* data){
	__disable_irq();
	int32_t newLine = mostRecentNewLinePos;
	uint16_t startOfData = this->startOfData;
	if(newLine == -1){
		__enable_irq();
		return false;
	}
	mostRecentNewLinePos = -1;
	this->startOfData = newLine+1;
	if(this->startOfData == readBufferLenght)
		this->startOfData = 0;
	__enable_irq();

	if(startOfData > newLine){
		uint16_t diff = readBufferLenght - startOfData;
		memcpy(data, (const void*)readBuffer + startOfData, diff);
		memcpy(data + diff, (const void*)readBuffer, newLine + 1);
		data[diff + newLine + 1] = '\0';
	}else{
		memcpy(data, (const void*)readBuffer + startOfData, newLine - startOfData + 1);
		data[newLine - startOfData + 1] = '\0';
	}

	return true;
}


uint16_t Uart::getWriteBufferLenght(){
	return writeBufferLenght;
}


uint16_t Uart::getReadBufferLenght(){
	return readBufferLenght;
}




