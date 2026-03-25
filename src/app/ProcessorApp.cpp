#include "app/ProcessorApp.hpp"

#include "compute/ComputeData.hpp"
#include "compute/Grid.hpp"
#include "config/Config.hpp"
#include "domain/WeatherCell.hpp"
#include "domain/types/ProcessingResult.hpp"
#include "ingest/IngestService.hpp"
#include "publish/RedisPublisher.hpp"

#include "sources/interfaces/ITrackSource.hpp"
#include "sources/interfaces/IWeatherSource.hpp"

#include "sources/simulations/RadarSimulator.hpp"
#include "sources/simulations/WeatherSimulator.hpp"

#include "sources/TrackSourceOpenSky.hpp"
#include "sources/TrackSourceSimulated.hpp"

#include "sources/WeatherSourceOpenMeteo.hpp"
#include "sources/WeatherSourceSimulated.hpp"

#include "utils/WeatherPatternUtils.hpp"
#include "utils/time/IsoTimestamp.hpp"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

ProcessorApp::ProcessorApp() = default;
ProcessorApp::~ProcessorApp() = default;

void ProcessorApp::run()
{
    Configuration config("configuration.cfg");
    Grid grid(config.grid());

    initializeSources(config);

    IngestService ingestService(config.grid(), trackSource_.get(), weatherSource_.get());
    ComputeData computeData(config);
    RedisPublisher publisher(config);

    double timeStep = config.getTimestepSize();
    auto sleepDuration = std::chrono::milliseconds(config.getLoopInterval());

    while (true)
    {
        if (simulationMode_)
            tickSimulators(timeStep);
        processTrackUpdates(ingestService, computeData);
        processWeatherUpdates(grid, ingestService, computeData);
        publishResults(computeData, publisher);

        std::this_thread::sleep_for(sleepDuration);
    }
}

void ProcessorApp::initializeSources(const Configuration &config)
{
    simulationMode_ = (config.getSourceType() == SourceType::Simulation);

    if (simulationMode_)
    {
        radarSimulator_ = std::make_unique<RadarSimulator>(config.grid());
        radarSimulator_->initializeFlights(config.getNumFlights());

        weatherSimulator_ =
            std::make_unique<WeatherSimulator>(config.grid(), config.getSortedWeatherLevels());
        // weatherSimulator_->setRandomWeatherPatterns();
        weatherSimulator_->setHorizontalWavePattern(0.1, 0.05);

        trackSource_ = std::make_unique<TrackSourceSimulated>(*radarSimulator_);
        weatherSource_ = std::make_unique<WeatherSourceSimulated>(*weatherSimulator_);
        return;
    }

    if (config.getSourceType() == SourceType::Api)
    {

        auto openSky = std::make_unique<TrackSourceOpenSky>();
        openSky->setRegion(config.grid());
        trackSource_ = std::move(openSky);

        weatherSource_ = std::make_unique<WeatherSourceOpenMeteo>();
        return;
    }

    throw std::runtime_error("Unsupported source type in configuration.");
}

void ProcessorApp::tickSimulators(double timeStep)
{

    radarSimulator_->tick(timeStep);
    weatherSimulator_->tick(timeStep);
}

void ProcessorApp::processTrackUpdates(IngestService &ingestService, ComputeData &computeData)
{
    auto tracks = ingestService.getAllTracks();

    for (const auto &track : tracks)
    {
        computeData.handleTrackUpdate(track);
    }
}

void ProcessorApp::processWeatherUpdates(const Grid &grid, IngestService &ingestService,
                                         ComputeData &computeData)
{
    std::string timestamp = createIsoTimestamp();

    for (int sectorId = 0; sectorId < grid.sectorCount(); ++sectorId)
    {
        Position pos = grid.sectorCenter(sectorId);
        WeatherSeverity severity = ingestService.getWeatherSeverity(pos);

        WeatherCell weatherCell(sectorId, timestamp, severity);
        computeData.handleWeatherUpdate(weatherCell);
    }
}

void ProcessorApp::publishResults(ComputeData &computeData, RedisPublisher &publisher)
{
    ProcessingResult result = computeData.collectProcessingResult();
    publisher.publish(result);
}