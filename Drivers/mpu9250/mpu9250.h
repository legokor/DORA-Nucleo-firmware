/*
 * mpu9250.h
 *
 *  Created on: May 20, 2024
 *      Author: dkiovics
 */

#ifndef MPU9250_MPU9250_H_
#define MPU9250_MPU9250_H_

//#define ASYNC_IMU

#include "stm32f4xx_hal.h"

/**
 * @brief MPU9250 IMU and compass IC I2C driver class for the STM32 microcontroller.
 * The implementation heavily relies on the HAL STM32 drivers.
 *
 */
class Mpu9250 {
public:
	#ifdef ASYNC_IMU
	/**
	 * @brief Constructs the Mpu9250 class for async data retrieval using DMA.
	 *
	 * @param hi2c - the I2C HAL object pointer. The global interrupts must be enabled in the CubeMX,
	 * furthermore a DMA needs to be setup to read from peripheral to memory.
	 * @param imuAddress - the IMU IC I2C address (in reality this is the MPU9250 IC, but the IC's package also contains
	 * an AK8963 magnetometer IC).
	 * @param magAddress - the magnetometer IC I2C address (generally it is 0x0C).
	 * @param dmaIr - the IRQn_Type handle of the I2C DMA channel's interrupt
	 */
	Mpu9250(I2C_HandleTypeDef* hi2c, uint8_t imuAddress, uint8_t magAddress, IRQn_Type dmaIr);
	#else
	/**
	 * @brief Constructs the Mpu9250 class for sync (blocking) data retrieval.
	 *
	 * @param hi2c - the I2C HAL object pointer.
	 * @param imuAddress - the IMU IC I2C address (in reality this is the MPU9250 IC, but the IC's package also contains
	 * an AK8963 magnetometer IC).
	 * @param magAddress - the magnetometer IC I2C address (generally it is 0x0C).
	 */
	Mpu9250(I2C_HandleTypeDef* hi2c, uint8_t imuAddress, uint8_t magAddress);
	#endif

	/**
	 * @brief Data storage struct for a float32 Vec3.
	 *
	 */
	struct Vec3 {
		float x, y, z;
		Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
		Vec3(): x(0), y(0), z(0) {}
	};

	/**
	 * @brief Checks whether the IMU (MPU9250) was detected.
	 *
	 * @return true, if the IMU (MPU9250) was detected.
	 */
	bool detectImu();

	/**
	 * @brief Checks whether the magnetometer (AK8963) was detected.
	 *
	 * @return true if the magnetometer (AK8963) was detected.
	 */
	bool detectMagnetometer();

	/**
	 * @brief Sets the IMU to the default settings, which will suite most applications:
	 * 	- The magnetometer is set to 16 bit resolution with a 100Hz sample rate
	 * 	- The accelerometer is set to +-4g range with a 200Hz sample rate and a 44.8Hz DLPF
	 * 	- The gyro and thermometer are set to 2000DPS range (in case of the gyro) with a 200Hz sample rate and a 41Hz DLPF
	 *
	 */
	void setDefaultSettings();

	/**
	 * @brief Sets the sample rate divider.
	 * The divider is only active when DLPF is enabled and set to 1-6 (both in case of the gyro/temp and the accelerometer).
	 *
	 * @param divider - the divider that divides the internal (1kHz sample rate) by (1 + divider).
	 */
	void setSampleRateDivider(uint8_t divider);

	/**
	 * @brief Enable DLPF for the accelerometer (set fchoice_b to 0 -> fchoice to 1).
	 *
	 * @param enable - whether to enable the DLPF for the accelerometer.
	 */
	void enableAccDLPF(bool enable);

	/**
	 * @brief Enable DLPF for the gyro and temp sensors (set fchoice_b's to 0 -> fchoice's to 1).
	 *
	 * @param enable - whether to enable the DLPF for the gyro and thermometer.
	 */
	void enableGyroAndTempDLPF(bool enable);

	/**
	 * @brief Details about the values: https://invensense.tdk.com/wp-content/uploads/2015/02/RM-MPU-9250A-00-v1.6.pdf
	 * 	- table on page 15
	 *
	 * @param value - the accelerometer DLPF value
	 */
	void setAccDLPF(uint8_t value);

