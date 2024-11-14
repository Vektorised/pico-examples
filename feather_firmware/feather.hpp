#ifndef FEATHER_HPP
#define FEATHER_HPP

#include "encoder.hpp"
#include "imu.hpp"

// Command Bytes for Feather Operations
#define INITIALIZE_SENSORS_BYTE 0x49 // 'I'
#define RESET_SENSORS_BYTE 0x5A // 'Z'

/**
 * @class Feather
 * @brief Manages the Feather's components
 */
class Feather {
    public:
        /**
         * @brief Construct a new Feather object, initializing the encoders and IMU.
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
         * @brief Process usb command words and return apporpriate responses.
         */
        void process_usb_communication();

        /**
         * @brief Handles the usb communication in an infinite loop.
         */
        void loop();

    private:
        /**
         * @brief Handle GPIO interrupts and delegate them to the correct encoder.
         */
        static void gpio_callback(uint gpio, uint32_t events);
        
        static Feather* feather_instance_; // Static pointer to the current instance of the Feather class.
        
        Encoder encoder1_; // Encoder object for the first encoder (left motor).
        Encoder encoder2_; // Encoder object for the second encoder (right motor).
        IMU imu_; // IMU object for reading IMU data.
};

#endif // FEATHER_HPP
