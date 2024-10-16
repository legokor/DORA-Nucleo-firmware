/*
 * binaryUart.h
 *
 *  Created on: Oct 9, 2024
 *      Author: dkiovics
 */

#ifndef UART_BINARYUART_H_
#define UART_BINARYUART_H_

#include "stm32f4xx_hal.h"


namespace comm
{

template<size_t maxFrameSize, size_t maxFrameCount, size_t frameBufferSize>
class BinaryUart
{
public:
	BinaryUart(UART_HandleTypeDef* huart, IRQn_Type uartIr, uint8_t frameStartFlag, uint8_t escapeFlag);

	enum CommandStatus
	{
		NO_DATA,
		DATA_PRESENT,
		DATA_OVERFLOW
	};

	CommandStatus writeData(const uint8_t* data);

	CommandStatus getData(uint8_t* data);

	void idleInterruptCb(const UART_HandleTypeDef* huart);
	void flagInterruptCb(const UART_HandleTypeDef* huart);
	void txCpltInterruptCb(const UART_HandleTypeDef* huart);

private:
	UART_HandleTypeDef* const m_huart;
	const IRQn_Type m_uartIr;
	const uint8_t m_frameStartFlag;
	const uint8_t m_escapeFlag;

	volatile uint8_t receiveBuffer[frameBufferSize];
	volatile uint8_t transmitBufferSize[frameBufferSize];

	static constexpr size_t encodingBufferSize = maxFrameSize * 2;
	uint8_t encodingBuffer[encodingBufferSize];

	volatile size_t rxFrameStartPtr[maxFrameCount];
	volatile size_t rxNextFrameIndex = 0;
	volatile size_t rxLastFrameIndex = 0;

	volatile size_t txStartOfData = -1;
	volatile size_t txEndOfData;
	volatile size_t txInProgress = false;
};

}

#endif /* UART_BINARYUART_H_ */
