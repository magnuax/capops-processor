#pragma once

#include "sources/interfaces/ITrackSource.hpp"
#include "sources/simulations/RadarSimulator.hpp"

class TrackSourceSimulated : public ITrackSource
{
  public:
    TrackSourceSimulated(RadarSimulator &simulator);

    std::optional<Track> getTrack(const std::string &icao) const override;

    std::vector<Track> getAllTracks() const override;

  private:
    RadarSimulator &simulator_;
};