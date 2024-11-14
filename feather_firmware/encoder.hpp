// encoder.hpp
// Carson Powers
// Header file for the encoder functionality on the Adafruit Feather RP2040 on the AHSR robot

#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <cstdint>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Define encoder pins
#define ENCODER1_PIN_A 9
#define ENCODER1_PIN_B 10
#define ENCODER2_PIN_A 11
#define ENCODER2_PIN_B 12

// Command Bytes for Encoder Operations
#define RETURN_ENCODERS_BYTE 0x45 // 'E'
#define RETURN_ENCODER_1_BYTE 0x31 // '1'
#define RETURN_ENCODER_2_BYTE 0x32 // '2'

// Removed rpm function as it can be calculated faster on jetson side

// Define encoder data buffer length
#define DUAL_ENCODER_DATA_BUFFER_LENGTH 8
#define SINGLE_ENCODER_DATA_BUFFER_LENGTH 4

// Number of pulses per revolution (configure based on your encoder)
#define PULSES_PER_REVOLUTION 800

/**
 * @class Encoder
 * @brief Manages a single quadrature encoder for a motor.
 * 
 * The Encoder class provides methods to initialize, read, and reset the 
 * position of a quadrature encoder. It uses GPIO interrupts to update 
 * the encoder position in real-time.
 */
class Encoder {
    public:
        /**
         * @brief Construct a new Encoder object.
         * @param[in] pin_a The GPIO pin for the A channel (uint8_t).
         * @param[in] pin_b The GPIO pin for the B channel (uint8_t).
         */
        Encoder(uint8_t pin_a, uint8_t pin_b);

        /**
         * @brief Initialize the encoder position.
         */
        void initializeEncoder();

        /**
         * @brief Get the current encoder position.
         * @return int32_t Current encoder position.
         */
        int32_t get_position() const;

        /**
         * @brief Reset the encoder position to zero.
         */
        void reset_position();

        /**
         * @brief Interrupt service routine for handling GPIO events.
         * 
         * This function is called when an encoder pin state changes. It updates
         * the encoder position based on the quadrature decoding of channels A and B.
         * 
         * @param[in] gpio The GPIO pin that triggered the interrupt (uint).
         * @param[in] events The events that occurred (uint32_t).
         */
        void handle_interrupt(uint gpio, uint32_t events);

    private:
        uint8_t pin_a_; // Pin number for channel A.
        uint8_t pin_b_; // Pin number for channel B.
        volatile int32_t position_; // Encoder position.

        bool last_a_; // Last state of pin A.
        bool last_b_; // Last state of pin B.
};

#endif // ENCODER_HPP
