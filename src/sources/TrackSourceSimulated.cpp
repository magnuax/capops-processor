
#include "sources/TrackSourceSimulated.hpp"
#include "domain/Track.hpp"
#include "utils/time/IsoTimestamp.hpp"
#include <cmath>
#include <functional>
#include <utility>

TrackSourceSimulated::TrackSourceSimulated(RadarSimulator &simulator) : simulator_(simulator)
{
}

std::optional<Track> TrackSourceSimulated::getTrack(const std::string &icao) const
{

    if (!simulator_.containsFlight(icao))
    {
        return std::nullopt;
    }

    auto velSim = simulator_.getVelocity(icao);
    auto posSim = simulator_.getPosition(icao);

    std::string timestamp = createIsoTimestamp();
    Position position{posSim.first, posSim.second};
    // Derive a deterministic cruise altitude per flight from its ICAO id
    std::hash<std::string> hasher;
    double altitudeFeet = 25000.0 + (hasher(icao) % 150) * 100.0;

    // Convert simulator speed (degrees/s) to knots (1 degree ≈ 60 nautical miles)
    double simSpeed = simulator_.getSpeed(icao);
    double groundSpeedKnots = simSpeed * 60.0 * 3600.0;

    double verticalSpeed = 0.0;
    double headingDegrees = std::atan2(velSim.second, velSim.first) * 180 / M_PI;
    double groundTrackDegrees = headingDegrees;

    return Track(icao, timestamp, position, altitudeFeet, groundSpeedKnots, verticalSpeed,
                 headingDegrees, groundTrackDegrees);
}

std::vector<Track> TrackSourceSimulated::getAllTracks() const
{
    std::vector<Track> tracks;

    for (const auto &icao : simulator_.getFlights())
    {
        auto trackOpt = getTrack(icao);

        if (trackOpt)
        {
            tracks.push_back(*trackOpt);
        }
    }
    return tracks;
}