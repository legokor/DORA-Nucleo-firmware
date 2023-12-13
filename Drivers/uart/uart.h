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
	void init(UART_HandleTypeDef *huart, IRQn_Type uartIr, uint16_t writeBufferLenght, uint16_t readBufferLenght);

	/*
	 * Call when the HAL_UART_TxCpltCallback function is called
	 */
	void handleTransmitCplt(UART_HandleTypeDef *huart);

	/*
	 * Call when the HAL_UART_RxCpltCallback function is called
	 */
	void handleReceiveCplt(UART_HandleTypeDef *huart);

	/*
	 * Transmits the data (max lenght is writeBufferLenght)
	 */
	void transmit(const char *fmt, ...);

	/*
	 * Receives the data until the last received \n, ignores \r (max lenght is readBufferLenght)
	 * Puts a \0 at the end of the data, returns false, if no data is available
	 */
	bool receive(char* data);

	uint16_t getWriteBufferLenght();

	uint16_t getReadBufferLenght();

private:
	UART_HandleTypeDef* huart;
	IRQn_Type uartIr;

	uint16_t writeBufferLenght;
	uint16_t readBufferLenght;

	volatile char* writeCircularBuffer;
	volatile int32_t startOfWriteData;
	volatile int32_t endOfWriteData;
	volatile char* writeBuffer;
	volatile bool transmissionInProgress;

	volatile char* readCircularBuffer;
	volatile uint16_t startOfReadData;
	volatile uint16_t readPtr;
	volatile bool readPtrOverflow;
	volatile int32_t mostRecentNewLinePos;
	volatile bool ok = false;
};






#endif /* UART_UART_H_ */
