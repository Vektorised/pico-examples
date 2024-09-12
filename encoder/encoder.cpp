// encoder.cpp
// Carson Powers
// Source file for encoder functionality on the Adafruit Feather RP2040 on the AHSR robot

#include "encoder.hpp" // Encoder header file

// Write automated testing for throughput

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
    // Init previous state
    static bool last_a = false;
    static bool last_b = false;

    // Get current state of pins
    bool a = gpio_get(ENCODER1_PIN_A);
    bool b = gpio_get(ENCODER1_PIN_B);

    // Determine direction based on A and B signals
    if (a != last_a) {
        if (b != a) {
            encoder1_position++; // Positive direction
        } else {
            encoder1_position--; // Negative Direction
        }
    } else if (b != last_b) {
        if (a == b) {
            encoder1_position++; // Positive direction
        } else {
            encoder1_position--; // Negative Direction
        }
    }

    // Update previous state
    last_a = a;
    last_b = b;

    // Debug output for encoder position
    if (active_reporting) {
        printf("Encoder 1 ISR --- Encoder 1 Position: %ld", encoder1_position);
    }
}
// ISR for encoder 2 position
void encoder2_isr(uint gpio, uint32_t events) {
    // Init previous state
    static bool last_a = false;
    static bool last_b = false;

    // Get current state of pins
    bool a = gpio_get(ENCODER2_PIN_A);
    bool b = gpio_get(ENCODER2_PIN_B);

    // Determine direction based on A and B signals
    if (a != last_a) {
        if (b != a) {
            encoder2_position++; // Positive direction
        } else {
            encoder2_position--; // Negative Direction
        }
    } else if (b != last_b) {
        if (a == b) {
            encoder2_position++; // Positive direction
        } else {
            encoder2_position--; // Negative Direction
        }
    }

    // Update previous state
    last_a = a;
    last_b = b;

    // Debug output for encoder position
    if (active_reporting) {
        printf("Encoder 2 ISR --- Encoder 2 Position: %ld\n", encoder2_position);
    }
}

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
}

// Function to calculate RPM for motors
float calculate_rpm(int32_t current_position, int32_t last_position, uint32_t time_interval_ms) {
    // Calculate the difference in encoder position
    int32_t position_difference = current_position - last_position;
    
    // Calculate revolutions based on position difference
    float revolutions = (float)position_difference / PULSES_PER_REVOLUTION;
    printf("Revolutions: %f\n", revolutions);
    
    // Convert the time interval from milliseconds to minutes
    float minutes = (float)time_interval_ms / 60000.0;
    printf("Minutes: %f\n", minutes);
    
    // Return the RPM
    return revolutions / minutes;
}
