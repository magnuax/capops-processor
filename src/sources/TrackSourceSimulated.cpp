
#include "sources/TrackSourceSimulated.hpp"
#include "domain/Track.hpp"
#include <cmath>
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

    std::string timestamp = "1970-01-01T00:00:00Z";
    Position position{posSim.first, posSim.second};
    double altitudeMeters = -1;
    double groundSpeedKnots = simulator_.getSpeed(icao);
    double verticalSpeed = -1;
    double headingDegrees = std::atan2(velSim.second, velSim.first) * 180 / M_PI;
    double groundTrackDegrees = headingDegrees;

    return Track(icao, timestamp, position, altitudeMeters, groundSpeedKnots, verticalSpeed,
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