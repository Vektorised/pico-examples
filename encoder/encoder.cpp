#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/uart.h"
#include "hardware/timer.h"

// Define pins for encoders
#define ENCODER1_PIN_A 9
#define ENCODER1_PIN_B 10
#define ENCODER2_PIN_A 11
#define ENCODER2_PIN_B 12

// Variables to store encoder positions
volatile int32_t encoder1_position = 0;
volatile int32_t encoder2_position = 0;

// ISR for encoder 1 position
void encoder1_isr(uint gpio, uint32_t events) {
    // Init previous state
    static bool last_a = false;
    static bool last_b = false;

    // Get current state of pins
    bool a = gpio_get(ENCODER1_PIN_A);
    bool b = gpio_get(ENCODER1_PIN_B);

    if (a != last_a || b != last_b) {
        if (last_a == last_b) {
            if (a != last_a) {
                encoder1_position++; // Positive Direction
            } else {
                encoder1_position--; // Negative Direction
            }
        } else {
            if (a == last_b) {
                encoder1_position++; // Positive Direction
            } else {
                encoder1_position--; // Negative Direction
            }
        }
    }

    // Update previous state
    last_a = a;
    last_b = b;

    // Debug output for encoder position
    printf("Encoder 1 ISR --- Encoder 1 Position: %ld, Encoder 2 Position: %ld\n", encoder1_position, encoder2_position);
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
    printf("Encoder 2 ISR --- Encoder 1 Position: %ld, Encoder 2 Position: %ld\n", encoder1_position, encoder2_position);
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

    // Attach interrupts to encoder 1 pins
    gpio_set_irq_enabled_with_callback(ENCODER1_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &encoder1_isr);
    gpio_set_irq_enabled(ENCODER1_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    // Attach interrupts to encoder 2 pins
    gpio_set_irq_enabled_with_callback(ENCODER2_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &encoder2_isr);
    gpio_set_irq_enabled(ENCODER2_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}

int main() {
    stdio_init_all(); // Initialize all standard IO, including USB

    // Initialize the encoders
    init_encoders();

    // Signal pins are initialized
    printf("Pins initialized\n");

    while (true) {
        tight_loop_contents(); // Prevent the main loop from exiting
    }

    return 0;

}
