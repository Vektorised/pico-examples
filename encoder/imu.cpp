#include "imu.hpp"

#include "pico/stdlib.h"
#include "hardware/i2c.h"

IMU::IMU() {
    this->ax = 0;
    this->ay = 0;
    this->az = 0;
    this->gx = 0;
    this->gy = 0;
    this->gz = 0;
    this->mx = 0;
    this->my = 0;
    this->mz = 0;
}

void IMU::readIMU(uint8_t reg, uint8_t* buffer, uint8_t bufferLength)
{
    i2c_write_blocking(i2c0, MPU6050_ADDR, &reg, 1, true);  // Register address
    i2c_read_blocking(i2c0, MPU6050_ADDR, buffer, bufferLength, false);  // Read data
}

void IMU::writeIMU(uint8_t reg, uint8_t data)
{
    uint8_t buffer[] = {reg, data};
    i2c_write_blocking(i2c0, MPU6050_ADDR, buffer, 2, false);
}

void IMU::initializeIMU()
{
    // Reset device
    uint8_t reset_buffer[] = {0x6B, 0x00};
    i2c_write_blocking(i2c0, MPU6050_ADDR, buffer, 2, false);

    sleep_ms(100); // Wait for reset

    // Wake up device
    uint8_t wakeup_buffer[] = {0x6B, 0x00};
    i2c_write_blocking(i2c0, MPU6050_ADDR, buffer, 2, false);
}

