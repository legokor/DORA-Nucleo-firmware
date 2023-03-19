/*
 * uart.h
 *
 *  Created on: Mar 13, 2023
 *      Author: dkiovics
 */

#ifndef UART_UART_H_
#define UART_UART_H_

#include "stm32f4xx_hal.h"


class Uart {
public:
	/*
	 * Initializes the UART object
	 */
	void init(UART_HandleTypeDef *huart, uint16_t writeBufferLenght, uint16_t readBufferLenght);

	/*
	 * Call when the HAL_UART_TxCpltCallback function is called
	 */
	void handleTransmitCplt(UART_HandleTypeDef *huart);

	/*
	 * Call when the HAL_UART_RxCpltCallback function is called
	 */
	void handleReceiveCplt(UART_HandleTypeDef *huart);

	/*
	 * Transmits the data until (max lenght is writeBufferLenght)
	 * returns false, if a transmission is already in progress or incorrect format
	 */
	bool transmit(const char *fmt, ...);

	/*
	 * Receives the data until the last received \n, ignores \r (max lenght is readBufferLenght)
	 * Puts a \0 at the end of the data, returns false, if no data is available
	 */
	bool receive(char* data);

	uint16_t getWriteBufferLenght();

	uint16_t getReadBufferLenght();

private:
	UART_HandleTypeDef* huart;

	uint16_t writeBufferLenght;
	uint16_t readBufferLenght;

	volatile char* writeBuffer;

	volatile bool writeInProgress;

	volatile char* readBuffer;
	volatile uint16_t startOfData;
	volatile uint16_t writePtr;
	volatile bool writePtrOverflow;
	volatile int32_t mostRecentNewLinePos;
};






#endif /* UART_UART_H_ */
