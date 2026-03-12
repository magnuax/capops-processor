
#include "ingestion/sources/SimSource.hpp"
#include <cmath>
#include <utility>

SimSource::SimSource(RadarSimulator &simulator) : simulator_(simulator)
{
}

std::optional<Track> SimSource::getTrack(const std::string &icao) const
{

    if (!simulator_.containsFlight(icao))
    {
        return std::nullopt;
    }

    auto velSim = simulator_.getVelocity(icao);
    auto posSim = simulator_.getPosition(icao);

    std::string timestamp = "2024-01-01T00:00:00Z";      // Placeholder timestamp
    Position position{posSim.first, posSim.second};
    double altitude = 30000;                             // Placeholder altitude

    double speed = simulator_.getSpeed(icao);
    double heading = std::atan2(velSim.second, velSim.first) * 180 / M_PI;

    return Track(icao, timestamp, position, altitude, speed, heading);
}

std::vector<Track> SimSource::getAllTracks() const
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