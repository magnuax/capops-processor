#pragma once

#include "sources/interfaces/IWeatherSource.hpp"
#include "sources/simulations/WeatherSimulator.hpp"

class WeatherSourceSimulated : public IWeatherSource
{
  public:
    WeatherSourceSimulated(WeatherSimulator &simulator);

    WeatherSeverity getWeatherSeverity(Position coordinates) override;

  private:
    WeatherSimulator &simulator_;
};