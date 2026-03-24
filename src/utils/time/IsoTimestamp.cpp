#include "utils/time/IsoTimestamp.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string createIsoTimestamp()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
    const auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::tm utcTime{};
#if defined(_WIN32)
    gmtime_s(&utcTime, &timeNow);
#else
    utcTime = *std::gmtime(&timeNow);
#endif

    std::ostringstream oss;
    oss << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%S") << '.' << std::setfill('0') << std::setw(3)
        << ms.count() << 'Z';
    return oss.str();
}