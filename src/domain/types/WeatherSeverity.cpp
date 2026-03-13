#include "domain/types/WeatherSeverity.hpp"

std::string weatherSeverityToString(WeatherSeverity severity)
{
    switch (severity)
    {
    case WeatherSeverity::OK:
        return "OK";
    case WeatherSeverity::DEGRADED:
        return "DEGRADED";
    case WeatherSeverity::SEVERE:
        return "SEVERE";
    case WeatherSeverity::EXTREME:
        return "EXTREME";
    }

    return "UNKNOWN";
}
