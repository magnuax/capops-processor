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

TrackPayload Track::toPayload() const
{
    return TrackPayload{icao_,
                        timestamp_,
                        PositionPayload{position_.latDeg, position_.lonDeg, altitudeMeters_},
                        VelocityPayload{groundSpeedKnots_, verticalSpeedFeetPerMinute_},
                        headingDegrees_,
                        groundTrackDegrees_};
}