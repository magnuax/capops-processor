#include "config/Config.hpp"
#include "domain/types/WeatherSeverity.hpp"
#include "test_helpers.hpp"
#include <catch2/catch_test_macros.hpp>

// ============================================================================
// UTILITY/EDGE CASE TESTS
// ============================================================================

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
