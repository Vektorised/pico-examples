// encoder.cpp
// Carson Powers
// Source file for the encoder functionality on the Adafruit Feather RP2040 on the AHSR robot

#include "encoder.hpp"
#include "pico/stdlib.h"
#include <cstdio>

/**
 * @brief Construct a new Encoder object.
 * @param pin_a GPIO pin for channel A.
 * @param pin_b GPIO pin for channel B.
 */
Encoder::Encoder(uint pin_a, uint pin_b)
    : pin_a_(pin_a), pin_b_(pin_b), position_(0), last_a_(false), last_b_(false) {}

/**
 * @brief Initialize the encoder pins and setup interrupts.
 */
void Encoder::initializeEncoder() {
    // Initialize GPIO pins for the encoder
    // gpio_init(pin_a_);
    // gpio_init(pin_b_);
    // gpio_set_dir(pin_a_, GPIO_IN);
    // gpio_set_dir(pin_b_, GPIO_IN);
    // gpio_pull_up(pin_a_);
    // gpio_pull_up(pin_b_);

    // // Attach interrupts to encoder pins
    // gpio_set_irq_enabled_with_callback(pin_a_, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, handle_interrupt);
    // gpio_set_irq_enabled(pin_b_, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}

/**
 * @brief Interrupt service routine for handling GPIO events.
 * Updates the encoder position based on quadrature decoding.
 * @param gpio GPIO pin that triggered the interrupt.
 * @param events Events that occurred.
 */
void Encoder::handle_interrupt(uint gpio, uint32_t events) {
    // Get current pin states
    bool a = gpio_get(pin_a_);
    bool b = gpio_get(pin_b_);

     // Quadrature decoding logic
    if (a != last_a_) {
        position_ += (b != a) ? 1 : -1;
    }

    // Update last states
    last_a_ = a;
    last_b_ = b;
}

/**
 * @brief Get the current position of the encoder.
 * @return int32_t Current encoder position.
 */
int32_t Encoder::get_position() const {
    return position_;
}

/**
 * @brief Reset the encoder position to zero.
 */
void Encoder::reset_position() {
    position_ = 0;
}
