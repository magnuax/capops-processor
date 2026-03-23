#include "config/Config.hpp"
#include "compute/Grid.hpp"
#include "domain/types/Position.hpp"
#include "domain/types/WeatherSeverity.hpp"
#include "sources/WeatherSourceOpenMeteo.hpp"
#include "sources/WeatherSourceSimulated.hpp"
#include "sources/simulations/WeatherSimulator.hpp"
#include "../test_helpers.hpp"
#include <QCoreApplication>
#include <catch2/catch_test_macros.hpp>

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
