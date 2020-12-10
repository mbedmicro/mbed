# Copyright (c) 2020 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Generate a file containing compile definitions
function(mbed_generate_options_for_linker target definitions_file)
    set(_compile_definitions
        "$<TARGET_PROPERTY:${target},COMPILE_DEFINITIONS>"
    )

    # Remove macro definitions that contain spaces as the lack of escape sequences and quotation marks
    # in the macro when retrieved using generator expressions causes linker errors.
    # This includes string macros, array macros, and macros with operations.
    # TODO CMake: Add escape sequences and quotation marks where necessary instead of removing these macros.
    set(_compile_definitions
       "$<FILTER:${_compile_definitions},EXCLUDE, +>"
    )

    # Append -D to all macros as we pass these as response file to cxx compiler
    set(_compile_definitions
        "$<$<BOOL:${_compile_definitions}>:-D$<JOIN:${_compile_definitions}, -D>>"
    )
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/compile_time_defs.txt" CONTENT "${_compile_definitions}\n")
    set(${definitions_file} @${CMAKE_BINARY_DIR}/compile_time_defs.txt PARENT_SCOPE)
endfunction()
# Set the system processor depending on the CPU core type
if (MBED_CPU_CORE STREQUAL Cortex-A9)
    set(CMAKE_SYSTEM_PROCESSOR cortex-a9)
elseif (MBED_CPU_CORE STREQUAL Cortex-M0+)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m0plus)
elseif (MBED_CPU_CORE STREQUAL Cortex-M0)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m0)
elseif (MBED_CPU_CORE STREQUAL Cortex-M1)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m1)
elseif (MBED_CPU_CORE STREQUAL Cortex-M23-NS)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m23)
elseif (MBED_CPU_CORE STREQUAL Cortex-M23)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m23)
elseif (MBED_CPU_CORE STREQUAL Cortex-M3)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m3)
elseif (MBED_CPU_CORE STREQUAL Cortex-M33-NS)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m33)
elseif (MBED_CPU_CORE STREQUAL Cortex-M33)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m33)
elseif (MBED_CPU_CORE STREQUAL Cortex-M33F-NS)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m33)
elseif (MBED_CPU_CORE STREQUAL Cortex-M33F)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m33)
elseif (MBED_CPU_CORE STREQUAL Cortex-M33FE-NS)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m33)
elseif (MBED_CPU_CORE STREQUAL Cortex-M33FE)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m33)
elseif (MBED_CPU_CORE STREQUAL Cortex-M4)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
elseif (MBED_CPU_CORE STREQUAL Cortex-M4F)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
elseif (MBED_CPU_CORE STREQUAL Cortex-M7)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m7)
elseif (MBED_CPU_CORE STREQUAL Cortex-M7F)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m7)
elseif (MBED_CPU_CORE STREQUAL Cortex-M7FD)
    set(CMAKE_SYSTEM_PROCESSOR cortex-m7)
endif()

# Compiler setup
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

# Clear toolchains options for all languages as Mbed OS uses
# different initialisation options (such as for optimization and debug symbols)
set(CMAKE_C_FLAGS_DEBUG "" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_RELEASE "" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_RELWITHDEBINFO "" CACHE STRING "" FORCE)

set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "" CACHE STRING "" FORCE)

set(CMAKE_ASM_FLAGS_DEBUG "" CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS_RELEASE "" CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS_RELWITHDEBINFO "" CACHE STRING "" FORCE)

# Use response files always
set(CMAKE_ASM_USE_RESPONSE_FILE_FOR_INCLUDES 1)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES 1)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES 1)

set(CMAKE_ASM_USE_RESPONSE_FILE_FOR_OBJECTS 1)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 1)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 1)

set(CMAKE_ASM_USE_RESPONSE_FILE_FOR_LIBRARIES 1)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_LIBRARIES 1)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_LIBRARIES 1)