/*
 * binaryUart.h
 *
 *  Created on: Jan 2, 2025
 *      Author: dkiovics
 */

#ifndef UART_BINARYUART_H_
#define UART_BINARYUART_H_

#include <cstring>
#include "stm32f4xx_hal.h"

namespace comm {

namespace status {
constexpr uint32_t UNKNOWN = 0;
constexpr uint32_t RX_NO_DATA = 1;
constexpr uint32_t RX_DATA_PRESENT = 2;
constexpr uint32_t TX_DATA_OVERFLOW = 4;
constexpr uint32_t RX_ESCAPE_ERROR = 8;
constexpr uint32_t RX_FRAME_RESTART = 16;
constexpr uint32_t RX_FRAME_OVERFLOW = 32;
constexpr uint32_t TX_DATA_SENT = 64;
constexpr uint32_t TX_FRAME_TOO_LARGE = 128;
constexpr uint32_t TX_HAL_ERROR = 256;
} // namespace status

class IBinaryUart {
public:
    virtual uint32_t writeData(const uint8_t* data, uint8_t length) = 0;
    virtual uint32_t processIncomingData(uint8_t* data, size_t& cnt) = 0;
    virtual void txCpltInterruptCb(UART_HandleTypeDef* huart) = 0;
};

template <size_t maxFrameSize, size_t rxFrameBufferSize, size_t txFrameBufferSize>
class BinaryUartImpl : public IBinaryUart {
private:
    UART_HandleTypeDef* huart;
    IRQn_Type txIr;
    uint8_t frameStartFlag;
    uint8_t frameEndFlag;
    uint8_t escapeFlag;

    volatile uint8_t rxBuffer[rxFrameBufferSize];
    volatile uint8_t txBuffer[txFrameBufferSize];

    static constexpr size_t encodingBufferSize = maxFrameSize * 2;
    uint8_t encodingBuffer[encodingBufferSize];

    volatile size_t txFrameStartPos = 0; // the start position of the next transmission
    volatile bool txHasPendingData = false;
    volatile size_t txFrameEndPos = 0; // the end position of the next transmission (exclusive)
    volatile bool txInProgress = false;

    bool rxIsFrameOpened = false;
    bool rxEscapeFlagActive = false;
    size_t rxOpenFrameSize = 0;
    size_t rxNextReadPtr = 0;
    uint8_t rxFrameBuffer[maxFrameSize];

public:
    // The flags must have a greater value than 10
    void init(UART_HandleTypeDef* huart,
              IRQn_Type txIr,
              uint8_t frameStartFlag,
              uint8_t frameEndFlag,
              uint8_t escapeFlag) {
        this->huart = huart;
        this->txIr = txIr;
        this->frameStartFlag = frameStartFlag;
        this->frameEndFlag = frameEndFlag;
        this->escapeFlag = escapeFlag;
        HAL_UART_Receive_DMA(huart, (uint8_t*) rxBuffer, rxFrameBufferSize);
        __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE | UART_IT_ERR | UART_IT_PE);

    }

    uint32_t writeData(const uint8_t* data, uint8_t length) override {
        uint32_t status = status::UNKNOWN;

        size_t encodedSize = 1;
        encodingBuffer[0] = frameStartFlag;
        for (size_t p = 0; p < length; p++) {
            if (encodedSize >= encodingBufferSize - 1)
                return status::TX_FRAME_TOO_LARGE;
            if (data[p] == frameStartFlag) {
                encodingBuffer[encodedSize++] = escapeFlag;
                if (encodedSize >= encodingBufferSize - 1)
                    return status::TX_FRAME_TOO_LARGE;
                encodingBuffer[encodedSize++] = 1;
            } else if (data[p] == escapeFlag) {
                encodingBuffer[encodedSize++] = escapeFlag;
                if (encodedSize >= encodingBufferSize - 1)
                    return status::TX_FRAME_TOO_LARGE;
                encodingBuffer[encodedSize++] = 2;
            } else if (data[p] == frameEndFlag) {
                encodingBuffer[encodedSize++] = escapeFlag;
                if (encodedSize >= encodingBufferSize - 1)
                    return status::TX_FRAME_TOO_LARGE;
                encodingBuffer[encodedSize++] = 3;
            } else {
                encodingBuffer[encodedSize++] = data[p];
            }
        }
        encodingBuffer[encodedSize++] = frameEndFlag;

        size_t spaceUntilEOB = txFrameBufferSize - txFrameEndPos;
        if (spaceUntilEOB >= encodedSize) {
            std::memcpy((void*) (txBuffer + txFrameEndPos), (const void*) encodingBuffer, encodedSize);
            size_t newEndPos = txFrameEndPos + encodedSize;
            if (newEndPos >= txFrameBufferSize)
                newEndPos = 0;

            HAL_NVIC_DisableIRQ(txIr);

            if (txInProgress) {
                if (txHasPendingData) {
                    if (txFrameEndPos <= txFrameStartPos && txFrameStartPos < newEndPos) {
                        txFrameStartPos = newEndPos;
                        status |= status::TX_DATA_OVERFLOW;
                    }
                } else {
                    txFrameStartPos = txFrameEndPos;
                    txHasPendingData = true;
                }
            } else {
                txInProgress = true;
                if (HAL_UART_Transmit_DMA(huart, (uint8_t*) txBuffer + txFrameEndPos, encodedSize) != HAL_OK)
                    status |= status::TX_HAL_ERROR;
            }

            txFrameEndPos = newEndPos;
            HAL_NVIC_EnableIRQ(txIr);
        } else {
            if (spaceUntilEOB > 0)
                std::memcpy((void*) (txBuffer + txFrameEndPos), (const void*) encodingBuffer, spaceUntilEOB);

            std::memcpy((void*) txBuffer, (const void*) (encodingBuffer + spaceUntilEOB), encodedSize - spaceUntilEOB);
            size_t newEndPos = encodedSize - spaceUntilEOB;
            if (newEndPos >= txFrameBufferSize)
                return status::TX_FRAME_TOO_LARGE;

            HAL_NVIC_DisableIRQ(txIr);
            if (txInProgress) {
                if (txHasPendingData) {
                    if (newEndPos > txFrameStartPos || txFrameEndPos <= txFrameStartPos) {
                        txFrameStartPos = newEndPos;
                        status |= status::TX_DATA_OVERFLOW;
                    }
                } else {
                    txHasPendingData = true;
                    txFrameStartPos = txFrameEndPos;
                }
            } else {
                txInProgress = true;
                if (HAL_UART_Transmit_DMA(huart, (uint8_t*) txBuffer + txFrameEndPos, spaceUntilEOB) != HAL_OK)
                    status |= status::TX_HAL_ERROR;
                txHasPendingData = true;
                txFrameStartPos = 0;
            }
            txFrameEndPos = newEndPos;

            HAL_NVIC_EnableIRQ(txIr);
        }

        return status | status::TX_DATA_SENT;
    }

