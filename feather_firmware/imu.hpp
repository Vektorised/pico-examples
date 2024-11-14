// imu.hpp
// Carson Powers
// Header file for the IMU functionality on the Adafruit Feather RP2040 on the AHSR robot

// IMU: MPU6050
// Datasheet: http://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
// Register Map: http://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf

#ifndef IMU_HPP
#define IMU_HPP

// Standard Libraries
#include <cstdint>

// Command Bytes for IMU Operations
#define RETURN_IMU_DATA_BYTE 0x49 // 'I'

// Addressing Bytes
#define MPU6050_ADDR 0x68
#define IMU_READ_START_BYTE 0x3B // Start reading at register 0x3B

// Register Map
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D    
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

// I2C pins on Feather RP2040
#define I2C_SDA_PIN 2
#define I2C_SCL_PIN 3

// IMU Data Buffer Length
#define IMU_DATA_BUFFER_LENGTH 14

class IMU {
    public:
        IMU();
        int16_t ax, ay, az;
        int16_t gx, gy, gz;
        int16_t temp;
        
        /**
         * @brief Read data from the IMU
         * 
         * @param reg The register to begin reading from.
         * @param read_buffer The buffer to read into.
         * @param bufferLength The length of the buffer and the number of bytes to read.
         */
        void readIMU(uint8_t reg, uint8_t* read_buffer, uint8_t bufferLength);

        /**
         * @brief Write single byte of data to the IMU
         * 
         * @param reg The register to write to.
         * @param write_buffer The data to write.
         */
        void writeIMU(uint8_t reg, uint8_t write_buffer);

        /**
         * @brief Initialize the IMU
         * 
         */
        void initializeIMU();

        /**
         * @brief Reset the IMU
         * 
         */
        void resetIMU();
};

#endif