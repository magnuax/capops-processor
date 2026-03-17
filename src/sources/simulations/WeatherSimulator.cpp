
#include "sources/simulations/WeatherSimulator.hpp"
#include <functional>
#include <stdexcept>

WeatherSimulator::WeatherSimulator(GridConfig config)
{
    timeStep_ = 1.0;
    currentTime_ = 0.0;
    gridConfig_ = config;

    initializeSectors();
}

double WeatherSimulator::getNormalizedSeverity(Position coordinates)
{
    int row = static_cast<int>(coordinates.latDeg * gridConfig_.rows);
    int col = static_cast<int>(coordinates.lonDeg * gridConfig_.cols);

    if (row < 0 || row >= gridConfig_.rows || col < 0 || col >= gridConfig_.cols)
    {
        throw std::out_of_range("Coordinates are out of simulation grid bounds");
    }

    return getNormalizedSeverity(row, col);
}

double WeatherSimulator::getNormalizedSeverity(int row, int col)
{
    return normalizedSeverities_[getId(row, col)];
}

void WeatherSimulator::initializeSectors()
{
    const int numRows = gridConfig_.rows;
    const int numCols = gridConfig_.cols;

    for (int row = 0; row < numRows; ++row)
    {
        for (int col = 0; col < numCols; ++col)
        {
            std::string sectorId = getId(row, col);

            sectorIds_.push_back(sectorId);
            normalizedSeverities_[sectorId] = 0.0;
            weatherPatterns_[sectorId] = constant(0.0);
        }
    }
}

void WeatherSimulator::start()
{
    isRunning_ = true;

    while (isRunning_)
        tick(timeStep_);
}

void WeatherSimulator::stop()
{
    isRunning_ = false;
}

void WeatherSimulator::tick(double timeStep)
{
    currentTime_ += timeStep;

    for (const auto &[sectorId, pattern] : weatherPatterns_)
    {
        normalizedSeverities_[sectorId] = pattern(currentTime_);
    }
}

void WeatherSimulator::setWeatherPattern(int row, int col, double normalizedSeverity)
{
    setWeatherPattern(row, col, constant(normalizedSeverity));
}

void WeatherSimulator::setWeatherPattern(int row, int col, WeatherPattern pattern)
{
    weatherPatterns_[getId(row, col)] = pattern;
}

WeatherSimulator::WeatherPattern WeatherSimulator::constant(double normalizedSeverity)
{
    return [normalizedSeverity](double) { return normalizedSeverity; };
}

WeatherSimulator::SectorID WeatherSimulator::getId(int row, int col)
{
    return std::to_string(row * gridConfig_.cols + col);
}
