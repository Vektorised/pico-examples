// encoder.cpp
// Carson Powers
// Source file for encoder functionality on the Adafruit Feather RP2040 on the AHSR robot

#include "encoder.hpp" // Encoder header file

// Pico libraries
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/structs/systick.h"

// TinyUSB for USB communication
#include "tusb.h"

// Variables to store encoder positions
volatile int32_t encoder1_position = 0;
volatile int32_t encoder2_position = 0;
int32_t encoder1_last_position = 0;
int32_t encoder2_last_position = 0;

// Last time (RPM Calculation)
uint32_t last_time = 0;

// Interrupts enabled
bool interrupts_enabled = false;

// Active reporting
bool active_reporting = false;

// ISR for encoder 1 position
void encoder1_isr(uint gpio, uint32_t events) {

    // -------------------- BENCHMARK CODE -------------------- //
    // // Start the cycle counter
    // uint32_t start_cycles = systick_hw->cvr;
    // -------------------- BENCHMARK CODE -------------------- //

    static bool last_a = false;
    static bool last_b = false;

    // Get gpio values
    bool a = gpio_get(ENCODER1_PIN_A);
    bool b = gpio_get(ENCODER1_PIN_B);

    // Quadrature decoding logic
    if (a != last_a) {
        encoder1_position += (b != a) ? 1 : -1;
    }

    // Assign last values
    last_a = a;
    last_b = b;

    // -------------------- BENCHMARK CODE -------------------- //
    // // Stop the cycle counter
    // uint32_t end_cycles = systick_hw->cvr;

    // // Calculate the elapsed cycles (assuming the counter is counting down)
    // uint32_t elapsed_cycles = start_cycles - end_cycles;

    // float elapsedTime = (elapsed_cycles * 8) / 1000.0f; // Convert cycles to us

    // // Debug output for encoder position and benchmark result
    // if (active_reporting) {
    //     printf("Encoder 1 ISR --- Encoder 1 Position: %ld --- Cycles: %u --- Time: %f us\n", encoder1_position, elapsed_cycles, elapsedTime);
    // }
    // -------------------- BENCHMARK CODE -------------------- //
}

// ISR for encoder 2 position
void encoder2_isr(uint gpio, uint32_t events) {
    // -------------------- BENCHMARK CODE -------------------- //
    // // Start the cycle counter
    // uint32_t start_cycles = systick_hw->cvr;
    // -------------------- BENCHMARK CODE -------------------- //

    static bool last_a = false;
    static bool last_b = false;

    // Get gpio values
    bool a = gpio_get(ENCODER2_PIN_A);
    bool b = gpio_get(ENCODER2_PIN_B);

    // Quadrature decoding logic
    if (a != last_a) {
        encoder2_position += (b != a) ? 1 : -1;
    }

    // Assign last values
    last_a = a;
    last_b = b;

    // -------------------- BENCHMARK CODE -------------------- //
    // // Stop the cycle counter
    // uint32_t end_cycles = systick_hw->cvr;

    // // Calculate the elapsed cycles (assuming the counter is counting down)
    // uint32_t elapsed_cycles = start_cycles - end_cycles;

    // float elapsedTime = (elapsed_cycles * 8) / 1000.0f; // Convert cycles to us

    // // Debug output for encoder position and benchmark result
    // if (active_reporting) {
    //     printf("Encoder 2 ISR --- Encoder 2 Position: %ld --- Cycles: %u --- Time: %f us\n", encoder2_position, elapsed_cycles, elapsedTime);
    // }
    // -------------------- BENCHMARK CODE -------------------- //
}

// ISR for both encoders (rp2040 has only 1 GPIO interrupt)
void gpio_isr_callback(uint gpio, uint32_t events) {
    if (gpio == ENCODER1_PIN_A || gpio == ENCODER1_PIN_B) {
        encoder1_isr(gpio, events);  // Call encoder 1 ISR
    } else if (gpio == ENCODER2_PIN_A || gpio == ENCODER2_PIN_B) {
        encoder2_isr(gpio, events);  // Call encoder 2 ISR
    }
}

// Function to initialize GPIO pins and attach interrupts
void init_encoders() {

    // Encoder 1 Pin Initialization
    gpio_init(ENCODER1_PIN_A);
    gpio_init(ENCODER1_PIN_B);
    gpio_set_dir(ENCODER1_PIN_A, GPIO_IN);
    gpio_set_dir(ENCODER1_PIN_B, GPIO_IN);
    gpio_pull_up(ENCODER1_PIN_A);
    gpio_pull_up(ENCODER1_PIN_B);

    // Encoder 2 Pin Initialization
    gpio_init(ENCODER2_PIN_A);
    gpio_init(ENCODER2_PIN_B);
    gpio_set_dir(ENCODER2_PIN_A, GPIO_IN);
    gpio_set_dir(ENCODER2_PIN_B, GPIO_IN);
    gpio_pull_up(ENCODER2_PIN_A);
    gpio_pull_up(ENCODER2_PIN_B);

    // Attach interrupts to all encoder pins using the same callback
    gpio_set_irq_enabled_with_callback(ENCODER1_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_isr_callback);
    gpio_set_irq_enabled(ENCODER1_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(ENCODER2_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(ENCODER2_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    // Zero the encoder positions
    encoder1_position = 0;
    encoder2_position = 0;
    encoder1_last_position = 0;
    encoder2_last_position = 0;
}

// Function to calculate RPM for motors
float calculate_rpm(int32_t current_position, int32_t last_position, uint32_t time_interval_ms) {
    // Calculate the difference in encoder position
    int32_t position_difference = current_position - last_position;
    
    // Calculate revolutions based on position difference
    float revolutions = (float)position_difference / PULSES_PER_REVOLUTION;
    
    // Convert the time interval from milliseconds to minutes
    float minutes = (float)time_interval_ms / 60000.0;
    
    // Return the RPM
    return revolutions / minutes;
}

void setup_systick() {
    // Set reload value to maximum (24-bit counter, max is 0xFFFFFF)
    systick_hw->rvr = 0x00FFFFFF;
    
    // Clear current value register
    systick_hw->cvr = 0;
    
    // Enable SysTick counter with processor clock
    systick_hw->csr = 0x00000005;  // Enable counter and use processor clock
}

void check_systick_counting() {
    // Print initial SysTick value
    uint32_t start_cycles = systick_hw->cvr;
    printf("SysTick start value: %u\n", start_cycles);

    // Create a small delay (e.g., a busy wait loop)
    for (volatile int i = 0; i < 1000000; i++);  // Busy wait loop
    
    // Print SysTick value after delay
    uint32_t end_cycles = systick_hw->cvr;
    printf("SysTick end value: %u\n", end_cycles);

    // Print the difference in cycles
    printf("Elapsed SysTick cycles: %u\n", start_cycles - end_cycles);
}
