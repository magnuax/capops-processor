#pragma once
#include "config/Config.hpp"
#include "domain/RiskEvent.hpp"
#include "domain/Sector.hpp"
#include "domain/SectorSummary.hpp"
#include "domain/Track.hpp"
#include "domain/WeatherCell.hpp"
#include "domain/types/ProcessingResult.hpp"
#include "domain/types/SectorState.hpp"
#include "compute/Grid.hpp"
#include <deque>
#include <string>
#include <unordered_map>

class ComputeData
{
  public:
    ComputeData(const Configuration& config);
    void handleTrackUpdate(const Track &track);
    void handleWeatherUpdate(const WeatherCell &weatherCell);

    // helpers
    int determineSector(const Position &position);
    void evaluateSectorState(int sectorId, std::int64_t timestamp);
    bool isAtRisk(int sectorId);

  private:
    std::unordered_map<std::string, Track> activeTracksByIcao_;
    std::unordered_map<int, WeatherCell> weatherBySectorId_;
    std::unordered_map<int, SectorSummary> sectorSummariesById_;
    std::unordered_map<int, Sector> sectorsById_;
    std::deque<RiskEvent> riskEvents_;
    ProcessingResult result_;
    Grid grid_;
    double defaultBaseCapacity_; 

    void initializeSectors();
};