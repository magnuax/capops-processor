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

    //getters
    int getTrackCount(); 
    double getBaseCapacity();
    SectorState getState(); 
    double getEffectiveCapacity();

    //helpers
    void increaseTrackCount(); 
    void updateState(SectorState newState); 

  private:
    std::string sectorId_;
    std::int64_t timestamp_;
    int trackCount_ = 0; 
    WeatherSeverity weatherSeverity_;
    double weatherFactor_;
    double baseCapacity_;
    double effectiveCapacity_;
    SectorState state_;
};