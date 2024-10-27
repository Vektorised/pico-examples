#include "feather.hpp"

// Initialize the static instance pointer
Feather* Feather::instance_ = nullptr;

/**
 * @brief Construct a new Feather object.
 * Initializes the encoders and IMU.
 */
Feather::Feather()
    : encoder1_(ENCODER1_PIN_A, ENCODER1_PIN_B),
      encoder2_(ENCODER2_PIN_A, ENCODER2_PIN_B) {
    
    // Set static instance pointer to current object
    instance_ = this;
}

/**
 * @brief Initialize the robot's components (encoders, IMU, etc.).
 */
void Feather::init() {
    // Initialize both encoders
    encoder1_.init();
    encoder2_.init();

    // Initialize the IMU
    imu_.initializeIMU();

    // Attach GPIO interrupts and delegate to the correct encoder
    gpio_set_irq_enabled_with_callback(ENCODER1_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, gpio_callback);
    gpio_set_irq_enabled(ENCODER2_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}

/**
 * @brief Main loop of the robot.
 * This will handle periodic tasks such as reading sensor data.
 */
void Feather::loop() {
    while (true) {


        // Sleep or handle other tasks
        sleep_ms(100);
    }
}

/**
 * @brief Handle GPIO interrupts and delegate them to the correct encoder.
 * @param gpio GPIO pin that triggered the interrupt.
 * @param events Event flags associated with the interrupt.
 */
void Feather::gpio_callback(uint gpio, uint32_t events) {
    if (gpio == ENCODER1_PIN_A || gpio == ENCODER1_PIN_B) {
        instance_->encoder1_.handle_interrupt(gpio, events);
    } else if (gpio == ENCODER2_PIN_A || gpio == ENCODER2_PIN_B) {
        instance_->encoder2_.handle_interrupt(gpio, events);
    }
}
