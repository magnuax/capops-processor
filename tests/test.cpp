#include <catch2/catch_test_macros.hpp>
#include "compute/ComputeData.hpp"
#include "config/Config.hpp"
#include "domain/Track.hpp"
#include "domain/WeatherCell.hpp"
#include "domain/types/Position.hpp"
#include "domain/types/WeatherSeverity.hpp"
#include "domain/types/SectorState.hpp"
#include <filesystem>
#include <fstream>
#include <cstdio>

TEST_CASE("Catch2 works")
{
    REQUIRE(true);
}

TEST_CASE("ComputeData initialization")
{
    // Create a temporary config file for testing
    std::string configPath = "/tmp/test_config.cfg";
    std::ofstream configFile(configPath);
    configFile << "[grid]\n"
               << "minLat=59.0\n"
               << "maxLat=61.0\n"
               << "minLon=4.0\n"
               << "maxLon=6.0\n"
               << "rows=3\n"
               << "cols=3\n"
               << "\n"
               << "[capacity]\n"
               << "defaultBaseCapacity=10\n"
               << "\n"
               << "[weatherFactors]\n"
               << "OK=1.0\n"
               << "DEGRADED=0.8\n"
               << "SEVERE=0.6\n"
               << "EXTREME=0.4\n";
    configFile.close();

    Configuration config(configPath);
    ComputeData computeData(config);

    // Test that sectors are initialized
    ProcessingResult result = computeData.collectDataForPublish();
    REQUIRE(result.sectorUpdatesToPublish.size() == 9); // 3x3 grid = 9 sectors

    // Check that all sectors start with NORMAL state and 0 tracks
    for (const auto& sector : result.sectorUpdatesToPublish) {
        REQUIRE(sector.getState() == SectorState::NORMAL);
        REQUIRE(sector.getLocalAircraftCount() == 0);
    }

    // Clean up
    std::remove(configPath.c_str());
}

TEST_CASE("ComputeData handleTrackUpdate - new track")
{
    std::string configPath = "/tmp/test_config.cfg";
    std::ofstream configFile(configPath);
    configFile << "[grid]\n"
               << "minLat=59.0\n"
               << "maxLat=61.0\n"
               << "minLon=4.0\n"
               << "maxLon=6.0\n"
               << "rows=3\n"
               << "cols=3\n"
               << "\n"
               << "[capacity]\n"
               << "defaultBaseCapacity=10\n"
               << "\n"
               << "[weatherFactors]\n"
               << "OK=1.0\n"
               << "DEGRADED=0.8\n"
               << "SEVERE=0.6\n"
               << "EXTREME=0.4\n";
    configFile.close();

    Configuration config(configPath);
    ComputeData computeData(config);

    // Create a track in sector 0 (bottom-left)
    Position pos{59.1, 4.1};
    Track track("ABC123", "1000", pos, 10000.0, 500.0, 0.0, 90.0, 90.0);

    computeData.handleTrackUpdate(track);

    ProcessingResult result = computeData.collectDataForPublish();

    // Check that track is in published tracks
    REQUIRE(result.trackUpdatesToPublish.size() == 1);
    REQUIRE(result.trackUpdatesToPublish[0].getIcao() == "ABC123");

    // Check that sector 0 has 1 track
    auto sector0 = std::find_if(result.sectorUpdatesToPublish.begin(),
                               result.sectorUpdatesToPublish.end(),
                               [](const SectorSummary& s) { return s.getSectorId() == 0; });
    REQUIRE(sector0 != result.sectorUpdatesToPublish.end());
    REQUIRE(sector0->getLocalAircraftCount() == 1);

    // Other sectors should have 0 tracks
    for (const auto& sector : result.sectorUpdatesToPublish) {
        if (sector.getSectorId() != 0) {
            REQUIRE(sector.getLocalAircraftCount() == 0);
        }
    }

    std::remove(configPath.c_str());
}

TEST_CASE("ComputeData handleTrackUpdate - track movement between sectors")
{
    std::string configPath = "/tmp/test_config.cfg";
    std::ofstream configFile(configPath);
    configFile << "[grid]\n"
               << "minLat=59.0\n"
               << "maxLat=61.0\n"
               << "minLon=4.0\n"
               << "maxLon=6.0\n"
               << "rows=3\n"
               << "cols=3\n"
               << "\n"
               << "[capacity]\n"
               << "defaultBaseCapacity=10\n"
               << "\n"
               << "[weatherFactors]\n"
               << "OK=1.0\n"
               << "DEGRADED=0.8\n"
               << "SEVERE=0.6\n"
               << "EXTREME=0.4\n";
    configFile.close();

    Configuration config(configPath);
    ComputeData computeData(config);

    // Initial track in sector 0
    Position pos1{59.1, 4.1};
    Track track1("ABC123", "1000", pos1, 10000.0, 500.0, 0.0, 90.0, 90.0);
    computeData.handleTrackUpdate(track1);

    // Updated track moved to sector 1
    Position pos2{59.1, 4.7};
    Track track2("ABC123", "2000", pos2, 10000.0, 500.0, 0.0, 90.0, 90.0);
    computeData.handleTrackUpdate(track2);

    ProcessingResult result = computeData.collectDataForPublish();

    // Check track count in sectors
    auto sector0 = std::find_if(result.sectorUpdatesToPublish.begin(),
                               result.sectorUpdatesToPublish.end(),
                               [](const SectorSummary& s) { return s.getSectorId() == 0; });
    auto sector1 = std::find_if(result.sectorUpdatesToPublish.begin(),
                               result.sectorUpdatesToPublish.end(),
                               [](const SectorSummary& s) { return s.getSectorId() == 1; });

    REQUIRE(sector0->getLocalAircraftCount() == 0);
    REQUIRE(sector1->getLocalAircraftCount() == 1);

    std::remove(configPath.c_str());
}