    uint32_t processIncomingData(uint8_t* data, size_t& cnt) override {
        uint32_t status = status::UNKNOWN;

        bool restart = true;
        while (restart) {
            restart = false;
            uint8_t newC;
            if (!rxIsFrameOpened) {
                while (readNext(newC)) {
                    if (newC == frameStartFlag) {
                        rxOpenFrameSize = 0;
                        rxIsFrameOpened = true;
                        rxEscapeFlagActive = false;
                        break;
                    }
                }
            }
            if (rxIsFrameOpened) {
                while (readNext(newC) && !restart) {
                    if (newC == frameStartFlag) {
                        rxOpenFrameSize = 0;
                        status |= status::RX_FRAME_RESTART;
                        rxEscapeFlagActive = false;
                    } else if (newC == frameEndFlag) {
                        if (rxEscapeFlagActive) {
                            status |= status::RX_ESCAPE_ERROR;
                            rxIsFrameOpened = false;
                            restart = true;
                        } else {
                            std::memcpy((void*) data, (const void*) rxFrameBuffer, rxOpenFrameSize);
                            cnt = rxOpenFrameSize;
                            rxIsFrameOpened = false;
                            return status | status::RX_DATA_PRESENT;
                        }
                    } else if (rxOpenFrameSize == maxFrameSize) {
                        rxIsFrameOpened = false;
                        status |= status::RX_FRAME_OVERFLOW;
                        restart = true;
                    } else if (newC == escapeFlag) {
                        if (rxEscapeFlagActive) {
                            rxIsFrameOpened = false;
                            status |= status::RX_ESCAPE_ERROR;
                            restart = true;
                        } else
                            rxEscapeFlagActive = true;
                    } else if (rxEscapeFlagActive) {
                        rxEscapeFlagActive = false;
                        switch (newC) {
                            case 1: rxFrameBuffer[rxOpenFrameSize++] = frameStartFlag; break;
                            case 2: rxFrameBuffer[rxOpenFrameSize++] = escapeFlag; break;
                            case 3: rxFrameBuffer[rxOpenFrameSize++] = frameEndFlag; break;
                            default:
                                rxIsFrameOpened = false;
                                status |= status::RX_ESCAPE_ERROR;
                                restart = true;
                                break;
                        }
                    } else {
                        rxFrameBuffer[rxOpenFrameSize++] = newC;
                    }
                }
            }
        }

        return status | status::RX_NO_DATA;
    }

    void txCpltInterruptCb(UART_HandleTypeDef* huart) override {
        if (huart != this->huart)
            return;
        if (!txHasPendingData) {
            txInProgress = false;
            return;
        }

        if (txFrameStartPos < txFrameEndPos) {
            HAL_UART_Transmit_DMA(huart, (uint8_t*) txBuffer + txFrameStartPos, txFrameEndPos - txFrameStartPos);
            txHasPendingData = false;
        } else {
            HAL_UART_Transmit_DMA(huart, (uint8_t*) txBuffer + txFrameStartPos, txFrameBufferSize - txFrameStartPos);
            txFrameStartPos = 0;
            if (txFrameEndPos == 0)
                txHasPendingData = false;
        }
    }

private:
    bool readNext(uint8_t& data) {
        size_t dmaPtr = rxFrameBufferSize - huart->hdmarx->Instance->NDTR;
        if (rxNextReadPtr != dmaPtr) {
            data = rxBuffer[rxNextReadPtr++];
            if (rxNextReadPtr == rxFrameBufferSize)
                rxNextReadPtr = 0;
            return true;
        }
        return false;
    }
};

} // namespace comm

#endif /* UART_BINARYUART_H_ */
