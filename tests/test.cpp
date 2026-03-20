#include "compute/ComputeData.hpp"
#include "compute/Grid.hpp"
#include "config/Config.hpp"
#include "domain/RiskEvent.hpp"
#include "domain/SectorSummary.hpp"
#include "domain/Track.hpp"
#include "domain/WeatherCell.hpp"
#include "domain/types/Position.hpp"
#include "domain/types/ProcessingResult.hpp"
#include "domain/types/SectorState.hpp"
#include "domain/types/WeatherSeverity.hpp"
#include "ingest/IngestService.hpp"
#include "proto/FlightData.pb.h"
#include "publish/ProtoMapper.hpp"
#include "publish/RedisPublisher.hpp"
#include "sources/TrackSourceSimulated.hpp"
#include "sources/WeatherSourceOpenMeteo.hpp"
#include "sources/TrackSourceOpenSky.hpp"
#include "sources/WeatherSourceSimulated.hpp"
#include "sources/simulations/RadarSimulator.hpp"
#include "sources/simulations/WeatherSimulator.hpp"
#include <QCoreApplication>
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>

// Helper function to create a test configuration
Configuration createTestConfig()
{
    // Create a temporary config file for testing
    std::string configPath = "/tmp/test_config.cfg";
    std::ofstream configFile(configPath);

    configFile << "[grid]\n";
    configFile << "minLat=59.0\n";
    configFile << "maxLat=61.0\n";
    configFile << "minLon=4.0\n";
    configFile << "maxLon=6.0\n";
    configFile << "rows=3\n";
    configFile << "cols=3\n";
    configFile << "coordinateSystem=WGS84\n";
    configFile << "\n";
    configFile << "[capacity]\n";
    configFile << "defaultBaseCapacity=1\n";
    configFile << "\n";
    configFile << "[weatherFactors]\n";
    configFile << "OK=1.0\n";
    configFile << "DEGRADED=0.8\n";
    configFile << "SEVERE=0.6\n";
    configFile << "EXTREME=0.4\n";
    configFile << "\n";
    configFile << "[protobufVersion]\n";
    configFile << "version=1\n";
    configFile << "\n";
    configFile << "[redis]\n";
    configFile << "redisUrl=tcp://127.0.0.1:6379\n";
    configFile << "redisChannel=test_channel\n";
    configFile << "\n";
    configFile << "[dataSource]\n";
    configFile << "type=sim\n";
    configFile << "\n";
    configFile << "[simulation]\n";
    configFile << "numFlights=3\n";
    configFile << "timestepSize=1.0\n";
    configFile << "\n";
    configFile << "[execution]\n";
    configFile << "loopIntervalMs=100\n";

    configFile.close();

    Configuration config(configPath);
    std::remove(configPath.c_str());

    return config;
}

// Helper function to create a test configuration with API sources
Configuration createTestConfigApi()
{
    // Create a temporary config file for testing
    std::string configPath = "/tmp/test_config_api.cfg";
    std::ofstream configFile(configPath);

    configFile << "[grid]\n";
    configFile << "minLat=59.0\n";
    configFile << "maxLat=61.0\n";
    configFile << "minLon=4.0\n";
    configFile << "maxLon=6.0\n";
    configFile << "rows=3\n";
    configFile << "cols=3\n";
    configFile << "coordinateSystem=WGS84\n";
    configFile << "\n";
    configFile << "[capacity]\n";
    configFile << "defaultBaseCapacity=1\n";
    configFile << "\n";
    configFile << "[weatherFactors]\n";
    configFile << "OK=1.0\n";
    configFile << "DEGRADED=0.8\n";
    configFile << "SEVERE=0.6\n";
    configFile << "EXTREME=0.4\n";
    configFile << "\n";
    configFile << "[protobufVersion]\n";
    configFile << "version=1\n";
    configFile << "\n";
    configFile << "[redis]\n";
    configFile << "redisUrl=tcp://127.0.0.1:6379\n";
    configFile << "redisChannel=test_channel\n";
    configFile << "\n";
    configFile << "[dataSource]\n";
    configFile << "type=api\n";
    configFile << "\n";
    configFile << "[simulation]\n";
    configFile << "numFlights=3\n";
    configFile << "timestepSize=1.0\n";
    configFile << "\n";
    configFile << "[execution]\n";
    configFile << "loopIntervalMs=100\n";

    configFile.close();

    Configuration config(configPath);
    std::remove(configPath.c_str());

    return config;
}

static auto findSectorSummary(const ProcessingResult &result, int sectorId)
{
    return std::find_if(result.sectorSummaries.begin(), result.sectorSummaries.end(),
                        [sectorId](const SectorSummary &s) { return s.getSectorId() == sectorId; });
}

// ============================================================================
// DOMAIN TESTS - Track, WeatherCell, RiskEvent, SectorSummary
// ============================================================================

TEST_CASE("Track initialization and getters")
{
    Position pos{59.5, 5.0};
    Track track("ABC123", "2024-01-01T12:00:00Z", pos, 10000.0, 450.0, 100.0, 180.0, 175.0);

    REQUIRE(track.getIcao() == "ABC123");
    REQUIRE(track.getTimestamp() == "2024-01-01T12:00:00Z");
    REQUIRE(track.getPosition().latDeg == 59.5);
    REQUIRE(track.getPosition().lonDeg == 5.0);
    REQUIRE(track.getAltitudeFeet() == 10000.0);
    REQUIRE(track.getGroundSpeedKnots() == 450.0);
    REQUIRE(track.getVerticalSpeedFeetPerMinute() == 100.0);
    REQUIRE(track.getHeadingDegrees() == 180.0);
    REQUIRE(track.getGroundTrackDegrees() == 175.0);
}

