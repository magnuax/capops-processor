#include "utils/WeatherPatternUtils.hpp"

#include <cmath>

namespace WeatherPatternUtils
{
WeatherPattern horizontalWave(double frequency, double speed, double phase)
{
    return [frequency, speed, phase](double time)
    { return 0.5 + 0.5 * std::sin(frequency * time - speed * phase); };
}

} // namespace WeatherPatternUtils