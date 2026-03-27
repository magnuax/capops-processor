
#include "sources/WeatherSourceSimulated.hpp"


WeatherSourceSimulated::WeatherSourceSimulated(WeatherSimulator &simulator) : simulator_(simulator)
{
}

WeatherSeverity WeatherSourceSimulated::getWeatherSeverity(Position coordinates)
{
    double normalizedSeverity = simulator_.getNormalizedSeverity(coordinates);
    const auto &weatherLevels = simulator_.getWeatherLevels();

    for (const auto &[severity, threshold] : weatherLevels)
    {
        if (normalizedSeverity <= threshold)
        {
            return severity;
        }
    }

    return weatherLevels.back().first;
}