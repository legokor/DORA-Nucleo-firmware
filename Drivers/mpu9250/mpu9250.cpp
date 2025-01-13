/*
 * mpu9250.c
 *
 *  Created on: May 20, 2024
 *      Author: dkiovics
 */

#include "mpu9250.h"
#include <string.h>


#define AK8963_ADDRESS   0x0C
#define AK8963_WHO_AM_I  0x00		// should return 0x48
#define AK8963_INFO      0x01
#define AK8963_ST1       0x02
#define AK8963_XOUT_L    0x03
#define AK8963_XOUT_H    0x04
#define AK8963_YOUT_L    0x05
#define AK8963_YOUT_H    0x06
#define AK8963_ZOUT_L    0x07
#define AK8963_ZOUT_H    0x08
#define AK8963_ST2       0x09
#define AK8963_CNTL      0x0A
#define AK8963_ASTC      0x0C
#define AK8963_I2CDIS    0x0F
#define AK8963_ASAX      0x10
#define AK8963_ASAY      0x11
#define AK8963_ASAZ      0x12

#define SELF_TEST_X_GYRO 0x00
#define SELF_TEST_Y_GYRO 0x01
#define SELF_TEST_Z_GYRO 0x02

#define SELF_TEST_X_ACCEL 0x0D
#define SELF_TEST_Y_ACCEL 0x0E
#define SELF_TEST_Z_ACCEL 0x0F

#define SELF_TEST_A      0x10

#define SMPLRT_DIV       0x19
#define CONFIG           0x1A
#define GYRO_CONFIG      0x1B
#define ACCEL_CONFIG     0x1C
#define ACCEL_CONFIG2    0x1D
#define LP_ACCEL_ODR     0x1E
#define WOM_THR          0x1F

#define MOT_DUR          0x20
#define ZMOT_THR         0x21
#define ZRMOT_DUR        0x22

#define FIFO_EN          0x23
#define INT_PIN_CFG      0x37
#define INT_ENABLE       0x38
#define DMP_INT_STATUS   0x39
#define INT_STATUS       0x3A
#define ACCEL_XOUT_H     0x3B
#define ACCEL_XOUT_L     0x3C
#define ACCEL_YOUT_H     0x3D
#define ACCEL_YOUT_L     0x3E
#define ACCEL_ZOUT_H     0x3F
#define ACCEL_ZOUT_L     0x40
#define TEMP_OUT_H       0x41
#define TEMP_OUT_L       0x42
#define GYRO_XOUT_H      0x43
#define GYRO_XOUT_L      0x44
#define GYRO_YOUT_H      0x45
#define GYRO_YOUT_L      0x46
#define GYRO_ZOUT_H      0x47
#define GYRO_ZOUT_L      0x48
#define MOT_DETECT_STATUS 0x61
#define SIGNAL_PATH_RESET  0x68
#define MOT_DETECT_CTRL  0x69
#define USER_CTRL        0x6A
#define PWR_MGMT_1       0x6B
#define PWR_MGMT_2       0x6C
#define DMP_BANK         0x6D
#define DMP_RW_PNT       0x6E
#define DMP_REG          0x6F
#define DMP_REG_1        0x70
#define DMP_REG_2        0x71
#define FIFO_COUNTH      0x72
#define FIFO_COUNTL      0x73
#define FIFO_R_W         0x74
#define WHO_AM_I_MPU9250 0x75 		// Should return 0x71
#define XA_OFFSET_H      0x77
#define XA_OFFSET_L      0x78
#define YA_OFFSET_H      0x7A
#define YA_OFFSET_L      0x7B
#define ZA_OFFSET_H      0x7D
#define ZA_OFFSET_L      0x7E


#ifdef ASYNC_IMU
void Mpu9250::i2cReceiveCpltCallback(I2C_HandleTypeDef* hi2c){
	if(this->hi2c != hi2c)
		return;
	if(isReadingImu){
		memcpy((void*)imuData, (void*)imuBuffer, 14);
		isReadingImu = false;
		if(readEnabled)
			HAL_I2C_Mem_Read_DMA(hi2c, magAddress, AK8963_XOUT_L, 1, (uint8_t*)magBuffer, 7);
		else
			readInProgress = false;
	}else{
		memcpy((void*)magData, (void*)magBuffer, 6);
		readInProgress = false;
		newData = true;
	}
}