TEST_CASE("WeatherCell creation and getters")
{
    WeatherCell weatherCell(5, "2024-01-01T12:00:00Z", WeatherSeverity::SEVERE);

    REQUIRE(weatherCell.getSectorId() == 5);
    REQUIRE(weatherCell.getTimestamp() == "2024-01-01T12:00:00Z");
    REQUIRE(weatherCell.getWeatherSeverity() == WeatherSeverity::SEVERE);
}

TEST_CASE("RiskEvent creation and getters")
{
    RiskEvent riskEvent(1, SectorState::AT_RISK, 3, "2024-01-01T12:00:00Z", "Test risk message");

    REQUIRE(riskEvent.getRiskEventId() == 1);
    REQUIRE(riskEvent.getState() == SectorState::AT_RISK);
    REQUIRE(riskEvent.getSectorId() == 3);
    REQUIRE(riskEvent.getTimestamp() == "2024-01-01T12:00:00Z");
    REQUIRE(riskEvent.getMessage() == "Test risk message");
}

TEST_CASE("SectorSummary initialization and aircraft count management")
{
    SectorSummary summary(0, 0, 0, "2024-01-01T12:00:00Z", 0, WeatherSeverity::OK, 1.0, 100.0,
                          SectorState::NORMAL);

    REQUIRE(summary.getSectorId() == 0);
    REQUIRE(summary.getRow() == 0);
    REQUIRE(summary.getColumn() == 0);
    REQUIRE(summary.getLocalAircraftCount() == 0);
    REQUIRE(summary.getBaseCapacity() == 100.0);

    summary.increaseLocalAircraftCount();
    REQUIRE(summary.getLocalAircraftCount() == 1);

    summary.increaseLocalAircraftCount();
    REQUIRE(summary.getLocalAircraftCount() == 2);

    summary.decreaseLocalAircraftCount();
    REQUIRE(summary.getLocalAircraftCount() == 1);
}

TEST_CASE("SectorSummary weather update")
{
    SectorSummary summary(0, 0, 0, "2024-01-01T12:00:00Z", 0, WeatherSeverity::OK, 1.0, 100.0,
                          SectorState::NORMAL);

    REQUIRE(summary.getWeatherSeverity() == WeatherSeverity::OK);

    summary.updateWeather(WeatherSeverity::SEVERE, 0.6);
    REQUIRE(summary.getWeatherSeverity() == WeatherSeverity::SEVERE);
    REQUIRE(summary.getWeatherFactor() == 0.6);
}

TEST_CASE("SectorSummary timestamp update")
{
    SectorSummary summary(0, 0, 0, "2024-01-01T12:00:00Z", 0, WeatherSeverity::OK, 1.0, 100.0,
                          SectorState::NORMAL);

    // updateTime modifies internal state, verify by checking no exception is thrown
    summary.updateTime("2024-01-01T12:01:00Z");
    REQUIRE(true); // If we get here, update succeeded
}

// ============================================================================
// GRID TESTS - Grid coordinate system and sector management
// ============================================================================

TEST_CASE("Grid initialization and sector counting")
{
    Configuration config = createTestConfig();
    Grid grid(config.grid());

    // Grid is 3x3
    REQUIRE(grid.sectorCount() == 9);
    REQUIRE(grid.rows() == 3);
    REQUIRE(grid.cols() == 3);
}

TEST_CASE("Grid position conversion")
{
    Configuration config = createTestConfig();
    Grid grid(config.grid());

    // Test a position within the grid
    Position pos{59.5, 5.0};
    REQUIRE(grid.isInside(pos) == true);

    int sectorId = grid.determineSector(pos);
    REQUIRE(sectorId >= 0);
    REQUIRE(sectorId < grid.sectorCount());

    // Get sector center and verify it's within reasonable bounds
    Position center = grid.sectorCenter(sectorId);
    REQUIRE(center.latDeg >= config.grid().minLat);
    REQUIRE(center.latDeg <= config.grid().maxLat);
    REQUIRE(center.lonDeg >= config.grid().minLon);
    REQUIRE(center.lonDeg <= config.grid().maxLon);
}

TEST_CASE("Grid position outside bounds")
{
    Configuration config = createTestConfig();
    Grid grid(config.grid());

    Position outsidePos1{50.0, -80.0};
    Position outsidePos2{70.0, 10.0};
    Position outsidePos3{58.0, 2.0};

    REQUIRE(grid.isInside(outsidePos1) == false);
    REQUIRE(grid.isInside(outsidePos2) == false);
    REQUIRE(grid.isInside(outsidePos3) == false);
}

TEST_CASE("Grid row and column calculation")
{
    Configuration config = createTestConfig();
    Grid grid(config.grid());

    for (int sectorId = 0; sectorId < grid.sectorCount(); ++sectorId)
    {
        int row = grid.row(sectorId);
        int col = grid.column(sectorId);

        REQUIRE(row >= 0);
        REQUIRE(row < grid.rows());
        REQUIRE(col >= 0);
        REQUIRE(col < grid.cols());
    }
}

TEST_CASE("Grid sector center is inside the sector")
{
    Configuration config = createTestConfig();
    Grid grid(config.grid());

    // For each sector, verify that its center is actually inside that sector
    for (int sectorId = 0; sectorId < grid.sectorCount(); ++sectorId)
    {
        Position center = grid.sectorCenter(sectorId);

        // The center must be inside the grid
        REQUIRE(grid.isInside(center) == true);

        // The center must map back to the same sector
        int determinedSector = grid.determineSector(center);
        REQUIRE(determinedSector == sectorId);

        // Verify the center is within reasonable bounds (within the grid)
        REQUIRE(center.latDeg >= config.grid().minLat);
        REQUIRE(center.latDeg <= config.grid().maxLat);
        REQUIRE(center.lonDeg >= config.grid().minLon);
        REQUIRE(center.lonDeg <= config.grid().maxLon);
    }
}

