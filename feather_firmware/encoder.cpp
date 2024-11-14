// encoder.cpp
// Carson Powers
// Source file for the encoder functionality on the Adafruit Feather RP2040 on the AHSR robot

#include "encoder.hpp"
#include "pico/stdlib.h"
#include <cstdio>

Encoder::Encoder(uint8_t pin_a, uint8_t pin_b)
    : pin_a_(pin_a), pin_b_(pin_b), position_(0), last_a_(false), last_b_(false) {}

void Encoder::initializeEncoder() {
    this->position_ = 0;
}

int32_t Encoder::get_position() const {
    return this->position_;
}

void Encoder::reset_position() {
    this->position_ = 0;
}

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
