#include <cstdio>
#include <chrono> // Time benchmarking
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/uart.h"
#include "hardware/timer.h"
#include "pico/multicore.h" // Used to blink LED

// Write automated testing for throughput

// Define pins for encoders
#define ENCODER1_PIN_A 9
#define ENCODER1_PIN_B 10
#define ENCODER2_PIN_A 11
#define ENCODER2_PIN_B 12

// Define pin for LED
#define LED_PIN 13

// Define control words byte
#define INITIALIZE_INTERRUPTS_BYTE 0x41
#define RETURN_ENCODER_BYTE 0x42
#define DISABLE_INTERRUPTS_BYTE 0x43
#define ENABLE_INTERRUPTS_BYTE 0x44
#define RESET_ENCODER_POS_BYTE 0x45
#define RETURN_RPM_BYTE 0x46
#define CLEAR_SCREEN_BYTE 0x47

#define PULSES_PER_REVOLUTION 800

// Variables to store encoder positions
volatile int32_t encoder1_position = 0;
volatile int32_t encoder2_position = 0;

static uint32_t last_time = 0;

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
    // printf("Encoder 1 ISR --- Encoder 1 Position: %ld, Encoder 2 Position: %ld\n", encoder1_position, encoder2_position);
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
    // printf("Encoder 2 ISR --- Encoder 1 Position: %ld, Encoder 2 Position: %ld\n", encoder1_position, encoder2_position);
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
float calculate_rpm(int32_t encoder_position, uint32_t time_interval_ms) {
    float revolutions = (float)encoder_position / PULSES_PER_REVOLUTION;
    printf("Revolutions: %f\n", revolutions);
    float minutes = (float)time_interval_ms / 60000.0; // Convert milliseconds to minutes
    printf("Minutes: %f\n", minutes);
    return revolutions / minutes;
}


int main() {

    stdio_init_all(); // Initialize all standard IO, including USB

    // Define the specific byte to wait for
    uint8_t receivedByte;

    // Wait for the specific byte
    printf("Waiting for byte: 0x%X\n", INITIALIZE_INTERRUPTS_BYTE);
    do {
        receivedByte = getchar(); // Blocking read
        printf("Received: 0x%X\n", receivedByte); // Echo back the received byte
    } while (receivedByte != INITIALIZE_INTERRUPTS_BYTE);

    // Signal byte received and initialize the encoders
    printf("Byte 0x%X received. Initializing encoders...\n", receivedByte);

    // Time benchmark start
    auto startTime = std::chrono::high_resolution_clock::now();

    // Initialize the encoders
    init_encoders();

    // Signal pins are initialized
    printf("Pins initialized\n");

    sleep_ms(1000); // Test for time benchmark

    // Time benchmark end
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    printf("Initialization took %ld nanoseconds\n", duration.count());

    // Prompt user for data return
    printf("Enter control word for feather board:\n");

    while (true) {
        receivedByte = getchar(); // Blocking read

        switch (receivedByte)
        {
            case RETURN_ENCODER_BYTE:
                printf("Encoder 1 Position: %ld, Encoder 2 Position: %ld\n", encoder1_position, encoder2_position);
                break;
            
            case RESET_ENCODER_POS_BYTE:
                encoder1_position = 0;
                encoder2_position = 0;
                printf("Encoder positions reset\n");
                break;

            case DISABLE_INTERRUPTS_BYTE:
                gpio_set_irq_enabled(ENCODER1_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false);
                gpio_set_irq_enabled(ENCODER1_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false);
                gpio_set_irq_enabled(ENCODER2_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false);
                gpio_set_irq_enabled(ENCODER2_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false);
                printf("Interrupts disabled\n");
                break;

            case ENABLE_INTERRUPTS_BYTE:
                gpio_set_irq_enabled(ENCODER1_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
                gpio_set_irq_enabled(ENCODER1_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
                gpio_set_irq_enabled(ENCODER2_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
                gpio_set_irq_enabled(ENCODER2_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
                printf("Interrupts enabled\n");
                break;

            case RETURN_RPM_BYTE: {
                uint32_t current_time = to_ms_since_boot(get_absolute_time());
                uint32_t time_interval = current_time - last_time;

                // Calculate RPMs for each encoder
                float rpm1 = calculate_rpm(encoder1_position, time_interval);
                float rpm2 = calculate_rpm(encoder2_position, time_interval);

                printf("Encoder 1 RPM: %.2f, Encoder 2 RPM: %.2f\n", rpm1, rpm2);

                // Update last_time and reset encoder positions
                last_time = current_time;
                break;
            }

            case CLEAR_SCREEN_BYTE:
                printf("\n\n\n\n\n\n\n\n\n\n\n");
                break;

            default:
                break;
        }

        tight_loop_contents(); // Prevent the main loop from exiting
    }

    return 0;
}