	/**
	 * @brief Details about the values: https://invensense.tdk.com/wp-content/uploads/2015/02/RM-MPU-9250A-00-v1.6.pdf
	 * 	- table on page 13
	 *
	 * @param value - the gyro and thermometer DLPF value
	 */
	void setGyroAndTempDLPF(uint8_t value);

	/**
	 * @brief Sets the gyro's sensitivity.
	 *
	 * @param sensitivity - possible (+/-) values are:
	 * 	- 0 - 250DPS
	 * 	- 1 - 500DPS
	 * 	- 2 - 1000DPS
	 * 	- 3 - 2000DPS
	 */
	void setGyroSensitivity(uint8_t sensitivity);

	/**
	 * @brief Sets the accelerometer's sensitivity.
	 *
	 * @param sensitivity - possible (+/-) values are:
	 * 	- 0 - 2G
	 * 	- 1 - 4G
	 * 	- 2 - 8G
	 * 	- 3 - 16G
	 */
	void setAccSensitivity(uint8_t sensitivity);

	#ifdef ASYNC_IMU
	/**
	 * @brief This function is only present when using the async version of the class.
	 * It needs to be called whenever the HAL_I2C_MemRxCpltCallback function is called.
	 *
	 * @param hi2c - the I2C HAL object pointer, this is used to do an internal check, whether it's the object's I2C.
	 */
	void i2cReceiveCpltCallback(I2C_HandleTypeDef* hi2c);

	/**
	 * @brief This function is only present when using the async version of the class.
	 * Call it whenever the I2C period timer overflows.
	 * It is advised to call this function at least as frequently as the highest sampling frequency set for any sensor,
	 * but not too frequently - the transmission of ~24 bytes must have plenty of time to complete between two calls.
	 *
	 */
	void timPeriodEllapsedCallback();

	/**
	 * @brief This function is only present when using the async version of the class.
	 * You can check whether new data is available to read using it.
	 *
	 * @return true, if new data is available (and clears the internal new data flag).
	 */
	bool newDataAvailable();
	#endif

	/**
	 * @brief Returns a vector containing the gyro data.
	 *
	 * @return the gyro data in °/s.
	 */
	Vec3 readGyroData();

	/**
	 * @brief Returns a vector containing the accelerometer data.
	 *
	 * @return the accelerometer data in m/s^2.
	 */
	Vec3 readAccData();

	/**
	 * @brief Returns the temperature data.
	 *
	 * @return the temperature in °C.
	 */
	float readTempData();

	/**
	 * @brief Returns a vector containing the magnetometer data.
	 *
	 * @return the magnetometer data in uT.
	 */
	Vec3 readMagData();

private:
	I2C_HandleTypeDef* const hi2c;
	const uint8_t imuAddress;
	const uint8_t magAddress;

	float magCoeff_x, magCoeff_y, magCoeff_z;
	float accSensitivity;
	float gyroSensitivity;

	volatile uint8_t imuData[14];
	volatile uint8_t magData[7];
	volatile bool initComplete = false;

	#ifdef ASYNC_IMU
	volatile uint8_t imuBuffer[14];
	volatile uint8_t magBuffer[7];
	volatile bool isReadingImu = false;
	volatile bool newData = false;

	const IRQn_Type dmaIr;

	volatile bool readEnabled = false;
	volatile bool readInProgress = false;
	#endif

	/**
	 * @brief Executes a blocking I2C byte write to a given memory address for a given device address.
	 * This is useful in case of the async version of the class - it handles the possible interrupts as well.
	 *
	 * @param devAddress - the I2C device address.
	 * @param regAddress - the register address on the device.
	 * @param data - the data byte.
	 * @return true on success.
	 */
	bool writeBlocking(uint8_t devAddress, uint8_t regAddress, uint8_t data);

	/**
	 * @brief Executes a blocking I2C read operation from a given memory address for a given device address
	 * into a buffer with a given length. This is useful in case of the async version of the class - it handles
	 * the possible interrupts as well.
	 *
	 * @param devAddress - the I2C device address.
	 * @param regAddress - the register address on the device.
	 * @param numBytes - the number of bytes to read.
	 * @param buffer - the pointer to the target buffer.
	 * @return true on success.
	 */
	bool readBlocking(uint8_t devAddress, uint8_t regAddress, uint8_t numBytes, volatile uint8_t* buffer);
};


#endif /* MPU9250_MPU9250_H_ */
