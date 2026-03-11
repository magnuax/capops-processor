#include "app/ProcessorApp.hpp"
#include "compute/ComputeData.hpp"
#include "config/Config.hpp"
#include "simulator/SimpleTestSimulator.hpp"

#include <variant>

void ProcessorApp::run()
{
    SimpleTestSimulator simulator;
    Configuration config("configuration.cfg");
    ComputeData computeData(config);

    // just for testing now, will remove later:
    while (simulator.hasNextEvent())
    {
        SimulationEvent event = simulator.nextEvent();

        std::visit(
            [&](const auto &e)
            {
                using T = std::decay_t<decltype(e)>;

                if constexpr (std::is_same_v<T, Track>)
                {
                    computeData.handleTrackUpdate(e);
                }
                else if constexpr (std::is_same_v<T, WeatherCell>)
                {
                    computeData.handleWeatherUpdate(e);
                }
            },
            event);
    }
}