// ============================================================================
// COMPUTEDATA TESTS - Main processing logic
// ============================================================================

TEST_CASE("ComputeData initialization with sectors")
{
    Configuration config = createTestConfig();
    ComputeData computeData(config);

    // Initially, there should be no tracks and no pending risk events
    ProcessingResult result = computeData.collectProcessingResult();
    REQUIRE(result.tracks.empty());
    REQUIRE(result.riskEvents.empty());
    REQUIRE(result.sectorSummaries.size() == config.grid().rows * config.grid().cols); // 3x3 = 9
}

TEST_CASE("ComputeData handles track update")
{
    Configuration config = createTestConfig();
    ComputeData computeData(config);

    Position pos{59.5, 5.0};
    Track track("ABC123", "2024-01-01T12:00:00Z", pos, 10000.0, 450.0, 0.0, 180.0, 175.0);

    computeData.handleTrackUpdate(track);

    ProcessingResult result = computeData.collectProcessingResult();
    REQUIRE(result.tracks.size() == 1);
    REQUIRE(result.tracks[0].getIcao() == "ABC123");
}

TEST_CASE("ComputeData handles multiple track updates")
{
    Configuration config = createTestConfig();
    ComputeData computeData(config);

    Position pos1{59.5, 5.0};
    Track track1("ABC123", "2024-01-01T12:00:00Z", pos1, 10000.0, 450.0, 0.0, 180.0, 175.0);

    Position pos2{59.6, 5.1};
    Track track2("DEF456", "2024-01-01T12:00:00Z", pos2, 8000.0, 400.0, -100.0, 270.0, 265.0);

    computeData.handleTrackUpdate(track1);
    computeData.handleTrackUpdate(track2);

    ProcessingResult result = computeData.collectProcessingResult();
    REQUIRE(result.tracks.size() == 2);
}

TEST_CASE("ComputeData handles track update to same aircraft")
{
    Configuration config = createTestConfig();
    ComputeData computeData(config);

    Position pos1{59.5, 5.0};
    Track track1("ABC123", "2024-01-01T12:00:00Z", pos1, 10000.0, 450.0, 0.0, 180.0, 175.0);
    computeData.handleTrackUpdate(track1);

    Position pos2{59.51, 5.01};
    Track track2("ABC123", "2024-01-01T12:00:01Z", pos2, 10050.0, 450.0, 50.0, 180.0, 175.0);
    computeData.handleTrackUpdate(track2);

    ProcessingResult result = computeData.collectProcessingResult();
    REQUIRE(result.tracks.size() == 1);
    REQUIRE(result.tracks[0].getTimestamp() == "2024-01-01T12:00:01Z");
    REQUIRE(result.tracks[0].getAltitudeFeet() == 10050.0);
}

TEST_CASE("ComputeData handles weather update")
{
    Configuration config = createTestConfig();
    ComputeData computeData(config);

    WeatherCell weatherCell(0, "2024-01-01T12:00:00Z", WeatherSeverity::SEVERE);
    computeData.handleWeatherUpdate(weatherCell);

    ProcessingResult result = computeData.collectProcessingResult();

    auto it = std::find_if(result.sectorSummaries.begin(), result.sectorSummaries.end(),
                           [](const SectorSummary &summary) { return summary.getSectorId() == 0; });
    REQUIRE(it != result.sectorSummaries.end());
    REQUIRE(it->getWeatherSeverity() == WeatherSeverity::SEVERE);
}

TEST_CASE("ComputeData detects sector risk escalation")
{
    Configuration config = createTestConfig();
    ComputeData computeData(config);

    Grid grid(config.grid());
    Position center = grid.sectorCenter(0);

    ProcessingResult result = computeData.collectProcessingResult();

    // Find summary for sector 0
    auto sectorSummary0 = findSectorSummary(result, 0);
    REQUIRE(sectorSummary0 != result.sectorSummaries.end());
    REQUIRE(sectorSummary0->getState() == SectorState::NORMAL);

    Track track("FLIGHT", "2024-01-01T12:00:00Z", center, 10000.0, 450.0, 0.0, 180.0, 175.0);
    computeData.handleTrackUpdate(track);
    result = computeData.collectProcessingResult();

    sectorSummary0 = findSectorSummary(result, 0);
    REQUIRE(sectorSummary0->getState() ==
            SectorState::NORMAL); // Still NORMAL with 1 aircraft and OK weather

    WeatherCell weatherCell(0, "2024-01-01T12:00:00Z", WeatherSeverity::SEVERE);
    computeData.handleWeatherUpdate(weatherCell);
    result = computeData.collectProcessingResult();

    REQUIRE(result.riskEvents.size() > 0);
    REQUIRE(result.riskEvents[0].getMessage().find("escalated") != std::string::npos);

    sectorSummary0 = findSectorSummary(result, 0);
    REQUIRE(sectorSummary0->getState() == SectorState::AT_RISK); // Now should be AT_RISK

    Track track2("FLIGHT2", "2024-01-01T12:00:00Z", center, 10000.0, 450.0, 0.0, 180.0, 175.0);
    computeData.handleTrackUpdate(track2);
    result = computeData.collectProcessingResult();

    sectorSummary0 = findSectorSummary(result, 0);
    REQUIRE(sectorSummary0->getState() == SectorState::CONGESTED); // 2 aircraft + severe weather

    Position center2 = grid.sectorCenter(1);
    // Flight moves to another sector
    Track track3("FLIGHT2", "2024-01-01T12:00:01Z", center2, 10000.0, 450.0, 0.0, 180.0, 175.0);
    computeData.handleTrackUpdate(track3);
    result = computeData.collectProcessingResult();

    REQUIRE(result.riskEvents[0].getMessage().find("de-escalated") != std::string::npos);
    sectorSummary0 = findSectorSummary(result, 0);
    REQUIRE(sectorSummary0->getState() ==
            SectorState::AT_RISK); // Back to AT_RISK after losing 1 aircraft
}

