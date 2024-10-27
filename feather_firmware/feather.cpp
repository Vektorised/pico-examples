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
 * @brief Initialize the sensor array components (encoders, IMU, etc.).
 */
void Feather::initializeFeather() {
    // Initialize both encoders
    encoder1_.initializeEncoder();
    encoder2_.initializeEncoder();

    // Initialize the IMU
    imu_.initializeIMU();

    // Attach GPIO interrupts and direct to middleman function
    gpio_set_irq_enabled_with_callback(ENCODER1_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, gpio_callback);
    gpio_set_irq_enabled(ENCODER1_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(ENCODER2_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(ENCODER2_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}

/**
 * @brief Main loop of the robot.
 * This will handle periodic tasks such as reading sensor data.
 */
void Feather::loop() {
    while (true) {
        process_usb_communication(); // Process USB command
        tight_loop_contents(); // Inline No-Op to keep compiler from optimizing out loop
    }
}

void Feather::process_usb_communication() {
    if (tud_cdc_available()) {
        uint8_t recievedByte = tud_cdc_read_char();

        switch (recievedByte) {
            case RETURN_ENCODER_BYTE:
            {
                // Initialize buffer for encoder positions
                uint8_t encoder_positions_buffer[ENCODER_DATA_BUFFER_LENGTH];

                // Get encoder positions
                int32_t encoder1_position = encoder1_.get_position();
                int32_t encoder2_position = encoder2_.get_position();

                // Copy encoder positions to buffer
                memcpy(&encoder_positions_buffer[0], &encoder1_position, sizeof(encoder1_position));
                memcpy(&encoder_positions_buffer[4], &encoder2_position, sizeof(encoder2_position));

                // Write encoder positions to USB
                tud_cdc_write(encoder_positions_buffer, ENCODER_DATA_BUFFER_LENGTH);

                // Flush write buffer
                tud_cdc_write_flush();

                break;
            }

            case RETURN_IMU_DATA_BYTE:
            {
                // Initialize buffer for IMU data
                uint8_t imu_data_buffer[IMU_DATA_BUFFER_LENGTH];
                
                // Read IMU data registers (0x38 - 0x48)
                // Data is 2 bytes per DOF, little endian
                // Indicies:
                // 0 - 5 : Accelerometer (X, Y, Z)
                // 6 - 7 : Temperature
                // 8 - 13 : Gyroscope (X, Y, Z)
                imu_.readIMU(0x3B, imu_data_buffer, IMU_DATA_BUFFER_LENGTH);
                
                // Write IMU data buffer to USB
                tud_cdc_write(imu_data_buffer, IMU_DATA_BUFFER_LENGTH);
                
                // Flush write buffer
                tud_cdc_write_flush();
                
                break;
            }
        }
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
