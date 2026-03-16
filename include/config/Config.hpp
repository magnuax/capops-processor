#pragma once

#include "domain/types/WeatherSeverity.hpp"
#include <string>
#include <unordered_map>

struct GridConfig
{
    double minLat;
    double maxLat;
    double minLon;
    double maxLon;
    int rows;
    int cols;
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

  private:
    GridConfig grid_;
    double defaultBaseCapacity_;
    std::unordered_map<WeatherSeverity, double> weatherFactors_;
    int protobufVersion_;
    std::string coordinateSystem_;
};