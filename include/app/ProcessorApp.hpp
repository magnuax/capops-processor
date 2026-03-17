#pragma once

#include "sources/interfaces/ITrackSource.hpp"
#include "sources/interfaces/IWeatherSource.hpp"
#include <memory>

class RadarSimulator;
class WeatherSimulator;
class Configuration;
class Grid;
class IngestService;
class ComputeData;
class RedisPublisher;

class ProcessorApp
{
  public:
    ProcessorApp();
    ~ProcessorApp();

    void run();

  private:
    void initializeSources(const Configuration &config);
    void tickSimulators(double timeStep);
    void processTrackUpdates(IngestService &ingestService, ComputeData &computeData);
    void processWeatherUpdates(const Grid &grid, IngestService &ingestService,
                               ComputeData &computeData);
    void publishResults(ComputeData &computeData, RedisPublisher &publisher);

    std::unique_ptr<ITrackSource> trackSource_;
    std::unique_ptr<IWeatherSource> weatherSource_;

    std::unique_ptr<RadarSimulator> radarSimulator_;
    std::unique_ptr<WeatherSimulator> weatherSimulator_;

    bool simulationMode_ = false;
};