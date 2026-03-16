#pragma once
#include "domain/types/WeatherSeverity.hpp"
#include "domain/types/Position.hpp"
#include <optional>
#include <vector>

class IWeatherSource
{
  public:
    virtual ~IWeatherSource() = default;
  
    virtual WeatherSeverity getWeatherSeverity(Position coordinates) = 0;
};