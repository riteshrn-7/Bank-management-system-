#pragma once
#include <string>

std::string getCurrentTimestamp();
std::string generateSalt(size_t length = 8);
size_t computeSaltedHash(const std::string& pin, const std::string& salt);
std::string getMaskedInput(const std::string& prompt);