#ifndef FEATHER_HPP
#define FEATHER_HPP

#include "encoder.hpp"
#include "imu.hpp"
#include "pico/stdlib.h"
#include "tusb.h"
#include "hardware/i2c.h"

#define INITIALIZE_SENSORS_BYTE 0x49 // 'I'

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
     * @brief Process usb command and return apporpriate response.
     */
    void process_usb_communication();

    /**
     * @brief Main loop of the robot.
     * This will handle periodic tasks such as reading sensor data.
     */
    void loop();

    /**
     * @brief Handle GPIO interrupts and delegate them to the correct encoder.
     */
    static void gpio_callback(uint gpio, uint32_t events);

    Encoder encoder1_;  ///< Encoder object for the first encoder.
    Encoder encoder2_;  ///< Encoder object for the second encoder.

    IMU imu_;           ///< IMU object for reading IMU data.

private:

    static Feather* instance_;  ///< Static pointer to the current instance of the Feather class.

    // Encoder encoder1_;  ///< Encoder object for the first encoder.
    // Encoder encoder2_;  ///< Encoder object for the second encoder.
    // IMU imu_;           ///< IMU object for reading IMU data.
};

#endif // FEATHER_HPP
