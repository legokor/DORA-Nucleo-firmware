/*
 * lcd.h
 *
 *  Created on: Apr 3, 2023
 *      Author: dkiovics
 */

#ifndef LCD_LCD_H_
#define LCD_LCD_H_

#include "stdarg.h"
#include "stdio.h"
#include "stm32f4xx_hal.h"

/*
 * Driver for a 16×2 LCD display through I2C
 */
class Lcd {
public:
    /*
     * Initializes the LCD
     */
    void init(I2C_HandleTypeDef* i2c, IRQn_Type i2cIr, uint8_t address, uint16_t cmdBufSize);

    /*
     * Clears the LCD
     */
    void clear();

    /*
     * Returns the cursor to home (0, 0)
     */
    void home();

    /*
     * Sets the cursor to the given position (0 based)
     */
    void setCursor(uint8_t row, uint8_t col);

    /*
     * Basic print from cursor position
     */
    void print(const char* text);

    /*
     * Advanced print with cursor set and printf formatting
     */
    void printf(int row, int col, const char* fmt, ...);

    /*
     * Call when an I2C transmission complete interrupt occurs
     */
    void handleTransmitCplt(I2C_HandleTypeDef* i2c);

    /*
     * Call periodically (every 1-10ms)
     */
    void handlePeriodElapsed();

    /*
     * Enable/disable backlight
     */
    void enableBacklight(bool enable);

private:
    I2C_HandleTypeDef* i2c;
    IRQn_Type i2cIr;
    uint8_t address;

    volatile uint8_t backlight;

    void addCommand(uint8_t cmd);
    void addData(uint8_t data);
    void send(uint32_t data);

    volatile uint16_t cmdBufSize, cmdBufWritePtr, cmdBufReadPtr;
    volatile uint32_t* cmdBuf;
    volatile bool transmissionInProgress;

    volatile bool enablePeriodHandle;
    volatile uint8_t periodCounter;
    volatile bool ok = false;

    const uint8_t periodCount = 12;
};

#endif /* LCD_LCD_H_ */
