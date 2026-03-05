#include "domain/Track.hpp"

Track::Track(std::string icao, std::int64_t timestamp, Position position, double altitude,
             double velocity, double heading)
    : icao_(std::move(icao)), timestamp_(timestamp), position_(std::move(position)),
      altitude_(altitude), velocity_(velocity), heading_(heading)
{
}
