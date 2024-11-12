#ifndef FEATHER_HPP
#define FEATHER_HPP

#include "encoder.hpp"
#include "imu.hpp"
#include "pico/stdlib.h"
#include "tusb.h"
#include "hardware/i2c.h"

#define INITIALIZE_SENSORS_BYTE 0x49 // 'I'
#define RESET_SENSORS_BYTE 0x5A // 'Z'

/**
 * @class Feather
 * @brief Manages the Feather's components
 */
class Feather {
public:
    /**
     * @brief Construct a new Feather object.
     */
    Feather();

    /**
     * @brief Initialize the robot's components (encoders, IMU, etc.).
     */
    void initializeFeather();

    /**
     * @brief Reset the sensor values (encoders, IMU, etc.).
     */
    void resetFeather();

    /**
     * @brief Process usb command and return apporpriate response.
     */
    void process_usb_communication();

    /**
     * @brief Main loop of the robot.
     * This will handle periodic tasks such as reading sensor data.
     */
    void loop();

private:
    /**
     * @brief Handle GPIO interrupts and delegate them to the correct encoder.
     */
    static void gpio_callback(uint gpio, uint32_t events);
    
    static Feather* feather_instance_;  ///< Static pointer to the current instance of the Feather class.
    
    Encoder encoder1_;  ///< Encoder object for the first encoder.
    Encoder encoder2_;  ///< Encoder object for the second encoder.
    IMU imu_;           ///< IMU object for reading IMU data.
};

#endif // FEATHER_HPP
