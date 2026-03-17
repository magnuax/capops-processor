#pragma once
#include "domain/types/Position.hpp"
#include "domain/types/WeatherSeverity.hpp"
#include <optional>
#include <vector>

class IWeatherSource
{
  public:
    virtual ~IWeatherSource() = default;

    virtual WeatherSeverity getWeatherSeverity(Position coordinates) = 0;
};