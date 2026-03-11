#pragma once

#include "domain/types/SectorState.hpp"
#include "domain/types/WeatherSeverity.hpp"
#include <cstdint>
#include <string>

class SectorSummary
{
  public:
    SectorSummary(int sectorId, std::int64_t timestamp, int trackCount,
                  WeatherSeverity weatherSeverity, double weatherFactor, double baseCapacity,
                  double effectiveCapacity, SectorState state);

    // getters
    int getSectorId() const; 
    int getTrackCount() const;
    double getBaseCapacity() const;
    SectorState getState() const;
    double getEffectiveCapacity() const;
    bool isAtRisk();
    bool isCongested();

    // helpers
    void increaseTrackCount();
    void decreaseTrackCount();
    void updateState();
    void updateTime(std::int64_t timestamp); 

  private:
    int sectorId_;
    std::int64_t timestamp_;
    int trackCount_ = 0;
    WeatherSeverity weatherSeverity_;
    double weatherFactor_;
    double baseCapacity_;
    double effectiveCapacity_;
    SectorState state_;
};