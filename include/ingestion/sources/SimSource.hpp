#pragma once

#include "ingestion/IDataSource.hpp"
#include "simulator/RadarSimulator.hpp"

class SimSource : public IDataSource
{
  public:
    SimSource(RadarSimulator &simulator);

    std::optional<Track> getTrack(const std::string &icao) const override;

    std::vector<Track> getAllTracks() const override;

  private:
    RadarSimulator &simulator_;
};