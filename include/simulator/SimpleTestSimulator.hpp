#pragma once

#include "domain/Track.hpp"
#include "domain/WeatherCell.hpp"

#include <cstddef>
#include <variant>
#include <vector>

using SimulationEvent = std::variant<Track, WeatherCell>;

class SimpleTestSimulator
{
  public:
    SimpleTestSimulator();

    bool hasNextEvent() const;
    SimulationEvent nextEvent();
    void reset();

  private:
    std::vector<SimulationEvent> events_;
    std::size_t currentEvent_{0};
};