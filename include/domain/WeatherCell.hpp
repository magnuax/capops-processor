#pragma once

#include "domain/types/WeatherSeverity.hpp"
#include <cstdint>
#include <string>

class WeatherCell
{
  public:
    WeatherCell(int sectorId, std::int64_t timestamp, WeatherSeverity severity);
    int getSectorId() const; 
    std::int64_t getTimestamp() const; 
    WeatherSeverity getWeatherSeverity() const; 

  private:
    int sectorId_;
    std::int64_t timestamp_;
    WeatherSeverity severity_;
};