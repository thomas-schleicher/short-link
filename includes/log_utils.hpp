#ifndef LOG_UTILS_HPP
#define LOG_UTILS_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <unordered_map>

// Define log levels
enum class LogLevel {
    INFO,
    ERROR,
    BADREQUEST
};

// Function to get a color based on file name
inline const char* getColorForFile(const std::string& file_name) {
    // Define color codes
    const char* colors[] = {
        "\033[34m", // Blue
        "\033[32m", // Green
        "\033[36m", // Cyan
        "\033[35m", // Magenta
        "\033[33m"  // Yellow
    };

    // Hash the file name to determine the color
    std::hash<std::string> hasher;
    size_t hash = hasher(file_name);
    return colors[hash % (sizeof(colors) / sizeof(colors[0]))];
}

// Logging function
inline void log(const LogLevel level, const std::string& message, const std::string& file_path) {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    const std::tm tm = *std::localtime(&time);

    const auto color_reset = "\033[0m";
    const auto color_error = "\033[31m";

    // Extract file name from full path
    std::string file_name = std::filesystem::path(file_path).filename().string();

    // Determine color based on log level
    const char* color = level == LogLevel::ERROR || level == LogLevel::BADREQUEST ? color_error : getColorForFile(file_name);

    // Print log
    std::cout << color
              << "[" << (level == LogLevel::INFO ? "INFO" : "ERROR") << "] "
              << std::put_time(&tm, "%H:%M:%S")  // Time only
              << " [" << file_name << "] "
              << message
              << color_reset << std::endl;
}

// Helper macro to automatically pass the file name
#define LOG(level, message) log(level, message, __FILE__)

#endif // LOG_UTILS_HPP
