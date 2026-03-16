#include "utils/time/IsoTimestamp.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string createIsoTimestamp()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t timeNow = std::chrono::system_clock::to_time_t(now);

    std::tm utcTime{};
#if defined(_WIN32)
    gmtime_s(&utcTime, &timeNow);
#else
    utcTime = *std::gmtime(&timeNow);
#endif

    std::ostringstream oss;
    oss << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}