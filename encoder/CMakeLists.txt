add_executable(encoder
    encoder.cpp
    )

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
    
# Link necessary libraries
target_link_libraries(encoder pico_stdlib hardware_gpio hardware_irq hardware_uart hardware_timer)

pico_enable_stdio_usb(encoder 1)
pico_enable_stdio_uart(encoder 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(encoder)

# add url via pico_set_program_url
example_auto_set_url(encoder)