#pragma once

#include "domain/types/WeatherSeverity.hpp"
#include <cstdint>
#include <string>

class WeatherCell
{
  public:
    WeatherCell(int sectorId, std::string timestamp, WeatherSeverity severity);
    int getSectorId() const;
    std::string getTimestamp() const;
    WeatherSeverity getWeatherSeverity() const;

  private:
    int sectorId_;
    std::string timestamp_;
    WeatherSeverity severity_;
};