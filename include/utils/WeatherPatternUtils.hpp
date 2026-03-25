#pragma once
#include <functional>

namespace WeatherPatternUtils
{

using WeatherPattern = std::function<double(double)>;

WeatherPattern horizontalWave(double frequency, double speed, double phase);

}; // namespace WeatherPatternUtils
