set(CMAKE_CXX_STANDARD 17)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(firmware
    firmware.cpp
    encoder.cpp
    )
    
# Link necessary libraries
target_link_libraries(firmware pico_stdlib hardware_gpio hardware_irq hardware_uart hardware_timer pico_multicore)

# enable usb / disable uart
pico_enable_stdio_usb(firmware 1)
pico_enable_stdio_uart(firmware 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(firmware)

# add url via pico_set_program_url
example_auto_set_url(firmware)