/*
 * lcd.cpp
 *
 *  Created on: Apr 11, 2023
 *      Author: dkiovics
 */

#include "lcd.h"
#include <stdlib.h>

void Lcd::init(I2C_HandleTypeDef* i2c, IRQn_Type i2cIr, uint8_t address, uint16_t cmdBufSize) {
    this->i2c = i2c;
    this->address = address;
    this->cmdBufSize = cmdBufSize / sizeof(uint32_t);
    this->cmdBufReadPtr = 0;
    this->cmdBufWritePtr = 0;
    this->cmdBuf = (uint32_t*) malloc(cmdBufSize / sizeof(uint32_t) * sizeof(uint32_t));
    this->i2cIr = i2cIr;
    this->backlight = 0;
    this->transmissionInProgress = false;
    this->periodCounter = periodCount;
    this->enablePeriodHandle = true;
    this->ok = true;

    addCommand(0x33);
    HAL_Delay(5);

    addCommand(0x32);
    HAL_Delay(5);

    addCommand(0x28);
    HAL_Delay(5);

    addCommand(0x0C);
    HAL_Delay(5);

    addCommand(0x06);
    HAL_Delay(5);

    addCommand(0x01);
    HAL_Delay(5);
}

void Lcd::clear() {
    addCommand(0x01);
}

void Lcd::home() {
    addCommand(0x02);
}

void Lcd::setCursor(uint8_t row, uint8_t col) {
    uint8_t row_offsets[] = { 0x00, 0x40 };
    addCommand(0x80 | (col + row_offsets[row]));
}

void Lcd::print(const char* text) {
    while (*text != '\0') {
        addData(*text++);
    }
}

void Lcd::printf(int row, int col, const char* fmt, ...) {
    char buffer[100];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    setCursor(row, col);
    print(buffer);
}

void Lcd::addCommand(uint8_t cmd) {
    uint32_t tmp;
    uint8_t data_u, data_l;
    data_u = (cmd & 0xf0);
    data_l = ((cmd << 4) & 0xf0);
    tmp = data_u | 0x04 | (backlight << 3);          // en=1, rs=0
    tmp |= (data_u | 0x00 | (backlight << 3)) << 8;  // en=0, rs=0
    tmp |= (data_l | 0x04 | (backlight << 3)) << 16; // en=1, rs=0
    tmp |= (data_l | 0x00 | (backlight << 3)) << 24; // en=0, rs=0
    send(tmp);
}

void Lcd::addData(uint8_t data) {
    uint32_t tmp;
    uint8_t data_u, data_l;
    data_u = (data & 0xf0);
    data_l = ((data << 4) & 0xf0);
    tmp = data_u | 0x05 | (backlight << 3);          // en=1, rs=1
    tmp |= (data_u | 0x01 | (backlight << 3)) << 8;  // en=0, rs=1
    tmp |= (data_l | 0x05 | (backlight << 3)) << 16; // en=1, rs=1
    tmp |= (data_l | 0x01 | (backlight << 3)) << 24; // en=0, rs=1
    send(tmp);
}

void Lcd::send(uint32_t data) {
    cmdBuf[cmdBufWritePtr] = data;
    HAL_NVIC_DisableIRQ(i2cIr);
    enablePeriodHandle = false;
    if (cmdBufWritePtr == cmdBufReadPtr && !transmissionInProgress) {
        cmdBufWritePtr++;
        if (cmdBufWritePtr == cmdBufSize)
            cmdBufWritePtr = 0;
        handleTransmitCplt(i2c);
        transmissionInProgress = true;
    } else {
        cmdBufWritePtr++;
        if (cmdBufWritePtr == cmdBufSize)
            cmdBufWritePtr = 0;
    }
    enablePeriodHandle = true;
    HAL_NVIC_EnableIRQ(i2cIr);
}

void Lcd::handleTransmitCplt(I2C_HandleTypeDef* i2c) {
    if (this->i2c != i2c)
        return;
    if (cmdBufWritePtr == cmdBufReadPtr) {
        transmissionInProgress = false;
        return;
    }
    if (periodCounter < periodCount)
        return;
    HAL_I2C_Master_Transmit_IT(i2c, address, (uint8_t*) (&cmdBuf[cmdBufReadPtr]), 4);
    if (cmdBuf[cmdBufReadPtr] & 0x01) {
        cmdBufReadPtr++;
        if (cmdBufReadPtr == cmdBufSize)
            cmdBufReadPtr = 0;
    } else {
        periodCounter = 0;
    }
}

void Lcd::enableBacklight(bool enable) {
    backlight = enable;
    home();
}

void Lcd::handlePeriodElapsed() {
    if (periodCounter == periodCount || !enablePeriodHandle || !ok)
        return;
    periodCounter++;
    if (periodCounter == periodCount) {
        cmdBufReadPtr++;
        if (cmdBufReadPtr == cmdBufSize)
            cmdBufReadPtr = 0;
        handleTransmitCplt(i2c);
    }
}
