cmake_minimum_required(VERSION 3.15)

project(AUREQ_Tests LANGUAGES CXX)

add_executable(AUREQ_FilterTests
    main.cpp
    BiquadCoefficientTests.cpp
    EQBandProcessingTests.cpp
    FilterStabilityTests.cpp
    
    # Compile the DSP classes directly to bypass linker target export complexities
    ../plugins/01_EQ/DSP/BiquadFilter.cpp
    ../plugins/01_EQ/DSP/ParameterSmoother.cpp
    ../plugins/01_EQ/DSP/EQBand.cpp
)

# Include paths for source headers
target_include_directories(AUREQ_FilterTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/01_EQ/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/01_EQ/DSP
)

# Link directly to the required JUCE modules (no need to link AUREQ_SharedCode)
target_link_libraries(AUREQ_FilterTests PRIVATE
    juce::juce_dsp
    juce::juce_audio_basics
)

# Enable C++17
target_compile_features(AUREQ_FilterTests PRIVATE cxx_std_17)

# Put test runner binary in build/tests
set_target_properties(AUREQ_FilterTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
)
