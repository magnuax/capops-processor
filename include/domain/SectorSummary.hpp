#pragma once

#include "domain/types/SectorState.hpp"
#include "domain/types/WeatherSeverity.hpp"
#include <cstdint>
#include <string>

struct SectorSummaryPayload
{
    int sectorId;
    int row;
    int column;
    std::string weatherSeverity;
    std::string riskSeverity;
    int localAircraftCount;
    int localAircraftBaseCapacity;
    int localAircraftEffectiveCapacity;
};

class SectorSummary
{
  public:
    SectorSummary(int sectorId, int row, int column, std::string timestamp, int localAircraftCount,
                  WeatherSeverity weatherSeverity, double weatherFactor,
                  double localAircraftBaseCapacity, SectorState riskSeverity);

    // getters
    int getSectorId() const;
    int getRow() const;
    int getColumn() const;
    int getLocalAircraftCount() const;
    double getBaseCapacity() const;
    SectorState getState() const;
    double getEffectiveCapacity() const;

    // helpers
    void increaseLocalAircraftCount();
    void decreaseLocalAircraftCount();
    void updateState();
    void updateTime(std::string timestamp);
    void updateWeather(WeatherSeverity weatherSeverity, double weatherFactor);
    bool isAtRisk();
    bool isCongested();

    SectorSummaryPayload toPayload() const;

  private:
    int sectorId_;
    int row_;
    int column_;
    std::string timestamp_;
    int localAircraftCount_ = 0;
    WeatherSeverity weatherSeverity_;
    double weatherFactor_;
    double localAircraftBaseCapacity_;
    SectorState riskSeverity_;
};