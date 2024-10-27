// main.cpp
// Carson Powers
// Main entry point for the Feather firmware on the AHSR robot

#include "feather.hpp"
#include "pico/stdlib.h"

int main() {
    // Initialize stdio for debugging purposes
    stdio_init_all();
    
    // Create an instance of the Feather class
    Feather feather;

    // Initialize the robot (encoders, IMU, etc.)
    feather.init();

    // Main control loop (handles periodic tasks)
    feather.loop();

    // In a real embedded program, execution would never reach here.
    return 0;
}
