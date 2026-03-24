#pragma once

#include "domain/types/SectorState.hpp"
#include "domain/types/WeatherSeverity.hpp"
#include <cstdint>
#include <string>
#include <vector>

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
    WeatherSeverity getWeatherSeverity() const;
    double getWeatherFactor() const;
    std::vector<std::string> getIcao24List() const;

    // helpers
    void increaseLocalAircraftCount();
    void decreaseLocalAircraftCount();
    void updateState();
    void updateTime(std::string timestamp);
    void updateWeather(WeatherSeverity weatherSeverity, double weatherFactor);
    bool isAtRisk();
    bool isCongested();
    void addIcao(const std::string &icao);
    void removeIcao(const std::string &icao);

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
    std::vector<std::string> icao24List_;
};