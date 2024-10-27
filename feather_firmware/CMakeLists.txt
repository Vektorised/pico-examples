# CMakeLists.txt
# Carson Powers
# CMakeList file for building feather firmware meant for the Adafruit Feather RP2040

set(CMAKE_CXX_STANDARD 17)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(feather_firmware
    main.cpp
    feather.cpp
    encoder.cpp
    imu.cpp
)
    
# Link necessary libraries
target_link_libraries(feather_firmware pico_stdlib hardware_gpio hardware_irq hardware_uart hardware_timer hardware_i2c pico_multicore)

# Might need to alter given serialized USB communications are needed
# enable usb / disable uart
pico_enable_stdio_usb(feather_firmware 1)
pico_enable_stdio_uart(feather_firmware 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(feather_firmware)

# add url via pico_set_program_url
example_auto_set_url(feather_firmware)
