#pragma once
#include "domain/types/Position.hpp"
#include <cstdint>
#include <string>

struct PositionPayload
{
    double latitudeDegrees;
    double longitudeDegrees;
    double altitudeMeters;
};

struct VelocityPayload
{
    double groundSpeedKnots;
    double verticalSpeedFeetPerMinute;
};

struct TrackPayload
{
    std::string icao24;
    std::string timestamp;
    PositionPayload position;
    VelocityPayload velocity;
    double headingDegrees;
    double groundTrackDegrees;
};

class Track
{

  public:
    Track(std::string icao, std::string timestamp, Position position, double altitudeMeters,
          double groundSpeedKnots, double verticalSpeedFeetPerMinute, double headingDegrees,
          double groundTrackDegrees);
    std::string getIcao() const;
    std::string getTimestamp() const;
    Position getPosition() const;

    TrackPayload toPayload() const;

  private:
    std::string icao_;
    std::string timestamp_;
    Position position_;
    double altitudeMeters_;
    double groundSpeedKnots_;
    double verticalSpeedFeetPerMinute_;
    double headingDegrees_;
    double groundTrackDegrees_;
};