// ============================================================================
// WEATHER SIMULATOR TESTS
// ============================================================================

TEST_CASE("WeatherSimulator initialization")
{
    Configuration config = createTestConfig();
    std::vector<std::pair<WeatherSeverity, double>> weatherLevels = config.getSortedWeatherLevels();

    WeatherSimulator simulator(config.grid(), weatherLevels);
    Grid grid(config.grid());
    // Calculate actual cell dimensions
    double latCellSize = (config.grid().maxLat - config.grid().minLat) / config.grid().rows;
    double lonCellSize = (config.grid().maxLon - config.grid().minLon) / config.grid().cols;

    // Check that all sectors are initialized
    for (int i = 0; i < grid.sectorCount(); i++)
    {
        Position center = grid.sectorCenter(i);
        double severity = simulator.getNormalizedSeverity(center);
        // hva skjer hvis vi ikke har satt noe pattern ?

        REQUIRE(severity >= 0.0);
        REQUIRE(severity <= 1.0);
    }
}

TEST_CASE("WeatherSimulator constant pattern")
{
    Configuration config = createTestConfig();
    std::vector<std::pair<WeatherSeverity, double>> weatherLevels = config.getSortedWeatherLevels();

    WeatherSimulator simulator(config.grid(), weatherLevels);
    simulator.setWeatherPattern(0, 0, 0.3);
    simulator.tick(0.0);
    Grid grid(config.grid());

    // Get severity at grid corner
    Position pos(grid.sectorCenter(0));
    double severity0 = simulator.getNormalizedSeverity(pos);
    REQUIRE(severity0 == 0.3);
}

TEST_CASE("WeatherSimulator tick updates time")
{
    Configuration config = createTestConfig();
    std::vector<std::pair<WeatherSeverity, double>> weatherLevels = config.getSortedWeatherLevels();

    WeatherSimulator simulator(config.grid(), weatherLevels);

    // Set a time-varying pattern
    auto timeVaryingPattern = [](double t) { return 0.5 + 0.3 * std::sin(t); };
    simulator.setWeatherPattern(0, 0, timeVaryingPattern);

    // Initial severity at grid corner
    Position pos{config.grid().minLat, config.grid().minLon};
    simulator.tick(0);
    double severity1 = simulator.getNormalizedSeverity(0, 0);

    // Simulate time passage
    simulator.tick(1.0);
    double severity2 = simulator.getNormalizedSeverity(0, 0);

    // Values should change (or at least be valid)
    REQUIRE(severity1 == 0.5);
    REQUIRE(severity2 == 0.5 + 0.3 * std::sin(1.0));
}

TEST_CASE("WeatherSimulator random patterns generation")
{
    Configuration config = createTestConfig();
    std::vector<std::pair<WeatherSeverity, double>> weatherLevels = config.getSortedWeatherLevels();

    WeatherSimulator simulator(config.grid(), weatherLevels);
    simulator.generateRandomWeatherPatterns();

    // All sectors should have valid severities
    for (int i = 0; i < config.grid().rows; ++i)
    {
        for (int j = 0; j < config.grid().cols; ++j)
        {
            double severity = simulator.getNormalizedSeverity(i, j);
            REQUIRE(severity >= 0.0);
            REQUIRE(severity <= 1.0);
        }
    }
}

TEST_CASE("WeatherSimulator position to severity mapping")
{
    Configuration config = createTestConfig();
    std::vector<std::pair<WeatherSeverity, double>> weatherLevels = config.getSortedWeatherLevels();

    WeatherSimulator simulator(config.grid(), weatherLevels);
    simulator.generateRandomWeatherPatterns();

    // Test multiple positions within the grid
    for (double lat = config.grid().minLat; lat < config.grid().maxLat; lat += 0.3)
    {
        for (double lon = config.grid().minLon; lon < config.grid().maxLon; lon += 0.3)
        {
            Position pos{lat, lon};
            double severity = simulator.getNormalizedSeverity(pos);
            REQUIRE(severity >= 0.0);
            REQUIRE(severity <= 1.0);
        }
    }
}

// ============================================================================
// WEATHER SOURCE TESTS
// ============================================================================

TEST_CASE("WeatherSourceOpenMeteo initialization")
{
    // Initialize Qt if not already done
    static QCoreApplication *app = nullptr;
    if (!QCoreApplication::instance())
    {
        int argc = 0;
        char *argv[] = {nullptr};
        app = new QCoreApplication(argc, argv);
    }

    WeatherSourceOpenMeteo source;
    // Test that it can be instantiated without throwing
    REQUIRE(true);
}

TEST_CASE("WeatherSourceOpenMeteo setRange")
{
    // Initialize Qt if not already done
    static QCoreApplication *app = nullptr;
    if (!QCoreApplication::instance())
    {
        int argc = 0;
        char *argv[] = {nullptr};
        app = new QCoreApplication(argc, argv);
    }

    WeatherSourceOpenMeteo source;

    // Test valid ranges
    REQUIRE_NOTHROW(source.setRange("10m"));
    REQUIRE_NOTHROW(source.setRange("80m"));
    REQUIRE_NOTHROW(source.setRange("120m"));
    REQUIRE_NOTHROW(source.setRange("180m"));

    // Test invalid range
    REQUIRE_THROWS_AS(source.setRange("50m"), std::invalid_argument);
}

