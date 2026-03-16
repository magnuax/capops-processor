
#include "sources/WeatherSourceSimulated.hpp"

WeatherSourceSimulated::WeatherSourceSimulated(WeatherSimulator &simulator)
    : simulator_(simulator)
{
}

WeatherSeverity WeatherSourceSimulated::getWeatherSeverity(Position coordinates)
{
    double normalizedSeverity = simulator_.getNormalizedSeverity(coordinates);

    WeatherSeverity severity;

    if (normalizedSeverity < 0.25)
        severity = WeatherSeverity::OK;

    else if (normalizedSeverity < 0.5)
        severity = WeatherSeverity::DEGRADED;

    else if (normalizedSeverity < 0.75)
        severity = WeatherSeverity::SEVERE;

    else
        severity = WeatherSeverity::EXTREME;

    return severity;

}