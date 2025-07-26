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

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
file(GLOB SOURCES "src/*.c")
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${SOURCES})

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    inc
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# --- IMPORTANT ---
# The linker script file 'STM32F103C8Tx_FLASH.ld' was not found in the project.
# Please obtain the correct linker script for your target (STM32F103C8Tx)
# and place it in the root directory of this project:
# /home/freeman.xiong/Documents/github/xiongchenyu6/LED_Blink/
# The path below assumes the file is in the root directory.
# Adjust the path if you place it elsewhere.
# ---
# Linker script is likely handled by cmake/stm32cubemx/CMakeLists.txt
# Remove explicit -T flag from here to avoid duplication.

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
