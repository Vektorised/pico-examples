// imu.hpp
// Carson Powers
// Header file for the IMU functionality on the Adafruit Feather RP2040 on the AHSR robot


#include <cstdint>

#define RETURN_IMU_DATA_BYTE 0x49 // 'I'

#define MPU6050_ADDR 0x68

#define I2C_SDA_PIN 2
#define I2C_SCL_PIN 3

class IMU {
    public:
        IMU();
        int16_t ax, ay, az;
        int16_t gx, gy, gz;
        int16_t mx, my, mz;

        void readIMU(uint8_t reg, uint8_t* buffer, uint8_t bufferLength);
        void writeIMU(uint8_t reg, uint8_t data);

        void initializeIMU();
};