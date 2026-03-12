#pragma once
#include "domain/Track.hpp"
#include <optional>
#include <vector>

class IDataSource
{
  public:
    virtual ~IDataSource() = default;
  
    virtual std::optional<Track> getTrack(const std::string &icao) const = 0;
  
    virtual std::vector<Track> getAllTracks() const = 0;
};