// main.cpp
// Carson Powers
// Main entry point for the Feather firmware on the AHSR robot

// Custom Hardware Libraries
#include "feather.hpp"
#include "encoder.hpp"
#include "imu.hpp"

// Standard Libraries
#include <stdio.h>

// Pico Libraries
#include "tusb.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

int main() {
    // Initialize stdio for USB communication
    stdio_init_all();

    // Initialize TinyUSB
    tusb_init();

    // Initialize I2C
    i2c_init(i2c_default, 400000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(I2C_SDA_PIN, I2C_SCL_PIN, GPIO_FUNC_I2C));
    
    // Create an instance of the Feather class
    Feather feather;

    // Temp byte to wait for
    uint8_t receivedByte;
    
    // Wait for the specific byte
    do {
        receivedByte = getchar(); // Blocking read
    } while (receivedByte != INITIALIZE_SENSORS_BYTE);

    // Initialize the feather (encoders, IMU, etc.)
    feather.initializeFeather();

    // Main control loop (infinite loop)
    feather.loop();

    return 0;
}