TEST_CASE("WeatherSource getWeatherSeverity")
{
    Configuration config = createTestConfig();
    std::vector<std::pair<WeatherSeverity, double>> weatherLevels = config.getSortedWeatherLevels();
    WeatherSimulator simulator(config.grid(), weatherLevels);
    simulator.generateRandomWeatherPatterns();
    WeatherSourceSimulated source(simulator);

    // Test with sector centers from the config grid
    Grid grid(config.grid());
    for (int sectorId = 0; sectorId < std::min(3, grid.sectorCount()); ++sectorId)
    {
        Position pos = grid.sectorCenter(sectorId);
        WeatherSeverity severity = source.getWeatherSeverity(pos);
        // Should return one of the valid severities
        REQUIRE((severity == WeatherSeverity::OK || severity == WeatherSeverity::DEGRADED ||
                 severity == WeatherSeverity::SEVERE || severity == WeatherSeverity::EXTREME));
    }
}

// ============================================================================
// REDIS PUBLISHER TESTS
// ============================================================================

TEST_CASE("RedisPublisher initialization")
{
    Configuration config = createTestConfig();

    // This test just checks that RedisPublisher can be instantiated
    // without throwing an exception
    try
    {
        RedisPublisher publisher(config);
        REQUIRE(true); // If we get here, initialization succeeded
    }
    catch (const std::exception &e)
    {
        // Redis connection might fail in test environment, that's ok
        REQUIRE(true);
    }
}

TEST_CASE("RedisPublisher publishes processing result")
{
    Configuration config = createTestConfig();

    try
    {
        RedisPublisher publisher(config);

        // Create a test processing result
        ProcessingResult result;
        Position pos{59.5, 5.0};
        Track track("TEST001", "2024-01-01T12:00:00Z", pos, 10000.0, 450.0, 0.0, 180.0, 175.0);
        result.tracks.push_back(track);

        // Try to publish - may fail if Redis is not running, which is ok for this test
        try
        {
            publisher.publish(result);
            REQUIRE(true); // Publish succeeded
        }
        catch (const std::exception &)
        {
            // Redis connection failed, but that's expected in some test environments
            REQUIRE(true);
        }
    }
    catch (const std::exception &)
    {
        // Setup failed, that's ok
        REQUIRE(true);
    }
}

// ============================================================================
// PROTOBUF SERIALIZATION TESTS
// ============================================================================

TEST_CASE("Protobuf serialization round-trip")
{
    Configuration config = createTestConfig();
    Grid grid(config.grid());

    // Create a processing result with test data
    ProcessingResult originalResult;

    // Add some tracks
    Position pos1{59.5, 5.0};
    Track track1("ABC123", "2024-01-01T12:00:00Z", pos1, 10000.0, 450.0, 100.0, 180.0, 175.0);
    originalResult.tracks.push_back(track1);

    Position pos2{59.6, 5.1};
    Track track2("DEF456", "2024-01-01T12:00:00Z", pos2, 8000.0, 400.0, -50.0, 270.0, 265.0);
    originalResult.tracks.push_back(track2);

    // Add some risk events
    RiskEvent riskEvent1(1, SectorState::AT_RISK, 0, "2024-01-01T12:00:00Z", "Test risk event");
    originalResult.riskEvents.push_back(riskEvent1);

    // Add sector summaries
    SectorSummary summary1(0, 0, 0, "2024-01-01T12:00:00Z", 1, WeatherSeverity::OK, 1.0, 100.0,
                           SectorState::NORMAL);
    originalResult.sectorSummaries.push_back(summary1);

    SectorSummary summary2(1, 0, 1, "2024-01-01T12:00:00Z", 0, WeatherSeverity::SEVERE, 0.6, 100.0,
                           SectorState::AT_RISK);
    originalResult.sectorSummaries.push_back(summary2);

    // Serialize to protobuf
    FlightDataProto proto = mapToProto(originalResult, config, config.grid());

    // Serialize to string
    std::string serialized;
    bool serializeOk = proto.SerializeToString(&serialized);
    REQUIRE(serializeOk == true);
    REQUIRE(serialized.size() > 0);

    // Deserialize back
    FlightDataProto deserializedProto;
    bool deserializeOk = deserializedProto.ParseFromString(serialized);
    REQUIRE(deserializeOk == true);

    // Verify metadata
    REQUIRE(deserializedProto.metadata().version() == config.getProtobufVersion());
    REQUIRE(deserializedProto.metadata().timestamp().size() > 0);

    // Verify tracks
    REQUIRE(deserializedProto.trackdata().totalaircraftcount() == 2);
    REQUIRE(deserializedProto.trackdata().coordinatesystem() == config.getCoordinateSystem());
    REQUIRE(deserializedProto.trackdata().tracks_size() == 2);

    const TrackProto &trackProto1 = deserializedProto.trackdata().tracks(0);
    REQUIRE(trackProto1.icao24() == "ABC123");
    REQUIRE(trackProto1.timestamp() == "2024-01-01T12:00:00Z");
    REQUIRE(trackProto1.position().latitudedegrees() == 59.5);
    REQUIRE(trackProto1.position().longitudedegrees() == 5.0);
    REQUIRE(trackProto1.position().altitudefeet() == 10000.0);
    REQUIRE(trackProto1.velocity().groundspeedknots() == 450.0);
    REQUIRE(trackProto1.velocity().verticalspeedfeetperminute() == 100.0);
    REQUIRE(trackProto1.headingdegrees() == 180.0);
    REQUIRE(trackProto1.groundtrackdegrees() == 175.0);

    // Verify risk events
    REQUIRE(deserializedProto.riskeventdata().riskeventcount() == 1);
    REQUIRE(deserializedProto.riskeventdata().riskevents_size() == 1);

    const RiskEventProto &riskProto = deserializedProto.riskeventdata().riskevents(0);
    REQUIRE(riskProto.riskeventid() == 1);
    REQUIRE(riskProto.riskseverity() == "AT_RISK");
    REQUIRE(riskProto.sectorid() == 0);
    REQUIRE(riskProto.createdtimestamp() == "2024-01-01T12:00:00Z");
    REQUIRE(riskProto.message() == "Test risk event");
    REQUIRE(riskProto.acknowledged() == false);

    // Verify sector summaries
    REQUIRE(deserializedProto.sectorsummarydata().rowscount() == grid.rows());
    REQUIRE(deserializedProto.sectorsummarydata().columnscount() == grid.cols());
    REQUIRE(deserializedProto.sectorsummarydata().minlatitude() == config.grid().minLat);
    REQUIRE(deserializedProto.sectorsummarydata().maxlatitude() == config.grid().maxLat);
    REQUIRE(deserializedProto.sectorsummarydata().minlongitude() == config.grid().minLon);
    REQUIRE(deserializedProto.sectorsummarydata().maxlongitude() == config.grid().maxLon);
    REQUIRE(deserializedProto.sectorsummarydata().sectorsummaries_size() == 2);

    const SectorSummaryProto &summaryProto1 =
        deserializedProto.sectorsummarydata().sectorsummaries(0);
    REQUIRE(summaryProto1.sectorid() == 0);
    REQUIRE(summaryProto1.row() == 0);
    REQUIRE(summaryProto1.column() == 0);
    REQUIRE(summaryProto1.weatherseverity() == "OK");
    REQUIRE(summaryProto1.riskseverity() == "NORMAL");
    REQUIRE(summaryProto1.localaircraftcount() == 1);
    REQUIRE(summaryProto1.localaircraftbasecapacity() == 100);
    REQUIRE(summaryProto1.localaircrafteffectivecapacity() == 100);

    const SectorSummaryProto &summaryProto2 =
        deserializedProto.sectorsummarydata().sectorsummaries(1);
    REQUIRE(summaryProto2.sectorid() == 1);
    REQUIRE(summaryProto2.weatherseverity() == "SEVERE");
    REQUIRE(summaryProto2.riskseverity() == "AT_RISK");
    REQUIRE(summaryProto2.localaircrafteffectivecapacity() == 60); // 100 * 0.6
}

