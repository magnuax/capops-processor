#pragma once

#include "domain/types/SourceType.hpp"
#include "domain/types/WeatherSeverity.hpp"
#include <string>
#include <unordered_map>
#include <vector>

struct GridConfig
{
    double minLat;
    double maxLat;
    double minLon;
    double maxLon;
    double cellSizeDeg = 1.0;
    int rows;
    int cols;
    void computeDimensions();
};

class Configuration
{
  public:
    explicit Configuration(const std::string &path);

    const GridConfig &grid() const;
    double defaultBaseCapacity() const;
    double weatherFactor(WeatherSeverity severity) const;
    void load(const std::string &path);
    int getProtobufVersion() const;
    std::string getCoordinateSystem() const;
    std::string getRedisUrl() const;
    std::string getRedisChannel() const;
    SourceType getSourceType() const;
    int getNumFlights() const;
    std::vector<std::pair<WeatherSeverity, double>> getSortedWeatherLevels() const;
    double getTimestepSize() const;
    int getLoopInterval() const;

  private:
    GridConfig grid_;
    double defaultBaseCapacity_;
    std::unordered_map<WeatherSeverity, double> weatherFactors_;
    int protobufVersion_;
    std::string coordinateSystem_;
    std::string redisUrl_;
    std::string redisChannel_;
    SourceType sourceType_;
    int numFlights_;
    double timestepSize_;
    int loopIntervalMs_;
};