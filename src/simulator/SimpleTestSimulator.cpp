#include "simulator/SimpleTestSimulator.hpp"

#include "domain/types/Position.hpp"
#include "domain/types/WeatherSeverity.hpp"

#include <stdexcept>

SimpleTestSimulator::SimpleTestSimulator()
{
    events_.push_back(Track("ABC123", 1000, Position{60.10, 5.10}, 10000.0, 230.0, 90.0));
    events_.push_back(Track("DEF456", 1001, Position{60.12, 5.12}, 11000.0, 240.0, 95.0));
    events_.push_back(WeatherCell("S1", 1002, WeatherSeverity::OK));

    events_.push_back(Track("ABC123", 2000, Position{60.11, 5.11}, 10000.0, 230.0, 90.0));
    events_.push_back(Track("DEF456", 2001, Position{60.13, 5.13}, 11000.0, 240.0, 95.0));
    events_.push_back(WeatherCell("S1", 2002, WeatherSeverity::SEVERE));

    events_.push_back(Track("GHI789", 3000, Position{60.14, 5.14}, 9500.0, 220.0, 100.0));
    events_.push_back(Track("JKL321", 3001, Position{60.15, 5.15}, 9800.0, 225.0, 105.0));
    events_.push_back(WeatherCell("S1", 3002, WeatherSeverity::SEVERE));
}

bool SimpleTestSimulator::hasNextEvent() const
{
    return currentEvent_ < events_.size();
}

SimulationEvent SimpleTestSimulator::nextEvent()
{
    if (!hasNextEvent())
    {
        throw std::out_of_range("No more simulation events available");
    }

    return events_[currentEvent_++];
}

void SimpleTestSimulator::reset()
{
    currentEvent_ = 0;
}