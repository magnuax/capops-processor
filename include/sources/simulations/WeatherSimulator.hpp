#pragma once

#include <atomic>
#include <functional>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "config/Config.hpp"
#include "domain/types/Position.hpp"
#include "domain/types/WeatherSeverity.hpp"

class WeatherSimulator
{
  public:
    using WeatherPattern = std::function<double(double)>;
    using SectorID = std::string;

    WeatherSimulator(GridConfig config,
                     const std::vector<std::pair<WeatherSeverity, double>> &weatherLevels);

    void start();

    void stop();

    void tick(double timeStep);

    void setRandomWeatherPatterns();

    void setHorizontalWavePattern(double frequency, double speed);

    void setWeatherPattern(int row, int col, double normalizedSeverity);

    void setWeatherPattern(int row, int col, WeatherPattern pattern);

    static WeatherPattern constant(double normalizedSeverity);

    double getNormalizedSeverity(Position coordinates);

    double getNormalizedSeverity(int row, int col);

    std::vector<std::pair<WeatherSeverity, double>> getWeatherLevels() const;

  private:
    void initializeSectors();
    WeatherPattern makeRandomWeatherPattern();
    int chooseNextIndex(int currentIndex);
    SectorID getId(int row, int col);

    std::mt19937 rng_;

    GridConfig gridConfig_;
    std::vector<SectorID> sectorIds_;
    std::unordered_map<SectorID, double> normalizedSeverities_;
    std::unordered_map<SectorID, WeatherPattern> weatherPatterns_;

    std::atomic<bool> isRunning_ = false;
    double currentTime_;
    double timeStep_;
    std::vector<std::pair<WeatherSeverity, double>> weatherLevels_;
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