#include <cstdio>
#include <chrono> // Time benchmarking
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/uart.h"
#include "hardware/timer.h"
#include "pico/multicore.h" // Used to blink LED

// Define pins for encoders
#define ENCODER1_PIN_A 9
#define ENCODER1_PIN_B 10
#define ENCODER2_PIN_A 11
#define ENCODER2_PIN_B 12

// Define pin for LED
#define LED_PIN 13

// Number of pulses per revolution
#define PULSES_PER_REVOLUTION 800

// Current (09/04/2024) function words are keyboard only

// Return Data Functions
#define RETURN_ENCODER_BYTE 0x45 // 'E'
#define RETURN_RPM_BYTE 0x52 // 'R'

// Interrupt Functions
#define INITIALIZE_INTERRUPTS_BYTE 0x49 // 'I'
#define TOGGLE_INTERRUPTS_BYTE 0x54 // 'T'

// Encoder Functions
#define RESET_ENCODER_POS_BYTE 0x5A // 'Z'

// Screen Functions
#define CLEAR_SCREEN_BYTE 0x43 // 'C'
#define ACTIVE_REPORT_BYTE 0x41 // 'A'

// Variables to store encoder positions
volatile int32_t encoder1_position = 0;
volatile int32_t encoder2_position = 0;

// Last time (RPM Calculation)
uint32_t last_time = 0;

bool interrupts_enabled = false;
bool active_reporting = false;

// ISR for encoder 1 position
void encoder1_isr(uint gpio, uint32_t events);

// ISR for encoder 2 position
void encoder2_isr(uint gpio, uint32_t events);

// Middleman function for ISR
void gpio_isr_callback(uint gpio, uint32_t events);

// Initialize encoders
void init_encoders();

// Function to calculate RPM for motors
float calculate_rpm(int32_t encoder_position, uint32_t time_interval_ms);