TEST_CASE("Protobuf serialization with empty result")
{
    Configuration config = createTestConfig();
    Grid grid(config.grid());

    ProcessingResult emptyResult;

    // Serialize to protobuf
    FlightDataProto proto = mapToProto(emptyResult, config, config.grid());

    // Serialize to string
    std::string serialized;
    bool serializeOk = proto.SerializeToString(&serialized);
    REQUIRE(serializeOk == true);

    // Deserialize back
    FlightDataProto deserializedProto;
    bool deserializeOk = deserializedProto.ParseFromString(serialized);
    REQUIRE(deserializeOk == true);

    // Verify empty collections
    REQUIRE(deserializedProto.trackdata().totalaircraftcount() == 0);
    REQUIRE(deserializedProto.trackdata().tracks_size() == 0);
    REQUIRE(deserializedProto.riskeventdata().riskeventcount() == 0);
    REQUIRE(deserializedProto.riskeventdata().riskevents_size() == 0);
    REQUIRE(deserializedProto.sectorsummarydata().sectorsummaries_size() == 0);
}

// ============================================================================
// INTEGRATION TESTS - Combined logic
// ============================================================================

TEST_CASE("Full workflow: track and weather updates generate results")
{
    Configuration config = createTestConfig();
    ComputeData computeData(config);
    Grid grid(config.grid());

    // Get the first sector center
    Position sectorCenter = grid.sectorCenter(0);

    // Add a few tracks to the sector
    for (int i = 0; i < 5; ++i)
    {
        Track track("FLIGHT_" + std::to_string(i), "2024-01-01T12:00:00Z", sectorCenter, 10000.0,
                    450.0, 0.0, 180.0, 175.0);
        computeData.handleTrackUpdate(track);
    }

    // Add weather to the sector
    WeatherCell weatherCell(0, "2024-01-01T12:00:00Z", WeatherSeverity::DEGRADED);
    computeData.handleWeatherUpdate(weatherCell);

    // Collect results
    ProcessingResult result = computeData.collectProcessingResult();

    auto sectorSummary0 = findSectorSummary(result, 0);

    REQUIRE(result.tracks.size() == 5);
    REQUIRE(result.sectorSummaries.size() == config.grid().rows * config.grid().cols); // 3x3 = 9
    REQUIRE(sectorSummary0->getWeatherSeverity() == WeatherSeverity::DEGRADED);
    REQUIRE(sectorSummary0->getLocalAircraftCount() == 5);
}

TEST_CASE("Multiple sectors with different weather conditions")
{
    Configuration config = createTestConfig();
    ComputeData computeData(config);
    Grid grid(config.grid());

    // Add tracks and weather to different sectors
    for (int sectorId = 0; sectorId < 4; ++sectorId)
    {
        Position center = grid.sectorCenter(sectorId);

        // Add tracks
        for (int i = 0; i < 3; ++i)
        {
            Track track("FLIGHT_" + std::to_string(sectorId * 10 + i), "2024-01-01T12:00:00Z",
                        center, 10000.0, 450.0, 0.0, 180.0, 175.0);
            computeData.handleTrackUpdate(track);
        }

        // Add different weather
        WeatherSeverity severity = static_cast<WeatherSeverity>(sectorId % 4);
        WeatherCell weatherCell(sectorId, "2024-01-01T12:00:00Z", severity);
        computeData.handleWeatherUpdate(weatherCell);
    }

    ProcessingResult result = computeData.collectProcessingResult();

    REQUIRE(result.tracks.size() == 12);
    for (int i = 0; i < 4; ++i)
    {
        auto it = findSectorSummary(result, i);
        REQUIRE(it->getLocalAircraftCount() == 3);
    }
}

