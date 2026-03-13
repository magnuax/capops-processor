#include "app/ProcessorApp.hpp"
#include "compute/ComputeData.hpp"
#include "config/Config.hpp"
#include "simulator/SimpleTestSimulator.hpp"
#include <iostream>
#include <variant>

void ProcessorApp::run()
{
    SimpleTestSimulator simulator;
    Configuration config("configuration.cfg");
    ComputeData computeData(config);

    // just for testing now, will remove later:
    while (simulator.hasNextEvent())
    {
        SimulationEvent event = simulator.nextEvent();

        std::visit(
            [&](const auto &e)
            {
                using T = std::decay_t<decltype(e)>;

                if constexpr (std::is_same_v<T, Track>)
                {
                    computeData.handleTrackUpdate(e);
                }
                else if constexpr (std::is_same_v<T, WeatherCell>)
                {
                    computeData.handleWeatherUpdate(e);
                }
            },
            event);
    }
    ProcessingResult result = computeData.collectDataForPublish();
    std::cout << "\n--- ProcessingResult ---\n";

    std::cout << "Tracks:\n";
    for (const auto &track : result.trackUpdatesToPublish)
    {
        std::cout << "ICAO: " << track.getIcao() << " lat: " << track.getPosition().latDeg
                  << " lon: " << track.getPosition().lonDeg << " ts: " << track.getTimestamp()
                  << "\n";
    }

    std::cout << "\nSector summaries:\n";
    for (const auto &summary : result.sectorUpdatesToPublish)
    {
        std::cout << "Sector: " << summary.getSectorId()
                  << " tracks: " << summary.getLocalAircraftCount()
                  << " state: " << static_cast<int>(summary.getState()) << "\n";
    }

    std::cout << "\nRisk events:\n";
    for (const auto &risk : result.riskEventsToPublish)
    {
        std::cout << "Sector: " << risk.getSectorId()
                  << " state: " << static_cast<int>(risk.getState())
                  << " ts: " << risk.getTimestamp() << "\n";
    }

    // while (running) {
    // // 1. prosesser innkommende track/weather events fortløpende

    // // 2. hvis det er tid for publisering:
    // ProcessingResult result = computeData.collectDataForPublish();

    // publisher.publishTracks(result.trackUpdatesToPublish);
    // publisher.publishSectorSummaries(result.sectorUpdatesToPublish);
    // publisher.publishRiskEvents(result.riskEventsToPublish);}
}