TEST_CASE("ComputeData handleWeatherUpdate")
{
    std::string configPath = "/tmp/test_config.cfg";
    std::ofstream configFile(configPath);
    configFile << "[grid]\n"
               << "minLat=59.0\n"
               << "maxLat=61.0\n"
               << "minLon=4.0\n"
               << "maxLon=6.0\n"
               << "rows=3\n"
               << "cols=3\n"
               << "\n"
               << "[capacity]\n"
               << "defaultBaseCapacity=10\n"
               << "\n"
               << "[weatherFactors]\n"
               << "OK=1.0\n"
               << "DEGRADED=0.8\n"
               << "SEVERE=0.6\n"
               << "EXTREME=0.4\n";
    configFile.close();

    Configuration config(configPath);
    ComputeData computeData(config);

    // Update weather in sector 0
    WeatherCell weather(0, "1000", WeatherSeverity::SEVERE);
    computeData.handleWeatherUpdate(weather);

    ProcessingResult result = computeData.collectDataForPublish();

    // Check that sector 0 has severe weather
    auto sector0 = std::find_if(result.sectorUpdatesToPublish.begin(),
                               result.sectorUpdatesToPublish.end(),
                               [](const SectorSummary& s) { return s.getSectorId() == 0; });
    REQUIRE(sector0 != result.sectorUpdatesToPublish.end());

    std::remove(configPath.c_str());
}

TEST_CASE("ComputeData sector state evaluation - overload")
{
    std::string configPath = "/tmp/test_config.cfg";
    std::ofstream configFile(configPath);
    configFile << "[grid]\n"
               << "minLat=59.0\n"
               << "maxLat=61.0\n"
               << "minLon=4.0\n"
               << "maxLon=6.0\n"
               << "rows=3\n"
               << "cols=3\n"
               << "\n"
               << "[capacity]\n"
               << "defaultBaseCapacity=1\n"  // Low capacity to trigger overload
               << "\n"
               << "[weatherFactors]\n"
               << "OK=1.0\n"
               << "DEGRADED=0.8\n"
               << "SEVERE=0.6\n"
               << "EXTREME=0.4\n";
    configFile.close();

    Configuration config(configPath);
    ComputeData computeData(config);

    // Add multiple tracks to sector 0 to exceed capacity
    Position pos{59.1, 4.1};
    for (int i = 0; i < 3; ++i) {
        std::string icao = "ABC" + std::to_string(i);
        std::string timestamp = std::to_string(1000 + i);
        Track track(icao, timestamp, pos, 10000.0, 500.0, 0.0, 90.0, 90.0);
        computeData.handleTrackUpdate(track);
    }

    ProcessingResult result = computeData.collectDataForPublish();

    // Check that sector 0 is overloaded
    auto sector0 = std::find_if(result.sectorUpdatesToPublish.begin(),
                               result.sectorUpdatesToPublish.end(),
                               [](const SectorSummary& s) { return s.getSectorId() == 0; });
    REQUIRE(sector0->getState() == SectorState::AT_RISK);

    // Should have generated risk events
    REQUIRE(result.riskEventsToPublish.size() > 0);

    std::remove(configPath.c_str());
}

TEST_CASE("ComputeData collectDataForPublish clears pending events")
{
    std::string configPath = "/tmp/test_config.cfg";
    std::ofstream configFile(configPath);
    configFile << "[grid]\n"
               << "minLat=59.0\n"
               << "maxLat=61.0\n"
               << "minLon=4.0\n"
               << "maxLon=6.0\n"
               << "rows=3\n"
               << "cols=3\n"
               << "\n"
               << "[capacity]\n"
               << "defaultBaseCapacity=1\n"
               << "\n"
               << "[weatherFactors]\n"
               << "OK=1.0\n"
               << "DEGRADED=0.8\n"
               << "SEVERE=0.6\n"
               << "EXTREME=0.4\n";
    configFile.close();

    Configuration config(configPath);
    ComputeData computeData(config);

    // Create overload to generate risk event
    Position pos{59.1, 4.1};
    Track track("ABC123", "1000", pos, 10000.0, 500.0, 0.0, 90.0, 90.0);
    computeData.handleTrackUpdate(track);
    Track track2("DEF456", "1001", pos, 10000.0, 500.0, 0.0, 90.0, 90.0);
    computeData.handleTrackUpdate(track2);

    // First collect should return the risk event
    ProcessingResult result1 = computeData.collectDataForPublish();
    REQUIRE(result1.riskEventsToPublish.size() == 1);

    // Second collect should have no risk events (cleared)
    ProcessingResult result2 = computeData.collectDataForPublish();
    REQUIRE(result2.riskEventsToPublish.size() == 0);

    // But tracks should still be there
    REQUIRE(result2.trackUpdatesToPublish.size() == 2);

    std::remove(configPath.c_str());
}
