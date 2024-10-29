// imu.hpp
// Carson Powers
// Header file for the IMU functionality on the Adafruit Feather RP2040 on the AHSR robot

// IMU: MPU6050
// Datasheet: http://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
// Register Map: http://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf

#ifndef IMU_HPP
#define IMU_HPP

#include <cstdint>
#include <cstdio>
#include <iostream>

#define RETURN_IMU_DATA_BYTE 0x49 // 'I'
#define IMU_DATA_BUFFER_LENGTH 14

#define MPU6050_ADDR 0x68

#define I2C_SDA_PIN 2
#define I2C_SCL_PIN 3

class IMU {
    public:
        IMU();
        int16_t ax, ay, az;
        int16_t gx, gy, gz;
        int16_t temp;

        void readIMU(uint8_t reg, uint8_t* read_buffer, uint8_t bufferLength);

        void writeIMU(uint8_t reg, uint8_t write_buffer);

        void initializeIMU();

        void updateIMU();
};

#endif