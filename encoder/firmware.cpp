// firmware.cpp
// Carson Powers
// Main firmware funcitons for the Adafruit Feather RP2040 board on the AHSR robot

// 09/12/2024 Carson Powers: Add drivers for IMU operations
// might need to undermine motor controller board and bypass serial
// console on the on board ATmega32U4

#include "encoder.hpp"

#include <iostream> // IO
#include <string> // String

// Pico libraries
#include "pico/platform.h"

// TinyUSB for USB communication
#include "tusb.h"

void process_usb_communications()
{
    if (tud_cdc_available())
    {
        uint8_t receivedByte = tud_cdc_read_char(); // Blocking read

        // Decipher recieved byte
        switch (receivedByte)
        {
            // 'E' returns both encoder positions
            case RETURN_ENCODER_BYTE: 
            {                
                // Little endian buffer
                uint8_t encoder_positions[8];

                // Copy encoder positions (need to cast to void* for memcpy)
                memcpy(&encoder_positions[0], (const void *)&encoder1_position, sizeof(encoder1_position));
                memcpy(&encoder_positions[4], (const void *)&encoder2_position, sizeof(encoder2_position));

                // Write to USB
                tud_cdc_write(encoder_positions, sizeof(encoder_positions));
                
                // Flush buffer
                tud_cdc_write_flush();
                
                // printf("Encoder 1 Position: %ld, Encoder 2 Position: %ld\n", encoder1_position, encoder2_position);
                break;
            }

            // '1' returns encoder 1 position
            case RETURN_ENCODER_1_BYTE: 
            {
                // Little endian buffer
                uint8_t encoder_positions[4];

                // Copy encoder positions (need to cast to void* for memcpy)
                memcpy(&encoder_positions[0], (const void *)&encoder1_position, sizeof(encoder1_position));

                // Write to USB
                tud_cdc_write(encoder_positions, sizeof(encoder_positions));
                
                // Flush buffer
                tud_cdc_write_flush();
                
                // printf("Encoder 1 Position: %ld\n", encoder1_position);
                break;
            }

            // '2' returns encoder 2 position
            case RETURN_ENCODER_2_BYTE:
            {    
                // Little endian buffer
                uint8_t encoder_positions[4];

                // Copy encoder positions (need to cast to void* for memcpy)
                memcpy(&encoder_positions[0], (const void *)&encoder2_position, sizeof(encoder2_position));

                // Write to USB
                tud_cdc_write(encoder_positions, sizeof(encoder_positions));
                
                // Flush buffer
                tud_cdc_write_flush();
                
                // printf("Encoder 2 Position: %ld\n", encoder2_position);
                break;
            }

            // 'Z' resets encoder positions
            case RESET_ENCODER_POS_BYTE:
            {
                encoder1_position = 0;
                encoder2_position = 0;

                // printf("Encoder positions reset\n");
                break;
            }

            // 'T' toggles interrupts
            case TOGGLE_INTERRUPTS_BYTE:
            {
                if (interrupts_enabled) 
                {
                    // Disable interrupts globally
                    irq_set_mask_enabled(0xFFFFFFFF, false);

                    // printf("Interrupts disabled\n");
                    interrupts_enabled = false;
                } 
                else 
                {
                    // Enable interrupts globally
                    irq_set_mask_enabled(0xFFFFFFFF, true);

                    // printf("Interrupts enabled\n");
                    interrupts_enabled = true;
                }
                break;
            }

            // 'R' returns RPM
            case RETURN_RPM_BYTE: 
            {
                uint32_t current_time = to_ms_since_boot(get_absolute_time());
                uint32_t time_interval = current_time - last_time;

                // Calculate RPMs for each encoder
                float rpm1 = calculate_rpm(encoder1_position, encoder1_last_position, time_interval);
                encoder1_last_position = encoder1_position;

                current_time = to_ms_since_boot(get_absolute_time());
                time_interval = current_time - last_time;

                float rpm2 = calculate_rpm(encoder2_position, encoder2_last_position, time_interval);
                encoder2_last_position = encoder2_position;

                // Little endian buffer
                uint8_t motor_rpms[8];

                // Copy encoder positions (need to cast to void* for memcpy)
                memcpy(&motor_rpms[0], (const void *)&rpm1, sizeof(rpm1));
                memcpy(&motor_rpms[4], (const void *)&rpm2, sizeof(rpm2));

                // Write to USB
                tud_cdc_write(motor_rpms, sizeof(motor_rpms));
                
                // Flush buffer
                tud_cdc_write_flush();

                // Update last_time and reset encoder positions
                last_time = current_time;
                
                // printf("Encoder 1 RPM: %.2f, Encoder 2 RPM: %.2f\n", rpm1, rpm2);
                break;
            }

            // 'A' toggles active reporting
            case ACTIVE_REPORT_BYTE:
            {
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
            }

            // 'C' clears screen
            case CLEAR_SCREEN_BYTE:
            {
                printf("\033[2J\033[1;1H"); // Wizard magic
                break;
            }

            // 'O' injects offset to encoder positions
            case INJECT_OFFSET_BYTE:
            {
                std::string offsetInput = "";
                int32_t offset = 0;

                if (receivedByte == '1') 
                {
                    encoder1_position += offset;
                } 
                else if (receivedByte == '2') 
                {
                    encoder2_position += offset;
                }
                break;
            }

            default:
                break;
        }
    }

    return;
}

int main() {

    stdio_init_all(); // Initialize all standard IO, including USB
    tusb_init(); // Initialize TinyUSB

    // Sets up systick counter for benchmarking
    setup_systick();    
    check_systick_counting();


    // -------------------- Wait to initialize -------------------- //

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

    // Initialize the encoders
    init_encoders();

    // Set interrupts as enabled
    interrupts_enabled = true;

    // Signal pins are initialized
    printf("Pins initialized\n");

    // -------------------- Initialized -------------------- //

    // Prompt user for data return
    printf("Waiting for control words.\n");

    while (true) {
        process_usb_communications(); // Process USB communications
        tight_loop_contents(); // Prevent the main loop from exiting
    }

    return 0;
}
