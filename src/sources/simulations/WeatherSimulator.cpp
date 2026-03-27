
#include "sources/simulations/WeatherSimulator.hpp"
#include <functional>
#include <random>
#include <stdexcept>

WeatherSimulator::WeatherSimulator(
    GridConfig config, const std::vector<std::pair<WeatherSeverity, double>> &weatherLevels)
    : rng_(994)
{
    timeStep_ = 1.0;
    currentTime_ = 0.0;
    gridConfig_ = config;
    weatherLevels_ = weatherLevels;

    initializeSectors();
}

double WeatherSimulator::getNormalizedSeverity(Position coordinates)
{
    double latFraction =
        (coordinates.latDeg - gridConfig_.minLat) / (gridConfig_.maxLat - gridConfig_.minLat);

    double lonFraction =
        (coordinates.lonDeg - gridConfig_.minLon) / (gridConfig_.maxLon - gridConfig_.minLon);

    int row = static_cast<int>(latFraction * gridConfig_.rows);
    int col = static_cast<int>(lonFraction * gridConfig_.cols);

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

std::vector<std::pair<WeatherSeverity, double>> WeatherSimulator::getWeatherLevels() const
{
    return weatherLevels_;
}

int WeatherSimulator::chooseNextIndex(int currentIndex)
{
    std::uniform_int_distribution<int> dirDist(0, 1);
    int lastIndex = static_cast<int>(weatherLevels_.size()) - 1;

    if (currentIndex == 0)
        return 1;

    if (currentIndex == lastIndex)
        return lastIndex - 1;

    return dirDist(rng_) == 0 ? currentIndex - 1 : currentIndex + 1;
}

WeatherSimulator::WeatherPattern WeatherSimulator::makeRandomWeatherPattern()
{
    std::uniform_int_distribution<int> levelDist(0, static_cast<int>(weatherLevels_.size()) - 1);
    std::uniform_real_distribution<double> durationDist(0.0, 1.0);

    int currentIndex = levelDist(rng_);
    double nextChangeTime = durationDist(rng_);

    return [this, currentIndex, nextChangeTime, durationDist](double time) mutable -> double
    {
        if (time >= nextChangeTime)
        {
            currentIndex = chooseNextIndex(currentIndex);
            nextChangeTime = time + durationDist(rng_);
        }

        return weatherLevels_[currentIndex].second;
    };
}
void WeatherSimulator::generateRandomWeatherPatterns()
{
    for (int row = 0; row < gridConfig_.rows; ++row)
    {
        for (int col = 0; col < gridConfig_.cols; ++col)
        {
            setWeatherPattern(row, col, makeRandomWeatherPattern());
        }
    }
}