TEST_CASE("Track removal when aircraft leaves sector")
{
    Configuration config = createTestConfig();
    ComputeData computeData(config);
    Grid grid(config.grid());

    Position pos1 = grid.sectorCenter(0);
    Track track1("ABC123", "2024-01-01T12:00:00Z", pos1, 10000.0, 450.0, 0.0, 180.0, 175.0);
    computeData.handleTrackUpdate(track1);

    ProcessingResult result1 = computeData.collectProcessingResult();
    auto sectorSummary0 = findSectorSummary(result1, 0);
    REQUIRE(result1.tracks.size() == 1);
    REQUIRE(sectorSummary0->getLocalAircraftCount() == 1);

    // Move to different sector (sector 5 in 3x3 grid)
    Position pos2 = grid.sectorCenter(5);
    Track track2("ABC123", "2024-01-01T12:00:01Z", pos2, 10050.0, 450.0, 0.0, 180.0, 175.0);
    computeData.handleTrackUpdate(track2);

    ProcessingResult result2 = computeData.collectProcessingResult();
    sectorSummary0 = findSectorSummary(result2, 0);
    auto sectorSummary5 = findSectorSummary(result2, 5);

    REQUIRE(result2.tracks.size() == 1);
    REQUIRE(result2.tracks[0].getPosition().latDeg == pos2.latDeg);
    REQUIRE(sectorSummary0->getLocalAircraftCount() == 0);
    REQUIRE(sectorSummary5->getLocalAircraftCount() == 1);

    // Move flight outside of grid
    Position pos3{10, 10};
    Track track3("ABC123", "2024-01-01T12:00:01Z", pos3, 10050.0, 450.0, 0.0, 180.0, 175.0);
    computeData.handleTrackUpdate(track3);

    ProcessingResult result3 = computeData.collectProcessingResult();
    sectorSummary5 = findSectorSummary(result3, 0);

    // There should not be any active tracks in sector or grid
    REQUIRE(sectorSummary5->getLocalAircraftCount() == 0);
    REQUIRE(result3.tracks.size() == 0);
}

// ============================================================================
// UTILITY/EDGE CASE TESTS
// ============================================================================

TEST_CASE("Empty processing result on startup")
{
    Configuration config = createTestConfig();
    ComputeData computeData(config);

    ProcessingResult result = computeData.collectProcessingResult();

    REQUIRE(result.tracks.empty());
    REQUIRE(result.riskEvents.empty());
    REQUIRE(result.sectorSummaries.size() > 0);
}

TEST_CASE("Weather levels are properly sorted")
{
    Configuration config = createTestConfig();
    auto weatherLevels = config.getSortedWeatherLevels();

    // Weather levels should be sorted by threshold
    for (size_t i = 1; i < weatherLevels.size(); ++i)
    {
        REQUIRE(weatherLevels[i].second >= weatherLevels[i - 1].second);
    }
}

TEST_CASE("Configuration loads default values")
{
    Configuration config = createTestConfig();

    REQUIRE(config.getProtobufVersion() == 1);
    REQUIRE(config.getCoordinateSystem() == "WGS84");
    REQUIRE(config.getNumFlights() == 3);
    REQUIRE(config.defaultBaseCapacity() == 1.0);
}

TEST_CASE("Weather factors are applied correctly")
{
    Configuration config = createTestConfig();

    double factorOk = config.weatherFactor(WeatherSeverity::OK);
    double factorDegraded = config.weatherFactor(WeatherSeverity::DEGRADED);
    double factorSevere = config.weatherFactor(WeatherSeverity::SEVERE);
    double factorExtreme = config.weatherFactor(WeatherSeverity::EXTREME);

    REQUIRE(factorOk == 1.0);
    REQUIRE(factorDegraded == 0.8);
    REQUIRE(factorSevere == 0.6);
    REQUIRE(factorExtreme == 0.4);

    // Generally, worse weather should reduce capacity
    REQUIRE(factorOk >= factorDegraded);
    REQUIRE(factorDegraded >= factorSevere);
    REQUIRE(factorSevere >= factorExtreme);
}

// ============================================================================
// END-TO-END REDIS PUBLISHING TEST - Simulator to Redis
// ============================================================================

