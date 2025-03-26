/*
 * control.cpp
 *
 *  Created on: Mar 5, 2025
 *      Author: dkiovics
 */

#include <cstdint>
#include "comm/commServer.h"
#include "comm/replyHandlers.h"
#include "comm/streamers.h"
#include "drive/drive.h"
#include "instance/config.h"
#include "instance/robotInstance.h"
#include "main.h"
#include "stm32f4xx_hal.h"

RobotInstance robotInstance;
static Drive drive;

static comm::CommServer commServer;

static comm::MotorSpeedRequestReply motorSpeedRR;

static comm::DriveSpeedStream driveSpeedStream;
static comm::StatusStream statusStream;

static void initRobot() {
    HAL_TIM_Base_Start_IT(PERIOD_TIM);

    robotInstance.jetsonUart.init(SERIAL_UART, SERIAL_UART_IR, 42, 69, 123);

    robotInstance.enc1.init(ENC1_TIM, ENC1_CHANNEL, ENC1_ACTIVE_CHANNEL, 65536, 22500000, ENC1_A_GPIO_Port, ENC1_A_Pin,
                            ENC1_B_GPIO_Port, ENC1_B_Pin, 2500, false);
    robotInstance.enc2.init(ENC2_TIM, ENC2_CHANNEL, ENC2_ACTIVE_CHANNEL, 65536, 22500000, ENC2_A_GPIO_Port, ENC2_A_Pin,
                            ENC2_B_GPIO_Port, ENC2_B_Pin, 2500, false);
    robotInstance.enc3.init(ENC3_TIM, ENC3_CHANNEL, ENC3_ACTIVE_CHANNEL, 65536, 22500000, ENC3_A_GPIO_Port, ENC3_A_Pin,
                            ENC3_B_GPIO_Port, ENC3_B_Pin, 2500, true);

    robotInstance.mot1.init(MOT1_TIM, MOT1_EN_CH, true, 1024, 1000, 18000000, MOT1_DIR_GPIO_Port, MOT1_DIR_Pin, false,
                            true, &robotInstance.enc1, 1.5f, 40.0f, 0.0f);
    robotInstance.mot3.init(MOT23_TIM, MOT3_EN_CH, false, 1024, 1000, 18000000, MOT3_DIR_GPIO_Port, MOT3_DIR_Pin, false,
                            true, &robotInstance.enc3, 1.5f, 40.0f, 0.0f);
    robotInstance.mot2.init(MOT23_TIM, MOT2_EN_CH, false, 1024, 1000, 18000000, MOT2_DIR_GPIO_Port, MOT2_DIR_Pin, false,
                            true, &robotInstance.enc2, 1.5f, 40.0f, 0.0f);

    robotInstance.vbat.init(BAT_ADC, 205);

    robotInstance.lcd.init(LCD_I2C, LCD_I2C_IR, 0x4e, 200);
    robotInstance.lcd.printf(0, 0, "Init complete");
    robotInstance.lcd.enableBacklight(true);

    robotInstance.initCplt = true;
}

static void initSystems() {
    drive.init();

    commServer.init(&robotInstance.jetsonUart, nullptr, 0);

    motorSpeedRR.init(&drive);

    driveSpeedStream.init(&commServer, &drive, 10);
    statusStream.init(&commServer, 500);
}

extern "C" {

void startControl() {
    initRobot();
    initSystems();

    drive.enableDrive(true);

    comm::RequestReply* rrTypes[] = { &motorSpeedRR };
    comm::IStream* streams[] = { &driveSpeedStream, &statusStream };
    commServer.setRequestReplyTypes(rrTypes, 2);

    statusStream.enable(true);
    driveSpeedStream.enable(true);

    while (true) {
        commServer.processRequests();
        for (auto stream : streams) {
            stream->update();
        }
    }
}
}
