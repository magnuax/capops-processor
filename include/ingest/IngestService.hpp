#pragma once

#include <vector>

#include "config/Config.hpp"

#include "domain/Track.hpp"
#include "domain/types/Position.hpp"
#include "domain/types/WeatherSeverity.hpp"

#include "sources/interfaces/ITrackSource.hpp"
#include "sources/interfaces/IWeatherSource.hpp"

class IngestService
{
  public:
    IngestService(GridConfig config, ITrackSource *trackSource, IWeatherSource *weatherSource);

    Track getTrack(const std::string &icao24) const;

    std::vector<Track> getAllTracks() const;

    WeatherSeverity getWeatherSeverity(Position position) const;

  private:
    GridConfig config_;
    ITrackSource *trackSource_;
    IWeatherSource *weatherSource_;
};