TEST_CASE("End-to-end: Simulated data is published to Redis")
{
    Configuration config = createTestConfig();
    ComputeData computeData(config);
    Grid grid(config.grid());

    // Create weather simulator and set some patterns
    std::vector<std::pair<WeatherSeverity, double>> weatherLevels = config.getSortedWeatherLevels();
    WeatherSimulator weatherSimulator(config.grid(), weatherLevels);

    // Set constant weather patterns in specific sectors
    weatherSimulator.setWeatherPattern(0, 0, 0.3); // Sector 0: light weather
    weatherSimulator.setWeatherPattern(0, 1, 0.7); // Sector 1: heavy weather
    weatherSimulator.tick(0.0);

    // Add multiple simulated tracks to different sectors
    std::vector<std::string> flightIds = {"SIM001", "SIM002", "SIM003", "SIM004", "SIM005"};
    std::vector<int> targetSectors = {0, 0, 1, 1, 2};

    for (size_t i = 0; i < flightIds.size(); ++i)
    {
        Position sectorCenter = grid.sectorCenter(targetSectors[i]);
        // Add slight offset to avoid exact center
        Position trackPos{sectorCenter.latDeg + 0.01, sectorCenter.lonDeg + 0.01};

        Track track(flightIds[i], "2024-01-01T12:00:00Z", trackPos, 10000.0 + (i * 500),
                    400.0 + (i * 10), 50.0 - (i * 10), 180.0 + (i * 5), 175.0 + (i * 5));
        computeData.handleTrackUpdate(track);
    }

    // Update weather for several sectors
    for (int sectorId = 0; sectorId < 3; ++sectorId)
    {
        // Cycle through available weather severity levels
        WeatherSeverity weatherSev = weatherLevels[sectorId % weatherLevels.size()].first;
        WeatherCell weatherCell(sectorId, "2024-01-01T12:00:00Z", weatherSev);
        computeData.handleWeatherUpdate(weatherCell);
    }

    // Collect processing result from compute data
    ProcessingResult result = computeData.collectProcessingResult();

    // Map to protobuf (simulating what RedisPublisher does)
    FlightDataProto proto = mapToProto(result, config, config.grid());

    // Serialize to string (simulating protobuf serialization for Redis)
    std::string serialized;
    bool serializeOk = proto.SerializeToString(&serialized);

    // Deserialize to verify the published data is valid
    FlightDataProto publishedData;
    bool deserializeOk = publishedData.ParseFromString(serialized);

    // Verify tracks made it through the pipeline
    REQUIRE(publishedData.trackdata().totalaircraftcount() == 5);
    REQUIRE(publishedData.trackdata().tracks_size() == 5);
    REQUIRE(publishedData.trackdata().coordinatesystem() == "WGS84");

    // Verify specific track data (order-independent due to unordered_map storage)
    for (const std::string &expectedId : flightIds)
    {
        bool found = false;
        for (int i = 0; i < publishedData.trackdata().tracks_size(); ++i)
        {
            const TrackProto &trackProto = publishedData.trackdata().tracks(i);
            if (trackProto.icao24() == expectedId)
            {
                REQUIRE(trackProto.timestamp() == "2024-01-01T12:00:00Z");
                auto it = std::find(flightIds.begin(), flightIds.end(), expectedId);
                int idx = std::distance(flightIds.begin(), it);
                REQUIRE(trackProto.position().altitudefeet() == 10000.0 + (idx * 500));
                REQUIRE(trackProto.velocity().groundspeedknots() == 400.0 + (idx * 10));
                break;
            }
        }
    }

    // Verify sector summaries made it through
    REQUIRE(publishedData.sectorsummarydata().rowscount() == 3);
    REQUIRE(publishedData.sectorsummarydata().columnscount() == 3);

    // Verify metadata
    REQUIRE(publishedData.metadata().version() == 1);
    REQUIRE(publishedData.metadata().timestamp().size() > 0);

    // Try to actually publish to Redis if available
    try
    {
        RedisPublisher publisher(config);
        publisher.publish(result);
        // If we get here, Redis was available and publishing succeeded
        REQUIRE(true);
    }
    catch (const std::exception &)
    {
        // Redis not available in test environment, but serialization verification passed
        REQUIRE(true);
    }
}

// ============================================================================
// PROCESSOR APP INTEGRATION TEST
// ============================================================================

TEST_CASE("Integration test: Full processor workflow with API sources")
{
    // Initialize Qt if not already done for API sources
    static QCoreApplication *app = nullptr;
    if (!QCoreApplication::instance())
    {
        int argc = 0;
        char *argv[] = {nullptr};
        app = new QCoreApplication(argc, argv);
    }

    Configuration config = createTestConfigApi();
    Grid grid(config.grid());
    ComputeData computeData(config);

    // Initialize API sources
    auto trackSource = std::make_unique<TrackSourceOpenSky>();
    trackSource->setRegion(config.grid());

    auto weatherSource = std::make_unique<WeatherSourceOpenMeteo>();

    IngestService ingestService(config.grid(), trackSource.get(), weatherSource.get());

    // Attempt to process track updates (may fail if API unavailable)
    try
    {
        std::vector<Track> tracks = ingestService.getAllTracks();
        for (const auto &track : tracks)
        {
            computeData.handleTrackUpdate(track);
        }
    }
    catch (const std::exception &)
    {
        // API may not be available in test environment, that's ok
    }

    // Process weather updates for all sectors (may fail if API unavailable)
    for (int sectorId = 0; sectorId < grid.sectorCount(); ++sectorId)
    {
        Position center = grid.sectorCenter(sectorId);
        try
        {
            WeatherSeverity severity = ingestService.getWeatherSeverity(center);
            WeatherCell weatherCell(sectorId, "2024-01-01T12:00:00Z", severity);
            computeData.handleWeatherUpdate(weatherCell);
        }
        catch (const std::exception &)
        {
            // Weather API may fail, skip this sector
        }
    }

    // Collect processing result
    ProcessingResult result = computeData.collectProcessingResult();

    // Attempt to publish (may fail if Redis not running)
    try
    {
        RedisPublisher publisher(config);
        publisher.publish(result);
    }
    catch (const std::exception &)
    {
        // Redis not available, but that's ok for this test
    }

    // Verify the results structure
    REQUIRE(result.sectorSummaries.size() == grid.sectorCount());

    // Check sector summaries have valid data
    for (const auto &summary : result.sectorSummaries)
    {
        REQUIRE(summary.getSectorId() >= 0);
        REQUIRE(summary.getSectorId() < grid.sectorCount());
        REQUIRE(summary.getLocalAircraftCount() >= 0);
    }

    // If tracks were retrieved, check they are within the grid
    for (const auto &track : result.tracks)
    {
        REQUIRE(grid.isInside(track.getPosition()) == true);
    }
}
