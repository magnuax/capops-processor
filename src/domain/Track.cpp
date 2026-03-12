#include "domain/Track.hpp"

Track::Track(std::string icao, std::string timestamp, Position position, double altitude,
             double velocity, double heading)
    : icao_(std::move(icao)), timestamp_(timestamp), position_(std::move(position)),
      altitude_(altitude), velocity_(velocity), heading_(heading)
{
}

std::string Track::getIcao() const
{
    return icao_;
}

std::string Track::getTimestamp() const
{
    return timestamp_;
}

Position Track::getPosition() const
{
    return position_;
}