#pragma once

#include "domain/types/SectorState.hpp"
#include "domain/types/WeatherSeverity.hpp"
#include <cstdint>
#include <string>

class SectorSummary
{
  public:
    SectorSummary(int sectorId, std::string timestamp, int trackCount,
                  WeatherSeverity weatherSeverity, double weatherFactor, double baseCapacity,
                  SectorState state);

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
    void updateTime(std::string timestamp);
    void updateWeather(WeatherSeverity weatherSeverity, double weatherFactor);

  private:
    int sectorId_;
    std::string timestamp_;
    int trackCount_ = 0;
    WeatherSeverity weatherSeverity_;
    double weatherFactor_;
    double baseCapacity_;
    // effective (sett inn)
    SectorState state_;
};