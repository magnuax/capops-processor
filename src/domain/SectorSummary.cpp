#include "domain/SectorSummary.hpp"

SectorSummary::SectorSummary(std::string sectorId, std::int64_t timestamp, int trackCount,
                             WeatherSeverity weatherSeverity, double weatherFactor,
                             double baseCapacity, double effectiveCapacity, SectorState state)
    : sectorId_(std::move(sectorId)), timestamp_(timestamp), trackCount_(trackCount),
      weatherSeverity_(weatherSeverity), weatherFactor_(weatherFactor), baseCapacity_(baseCapacity),
      effectiveCapacity_(effectiveCapacity), state_(state)
{
}