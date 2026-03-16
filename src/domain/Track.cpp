#include "domain/Track.hpp"

Track::Track(std::string icao, std::string timestamp, Position position, double altitudeMeters,
             double groundSpeedKnots, double verticalSpeedFeetPerMinute, double headingDegrees,
             double groundTrackDegrees)
    : icao_(std::move(icao)), timestamp_(std::move(timestamp)), position_(position),
      altitudeMeters_(altitudeMeters), groundSpeedKnots_(groundSpeedKnots),
      verticalSpeedFeetPerMinute_(verticalSpeedFeetPerMinute), headingDegrees_(headingDegrees),
      groundTrackDegrees_(groundTrackDegrees)
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

double Track::getHeadingDegrees() const
{
    return headingDegrees_;
}

double Track::getGroundTrackDegrees() const
{
    return groundTrackDegrees_;
}

double Track::getAltitudeMeters() const
{
    return altitudeMeters_;
}

double Track::getGroundSpeedKnots() const
{
    return groundSpeedKnots_;
}

double Track::getVerticalSpeedFeetPerMinute() const
{
    return verticalSpeedFeetPerMinute_;
}
