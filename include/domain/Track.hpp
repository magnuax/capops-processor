#pragma once
#include "domain/types/Position.hpp"
#include <cstdint>
#include <string>

class Track
{

  public:
    Track(std::string icao, std::string timestamp, Position position, double altitudeMeters,
          double groundSpeedKnots, double verticalSpeedFeetPerMinute, double headingDegrees,
          double groundTrackDegrees);
    std::string getIcao() const;
    std::string getTimestamp() const;
    Position getPosition() const;
    double getHeadingDegrees() const;
    double getGroundTrackDegrees() const;
    double getAltitudeMeters() const;
    double getGroundSpeedKnots() const;
    double getVerticalSpeedFeetPerMinute() const;


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