cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb -Os -ffunction-sections -fdata-sections")
# Remove linker script path from global flags
set(CMAKE_EXE_LINKER_FLAGS "") # Or remove this line if no other global flags are needed

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME LED_Blink)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME} C ASM)
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link STM32_Drivers (HAL sources) to your main target
target_link_libraries(${CMAKE_PROJECT_NAME} STM32_Drivers)

# Additional linker flags might be needed for I2C support
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    USE_HAL_DRIVER
    STM32F103xB  # Adjust this to match your specific MCU
)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
# Adjust GLOB to include Core/Src or add files explicitly
file(GLOB SOURCES
    "src/*.c"
    "Core/Src/*.c"
)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${SOURCES})

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    inc
    Core/Inc
    Drivers/STM32F1xx_HAL_Driver/Inc  # Make sure HAL driver headers are included
    Drivers/CMSIS/Device/ST/STM32F1xx/Include  # CMSIS device-specific headers
    Drivers/CMSIS/Include  # CMSIS core headers
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linker options specifically for this target
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    # Remove the linker script specification from here:
    # "-T${CMAKE_CURRENT_SOURCE_DIR}/STM32F103XX_FLASH.ld"
    # Keep other linker flags:
    "-Wl,--gc-sections"
)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    # Use the generator expression $<TARGET_FILE:...> to get the correct executable path
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
    COMMENT "Generating ${CMAKE_PROJECT_NAME}.bin"
)

# Fix multiple definition errors by excluding system_stm32f1xx.c from one location
set_source_files_properties(
  ${CMAKE_SOURCE_DIR}/Core/Src/system_stm32f1xx.c
  PROPERTIES
  HEADER_FILE_ONLY TRUE  # This prevents compilation of this file in the main target
)
