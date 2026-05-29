#include <iostream>
#include <exception>

// Test suites declarations
void runBiquadCoefficientTests();
void runEQBandProcessingTests();
void runFilterStabilityTests();

int main()
{
    std::cout << "\n";
    std::cout << "========================================================\n";
    std::cout << "         AUREQ - DSP FILTER VALIDATION TESTS            \n";
    std::cout << "========================================================\n\n";

    int failedSuites = 0;

    // Run Biquad Coefficient Tests
    try
    {
        std::cout << "[SUITE] Running Biquad Coefficient Tests...\n";
        runBiquadCoefficientTests();
        std::cout << "[PASS] Biquad Coefficient Tests completed successfully.\n\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "[FAIL] Biquad Coefficient Tests failed: " << e.what() << "\n\n";
        failedSuites++;
    }
    catch (...)
    {
        std::cerr << "[FAIL] Biquad Coefficient Tests failed with unknown error.\n\n";
        failedSuites++;
    }

    // Run EQBand Processing Tests
    try
    {
        std::cout << "[SUITE] Running EQBand Block Processing Tests...\n";
        runEQBandProcessingTests();
        std::cout << "[PASS] EQBand Block Processing Tests completed successfully.\n\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "[FAIL] EQBand Block Processing Tests failed: " << e.what() << "\n\n";
        failedSuites++;
    }
    catch (...)
    {
        std::cerr << "[FAIL] EQBand Block Processing Tests failed with unknown error.\n\n";
        failedSuites++;
    }

    // Run Filter Stability Tests
    try
    {
        std::cout << "[SUITE] Running Filter Stability and Safety Tests...\n";
        runFilterStabilityTests();
        std::cout << "[PASS] Filter Stability and Safety Tests completed successfully.\n\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "[FAIL] Filter Stability and Safety Tests failed: " << e.what() << "\n\n";
        failedSuites++;
    }
    catch (...)
    {
        std::cerr << "[FAIL] Filter Stability and Safety Tests failed with unknown error.\n\n";
        failedSuites++;
    }

    std::cout << "========================================================\n";
    if (failedSuites == 0)
    {
        std::cout << "      SUCCESS: All AUREQ DSP Filter Tests Passed!      \n";
        std::cout << "========================================================\n\n";
        return 0;
    }
    else
    {
        std::cout << "      FAILURE: " << failedSuites << " Test Suite(s) Failed!      \n";
        std::cout << "========================================================\n\n";
        return 1;
    }
}
