#pragma once

#include "domain/types/SectorState.hpp"
#include "domain/types/WeatherSeverity.hpp"
#include <cstdint>
#include <string>

class SectorSummary
{
  public:
    SectorSummary(std::string sectorId, std::int64_t timestamp, int trackCount,
                  WeatherSeverity weatherSeverity, double weatherFactor, double baseCapacity,
                  double effectiveCapacity, SectorState state);

  private:
    std::string sectorId_;
    std::int64_t timestamp_;
    int trackCount_;
    WeatherSeverity weatherSeverity_;
    double weatherFactor_;
    double baseCapacity_;
    double effectiveCapacity_;
    SectorState state_;
};