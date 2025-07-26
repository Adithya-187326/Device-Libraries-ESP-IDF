# ADS1115 ADC Driver Component for ESP-IDF
# CERT-C compliant implementation with comprehensive error handling

idf_component_register(
    SRCS 
        "src/ads1115.c"
    INCLUDE_DIRS
        "include"
        "."  # For backward compatibility
    REQUIRES
        log
        esp_driver_i2c
        esp_driver_gpio
        esp_common
        freertos
    PRIV_REQUIRES
        # Add any private dependencies here if needed
)

# Component version information
set(COMPONENT_VERSION "1.1.0")

# Compiler flags for CERT-C compliance
target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wall
    -Wextra  
    -Werror
    -Wstrict-prototypes
    -Wmissing-prototypes
    -Wcast-qual
    -Wpointer-arith
    -Wunused-parameter
    -pedantic
)

# Add preprocessor definitions
target_compile_definitions(${COMPONENT_LIB} PRIVATE
    ADS1115_VERSION_STRING="${COMPONENT_VERSION}"
)

# Documentation target (optional)
# Requires Doxygen to be installed
find_program(DOXYGEN_EXECUTABLE doxygen)
if(DOXYGEN_EXECUTABLE)
    add_custom_target(ads1115_docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating ADS1115 documentation with Doxygen"
        VERBATIM
    )
endif()