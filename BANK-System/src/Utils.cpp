#include "Utils.h"
#include <chrono>
#include <ctime>

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    time_t in_time_t = std::chrono::system_clock::to_time_t(now);
    tm buf;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&buf, &in_time_t);
#else
    localtime_r(&in_time_t, &buf);
#endif
    char str[20];
    strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", &buf);
    return std::string(str);
}