void Mpu9250::timPeriodEllapsedCallback(){
	if(!readEnabled || !initComplete)
		return;
	readInProgress = true;
	isReadingImu = true;
	HAL_I2C_Mem_Read_DMA(hi2c, imuAddress, ACCEL_XOUT_H, 1, (uint8_t*)imuBuffer, 14);
}

bool Mpu9250::newDataAvailable(){
	bool tmp = newData;
	if(tmp)
		newData = false;
	return newData;
}
#endif


bool Mpu9250::writeBlocking(uint8_t devAddress, uint8_t regAddress, uint8_t data){
#ifdef ASYNC_IMU
	readEnabled = false;
	while(readInProgress) {}
#endif
	uint8_t txData[] = {regAddress, data};
	bool ok = false;
	while(1) {
		HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hi2c, devAddress, txData, 2, 1000);
		if(status != HAL_BUSY){
			ok = status == HAL_OK;
			break;
		}
	}
#ifdef ASYNC_IMU
	readEnabled = true;
#endif
	return ok;
}

bool Mpu9250::readBlocking(uint8_t devAddress, uint8_t regAddress, uint8_t numBytes, volatile uint8_t* buffer){
#ifdef ASYNC_IMU
	readEnabled = false;
	while(readInProgress) {}
#endif
	bool ok = false;
	while(1) {
		HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, devAddress, regAddress, 1, (uint8_t*)buffer, numBytes, 1000);
		if(status != HAL_BUSY){
			ok = status == HAL_OK;
			break;
		}
	}
#ifdef ASYNC_IMU
	readEnabled = true;
#endif
	return ok;
}


