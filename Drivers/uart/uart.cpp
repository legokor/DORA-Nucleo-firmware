/*
 * uart.cpp
 *
 *  Created on: Mar 13, 2023
 *      Author: dkiovics
 */

#include "uart.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Uart::init(UART_HandleTypeDef* huart, IRQn_Type uartIr, uint16_t writeBufferLenght, uint16_t readBufferLenght) {
    this->huart = huart;
    this->writeBufferLenght = writeBufferLenght;
    this->readBufferLenght = readBufferLenght;
    this->mostRecentNewLinePos = -1;
    this->startOfReadData = 0;
    this->readPtr = 0;
    this->readPtrOverflow = false;
    this->uartIr = uartIr;

    this->startOfWriteData = -1;
    this->endOfWriteData = writeBufferLenght - 1;
    this->transmissionInProgress = false;

    writeBuffer = (char*) malloc(writeBufferLenght + 1);
    writeCircularBuffer = (char*) malloc(writeBufferLenght + 1);
    readCircularBuffer = (char*) malloc(readBufferLenght + 1);

    HAL_UART_Receive_IT(huart, (uint8_t*) readCircularBuffer, 1);
    this->ok = true;
}

void Uart::handleTransmitCplt(UART_HandleTypeDef* huart) {
    if (this->huart != huart || !ok)
        return;

    if (startOfWriteData == -1) {
        transmissionInProgress = false;
        return;
    }

    int charCount;
    if (startOfWriteData <= endOfWriteData) {
        charCount = endOfWriteData - startOfWriteData + 1;
        HAL_UART_Transmit_IT(huart, (uint8_t*) writeCircularBuffer + startOfWriteData, charCount);
        startOfWriteData = -1;
    } else {
        charCount = writeBufferLenght - startOfWriteData;
        HAL_UART_Transmit_IT(huart, (uint8_t*) writeCircularBuffer + startOfWriteData, charCount);
        startOfWriteData = 0;
    }
}

void Uart::transmit(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int size = vsprintf((char*) writeBuffer, fmt, args);
    if (size <= 0)
        return;

    int spaceTillBufferEnd = writeBufferLenght - endOfWriteData - 1;

    if (spaceTillBufferEnd >= size) {
        memcpy((void*) writeCircularBuffer + endOfWriteData + 1, (const void*) writeBuffer, size);
        HAL_NVIC_DisableIRQ(uartIr);

        if (startOfWriteData == -1) {
            if (transmissionInProgress) {
                startOfWriteData = endOfWriteData + 1;
            } else {
                HAL_UART_Transmit_IT(huart, (uint8_t*) writeCircularBuffer + endOfWriteData + 1, size);
                transmissionInProgress = true;
            }
        }

        endOfWriteData = endOfWriteData + size;
        HAL_NVIC_EnableIRQ(uartIr);
    } else {
        if (spaceTillBufferEnd > 0)
            memcpy((void*) writeCircularBuffer + endOfWriteData + 1, (const void*) writeBuffer, spaceTillBufferEnd);

        memcpy((void*) writeCircularBuffer, (const void*) writeBuffer + spaceTillBufferEnd, size - spaceTillBufferEnd);
        HAL_NVIC_DisableIRQ(uartIr);

        if (startOfWriteData == -1) {
            if (spaceTillBufferEnd == 0) {
                if (transmissionInProgress) {
                    startOfWriteData = 0;
                } else {
                    transmissionInProgress = true;
                    HAL_UART_Transmit_IT(huart, (uint8_t*) writeCircularBuffer, size);
                }

                endOfWriteData = size - 1;
            } else {
                if (transmissionInProgress) {
                    startOfWriteData = endOfWriteData + 1;
                } else {
                    transmissionInProgress = true;
                    startOfWriteData = 0;
                    HAL_UART_Transmit_IT(huart, (uint8_t*) writeCircularBuffer + endOfWriteData + 1,
                                         spaceTillBufferEnd);
                }

                endOfWriteData = size - spaceTillBufferEnd - 1;
            }
        } else
            endOfWriteData = size - spaceTillBufferEnd - 1;

        HAL_NVIC_EnableIRQ(uartIr);
    }
}

void Uart::handleReceiveCplt(UART_HandleTypeDef* huart) {
    if (this->huart != huart)
        return;

    char c = readCircularBuffer[readPtr];

    if (c == '\r') {
        HAL_UART_Receive_IT(huart, (uint8_t*) readCircularBuffer + readPtr, 1);
        return;
    }

    if (readCircularBuffer[readPtr] == '\n')
        mostRecentNewLinePos = readPtr;

    readPtr++;
    if (readPtr == readBufferLenght) {
        readPtrOverflow = true;
        readPtr = 0;
    }

    if (readPtr == mostRecentNewLinePos)
        mostRecentNewLinePos = -1;

    if (readPtr == startOfReadData) {
        startOfReadData++;
        if (startOfReadData == readBufferLenght)
            startOfReadData = 0;
    }

    HAL_UART_Receive_IT(huart, (uint8_t*) readCircularBuffer + readPtr, 1);
}

bool Uart::receive(char* data) {
    HAL_NVIC_DisableIRQ(uartIr);
    int32_t newLine = mostRecentNewLinePos;
    uint16_t startOfData = this->startOfReadData;

    if (newLine == -1) {
        HAL_NVIC_EnableIRQ(uartIr);
        return false;
    }

    mostRecentNewLinePos = -1;
    this->startOfReadData = newLine + 1;

    if (this->startOfReadData == readBufferLenght)
        this->startOfReadData = 0;

    HAL_NVIC_EnableIRQ(uartIr);

    if (startOfData > newLine) {
        uint16_t diff = readBufferLenght - startOfData;
        memcpy(data, (const void*) readCircularBuffer + startOfData, diff);
        memcpy(data + diff, (const void*) readCircularBuffer, newLine + 1);
        data[diff + newLine + 1] = '\0';
    } else {
        memcpy(data, (const void*) readCircularBuffer + startOfData, newLine - startOfData + 1);
        data[newLine - startOfData + 1] = '\0';
    }

    return true;
}

uint16_t Uart::getWriteBufferLenght() {
    return writeBufferLenght;
}

uint16_t Uart::getReadBufferLenght() {
    return readBufferLenght;
}
