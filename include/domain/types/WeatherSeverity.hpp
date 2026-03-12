#pragma once
#include <string>

enum class WeatherSeverity
{
    OK,
    DEGRADED,
    SEVERE,
    EXTREME
};

std::string weatherSeverityToString(WeatherSeverity severity);