#ifdef ASYNC_IMU
Mpu9250::Mpu9250(I2C_HandleTypeDef* hi2c, uint8_t p_imuAddress, uint8_t p_magAddress, IRQn_Type dmaIr)
		: hi2c(hi2c), imuAddress(p_imuAddress << 1), magAddress(p_magAddress << 1), dmaIr(dmaIr) {
#else
Mpu9250::Mpu9250(I2C_HandleTypeDef* hi2c, uint8_t p_imuAddress, uint8_t p_magAddress)
		: hi2c(hi2c), imuAddress(p_imuAddress << 1), magAddress(p_magAddress << 1) {
#endif
	writeBlocking(imuAddress, PWR_MGMT_1, 0x00);		//RESET, enable all sensors
	HAL_Delay(100);
	writeBlocking(imuAddress, PWR_MGMT_1, 0x01);		//Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
	HAL_Delay(100);
	writeBlocking(imuAddress, INT_PIN_CFG, 0x22);		//Enable bypass to magnetometer
	HAL_Delay(100);

	writeBlocking(magAddress, AK8963_CNTL, 0x00);		//Power down magnetometer
	HAL_Delay(10);
	writeBlocking(magAddress, AK8963_CNTL, 0x0F);		//Enter fuse access mode
	HAL_Delay(10);
	uint8_t temp[3];
	readBlocking(magAddress, AK8963_ASAX, 3, temp);		//Read calibration values
	magCoeff_x = (((float)temp[0] - 128.0) / 256.0) + 1;
	magCoeff_y = (((float)temp[1] - 128.0) / 256.0) + 1;
	magCoeff_z = (((float)temp[2] - 128.0) / 256.0) + 1;
	writeBlocking(magAddress, AK8963_CNTL, 0);			//Power down magnetometer
	HAL_Delay(10);
	writeBlocking(magAddress, AK8963_CNTL, 0x16);		//16 bit resolution, continuous measurement at 100Hz
	HAL_Delay(10);

	bool imuDetected = detectImu();
	bool magDetected = detectMagnetometer();

	initComplete = imuDetected && magDetected;
#ifdef ASYNC_IMU
	readEnabled = initComplete;
#endif
}

bool Mpu9250::detectImu(){
	uint8_t whoAmI;
	readBlocking(imuAddress, WHO_AM_I_MPU9250, 1, &whoAmI);
	return whoAmI == 0x71;
}

bool Mpu9250::detectMagnetometer(){
	uint8_t whoAmI;
	readBlocking(magAddress, AK8963_WHO_AM_I, 1, &whoAmI);
	return whoAmI == 0x48;
}

void Mpu9250::setDefaultSettings(){
	if(!initComplete)
		return;

	setGyroSensitivity(3);				//Set gyro full scale range (+-2000DPS)
	enableGyroAndTempDLPF(true);		//Enable DLPF for the gyro and temp sensors (set fchoice_b's to 0 -> fchoice's to 1)
	setGyroAndTempDLPF(3);				//Set gyro and temp DLPF to 41Hz (results in a 5.9ms delay and a 1kHz sample rate)

	setAccSensitivity(1);				//Set accelerometer sensitivity to +-4g
	enableAccDLPF(true);				//Enable DLPF for accelerometer (set fchoice_b to 0 -> fchoice to 1)
	setAccDLPF(3);						//Set accelerometer DLPF to 44.8Hz (results in a 4.88ms delay and a 1kHz sample rate)

	setSampleRateDivider(4);			//Set the sample rate divider to 4+1=5 (so that the gyro/temp and accelerometer data rate is 200Hz)
}


void Mpu9250::setSampleRateDivider(uint8_t divider){
	if(!initComplete)
		return;

	writeBlocking(imuAddress, SMPLRT_DIV, divider);
}


void Mpu9250::enableAccDLPF(bool enable){
	if(!initComplete)
		return;

	uint8_t accConfigTmp;
	readBlocking(imuAddress, ACCEL_CONFIG2, 1, &accConfigTmp);
	accConfigTmp &= 0xf7;
	if(enable)
		writeBlocking(imuAddress, ACCEL_CONFIG2, accConfigTmp | 0x00);
	else
		writeBlocking(imuAddress, ACCEL_CONFIG2, accConfigTmp | 0x08);
}


void Mpu9250::enableGyroAndTempDLPF(bool enable){
	if(!initComplete)
		return;

	uint8_t gyroConfigTmp;
	readBlocking(imuAddress, GYRO_CONFIG, 1, &gyroConfigTmp);
	gyroConfigTmp &= 0xfc;
	if(enable)
		writeBlocking(imuAddress, GYRO_CONFIG, gyroConfigTmp | 0x00);
	else
		writeBlocking(imuAddress, GYRO_CONFIG, gyroConfigTmp | 0x03);
}


void Mpu9250::setAccDLPF(uint8_t value){
	if(!initComplete)
		return;

	uint8_t accConfigTmp;
	readBlocking(imuAddress, ACCEL_CONFIG2, 1, &accConfigTmp);
	accConfigTmp &= 0xf8;
	writeBlocking(imuAddress, ACCEL_CONFIG2, accConfigTmp | value);
}


void Mpu9250::setGyroAndTempDLPF(uint8_t value){
	if(!initComplete)
		return;

	uint8_t configTmp;
	readBlocking(imuAddress, CONFIG, 1, &configTmp);
	configTmp &= 0xf8;
	writeBlocking(imuAddress, CONFIG, configTmp | value);
}


void Mpu9250::setAccSensitivity(uint8_t sensitivity){
	if(!initComplete)
		return;

	uint8_t accConfigTmp;
	readBlocking(imuAddress, ACCEL_CONFIG, 1, &accConfigTmp);
	accConfigTmp &= 0xe7;
	switch(sensitivity) {
	case 0:
		writeBlocking(imuAddress, ACCEL_CONFIG, (0x00 << 3) | accConfigTmp);
		accSensitivity = 2.0 / 32768.0;
		break;
	case 1:
		writeBlocking(imuAddress, ACCEL_CONFIG, (0x01 << 3) | accConfigTmp);
		accSensitivity = 4.0 / 32768.0;
		break;
	case 2:
		writeBlocking(imuAddress, ACCEL_CONFIG, (0x02 << 3) | accConfigTmp);
		accSensitivity = 8.0 / 32768.0;
		break;
	case 3:
		writeBlocking(imuAddress, ACCEL_CONFIG, (0x03 << 3) | accConfigTmp);
		accSensitivity = 16.0 / 32768.0;
		break;
	}
}


void Mpu9250::setGyroSensitivity(uint8_t sensitivity){
	if(!initComplete)
		return;

	uint8_t gyroConfigTmp;
	readBlocking(imuAddress, GYRO_CONFIG, 1, &gyroConfigTmp);
	gyroConfigTmp &= 0xe7;
	switch(sensitivity) {
	case 0:
		writeBlocking(imuAddress, GYRO_CONFIG, (0x00 << 3) | gyroConfigTmp);
		gyroSensitivity = 250.0 / 32768.0;
		break;
	case 1:
		writeBlocking(imuAddress, GYRO_CONFIG, (0x01 << 3) | gyroConfigTmp);
		gyroSensitivity = 500.0 / 32768.0;
		break;
	case 2:
		writeBlocking(imuAddress, GYRO_CONFIG, (0x02 << 3) | gyroConfigTmp);
		gyroSensitivity = 1000.0 / 32768.0;
		break;
	case 3:
		writeBlocking(imuAddress, GYRO_CONFIG, (0x03 << 3) | gyroConfigTmp);
		gyroSensitivity = 2000.0 / 32768.0;
		break;
	}
}


Mpu9250::Vec3 Mpu9250::readGyroData(){
	if(!initComplete)
		return Vec3(0, 0, 0);

#ifndef ASYNC_IMU
	readBlocking(imuAddress, GYRO_XOUT_H, 6, imuData + 8);
#else
	HAL_NVIC_DisableIRQ(dmaIr);
#endif
	int16_t x = imuData[8] << 8 | imuData[9];
	int16_t y = imuData[10] << 8 | imuData[11];
	int16_t z = imuData[12] << 8 | imuData[13];
#ifdef ASYNC_IMU
	HAL_NVIC_EnableIRQ(dmaIr);
#endif
	Vec3 data(x * gyroSensitivity, y * gyroSensitivity, z * gyroSensitivity);
	return data;
}


Mpu9250::Vec3 Mpu9250::readAccData(){
	if(!initComplete)
		return Vec3(0, 0, 0);

#ifndef ASYNC_IMU
	readBlocking(imuAddress, ACCEL_XOUT_H, 6, imuData);
#else
	HAL_NVIC_DisableIRQ(dmaIr);
#endif
	int16_t x = imuData[0] << 8 | imuData[1];
	int16_t y = imuData[2] << 8 | imuData[3];
	int16_t z = imuData[4] << 8 | imuData[5];
#ifdef ASYNC_IMU
	HAL_NVIC_EnableIRQ(dmaIr);
#endif
	Vec3 data(x * accSensitivity, y * accSensitivity, z * accSensitivity);
	return data;
}


float Mpu9250::readTempData(){
	if(!initComplete)
		return 0;

#ifndef ASYNC_IMU
	readBlocking(imuAddress, TEMP_OUT_H, 2, imuData + 6);
#else
	HAL_NVIC_DisableIRQ(dmaIr);
#endif
	int16_t t = imuData[6] << 8 | imuData[7];
#ifdef ASYNC_IMU
	HAL_NVIC_EnableIRQ(dmaIr);
#endif
	return (t - 21.0) / 333.87 + 21.0;
}


Mpu9250::Vec3 Mpu9250::readMagData(){
	if(!initComplete)
		return Vec3(0, 0, 0);

#ifndef ASYNC_IMU
	readBlocking(magAddress, AK8963_XOUT_L, 7, magData);
#else
	HAL_NVIC_DisableIRQ(dmaIr);
#endif
	int16_t x = magData[1] << 8 | magData[0];
	int16_t y = magData[3] << 8 | magData[2];
	int16_t z = magData[5] << 8 | magData[4];
#ifdef ASYNC_IMU
	HAL_NVIC_EnableIRQ(dmaIr);
#endif
	const float sensitivity = 4912.0 / 32768.0;
	Vec3 data(x * magCoeff_x * sensitivity, y * magCoeff_y * sensitivity, z * magCoeff_z * sensitivity);
	return data;
}



