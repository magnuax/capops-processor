#pragma once

#include <atomic>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "config/Config.hpp"
#include "domain/types/WeatherSeverity.hpp"
#include "domain/types/Position.hpp"

class WeatherSimulator
{
  public:
    using WeatherPattern = std::function<double(double)>;
    using SectorID = std::string;

    WeatherSimulator(GridConfig config);

    void start();

    void stop();

    void tick(double timeStep);

    void setWeatherPattern(int row, int col, double normalizedSeverity);
    
    void setWeatherPattern(int row, int col, WeatherPattern pattern);

    static WeatherPattern constant(double normalizedSeverity);

    double getNormalizedSeverity(Position coordinates);

    double getNormalizedSeverity(int row, int col);

  private:
    void initializeSectors();

    SectorID getId(int row, int col);

    GridConfig gridConfig_;
    std::vector<SectorID> sectorIds_;
    std::unordered_map<SectorID, double> normalizedSeverities_;
    std::unordered_map<SectorID, WeatherPattern> weatherPatterns_;

    std::atomic<bool> isRunning_ = false;
    double currentTime_;
    double timeStep_;
};

/*
Usage example:

#include <cmath>

GridConfig myConfig;
myConfig.rows = 1
myConfig.cols = 1

mySim = WeatherSimulator(myConfig)

myWeather = [](double time) { std::sin(time) + std::sin(0.5*time)}

mySim.setWeatherPattern(0, 0, myWeather);

**/