#pragma once

#include <cstdint>
#include <string>
#include "domain/types/WeatherSeverity.hpp"

class WeatherCell {
public:
    WeatherCell(std::string sectorId, std::int64_t timestamp, WeatherSeverity severity); 

private:
    std::string sectorId_;
    std::int64_t timestamp_;
    WeatherSeverity severity_;
};