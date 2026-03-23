#include "compute/ComputeData.hpp"
#include "compute/Grid.hpp"
#include "config/Config.hpp"
#include "domain/Track.hpp"
#include "domain/WeatherCell.hpp"
#include "domain/types/Position.hpp"
#include "domain/types/ProcessingResult.hpp"
#include "domain/types/SectorState.hpp"
#include "domain/types/WeatherSeverity.hpp"
#include "../test_helpers.hpp"
#include <catch2/catch_test_macros.hpp>

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

TEST_CASE("Empty processing result on startup")
{
    Configuration config = createTestConfig();
    ComputeData computeData(config);

    ProcessingResult result = computeData.collectProcessingResult();

    REQUIRE(result.tracks.empty());
    REQUIRE(result.riskEvents.empty());
    REQUIRE(result.sectorSummaries.size() > 0);
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
