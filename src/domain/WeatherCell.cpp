#include "domain/WeatherCell.hpp"

WeatherCell::WeatherCell(int sectorId, std::int64_t timestamp, WeatherSeverity severity)
    : sectorId_(std::move(sectorId)), timestamp_(timestamp), severity_(severity)
{
}

int WeatherCell::getSectorId() const{
    return sectorId_; 
}

std::int64_t WeatherCell::getTimestamp() const{
    return timestamp_; 
}


WeatherSeverity WeatherCell::getWeatherSeverity() const{
    return severity_; 
}