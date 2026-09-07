#include "Utils.h"
#include <chrono>
#include <ctime>
#include <random>
#include <iostream>
#include <termios.h>
#include <unistd.h>

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&in_time_t));
    return std::string(buf);
}

std::string generateSalt(size_t length) {
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, chars.size() - 1);
    std::string salt;
    for (size_t i = 0; i < length; ++i) {
        salt += chars[dist(generator)];
    }
    return salt;
}

size_t computeSaltedHash(const std::string& pin, const std::string& salt) {
    return std::hash<std::string>{}(pin + salt);
}

std::string getMaskedInput(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char ch;
    while ((ch = std::cin.get()) != '\n' && ch != EOF) {
        if (ch == 127 || ch == '\b') { // Backspace
            if (!input.empty()) {
                input.pop_back();
                std::cout << "\b \b";
            }
        } else {
            input.push_back(ch);
            std::cout << '*';
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    std::cout << "\n";
    return input;
}