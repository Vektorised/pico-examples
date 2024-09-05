#include "encoder.hpp"

int main() {

    stdio_init_all(); // Initialize all standard IO, including USB

    // Define the specific byte to wait for
    uint8_t receivedByte;

    // Wait for the specific byte
    printf("Waiting for byte: 0x%X\n", INITIALIZE_INTERRUPTS_BYTE);
    do {
        receivedByte = getchar(); // Blocking read
        printf("Received: 0x%X, waiting for 0x%X\n", receivedByte, INITIALIZE_INTERRUPTS_BYTE); // Echo back the received byte
    } while (receivedByte != INITIALIZE_INTERRUPTS_BYTE);

    // Signal byte received and initialize the encoders
    printf("Byte 0x%X received. Initializing encoders...\n", receivedByte);

    // Time benchmark start
    auto startTime = std::chrono::high_resolution_clock::now();

    // Initialize the encoders
    init_encoders();

    // Set interrupts as enabled
    interrupts_enabled = true;

    // Signal pins are initialized
    printf("Pins initialized\n");

    // Time benchmark end
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    printf("Initialization took %ld nanoseconds\n", duration.count());

    // Prompt user for data return
    printf("Waiting for control words.\n");

    while (true) {
        receivedByte = getchar(); // Blocking read

        // printf("Received: 0x%X\n", receivedByte); // Echo back the received byte

        switch (receivedByte)
        {
            // 'E'
            case RETURN_ENCODER_BYTE: {
                
                printf("Encoder 1 Position: %ld, Encoder 2 Position: %ld\n", encoder1_position, encoder2_position);
                break;
            }
            
            // 'Z'
            case RESET_ENCODER_POS_BYTE: {
                encoder1_position = 0;
                encoder2_position = 0;
                printf("Encoder positions reset\n");
                break;
            }

            // 'T'
            case TOGGLE_INTERRUPTS_BYTE:
                if (interrupts_enabled) {
                    gpio_set_irq_enabled(ENCODER1_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false);
                    gpio_set_irq_enabled(ENCODER1_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false);
                    gpio_set_irq_enabled(ENCODER2_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false);
                    gpio_set_irq_enabled(ENCODER2_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false);
                    printf("Interrupts disabled\n");
                    interrupts_enabled = false;
                } else {
                    gpio_set_irq_enabled(ENCODER1_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);    
                    gpio_set_irq_enabled(ENCODER1_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
                    gpio_set_irq_enabled(ENCODER2_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
                    gpio_set_irq_enabled(ENCODER2_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
                    printf("Interrupts enabled\n");
                    interrupts_enabled = true;
                }
                break;

            // 'R'
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

            // 'A'
            case ACTIVE_REPORT_BYTE:
                if (active_reporting)
                {
                    printf("Active reporting disabled\n");
                    active_reporting = false;
                }
                else
                {
                    printf("Active reporting enabled\n");
                    active_reporting = true;
                }
                break;

            // 'C'
            case CLEAR_SCREEN_BYTE:
                printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
                break;

            default:
                break;
        }

        // tight_loop_contents(); // Prevent the main loop from exiting
    }

    return 0;
}