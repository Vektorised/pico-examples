// encoder.hpp
// Carson Powers
// Header file for the firmware funcitonality on the Adafruit Feather RP2040 for the AHSR robot

#include <cstdio>

// Time benchmarking
#include <chrono>
#include "hardware/sync.h"
#include "hardware/structs/systick.h"

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

// Clock Frequency
#define CLOCK_FREQUENCY_HZ 125000000  // 125 MHz for RP2040

// Current (09/04/2024) function words are keyboard only

// Return Data Functions
#define RETURN_ENCODER_BYTE 0x45 // 'E'
#define RETURN_ENCODER_1_BYTE 0x31 // '1'
#define RETURN_ENCODER_2_BYTE 0x32 // '2'
#define INJECT_OFFSET_BYTE 0x4F // 'O'

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
extern volatile int32_t encoder1_position;
extern volatile int32_t encoder2_position;
extern int32_t encoder1_last_position;
extern int32_t encoder2_last_position;

// Last time (RPM Calculation)
extern uint32_t last_time;

extern bool interrupts_enabled;
extern bool active_reporting;

/**
 * @brief Interrupt service routine for encoder 1.
 * 
 * Called when interrupt is triggered. Updates encoder1_position based on encoder1_pin_a and encoder1_pin_b.
 *
 * @param gpio GPIO number
 * @param events Events
 * @return void
 */
void encoder1_isr(uint gpio, uint32_t events);

/**
 * @brief Interrupt service routine for encoder 2.
 * 
 * Called when interrupt is triggered. Updates encoder2_position based on encoder2_pin_a and encoder2_pin_b.
 *
 * @param gpio GPIO number
 * @param events Events
 * @return void
 */
void encoder2_isr(uint gpio, uint32_t events);

/**
 * @brief Middleman function for encoder ISRs.
 * 
 * Called when interrupt is triggered on either encoder channel.
 *
 * @param gpio GPIO number
 * @param events Events
 * @return void
 */
void gpio_isr_callback(uint gpio, uint32_t events);

/**
 * @brief Initialize encoders.
 * 
 * Initialize encoders and set up interrupt service routines.
 *
 * @return void
 */
void init_encoders();

/**
 * @brief Function to calculate RPM for motors
 * 
 * Calculates RPM based on current position, last position, and time interval.
 *
 * @param current_position Current position
 * @param last_position Last position
 * @param time_interval_ms Time interval
 * 
 * @return float
 */
float calculate_rpm(int32_t current_position, int32_t last_position, uint32_t time_interval_ms);

/**
 * @brief DEBUG: Function to set up systick clock counter
 * 
 * Sets up systick clock counter.
 * 
 * @return void
 */
void setup_systick();

/**
 * @brief DEBUG: Function to check systick clock counter
 * 
 * Busy loop to evalutate difference in cycle count
 * 
 * @return void
 */
void check_systick_counting();