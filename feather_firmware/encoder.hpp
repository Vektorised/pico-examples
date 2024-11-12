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

// Define command bytes
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
 */
class Encoder {
public:
    /**
     * @brief Construct a new Encoder object.
     * @param pin_a The GPIO pin for the A channel.
     * @param pin_b The GPIO pin for the B channel.
     */
    Encoder(uint pin_a, uint pin_b);

    /**
     * @brief Initialize the encoder pins and interrupts.
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
     * @brief Static interrupt service routine for handling GPIO events.
     * Updates the encoder position based on quadrature decoding.
     */
    void handle_interrupt(uint gpio, uint32_t events);

private:
    uint pin_a_; ///< Pin for channel A.
    uint pin_b_; ///< Pin for channel B.
    volatile int32_t position_; ///< Encoder position.

    bool last_a_; ///< Last state of pin A.
    bool last_b_; ///< Last state of pin B
};

#endif // ENCODER_HPP
