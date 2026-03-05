#include "domain/WeatherCell.hpp"

WeatherCell::WeatherCell(std::string sectorId, std::int64_t timestamp, WeatherSeverity severity) 
    : sectorId_(std::move(sectorId)), timestamp_(timestamp), severity_(severity) {}

    