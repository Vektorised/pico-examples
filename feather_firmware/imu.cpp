// imu.cpp
// Carson Powers
// Source file for the IMU functionality on the Adafruit Feather RP2040 on the AHSR robot

#include "imu.hpp"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

IMU::IMU() {
    this->ax = 0;
    this->ay = 0;
    this->az = 0;
    this->gx = 0;
    this->gy = 0;
    this->gz = 0;
    this->temp = 0;
}

void IMU::readIMU(uint8_t reg, uint8_t* read_buffer, uint8_t bufferLength)
{
    i2c_write_blocking(i2c_default, MPU6050_ADDR, &reg, 1, true);  // Register address
    i2c_read_blocking(i2c_default, MPU6050_ADDR, read_buffer, bufferLength, false);  // Read data
}

void IMU::writeIMU(uint8_t reg, uint8_t data)
{
    uint8_t write_buffer[] = {reg, data};
    i2c_write_blocking(i2c_default, MPU6050_ADDR, write_buffer, 2, false);
}

void IMU::initializeIMU()
{
    // Reset device
    writeIMU(0x6B, 0x80);

    sleep_ms(100); // Wait for reset

    // Wake up device
    writeIMU(0x6B, 0x00);

    sleep_ms(100); // Wait for wake up

    // Set gyro full scale range 500dps
    writeIMU(0x1B, 0x08);

    sleep_ms(100); // Wait for change

    // Set accel full scale range 4g
    writeIMU(0x1C, 0x08);
}

void IMU::resetIMU()
{
    this->ax = 0;
    this->ay = 0;
    this->az = 0;
    this->gx = 0;
    this->gy = 0;
    this->gz = 0;
}
